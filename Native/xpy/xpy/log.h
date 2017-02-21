/* Created on 2017.2.20
 *
 * @author: jia
 */

#pragma once
#include "xdef.h"
#include "fmt/format.h"
#include <string>
typedef void (*DebugCallback)(int level, const char *str);

extern "C" {
    EXPORT_API void RegisterDebugCallback(DebugCallback callback);
}


namespace xpy
{
    void xlog(int level, const char* message);

    class logger
    {
    public:
        template<typename... Args>
        static void debug(const char *format, Args... args)
        {
            std::string s = fmt::format(format, args...);
            xlog(1, s.c_str());
        }

        template<typename... Args>
        static void info(const char *format, Args... args)
        {
            std::string s = fmt::format(format, args...);
            xlog(2, s.c_str());
        }

        template<typename... Args>
        static void error(const char *format, Args... args)
        {
            std::string s = fmt::format(format, args...);
            xlog(3, s.c_str());
        }

        template<typename... Args>
        static void warnning(const char *format, Args... args)
        {
            std::string s = fmt::format(format, args...);
            xlog(4, s.c_str());
        }
    };
}
