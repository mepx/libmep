// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#ifndef mep_utils_H
#define mep_utils_H
//----------------------------------------------------------------
struct s_value_class{
	double value;
	int data_class;
};
//----------------------------------------------------------------
int sort_function_value_class(const void *a, const void *b);
//----------------------------------------------------------------

int my_strcmp(char *s1, char *s2);

#define MEP_OK 0
#define E_NO_DATA 1
#define E_DATA_MUST_HAVE_REAL_TYPE 2
#define E_DATA_MUST_HAVE_STRING_TYPE 3

#endif