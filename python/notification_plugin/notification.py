#!/usr/bin/python
# -*- coding: utf-8

import logging
import os
import re
from random import randrange
from time import sleep

from sms.smsplugin import SMSPlugin

logger = logging.getLogger('notification')


class SMS(SMSPlugin):
    def send(self, *args, **kwargs):
        numbers, message = args[0], args[1]
        for number in numbers:
            send_message(number, message, self.config)


def process_delay_send(number, message, _config):
    print 'Отложенная отсылка "%s" по номеру "%s"' % (message, number)
    fd = open('/var/opt/notification_delay.txt', 'a')
    fd.write(number)
    fd.write(':')
    fd.write(message)
    fd.write('\n')
    fd.close()


def send_message(number, message, config):
    match = re.search('Не забудьте пополнить счет', message)
    if match:
        process_delay_send(number, message, config)
    else:
        # Случайной число в диапазоне 1, 2
        delay = randrange(2, 4)
        # Задержка в серкундах
        sleep(delay)
        logger.debug('Send "%s" to %s' % (message, number))
        print 'Отсылка "%s" по номеру "%s"' % (message, number)
        command = "yowsup-cli demos --config-phone 74956642548 --send {0} \"{1}\"".format(number, message)
        os.system(command)
        logger.debug('Send command: %s' % command)
