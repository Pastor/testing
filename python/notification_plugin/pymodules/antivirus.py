#!/usr/bin/python
# -*- coding: utf-8 -*-

import ConfigParser
import datetime
import Queue
import uuid

import encodings  # NOQA
try:
    import json
except ImportError:
    import simplejson as json

import fsecure
import lbcore

ORDERING_WSDL = ""
SUBSCRIPTION_MANAGMENT_WSDL = ""
SECURE_ECHO_WSDL = ""
USERNAME = ""
PASSWORD = ""
OPERID = 0

# Usbox service uuid prefix
SERVICE_UUID_PREFIX = 'Antivirus'


VERSION = '0.3.2'

module = lbcore.Module()
module.SetName('antivirus-module', 1)
module.SetTitle('LANBilling Antivirus Module')

DATETIME_FORMAT = '%Y-%m-%d %H:%M:%S'
DATETIME_FORMAT_WS = '%Y-%m-%d %H:%M:00'
MYSQL_MAX_DATETIME = '9999-12-31 23:59:59'

PROMO_PERIOD_OPTION = 'antivirus_promo_period'
PROMO_PERIOD_DEFAULT = 30

ACTIVATION_PERIOD_OPTION = 'antivirus_activation_period'
ACTIVATION_PERIOD_DEFAULT = 30

NOTIFY_ACTIVATION_EXPIRE_OPTION = 'antivirus_notify_activation_expire'
NOTIFY_ACTIVATION_EXPIRE_DEFAULT = 10

BILLING_IDENTITY_OPTIONS = 'billing_identity'

CHECK_ACTIVATED_TIMEOUT = 15 * 60
TIMEOUT_ON_ERROR = 60


# -- verbose log
ENABLE_VERBOSE_LOG = True


def log(f):
    def wrapper(*args, **kwargs):
        con = module.con if hasattr(module, 'con') else None
        if ENABLE_VERBOSE_LOG and con:
            con.log_debug('ANTIVIRUS: Call: %s%s' % (f.__name__, args))
        return f(*args, **kwargs)

    wrapper.__name__ = f.__name__
    return wrapper

# --


# decorator for creation LBcore Functors
def lbcore_fuctor(var, name):
    def wrapper(f):
        module.__dict__[var] = lbcore.Functor(f, name)

        def inner(*args, **kwargs):
            return f(*args, **kwargs)

        inner.__name__ = f.__name__
        return inner

    return wrapper


# decorator for creation LBcore Messengers
def lbcore_messenger(var, msg):
    def wrapper(f):
        module.__dict__[var] = lbcore.Messenger(f, msg)

        def inner(*args, **kwargs):
            return f(*args, **kwargs)

        inner.__name__ = f.__name__
        return inner

    return wrapper


# decorator for creation LBcore Service
def lbcore_service(var, name):
    def wrapper(f):
        module.__dict__[var] = lbcore.Service(f, name)

        def inner(*args, **kwargs):
            return f(*args, **kwargs)

        inner.__name__ = f.__name__
        return inner

    return wrapper


# datetime.timedelta.total_seconds for python < 2.7
def total_seconds(td):
    return (td.microseconds + (td.seconds + td.days * 24 * 3600) * 10 ** 6) / 10 ** 6


def load_settings(con):
    try:
        global ORDERING_WSDL
        global SUBSCRIPTION_MANAGMENT_WSDL
        global SECURE_ECHO_WSDL
        global USERNAME
        global PASSWORD
        global OPERID

        config = ConfigParser.ConfigParser()
        config.read('pymodules/antivirus_local_settings.cfg')

        ORDERING_WSDL = config.get('Settings', 'ORDERING_WSDL')
        SUBSCRIPTION_MANAGMENT_WSDL = config.get('Settings', 'SUBSCRIPTION_MANAGMENT_WSDL')
        SECURE_ECHO_WSDL = config.get('Settings', 'SECURE_ECHO_WSDL')
        USERNAME = config.get('Settings', 'USERNAME')
        PASSWORD = config.get('Settings', 'PASSWORD')
        OPERID = config.get('Settings', 'OPERID')
    except Exception, e:
        con.log_info('Antivirus is not set: %s' % e)
        return False
    return True


