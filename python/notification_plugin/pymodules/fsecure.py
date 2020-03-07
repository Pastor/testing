#!/usr/bin/python

from suds.client import Client
from suds.sax.element import Element
from suds.wsse import Security, UsernameToken


OrderingClient = None
SubscriptionManagementClient = None


def create_client(username, password, operator_id, wsdl):
    client = Client(wsdl)
    security = Security()
    security.tokens.append(UsernameToken(username, password))
    operator = Element('ns0:operatorId').setText(operator_id)
    client.set_options(wsse=security)
    client.set_options(soapheaders=operator)
    return client


def init(username, password, operator_id, ordering_wsdl, subscription_management_wsdl):
    global OrderingClient
    global SubscriptionManagementClient
    OrderingClient = create_client(username, password, operator_id, ordering_wsdl)
    SubscriptionManagementClient = create_client(username, password, operator_id, subscription_management_wsdl)


def add_key(serv_id, product):
    subscription = OrderingClient.factory.create("Subscribe")
    subscription['externalReference'] = serv_id
    subscription['amount'] = 3
    subscription['type'] = 'continuous'
    subscription['product'] = product
    result = OrderingClient.service.customerOrder(serv_id, subscribe=subscription)
    key = {'key': str(result.installerKey)}
    for i in result.installers:
        if 'Windows' in i.operatingSystem:
            key['url'] = str(i.downloadUrl)
    return key


def activate_key(serv_id):
    subscription = SubscriptionManagementClient.factory.create("Identity")
    subscription['externalReference'] = serv_id
    modify = SubscriptionManagementClient.factory.create("SubscriptionContent")
    modify['status'] = 'enabled'
    SubscriptionManagementClient.service.subscriptionModification(subscription, modify)


def deactivate_key(serv_id):
    subscription = SubscriptionManagementClient.factory.create("Identity")
    subscription['externalReference'] = serv_id
    modify = SubscriptionManagementClient.factory.create("SubscriptionContent")
    modify['status'] = 'disabled'
    SubscriptionManagementClient.service.subscriptionModification(subscription, modify)


def get_key(serv_id):
    res = {'status': False, 'activationDate': None}
    customer = SubscriptionManagementClient.factory.create("Identity")
    customer['externalReference'] = serv_id
    result = SubscriptionManagementClient.service.subscriptionQuery(customer)
    if 'subscriptionDetail' in result.__dict__:
        res['status'] = True
        used = filter(lambda x: 'firstUse' in x.__dict__, result['subscriptionDetail'])
        if len(used):
            res['activationDate'] = min(used, key=lambda x: x['firstUse'])['firstUse']
    return res


def status():
    return OrderingClient is not None and SubscriptionManagementClient is not None


def echo(wsdl, msg, username=None, password=None, operator_id=None):
    client = Client(wsdl)
    if username is not None or password is not None or operator_id is not None:
        security = Security()
        security.tokens.append(UsernameToken(username, password))
        operator = Element('ns0:operatorId').setText(operator_id)
        client.set_options(wsse=security)
        client.set_options(soapheaders=operator)
    result = client.service.sendEcho(msg)
    return result
