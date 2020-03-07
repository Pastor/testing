#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# This file presents an example of how to write a python module for LANBilling
#
# It is allowed to import any python module as it would be ordinary python script
# To interact with LANBilling you need to import "lbcore" module
#

import lbcore

msg_to_send = None
queue = []

#
# Module is not obligatory element describing The Module.
# Function SetName receives the name and the version.
# If there are two modules with the same name
# the one with the higer version will be loaded.
#

module = lbcore.Module()
module.SetName("example-module", 1)
module.SetTitle("LANBilling Python Example Module")


#
# Functor is a JSON API function accessable from outside
#
# Inside of a funtor you have connection to LANBilling named 'con'
# It is possible to run any sql-query with con.query
# or to send a message to whom it may concern
#


def py_functor_impl(arg, con):
    if arg is None:
        con.msg('pyFunctor_called', arg)
        r = con.query('SELECT NOW() AS `time`')
        return [None, True, False, 0, 0.0, "", {'data': r}]
    if 'msg' in arg:
        con.msg(arg['msg'], arg['arg'])
        return queue
    if str(arg['z']) == '3.14':
        raise lbcore.Error('Passed invalid value z = "$1". Should not be "$2"', [arg['z'], '3.14'])
    return str(arg['x'] + arg['y']) + str(arg['z'])


f = lbcore.Functor(py_functor_impl, "pyFunctor")

#
# Service is called by LANBilling in a cycle without a stop
#

# def py_service_impl(con):
#     global msg_to_send
#     if msg_to_send is not None:
#         con.msg(msg_to_send)
#
# g = lbcore.Service( pyServiceImpl, "pyService" ) # uncomment this line to check

#
#  It is possible to catch any message sent by LANBilling
#  Message sender will always wait until this script handles it completely
#


def py_on_message(msg, arg, con):
    global msg_to_send
    global queue
    msg_to_send = None
    if arg is None:
        queue = []
    queue += [arg]

h = lbcore.Messenger(py_on_message, "test_message")
