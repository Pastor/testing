#!/usr/bin/env python
# -*- coding: utf-8 -*-

import lbcore
from sms.smsplugin import SMSPluginManager


def get_list_sms_plugins(arg, con):
    """ getListSmsPlugins functor.
    Returns list of plugins existing in system.

    """

    manager = SMSPluginManager()
    return manager.get_list_plugins()


def get_cfg_sms_plugin(arg, con):
    """ getCfgSmsPlugin functor.
    Return config for plugin.

    args: (plugin)

    """

    manager = SMSPluginManager()
    return manager.get_cfg_plugin(arg['plugin'])


def set_cfg_sms_plugin(arg, con):
    """ setCfgSmsPlugin functor.
    Save config for plugin.

    args: (plugin, cfg)

    """

    manager = SMSPluginManager()
    manager.set_cfg_plugin(arg['plugin'], arg['cfg'])


def get_default_sms_plugin(arg, con):
    """ getDefaultSmsPlugin functor.
    Return name default of plugin.

    """

    manager = SMSPluginManager()
    return manager.use_plugin


def set_default_sms_plugin(arg, con):
    """ setDefaultSmsPlugin functor.
    Set name plugin, which will be used by default.

    args: (plugin)

    """

    manager = SMSPluginManager()
    manager.use_plugin = arg['plugin']


def sms_is_ready(arg, con):
    """ smsIsReady functor.
    Check settings of system

    """

    manager = SMSPluginManager()

    return manager.plugin_is_exists(manager.use_plugin)


f_get_list_sms_plugins = lbcore.Functor(get_list_sms_plugins, "getListSmsPlugins")
f_get_cfg_sms_plugin = lbcore.Functor(get_cfg_sms_plugin, "getCfgSmsPlugin")
f_set_cfg_sms_plugin = lbcore.Functor(set_cfg_sms_plugin, "setCfgSmsPlugin")
f_get_default_sms_plugin = lbcore.Functor(get_default_sms_plugin, "getDefaultSmsPlugin")
f_set_default_sms_plugin = lbcore.Functor(set_default_sms_plugin, "setDefaultSmsPlugin")
f_sms_is_ready = lbcore.Functor(sms_is_ready, "smsIsReady")
