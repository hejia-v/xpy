#include "log.h"
#include <list>
#include <algorithm>
#include <iostream>
#ifdef WIN32
#include "windows.h"
#endif
using namespace std;

void default_log_handler(int level, const char *msg);
std::list<LogHandler> g_LogHandlers = {default_log_handler}; // consider thread safety

void add_log_handler(LogHandler handler)
{
    if (!handler)
        return;

    for (LogHandler hdr : g_LogHandlers)
    {
        if (hdr == handler)
            return;
    }
    g_LogHandlers.push_back(handler);
}

void del_log_handler(LogHandler handler)
{
    if (!handler)
        return;
    g_LogHandlers.erase(remove_if(g_LogHandlers.begin(), g_LogHandlers.end(), [&handler](LogHandler &hdr) { return hdr == handler; }), g_LogHandlers.end());
}

void reset_log_handler()
{
	g_LogHandlers = {default_log_handler};
}

void xpy::xlog(int level, const char *message, bool newline)
{
    for (LogHandler handler : g_LogHandlers)
    {
        if (newline)
        {
            handler(level, (std::string(message) + "\n").c_str());
        }
        else
        {
            handler(level, message);
        }
    }
}

void default_log_handler(int level, const char *msg)
{
    cout << msg;
}
// TODO: remote console in another thread socket handler. support command

#ifdef WIN32
static HWND s_hConsole = NULL;
#endif

void RegisterDebugCallback(DebugCallback callback)
{
#ifdef WIN32
    // s_hConsole = GetConsoleWindow();
    // if (s_hConsole == NULL)
    // {
    //     AllocConsole();
    //     s_hConsole = GetConsoleWindow();
    // }

    // if (s_hConsole != NULL)
    // {
    //     freopen("CONIN$", "r", stdin);
    //     freopen("CONOUT$", "w", stdout);
    //     freopen("CONOUT$", "w", stderr);
    // }
#endif
    add_log_handler(callback);
}
