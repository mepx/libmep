#ifndef MEP_BINARY_CLASSIFICATION_UTILS_H
#define MEP_BINARY_CLASSIFICATION_UTILS_H
//----------------------------------------------------------------
struct s_value_class{
	double value;
	int class_index;
};
//----------------------------------------------------------------
int sort_function_value_class(const void *a, const void *b);
//----------------------------------------------------------------
#endif