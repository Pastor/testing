#!/usr/bin/env python
# -*- coding: utf-8 -*-

import random
import re
import string

TEMPLATE_REG_ID = 'sms.accept_code'
TEMPLATE_CONFIRM_ID = 'sms.registration'
TEMPLATE_RECOVERY_ACCESS = 'sms.recovery'


def get(arg, key, default):
    return arg[key] if key in arg else default


def send_sms(arg, con):
    params = {
        'numbers': [arg['phone']],
        'template_id': arg['template_id'],
        'params': arg['params'],
        'sync': True
    }

    plugin = get(arg, 'sms_plugin', None)
    if plugin:
        params['plugin'] = plugin

    con.call('sendSms', params)


def pre_registration_captive_portal_user(arg, con):
    """ preRegistrationCaptivePortalUser functor.
    Saves the user's personal data and send SMS with confirmation code.
    Returns pre-registration id.

    args: (user_phone, user_name, user_surname, user_patronymic, agrm_num, pay_code, sms_plugin, set_id, tariff_id)

    """

    # personal data
    user_phone = get(arg, 'user_phone', '')
    user_name = get(arg, 'user_name', '')
    user_surname = get(arg, 'user_surname', '')
    user_patronymic = get(arg, 'user_patronymic', '')
    user_email = get(arg, 'user_email', '')
    # data for an existing user
    agrm_num = get(arg, 'agrm_num', '')  # agreement number
    pay_code = get(arg, 'pay_code', '')  # payment code
    # SMS plugin (optional)
    sms_plugin = get(arg, 'sms_plugin', None)

    # id set cards
    set_id = get(arg, 'tariff_id', 0)
    if not set_id:
        set_id = get(arg, 'set_id', 0)

    agrm_id = None

    param_get_agrm_id = {}
    if agrm_num:
        param_get_agrm_id['agrm_num'] = agrm_num + '#'
    if pay_code:
        param_get_agrm_id['pay_code'] = pay_code + '#'

    if param_get_agrm_id:
        agrms = con.call('getAgreements', param_get_agrm_id)
        if not agrms:
            raise lbcore.Error('Entered invalid data: agrm_num $1, pay_code $2', [agrm_num, pay_code])
        agrm_id = agrms[0]['agrm_id']
        uid = agrms[0]['uid']

        account = con.call('getAccount', {'uid': uid})
        if not account:
            raise lbcore.Error('The account is not found for: uid $1', [uid])
        if account.get('set_id'):
            raise lbcore.Error('This user is already registered')
        user_phone = account['mobile']
    elif user_phone:
        accounts = con.call('getAccounts', {'phone': user_phone, 'is_archive': False})
        if len(accounts):
            for acc in accounts:
                account = con.call('getAccount', {'uid': acc.get('uid')})
                if account.get('set_id'):
                    raise lbcore.Error('This user is already registered')

    if not user_phone:
        raise lbcore.Error('The phone number is not found')

    # len_confirm_code - длина кода подтверждения
    len_confirm_code = 6
    get_option = con.call('getOption', {'name': 'len_confirm_code'})
    if get_option['value']:
        len_confirm_code = int(get_option['value'])
    code = [random.choice(string.digits) for x in xrange(len_confirm_code)]
    accept_code = "".join(code)

    format_phone = con.call('getOption', {'name': 'user_mobile_format'})['value']
    if format_phone and not re.match(format_phone, user_phone):
        raise lbcore.Error('Wrong format of the user mobile: required format $1, mobile $2', [format_phone, user_phone])

    params = {'accept_code': accept_code}
    if not agrm_id and not pay_code:
        params['user_name'] = user_name
        params['user_surname'] = user_surname
        params['user_patronymic'] = user_patronymic
        params['user_phone'] = user_phone
        params['user_email'] = user_email
        params['set_id'] = set_id
    else:
        params['agrm_id'] = agrm_id

    user_id = int(con.call('setCaptivePortalUser', params))

    sms_params = {
        'phone': user_phone,
        'accept_code': accept_code,
        'template_id': TEMPLATE_REG_ID,
        'params': {
            'accept_code': accept_code,
            'id': user_id,
        },
    }
    if sms_plugin:
        sms_params['sms_plugin'] = sms_plugin

    send_sms(sms_params, con)

    return user_id


