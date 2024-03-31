// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <string.h>
//-----------------------------------------------------------------
#include "mep_data.h"
//-----------------------------------------------------------------
unsigned int t_mep_data::get_num_rows(void)const
{
	return num_data;
}
//-----------------------------------------------------------------
unsigned int t_mep_data::get_num_cols(void)const
{
	return num_cols;
}
//-----------------------------------------------------------------
char t_mep_data::get_data_type(void)const
{
	return data_type;
}
//-----------------------------------------------------------------
double* t_mep_data::get_row_as_double(unsigned int row)const
{
	return _data_double[row];
}
//-----------------------------------------------------------------
long long* t_mep_data::get_row_as_long(unsigned int row)const
{
	return _data_long_long[row];
}
//-----------------------------------------------------------------
double t_mep_data::get_value_double(unsigned int row, unsigned int col)const
{
	return _data_double[row][col];
}
//-----------------------------------------------------------------
void t_mep_data::get_range_values_double(unsigned int row,
										   unsigned int start_col, unsigned int count,
										   double* values)const
{
	memcpy(values, _data_double[row] + start_col, sizeof(double) * count);
}
//-----------------------------------------------------------------
long long t_mep_data::get_value_long(unsigned int row, unsigned int col)const
{
	return _data_long_long[row][col];
}
//-----------------------------------------------------------------
void t_mep_data::get_range_values_long(unsigned int row,
										   unsigned int start_col, unsigned int count,
										   long long* values)const
{
	memcpy(values, _data_long_long[row] + start_col, sizeof(long long) * count);
}
//-----------------------------------------------------------------
char* t_mep_data::get_value_string(unsigned int row, unsigned int col)const
{
	return _data_string[row][col];
}
//-----------------------------------------------------------------
double** t_mep_data::get_data_matrix_as_double(void) const
{
	return _data_double;
}
//-----------------------------------------------------------------
long long** t_mep_data::get_data_matrix_as_long(void) const
{
	return _data_long_long;
}
//-----------------------------------------------------------------
unsigned int t_mep_data::get_num_items_class_0(void)const
{
	return num_class_0;
}
//-----------------------------------------------------------------
char*** t_mep_data::get_data_matrix_as_string(void)const
{
	return _data_string;
}
//-----------------------------------------------------------------
unsigned int t_mep_data::get_num_classes(void)const
{
	return num_classes;
}
//-----------------------------------------------------------------
int t_mep_data::get_class_label_index(unsigned int row) const
{
    return class_index_of_output_col[row];
}
//-----------------------------------------------------------------
unsigned int* t_mep_data::get_class_label_index_as_array(void) const
{
    return class_index_of_output_col;
}
//-----------------------------------------------------------------
