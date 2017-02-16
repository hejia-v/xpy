#pragma once
#include "xdef.h"

extern "C" {
	EXPORT_API char* native_get_current_path();
	void EXPORT_API native_release_memory(void *pBuffer);
}