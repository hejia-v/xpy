# -*- coding:utf-8 -*-

def test_print():
    import xpy
    print("Number of arguments", xpy.numargs())
    xpy.writelog(1, 'log test\n')
    print("hi print!")
    print(1)
    print([3, 'ac', 7.8])
    print('uuuyyyy')
    a = 9 / 0
