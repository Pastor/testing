#!/usr/bin/python
# -*- coding: utf-8; tab-width: 4; indent-tabs-mode: t; python-indent: 4 -*-

"""
Import block
"""

from ConfigParser import ConfigParser
from ConfigParser import Error as ConfigError
import logging
import os
from StringIO import StringIO
import sys

from yapsy import NormalizePluginNameForModuleName
from yapsy.IPlugin import IPlugin
from yapsy.PluginFileLocator import PluginFileAnalyzerWithInfoFile, PluginFileLocator
from yapsy.PluginManager import PluginManager

"""
Definitions
"""


class PluginConfigSection(object):
    """
    PluginConfigSection class
    """

    def __init__(self, name, entries=None):
        if entries:
            self.name = name
            self.__dict__.update(entries)

    def __getitem__(self, item):
        item = item.lower()
        if item in self.__dict__:
            return self.__dict__[item]
        else:
            raise ConfigError("Config file has no option '%s' in section '%s'" % (item, self.name))

    def __getattr__(self, item):
        return self[item]


class PluginConfig(object):
    """
    PluginConfig calss
    """

    def __init__(self, parser):
        for s in parser.sections():
            setattr(self, s.lower(), PluginConfigSection(s, parser.items(s)))

    def __getitem__(self, item):
        item = item.lower()
        if item in self.__dict__:
            return self.__dict__[item]
        else:
            raise ConfigError("Config file has no section '%s'" % item)

    def __getattr__(self, item):
        return self[item]

    def get(self, section, option, default=None):
        try:
            return self[section][option]
        except ConfigError:
            return default

    def get_boolian(self, section, option, default=None):
        return self.get(section, option, default) in ['True', 'true']


class ActionPlugin(IPlugin):
    """
    ActionPlugin class
    """

    def __init__(self):
        super(ActionPlugin, self).__init__()
        self.config = None

    def action_handler(self, args, con):
        pass


class ActionLogger(object):
    """
    ActionLogger class
    """

    def __new__(cls):
        if not hasattr(cls, 'instance'):
            cls.instance = super(ActionLogger, cls).__new__(cls)
        return cls.instance

    def __init__(self):
        self._logger = logging.getLogger('yapsy')
        self._logger.setLevel(logging.DEBUG)

    def set_file(self, file_name=None):
        if file_name:
            if file_name == 'stdout':
                handler = logging.StreamHandler(sys.stdout)
            else:
                handler = logging.FileHandler(file_name)
            handler.setFormatter(logging.Formatter('%(asctime)s [%(filename)s:%(lineno)d] %(levelname)s %(message)s'))
        else:
            # No logs
            class NullHandler(logging.Handler):
                def emit(self, record):
                    pass

            handler = NullHandler()

        for hdlr in self._logger.handlers:
            self._logger.removeHandler(hdlr)

        self._logger.addHandler(handler)


class ExtPluginFileLocator(PluginFileLocator):
    """
    ExtPluginFileLocator class
    """

    def __init__(self, *args, **kwargs):
        super(ExtPluginFileLocator, self).__init__(*args, **kwargs)

    def locatePlugins(self):  # NOQA
        """
        Use locatePlugins from base class.
        Add config file path to each plugin_info.
        """
        plugins, count = super(ExtPluginFileLocator, self).locatePlugins()
        for cfg, path, info in plugins:
            info.cfg = cfg
        return plugins, count