# class for using FSecure API
class FSecureAPI(object):
    block_queue = Queue.Queue()
    block_queue_err = Queue.Queue()

    @staticmethod
    @log
    def init():
        try:
            fsecure.echo(SECURE_ECHO_WSDL, 'msg', USERNAME, PASSWORD, OPERID)
            fsecure.init(USERNAME, PASSWORD, OPERID, ORDERING_WSDL, SUBSCRIPTION_MANAGMENT_WSDL)
        except Exception, e:
            raise Exception('FSecure init error. %s' % str(e.message))

    @staticmethod
    @log
    def check_status():
        return fsecure.status()

    @staticmethod
    @log
    def get_key(con, service):
        if not FSecureAPI.check_status():
            FSecureAPI.init()

        key = LB.get_fsecure_id(con, service)

        con.log_debug('ANTIVIRUS: GetKey returns %s' % key)
        try:
            return fsecure.get_key(key)
        except Exception, e:
            con.log_warning("ANTIVIRUS: FSecure Get key failed, try again. Msg: %s" % str(e))
            return fsecure.get_key(key)

    @staticmethod
    @log
    def add_key(con, id_, product):
        if not FSecureAPI.check_status():
            FSecureAPI.init()

        con.log_debug('ANTIVIRUS: FSecure: Add key %s' % id_)
        try:
            return fsecure.add_key(id_, product)
        except Exception, e:
            con.log_warning("ANTIVIRUS: Fsecure Add key failed, try again. Msg: %s" % str(e))
            return fsecure.add_key(id_, product)

    @staticmethod
    @log
    def do_activate_key(key):
        fsecure.activate_key(key)

    @staticmethod
    @log
    def activate_key(con, key, sync=False):
        if sync:
            if not FSecureAPI.check_status():
                FSecureAPI.init()
            try:
                FSecureAPI.do_activate_key(key)
            except Exception, e:
                con.log_warning("ANTIVIRUS: Fsecure Activate key failed, try again. Msg: %s" % str(e))
                FSecureAPI.do_activate_key(key)
            con.log_verbose('ANTIVIRUS: Key %s is activated' % key)
        else:
            FSecureAPI.block_queue.put({'operation': FSecureAPI.activate_key, 'key': key})

    @staticmethod
    @log
    def do_deactivate_key(con, key):
        if fsecure.get_key(key)['status']:
            fsecure.deactivate_key(key)
        else:
            con.log_verbose('ANTIVIRUS: Already deativated key %s' % key)

    @staticmethod
    @log
    def deactivate_key(con, key, sync=False):
        if sync:
            if not FSecureAPI.check_status():
                FSecureAPI.init()
            try:
                FSecureAPI.do_deactivate_key(con, key)
            except Exception, e:
                con.log_warning("ANTIVIRUS: Fsecure Deactivate key %s failed, try again. Msg: %s" % (key, str(e)))
                FSecureAPI.do_deactivate_key(con, key)
            con.log_verbose('ANTIVIRUS: Key %s is deactivated' % key)
        else:
            FSecureAPI.block_queue.put({'operation': FSecureAPI.deactivate_key, 'key': key})


