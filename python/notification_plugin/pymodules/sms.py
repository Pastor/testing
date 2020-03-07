#!/usr/bin/python
# -*- coding: utf-8; tab-width: 4; indent-tabs-mode: t; python-indent: 4 -*-

# Available params:
# {login} - account login
# {name} - account name
# {vg_login} - vgroup login
# {agrm_number} - agreement number

import datetime
import re

import lbcore
from sms.smsplugin import SMSPluginManager

import encodings  # NOQA
try:
    import json
except ImportError:
    import simplejson as json


check_queue = True
PERIOD = 600  # seconds


def get(arg, key, default):
    return arg[key] if key in arg else default


def get_user_params(con, uid=None, agrm_id=None, vg_id=None):
    params = {}
    if uid:
        accounts = con.call('getAccounts', {'uid': uid})
        if accounts:
            account = accounts[0]
            params['login'] = account['login']
            params['name'] = account['name']
    if agrm_id:
        agreements = con.call('getAgreements', {'agrm_id': agrm_id})
        if agreements:
            agreement = agreements[0]
            params['agrm_number'] = agreement['agrm_num']
    if vg_id:
        vgroups = con.call('getVgroups', {'vg_id': vg_id})
        if vgroups:
            vg = vgroups[0]
            params['vg_login'] = vg['login']

    return params


def sync_send_sms(con, numbers, message, plugin=None, arg=None):
    manager = SMSPluginManager()
    if plugin is None:
        plugin = manager.use_plugin
    plugin = manager.get_plugin_by_name(plugin)

    billing_id = (con.call('getOption', {'name': 'billing_identity'})).get('value')
    if not billing_id:
        con.log_warning("Option 'billing_identity' not set")
    con.log_verbose(
        "SendSms: Call smsplugin '%s', numbers=(%s), message=(%s), template_id=(%s), template_params=(%s), \
billing_id=(%s)" % (plugin, numbers, message, arg['template_id'].encode('UTF-8'), arg['params'], billing_id)
    )
    plugin.send(
        numbers,
        message,
        template_id=arg['template_id'],
        template_params=arg['params'],
        billing_id=billing_id
    )


def send_sms(arg, con):
    numbers = get(arg, 'numbers', None)
    message = get(arg, 'message', '')
    template_id = get(arg, 'template_id', '')
    template_message = get(arg, 'template_message', '')
    params = get(arg, 'params', dict())
    uid = get(arg, 'uid', None)
    argm_id = get(arg, 'agrm_id', None)
    vg_id = get(arg, 'vg_id', None)
    plugin = get(arg, 'plugin', None)
    sync = get(arg, 'sync', False)

    # Use number from accounts if arg[number] is not exists
    if not numbers:
        if uid:
            accounts = con.call('getAccounts', {'uid': uid})
            if accounts:
                numbers = [accounts[0]['mobile']]

    if not numbers:
        raise Exception('Could not send SMS to empty number')

    templates_number = ['^\+(\d{10,15})$', '^(\d{10,15})$']
    con.log_verbose("Check numbers %s in the corresponding templates %s" % (numbers, templates_number))
    valid_numbers = []

    for number in numbers:
        res_num = filter(lambda x: len(x), [re.findall(tpl, number) for tpl in templates_number])
        if not len(res_num):
            con.log_error("Number '%s' is not valid" % number)
        else:
            # для российских номеров в десятизначном формате (9055555555)
            if len(number) == 10:
                number = '+7' + number
            valid_numbers.append(number)

    con.log_verbose("List of valid numbers: %s" % valid_numbers)

    if not len(valid_numbers):
        raise Exception('SMS is not sent: valid numbers not found')

    # Try template if message not exists
    if not message:
        tags = None
        if template_id:
            templates = con.call('getSbssNoticeTemplate', {'id': template_id})
            if templates:
                template_message = templates[0]['body']
                tags = templates[0]['tags']
        if not template_message:
            raise Exception('Could not send empty SMS')

        # Use API if params is not exists
        # Only available params list
        if not params:
            if uid or argm_id or vg_id:
                params = get_user_params(con, uid, argm_id, vg_id)

        # Replace variables in the template
        con.log_debug("template_message: %s" % str(template_message))
        con.log_debug("params: %s" % str(params))
        if not params and not tags:
            message = str(template_message)
        else:
            message = template_message.format(**params)

    args = {'template_id': template_id, 'params': params}
    if sync:
        sync_send_sms(
            con=con,
            numbers=valid_numbers,
            message=message,
            plugin=plugin,
            arg=args
        )
    else:
        if plugin:
            con.log_warning('In an asynchronous mode to use the default plugin')
        tels = ','.join(valid_numbers)
        con.call('setSmsQueue', {
            'plugin': plugin,
            'template_params': json.dumps(args),
            'phone_to': tels,
            'message': message
        })


def async_send_sms(con):
    global check_queue
    last_check = getattr(async_send_sms, 'last_check', datetime.datetime.min)
    if check_queue is False and datetime.datetime.now() < last_check + datetime.timedelta(seconds=PERIOD):
        return

    check_queue = False

    queue = con.call('getSmsQueue', {})

    if len(queue) > 0:
        con.log_debug('In queue is found %d sms' % len(queue))

        idle = con.idle('process', len(queue))
        for sms in queue:
            status_msg = ''
            try:
                tels = sms['phone_to'].replace(',', ';')
                tels = tels.replace(' ', ';')
                sync_send_sms(
                    con=con,
                    numbers=tels.split(';'),
                    message=sms['message'],
                    plugin=sms['plugin'],
                    arg=json.loads(sms['template_params'])
                )
            except Exception, e:
                status_msg = str(e)

            con.call(
                'setSmsHistory', {
                    'record_id': sms['record_id'],
                    'phone_to': sms['phone_to'],
                    'message': sms['message'],
                    'create_date': sms['create_date'],
                    'status_msg': status_msg,
                    'template_params': sms['template_params'],
                    'plugin': sms['plugin']
                }
            )
            con.call('delSmsQueue', {'record_id': sms['record_id']})
            idle()

    async_send_sms.last_check = datetime.datetime.now()


def add_sms_in_queue(self, arg, con):
    global check_queue
    check_queue = True


f_send_sms = lbcore.Functor(send_sms, "sendSms")
s_async_send_sms = lbcore.Service(async_send_sms, "AsyncSendSms")
m_add_sms_in_queue = lbcore.Messenger(add_sms_in_queue, "add_sms_in_queue")
