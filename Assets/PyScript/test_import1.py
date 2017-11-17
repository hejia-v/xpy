# -*- coding:utf-8 -*-

"""Use custom meta hook to import modules available as strings.
Cp. PEP 302 http://www.python.org/dev/peps/pep-0302/#specification-part-2-registering-hooks"""
import sys
import imp

modules = {"aa" :
"""def hello():
    return 'Hello World A!'""",
"bb":
"""def hello():
    return 'Hello World B!'"""}

class StringImporter(object):

    def __init__(self, modules):
       self._modules = dict(modules)


    def find_module(self, fullname, path):
        print('11111111', fullname,path)
        if fullname in self._modules.keys():
            print('00000000000')
            return self
        return None

    def load_module(self, fullname):
        print('2222222',fullname)
        if not fullname in self._modules.keys():
            raise ImportError(fullname)

        new_module = imp.new_module(fullname)
        exec(self._modules[fullname], new_module.__dict__)
        print('--------------------')
        print(self._modules[fullname])
        print(dir(new_module))
        return new_module

# 加入pycharm 远程调试
def test():
    sys.meta_path.append(StringImporter(modules))

    # Let's use our import hook
    from aa import hello
    print(hello())
    from bb import hello
    print(hello())

# 本模块最好写到c

# 1. 脚本加密 2. 脚本文件加载 3.pycharm调试支持 4. c# wrapper 5. android 编译 6. windows开发测试 7.单元测试 8. gc 相关的处理 9. 性能优化