// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
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
unsigned int t_mep_data::get_data_type(void)const
{
	return data_type;
}
//-----------------------------------------------------------------
double* t_mep_data::get_row_as_double(unsigned int row)const
{
	return _data_double[row];
}
//-----------------------------------------------------------------
long long* t_mep_data::get_row_as_long_long(unsigned int row)const
{
	return _data_long_long[row];
}
//-----------------------------------------------------------------
double t_mep_data::get_value_double(unsigned int row, unsigned int col)const
{
	return _data_double[row][col];
}
//-----------------------------------------------------------------
long long t_mep_data::get_value_long_long(unsigned int row, unsigned int col)const
{
	return _data_long_long[row][col];
}
//-----------------------------------------------------------------
char* t_mep_data::get_value_string(unsigned int row, unsigned int col)const
{
	return _data_string[row][col];
}
//-----------------------------------------------------------------
unsigned int t_mep_data::get_num_outputs(void)const
{
	return num_outputs;
}
//-----------------------------------------------------------------
double** t_mep_data::get_data_matrix_double(void) const
{
	return _data_double;
}
//-----------------------------------------------------------------
long long** t_mep_data::get_data_matrix_long_long(void) const
{
	return _data_long_long;
}
//-----------------------------------------------------------------
unsigned int t_mep_data::get_num_items_class_0(void)const
{
	return num_class_0;
}
//-----------------------------------------------------------------
char*** t_mep_data::get_data_matrix_string(void)const
{
	return _data_string;
}
//-----------------------------------------------------------------
void t_mep_data::set_num_outputs(unsigned int new_num)
{
	num_outputs = new_num;
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
