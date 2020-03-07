#!/usr/bin/python
# -*- coding: utf-8 -*-

EMAIL_TMPL_ID_SET_USR_BLK = 'email.set_user_block'
SMS_TMPL_ID_SET_USR_BLK = 'sms.set_user_block'

EMAIL_TMPL_ID_UNSET_USR_BLK = 'email.unset_user_block'
SMS_TMPL_ID_UNSET_USR_BLK = 'sms.unset_user_block'

SET_USR_BLK_NOTICE_TYPE = 7
UNSET_USR_BLK_NOTICE_TYPE = 8

BLK_TYPE_USER = 2
BLK_TYPE_ACTIVE = 0


def notify_user_block(msg, arg, con):
    try:
        con.log_debug('Block state changes. Check notify block %s' % arg)

        blk_req = arg['blk_req']
        vg_id = arg['vgid']

        # при активации надо понять, из какой блокировки переходим. Если из польз -- идем дальше
        to_user = False
        from_user = False

        if blk_req == BLK_TYPE_USER:
            to_user = True
        if blk_req == BLK_TYPE_ACTIVE:
            blocks = con.call('getVgBlockList', {'vg_id': vg_id, "sort": [{"ascdesc": 1, "name": "change_time"}]})

            # здесь анализируем последнюю блокировку из истории, есл это пользовательская -- уведомляем
            for block in blocks:
                if block['is_history']:
                    if block['block_type'] == BLK_TYPE_USER:
                        from_user = True
                    break

        if to_user or from_user:
            # получим учетку
            vgroup = con.call('getVgroupSimple', {'vg_id': vg_id})
            if not vgroup:
                raise Exception('vgroup not exists for vg_id %s' % vg_id)

            uid = vgroup['uid']
            agrmid = vgroup['agrm_id']

            # получим подписку
            notices = con.call(
                'getAccountNotices',
                {'uid': uid, 'notice_type': SET_USR_BLK_NOTICE_TYPE if to_user else UNSET_USR_BLK_NOTICE_TYPE}
            )
            if not notices:
                con.log_debug(
                    'Notice of type %s is off for account, uid %s'
                    %
                    (SET_USR_BLK_NOTICE_TYPE if to_user else UNSET_USR_BLK_NOTICE_TYPE, uid)
                )
                return
            notice = notices[0]

            notify_mail = notice['notices'][0]['is_email'] is True
            notify_sms = notice['notices'][0]['is_sms'] is True

            if notify_mail or notify_sms:
                # получим договор
                agreements = con.call('getAgreements', {'agrm_id': agrmid})
                if not agreements:
                    raise Exception('Agreement not exists for agrm_id %s' % agrmid)
                agreement = agreements[0]

                # получим пользователя
                account = con.call('getAccount', {'uid': uid})
                if not account:
                    raise Exception('Account not exists for uid %s' % uid)

                # получим параметры
                params = {}
                params['username'] = account['name']
                params['agrm_number'] = agreement['agrm_num']
                params['vglogin'] = vgroup['login']

                if notify_mail:
                    if not account['email']:
                        con.log_warning('Empty user email for uid %d.' % uid)
                    else:
                        msg = {
                            'template_id': EMAIL_TMPL_ID_SET_USR_BLK if to_user else EMAIL_TMPL_ID_UNSET_USR_BLK,
                            'params': params,
                            'email_to': account['email']
                        }
                        con.call('sendMail', msg)

                if notify_sms:
                    if not account['mobile']:
                        con.log_warning('Empty user mobile phone for uid %d.' % uid)
                    else:
                        send_params = {
                            'numbers': [account['mobile']],
                            'template_id': SMS_TMPL_ID_SET_USR_BLK if to_user else SMS_TMPL_ID_UNSET_USR_BLK,
                            'params': params
                        }
                        con.call('sendSms', send_params)
            else:
                con.log_debug('Notice of type %s is off for account, uid %s' % (
                    SET_USR_BLK_NOTICE_TYPE if to_user else UNSET_USR_BLK_NOTICE_TYPE, uid))
                return

    except Exception, e:
        con.log_error('Cant notify user block: %s' % str(e))


import lbcore  # NOQA

m = lbcore.Messenger(notify_user_block, 'block_vg')