# class for LB interaction
class LB(object):
    @staticmethod
    @log
    def service_prefix():
        return SERVICE_UUID_PREFIX

    @staticmethod
    @log
    def get_activation_period(con):
        try:
            return int(con.call('getOption', {'name': ACTIVATION_PERIOD_OPTION})['value'])
        except Exception:
            return ACTIVATION_PERIOD_DEFAULT

    @staticmethod
    @log
    def get_promo_period(con):
        try:
            return int(con.call('getOption', {'name': PROMO_PERIOD_OPTION})['value'])
        except Exception:
            return PROMO_PERIOD_DEFAULT

    @staticmethod
    @log
    def get_identity(con):
        try:
            return con.call('getOption', {'name': BILLING_IDENTITY_OPTIONS})['value']
        except:
            raise Exception('billing_identity is not defined')

    @staticmethod
    @log
    def get_notify_period(con):
        try:
            return int(con.call('getOption', {'name': NOTIFY_ACTIVATION_EXPIRE_OPTION})['value'])
        except Exception:
            return NOTIFY_ACTIVATION_EXPIRE_DEFAULT

    @staticmethod
    @log
    def generate_fsecure_id():
        return str(uuid.uuid1())

    @staticmethod
    @log
    def get_fsecure_id(con, service):
        external_data = json.loads(service['external_data'])
        return external_data['id'].encode('ascii')

    @staticmethod
    @log
    def is_antivirus_service(con, service):
        service_uuid = service['uuid']
        if service_uuid is not None:
            try:
                (prefix, product) = tuple(service_uuid.split('#'))
                return prefix == LB.service_prefix()
            except ValueError:
                pass
        return False

    @staticmethod
    @log
    def is_activated_service(con, service):
        try:
            external_data = json.loads(service['external_data'])
        except Exception, e:
            con.log_warning(
                'ANTIVIRUS: Broken service %s. Message: %s' % (service, str(e.message))
            )
            return False

        try:
            return datetime.datetime.strptime(external_data['activated'], DATETIME_FORMAT) <= datetime.datetime.now()
        except Exception, e:
            con.log_debug(
                'ANTIVIRUS: Activation time is not defined for service %s. Message: %s' % (service, str(e.message))
            )
            return False

    @staticmethod
    @log
    def get_usbox_service(con, serv_id):
        services = con.call('getUsboxServices', {'serv_id': serv_id, 'not_usbox': False})
        if len(services) != 1:
            raise Exception('Error on get usbox service with serv_id = %d' % serv_id)
        return services[0]

    @staticmethod
    @log
    def get_service(con, service_id):
        services = con.call('getUsboxServices', {'serv_id': service_id, 'not_usbox': True})
        if len(services) != 1:
            raise Exception('Error on get service with serv_id = %d' % service_id)
        return services[0]

    @staticmethod
    @log
    def update_usbox_service(con, serv_id, timefrom=None, timeto=None, activated=None, external_data=None):
        if timefrom or timeto or activated or external_data:
            if ENABLE_VERBOSE_LOG:
                con.log_debug('ANTIVIRUS: timefrom %s' % timefrom)
                con.log_debug('ANTIVIRUS: timeto %s' % timeto)
                con.log_debug('ANTIVIRUS: activated %s' % activated)
                con.log_debug('ANTIVIRUS: external_data %s' % external_data)

            timefrom_query = "'%s'" % timefrom.strftime(DATETIME_FORMAT) if timefrom is not None else 'timefrom'
            timeto_query = "'%s'" % timeto.strftime(DATETIME_FORMAT) if timeto is not None else 'timeto'
            activated_query = "'%s'" % activated.strftime(DATETIME_FORMAT) if activated is not None else 'activated'
            external_data_query = "'%s'" % external_data if external_data is not None else 'external_data'

            con.query("UPDATE usbox_services SET timefrom = %s, timeto = %s, activated = %s, external_data = %s "
                      "WHERE serv_id = %d" %
                      (timefrom_query, timeto_query, activated_query, external_data_query, serv_id))

    @staticmethod
    @log
    def update_service(con, service_id, timefrom=None, timeto=None, activated=None, external_data=None):
        if timefrom or timeto or activated or external_data:
            if ENABLE_VERBOSE_LOG:
                con.log_debug('ANTIVIRUS: timefrom %s' % timefrom)
                con.log_debug('ANTIVIRUS: timeto %s' % timeto)
                con.log_debug('ANTIVIRUS: activated %s' % activated)
                con.log_debug('ANTIVIRUS: external_data %s' % external_data)

            timefrom_query = "'%s'" % timefrom.strftime(DATETIME_FORMAT) if timefrom is not None else 'timefrom'
            timeto_query = "'%s'" % timeto.strftime(DATETIME_FORMAT) if timeto is not None else 'timeto'
            activated_query = "'%s'" % activated.strftime(DATETIME_FORMAT) if activated is not None else 'activated'
            external_data_query = "'%s'" % external_data if external_data is not None else 'external_data'

            con.query("UPDATE services SET timefrom = %s, timeto = %s, activated = %s, external_data = %s "
                      "WHERE service_id = %d" %
                      (timefrom_query, timeto_query, activated_query, external_data_query, service_id))

    @staticmethod
    @log
    def get_user_promo_period(con, uid):
        services = con.call('getUsboxServices', {'uid': uid, 'cat_uuid': LB.service_prefix()})
        for service in services:
            try:
                external_data = json.loads(service['external_data'])
            except Exception, e:
                con.log_warning('ANTIVIRUS: Broken service %s. Message: %s' % (service, str(e.message)))
                continue
            try:
                activated = datetime.datetime.strptime(external_data['activated'], DATETIME_FORMAT)
                if activated < datetime.datetime.now():
                    return 0
            except Exception, e:
                con.log_debug(
                    'ANTIVIRUS: Activation time is not defined for service %s. Message: %s' % (service, str(e.message))
                )
                continue
        return LB.get_promo_period(con)


