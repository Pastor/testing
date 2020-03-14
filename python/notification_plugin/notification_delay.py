#!/usr/bin/python
# coding=utf-8
import os
from random import randrange
from time import sleep

fd = open('/var/opt/notification_delay.txt', 'r+')
while True:
    line = fd.readline()
    if len(line) == 0:
        break
    line = line.strip()
    if len(line) == 0:
        continue
    i = line.find(':')
    if i > 0:
        number = line[:i]
        message = line[i + 1:]
        # Случайной число в диапазоне 1, 2
        delay = randrange(2, 4)
        # Задержка в серкундах
        sleep(delay)
        command = "yowsup-cli demos --config-phone 74956642548 --send {0} \"{1}\" >> " \
                  "/var/opt/notification_delay.log 2>&1".format(number, message)
        os.system(command)
        print 'Отсылка "%s" по номеру "%s"' % (message, number)
fd.seek(0, 0)
fd.truncate()
fd.close()