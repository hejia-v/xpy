# -*- coding:utf-8 -*-
import logging

logger = logging.getLogger('table')


def to_int(value):
    if value:
        return int(value)
    else:
        return "0"


def to_float(value):
    if value:
        return value
    else:
        return "0.0"


def to_bool(value):
    if str(value).lower() == "true":
        return "True"
    if str(value).lower() == "false":
        return "False"
    if value:
        return "True"
    else:
        return "False"


def to_string(value):
    if value:
        return "'" + value + "'"
    else:
        return "''"


def to_tuple(value):
    if not value:
        return '()'
    try:
        value = eval(value)
    except:
        logger.exception('读取元组数据类型出错:%s', value)
        raise
    return value


def to_object(value):
    if value:
        return value
    else:
        return "None"


def convert_value(stype, value):
    funcMap = {
        'i': to_int,
        'f': to_float,
        'b': to_bool,
        's': to_string,
        't': to_tuple,
        'o': to_object,
    }

    func = funcMap.get(stype)
    if not func:
        logger.error('未定义的数据类型：%s', stype)
        raise
    return func(value)
