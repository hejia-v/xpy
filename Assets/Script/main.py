# -*- coding:utf-8 -*-

import xpy

g_csfunc = None


def test_1(*arg):
    print(arg)
    return test_2


def test_2(a):
    print('test_2 [a: %s]' % a)
    return a


def test_3(*arg):
    print(arg)
    return test_4


def test_4(a, b):
    print('test_4 [a: %s], [b: %s]' % (a, b))
    return a, b


def init(c):
    global g_csfunc
    g_csfunc = c


def callback(*arg):
    ret = xpy.csharpcall(g_csfunc, *arg)
    print("Return:" , ret)


def main():
    import log
    log.init()  # 初始化输出流
    print('Python script start!')
    print(dir(xpy))
    # callback(74, 'ag', (2, 'pt'), '8')
    # callback(None)
    callback(test_1)
    callback(None, True, False, 124, 0xffffffff + 1234, 34.5, 'hello 哈哈')
    return True
