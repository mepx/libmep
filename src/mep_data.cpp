// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <cstring>
#include <regex>

#ifdef _WIN32
	#include <windows.h>
#endif // WIN32

#include "mep_data.h"
#include "mep_utils.h"
#include "mep_rands.h"
#include "string_validation.h"

//-----------------------------------------------------------------
t_mep_data::t_mep_data(void)
{
	init();
}
//-----------------------------------------------------------------
t_mep_data::~t_mep_data()
{
	delete_data();
}
//-----------------------------------------------------------------
void t_mep_data::delete_double_data(void)
{
	if (_data_double) {
		for (unsigned int i = 0; i < num_data; i++)
			delete[] _data_double[i];
		delete[] _data_double;
		_data_double = NULL;
	}
}
//-----------------------------------------------------------------
void t_mep_data::delete_string_data(void)
{
	if (_data_string) {
		for (unsigned int i = 0; i < num_data; i++) {
			for (unsigned int v = 0; v < num_cols; v++)
				if (_data_string[i][v])
					delete[] _data_string[i][v];
			delete[] _data_string[i];
		}
		delete[] _data_string;
		_data_string = NULL;
	}
}
//-----------------------------------------------------------------
void t_mep_data::delete_data(void)
{
	delete_double_data();
	delete_string_data();
	if (class_index_of_output_col) {
		delete[] class_index_of_output_col;
        class_index_of_output_col = NULL;
	}
	
	if (class_labels){
		delete[] class_labels;
		class_labels = NULL;
	}
}
//-----------------------------------------------------------------
void t_mep_data::clear_data(void)
{
	delete_data();
	num_data = 0;
	num_cols = 0;
	data_type = MEP_DATA_DOUBLE;// double
	num_outputs = 1;
	num_classes = 0;
	num_class_0 = 0;

	_modified = false;
}
//-----------------------------------------------------------------
void t_mep_data::init(void)
{
	num_data = 0;
	num_cols = 0;

	_data_double = NULL;

	_data_string = NULL;

	list_separator = ';';

	data_type = MEP_DATA_DOUBLE;// double
	num_outputs = 1;

	num_classes = 0;

	_modified = false;
	num_class_0 = 0;
    class_index_of_output_col = NULL;
	class_labels = NULL;
}
//-----------------------------------------------------------------
void t_mep_data::count_0_class(void)
{
	num_class_0 = 0;
	if (!class_index_of_output_col)
		return;

	for (unsigned int i = 0; i < num_data; i++)
		if (class_index_of_output_col[i] == 0)
			num_class_0++;
}
//-----------------------------------------------------------------
bool t_mep_data::to_double(void)
{
	if (!_data_string) {
		data_type = MEP_DATA_DOUBLE;
		return true;
	}

	delete_double_data();

	_data_double = new double* [num_data];
	for (unsigned int r = 0; r < num_data; r++)
		_data_double[r] = new double[num_cols];
	
	for (unsigned int r = 0; r < num_data; r++)
		for (unsigned int c = 0; c < num_cols; c++)
			if (!is_valid_double(_data_string[r][c], &_data_double[r][c])) {
				// must delete all data double
				delete_double_data();
				return false;
			}

	data_type = MEP_DATA_DOUBLE;
	delete_string_data();

	return true;
}
//-----------------------------------------------------------------
void t_mep_data::set_to_minus_one(void)
{// data must be valid
	for (unsigned int r = 0; r < num_data; r++)
		for (unsigned int c = 0; c < num_cols; c++)
			_data_double[r][c] = -1;
}
//-----------------------------------------------------------------
int t_mep_data::to_numeric(t_mep_data *other_data1, t_mep_data* other_data2, char _decimal_separator)
{
	if (_decimal_separator == '.') {
		setlocale(LC_NUMERIC, "C");
	}
	else {
		setlocale(LC_NUMERIC, "ro_RO");
	}


	if (num_data || other_data1 && other_data1->num_data || other_data2 && other_data2->num_data) {

		//		int count_new_strings = 0;
		if (data_type != MEP_DATA_STRING &&
			(!other_data1 || other_data1 && other_data1->data_type != MEP_DATA_STRING) &&
			(other_data2 || other_data2 && other_data2->data_type != MEP_DATA_STRING)) {

			setlocale(LC_NUMERIC, "");
			return E_DATA_MUST_HAVE_STRING_TYPE;
		}

		if (_data_string) {
			delete_double_data();

			_data_double = new double*[num_data];
			for (unsigned int r = 0; r < num_data; r++)
				_data_double[r] = new double[num_cols];

			set_to_minus_one();
		}

		if (other_data1 && other_data1->_data_string) {

			other_data1->delete_double_data();

			other_data1->_data_double = new double*[other_data1->num_data];
			for (unsigned int r = 0; r < other_data1->num_data; r++)
				other_data1->_data_double[r] = new double[other_data1->num_cols];
			other_data1->set_to_minus_one();
		}

		if (other_data2 && other_data2->_data_string) {

			other_data2->delete_double_data();

			other_data2->_data_double = new double*[other_data2->num_data];
			for (unsigned int r = 0; r < other_data2->num_data; r++)
				other_data2->_data_double[r] = new double[other_data2->num_cols];
			other_data2->set_to_minus_one();
		}

		for (unsigned int v = 0; v < num_cols; v++) {
			int k = 0; // this will be the replacement
			//is this numeric or alpha ?
			// search in the current dataset
			if (data_type == MEP_DATA_STRING) {
				double tmp_double;
				for (unsigned int r = 0; r < num_data; r++) {
					if (!_data_string[r][v]) {
						//_data_double[r][v] = -1; // invalid data
						continue;
					}
					if (!is_valid_double(_data_string[r][v], &tmp_double)) {
						// search for it in the current set
						for (unsigned int t = r + 1; t < num_data; t++)
							if (_data_string[t][v])
								if (my_strcmp(_data_string[r][v], _data_string[t][v]) == 0) {
									_data_double[t][v] = k;
									delete[] _data_string[t][v];
									_data_string[t][v] = NULL;
								}

						// replace it in the other datasets too
						if (other_data1 && other_data1->data_type == MEP_DATA_STRING)
							for (unsigned int t = 0; t < other_data1->num_data; t++)
								if (other_data1->_data_string[t][v])
									if (my_strcmp(_data_string[r][v], other_data1->_data_string[t][v]) == 0) {
										other_data1->_data_double[t][v] = k;
										delete[] other_data1->_data_string[t][v];
										other_data1->_data_string[t][v] = NULL;
									}
						// replace it in the other datasets too
						if (other_data2 && other_data2->data_type == MEP_DATA_STRING)
							for (unsigned int t = 0; t < other_data2->num_data; t++)
								if (other_data2->_data_string[t][v])
									if (my_strcmp(_data_string[r][v], other_data2->_data_string[t][v]) == 0) {
										other_data2->_data_double[t][v] = k;
										delete[] other_data2->_data_string[t][v];
										other_data2->_data_string[t][v] = NULL;
									}
						// also replace the current value
						_data_double[r][v] = k;
						delete[] _data_string[r][v];
						_data_string[r][v] = NULL;

						_modified = true;
						k++;
					}
					else {
						_data_double[r][v] = tmp_double;
						delete[] _data_string[r][v];
						_data_string[r][v] = NULL;
					}
				}
			}

			// search in the other dataset
			
			if (other_data1 && other_data1->data_type == MEP_DATA_STRING) {
				double tmp_double1;
				for (unsigned int r = 0; r < other_data1->num_data; r++) {
					if (!other_data1->_data_string[r][v]) {
						//other_data1->_data_double[r][v] = -1;
						continue;
					}
					if (!is_valid_double(other_data1->_data_string[r][v], &tmp_double1)) {
						// search for it in the current set

						for (unsigned int t = r + 1; t < other_data1->num_data; t++)
							if (other_data1->_data_string[t][v])
								if (my_strcmp(other_data1->_data_string[r][v], other_data1->_data_string[t][v]) == 0) {
									other_data1->_data_double[t][v] = k;
									delete[] other_data1->_data_string[t][v];
									other_data1->_data_string[t][v] = NULL;
								}
						// replace it in the other datasets too
						if (other_data2 && other_data2->data_type == MEP_DATA_STRING)
							for (unsigned int t = 0; t < other_data2->num_data; t++)
								if (other_data2->_data_string[t][v])
									if (my_strcmp(other_data1->_data_string[r][v], other_data2->_data_string[t][v]) == 0) {
										other_data2->_data_double[t][v] = k;
										delete[] other_data2->_data_string[t][v];
										other_data2->_data_string[t][v] = NULL;
									}

						other_data1->_data_double[r][v] = k;
						delete[] other_data1->_data_string[r][v];
						other_data1->_data_string[r][v] = NULL;

						_modified = true;
						k++;
					}
					else {
						other_data1->_data_double[r][v] = tmp_double1;
						delete[] other_data1->_data_string[r][v];
						other_data1->_data_string[r][v] = NULL;
					}
				}
			}

			// search in the other dataset
			if (other_data2 && other_data2->data_type == MEP_DATA_STRING) {
				double tmp_double2;
				for (unsigned int r = 0; r < other_data2->num_data; r++) {
					if (!other_data2->_data_string[r][v]) {
						//other_data2->_data_double[r][v] = -1;
						continue;
					}
					if (!is_valid_double(other_data2->_data_string[r][v], &tmp_double2)) {
						// search for it in the current set
						for (unsigned int t = r + 1; t < other_data2->num_data; t++)
							if (other_data2->_data_string[t][v])
								if (my_strcmp(other_data2->_data_string[r][v], other_data2->_data_string[t][v]) == 0) {
									other_data2->_data_double[t][v] = k;
									delete[] other_data2->_data_string[t][v];
									other_data2->_data_string[t][v] = NULL;
								}

						other_data2->_data_double[r][v] = k;
						delete[] other_data2->_data_string[r][v];
						other_data2->_data_string[r][v] = NULL;

						_modified = true;
						k++;
					}
					else {
						other_data2->_data_double[r][v] = tmp_double2;
						delete[] other_data2->_data_string[r][v];
						other_data2->_data_string[r][v] = NULL;
					}
				}
			}
		}

		data_type = MEP_DATA_DOUBLE;
		if (_data_string) {
			for (unsigned int r = 0; r < num_data; r++)
				delete[] _data_string[r];
			delete[] _data_string;
			_data_string = NULL;
		}
		if (other_data1) {
			if (other_data1->_data_string) {
				for (unsigned int r = 0; r < other_data1->num_data; r++)
					delete[] other_data1->_data_string[r];
				delete[] other_data1->_data_string;
				other_data1->_data_string = NULL;
			}
			other_data1->data_type = MEP_DATA_DOUBLE;
		}
		if (other_data2) {
			if (other_data2->_data_string) {
				for (unsigned int r = 0; r < other_data2->num_data; r++)
					delete[] other_data2->_data_string[r];
				delete[] other_data2->_data_string;
				other_data2->_data_string = NULL;
			}
			other_data2->data_type = MEP_DATA_DOUBLE;
		}
		setlocale(LC_NUMERIC, "");
		return MEP_OK;
	}
	else {
		setlocale(LC_NUMERIC, "");
		return E_NO_DATA;
	}
}
//-----------------------------------------------------------------
int t_mep_data::to_interval_selected_col(double min, double max, unsigned int col, 
	t_mep_data *other_data1, t_mep_data* other_data2)
{
	if (num_data || other_data1 && other_data1->num_data || other_data2 && other_data2->num_data) {
		if (data_type != MEP_DATA_DOUBLE || other_data1 && other_data1->num_data && other_data1->data_type != MEP_DATA_DOUBLE || other_data2 && other_data2->num_data && other_data2->data_type != MEP_DATA_DOUBLE) // string
			return E_DATA_MUST_HAVE_REAL_TYPE;

		double min_col = 0;
		double max_col = 0;

		if (num_data && num_cols > col) {
			min_col = _data_double[0][col];
			max_col = _data_double[0][col];
		}
		else
			if (other_data1->num_data && other_data1->num_cols > col) {
				min_col = other_data1->_data_double[0][col];
				max_col = other_data1->_data_double[0][col];
			}
			else
				if (other_data2->num_data && other_data2->num_cols > col) {
					min_col = other_data2->_data_double[0][col];
					max_col = other_data2->_data_double[0][col];
				}

		if (num_cols > col)
			for (unsigned int t = 0; t < num_data; t++) {
				if (min_col > _data_double[t][col])
					min_col = _data_double[t][col];
				if (max_col < _data_double[t][col])
					max_col = _data_double[t][col];
			}

		if (other_data1->num_cols > col)
			for (unsigned int t = 0; t < other_data1->num_data; t++) {
				if (min_col > other_data1->_data_double[t][col])
					min_col = other_data1->_data_double[t][col];
				if (max_col < other_data1->_data_double[t][col])
					max_col = other_data1->_data_double[t][col];
			}

		if (other_data2->num_cols > col)
			for (unsigned int t = 0; t < other_data2->num_data; t++) {
				if (min_col > other_data2->_data_double[t][col])
					min_col = other_data2->_data_double[t][col];
				if (max_col < other_data2->_data_double[t][col])
					max_col = other_data2->_data_double[t][col];
			}
		// min, max found
		// now scale
		if (num_cols > col) {
			_modified = true;
			for (unsigned int t = 0; t < num_data; t++)
				_data_double[t][col] -= min_col;
			if (max_col > min_col)
				for (unsigned int t = 0; t < num_data; t++) {
					_data_double[t][col] /= (max_col - min_col);
					_data_double[t][col] *= (max - min);
				}
			for (unsigned int t = 0; t < num_data; t++)
				_data_double[t][col] += min;
		}

		if (other_data1->num_cols > col) {
			other_data1->_modified = true;
			for (unsigned int t = 0; t < other_data1->num_data; t++)
				other_data1->_data_double[t][col] -= min_col;
			if (max_col > min_col)
				for (unsigned int t = 0; t < other_data1->num_data; t++) {
					other_data1->_data_double[t][col] /= (max_col - min_col);
					other_data1->_data_double[t][col] *= (max - min);
				}
			for (unsigned int t = 0; t < other_data1->num_data; t++)
				other_data1->_data_double[t][col] += min;

		}

		if (other_data2->num_cols > col) {
			other_data2->_modified = true;
			for (unsigned int t = 0; t < other_data2->num_data; t++)
				other_data2->_data_double[t][col] -= min_col;
			if (max_col > min_col)
				for (unsigned int t = 0; t < other_data2->num_data; t++) {
					other_data2->_data_double[t][col] /= (max_col - min_col);
					other_data2->_data_double[t][col] *= (max - min);
				}
			for (unsigned int t = 0; t < other_data2->num_data; t++)
				other_data2->_data_double[t][col] += min;
		}

		return MEP_OK;
	}
	else
		return E_NO_DATA;

}
//-----------------------------------------------------------------
int t_mep_data::to_interval_everywhere(double min, double max, 
	t_mep_data *other_data1, t_mep_data* other_data2)
{
	int result = to_interval_all_variables(min, max, other_data1, other_data2);
	if (result != MEP_OK)
		return result;
	if (num_outputs)
		return to_interval_selected_col(min, max, num_cols - 1, other_data1, other_data2);
	return MEP_OK;
}
//-----------------------------------------------------------------
int t_mep_data::to_interval_all_variables(double min, double max, 
	t_mep_data *other_data1, t_mep_data* other_data2)
{
	for (unsigned int v = 0; v < num_cols - 1; v++) {
		int result = to_interval_selected_col(min, max, v, other_data1, other_data2);
		if (result != MEP_OK)
			return result;
		//is this numeric or alpha ?
	}
	return MEP_OK;
}
//-----------------------------------------------------------------
int t_mep_data::move_to(t_mep_data *dest, unsigned int count)
{
	if (!(data_type == dest->data_type || !dest->num_data))
		return MEP_E_CANNOT_MOVE_DATA_OF_DIFFERENT_TYPES; // can move only of the same type

	if (num_data && dest->num_data && num_cols != dest->num_cols)
		return MEP_E_DEST_AND_SOURCE_MUST_HAVE_THE_SAME_NUMBER_OF_COLUMNS; // 

	if (num_data >= count) {

		_modified = true;

		if (dest->num_data == 0) {
			dest->num_cols = num_cols;
			dest->num_outputs = num_outputs;
		}

		if (data_type == MEP_DATA_DOUBLE) {// double

			double** tmp_data_double = new double*[dest->num_data + count];
			for (unsigned int i = 0; i < dest->num_data; i++)
				tmp_data_double[i] = dest->_data_double[i];

			for (unsigned int i = 0; i < count; i++)
				tmp_data_double[i + dest->num_data] = _data_double[num_data - count + i];

			if (dest->_data_double)
				delete[] dest->_data_double;
			dest->_data_double = tmp_data_double;

			if (num_data - count > 0) {
				tmp_data_double = new double*[num_data - count];
				for (unsigned int i = 0; i < num_data - count; i++)
					tmp_data_double[i] = _data_double[i];

				delete[] _data_double;
				_data_double = tmp_data_double;
			}
			else {// no more data left in the source
				delete[] _data_double;
				_data_double = NULL;
			}

			dest->data_type = MEP_DATA_DOUBLE;
		}
		else {// string
			char*** tmp_data_string = new char**[dest->num_data + count];
			for (unsigned int i = 0; i < dest->num_data; i++)
				tmp_data_string[i] = dest->_data_string[i];

			for (unsigned int i = 0; i < count; i++)
				tmp_data_string[i + dest->num_data] = _data_string[num_data - count + i];

			if (dest->_data_string)
				delete[] dest->_data_string;
			dest->_data_string = tmp_data_string;

			if (num_data - count > 0) {
				tmp_data_string = new char**[num_data - count];
				for (unsigned int i = 0; i < num_data - count; i++)
					tmp_data_string[i] = _data_string[i];

				delete[] _data_string;
				_data_string = tmp_data_string;
			}
			else {// no more data left in the source
				delete[] _data_string;
				_data_string = NULL;
			}

			dest->data_type = MEP_DATA_STRING;
		}


		num_data -= count;
		dest->num_data += count;

	}
	else
		return MEP_E_NOT_ENOUGH_DATA_TO_MOVE; // not enough data
	return MEP_E_OK;
}
//-----------------------------------------------------------------
bool re_match(char *str, const char *pattern, bool use_regular)
{
	if (!use_regular)
		return !strcmp(pattern, str);
	else {
		std::regex re_pattern(pattern, std::regex::basic | std::regex::icase);
		if (std::regex_match(str, re_pattern))
			return true;
		else
			return false;
	}
}
//-----------------------------------------------------------------
int t_mep_data::replace_symbol_from_selected_col(const char *s_find_what, const char* s_replace_with, unsigned int col, bool use_regular)
{
	int count_replaced = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (unsigned int t = 0; t < num_data; t++)
			if (re_match(_data_string[t][col], s_find_what, use_regular)) {
				// this is a missing value
				strcpy(_data_string[t][col], s_replace_with);
				count_replaced++;
			}
	}
	else
		if (data_type == MEP_DATA_DOUBLE) { // double
			// try to convert them to double
			char* pEnd;
			double d_find_what;
			d_find_what = strtod(s_find_what, &pEnd);

			double d_replace_with;
			d_replace_with = strtod(s_replace_with, &pEnd);

			for (unsigned int t = 0; t < num_data; t++)
				if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
					// this is a missing value
					_data_double[t][col] = d_replace_with;
					count_replaced++;
				}
		}

	_modified = true;
	return count_replaced;
}
//-----------------------------------------------------------------
int  t_mep_data::replace_symbol_everywhere(const char *s_find_what, const char* s_replace_with, bool use_regular)
{
	int tmp_count;
	tmp_count = replace_symbol_from_all_variables(s_find_what, s_replace_with, use_regular);
	int count_replaced = tmp_count;
	tmp_count = replace_symbol_from_selected_col(s_find_what, s_replace_with, num_cols - 1, use_regular);
	count_replaced += tmp_count;

	_modified = true;

	return count_replaced;
}
//-----------------------------------------------------------------
int  t_mep_data::replace_symbol_from_all_variables(const char *s_find_what, const char* s_replace_with, bool use_regular)
{
	int count_replaced = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (unsigned int col = 0; col < num_cols - 1; col++)
			for (unsigned int t = 0; t < num_data; t++)
				if (re_match(_data_string[t][col], s_find_what, use_regular)) {
					// this is a missing value
					strcpy(_data_string[t][col], s_replace_with);
					count_replaced++;
				}
	}
	else
		if (data_type == MEP_DATA_DOUBLE) { // double
			// try to convert them to double
			char* pEnd;
			double d_find_what;
			d_find_what = strtod(s_find_what, &pEnd);

			double d_replace_with;
			d_replace_with = strtod(s_replace_with, &pEnd);

			for (unsigned int col = 0; col < num_cols - 1; col++)
				for (unsigned int t = 0; t < num_data; t++)
					if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
						// this is a missing value
						_data_double[t][col] = d_replace_with;
						count_replaced++;
					}
		}

	_modified = true;
	return count_replaced;
}
//-----------------------------------------------------------------
int t_mep_data::find_symbol_from_selected_col(const char *s_find_what, unsigned int col, bool use_regular)
{
	int count_found = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (unsigned int t = 0; t < num_data; t++)
			if (re_match(_data_string[t][col], s_find_what, use_regular)) {
				// this is a missing value
				count_found++;
			}
	}
	else
		if (data_type == MEP_DATA_DOUBLE) { // double
			// try to convert them to double
			char* pEnd;
			double d_find_what;
			d_find_what = strtod(s_find_what, &pEnd);

			for (unsigned int t = 0; t < num_data; t++)
				if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
					// this is a missing value
					count_found++;
				}
		}

	_modified = true;
	return count_found;
}
//-----------------------------------------------------------------
int t_mep_data::find_symbol_everywhere(const char *s_find_what, bool use_regular)
{
	int tmp_count;
	tmp_count = find_symbol_from_all_variables(s_find_what, use_regular);
	int count_found = tmp_count;
	tmp_count = find_symbol_from_selected_col(s_find_what, num_cols - 1, use_regular);
	count_found += tmp_count;

	return count_found;
}
//-----------------------------------------------------------------
int t_mep_data::find_symbol_from_all_variables(const char *s_find_what, bool use_regular)
{
	int count_found;
	count_found = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (unsigned int col = 0; col < num_cols - 1; col++)
			for (unsigned int t = 0; t < num_data; t++)
				if (re_match(_data_string[t][col], s_find_what, use_regular)) {
					// this is a missing value

					count_found++;
				}
	}
	else
		if (data_type == MEP_DATA_DOUBLE) { // double
			// try to convert them to double
			char* pEnd;
			double d_find_what;
			d_find_what = strtod(s_find_what, &pEnd);

			for (unsigned int col = 0; col < num_cols - 1; col++)
				for (unsigned int t = 0; t < num_data; t++)
					if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
						// this is a missing value
						count_found++;
					}
		}

	return count_found;
}
//-----------------------------------------------------------------
void t_mep_data::shuffle(t_seed &seed)
{
	if (data_type == MEP_DATA_DOUBLE) { // double
		for (unsigned int i = num_data - 1; i >= 1; i--) {
			unsigned int j = mep_unsigned_int_rand(seed, 0, i);
			double *row = _data_double[i];
			_data_double[i] = _data_double[j];
			_data_double[j] = row;
		}
	}
	else {
		// string
		for (unsigned int i = num_data - 1; i >= 1; i--) {
			unsigned int j = mep_unsigned_int_rand(seed, 0, i);
			char**row = _data_string[i];
			_data_string[i] = _data_string[j];
			_data_string[j] = row;
		}
	}
	_modified = true;
}
//-----------------------------------------------------------------
bool t_mep_data::are_all_output_int(void)const
{
	for (unsigned int i = 0; i < num_data; i++) {
		if (fabs(_data_double[i][num_cols - 1] - (int)_data_double[i][num_cols - 1]) > 1E-6)
			// all must be int
			return false;
	}
    return true;
}
//-----------------------------------------------------------------
bool t_mep_data::is_binary_classification_problem(void)const
{
	// call compute_class_labels first!!!
	if (num_cols < 2)
		return false;

	if (num_classes != 2)
		return false;

	return true;
}
//-----------------------------------------------------------------
bool t_mep_data::is_multi_class_classification_problem(void)const
{
	// call compute_class_labels first!!!
	if (num_cols < 2)
		return false;

	if (num_classes < 2)
		return false;

	return true;
}
//-----------------------------------------------------------------
bool t_mep_data::is_multi_class_classification_problem_within_class_labels(
	int *training_class_labels, unsigned long num_labels) const
{
	if (num_cols < 2)
		return false;

	for (unsigned int i = 0; i < num_data; i++) {
		bool found = false;
		for (unsigned long j = 0; j < num_labels; j++) {
			if ((int)_data_double[i][num_cols - 1] == training_class_labels[j]) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false; // some data has another target different from existing class labels
		}
	}

	return true;
}
//-----------------------------------------------------------------
int t_mep_data::is_one_of_m_multi_class_classification_problem(unsigned int presumed_num_classes)const
{
	// test if last presumed_num_classes cols are 0 and 1 only
	if (presumed_num_classes < 2)
		return MEP_DATA_NUM_CLASSES_TOO_FEW;

	if (presumed_num_classes >= num_cols)
		return MEP_DATA_NUM_CLASSES_TOO_MANY;

	for (unsigned int r = 0; r < num_data; r++) {
		unsigned int count_0 = 0;
		unsigned int count_1 = 0;
		for (unsigned int c = num_cols - presumed_num_classes; c < num_cols; c++) {
			double value;
			if (!is_valid_double(_data_string[r][c], &value))
				return MEP_DATA_NOT_NUMERICAL_VALUE;
			if (fabs(value) < 1E-6)
				count_0++;
			else
				if (fabs(value - 1.0) < 1E-6)
					count_1++;
		}
		if (count_1 + count_0 != presumed_num_classes)
			return MEP_DATA_NOT_ONE_OF_M;
		if (count_1 != 1)
			return MEP_DATA_MORE_THAN_ONE_1;
	}
	return MEP_E_OK;
}
//-----------------------------------------------------------------
void t_mep_data::to_one_of_m_multi_class_classification_problem(unsigned int presumed_num_classes)
{
	if (_data_string) {
		for (unsigned int r = 0; r < num_data; r++) {
			unsigned int class_index = 0;
			for (unsigned int c = num_cols - presumed_num_classes; c < num_cols; c++) {
				double value;
				is_valid_double(_data_string[r][c], &value); // it should always be valid since I have tested it earlier
				if (fabs(value - 1.0) < 1E-6)
					class_index = c - (num_cols - presumed_num_classes);
			}
			// delete all memory after the last new Target col
			for (unsigned int c = num_cols - presumed_num_classes; c < num_cols; c++) {
				if (_data_string[r][c])
					delete[] _data_string[r][c];
			}
			char buf[12];
			sprintf(buf, "%u", class_index);
			_data_string[r][num_cols - presumed_num_classes] = new char[strlen(buf) + 1];
			strcpy(_data_string[r][num_cols - presumed_num_classes], buf);
		}
	}
	else {// data double
		for (unsigned int r = 0; r < num_data; r++) {
			unsigned int class_index = 0;
			for (unsigned int c = num_cols - presumed_num_classes; c < num_cols; c++) {
				if (fabs(_data_double[r][c] - 1.0) < 1E-6)
					class_index = c - (num_cols - presumed_num_classes);
			}
			_data_double[r][num_cols - presumed_num_classes] = class_index;
		}
	}
	num_cols -= presumed_num_classes - 1;
	num_classes = presumed_num_classes;
	num_outputs = 1;
}
//-----------------------------------------------------------------
bool t_mep_data::is_modified(void)const
{
	return _modified;
}
//-----------------------------------------------------------------
/*
void t_mep_data::count_num_classes(int * training_class_labels)
{
	if (data_type == MEP_DATA_DOUBLE) {
		if (num_outputs && num_data) {
			unsigned int max_value = (unsigned int)_data_double[0][num_cols - 1];
			for (unsigned int i = 1; i < num_data; i++)
				if (max_value < _data_double[i][num_cols - 1])
					max_value = (unsigned int)_data_double[i][num_cols - 1];
			num_classes = max_value + 1;
		}
		else
			num_classes = 0;
	}
	else
		num_classes = 0;
}
*/
//-----------------------------------------------------------------
void t_mep_data::add_string_data(unsigned int row, unsigned int col, const char* data)
{
	if (row >= num_data) {
		// must add some new rows
		char*** new_data_string = new char** [row + 1];
		for (unsigned int r = 0; r < num_data; r++)
			new_data_string[r] = _data_string[r];
		for (unsigned int r = num_data; r <= row; r++) {
			if (num_cols) {
				new_data_string[r] = new char* [num_cols];
				for (unsigned int c = 0; c < num_cols; c++)
					new_data_string[r][c] = NULL;
			}
			else
				new_data_string[r] = NULL;
		}

		if (_data_string)
			delete[] _data_string;
		_data_string = new_data_string;

		num_data = row + 1;
	}

	if (col >= num_cols) {
		// must add some new cols
		// previous rows must be updated
		for (unsigned int r = 0; r < num_data; r++) {
			char** old_row = _data_string[r];
			_data_string[r] = new char* [col + 1];
			for (unsigned int c = 0; c < num_cols; c++)
				_data_string[r][c] = old_row[c];
			for (unsigned int c = num_cols; c <= col; c++)
				_data_string[r][c] = NULL;

			if (old_row)
				delete[] old_row;
		}

		num_cols = col + 1;
	}

	// now add the data
	if (data) {
		// must do TRIM first
		_data_string[row][col] = new char[strlen(data) + 1];
		
		trim_and_strcpy(_data_string[row][col], data);
	}
}
//-----------------------------------------------------------------
void t_mep_data::add_string_data_to_row(unsigned int row, unsigned int col, const char* data)
{
	if (col >= num_cols) {
		// must add some new cols
		// previous rows must be updated
		for (unsigned int r = 0; r < num_data; r++) {
			char** old_row = _data_string[r];
			_data_string[r] = new char* [col + 1];
			for (unsigned int c = 0; c < num_cols; c++)
				_data_string[r][c] = old_row[c];
			for (unsigned int c = num_cols; c <= col; c++)
				_data_string[r][c] = NULL;

			if (old_row)
				delete[] old_row;
		}

		num_cols = col + 1;
	}

	// now add the data
	if (data) {
		// must do TRIM first
		_data_string[row][col] = new char[strlen(data) + 1];

		trim_and_strcpy(_data_string[row][col], data);
	}
}
//-----------------------------------------------------------------
void t_mep_data::from_tab_separated_string(const char* s, char _list_separator, char _decimal_separator)
{
	clear_data();

	list_separator = _list_separator;
	//decimal_separator = _decimal_separator;

	clear_data();
	lconv* lc;
	lc = localeconv();
	//char saved_decimal_separator = lc->decimal_point[0];

	if (_decimal_separator == '.') {
		setlocale(LC_NUMERIC, "C");
	}
	else {
		setlocale(LC_NUMERIC, "ro_RO");
	}
	
	data_type = MEP_DATA_STRING;
	get_csv_info_from_string(s, list_separator, this);

	remove_empty_rows_string();

	// try to convert to double
	to_double();

	setlocale(LC_NUMERIC, "");
}
//-----------------------------------------------------------------
void t_mep_data::from_tab_separated_string_no_conversion_to_double (const char* s, char _list_separator)
{
	clear_data();

	list_separator = _list_separator;

	data_type = MEP_DATA_STRING;
	get_csv_info_from_string(s, list_separator, this);

	remove_empty_rows_string();

	setlocale(LC_NUMERIC, "");
}
//-----------------------------------------------------------------
void t_mep_data::remove_empty_rows_string(void)
{
	if (data_type == MEP_DATA_STRING) {
		// try to remove the empty rows
		if (num_data && num_cols) {
			char*** _new_data_string = new char** [num_data];
			unsigned int new_num_data = 0;
			for (unsigned int r = 0; r < num_data; r++) {
				if (_data_string[r]) {
					bool is_empty = true;
					for (unsigned int c = 0; c < num_cols; c++)
						if (_data_string[r][c] && _data_string[r][c][0]) {
							is_empty = false;
							break;
						}
					if (!is_empty) {
						_new_data_string[new_num_data] = _data_string[r];
						new_num_data++;
					}
					else {
						for (unsigned int c = 0; c < num_cols; c++)
							if (_data_string[r][c])
								delete[] _data_string[r][c];
						delete[] _data_string[r];
					}
				}
			}
			if (new_num_data < num_data) {
				delete[] _data_string;
				_data_string = _new_data_string;
				num_data = new_num_data;
			}
			else
				delete[] _new_data_string;
		}
	}
}
//-----------------------------------------------------------------
bool t_mep_data::could_be_univariate_time_serie(void) const
{
	return (num_cols == 1 && num_data >= 2);// if num_data == 2 it makes no sense to make an extra step to time serie because is the same
}
//-----------------------------------------------------------------
bool t_mep_data::to_time_serie_from_univariate(const t_mep_data& source, unsigned int window_size)
{
	if (!(source.num_cols == 1 && source.num_data >= window_size + 1))
		return false;

	if (data_type == MEP_DATA_DOUBLE) {
		// double
		// 1 col only
		_data_double = new double* [source.num_data - window_size];
		for (unsigned int r = 0; r < source.num_data - window_size; r++) {
			_data_double[r] = new double[window_size + 1];
			for (unsigned int c = 0; c < window_size + 1; c++)
				_data_double[r][c] = source._data_double[r + c][0];
		}
	}
	else
		return false;

	num_data = source.num_data - window_size;
	num_cols = window_size + 1;

	return true;
}
//-----------------------------------------------------------------
/*
bool t_mep_data::to_time_serie_from_single_col(const t_mep_data& source, const t_mep_data* prev_data1, const t_mep_data* prev_data2, unsigned int window_size)
{
	if (source.num_cols != 1)
		return false;

	if (data_type != MEP_DATA_DOUBLE)
		return false;

	if (!prev_data1) {// no previous data; it means that is the training set
		_data_double = new double* [source.num_data - window_size];
		for (unsigned int r = 0; r < source.num_data - window_size; r++) {
			_data_double[r] = new double[window_size + 1];
			for (unsigned int c = 0; c < window_size + 1; c++)
				_data_double[r][c] = source._data_double[r + c][0];
		}
		num_data = source.num_data - window_size;
		num_cols = window_size + 1;
	}
	else {
		// prev_data1 is available; so we have validation or test
		_data_double = new double* [source.num_data];
		for (unsigned int r = 0; r < source.num_data - window_size; r++) {
			_data_double[r] = new double[window_size + 1];
			for (unsigned int c = 0; c < window_size + 1; c++)
				_data_double[r][c] = source._data_double[r + c][0];
		}
		num_data = source.num_data;
		num_cols = window_size + 1;
	}


	return true;
}
*/
//-----------------------------------------------------------------
bool t_mep_data::compute_min_max_of_target(double& min_target, double& max_target)
{
	if (data_type != MEP_DATA_DOUBLE)
		return false;
	if (!num_outputs)
		return false;

	min_target = _data_double[0][num_cols - 1];
	max_target = _data_double[0][num_cols - 1];
	for (unsigned int i = 1; i < num_data; i++) {
		if (min_target > _data_double[0][num_cols - 1])
			min_target = _data_double[0][num_cols - 1];
		if (max_target < _data_double[0][num_cols - 1])
			max_target = _data_double[0][num_cols - 1];
	}
	return true;
}
//-----------------------------------------------------------------
void t_mep_data::compute_class_labels(void)
{
    // this is for training data only
    // for all others use the next function
	if (class_labels) {
		delete[] class_labels;
		class_labels = NULL;
	}
    
    if (class_index_of_output_col){
        delete[] class_index_of_output_col;
        class_index_of_output_col = NULL;
    }

	if (data_type == MEP_DATA_DOUBLE) {
		// count num classes first

		num_classes = 0;
		if (num_outputs && num_data) {
			class_labels = new int[num_data]; // I allocate more memory than required because I do not know yet how many classes are there
			// get the list of labels
			for (unsigned int i = 0; i < num_data; i++) {
				bool found = false;
				for (unsigned int j = 0; j < num_classes; j++)
					if ((int)_data_double[i][num_cols - 1] == class_labels[j]) {
						found = true;
						break;
					}
				if (!found) {
					class_labels[num_classes] = (int)_data_double[i][num_cols - 1];
					num_classes++;
				}
			}
			// now I sort the class labels
			qsort(class_labels, num_classes, sizeof(int), compare_int);
			// and I assign an index of a label to each
			class_index_of_output_col = new unsigned int[num_data];
			for (unsigned int i = 0; i < num_data; i++) {
				bool found = false;
				unsigned int j;
				for (j = 0; j < num_classes; j++)
					if ((int)_data_double[i][num_cols - 1] == class_labels[j]) {
						found = true;
						break;
					}
				if (found) {
					class_index_of_output_col[i] = j;
				}
			}
		}
		else
			num_classes = 0;
	}
	else
		num_classes = 0;

}
//-----------------------------------------------------------------
int* t_mep_data::get_class_labels_ptr(void)
{
	return class_labels;
}
//-----------------------------------------------------------------
void t_mep_data::assign_class_index_from_training_class_labels(
    int* training_class_labels, unsigned int training_num_classes)
{
    if (class_index_of_output_col){
        delete[] class_index_of_output_col;
        class_index_of_output_col = NULL;
    }
    
    if (num_data && training_num_classes)
        class_index_of_output_col = new unsigned int[num_data];
	num_classes = 0;
    for (unsigned int i = 0; i < num_data; i++) {
        bool found = false;
        for (unsigned int j = 0; j < training_num_classes; j++)
            if (training_class_labels[j] ==
                (int)_data_double[i][num_cols - 1]) {
                found = true;
				class_index_of_output_col[i] = j;
                break;
            }
        if (found) {
            num_classes++;
        }
        else
            class_index_of_output_col[i] = training_num_classes;// assign the highest class index plus one
    }
}
//-----------------------------------------------------------------
