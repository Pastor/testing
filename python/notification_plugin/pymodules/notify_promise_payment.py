#!/usr/bin/python
# -*- coding: utf-8 -*-

import datetime


EMAIL_TEMPLATE_ID = 'email.promise_payment_down'
SMS_TEMPLATE_ID = 'sms.promise_payment_down'
PROMISE_PAYMENT_EXPIRED_NOTICE_TYPE = 2
EXPIRE_SECONDS = 24 * 60 * 60

TRANSPORT_EMAIL = 0
TRANSPORT_SMS = 1


def send_mail(con, email_to, template_id, params):
    msg = {'template_id': template_id, 'params': params, 'email_to': email_to}
    con.call('sendMail', msg)


def notify_promise_payment(msg, arg, con):

    class SkipNotice(Exception):
        pass

    date_expare = datetime.datetime.now() + datetime.timedelta(seconds=EXPIRE_SECONDS)
    uid_notices = con.call(
        'getAccountNotices', {'notice_type': PROMISE_PAYMENT_EXPIRED_NOTICE_TYPE}
    )
    try:
        for promise_payment in con.call('getPromisePayments', {'payed': 3}):
            try:
                prom_till = datetime.datetime.strptime(promise_payment['prom_till'], '%Y-%m-%d %H:%M:%S')
                uid = promise_payment['uid']

                if prom_till < date_expare and filter(lambda x: x['uid'] == uid, uid_notices):
                    agrm_id = promise_payment['agrm_id']

                    # check user notice
                    notices = con.call(
                        'getAccountNotices',
                        {'uid': uid, 'notice_type': PROMISE_PAYMENT_EXPIRED_NOTICE_TYPE}
                    )

                    if not notices:
                        raise SkipNotice('Notice of type %d is off for account, uid %s'
                                         % (PROMISE_PAYMENT_EXPIRED_NOTICE_TYPE, uid))

                    agreements = con.call('getAgreements', {'agrm_id': agrm_id})
                    if not agreements:
                        raise SkipNotice('Agreements not exists for agrm_id %s' % agrm_id)

                    agreement = agreements[0]

                    account = con.call('getAccount', {'uid': uid})
                    if not account:
                        raise SkipNotice('Account not exists for uid %s' % uid)

                    params = {}
                    params['username'] = account['name']
                    params['agrm_number'] = agreement['agrm_num']
                    params['prom_date'] = promise_payment['prom_date']
                    params['debt_date'] = promise_payment['prom_till']
                    params['amount'] = promise_payment['amount']
                    params['debt_amount'] = promise_payment['debt']
                    params['balance'] = round(agreement['balance'], 2)
                    params['currency'] = agreement['symbol']

                    def check_notific(transport):
                        params_ = {
                            'agrm_id': agrm_id,
                            'notice_type': PROMISE_PAYMENT_EXPIRED_NOTICE_TYPE,
                            'transport': transport
                        }
                        agrm_notices = con.call('getAgreementNotices', params_)
                        return True if agrm_notices and \
                                       filter(lambda x: x['last_notice'], agrm_notices[0]['notices']) else False

                    def send_email_notice():
                        if account['email']:
                            send_mail(con, account['email'], EMAIL_TEMPLATE_ID, params)
                        else:
                            con.log_warning('Empty user email for uid %d.' % uid)

                    def send_sms_notice():
                        if account['mobile']:
                            con.call('sendSms', {
                                'numbers': [account['mobile']],
                                'template_id': SMS_TEMPLATE_ID,
                                'params': params
                            })
                        else:
                            con.log_warning('Empty user mobile phone for uid %d.' % uid)

                    def update_agreements_notices(transport):
                        now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                        con.call(
                            'setAgreementNotice', {
                                'agrm_id': agreement['agrm_id'],
                                'notice_type': PROMISE_PAYMENT_EXPIRED_NOTICE_TYPE,
                                'last_notice': now,
                                'transport': transport
                            }
                        )

                    if notices[0]['notices'][0]['is_email']:
                        if not check_notific(TRANSPORT_EMAIL):
                            send_email_notice()
                            update_agreements_notices(TRANSPORT_EMAIL)
                        else:
                            con.log_debug('Promise_payment already notificated by email, agrm_id %s' % agrm_id)

                    if notices[0]['notices'][0]['is_sms']:
                        if not check_notific(TRANSPORT_SMS):
                            send_sms_notice()
                            update_agreements_notices(TRANSPORT_SMS)
                        else:
                            con.log_debug('Promise_payment already notificated by mobile phone, agrm_id %s' % agrm_id)

            except SkipNotice, e:
                con.log_debug('Skip promise_payment notice: %s' % str(e))
                continue

    except Exception, e:
        con.log_error('Cant notify promise_payment: %s' % str(e))


def payment(msg, arg, con):
    # unset last_notice
    con.call('setAgreementNotice', {'agrm_id': arg['agrmid'],
                                    'notice_type': PROMISE_PAYMENT_EXPIRED_NOTICE_TYPE,
                                    'last_notice': None})

import lbcore  # NOQA
on_hour = lbcore.Messenger(notify_promise_payment, 'hour')
on_payment = lbcore.Messenger(payment, 'payment')
