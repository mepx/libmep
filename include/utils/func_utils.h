// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#ifndef func_utils_H
#define func_utils_H
//----------------------------------------------------------------
#include <stddef.h>
//----------------------------------------------------------------
int compare_int (const void * a, const void * b);
int my_strcmp(char *s1, char *s2);
void increase_string_capacity(char*& s_prg, size_t& capacity, size_t actual_length, size_t extra_length);
void increase_string_capacity2(char*& s_prg, size_t& capacity, size_t extra_length);
//----------------------------------------------------------------
#endif
