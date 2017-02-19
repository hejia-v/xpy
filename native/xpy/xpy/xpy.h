/* Created on 2017.1.29
 *
 * @author: jia
 */

#pragma once
#include "xdef.h"

typedef void (*DebugCallback)(const char *str);

extern "C" {
    EXPORT_API void RegisterDebugCallback(DebugCallback callback);
    EXPORT_API const char* Native_GetCurrentPath();
    EXPORT_API void Native_ReleaseMemory(void *pBuffer);
    EXPORT_API void Python_RegisterModule();
    EXPORT_API void Python_Start(const char* program, const char* home);
    EXPORT_API bool Python_CheckInterpreter(const char *program);
    EXPORT_API bool Python_Finalize();
}
