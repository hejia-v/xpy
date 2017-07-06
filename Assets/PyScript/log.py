# -*- coding: utf8 -*-
# Created on 2015.1.13
# @author: hejia
# @desc: log模块

import sys
import xpy


class OutPutEx:
    def __init__(self):
        self._buffer = 'python: '

    def write(self, s):
        s = str(s)
        self._buffer += s

        if s == "\n":
            xpy.writelog(1, self._buffer)
            self._buffer = ''
            self._buffer = 'python: '

    def writeline(self, sl):
        map(self.write, sl)

    def flush(self):  # TODO:
        pass


class ErrOutPutEx:
    def __init__(self):
        self._buffer = ''

    def write(self, s):
        # TODO: 待优化
        s = str(s)
        self._buffer += s

        if s == "\n":
            xpy.writelog(4, self._buffer)
            self._buffer = ''

    def writeline(self, sl):
        map(self.write, sl)

    def flush(self):  # TODO:
        pass


def init():
    # python输出重定向
    sys.stdout = OutPutEx()
    sys.stderr = ErrOutPutEx()
