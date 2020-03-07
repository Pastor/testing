#!/usr/bin/python
# -*- coding: utf-8 -*-

import datetime

EMAIL_TEMPLATE_ID = 'email.balance_down'
SMS_TEMPLATE_ID = 'sms.balance_down'
BALANCE_NOTICE_TYPE = 1

TRANSPORT_EMAIL = 0
TRANSPORT_SMS = 1


def send_mail(con, email_to, template_id, params):
    msg = {'template_id': template_id, 'params': params, 'email_to': email_to}
    con.call('sendMail', msg)


def notify_balance(msg, arg, con):

    class SkipNotice(Exception):
        pass

    try:
        con.log_debug('Balance changed. Check notify balance %s' % arg)

        # check user notice
        notices = con.call('getAccountNotices', {'uid': arg['uid'], 'notice_type': BALANCE_NOTICE_TYPE})

        if not notices:
            raise SkipNotice('Notice of type 1 is off for account, uid %s' % arg['uid'])

        if not filter(lambda x: float(x['notice_value']) > float(arg['balance']), notices[0]['notices']):
            raise SkipNotice('Good balance for agreement, agrm_id %s' % arg['agrm_id'])

        agreements = con.call('getAgreements', {'agrm_id': arg['agrm_id']})

        if not agreements:
            raise Exception('Agreement not exists for agrm_id %s' % arg['agrm_id'])

        agreement = agreements[0]
        account = con.call('getAccount', {'uid': agreement['uid']})

        if not account:
            raise Exception('Account not exists for uid %s' % agreement['uid'])

        currencies = con.call('getCurrencies', {'cur_id': agreement['cur_id']})

        if not currencies:
            raise Exception('Currency not exists for cur_id %s' % agreement['cur_id'])

        currency = currencies[0]

        params = {}
        params['username'] = account['name']
        params['agrm_number'] = agreement['agrm_num']
        params['balance'] = round(agreement['balance'], 2)
        params['currency'] = currency['symbol']

        def check_notific(transport):
            params_ = {
                'agrm_id': arg['agrm_id'],
                'notice_type': BALANCE_NOTICE_TYPE,
                'transport': transport
            }
            agrm_notices = con.call('getAgreementNotices', params_)
            return True if agrm_notices and filter(lambda x: x['last_notice'], agrm_notices[0]['notices']) else False

        def send_email_notice():
            if account['email']:
                send_mail(con, account['email'], EMAIL_TEMPLATE_ID, params)
            else:
                con.log_warning('Empty user email for uid %d.' % agreement['uid'])

        def send_sms_notice():
            if account['mobile']:
                con.call('sendSms', {
                    'numbers': [account['mobile']],
                    'template_id': SMS_TEMPLATE_ID,
                    'params': params
                })
            else:
                con.log_warning('Empty user mobile phone for uid %d.' % agreement['uid'])

        def update_agreements_notices(transport):
            now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            con.call(
                'setAgreementNotice', {
                    'agrm_id': agreement['agrm_id'],
                    'notice_type': BALANCE_NOTICE_TYPE,
                    'last_notice': now,
                    'transport': transport
                }
            )

        if notices[0]['notices'][0]['is_email']:
            if not check_notific(TRANSPORT_EMAIL):
                send_email_notice()
                update_agreements_notices(TRANSPORT_EMAIL)
            else:
                con.log_debug('Balance already notificated by email, agrm_id %s' % arg['agrm_id'])

        if notices[0]['notices'][0]['is_sms']:
            if not check_notific(TRANSPORT_SMS):
                send_sms_notice()
                update_agreements_notices(TRANSPORT_SMS)
            else:
                con.log_debug('Balance already notificated by mobile phone, agrm_id %s' % arg['agrm_id'])

    except SkipNotice, e:
        con.log_debug('Skip balance notice: %s' % str(e))

    except Exception, e:
        con.log_error('Cant notify balance: %s' % str(e))


def payment(msg, arg, con):
    # unset last_notice
    con.call('setAgreementNotice', {'agrm_id': arg['agrmid'], 'notice_type': BALANCE_NOTICE_TYPE, 'last_notice': None})


import lbcore  # NOQA
on_balance = lbcore.Messenger(notify_balance, 'change_balance')
on_payment = lbcore.Messenger(payment, 'payment')
