#pragma once

#include <string>


inline char *wchar2char(const wchar_t *wstr)
{
    size_t len = wcslen(wstr) + 1;
    size_t converted = 0;
    char *str = new char[len];
    memset(str, 0, sizeof(char) * len);
    wcstombs_s(&converted, str, len, wstr, _TRUNCATE);
    return str;
}

inline wchar_t *char2wchar(const char *str)
{
    size_t len = strlen(str) + 1;
    size_t converted = 0;
    wchar_t *wstr = new wchar_t[len];
    memset(wstr, 0, sizeof(wchar_t) * len);
    mbstowcs_s(&converted, wstr, len, str, _TRUNCATE);
    return wstr;
}

#define error_nc(err, msg)                   \
    {                                        \
        const char *_s = msg;                \
        char *_e = new char[strlen(_s) + 1]; \
        strcpy(_e, _s);                      \
        err = _e;                            \
    }
