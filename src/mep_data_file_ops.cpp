// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include "mep_data.h"

#define MAX_ROW_CHARS 100000

//-----------------------------------------------------------------
bool my_fgets(char* buf, int buf_size, FILE* f)
{
	// I wrote this function because maybe strings on Windows and Mac are different
	int i = 0;
	buf[0] = 0;
	while (i + 1 < buf_size && !feof(f)) {
		buf[i] = getc(f);
		if (buf[i] == '\n') {
			buf[i] = 0;
			break;
		}
		else
			if (buf[i] == '\r') {
				// check what is next
				buf[i + 1] = getc(f);
				if (buf[i + 1] != '\n')
					ungetc(buf[i + 1], f); // put it back
				buf[i] = 0;
				break;
			}
		i++;
	}
	if (feof(f))
		if (i)
			buf[i - 1] = 0;
	return (i > 0);
}
//-----------------------------------------------------------------
bool t_mep_data::from_one_of_m_format(const char* filename, int given_num_classes)
{
	FILE* f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(filename, "r");
#endif

	if (!f)
		return false;

	delete_data();

	char* buf = new char[MAX_ROW_CHARS];
	char* start_buf = buf;

	num_data = 0;
	data_type = MEP_DATA_DOUBLE;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		size_t len = strlen(buf);
		if (len > 1)
			num_data++;
		if (num_data == 1) {
			num_cols = 0;

			char tmp_str[1000];
			int size;
			int skipped;
			bool result = get_next_field(buf, ' ', tmp_str, size, skipped);
			while (result) {
				num_cols++;
				if (!buf[size])
					break;
				buf = buf + size + skipped;
				result = get_next_field(buf, ' ', tmp_str, size, skipped);

			}
			buf = start_buf;
		}
	}
	num_cols -= given_num_classes;
	if (num_cols == 0)
		return false;
	if (given_num_classes)
		num_cols++;

	rewind(f);

	num_classes = given_num_classes;

	_data_double = new double* [num_data];

	int out_class;
	for (int r = 0; r < num_data; r++) {
		_data_double[r] = new double[num_cols];
		for (int c = 0; c < num_cols - 1; c++)
			fscanf(f, "%lf", &_data_double[r][c]);
		// now scan the outputs
		if (given_num_classes) // classification problem
			for (int c = 0; c < given_num_classes; c++) {
				fscanf(f, "%d", &out_class);
				if (out_class == 1)
					_data_double[r][num_cols - 1] = c;
			}
		else// regression problem
			fscanf(f, "%lf", &_data_double[r][num_cols - 1]);
	}

	fclose(f);
	delete[] buf;
	return true;
}
//-----------------------------------------------------------------
bool t_mep_data::from_csv(const char* filename)
{
	if (!detect_list_separator(filename))
		return false;

	_modified = true;

	from_csv_string(filename);
	// try to convert to double
	if (to_double()) {
		data_type = MEP_DATA_DOUBLE;
	}

	return true;
}
//-----------------------------------------------------------------
bool t_mep_data::from_csv_string(const char* filename)
{
	FILE* f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(filename, "r");
#endif

	if (!f)
		return false;
	delete_data();

	data_type = MEP_DATA_STRING;
	num_data = 0;
	num_cols = 0;

	get_csv_info_from_file(f, list_separator, this);

	fclose(f);

	remove_empty_rows();
	return true;
}
//-----------------------------------------------------------------
bool t_mep_data::from_csv_double(const char* filename)
{
	FILE* f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(filename, "r");
#endif

	if (!f)
		return false;

	delete_data();

	char* buf = new char[MAX_ROW_CHARS];
	char* start_buf = buf;

	num_data = 0;
	data_type = MEP_DATA_DOUBLE;
	char tmp_str[1000];

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		size_t len = strlen(buf);
		if (len > 1)
			num_data++;
		if (num_data == 1) {
			num_cols = 0;

			int size;
			int skipped;
			bool result = get_next_field(buf, list_separator, tmp_str, size, skipped);
			while (result) {
				num_cols++;
				if (!buf[size])
					break;
				buf = buf + size + skipped;
				result = get_next_field(buf, list_separator, tmp_str, size, skipped);

			}
			buf = start_buf;
		}
	}
	//	num_cols--;
	rewind(f);

	_data_double = new double* [num_data];
	int count_mep_data = 0;
	//char tmp_str[1000];

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		size_t len = strlen(buf);
		if (len > 1) {
			int col = 0;

			int size;
			int skipped;
			_data_double[count_mep_data] = new double[num_cols];
			bool result = get_next_field(buf, list_separator, tmp_str, size, skipped);
			while (result) {
				if (col < num_cols)
					_data_double[count_mep_data][col] = atof(tmp_str);
				else {
					break;
				}
				buf = buf + size + skipped;
				result = get_next_field(buf, list_separator, tmp_str, size, skipped);

				col++;
			}
			count_mep_data++;
		}
		buf = start_buf;
	}
	fclose(f);
	delete[] buf;
	return true;
}
//-----------------------------------------------------------------
bool t_mep_data::to_csv(const char* filename, char _list_separator)
{
	// this list separator is sent from outside

	FILE* f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"w");
	delete[] w_filename;

