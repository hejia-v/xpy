# -*- coding: utf8 -*-
#-------------------------------------------------------------------------------
# @Author      : hejia
# @Created     : 2015/1/13
# @Desc        : log模块
#-------------------------------------------------------------------------------

import sys
import emb


class OutPut:
    def __init__(self):
        self._buffer = 'python: '

    def write(self, s):
        s = str(s)
        self._buffer += s

        if s == "\n":
            emb.writelog(1, self._buffer)
            self._buffer = ''
            self._buffer = 'python: '

    def writeline(self, sl):
        map(self.write, sl)

    def flush(self):
        pass


class ErrOutPut:
    def __init__(self):
        self._buffer = ''

    def write(self, s):
        # TODO: 待优化
        s = str(s)
        self._buffer += s

        if s == "\n":
            emb.writelog(4, self._buffer)
            self._buffer = ''

    def writeline(self, sl):
        map(self.write, sl)


def init():
    # python输出重定向
    sys.stdout = OutPut()
    sys.stderr = ErrOutPut()
