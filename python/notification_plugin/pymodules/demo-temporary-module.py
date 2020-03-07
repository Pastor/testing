#!/usr/bin/python
# -*- coding: utf-8 -*-

import datetime
import random

import lbcore

module = lbcore.Module()
module.SetName("ra-stat-module2", 1)
module.SetTitle("RA add-statistic Module2")


def py_functor_impl(arg, con):
    # table_type, agent_id, IP, vg_id, uid, agrm_id, tar_id, cat_idx
    table_type = arg[0]
    nnn = "00" + str(arg[1])
    # table_name = "rad"+ str(nnn) + str(time.strftime('%Y%m%d'))
    sql_query = 'create table IF NOT EXISTS ' + table_type + nnn + str(
        (datetime.datetime.today() - datetime.timedelta(days=1)).strftime('%Y%m%d')) + ' like userNNNYYYYmmdd'
    con.query(sql_query)
    ip = str(arg[2])
    print ip
    for i in range(0, 5):
        remote = str(random.randint(10, 126)) + '.' + str(random.randint(10, 126)) + '.' + str(
            random.randint(200, 250)) + '.' + str(random.randint(1, 254))
        cin = str(random.randint(512, 10485760))
        cout = str(random.randint(512, 10485760))
        timefrom = str((datetime.datetime.today() - datetime.timedelta(days=1)).strftime('%Y-%m-%d %H:%M:')) + str(
            random.randint(10, 17))
        timeto = str((datetime.datetime.today() - datetime.timedelta(days=1)).strftime('%Y-%m-%d %H:%M:')) + str(
            random.randint(35, 59))
        vg_id = str(arg[3])
        # c_date = str(time.strftime('%Y-%m-%d'))
        uid = str(arg[4])
        agrm_id = str(arg[5])
        tar_id = str(arg[6])
        cat_idx = str(arg[7])
        traff_type = str(random.randint(1, 2))
        oper_id = "1"
        need_calc = "0"
        sql_query = 'insert ' + table_type + nnn + \
                    str((datetime.datetime.today() - datetime.timedelta(days=1)).strftime('%Y%m%d')) + \
                    ' (ip, remote, cin, cout, timefrom, timeto, vg_id, uid, agrm_id, tar_id, cat_idx, ' + \
                    '  traff_type, oper_id, need_calc) ' + \
                    ' values(' + "inet_aton('" + ip + "'),inet_aton('" + remote + "'),'" + cin + "','" + \
                    cout + "','" + timefrom + "','" + timeto + "','" + vg_id + "','" + uid + "','" + \
                    agrm_id + "','" + tar_id + "','" + cat_idx + "','" + traff_type + "','" + \
                    oper_id + "','" + need_calc + "');"
        con.query(sql_query)
    return [None, True, False, 0, 0.0, "", arg]


f = lbcore.Functor(py_functor_impl, "MYpyStatFunctor2")
