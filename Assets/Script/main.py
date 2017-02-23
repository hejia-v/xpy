# -*- coding:utf-8 -*-

import xpy

g_csfunc = None

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
    callback(74, 'ag', (2, 'pt'), '8')
    return True
