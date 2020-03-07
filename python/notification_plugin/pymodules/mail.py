#!/usr/bin/python
# -*- coding: utf-8 -*-

import datetime
from email.header import Header
from email.mime.text import MIMEText
import imaplib
import poplib
import smtplib
import sys
import traceback

import encodings  # NOQA
try:
    import json
except ImportError:
    import simplejson as json


PYTHON_VERSION = sys.version[:5]
if PYTHON_VERSION >= '2.6.0':
    import ssl

check_queue = True
PERIOD = 600  # seconds


def get(arg, key, default):
    return arg[key] if key in arg else default


def get_option(con, name, default=''):
    value = default
    r = con.query("select value from options where name = '%s'" % name)
    if r:
        value = get(r[0], 'value', default)
    return value


def sync_send_mail(con, email_from, email_to, msg, smtp_settings={}):
    smtphost = get(smtp_settings, 'smtphost', '')
    smtpport = get(smtp_settings, 'smtpport', '')
    smtpuser = get(smtp_settings, 'smtpuser', '')
    smtppass = get(smtp_settings, 'smtppass', '')
    smtptls = get(smtp_settings, 'smtptls', '')
    smtpmethod = get(smtp_settings, 'smtpmethod', '')
    smtptimeout = get(smtp_settings, 'smtptimeout', 10)

    server = None
    try:
        con.log_debug('SendMail: host: %s , port:%s , timeout: %s'
                      % (str(smtphost), str(smtpport), str(smtptimeout)))
        if PYTHON_VERSION >= '2.6.0':
            if smtptls == '1':
                try:
                    server = smtplib.SMTP_SSL(host=smtphost, port=smtpport.encode('utf8'), timeout=smtptimeout)
                    server.ehlo()
                except ssl.SSLError:
                    con.log_debug('SendMail: Server raise SSLError, try STARTTLS...')
            if server is None:
                server = smtplib.SMTP(host=smtphost, port=smtpport.encode('utf8'), timeout=smtptimeout)
                server.ehlo()
        else:
            if not hasattr(send_mail, 'ssl_warning_sended'):
                con.log_warning('SendMail: SSL not supported SSL for python < 2.6.0, please use STARTTLS smtp servers')
                send_mail.ssl_warning_sended = True
            server = smtplib.SMTP(host=str(smtphost), port=smtpport.encode('utf8'), timeout=smtptimeout)
            server.ehlo()

        if smtptls == '1' and server.has_extn('STARTTLS'):
            server.starttls()
            server.ehlo()

        #  server.set_debuglevel(1)

        if smtpmethod > '0' and smtpuser and smtppass:
            server.login(smtpuser.encode('utf8'), smtppass.encode('utf8'))

        email_to = email_to.replace(',', ';')
        email_to = email_to.replace(' ', ';')
        emails = filter(lambda x: len(x) > 0, email_to.split(';'))
        server.sendmail(email_from, emails, msg)

        server.quit()
    except Exception, e:
        con.log_error(str(e) + " traceback: " + str(traceback.format_exc()))
        if server:
            server.close()
        return False
    return True


def send_mail(arg, con):
    email_from = get(arg, 'email_from', '')
    email_to = get(arg, 'email_to', '')
    subject = get(arg, 'subject', '')
    message = get(arg, 'message', '')
    message_type = get(arg, 'message_type', '')
    sync = get(arg, 'sync', False)
    template_id = get(arg, 'template_id', '')
    template_message = get(arg, 'template_message', '')
    params = get(arg, 'params', dict())

    if not email_to:
        con.log_warning('SendMail: email_to is empty. Skipped.')
        return False

    if not message_type:
        message_type = get_option(con, 'crm_content_type', 'plain')

    if message_type != 'html':
        message_type = 'plain'

    if not email_from:
        email_from = get_option(con, 'crm_email_box')

    if not subject:
        subject = get_option(con, 'crm_email_subject')

    # Try template if message not exists
    if not message:
        if template_id:
            templates = con.call('getSbssNoticeTemplate', {'id': template_id})
            if templates:
                template_message = templates[0]['body']
                subject = templates[0]['theme']
            else:
                con.log_error('Message template not found')
        if not template_message:
            raise Exception('Could not send empty message')

        # Replace variables in the template
        con.log_debug("template_message: %s" % str(template_message))
        con.log_debug("params: %s" % str(params))
        message = template_message.format(**params)

    msg = MIMEText(message, message_type, 'UTF-8')
    msg['Subject'] = Header(subject, 'UTF-8')
    msg['From'] = email_from
    msg['To'] = email_to

    smtphost = get(arg, 'smtphost', '')
    smtpport = get(arg, 'smtpport', '')
    smtpuser = get(arg, 'smtpuser', '')
    smtppass = get(arg, 'smtppass', '')
    smtptls = get(arg, 'smtptls', '')
    smtpmethod = get(arg, 'smtpmethod', '')
    smtptimeout = get(arg, 'smtptimeout', 0)

    if not smtphost:
        smtphost = get_option(con, 'crm_email_smtphost')

    if not smtpport:
        smtpport = get_option(con, 'crm_email_smtpport')

    if not smtpuser:
        smtpuser = get_option(con, 'crm_email_smtpuser')

    if not smtppass:
        smtppass = get_option(con, 'crm_email_smtppass')

    if not smtptls:
        smtptls = get_option(con, 'crm_email_smtptls')

    if not smtpmethod:
        smtpmethod = get_option(con, 'crm_email_smtpmethod')
    if not smtptimeout:
        smtptimeout = int(get_option(con, 'crm_email_smtptimeout', 10))

    smtp_settings = {
        'smtphost': smtphost,
        'smtpport': smtpport,
        'smtpuser': smtpuser,
        'smtppass': smtppass,
        'smtptls': smtptls,
        'smtpmethod': smtpmethod,
        'smtptimeout': smtptimeout
    }

    if sync:
        return sync_send_mail(
            con=con,
            email_from=email_from,
            email_to=email_to,
            msg=msg.as_string(),
            smtp_settings=smtp_settings
        )
    else:
        con.call('setEmailQueue', {
            'email_from': email_from,
            'email_to': email_to,
            'message': msg.as_string(),
            'template_params': json.dumps(params),
            'smtp_settings': json.dumps(smtp_settings)
        })

    return True


