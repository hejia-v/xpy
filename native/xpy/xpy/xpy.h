#pragma once
#include "xdef.h"

extern "C" {
	EXPORT_API const char* native_get_current_path();
	EXPORT_API void native_release_memory(void *pBuffer);
	EXPORT_API void python_start(const char* program, const char* home);
	EXPORT_API bool python_check_interpreter(const char *program);
	EXPORT_API bool python_finalize();
}