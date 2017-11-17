/* Created on 2017.3.7
 *
 * @author: jia
 */

#pragma once
#include <assert.h>
#include <utility>

template <typename T>
class SingleTon
{
public:
    template <typename... Args>
    static T *instance(Args &&... args)
    {
        return new T(std::forward<Args>(args)...);
    }

    static T *getInstance()
    {
        return s_pInstance;
    }

    static void destroyInstance()
    {
        delete s_pInstance;
        s_pInstance = nullptr;
    }

protected:
    // 禁止直接创建SingleTon对象
    SingleTon()
    {
        assert(!s_pInstance && "the SingleTon object repeat construct");
// #if defined(_MSC_VER) && _MSC_VER < 1200
//         int offset = (int)(T *)1 - (int)(SingleTon<T> *)(T *)1; // 计算派生类偏移
//         s_pInstance = (T *)((int)this + offset);
// #else
        s_pInstance = static_cast<T *>(this);
// #endif
    }

    virtual ~SingleTon()
    {
        assert(s_pInstance && "the SingleTon object repeat destruct");
        s_pInstance = 0;
    }

private:
    SingleTon(const SingleTon &source) {}  // 禁止拷贝构造函数

private:
    static T *s_pInstance;
};

template <typename T>
T *SingleTon<T>::s_pInstance = 0;  //  实例化的详细过程?
