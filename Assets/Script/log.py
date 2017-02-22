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
        self.__curLineWrtCnt = 0

    def write(self, s):
        if str(s) == "\n":
            self.__curLineWrtCnt = -1
        if self.__curLineWrtCnt == 0:
            emb.writelog(1, "python:  ")
        self.__curLineWrtCnt += 1
        # TODO: 前缀、消息和换行符合成一个字符串，再发到unity的console
        emb.writelog(1, str(s))

    def writeline(self, sl):
        map(self.write, sl)

    def flush(self):
        pass


class ErrOutPut:
    def write(self, s):
        emb.writelog(5, str(s))

    def writeline(self, sl):
        map(self.write, sl)


def init():
    # python输出重定向
    sys.stdout = OutPut()
    sys.stderr = ErrOutPut()
