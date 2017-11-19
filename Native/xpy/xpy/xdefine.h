/* Created on 2017.3.30
 *
 * @author: jia
 */
#pragma once

#ifdef _WINDOWS
# define EXPORT_API __declspec (dllexport)
#else
# define EXPORT_API
#endif

#define NAMESPACE_XPY_BEGIN namespace xpy \
                            {

#define NAMESPACE_XPY_END }
