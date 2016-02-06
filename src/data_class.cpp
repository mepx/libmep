#include "data_class.h"
#include "utils_mep.h"
#include "my_rand.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <cstring>
#include <regex>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#define MAX_ROW_CHARS 1000000

char possible_chars[] = "abcdfghijklmnopqrstyvwxyzABCDFGHIJKLMNOPQRSTUVWXYZ!$%^&*()_={}[]~#<>?/|";
//-----------------------------------------------------------------
t_data::t_data(void)
{
	init();
}
//-----------------------------------------------------------------
t_data::~t_data()
{
	delete_data();
}
//-----------------------------------------------------------------
void t_data::delete_double_data(void)
{
	if (_data_double) {
		for (int i = 0; i < num_data; i++)
			delete[] _data_double[i];
		delete[] _data_double;
		_data_double = NULL;
	}
}
//-----------------------------------------------------------------
void t_data::delete_string_data(void)
{
	if (_data_string) {
		for (int i = 0; i < num_data; i++) {
			for (int v = 0; v < num_cols; v++)
				if (_data_string[i][v])
					delete[] _data_string[i][v];
			delete[] _data_string[i];
		}
		delete[] _data_string;
		_data_string = NULL;
	}
}
//-----------------------------------------------------------------
void t_data::delete_data(void)
{
	delete_double_data();
	delete_string_data();
}
//-----------------------------------------------------------------
void t_data::clear_data(void)
{
	delete_data();
	num_data = 0;
	num_cols = 0;
	data_type = MEP_DATA_DOUBLE;// double
	num_targets = 1;
}
//-----------------------------------------------------------------
void t_data::init(void)
{
	num_data = 0;
	num_cols = 0;

	_data_double = NULL;

	_data_string = NULL;

	list_separator = ' ';

	data_type = MEP_DATA_DOUBLE;// double
	num_targets = 1;

}
//-----------------------------------------------------------------
bool get_next_field(char *start_sir, char list_separator, char* dest, int & size, int &skipped)
{
	skipped = 0;
	char *tmp_start = start_sir;
	while (*tmp_start && (*tmp_start == ' ' || *tmp_start == '\t' || *tmp_start == list_separator)) {
		tmp_start++;
		skipped++;
	}

	size = 0;
	while (tmp_start[size] && (tmp_start[size] != list_separator) && (tmp_start[size] != '\n')) {
		size++;
	}
	if (!size && !tmp_start[size])
		return false;
	strncpy(dest, tmp_start, size);
	dest[size] = '\0';
	return true;
}
// ---------------------------------------------------------------------------
int t_data::from_xml(pugi::xml_node parent)
{
	clear_data();

	pugi::xml_node node = parent.child("num_data");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_data = atoi(value_as_cstring);
	}
	else
		num_data = 0;

	node = parent.child("num_variables");
	if (node) {
		const char *value_as_cstring = node.child_value();
		int num_variables = atoi(value_as_cstring);
		if (num_variables)
			num_cols = num_variables + 1;
		else
			num_cols = 0;
	}
	else{
		node = parent.child("num_cols");
		if (node) {
			const char *value_as_cstring = node.child_value();
			num_cols = atoi(value_as_cstring);
		}
		else
			num_cols = 0;
	}

	node = parent.child("num_targets");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_targets = atoi(value_as_cstring);
	}
	else
		num_targets = 1;

	node = parent.child("data_type");
	if (node) {
		const char *value_as_cstring = node.child_value();
		data_type = atoi(value_as_cstring);
	}
	else
		data_type = MEP_DATA_DOUBLE;// double by default
	/*
	node = parent.child("has_missing_values");
	if (node) {
	const char *value_as_cstring = node.child_value();
	has_missing_values = atoi(value_as_cstring);
	}
	else
	has_missing_values = 0;// double by default
	*/
	node = parent.child("list_separator");
	if (node) {
		const char *value_as_cstring = node.child_value();
		list_separator = value_as_cstring[0];
	}
	else
		list_separator = ' ';// blank space by default

	if (data_type == MEP_DATA_DOUBLE) {
		if (num_data) {
			_data_double = new double*[num_data];
			for (int r = 0; r < num_data; r++)
				_data_double[r] = new double[num_cols];
		}
	}
	else {
		if (num_data) {
			_data_string = new char**[num_data];
			for (int r = 0; r < num_data; r++) {
				_data_string[r] = new char*[num_cols];
				for (int v = 0; v < num_cols; v++)
					_data_string[r][v] = NULL;
			}
		}
	}

	pugi::xml_node node_data = parent.child("data");
	if (!node_data)
		return true;
	int r = 0;
	if (data_type == MEP_DATA_DOUBLE) {// double
		for (pugi::xml_node row = node_data.child("row"); row; row = row.next_sibling("row"), r++) {
			const char *value_as_cstring = row.child_value();
			int num_jumped_chars = 0;

			for (int c = 0; c < num_cols; c++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &_data_double[r][c]);
				long local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
	}
	else {// string
		for (pugi::xml_node row = node_data.child("row"); row; row = row.next_sibling("row"), r++) {
			const char *value_as_cstring = row.child_value();
			char *buf = (char*)value_as_cstring;
			char tmp_str[1000];
			int size;
			int c = 0;
			int skipped;

			bool result = get_next_field(buf, ' ', tmp_str, size, skipped);
			while (result) {
				if (c < num_cols) {
					_data_string[r][c] = new char[strlen(tmp_str) + 1];
					strcpy(_data_string[r][c], tmp_str);
				}
				buf = buf + size + 1 + skipped;
				//if (buf - start_buf >= len)
				//	break;
				result = get_next_field(buf, ' ', tmp_str, size, skipped);

				c++;
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------
int t_data::to_xml(pugi::xml_node parent)
{
	char *tmp_str = new char[MAX_ROW_CHARS];

	pugi::xml_node node = parent.append_child("num_data");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", num_data);
	data.set_value(tmp_str);

	node = parent.append_child("num_cols");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", num_cols);
	data.set_value(tmp_str);

	node = parent.append_child("num_targets");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", num_targets);
	data.set_value(tmp_str);

	node = parent.append_child("data_type");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", data_type);
	data.set_value(tmp_str);

	node = parent.append_child("list_separator");
	data = node.append_child(pugi::node_pcdata);
	tmp_str[0] = list_separator;
	tmp_str[1] = 0;
	data.set_value(tmp_str);

	if (!num_data || !num_cols) {
		delete[] tmp_str;
		return true;
	}

	for (int c = 0; c < num_cols; c++) {
	}

	pugi::xml_node node_data = parent.append_child("data");

	if (_data_double)
		for (int r = 0; r < num_data; r++) {
			node = node_data.append_child("row");
			data = node.append_child(pugi::node_pcdata);
			tmp_str[0] = 0;
			for (int c = 0; c < num_cols; c++) {
				char tmp_s[30];
				sprintf(tmp_s, "%lg", this->_data_double[r][c]);
				strcat(tmp_str, tmp_s);
				strcat(tmp_str, " ");
			}
			//now the target if there is one...
			data.set_value(tmp_str);
		}
	else
		if (_data_string) {
			for (int r = 0; r < num_data; r++) {
				node = node_data.append_child("row");
				data = node.append_child(pugi::node_pcdata);
				tmp_str[0] = 0;
				for (int c = 0; c < num_cols; c++) {
					strcat(tmp_str, _data_string[r][c]);
					strcat(tmp_str, " ");
				}
				//now the target if there is one...

				data.set_value(tmp_str);
			}
		}

	delete[] tmp_str;
	return true;
}
//-----------------------------------------------------------------
bool my_fgets(char*buf, int max_n, FILE *f)
{
	int i = 0;
	buf[0] = 0;
	while (!feof(f)) {
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
//------------------------------------------------------------
bool t_data::to_csv(const char *filename, char list_separator)
{
	FILE *f = NULL;
#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"w");
	delete[] w_filename;

#else
	f = fopen(filename, "w");
#endif

	if (!f)
		return false;
	for (int d = 0; d < num_data; d++) {
		for (int v = 0; v < num_cols; v++)
			fprintf(f, "%lg%c", _data_double[d][v], list_separator);
	}

	fclose(f);
	return true;
}
//-----------------------------------------------------------------
bool t_data::detect_list_separator(const char *file_name)
{
	FILE *f = NULL;
#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, file_name, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, file_name, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(file_name, "r");
#endif

	if (!f)
		return false;

	char *buf = new char[MAX_ROW_CHARS];

	my_fgets(buf, MAX_ROW_CHARS, f);
	if (strlen(buf) < 1) {
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

	double x;
	if (sscanf(buf, "%lf", &x) == 1) {// most likely there is only variable and no output
		list_separator = ' ';
		delete[] buf;
		return true;
	}

	delete[] buf;
	return false;
}
//-----------------------------------------------------------------
void t_data::count_0_class(int target_col)
{
	num_class_0 = 0;
	for (int i = 0; i < num_data; i++)
		if (fabs(_data_double[i][target_col]) < 1e-6)
			num_class_0++;
}
//-----------------------------------------------------------------
bool t_data::from_csv(const char *filename) // extra_variable is used by test data when we are not sure if the test data has or not target
{

	if (!detect_list_separator(filename))
		return false;

	FILE *f = NULL;
#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(filename, "r");
#endif

	if (!f)
		return false;

	char buf[1000];
	// lets see what is there
	fgets(buf, 1000, f);
	fclose(f);

	if (strpbrk(buf, "abcdfghijklmnopqrstyvwxyzABCDFGHIJKLMNOPQRSTUVWXYZ!$%^&*()_={}[]~#<>?/|"))
		return from_csv_string(filename);
	else
		return from_csv_double(filename);

}
//-----------------------------------------------------------------
bool t_data::from_csv_string(const char *filename) // extra_variable is used by test data when we are not sure if the test data has or not target
{
	FILE *f = NULL;
#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(filename, "r");
#endif

	if (!f)
		return false;

	delete_data();

	char *buf = new char[MAX_ROW_CHARS];
	char * start_buf = buf;

	data_type = MEP_DATA_STRING;
	num_data = 0;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		long len = strlen(buf);
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

	_data_string = new char**[num_data];
	int count_data = 0;
	//has_missing_values = 0;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		long len = strlen(buf);
		if (len > 1) {
			int col = 0;
			char tmp_str[1000];
			int size;
			_data_string[count_data] = new char*[num_cols];
			for (int c = 0; c < num_cols; c++)
				_data_string[count_data][col] = NULL;

			int skipped = 0;
			bool result = get_next_field(buf, list_separator, tmp_str, size, skipped);
			while (result) {
				if (col < num_cols) {
					_data_string[count_data][col] = new char[strlen(tmp_str) + 1];
					strcpy(_data_string[count_data][col], tmp_str);
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
			count_data++;
		}
		buf = start_buf;
	}
	fclose(f);
	//delete[] start_buf;
	delete[] buf;
	return true;
}
//-----------------------------------------------------------------
bool t_data::from_csv_double(const char *filename) // extra_variable is used by test data when we are not sure if the test data has or not target
{
	FILE *f = NULL;
#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"r");

	delete[] w_filename;
#else
	f = fopen(filename, "r");
#endif

	if (!f)
		return false;

	delete_data();

	char *buf = new char[MAX_ROW_CHARS];
	char * start_buf = buf;

	num_data = 0;
	data_type = MEP_DATA_DOUBLE;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		long len = strlen(buf);
		if (len > 1)
			num_data++;
		if (num_data == 1) {
			num_cols = 0;

			char tmp_str[1000];
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

	_data_double = new double*[num_data];
	int count_data = 0;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		long len = strlen(buf);
		if (len > 1) {
			int col = 0;
			char tmp_str[MAX_ROW_CHARS];
			int size;
			int skipped;
			_data_double[count_data] = new double[num_cols];
			bool result = get_next_field(buf, list_separator, tmp_str, size, skipped);
			while (result) {
				if (col < num_cols)
					_data_double[count_data][col] = atof(tmp_str);
				else {
					break;
				}
				buf = buf + size + skipped;
				result = get_next_field(buf, list_separator, tmp_str, size, skipped);

				col++;
			}
			count_data++;
		}
		buf = start_buf;
	}
	fclose(f);
	delete[] buf;
	return true;
}
//-----------------------------------------------------------------
void t_data::to_numeric(void)
{
	if (num_data) {
		int *index_new_strings = new int[num_data];
		//		int count_new_strings = 0;
		if (data_type == MEP_DATA_STRING) {// string
			if (_data_string) {

				delete_double_data();

				_data_double = new double*[num_data];
				for (int r = 0; r < num_data; r++)
					_data_double[r] = new double[num_cols];

				for (int v = 0; v < num_cols; v++) {
					//is this numeric or alpha ?
					bool is_numeric = true;
					for (int t = 0; t < num_data; t++) {
						if (strpbrk(_data_string[t][v], "abcdfghijklmnopqrstyvwxyzABCDFGHIJKLMNOPQRSTUVWXYZ!$%^&*()_={}[]~#<>?/|")) {
							is_numeric = false;
							break;
						}
					}
					if (!is_numeric) {
						int count_new_strings = 0;
						for (int t = 0; t < num_data; t++) {
							// caut sa vad daca exista
							int k = 0;
							while (k < count_new_strings && my_strcmp(_data_string[index_new_strings[k]][v], _data_string[t][v])) {
								k++;
							}
							_data_double[t][v] = k;
							if (k == count_new_strings) {
								// not found
								index_new_strings[count_new_strings] = t;
								count_new_strings++;
							}
						}
					}
					else {
						// numeric - must be copy just like that
						for (int t = 0; t < num_data; t++)
							_data_double[t][v] = atof(_data_string[t][v]);
					}

				}
			}
		}

		delete[] index_new_strings;
	}
	data_type = MEP_DATA_DOUBLE;
}
//-----------------------------------------------------------------
int t_data::to_interval_everywhere(double min, double max)
{
    int result = to_interval_all_variables(min, max);
	if (!result)
		return 0;
	if (num_targets)
      return to_interval_selected_col(min, max, num_cols - 1);
	return 1;
}
//-----------------------------------------------------------------
int t_data::to_interval_selected_col(double min, double max, int col)
{
	if (num_data){
		if (data_type == MEP_DATA_DOUBLE) {// double


			double min_col = _data_double[0][col];
			double max_col = _data_double[0][col];

			for (int t = 0; t < num_data; t++) {
				if (min_col > _data_double[t][col])
					min_col = _data_double[t][col];
				if (max_col < _data_double[t][col])
					max_col = _data_double[t][col];
			}

			for (int t = 0; t < num_data; t++) {
				_data_double[t][col] -= min_col;
				if (max_col > min_col)
					_data_double[t][col] /= (max_col - min_col);
				_data_double[t][col] *= (max - min);
				_data_double[t][col] += min;
			}
			return 1;
		}
		else
			return 0;
    }
	return 1;
}
//-----------------------------------------------------------------
int t_data::to_interval_all_variables(double min, double max)
{
	if (num_data){
		if (data_type == MEP_DATA_DOUBLE) {// double
			for (int v = 0; v < num_cols; v++) {
				//is this numeric or alpha ?

				double min_col = _data_double[0][v];
				double max_col = _data_double[0][v];

				for (int t = 0; t < num_data; t++) {
					if (min_col > _data_double[t][v])
						min_col = _data_double[t][v];
					if (max_col < _data_double[t][v])
						max_col = _data_double[t][v];
				}

				for (int t = 0; t < num_data; t++) {
					_data_double[t][v] -= min_col;
					if (max_col > min_col)
						_data_double[t][v] /= (max_col - min_col);
					_data_double[t][v] *= (max - min);
					_data_double[t][v] += min;
				}
			}
			return 1;
		}
		else
			return 0;
    }
	return 1;
}
//-----------------------------------------------------------------
int t_data::move_to(t_data *dest, int count)
{
	if (!(data_type == dest->data_type || !dest->num_data))
		return 1; // can move only of the same type
	if (num_data >= count) {

        if (dest->num_data == 0) {
			dest->num_cols = num_cols;
			dest->num_targets = num_targets;
		}

		if (data_type == MEP_DATA_DOUBLE) {// double

			double** tmp_data_double = new double*[dest->num_data + count];
			for (int i = 0; i < dest->num_data; i++)
				tmp_data_double[i] = dest->_data_double[i];

			for (int i = 0; i < count; i++)
				tmp_data_double[i + dest->num_data] = _data_double[num_data - count + i];

			if (dest->_data_double)
				delete[] dest->_data_double;
			dest->_data_double = tmp_data_double;

			if (num_data - count > 0) {
				tmp_data_double = new double*[num_data - count];
				for (int i = 0; i < num_data - count; i++)
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
			for (int i = 0; i < dest->num_data; i++)
				tmp_data_string[i] = dest->_data_string[i];

			for (int i = 0; i < count; i++)
				tmp_data_string[i + dest->num_data] = _data_string[num_data - count + i];

			if (dest->_data_string)
				delete[] dest->_data_string;
			dest->_data_string = tmp_data_string;

			if (num_data - count > 0) {
				tmp_data_string = new char**[num_data - count];
				for (int i = 0; i < num_data - count; i++)
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
	return 0;
}
//-----------------------------------------------------------------
bool re_match(char *str, const char *pattern, bool use_regular)
{
	if (!use_regular)
		return !strcmp(pattern, str);
	else{
		std::regex re_pattern(pattern, std::regex::basic | std::regex::icase);
		if (std::regex_match(str, re_pattern))
				return true;
			else
				return false;
		}
}
//-----------------------------------------------------------------
int t_data::replace_symbol_from_selected_col(const char *s_find_what, const char* s_replace_with, int col, bool use_regular)
{
	int count_replaced = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (int t = 0; t < num_data; t++)
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

			for (int t = 0; t < num_data; t++)
				if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
					// this is a missing value
					_data_double[t][col] = d_replace_with;
					count_replaced++;
				}
		}
	return count_replaced;
}
//-----------------------------------------------------------------
int  t_data::replace_symbol_everywhere(const char *s_find_what, const char* s_replace_with, bool use_regular)
{
    int tmp_count;
	tmp_count = replace_symbol_from_all_variables(s_find_what, s_replace_with, use_regular);
    int count_replaced = tmp_count;
	tmp_count = replace_symbol_from_selected_col(s_find_what, s_replace_with, num_cols - 1, use_regular);
    count_replaced += tmp_count;

	return count_replaced;
}
//-----------------------------------------------------------------
int  t_data::replace_symbol_from_all_variables(const char *s_find_what, const char* s_replace_with, bool use_regular)
{
	int count_replaced = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (int col = 0; col < num_cols - 1; col++)
			for (int t = 0; t < num_data; t++)
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

			for (int col = 0; col < num_cols - 1; col++)
				for (int t = 0; t < num_data; t++)
					if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
						// this is a missing value
						_data_double[t][col] = d_replace_with;
						count_replaced++;
					}
		}
	return count_replaced;
}
//-----------------------------------------------------------------
int t_data::find_symbol_from_selected_col(const char *s_find_what, int col, bool use_regular)
{
	int count_found = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (int t = 0; t < num_data; t++)
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

			for (int t = 0; t < num_data; t++)
				if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
					// this is a missing value
					count_found++;
				}
		}

	return count_found;
}
//-----------------------------------------------------------------
int t_data::find_symbol_everywhere(const char *s_find_what, bool use_regular)
{
	int tmp_count;
	tmp_count = find_symbol_from_all_variables(s_find_what, use_regular);
	int count_found = tmp_count;
	tmp_count = find_symbol_from_selected_col(s_find_what, num_cols - 1, use_regular);
	count_found += tmp_count;

	return count_found;
}
//-----------------------------------------------------------------
int t_data::find_symbol_from_all_variables(const char *s_find_what, bool use_regular)
{
	int count_found;
	count_found = 0;
	if (data_type == MEP_DATA_STRING) { // string
		for (int col = 0; col < num_cols - 1; col++)
			for (int t = 0; t < num_data; t++)
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

			for (int col = 0; col < num_cols - 1; col++)
				for (int t = 0; t < num_data; t++)
					if (fabs(_data_double[t][col] - d_find_what) < 1E-10) {
						// this is a missing value
						count_found++;
					}
		}

	return count_found;
}
//-----------------------------------------------------------------
void t_data::shuffle(void)
{
	if (data_type == MEP_DATA_DOUBLE) { // double
		for (int i = num_data - 1; i >= 1; i--){
			int j = my_rand() % (i + 1);
			double *row = _data_double[i];
			_data_double[i] = _data_double[j];
			_data_double[j] = row;
		}
	}
	else{
		// string
		for (int i = num_data - 1; i >= 1; i--){
			int j = my_rand() % (i + 1);
			char**row = _data_string[i];
			_data_string[i] = _data_string[j];
			_data_string[j] = row;
		}
	}
}
//-----------------------------------------------------------------
bool t_data::is_classification_problem(void)
{
	for (int i = 0; i < num_data; i++)
		if (fabs(_data_double[i][num_cols - 1]) > 1E-6 &&  fabs(_data_double[i][num_cols - 1] - 1.0) > 1E-6)
			return false;
	return true;
}
//-----------------------------------------------------------------
