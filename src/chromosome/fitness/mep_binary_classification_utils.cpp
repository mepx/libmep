// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <string.h>
//---------------------------------------------------------------------------
#include "mep_binary_classification_utils.h"
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
