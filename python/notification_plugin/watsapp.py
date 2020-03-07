#!/usr/bin/python
# -*- coding: utf-8

import logging
import os

from sms.smsplugin import SMSPlugin

logger = logging.getLogger('yapsy')


class SMS(SMSPlugin):
    def send(self, *args, **kwargs):
	numbers, message = args[0], args[1]
	for number in numbers:
	    command = "yowsup-cli demos -d --config-phone 74956642548 --send {} \"{}\"".format(number, message)
            os.system(command)
            logger.debug('Send command: %s' % command)