def confirm_registration_captive_portal_user(arg, con):
    """ confirmRegistrationCaptivePortalUser functor.
    Checks confirmation code and activate first payment card in cards set.
    Send SMS messages for each created service.

    args: (id, accept_code, sms_plugin)

    """

    cp_user_id = arg['id']
    accept_code = arg['accept_code']

    # SMS plugin (optional)
    sms_plugin = get(arg, 'sms_plugin', None)

    pre_users = con.call('getCaptivePortalUsers', {'id': cp_user_id, 'registered': False})

    if not pre_users:
        raise lbcore.Error('Could not find unregistered user with id $1', [cp_user_id])

    pre_user = pre_users[0]

    if accept_code != pre_user['accept_code']:
        raise lbcore.Error('Invalid accept code')

    card_set = con.call('getCardSets', {'set_id': pre_user['set_id']})
    if not len(card_set):
        raise lbcore.Error('Card set is not found')
    if not card_set[0].get('portal_id'):
        raise lbcore.Error('Card set is not used in the captive portal')
    # use first card in cardset set_id
    pc_args = {
        'set_id': pre_user['set_id'],
        'is_activated': False,
        'is_actually': True,
        'pg_num': 1,
        'pg_size': 1,
    }
    paycards = con.call('getPayCards', pc_args)

    if not paycards:
        raise lbcore.Error('Could not find paycard in cardset $1', [pre_user['set_id']])

    ser_no = paycards[0]['ser_no']

    agrm_id = get(pre_user, 'agrm_id', None)

    params = {
        'ser_no': ser_no
    }

    if agrm_id:
        params['agrm_id'] = agrm_id
    else:
        params['name'] = pre_user['user_name']
        params['surname'] = pre_user['user_surname']
        params['patronymic'] = pre_user['user_patronymic']
        params['phone'] = pre_user['user_phone']
        params['email'] = pre_user['user_email']

    # call ActivateCard
    services = []
    try:
        act = con.call('ActivateCard', params)
        agrm_id = act['agrm_id']

        accounts = con.call('getAccounts', {'agrm_id': agrm_id, 'get_full': True})
        acc = accounts[0]

        agrm_num = None
        for ag in acc['agreements']:
            if ag['agrm_id'] == agrm_id:
                agrm_num = ag['agrm_num']

        if agrm_num is None:
            raise lbcore.Error('Could find agreement $1 in account $2', [acc['uid'], agrm_id])

        user_name = acc['name']
        user_phone = acc['mobile']

        for act_vg in act['vgroups']:
            vg = con.call('getVgroupSimple', {'vg_id': act_vg['vg_id']})
            services.append({
                'vglogin': vg['login'],
                'vgpass': vg['pass'],
                'service_name': vg['agent_name'],
            })

        if not user_phone:
            raise lbcore.Error('The phone number is not found')

    except Exception as e:
        raise lbcore.Error('ActivateCard failed: $1' % [str(e)])

    pre_user['ser_no'] = ser_no
    pre_user['agrm_id'] = agrm_id
    con.call('setCaptivePortalUser', pre_user)

    # send sms for each vgroup returned from ActivateCard
    sms_count = 0
    for s in services:
        params = s
        params['username'] = user_name
        params['agrm_number'] = agrm_num

        arg = {
            'phone': user_phone,
            'template_id': TEMPLATE_CONFIRM_ID,
            'params': params,
        }
        if sms_plugin:
            arg['sms_plugin'] = sms_plugin
        try:
            send_sms(arg, con)
            sms_count += 1
        except Exception as e:
            raise lbcore.Error('Send SMS failed: $1', [str(e)])

    return agrm_id


def get_captive_portal_tariffs(arg, con):
    portal_id = arg['portal_id']
    card_set = con.call('getCardSets', {'portal_id': portal_id})
    portals = []
    for cs in card_set:
        portals.append({
            'tariff_id': cs['set_id'],
            'name': cs['set_descr']
        })
    return portals


def recover_captive_portal_user(arg, con):
    """ recoverCaptivePortalUser functor.
    Send SMS to a specified number.

    args: (phone, set_id, sms_plugin, portal_id)

    """

    phone = get(arg, 'phone', '')
    portal_id = get(arg, 'portal_id', 0)
    set_id = None
    if not portal_id:
        set_id = get(arg, 'set_id', 0)
    # SMS plugin (optional)
    sms_plugin = get(arg, 'sms_plugin', None)

    if not phone:
        raise lbcore.Error('The phone number is not found')

    get_account_params = {
        'phone': phone,
        'is_archive': False
    }
    if not portal_id:
        get_account_params['set_id'] = set_id
    accounts = con.call('getAccounts', get_account_params)

    if not len(accounts):
        raise lbcore.Error('This user is not registered')

    if portal_id:
        card_sets = con.call('getCardSets', {'portal_id': portal_id})
        account = con.call('getAccount', {'uid': accounts[0].get('uid')})
        card_sets_for_account = filter(lambda x: x.get('set_id') == account['set_id'], card_sets)

        if not len(card_sets_for_account):
            raise lbcore.Error('This user is not registered')

        set_id = account['set_id']

    if not set_id:
        raise lbcore.Error('Empty id set cards')

    vgroups = con.call('getVgroups', {'uid': accounts[0]['uid'], 'set_id': set_id, 'archive': 0})
    if not len(vgroups):
        raise lbcore.Error('User not has connected services')

    for vgroup in vgroups:
        password = con.call('getVgroupSimple', {'vg_id': vgroup['vg_id']})['pass']
        sms_params = {
            'phone': phone,
            'template_id': TEMPLATE_RECOVERY_ACCESS,
            'params': {
                'username': accounts[0]['name'],
                'agrm_number': vgroup['agrm_num'],
                'vglogin': vgroup['login'],
                'vgpass': password,
                'service_name': vgroup['agent_name'],
            },
        }
        if sms_plugin:
            sms_params['sms_plugin'] = sms_plugin

        send_sms(sms_params, con)

import lbcore  # NOQA
f1 = lbcore.Functor(pre_registration_captive_portal_user, "preRegistrationCaptivePortalUser")
f2 = lbcore.Functor(confirm_registration_captive_portal_user, "confirmRegistrationCaptivePortalUser")
f3 = lbcore.Functor(recover_captive_portal_user, "recoverCaptivePortalUser")
f4 = lbcore.Functor(get_captive_portal_tariffs, "getCaptivePortalTariffs")
