/* Created on 2017.3.30
 *
 * @author: jia
 */

#pragma once
#include <stdint.h>
#include "xdefine.h"

#define SHARPPY_MAJOR_VERSION 1
#define SHARPPY_MINOR_VERSION 0
#define SHARPPY_PATCH_VERSION 2

#define SHARPPY_VERSION "SHARPPY_MAJOR_VERSION.SHARPPY_MINOR_VERSION.SHARPPY_PATCH_VERSION"

enum class var_type
{
    NONE = 0,
    INTEGER = 1,
    INT64 = 2,
    REAL = 3,
    BOOLEAN = 4,
    STRING = 5,
    POINTER = 6,
    PYTHONOBJ = 7,
    SHARPOBJ = 8,
};

struct var
{
    var_type type;
    int d;
    int64_t d64;
    double f;
    void *str;  // 与ptr区分开, 字符串传到对方后，对方要立即自己存储起来，然后释放本字符串
    void *ptr;
};

struct string_pusher
{
    int done;
};

typedef int(*csharp_callback)(int argc, var *argv, string_pusher *sp);

extern "C" {
    /**
    * 释放*pBuffer处的内存, 并将*pBuffer置为nullptr
    *
    * @param pBuffer - 二级指针, 方便在释放内存后将地址设置为nullptr
    * @return void
    */
    EXPORT_API void Native_ReleaseMemory(void **pBuffer);
    /** 获取当前路径 */
    EXPORT_API const char *Native_GetCurrentPath();
    EXPORT_API void Python_RegisterModule();
    EXPORT_API bool Python_Start(const char *program, const char *home);
    EXPORT_API bool Python_CheckInterpreter(const char *program);
    EXPORT_API bool Python_Finalize();
    EXPORT_API int Python_InitScript(const char *scriptroot);
    /** 注册sharp的回调 */
    EXPORT_API int Python_InitSharpCall(csharp_callback cb);
    EXPORT_API int Python_RunString(const char *script);
    /**
    * 获取Python函数的代理id. 获取到Python函数后, 调用sharp._proxy获取代理id, 同时将该Python函数缓存起来
    */
    EXPORT_API int Python_GetFunction(const char *module, const char *funcname, int *id, const char **err);
    EXPORT_API int Python_CreateClassInstance(const char *module, const char *classname, int *id, int argc, var *argv, int strc, const char **strs, const char **err);
    /**
    * c#调用Python函数, c层会解析c#传过来的参数,
    * 参数中如果是Python对象的代理id, 则通过sharp._object从缓存中取出Python对象,
    * 参数中如果是c#对象, 在c#层先获取代理id并缓存c#对象, 通过sharp._object获取c#对象的代理对象，并将该代理对象通过SharpCache引用起来
    * 执行Python函数得到结果后, 如果返回值中有c#或者Python对象, 则通过sharp._proxy获取到对应的代理id, 并传递到c#层
    */
    EXPORT_API int Python_CallFunction(int argc, var *argv, int strc, const char **strs, const char **err);
    EXPORT_API int Python_CallInstanceMethod(const char *method, int argc, var *argv, int strc, const char **strs, const char **err);
    EXPORT_API int Python_GetInstanceMethod(const char *method, var *argv, const char **err);
    EXPORT_API int Python_SharpCollectGarbage(int n, int *result);
    EXPORT_API int Python_RunFunction(const char *pythonfile, const char *funcname, const char *args);
}