#else
	f = fopen(filename, "w");
#endif

	if (!f)
		return false;

	if (_data_double)
		for (int d = 0; d < num_data; d++) {
			for (int v = 0; v < num_cols; v++)
				fprintf(f, "%lg%c", _data_double[d][v], _list_separator);
			fprintf(f, "\n");
		}
	else
		if (_data_string)
			for (int d = 0; d < num_data; d++) {
				for (int v = 0; v < num_cols; v++) {
					if (strchr(_data_string[d][v], '\n') ||
						strchr(_data_string[d][v], '\r') ||
						strchr(_data_string[d][v], list_separator) ||
						strchr(_data_string[d][v], '"')) {

						size_t len = strlen(_data_string[d][v]);
						char* tmp_str = new char[2 + 2 * len + 1];
						size_t tmp_pos = 0;

						tmp_str[tmp_pos++] = '"';
						for (size_t i = 0; i < len; i++) {
							if (_data_string[d][v][i] == '"')
								tmp_str[tmp_pos++] = '"';

							tmp_str[tmp_pos++] = _data_string[d][v][i];
						}
						tmp_str[tmp_pos++] = '"';
						tmp_str[tmp_pos] = '\0';

						fprintf(f, "%s%c", tmp_str, _list_separator);

						delete[] tmp_str;
					}
					else
						fprintf(f, "%s%c", _data_string[d][v], _list_separator);
				}
				fprintf(f, "\n");
			}

	fclose(f);

	_modified = false;
	return true;
}
//-----------------------------------------------------------------
bool t_mep_data::detect_list_separator(const char* file_name)
{
	FILE* f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, file_name, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, file_name, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(file_name, "r");
#endif

	if (!f)
		return false;

	char* buf = new char[MAX_ROW_CHARS];

	my_fgets(buf, MAX_ROW_CHARS, f);

	while (!feof(f)){
		if (strlen(buf) > 0)// lines with at least one character
			break;
		my_fgets(buf, MAX_ROW_CHARS, f);
	}

	if (feof(f)){// nothing in the file
		delete[] buf;
		return false;
	}

// detect the ;
	if (strchr(buf, ';')) {
		list_separator = ';';
		delete[] buf;
		return true;
	}
	if (strchr(buf, ',')) {
		list_separator = ',';
		delete[] buf;
		return true;
	}
	if (strchr(buf, ' ')) {
		list_separator = ' ';
		delete[] buf;
		return true;
	}
	if (strchr(buf, '\t')) {
		list_separator = '\t';
		delete[] buf;
		return true;
	}

	// most likely there is only variable and no output
	/*
	double x;
	if (sscanf(buf, "%lf", &x) == 1) {
		list_separator = ';';
		delete[] buf;
		return true;
	}
	*/

	delete[] buf;
	return true;
}
//-----------------------------------------------------------------
/*
bool t_mep_data::from_csv_string_old(const char* filename)
{
	FILE* f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(filename, "r");
#endif

	if (!f)
		return false;

	delete_data();

	char* buf = new char[MAX_ROW_CHARS];
	char* start_buf = buf;

	data_type = MEP_DATA_STRING;
	num_data = 0;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		size_t len = strlen(buf);
		if (len > 1)
			num_data++;
		if (num_data == 1) {
			num_cols = 0;
			int skipped;

			char tmp_str[1000];
			int size;
			bool result = get_next_field(buf, list_separator, tmp_str, size, skipped);
			while (result) {
				num_cols++;
				if (!buf[size + skipped])
					break;
				buf = buf + size + skipped;
				result = get_next_field(buf, list_separator, tmp_str, size, skipped);
			}
			buf = start_buf;
		}
		}
	//	num_cols--;
	rewind(f);

	_data_string = new char** [num_data];
	int count_mep_data = 0;
	//has_missing_values = 0;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		size_t len = strlen(buf);
		if (len > 1) {
			int col = 0;
			char tmp_str[1000];
			int size;
			_data_string[count_mep_data] = new char* [num_cols];
			for (int c = 0; c < num_cols; c++)
				_data_string[count_mep_data][col] = NULL;

			int skipped = 0;
			bool result = get_next_field(buf, list_separator, tmp_str, size, skipped);
			while (result) {
				if (col < num_cols) {
					_data_string[count_mep_data][col] = new char[strlen(tmp_str) + 1];
					strcpy(_data_string[count_mep_data][col], tmp_str);
				}
				else {
					break;
				}
				buf = buf + size + skipped;
				//if (buf - start_buf >= len)
				//break;
				result = get_next_field(buf, list_separator, tmp_str, size, skipped);

				col++;
			}
			count_mep_data++;
		}
		buf = start_buf;
	}
	fclose(f);
	//delete[] start_buf;
	delete[] buf;

	return true;
}
//-----------------------------------------------------------------
*/