def async_send_mail(con):
    global check_queue
    last_check = getattr(async_send_mail, 'last_check', datetime.datetime.min)
    if check_queue is False and datetime.datetime.now() < last_check + datetime.timedelta(seconds=PERIOD):
        return

    check_queue = False

    queue = con.call('getEmailQueue', {})

    if len(queue) > 0:
        con.log_debug('In queue is found %d mail' % len(queue))

        idle = con.idle('process', len(queue))
        for mail in queue:
            status_msg = ''
            try:
                sync_send_mail(
                    con=con,
                    email_from=mail['email_from'],
                    email_to=mail['email_to'],
                    msg=mail['message'],
                    smtp_settings=json.loads(mail['smtp_settings'])
                )
            except Exception, e:
                status_msg = str(e)

            con.call(
                'setEmailHistory', {
                    'queue_id': mail['record_id'],
                    'email_from': mail['email_from'],
                    'email_to': mail['email_to'],
                    'message': mail['message'],
                    'create_date': mail['create_date'],
                    'status_msg': status_msg,
                    'template_params': mail['template_params'],
                    'smtp_settings': mail['smtp_settings']
                }
            )
            con.call('delEmailQueue', {'record_id': mail['record_id']})
            idle()

    async_send_mail.last_check = datetime.datetime.now()


def add_mail_in_queue(self, arg, con):
    global check_queue
    check_queue = True


def email_is_ready(arg, con):
    smtphost = get_option(con, 'crm_email_smtphost')
    smtpport = get_option(con, 'crm_email_smtpport')

    return True if smtphost and smtpport else False


def _get_check_mail_params(arg, con):
    protocol = get(arg, 'protocol', '')
    hostname = get(arg, 'hostname', '')
    port = get(arg, 'port', '')
    use_tls = get(arg, 'use_tls', '')
    username = get(arg, 'username', '')
    password = get(arg, 'password', '')
    email_box = get(arg, 'email_box', '')
    imap_folder = get(arg, 'imap_folder', '')

    return (protocol, hostname, port, use_tls, username, password, email_box, imap_folder)


def _login_on_pop3(con, host, port, username, password, use_tls):
    if PYTHON_VERSION < '2.6.0' and use_tls == '1':
        con.log_warning('_login_on_pop3: python version < 2.6.0 and set TLS option.'
                        ' SSL do not supported for this python version. Skipped.')
        return None

    try:
        pop3_connection = None
        if use_tls == '0':
            pop3_connection = poplib.POP3(host, port)
        elif use_tls == '1':
            pop3_connection = poplib.POP3_SSL(host, port)
        else:
            return None

        pop3_connection.user(username)
        pop3_connection.pass_(password)

        if '+OK' in pop3_connection.getwelcome():
            return pop3_connection
        else:
            return None
    except poplib.error_proto:
        con.log_warning('_login_on_pop3: Error when authentication.')
        return None


