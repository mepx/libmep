
#include "utils_mep.h"
#include <cstring>

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
	if (s1)
		if (s2)
			return strcmp(s1, s2);
		else
			return 1;
	else
		if (s2)
			return 1;
	return 0;// both NULL
}
//---------------------------------------------------------------------------