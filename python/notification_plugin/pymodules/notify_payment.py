#!/usr/bin/python
# -*- coding: utf-8 -*-


EMAIL_TEMPLATE_ID__PAYMENT = 'email.payment'
SMS_TEMPLATE_ID__PAYMENT = 'sms.payment'

EMAIL_TEMPLATE_ID__CANCEL_PAYMENT = 'email.cancel_payment'
SMS_TEMPLATE_ID__CANCEL_PAYMENT = 'sms.cancel_payment'

PAYMENT_NOTICE_TYPE = 4


def notify_payment(msg, arg, con):
    try:
        con.log_debug('Payment received. Check notify payment %s' % arg)

        agreements = con.call('getAgreements', {'agrm_id': arg['agrmid']})

        if not agreements:
            raise Exception('Agreement not exists for agrm_id %s' % arg['agrm_id'])

        agreement = agreements[0]
        account = con.call('getAccount', {'uid': agreement['uid']})

        if not account:
            raise Exception('Account not exists for uid %s' % agreement['uid'])

        # check user notice
        notices = con.call('getAccountNotices', {'uid': agreement['uid'], 'notice_type': PAYMENT_NOTICE_TYPE})
        if not notices:
            con.log_debug('Notice of type 4 is off for account, uid %s' % agreement['uid'])
            return

        currencies = con.call('getCurrencies', {'cur_id': agreement['cur_id']})

        if not currencies:
            raise Exception('Currency not exists for cur_id %s' % agreement['cur_id'])

        currency = currencies[0]

        params = {}
        params['username'] = account['name']
        params['amount'] = arg['amount']
        params['currency'] = currency['symbol']

        is_cancelled = arg.get('is_cancelled', False)

        if not is_cancelled:
            params['agrm_number'] = agreement['agrm_num']
            params['balance'] = round(agreement['balance'], 2)

        if notices[0]['notices'][0]['is_email'] is True:
            if not account['email']:
                con.log_warning('Empty user email for uid %d.' % agreement['uid'])
            else:
                msg = {
                    'template_id': EMAIL_TEMPLATE_ID__CANCEL_PAYMENT if is_cancelled else EMAIL_TEMPLATE_ID__PAYMENT,
                    'params': params,
                    'email_to': account['email']
                }
                con.call('sendMail', msg)

        if notices[0]['notices'][0]['is_sms'] is True:
            if not account['mobile']:
                con.log_warning('Empty user mobile phone for uid %d.' % agreement['uid'])
            else:
                send_params = {
                    'numbers': [account['mobile']],
                    'template_id': SMS_TEMPLATE_ID__CANCEL_PAYMENT if is_cancelled else SMS_TEMPLATE_ID__PAYMENT,
                    'params': params
                }
                con.call('sendSms', send_params)

    except Exception, e:
        con.log_error('Cant notify payment: %s' % str(e))


import lbcore  # NOQA
m = lbcore.Messenger(notify_payment, 'notify_payment')
