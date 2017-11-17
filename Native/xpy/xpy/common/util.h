#pragma once

#include <string>

// wchar2char 在 android 上的表现需要测试, 实际需要使用时，再进行测试
//inline char *wchar2char(const wchar_t *wstr)
//{
//	//size_t len = wcstombs(NULL, wstr, 0);
//
//	size_t len = wcslen(wstr) + 1;
//	len = sizeof(wchar_t) * len;
//
//	char *str = new char[len];
//	memset(str, 0, sizeof(char) * len);
//	size_t size = wcstombs(str, wstr, len);  // android 上测试一下这个
//	return str;
//}

/**
* char2wchar在无中文的情况下是没问题的, 有中文的情况需要测试
*/
inline wchar_t *char2wchar(const char *str)
{
    size_t len = strlen(str) + 1;
    wchar_t *wstr = new wchar_t[len];
    memset(wstr, 0, sizeof(wchar_t) * len);
    size_t size = mbstowcs(wstr, str, sizeof(wchar_t) * len);
    return wstr;
}

#define error_nc(err, msg)                   \
    {                                        \
        const char *_s = msg;                \
        char *_e = new char[strlen(_s) + 1]; \
        strcpy(_e, _s);                      \
        err = _e;                            \
    }
