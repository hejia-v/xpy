#pragma once

#ifdef _WINDOWS
# define EXPORT_API __declspec (dllexport)
#else
# define EXPORT_API 
#endif
