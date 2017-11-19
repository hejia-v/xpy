# -*- coding:utf-8 -*-

import xpy


def main():
    import log
    import sys
    import os
    import globalvars
    log.init()  # 初始化输出流
    print('Python script start!')

    # 开启 PyCharm 远程调试，需要编译_socket, select
    if globalvars.DEBUG:
        pydevd_path = os.path.dirname(os.path.abspath(__file__))
        pydevd_path = os.path.normpath(f'{pydevd_path}/libs/pycharm-debug-py3k.egg')
        sys.path.append(pydevd_path)
        import pydevd
        pydevd.settrace(globalvars.DEBUG_ADDRESS, port=globalvars.DEBUG_PORT, stdoutToServer=True, stderrToServer=True)

    print(dir(xpy))
    import test_import1
    test_import1.test()
    return True
