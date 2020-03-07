#!/usr/bin/env python
# -*- coding: utf-8 -*-

import datetime
from email.mime.text import MIMEText
import logging
import smtplib
import sys


SMTP_TIMEOUT = 10
PYTHON_VERSION = sys.version[:5]

logger = logging.getLogger('')


class SMTP(object):
    class SSLError(Exception):
        pass

    def __init__(self, host, port, tls=False, starttls=False, auth=False, user='', password=''):
        self.host = host
        self.port = port
        self.tls = tls
        self.starttls = starttls
        self.auth = auth
        self.user = user
        self.passwod = password

    def send(self, email_from, email_to, subject, message):
        if self.tls:
            if PYTHON_VERSION >= '2.6.0':
                server = smtplib.SMTP_SSL(host=self.host, port=self.port, timeout=SMTP_TIMEOUT)
            else:
                raise SMTP.SSLError("SSL not supported for python < 2.6.0, please use STARTTLS smtp servers)")
        else:
            if PYTHON_VERSION >= '2.6.0':
                server = smtplib.SMTP(host=self.host, port=self.port, timeout=SMTP_TIMEOUT)
            else:
                server = smtplib.SMTP(host=self.host, port=self.port)

        server.ehlo()

        if self.starttls:
            if server.has_extn('STARTTLS'):
                server.starttls()
                server.ehlo()
            else:
                raise SMTP.SSLError("Server has no STARTTLS extension")

        # server.set_debuglevel(1)

        if self.auth:
            server.login(self.user, self.passwod)

        email_date = datetime.datetime.now().strftime("%d/%m/%Y %H:%M")
        msg = MIMEText(message, 'plain', 'UTF-8')
        msg['Date'] = email_date
        msg['Subject'] = subject
        msg['From'] = email_from
        msg['To'] = email_to

        logger.debug('SMS from plugin: send Mail to %s, Msg: %s, Body: %s'
                     % (email_to,
                        msg.as_string().replace('\n', ' [\\n] '),
                        message.replace('\n', ' [\\n] ')
                        )
                     )

        server.sendmail(email_from, email_to, msg.as_string())
