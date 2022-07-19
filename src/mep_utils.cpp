// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------

#include "mep_utils.h"
#include <string.h>

//---------------------------------------------------------------------------
int sort_function_value_class(const void *a, const void *b)
{
	if (((s_value_class *)a)->value < ((s_value_class *)b)->value)
		return -1;
	else
		if (((s_value_class *)a)->value > ((s_value_class *)b)->value)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int my_strcmp(char *s1, char *s2)
{
	int result;
	if (s1)
		if (s2)
			result = strcmp(s1, s2);
		else
			result = 1;
	else
		if (s2)
			result = 1;
		else
			result = 0;// both NULL

	return result;
}
//---------------------------------------------------------------------------
int compare_int (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
//---------------------------------------------------------------------------

void increase_string_capacity(char*& s_prg, size_t& capacity, size_t actual_length, size_t extra_length)
{
	if (actual_length + extra_length >= capacity) {
		capacity += extra_length + 100;
		char* s = new char[capacity];
		strcpy(s, s_prg);
		delete[] s_prg;
		s_prg = s;
	}
}
//---------------------------------------------------------------------------
void increase_string_capacity2(char*& s_prg, size_t& capacity, size_t extra_length)
{
	capacity += extra_length + 2;
	char* s = new char[capacity];
	strcpy(s, s_prg);
	delete[] s_prg;
	s_prg = s;
}
//---------------------------------------------------------------------------
