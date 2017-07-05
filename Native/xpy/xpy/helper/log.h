/* Created on 2017.2.20
 *
 * @author: jia
 */

#pragma once
#include "fmt/format.h"
#include "xdefine.h"
#include <string>

typedef void (*LogHandler)(int level, const char *str);

void add_log_handler(LogHandler handler);
void del_log_handler(LogHandler handler);
void reset_log_handler();

namespace xpy
{
    void xlog(int level, const char* message, bool newline = true);

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
        static void warnning(const char *format, Args... args)
        {
            std::string s = fmt::format(format, args...);
            xlog(3, s.c_str());
        }

        template<typename... Args>
        static void error(const char *format, Args... args)
        {
            std::string s = fmt::format(format, args...);
            xlog(4, s.c_str());
        }
    };
}

typedef void (*DebugCallback)(int level, const char *str);

extern "C" {
    EXPORT_API void RegisterDebugCallback(DebugCallback callback);
}