# Antivirus API implementation
class APIAntivirus(object):
    def __init__(self):
        global module
        module.activate_usbox_key = lbcore.Functor(self.activate_usbox_service, "antivirusActivateUsboxKey")
        module.activate_service_key = lbcore.Functor(self.activate_service, "antivirusActivateServiceKey")
        module.on_create_usbox_service = lbcore.Messenger(self.on_create_usbox_service, "after_create_usbox")
        module.on_create_service = lbcore.Messenger(self.on_create_service, "after_create_service")
        module.on_vg_block = lbcore.Messenger(self.on_vg_block, "block_vg")
        module.on_stop_usbox = lbcore.Messenger(self.on_stop_usbox_service, "stop_usbox")
        module.on_stop_seervice = lbcore.Messenger(self.on_stop_service, "stop_service")
        module.on_start_usbox = lbcore.Messenger(self.on_start_usbox_service, "start_usbox")
        module.on_start_service = lbcore.Messenger(self.on_start_service, "start_service")
        module.on_del_usbox = lbcore.Messenger(self.on_del_usbox_service, "pre_del_usbox")
        module.on_del_service = lbcore.Messenger(self.on_del_service, "pre_del_service")
        module.activator = lbcore.Service(self.activator, "AntivirusActivator")
        module.blocker = lbcore.Service(self.blocker, "AntivirusBlocker")

    @log
    def activate_usbox_service(self, arg, con):
        serv_id = arg['serv_id']
        service = LB.get_usbox_service(con, serv_id)

        activated = datetime.datetime.strptime(arg['activated'], DATETIME_FORMAT)

        if LB.is_antivirus_service(con, service):
            promo_period = LB.get_user_promo_period(con, service['uid'])
            if promo_period:
                activated = activated.date() + datetime.timedelta(days=promo_period)
            external_data = json.loads(service['external_data'])
            external_data['activated'] = arg['activated']
            LB.update_usbox_service(
                con,
                serv_id,
                activated=activated,
                external_data=json.dumps(external_data),
                timeto=datetime.datetime.strptime(MYSQL_MAX_DATETIME, DATETIME_FORMAT)
            )

    @log
    def activate_service(self, arg, con):
        service_id = arg['service_id']
        service = LB.get_service(con, service_id)

        activated = datetime.datetime.strptime(arg['activated'], DATETIME_FORMAT)

        if LB.is_antivirus_service(con, service):
            promo_period = LB.get_user_promo_period(con, service['uid'])
            if promo_period:
                activated = activated.date() + datetime.timedelta(days=promo_period)
            external_data = json.loads(service['external_data'])
            external_data['activated'] = arg['activated']
            LB.update_service(
                con,
                service_id,
                activated=activated,
                external_data=json.dumps(external_data),
                timeto=datetime.datetime.strptime(MYSQL_MAX_DATETIME, DATETIME_FORMAT)
            )

    @log
    def on_create_usbox_service(self, msg, arg, con):

        serv_id = arg['serv_id']

        service = None
        try:
            service = LB.get_usbox_service(con, serv_id)
            if LB.is_antivirus_service(con, service):
                product = service['uuid'].split('#')[1]
                fsecure_id = LB.generate_fsecure_id()
                generated_key = FSecureAPI.add_key(con, fsecure_id, product)

                if ENABLE_VERBOSE_LOG:
                    con.log_debug('ANTIVIRUS: FSecure key: "%s" for service %d: %s'
                                  % (generated_key, serv_id, service['cat_descr']))

                activation_period = LB.get_activation_period(con)
                timefrom = datetime.datetime.strptime(service['time_from'], DATETIME_FORMAT)
                activated = timefrom + datetime.timedelta(days=activation_period)

                external_data = {
                    'key': generated_key['key'],
                    'url': generated_key['url'],
                    'id': fsecure_id,
                }

                LB.update_usbox_service(
                    con,
                    serv_id,
                    activated=activated,
                    timeto=activated,
                    external_data=json.dumps(external_data)
                )
        except Exception, e:
            serv_desc = str(serv_id) if service is None else service
            con.log_error(
                'ANTIVIRUS: OnCreateUsboxService filed for usbox service %s. Message: %s' % (serv_desc, str(e.message))
            )
            serv_desc = str(serv_id) if service is None else service['cat_descr']
            raise lbcore.Error('Cant create Antivirus usbox service "$1". See logs for details.', [serv_desc])

    @log
    def on_create_service(self, msg, arg, con):

        service_id = arg['service_id']

        service = None
        try:
            service = LB.get_service(con, service_id)
            if LB.is_antivirus_service(con, service):
                product = service['uuid'].split('#')[1]
                fsecure_id = LB.generate_fsecure_id()
                generated_key = FSecureAPI.add_key(con, fsecure_id, product)

                if ENABLE_VERBOSE_LOG:
                    con.log_debug(
                        'ANTIVIRUS: FSecure key: "%s" for service %d: %s' %
                        (generated_key, service_id, service['cat_descr'])
                    )

                activation_period = LB.get_activation_period(con)
                timefrom = datetime.datetime.strptime(service['time_from'], DATETIME_FORMAT)
                activated = timefrom + datetime.timedelta(days=activation_period)

                external_data = {
                    'key': generated_key['key'],
                    'url': generated_key['url'],
                    'id': fsecure_id,
                }

                LB.update_service(
                    con,
                    service_id,
                    activated=activated,
                    timeto=activated,
                    external_data=json.dumps(external_data)
                )
        except Exception, e:
            serv_desc = str(service_id) if service is None else service
            con.log_error(
                'ANTIVIRUS: OnCreateService filed for service %s. Message: %s' % (serv_desc, str(e.message)))
            serv_desc = str(service_id) if service is None else service['cat_descr']
            raise lbcore.Error('Cant create Antivirus service "$1". See logs for details.', [serv_desc])

    @log
    def on_vg_block(self, msg, arg, con):
        vg_id = arg['vgid']
        r = con.query('SELECT blocked FROM vgroups WHERE vg_id = %d' % vg_id)
        if len(r) != 1:
            raise Exception('ANTIVIRUS: OnVgBlock failed for vg_id %d. Cant get blocked from vgroups' % vg_id)

        blocked = r[0]['blocked']
        f = [FSecureAPI.deactivate_key, FSecureAPI.activate_key][blocked == 0]

        services = con.call('getUsboxServices', {'vg_id': vg_id, 'cat_uuid': LB.service_prefix(), 'state': 1})
        for service in services:
            try:
                key = LB.get_fsecure_id(con, service)
            except Exception, e:
                if service['not_usbox']:
                    serv_str = 'Service service_id %d' % service['serv_id']
                else:
                    serv_str = 'Usbox service serv_id %d' % service['serv_id']
                con.log_error('ANTIVIRUS: OnVgBlock failed for %s. Skipped. Msg: %s' % (serv_str, str(e.message)))
                continue

            # execute
            f(con, key, False)

    @log
    def on_stop_usbox_service(self, msg, arg, con):
        serv_id = arg['serv_id']
        try:
            service = LB.get_usbox_service(con, serv_id)
            if LB.is_antivirus_service(con, service):
                key = LB.get_fsecure_id(con, service)
                FSecureAPI.deactivate_key(con, key, False)
        except Exception, e:
            con.log_error(
                'ANTIVIRUS: OnStopUsboxService failed for serv_id %d. Message: %s' % (serv_id, str(e.message))
            )

    @log
    def on_stop_service(self, msg, arg, con):
        service_id = arg['service_id']
        try:
            service = LB.get_service(con, service_id)
            if LB.is_antivirus_service(con, service):
                key = LB.get_fsecure_id(con, service)
                FSecureAPI.deactivate_key(con, key, False)
        except Exception, e:
            con.log_error(
                'ANTIVIRUS: OnStopService failed for service_id %d. Message: %s' % (service_id, str(e.message))
            )

    @log
    def on_start_usbox_service(self, msg, arg, con):
        serv_id = arg['serv_id']
        try:
            service = LB.get_usbox_service(con, serv_id)
            if LB.is_antivirus_service(con, service):
                key = LB.get_fsecure_id(con, service)
                FSecureAPI.activate_key(con, key, False)
        except Exception, e:
            con.log_error(
                'ANTIVIRUS: OnStartUsboxService failed for serv_id %d. Message: %s' % (serv_id, str(e.message))
            )

    @log
    def on_start_service(self, msg, arg, con):
        service_id = arg['service_id']
        try:
            service = LB.get_service(con, service_id)
            if LB.is_antivirus_service(con, service):
                key = LB.get_fsecure_id(con, service)
                FSecureAPI.activate_key(con, key, False)
        except Exception, e:
            con.log_error(
                'ANTIVIRUS: OnStartService failed for service_id %d. Message: %s' % (service_id, str(e.message))
            )

    @log
    def on_del_usbox_service(self, msg, arg, con):
        serv_id = arg['serv_id']
        try:
            usbox_service = LB.get_usbox_service(con, serv_id)
            if LB.is_antivirus_service(con, usbox_service):
                key = LB.get_fsecure_id(con, usbox_service)
                FSecureAPI.deactivate_key(con, key, True)
        except Exception, e:
            con.log_error(
                'ANTIVIRUS: OnDelUsboxService failed for serv_id %d. Message: %s' % (serv_id, str(e.message))
            )
            raise lbcore.Error('Cant deactivate key for usbox service "$1". See logs for details.', [str(serv_id)])

    @log
    def on_del_service(self, msg, arg, con):
        service_id = arg['service_id']
        try:
            service = LB.get_service(con, service_id)
            if LB.is_antivirus_service(con, service):
                key = LB.get_fsecure_id(con, service)
                FSecureAPI.deactivate_key(con, key, True)
        except Exception, e:
            con.log_error(
                'ANTIVIRUS: OnDelService failed for service_id %d. Message: %s' % (service_id, str(e.message))
            )
            raise lbcore.Error('Cant deactivate key for service "$1". See logs for details.', [str(service_id)])

    def activator(self, con):
        # wait for CheckOldServices
        if not hasattr(APIAntivirus.check_old_services, 'checked'):
            return

        if not hasattr(self, 'time') or total_seconds(datetime.datetime.now() - self.time) >= CHECK_ACTIVATED_TIMEOUT:
            services = con.call('getUsboxServices', {'cat_uuid': LB.service_prefix(), 'state': 1})
            notify_period = LB.get_notify_period(con)

            for service in services:
                if not LB.is_activated_service(con, service):
                    try:
                        serv_id = service['serv_id']
                        key = FSecureAPI.get_key(con, service)
                        if key['activationDate'] is not None:
                            activated = key['activationDate'].strftime(DATETIME_FORMAT_WS)
                            if service['not_usbox']:
                                self.activate_service({'service_id': serv_id, 'activated': activated}, con)
                            else:
                                self.activate_usbox_service({'serv_id': serv_id, 'activated': activated}, con)
                        else:
                            timeto = datetime.datetime.strptime(service['time_to'], DATETIME_FORMAT)
                            seconds_left = total_seconds(timeto - datetime.datetime.now())
                            if 0 < seconds_left < notify_period * 86400:
                                con.msg('antivirus_activation_expire', {'service': service})
                    except Exception, e:
                        con.log_error(
                            'ANTIVIRUS: Activator fail for service %s. Message: %s' % (service, str(e.message))
                        )
                        continue
            self.time = datetime.datetime.now()

    def blocker(self, con):

        def do_queue(queue):
            errors_ = []
            while queue.qsize():
                item_ = queue.get()
                f = item_['operation']
                key = item_['key']
                try:
                    if f in (FSecureAPI.activate_key, FSecureAPI.deactivate_key):
                        f(con, key, True)
                except Exception, e:
                    con.log_warning(
                        'ANTIVIRUS: Cant execute %s for key %s. The operation will try later. Message: %s'
                        % (f.__name__, key, str(e.message))
                    )
                    errors_.append(item_)
            return errors_

        # block_queue
        errors = do_queue(FSecureAPI.block_queue)

        # block_queue_error
        if not hasattr(self, 'time_err_check') \
                or total_seconds(datetime.datetime.now() - self.time_err_check) >= TIMEOUT_ON_ERROR:
            errors += do_queue(FSecureAPI.block_queue_err)
            self.time_err_check = datetime.datetime.now()

        # store items with error
        for item in errors:
            FSecureAPI.block_queue_err.put(item)

    @staticmethod
    @lbcore_messenger('on_startup', 'startup')
    def on_startup_antivirus(msg, agr, con):
        try:
            global module
            if not load_settings(con):
                return
            FSecureAPI.init()
            module.api = APIAntivirus()
            module.con = con
            con.log_info('Starting Antivirus module. VERSION %s' % VERSION)
        except Exception, e:
            con.log_error('Antivirus module failed. %s' % str(e.message))

    @staticmethod
    @lbcore_messenger('check_old_services', 'startup')
    def check_old_services(msg, arg, con):
        # change external_data format for old services
        services = con.call('getUsboxServices', {'cat_uuid': LB.service_prefix()})
        for service in services:
            external_data = service['external_data'].split('#')
            if len(external_data) == 2:
                key, url = external_data
                external_data = {
                    'key': key,
                    'url': url,
                }
                if service['not_usbox']:
                    LB.update_service(con, service['serv_id'], external_data=json.dumps(external_data))
                else:
                    LB.update_usbox_service(con, service['serv_id'], external_data=json.dumps(external_data))

        # add fsecure_id (in old style) to external_data
        billing_identity = LB.get_identity(con)
        services = con.call('getUsboxServices', {'cat_uuid': LB.service_prefix()})
        for service in services:
            serv_id = service['serv_id']
            try:
                external_data = json.loads(service['external_data'])
            except Exception, e:
                if service['not_usbox']:
                    serv_str = 'Service service_id %s' % service['serv_id']
                else:
                    serv_str = 'Usbox service serv_id %s' % service['serv_id']
                con.log_warning('ANTIVIRUS: Cant set id for %s. Skipped. Msg %s' % (serv_str, str(e.message)))
                continue
            if 'id' not in external_data:
                if service['not_usbox']:
                    uniq_id = '%s_service_id_%d' % (billing_identity, serv_id)
                else:
                    uniq_id = '%s_%d' % (billing_identity, serv_id)

                external_data['id'] = uniq_id
                if service['not_usbox']:
                    LB.update_service(con, serv_id, external_data=json.dumps(external_data))
                else:
                    LB.update_usbox_service(con, serv_id, external_data=json.dumps(external_data))

        # done
        APIAntivirus.check_old_services.checked = True

    @staticmethod
    @lbcore_fuctor('check_antivir_module', 'checkAntivirusService')
    def check_antivirus_module(arg, con):
        global module
        return hasattr(module, 'api')

    @staticmethod
    @lbcore_fuctor('antivirus_prefix', 'getAntivirusServicePrefix')
    def antivirus_service_prefix(arg, con):
        return LB.service_prefix()
