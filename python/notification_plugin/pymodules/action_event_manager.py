#!/usr/bin/python
# -*- coding: utf-8; tab-width: 4; indent-tabs-mode: t; python-indent: 4 -*-

"""
Import block
"""
# import datetime

from actions.action_plugin_manager import ActionPluginManager

import lbcore

"""
Definitions
"""
# Список событий акций
AET_ADD_ACT = '1'       # создание новой акции
AET_CHANGE_ACT = '2'    # изменение настроек акции
AET_DEL_ACT = '3'       # удаление акции
AET_ADD_STAFF = '4'     # добавление участника акции
AET_DEL_STAFF = '5'     # удаление участника акции
AET_NO_TYPE = '0'


# Имена параметров, передаваемых плагинам-постобработчикам событий акций
# Базовые параметры
ACTION_ID = "action_id"
EVENT_TYPE = "event_type"
# Дополнительные параметры
ADD_PARAMS = "additional_params"
USER_ID = "user_id"
UNIQ_ACT_CODE = "unique_action_code"
ACT_STAFF_ID = "action_staff_id"


def get_param(arg, key, default):
    """
    get_param - возвращает значение аргумента по его имени, либо значение
    по умолчанию
    """
    return arg[key] if key in arg else default


def param_exists(arg, key):
    """
    param_exists - возвращает True если параметр существует, иначе False
    """
    return True if key in arg else False


def action_event_manager(args, con):
    """
    action_event_manager - вызывается ядром в случае совершения какого-либо собития
    связанного с акциями. Находит соответствующие акциям плагины-обработчики и
    передает управление им.
    """
    # base parameters
    action_id = get_param(args, ACTION_ID, None)
    if not action_id:
        raise Exception("Action id is not set")

    manager = ActionPluginManager()
    # plugin = manager.get_plugin_by_action_id(action_id)
    plugin_list = manager.get_all_plugins_by_action_id(action_id)

    if plugin_list == "PLUGIN_NOT_FOUND":
        con.log_verbose("ActionEventManager: None action plugin was found for action_id: %s" % action_id)
        return

    for plugin in plugin_list:
        con.log_verbose("ActionEventManager: Call plugin '%s', args=(%s), con=(%s)" % (plugin, args, con))
        plugin.action_handler(args, con)


"""
Functors
"""


f_action_event_manager = lbcore.Functor(action_event_manager, "ActionEventManager")
