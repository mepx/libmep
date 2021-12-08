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