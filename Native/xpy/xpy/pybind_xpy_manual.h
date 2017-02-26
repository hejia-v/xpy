/* Created on 2017.1.29
 *
 * @author: jia
 */

#pragma once
#include "xdef.h"

int init_csharp_python_funcs(csharp_callback cb);
const char *get_python_function(const char *module, const char *funcname, int *id);

int register_xpy_functions();
