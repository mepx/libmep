// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
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
#include "utils/validation_string.h"
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
int t_mep_data::replace_symbol_from_selected_col(const char *s_find_what,
												 const char* s_replace_with,
												 unsigned int col,
												 bool use_regular)
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
		else
			if (data_type == MEP_DATA_LONG_LONG) { // double
				// try to convert them to long long
				char* pEnd;
				long long d_find_what;
				d_find_what = strtoll(s_find_what, &pEnd, 10);

				long long d_replace_with;
				d_replace_with = strtoll(s_replace_with, &pEnd, 10);

				for (unsigned int t = 0; t < num_data; t++)
					if (_data_long_long[t][col] == d_find_what) {
						// this is a missing value
						_data_long_long[t][col] = d_replace_with;
						count_replaced++;
					}
			}
	_modified = true;
	return count_replaced;
}
//-----------------------------------------------------------------
int  t_mep_data::replace_symbol_everywhere(const char *s_find_what,
										   const char* s_replace_with,
										   bool use_regular)
{
	int count_replaced = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (unsigned int col = 0; col < num_cols; col++)
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

			for (unsigned int col = 0; col < num_cols; col++)
				for (unsigned int t = 0; t < num_data; t++)
					if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
						// this is a missing value
						_data_double[t][col] = d_replace_with;
						count_replaced++;
					}
		}
		else
			if (data_type == MEP_DATA_LONG_LONG) { // double
				// try to convert them to long long
				char* pEnd;
				long long d_find_what;
				d_find_what = strtoll(s_find_what, &pEnd, 10);

				long long d_replace_with;
				d_replace_with = strtoll(s_replace_with, &pEnd, 10);

				for (unsigned int col = 0; col < num_cols; col++)
					for (unsigned int t = 0; t < num_data; t++)
						if (_data_long_long[t][col] == d_find_what) {
							// this is a missing value
							_data_long_long[t][col] = d_replace_with;
							count_replaced++;
						}
			}
	_modified = true;
	return count_replaced;
}
//-----------------------------------------------------------------
/*
int  t_mep_data::replace_symbol_from_all_variables(const char *s_find_what,
												   const char* s_replace_with,
												   bool use_regular)
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
		else
			if (data_type == MEP_DATA_LONG_LONG) { // double
				// try to convert them to long long
				char* pEnd;
				long long d_find_what;
				d_find_what = strtoll(s_find_what, &pEnd, 10);

				long long d_replace_with;
				d_replace_with = strtoll(s_replace_with, &pEnd, 10);

				for (unsigned int col = 0; col < num_cols - 1; col++)
					for (unsigned int t = 0; t < num_data; t++)
						if (_data_long_long[t][col] == d_find_what) {
							// this is a missing value
							_data_long_long[t][col] = d_replace_with;
							count_replaced++;
						}
			}
	_modified = true;
	return count_replaced;
}
*/
//-----------------------------------------------------------------
int t_mep_data::find_symbol_from_selected_col(const char *s_find_what,
											  unsigned int col,
											  bool use_regular)
{
	int count_found = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (unsigned int t = 0; t < num_data; t++)
			if (re_match(_data_string[t][col], s_find_what, use_regular)) {
				// this is a missing value
				count_found++;
			}
	}
	else{
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
		else{
			if (data_type == MEP_DATA_LONG_LONG) { // double
				// try to convert them to double
				char* pEnd;
				long long d_find_what;
				d_find_what = strtoll(s_find_what, &pEnd, 10);

				for (unsigned int t = 0; t < num_data; t++)
					if (_data_long_long[t][col] == d_find_what) {
						// this is a missing value
						count_found++;
					}
			}
		}
	}
	_modified = true;
	return count_found;
}
//-----------------------------------------------------------------
int t_mep_data::find_symbol_everywhere(const char *s_find_what, bool use_regular)
{
	int count_found;
	count_found = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (unsigned int col = 0; col < num_cols; col++)
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

			for (unsigned int col = 0; col < num_cols; col++)
				for (unsigned int t = 0; t < num_data; t++)
					if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
						// this is a missing value
						count_found++;
					}
		}
		else
			if (data_type == MEP_DATA_LONG_LONG) { // double
				// try to convert them to double
				char* pEnd;
				long long d_find_what;
				d_find_what = strtoll(s_find_what, &pEnd, 10);

				for (unsigned int col = 0; col < num_cols; col++)
					for (unsigned int t = 0; t < num_data; t++)
						if (_data_long_long[t][col] == d_find_what) {
							// this is a missing value
							count_found++;
						}
			}

	return count_found;

}
//-----------------------------------------------------------------
/*
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
		else
			if (data_type == MEP_DATA_LONG_LONG) { // double
				// try to convert them to double
				char* pEnd;
				long long d_find_what;
				d_find_what = strtoll(s_find_what, &pEnd, 10);

				for (unsigned int col = 0; col < num_cols - 1; col++)
					for (unsigned int t = 0; t < num_data; t++)
						if (_data_long_long[t][col] == d_find_what) {
							// this is a missing value
							count_found++;
						}
			}

	return count_found;
}
//-----------------------------------------------------------------
*/
