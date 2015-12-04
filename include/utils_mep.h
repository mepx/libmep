#ifndef UTILS_MEP
#define UTILS_MEP

//----------------------------------------------------------------
struct s_value_class{
	double value;
	int clasa;
};

//----------------------------------------------------------------
int sort_function_value_class(const void *a, const void *b);
//----------------------------------------------------------------

int my_strcmp(char *s1, char *s2);

#endif