class ActionPluginManager(object):
    """
    ActionPluginManager class
    """
    DEFAULT_CONFIG_FILE = 'modules/actions/action_plugin.cfg'
    DEV_DEFAULT_CONFIG_FILE = 'pymodules/actions/action_plugin.cfg'
    DEFAULT_FILE_EXT = 'cfg'
    DEFAULT_DIR = 'plugins/actions'
    DEFAULT_LOG_FILE = "plugins/actions/action_plugin.log"
    PLUGINS_CONFIG_PATH = ''
    SPLITTER = ';'

    def __init__(self, config_file=DEFAULT_CONFIG_FILE):
        if os.path.isfile(config_file):
            self._config_file = config_file
        else:
            self._config_file = self.DEV_DEFAULT_CONFIG_FILE

        self._init_config()

        # logger
        ActionLogger().set_file(self._log_file)

        plugin_locator = ExtPluginFileLocator([PluginFileAnalyzerWithInfoFile('info_ext', self._file_ext)])
        self._manager = PluginManager(plugin_locator=plugin_locator)
        self._manager.setPluginPlaces([self._plugin_dir])
        self._manager.locatePlugins()

        candidates = []
        try:
            candidates = self._manager.getPluginCandidates()
        finally:
            pass

        for candidate_infofile, candidate_filepath, plugin_info in candidates:
            plugin_module_name_template = \
                NormalizePluginNameForModuleName("yapsy_loaded_plugin_" + plugin_info.name) + "_%d"
            for plugin_name_suffix in range(len(sys.modules)):
                plugin_module_name = plugin_module_name_template % plugin_name_suffix

                if plugin_module_name in sys.modules:
                    del sys.modules[plugin_module_name]

        self._manager.loadPlugins()

    def set_option(self, section, option, value):
        self._config.set(section, option, value)

        with open(self._config_file, 'w') as configfile:
            self._config.write(configfile)

    @property
    def use_plugin(self):
        return self._config.get('Core', 'Plugin')

    @use_plugin.setter
    def use_plugin(self, value):
        if value and not self.plugin_is_exists(value):
            raise Exception("Plugin '%s' not exists" % value)
        self.set_option('Core', 'Plugin', value)

    @property
    def file_ext(self):
        return self._file_ext

    @file_ext.setter
    def file_ext(self, value):
        self.set_option('Core', 'FileExt', value)

    @property
    def plugin_dir(self):
        return self._plugin_dir

    @plugin_dir.setter
    def plugin_dir(self, value):
        self.set_option('Core', 'Dir', value)

    @property
    def log_file(self):
        return self._log_file

    @log_file.setter
    def log_file(self, value):
        self.set_option('Core', 'Log', value)

    def get_config_value(self, config, section, value, default):
        try:
            return config.get(section, value)
        except ConfigError:
            return default

    def _init_config(self):
        self._config = ConfigParser()
        self._config.read(self._config_file)

        self._file_ext = self.get_config_value(self._config, 'Core', 'FileExt', ActionPluginManager.DEFAULT_FILE_EXT)
        self._plugin_dir = self.get_config_value(self._config, 'Core', 'Dir', ActionPluginManager.DEFAULT_DIR)
        self._log_file = self.get_config_value(self._config, 'Core', 'Log', ActionPluginManager.DEFAULT_LOG_FILE)

    def plugin_is_exists(self, plugin):
        plugin = filter(lambda x: x == plugin, self.get_list_plugins())
        return len(plugin) > 0

    def get_plugin_by_name(self, name):
        plugin_info = self._manager.getPluginByName(name)
        if plugin_info is None:
            raise Exception('Action plugin not found: %s' % name)

        plugin = plugin_info.plugin_object

        # Add all options from config file
        plugin.config = PluginConfig(plugin_info.details)

        return plugin

    def get_plugin_by_action_id(self, action_id):
        for plugin_info in self._manager.getAllPlugins():
            plugin_config = ConfigParser()
            plugin_config.read(plugin_info.cfg)
            config_action_ids = self.get_config_value(plugin_config, 'Action_IDs', 'ActionIDs', None)

            if not config_action_ids:
                continue

            action_ids = [act_id for act_id in config_action_ids.split(ActionPluginManager.SPLITTER)]

            if not len(action_ids):
                continue

            for act_id in action_ids:
                if act_id == action_id:
                    plugin = plugin_info.plugin_object
                    # Add all options from config file
                    plugin.config = PluginConfig(plugin_info.details)
                    return plugin

        return "PLUGIN_NOT_FOUND"

    def get_all_plugins_by_action_id(self, action_id):
        plugin_object_list = []
        for plugin_info in self._manager.getAllPlugins():
            plugin_config = ConfigParser()
            plugin_config.read(plugin_info.cfg)
            config_action_ids = self.get_config_value(plugin_config, 'Action_IDs', 'ActionIDs', None)

            if not config_action_ids:
                continue

            action_ids = [act_id for act_id in config_action_ids.split(ActionPluginManager.SPLITTER)]

            if not len(action_ids):
                continue

            for act_id in action_ids:
                if act_id == action_id or act_id == "any":
                    plugin = plugin_info.plugin_object
                    # Add all options from config file
                    plugin.config = PluginConfig(plugin_info.details)
                    plugin_object_list.append(plugin)
                    break

        if len(plugin_object_list):
            return plugin_object_list
        else:
            return "PLUGIN_NOT_FOUND"

    def get_cfg_plugin(self, plugin):
        plugin_info = self._manager.getPluginByName(plugin)
        if not plugin_info:
            raise Exception("Plugin '%s' not exists" % plugin)

        # file_name = '%s.%s' % (plugin.replace(' ', '_').lower(), self._file_ext)
        # file_path = os.path.join(self._plugin_dir, file_name)

        with open(plugin_info.cfg, 'r') as file_:
            return file_.read()

    def set_cfg_plugin(self, plugin, cfg):
        # if not self.plugin_is_exists(plugin):
        #     raise Exception("Plugin '%s' not exists" % plugin)
        plugin_info = self._manager.getPluginByName(plugin)
        if not plugin_info:
            raise Exception("Plugin '%s' not exists" % plugin)

        # Check config string
        try:
            parser = ConfigParser()
            parser.readfp(StringIO(cfg))

            parser.get('Core', 'Name')
            module = parser.get('Core', 'Module')

            file_name = os.path.join(self._plugin_dir, '%s.py' % module)
            with open(file_name, 'r'):
                pass

        except Exception, e:
            raise ConfigError('Invalid plugin config string: %s' % e)

        # file_name = '%s.%s' % (plugin.replace(' ', '_').lower(), self._file_ext)
        # file_path = os.path.join(self._plugin_dir, file_name)

        # with open(file_path, 'w') as file_:
        #     file_.write(cfg)

        with open(plugin_info.cfg, 'w') as file_:
            file_.write(cfg)

    def get_list_plugins(self):
        result = []
        for plugin in self._manager.getAllPlugins():
            result.append(plugin.name)
        return result
