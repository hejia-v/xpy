# -*- coding: utf8 -*-
# Created on 2017.2.22
# @author: jia
# @Desc: sharpcall

import sys

class objcache(object):
    obj_id = 0
    obj_cache = {}
    obj_ref = {}

    @staticmethod
    def fetch(obj):  # 获取Python对象或csharp对象的代理id
        # TODO:同一个obj多次调用 ?
        # print('---------------- objcache.fetch, obj:', obj)
        obj_id = objcache.obj_id
        obj_cache = objcache.obj_cache
        obj_ref = objcache.obj_ref

        if obj_id > 1000000:  # max number of cache
            obj_id = len(obj_cache)

        obj_id += 1
        obj_cache[obj] = obj_id
        obj_cache[obj_id] = obj
        obj_ref[obj_id] = obj	# strong reference
        objcache.obj_id = obj_id
        return obj_id


class sharpcache(object):
    sharp_set = {}
    sharp_cache = {}

    @staticmethod
    def fetch(_id):
        proxy_obj = sharpobj(_id)
        sharpcache.sharp_cache[_id] = proxy_obj
        sharpcache.sharp_set[_id] = True
        # print('---------------- objcache.sharpcache, id:', _id, 'obj:', proxy_obj, 'refcnt:', sys.getrefcount(proxy_obj))
        return proxy_obj


class sharpobj(object):
    # todo: sharp object
    def __init__(self, _id):
        self.id = _id


class sharp(object):
    @staticmethod
    def unref():
        if not objcache.obj_ref:
            objcache.obj_ref = {}  # clear strong reference, temp object in python would be collect later.

    @staticmethod
    def _proxy(obj):
        if isinstance(obj, sharpobj):
            return 'S', obj.id  # sharp object
        else:
            return 'P', objcache.fetch(obj)  # python object

    @staticmethod
    def _object(_type, _id):
        if _type == 'P':
            # python object
            return objcache.obj_cache.get(_id)
        else:
            # type == 'S'
            return sharpcache.fetch(_id)

    @staticmethod
    def _garbage():
        for k in sharpcache.sharp_set:
            if not sharpcache.sharp_cache.get(k):
                del sharpcache.sharp_set[k]
                return k
        return None
