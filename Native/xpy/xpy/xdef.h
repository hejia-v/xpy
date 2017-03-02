#pragma once
#include <stdint.h>

#ifdef _WINDOWS
# define EXPORT_API __declspec (dllexport)
#else
# define EXPORT_API
#endif

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
    void *ptr;
};

struct string_pusher
{
    int done;
};

typedef int(*csharp_callback)(int argc, var *argv, string_pusher *sp);

#define error_nc(err, msg)                   \
    {                                        \
        const char *_s = msg;                \
        char *_e = new char[strlen(_s) + 1]; \
        strcpy(_e, _s);                      \
        err = _e;                            \
    }
