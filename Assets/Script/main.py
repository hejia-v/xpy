# -*- coding:utf-8 -*-

def main():
    import emb
    import log
    print('hello world!')
    print("Number of arguments", emb.numargs())
    emb.writelog(1, 'log test\n')
    log.init()  # 初始化输出流
    print("hi print!")
    return True
