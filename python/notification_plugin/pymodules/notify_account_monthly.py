#!/usr/bin/python
# -*- coding: utf-8 -*-

import datetime

email_template_id = 'email.balance'
sms_template_id = 'sms.balance'
account_monthly_notice_type = 3
day_of_month = '01'
hour_of_day = '09'
email_time = 0
sms_time = 0


def send_mail(con, email_to, template_id, params):
    msg = {'template_id': template_id, 'params': params, 'email_to': email_to}
    con.call('sendMail', msg)


def notify_account_monthly(msg, arg, con):

    class SkipNotice(Exception):
        pass

    try:
        con.log_debug('Check notify balance at new month')

        # find users subcribed to monthly report
        notices = con.call('getAccountNotices', {'notice_type': account_monthly_notice_type})
        if not notices:
            raise SkipNotice('No accounts having notice of type 3 enabled are found')

        bd_option = con.call('getOption', {'name': 'notify_check_blocked'})

        # process all monthly report notices one by one
        for notice in notices:

            try:
                # find account
                account = con.call('getAccount', {'uid': notice['uid']})
                if not account:
                    raise Exception('Account not exists for uid %s' % notice['uid'])

                # process all account's agreements one by one
                agreements = con.call('getAgreements', {'uid': account['uid']})
                if not agreements:
                    raise SkipNotice('No agreements for account uid=%s are found' % account['uid'])

                for agreement in agreements:

                    # get agreement's currency
                    currencies = con.call('getCurrencies', {'cur_id': agreement['cur_id']})
                    if not currencies:
                        raise Exception('Currency not exists for cur_id %s' % agreement['cur_id'])
                    currency = currencies[0]

                    if bd_option['value'] == str(1):
                        vgroups_cnt = check_blocked_vg(agreement['agrm_id'], con)
                    else:
                        vgroups_cnt = 0

                    params = {}
                    params['username'] = account['name']
                    params['agrm_number'] = agreement['agrm_num']
                    params['balance'] = round(agreement['balance'], 2)
                    params['currency'] = currency['symbol']

                    if notice['notices'][0]['is_email'] is True and email_time == 1 and vgroups_cnt == 0:
                        if not account['email']:
                            con.log_warning('Empty user email for uid %d.' % notice['uid'])
                        else:
                            # send e-mail
                            send_mail(con, account['email'], email_template_id, params)

                            # update agreements_notices
                            now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                            con.call(
                                'setAgreementNotice', {
                                    'agrm_id': agreement['agrm_id'],
                                    'notice_type': account_monthly_notice_type,
                                    'last_notice': now,
                                    'transport': 0
                                }
                            )

                    if notice['notices'][0]['is_sms'] is True and sms_time == 1 and vgroups_cnt == 0:
                        if not account['mobile']:
                            con.log_warning('Empty user mobile phone for uid %d.' % notice['uid'])
                        else:
                            send_params = {
                                'numbers': [account['mobile']],
                                'template_id': sms_template_id,
                                'params': params
                            }

                            con.call('sendSms', send_params)

                            # update agreements_notices
                            now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                            con.call(
                                'setAgreementNotice', {
                                    'agrm_id': agreement['agrm_id'],
                                    'notice_type': account_monthly_notice_type,
                                    'last_notice': now,
                                    'transport': 1
                                }
                            )

            except SkipNotice, e:
                con.log_debug('Skip monthly balance notice: %s' % str(e))

            except Exception, e:
                con.log_error('Cant notify balance: %s' % str(e))

    except SkipNotice, e:
        con.log_debug('Skip monthly balance notice: %s' % str(e))

    except Exception, e:
        con.log_error('Cant notify balance: %s' % str(e))


def notify_account_monthly_auto(msg, arg, con):
    # get email time
    email_notice = con.call('getSbssSettingsNotices', {'id': email_template_id})

    if email_notice[0]['days_before']:
        email_day_of_month = str(email_notice[0]['days_before']).zfill(2)
    else:
        email_day_of_month = str(day_of_month)
    if email_notice[0]['sending_timefrom']:
        email_hour_of_day = email_notice[0]['sending_timefrom'][0:2]
    else:
        email_hour_of_day = str(hour_of_day)

    # fill current time
    current_day = str(datetime.datetime.now().day).zfill(2)
    current_hour = str(datetime.datetime.now().hour).zfill(2)

    global email_time
    if current_day == email_day_of_month and current_hour == email_hour_of_day:
        email_time = 1
    else:
        email_time = 0

    # get sms time
    sms_notice = con.call('getSbssSettingsNotices', {'id': sms_template_id})

    if sms_notice[0]['days_before']:
        sms_day_of_month = str(sms_notice[0]['days_before']).zfill(2)
    else:
        sms_day_of_month = str(day_of_month)
    if sms_notice[0]['sending_timefrom']:
        sms_hour_of_day = sms_notice[0]['sending_timefrom'][0:2]
    else:
        sms_hour_of_day = str(hour_of_day)

    # fill current time
    current_day = str(datetime.datetime.now().day).zfill(2)
    current_hour = str(datetime.datetime.now().hour).zfill(2)

    global sms_time
    if current_day == sms_day_of_month and current_hour == sms_hour_of_day:
        sms_time = 1
    else:
        sms_time = 0

    if sms_time == 1 or email_time == 1:
        notify_account_monthly(msg, arg, con)


def check_blocked_vg(agreement, con):
    # check vg blocked in 0,2 for agrm

    sql_query10 = 'SELECT count(1) as cnt FROM vgroups WHERE blocked <> 10 AND agrm_id="' + str(agreement) + '";'
    sql_query02 = 'SELECT count(1) as cnt FROM vgroups WHERE blocked IN (0,2) AND agrm_id="' + str(agreement) + '";'

    count02 = con.query(sql_query02)

    if int(count02[0]['cnt']) > 0:
        count10 = con.query(sql_query10)
        if count02[0]['cnt'] == count10[0]['cnt']:
            ret = 0
        else:
            ret = 1
    else:
        ret = 1

    return ret


import lbcore  # NOQA
on_new_hour = lbcore.Messenger(notify_account_monthly_auto, 'hour')
