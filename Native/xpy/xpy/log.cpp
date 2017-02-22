#include "log.h"
#include <iostream>
using namespace std;


DebugCallback g_DebugCallback;

void RegisterDebugCallback(DebugCallback callback)
{
    if (callback)
    {
        g_DebugCallback = callback;
    }
}

void xpy::xlog(int level, const char* message, bool newline)
{
    if (g_DebugCallback)
    {
        g_DebugCallback(level, message);
    }
    else
    {
        cout << message;
        if (newline)
        {
            cout << "\n";
        }
    }
}
