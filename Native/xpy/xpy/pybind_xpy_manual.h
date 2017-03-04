/* Created on 2017.1.29
 *
 * @author: jia
 */

#pragma once
#include "xdef.h"

int init_csharp_python_funcs(csharp_callback cb);
int sharp_collect_garbage(int n, int *result);
const char *get_python_function(const char *module, const char *funcname, int *id);
int call_python_function(int argc, var *argv, int strc, const char **strs, const char **err);

int register_xpy_functions();
