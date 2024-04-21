// Multi Expression Programming library
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
//-----------------------------------------------------------------
#ifdef _WIN32
	#include <windows.h>
#endif // WIN32
//-----------------------------------------------------------------
#include "mep_data.h"
#include "utils/func_utils.h"
#include "utils/rands_generator_utils.h"
#include "utils/validation_string.h"
//-----------------------------------------------------------------
int t_mep_data::from_string_automatic_replace_to_numeric_per_column_all(
													  t_mep_data *other_data1,
						   t_mep_data* other_data2,
						   char _decimal_separator,
										  int new_data_type)
{
	if (new_data_type == MEP_DATA_DOUBLE)
		return from_string_automatic_replace_to_double_per_column_all(other_data1, other_data2, _decimal_separator);
	else
		return from_string_automatic_replace_to_long_per_column_all(other_data1, other_data2);
}
//-----------------------------------------------------------------
int t_mep_data::from_string_automatic_replace_to_double_per_column_all(
													 t_mep_data *other_data1,
						   t_mep_data* other_data2,
						   char _decimal_separator)
{
	
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	char* saved_locale = strdup (old_locale);
	
	if (_decimal_separator == '.') 
		setlocale(LC_NUMERIC, "C");
	else
		setlocale(LC_NUMERIC, "");

	if (num_data ||
		(other_data1 && other_data1->num_data) ||
		(other_data2 && other_data2->num_data)) {

		//		int count_new_strings = 0;
		if (data_type != MEP_DATA_STRING &&
			(!other_data1 ||
			 (other_data1 && other_data1->data_type != MEP_DATA_STRING)) &&
			(other_data2 ||
			 (other_data2 && other_data2->data_type != MEP_DATA_STRING))
			) {

			setlocale(LC_NUMERIC, saved_locale);
			free (saved_locale);

			return E_MEP_DATA_MUST_HAVE_STRING_TYPE;
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
		setlocale(LC_NUMERIC, saved_locale);
		free (saved_locale);

		return E_MEP_DATA_OK;
	}
	else {
		setlocale(LC_NUMERIC, saved_locale);
		free (saved_locale);

		return E_MEP_NO_DATA;
	}
}
//-----------------------------------------------------------------
int t_mep_data::from_string_automatic_replace_to_long_per_column_all(
													 t_mep_data *other_data1,
						   t_mep_data* other_data2)
{
	if (num_data ||
		(other_data1 && other_data1->num_data) ||
		(other_data2 && other_data2->num_data)) {

		//		int count_new_strings = 0;
		if (data_type != MEP_DATA_STRING &&
			(!other_data1 || (other_data1 && other_data1->data_type != MEP_DATA_STRING)) &&
			(other_data2 || (other_data2 && other_data2->data_type != MEP_DATA_STRING))) {

			return E_MEP_DATA_MUST_HAVE_STRING_TYPE;
		}

		if (_data_string) {
			delete_long_data();

			_data_long_long = new long long*[num_data];
			for (unsigned int r = 0; r < num_data; r++)
				_data_long_long[r] = new long long[num_cols];

			set_to_minus_one();
		}

		if (other_data1 && other_data1->_data_string) {

			other_data1->delete_long_data();

			other_data1->_data_long_long = new long long*[other_data1->num_data];
			for (unsigned int r = 0; r < other_data1->num_data; r++)
				other_data1->_data_long_long[r] = new long long[other_data1->num_cols];
			other_data1->set_to_minus_one();
		}

		if (other_data2 && other_data2->_data_long_long) {

			other_data2->delete_long_data();

			other_data2->_data_long_long = new long long*[other_data2->num_data];
			for (unsigned int r = 0; r < other_data2->num_data; r++)
				other_data2->_data_long_long[r] = new long long[other_data2->num_cols];
			other_data2->set_to_minus_one();
		}

		for (unsigned int v = 0; v < num_cols; v++) {
			int k = 0; // this will be the replacement
			//is this numeric or alpha ?
			// search in the current dataset
			if (data_type == MEP_DATA_STRING) {
				long long tmp_long;
				for (unsigned int r = 0; r < num_data; r++) {
					if (!_data_string[r][v]) {
						//_data_long_long[r][v] = -1; // invalid data
						continue;
					}
					if (!is_valid_long_long(_data_string[r][v], &tmp_long)) {
						// search for it in the current set
						for (unsigned int t = r + 1; t < num_data; t++)
							if (_data_string[t][v])
								if (my_strcmp(_data_string[r][v], _data_string[t][v]) == 0) {
									_data_long_long[t][v] = k;
									delete[] _data_string[t][v];
									_data_string[t][v] = NULL;
								}

						// replace it in the other datasets too
						if (other_data1 && other_data1->data_type == MEP_DATA_STRING)
							for (unsigned int t = 0; t < other_data1->num_data; t++)
								if (other_data1->_data_string[t][v])
									if (my_strcmp(_data_string[r][v], other_data1->_data_string[t][v]) == 0) {
										other_data1->_data_long_long[t][v] = k;
										delete[] other_data1->_data_string[t][v];
										other_data1->_data_string[t][v] = NULL;
									}
						// replace it in the other datasets too
						if (other_data2 && other_data2->data_type == MEP_DATA_STRING)
							for (unsigned int t = 0; t < other_data2->num_data; t++)
								if (other_data2->_data_string[t][v])
									if (my_strcmp(_data_string[r][v], other_data2->_data_string[t][v]) == 0) {
										other_data2->_data_long_long[t][v] = k;
										delete[] other_data2->_data_string[t][v];
										other_data2->_data_string[t][v] = NULL;
									}
						// also replace the current value
						_data_long_long[r][v] = k;
						delete[] _data_string[r][v];
						_data_string[r][v] = NULL;

						_modified = true;
						k++;
					}
					else {
						_data_long_long[r][v] = tmp_long;
						delete[] _data_string[r][v];
						_data_string[r][v] = NULL;
					}
				}
			}

			// search in the other dataset
			
			if (other_data1 && other_data1->data_type == MEP_DATA_STRING) {
				long long tmp_long1;
				for (unsigned int r = 0; r < other_data1->num_data; r++) {
					if (!other_data1->_data_string[r][v]) {
						//other_data1->_data_double[r][v] = -1;
						continue;
					}
					if (!is_valid_long_long(other_data1->_data_string[r][v], &tmp_long1)) {
						// search for it in the current set

						for (unsigned int t = r + 1; t < other_data1->num_data; t++)
							if (other_data1->_data_string[t][v])
								if (my_strcmp(other_data1->_data_string[r][v], other_data1->_data_string[t][v]) == 0) {
									other_data1->_data_long_long[t][v] = k;
									delete[] other_data1->_data_string[t][v];
									other_data1->_data_string[t][v] = NULL;
								}
						// replace it in the other datasets too
						if (other_data2 && other_data2->data_type == MEP_DATA_STRING)
							for (unsigned int t = 0; t < other_data2->num_data; t++)
								if (other_data2->_data_string[t][v])
									if (my_strcmp(other_data1->_data_string[r][v], other_data2->_data_string[t][v]) == 0) {
										other_data2->_data_long_long[t][v] = k;
										delete[] other_data2->_data_string[t][v];
										other_data2->_data_string[t][v] = NULL;
									}

						other_data1->_data_long_long[r][v] = k;
						delete[] other_data1->_data_string[r][v];
						other_data1->_data_string[r][v] = NULL;

						_modified = true;
						k++;
					}
					else {
						other_data1->_data_long_long[r][v] = tmp_long1;
						delete[] other_data1->_data_string[r][v];
						other_data1->_data_string[r][v] = NULL;
					}
				}
			}

			// search in the other dataset
			if (other_data2 && other_data2->data_type == MEP_DATA_STRING) {
				long long tmp_long2;
				for (unsigned int r = 0; r < other_data2->num_data; r++) {
					if (!other_data2->_data_string[r][v]) {
						//other_data2->_data_long_long[r][v] = -1;
						continue;
					}
					if (!is_valid_long_long(other_data2->_data_string[r][v], &tmp_long2)) {
						// search for it in the current set
						for (unsigned int t = r + 1; t < other_data2->num_data; t++)
							if (other_data2->_data_string[t][v])
								if (my_strcmp(other_data2->_data_string[r][v], other_data2->_data_string[t][v]) == 0) {
									other_data2->_data_long_long[t][v] = k;
									delete[] other_data2->_data_string[t][v];
									other_data2->_data_string[t][v] = NULL;
								}

						other_data2->_data_long_long[r][v] = k;
						delete[] other_data2->_data_string[r][v];
						other_data2->_data_string[r][v] = NULL;

						_modified = true;
						k++;
					}
					else {
						other_data2->_data_long_long[r][v] = tmp_long2;
						delete[] other_data2->_data_string[r][v];
						other_data2->_data_string[r][v] = NULL;
					}
				}
			}
		}

		data_type = MEP_DATA_LONG_LONG;
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
			other_data1->data_type = MEP_DATA_LONG_LONG;
		}
		if (other_data2) {
			if (other_data2->_data_string) {
				for (unsigned int r = 0; r < other_data2->num_data; r++)
					delete[] other_data2->_data_string[r];
				delete[] other_data2->_data_string;
				other_data2->_data_string = NULL;
			}
			other_data2->data_type = MEP_DATA_LONG_LONG;
		}
		return E_MEP_DATA_OK;
	}
	else {
		return E_MEP_NO_DATA;
	}
}
//-----------------------------------------------------------------
