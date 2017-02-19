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
        # fd = open("log.text","a")
        # fd.write(s)
        # fd.close()

        if str(s) == "\n":
            self.__curLineWrtCnt = -1
        if self.__curLineWrtCnt == 0:
            emb.writelog(1, "python:  ")
        self.__curLineWrtCnt += 1

        emb.writelog(1, str(s))

    def writeline(self, sl):
        map(self.write, sl)

    def flush(self):
        pass

class ErrOutPut:
    def write(self, s):
        # fd = open("log.text","a")
        # fd.write(s)
        # fd.close()
        emb.writelog(5, str(s))

    def writeline(self, sl):
        map(self.write, sl)


def init():
    # 清空log
    # fd = open("log.text","w")
    # fd.write("")
    # fd.close()

    # python输出重定向
    sys.stdout = OutPut()
    sys.stderr = ErrOutPut()