def _login_on_imap(con, host, port, username, password, use_tls):
    if PYTHON_VERSION < '2.6.0' and use_tls == '1':
        con.log_warning('_login_on_imap: python version < 2.6.0 and set TLS option.'
                        ' SSL do not supported for this python version. Skipped.')
        return None

    try:
        imap_connection = None
        if use_tls == '0':
            imap_connection = imaplib.IMAP4(host, port)
        elif use_tls == '1':
            imap_connection = imaplib.IMAP4_SSL(host, port)
        else:
            return None

        if imap_connection.login(username, password)[0] == 'OK':
            return imap_connection

    except imaplib.IMAP4.error:
        con.log_warning('_login_on_imap: Error when authentication.')
        return None


def _try_get_mail_list_pop3(con, connection):
    response, lst, octets = connection.list()
    if '+OK' in response:
        return True
    else:
        con.log_warning('_try_get_mail_list_pop3: Error when get mail list: ' + response)
        return False


def _try_get_mail_list_imap(con, connection, imap_folder):
    status, data = connection.select(imap_folder)
    if status == 'OK':
        return True
    else:
        con.log_warning('_try_get_mail_list_imap: Error when get mail list: ' + status)
        return False


def check_email_login(arg, con):
    protocol, hostname, port, use_tls, username, password, email_box, imap_folder = _get_check_mail_params(arg, con)

    if protocol == '0':
        result = _login_on_pop3(con, hostname, port, username, password, use_tls)
        if result:
            result.quit()
            return True
        else:
            return False
    elif protocol == '1':
        result = _login_on_imap(con, hostname, port, username, password, use_tls)
        if result:
            result.logout()
            return True
        else:
            return False
    else:
        return False


def check_mail_list_exists(arg, con):
    protocol, hostname, port, use_tls, username, password, email_box, imap_folder = _get_check_mail_params(arg, con)

    result = None
    if protocol == '0':
        result = _login_on_pop3(con, hostname, port, username, password, use_tls)
        if not result:
            return False
        get_mails = _try_get_mail_list_pop3(con, result)
        if get_mails:
            result.quit()
            return True
        else:
            result.quit()
            return False
    elif protocol == '1':
        result = _login_on_imap(con, hostname, port, username, password, use_tls)
        if not result:
            return False
        get_mails = _try_get_mail_list_imap(con, result, imap_folder)
        if get_mails:
            result.close()
            result.logout()
            return True
        else:
            result.logout()
            return False
    else:
        return False


def check_smtp_settings(arg, con):
    smtphost = get(arg, 'smtphost', '')
    smtpport = get(arg, 'smtpport', '')
    smtpuser = get(arg, 'smtpuser', '')
    smtppass = get(arg, 'smtppass', '')
    smtptls = get(arg, 'smtptls', '')
    smtpmethod = get(arg, 'smtpmethod', '')
    smtptimeout = float(get(arg, 'smtptimeout', 10))

    server = None
    try:
        con.log_debug('SendMail: host: %s , port:%s , timeout: %s'
                      % (str(smtphost), str(smtpport), str(smtptimeout)))
        if PYTHON_VERSION >= '2.6.0':
            if smtptls == '1':
                try:
                    server = smtplib.SMTP_SSL(host=smtphost, port=smtpport.encode('utf8'), timeout=smtptimeout)
                    server.ehlo()
                except ssl.SSLError:
                    con.log_debug('SendMail: Server raise SSLError, try STARTTLS...')
            if server is None:
                server = smtplib.SMTP(host=smtphost, port=smtpport.encode('utf8'), timeout=smtptimeout)
                server.ehlo()
        else:
            if not hasattr(send_mail, 'ssl_warning_sended'):
                con.log_warning('SendMail: SSL not supported SSL for python < 2.6.0, please use STARTTLS smtp servers')
                send_mail.ssl_warning_sended = True
            server = smtplib.SMTP(host=str(smtphost), port=smtpport.encode('utf8'), timeout=smtptimeout)
            server.ehlo()

        if smtptls == '1' and server.has_extn('STARTTLS'):
            server.starttls()
            server.ehlo()

        if smtpmethod > '0' and smtpuser and smtppass:
            server.login(smtpuser.encode('utf8'), smtppass.encode('utf8'))
        server.quit()
    except Exception, e:
        con.log_error(str(e) + " traceback: " + str(traceback.format_exc()))
        if server:
            server.close()
        return False
    return True


import lbcore  # NOQA
f_send_mail = lbcore.Functor(send_mail, "sendMail")
f_email_is_ready = lbcore.Functor(email_is_ready, "emailIsReady")
f_check_mail_login = lbcore.Functor(check_email_login, "checkEmailLogin")
f_check_mail_list_exists = lbcore.Functor(check_mail_list_exists, "checkMailList")
f_check_smtp_settings = lbcore.Functor(check_smtp_settings, "checkSMTP")
s_async_send_mail = lbcore.Service(async_send_mail, "AsyncSendMail")
m_async_send_mail = lbcore.Messenger(add_mail_in_queue, "add_email_in_queue")
