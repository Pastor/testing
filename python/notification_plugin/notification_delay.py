#!/usr/bin/python
# coding=utf-8
import requests
import datetime
from filelock import FileLock
from random import randrange
from time import sleep

base_directory = '/var/opt/'
base_directory = ""
not_sent = []

with FileLock(base_directory + 'notification_delay.lock'):
    fd = open(base_directory + 'notification_delay.txt', 'r+')
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
            delay = randrange(1, 2)
            # Задержка в серкундах
            sleep(delay)
            fdo = open(base_directory + 'notification_delay.log', 'a+')
            try:
                response = requests.post('http://127.0.0.1:8090/send', json={"to": number, "text": message})
                fdo.write('[%s] %s: %s' % (datetime.datetime.now(), number, message))
            except:
                not_sent.append(line)
                fdo.write('[%s] Ошибка отправки.  %s: %s' % (datetime.datetime.now(), number, message))
            fdo.write("\n")
            fdo.close()
    fd.seek(0, 0)
    fd.truncate()
    fd.seek(0, 0)
    for line in not_sent:
        fd.write(line)
        fd.write("\n")
    fd.close()
