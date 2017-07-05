using System;
using System.Collections.Generic;

class SharpObject
{
    object[] cache = new object[1024];  // default pool size 1024
    Dictionary<object, int> map = new Dictionary<object, int>();
    int freelist = 0;
    int cap = 0;

    int AddKey(object obj)
    {
        int id;
        if (freelist == 0)
        {
            // full
            id = cap;
            ++cap;
            if (cap > cache.Length)
            {
                Array.Resize(ref cache, cap * 2);
            }
        }
        else
        {
            id = freelist;
            freelist = (int)cache[freelist];    // next free node
        }
        cache[id] = obj;
        ++id;
        map.Add(obj, id);
        return id;
    }

    public int Query(object obj)
    {
        int id;
        if (map.TryGetValue(obj, out id))
        {
            return id;
        }
        return AddKey(obj);
    }

    public void Remove(int id)
    {
        object obj = cache[id - 1];
        int mapid;
        if (map.TryGetValue(obj, out mapid) && mapid == id)  
        {
            // exist in cache
            --id;
            map.Remove(obj);
            // freelist存放的是上一个被释放的对象的位置, 方便下次插入新对象时使用该位置
            cache[id] = freelist;
            freelist = id;
        }
    }

    public object Get(int id)
    {
        object obj = cache[id - 1];
        if (obj != null)
        {
            if (obj.GetType().IsClass)
            {
                return obj;
            }
        }
        return null;
    }
};
