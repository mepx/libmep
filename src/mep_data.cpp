#include "mep_data.h"
#include "mep_utils.h"
#include "mep_rands.h"

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
		for (int i = 0; i < num_data; i++)
			delete[] _data_double[i];
		delete[] _data_double;
		_data_double = NULL;
	}
}
//-----------------------------------------------------------------
void t_mep_data::delete_string_data(void)
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
void t_mep_data::delete_data(void)
{
	delete_double_data();
	delete_string_data();
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

	list_separator = ' ';

	data_type = MEP_DATA_DOUBLE;// double
	num_outputs = 1;

	num_classes = 0;

	_modified = false;
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
int t_mep_data::from_xml(pugi::xml_node parent)
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
	else {
		node = parent.child("num_cols");
		if (node) {
			const char *value_as_cstring = node.child_value();
			num_cols = atoi(value_as_cstring);
		}
		else
			num_cols = 0;
	}

	node = parent.child("num_outputs");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_outputs = atoi(value_as_cstring);
	}
	else
		num_outputs = 1;

    
    node = parent.child("num_classes");
    if (node) {
        const char *value_as_cstring = node.child_value();
        num_classes = atoi(value_as_cstring);
    }
    else
        num_classes = 0;
    
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

	_modified = false;
	return true;
}
//-----------------------------------------------------------------
int t_mep_data::to_xml(pugi::xml_node parent)
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

	node = parent.append_child("num_outputs");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", num_outputs);
	data.set_value(tmp_str);
    
    node = parent.append_child("num_classes");
    data = node.append_child(pugi::node_pcdata);
    sprintf(tmp_str, "%d", num_classes);
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
	_modified = false;
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
bool t_mep_data::to_csv(const char *filename, char list_separator)
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

	if (_data_double)
		for (int d = 0; d < num_data; d++) {
			for (int v = 0; v < num_cols; v++)
				fprintf(f, "%lg%c", _data_double[d][v], list_separator);
			fprintf(f, "\n");
		}
	else
		if (_data_string)
			for (int d = 0; d < num_data; d++) {
				for (int v = 0; v < num_cols; v++)
					fprintf(f, "%s%c", _data_string[d][v], list_separator);
				fprintf(f, "\n");
			}

	fclose(f);

	_modified = false;
	return true;
}
//-----------------------------------------------------------------
bool t_mep_data::detect_list_separator(const char *file_name)
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
void t_mep_data::count_0_class(int target_col)
{
	num_class_0 = 0;
	for (int i = 0; i < num_data; i++)
		if (fabs(_data_double[i][target_col]) < 1e-6)
			num_class_0++;
}
//-----------------------------------------------------------------
bool t_mep_data::from_PROBEN1_format(const char *filename, int num_classes)
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
	num_cols -= num_classes;
	if (num_classes)
	  num_cols++;

	rewind(f);

	_data_double = new double*[num_data];

	int out_class;
	for (int r = 0; r < num_data; r++) {
		_data_double[r] = new double[num_cols];
		for (int c = 0; c < num_cols - 1; c++)
			fscanf(f, "%lf", &_data_double[r][c]);
		// now scan the outputs
		if (num_classes) // classification problem
		for (int c = 0; c < num_classes; c++) {
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

bool t_mep_data::from_csv(const char *filename) 
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

	_modified = true;

	if (strpbrk(buf, "abcdfghijklmnopqrstyvwxyzABCDFGHIJKLMNOPQRSTUVWXYZ!$%^&*()_={}[]~#<>?/|"))
		return from_csv_string(filename);
	else
		return from_csv_double(filename);

}
//-----------------------------------------------------------------
bool t_mep_data::from_csv_string(const char *filename) 
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
	int count_mep_data = 0;
	//has_missing_values = 0;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		long len = strlen(buf);
		if (len > 1) {
			int col = 0;
			char tmp_str[1000];
			int size;
			_data_string[count_mep_data] = new char*[num_cols];
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
bool t_mep_data::from_csv_double(const char *filename) 
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
	int count_mep_data = 0;

	while (my_fgets(buf, MAX_ROW_CHARS, f)) {
		long len = strlen(buf);
		if (len > 1) {
			int col = 0;
			char tmp_str[MAX_ROW_CHARS];
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
int t_mep_data::to_numeric(t_mep_data *other_data1, t_mep_data* other_data2)
{
	if (num_data || other_data1 && other_data1->num_data || other_data2 && other_data2->num_data) {

		//		int count_new_strings = 0;
		if (data_type != MEP_DATA_STRING && (!other_data1 || other_data1 && other_data1->data_type != MEP_DATA_STRING) && (other_data2 || other_data2 && other_data2->data_type != MEP_DATA_STRING))
			return E_DATA_MUST_HAVE_STRING_TYPE;

		if (_data_string) {
			delete_double_data();

			_data_double = new double*[num_data];
			for (int r = 0; r < num_data; r++)
				_data_double[r] = new double[num_cols];
		}

		if (other_data1 && other_data1->_data_string) {

			other_data1->delete_double_data();

			other_data1->_data_double = new double*[other_data1->num_data];
			for (int r = 0; r < other_data1->num_data; r++)
				other_data1->_data_double[r] = new double[other_data1->num_cols];
		}

		if (other_data2 && other_data2->_data_string) {

			other_data2->delete_double_data();

			other_data2->_data_double = new double*[other_data2->num_data];
			for (int r = 0; r < other_data2->num_data; r++)
				other_data2->_data_double[r] = new double[other_data2->num_cols];
		}

		int k = 0; // this will be the replacement
		for (int v = 0; v < num_cols; v++) {
			//is this numeric or alpha ?
			// search in the current dataset
			for (int r = 0; r < num_data; r++)
				if (strpbrk(_data_string[r][v], "abcdfghijklmnopqrstyvwxyzABCDFGHIJKLMNOPQRSTUVWXYZ!$%^&*()_={}[]~#<>?/|")) {
					// search for it in the current set
					for (int t = r; t < num_data; t++)
						if (!my_strcmp(_data_string[r][v], _data_string[t][v]))
							_data_double[t][v] = k;
					// replace it in the other datasets too
					if (other_data1 && other_data1->data_type == MEP_DATA_STRING)
						for (int t = 0; t < other_data1->num_data; t++)
							if (!my_strcmp(_data_string[r][v], other_data1->_data_string[t][v]))
								other_data1->_data_double[t][v] = k;
					// replace it in the other datasets too
					if (other_data2 && other_data2->data_type == MEP_DATA_STRING)
						for (int t = 0; t < other_data2->num_data; t++)
							if (!my_strcmp(_data_string[r][v], other_data2->_data_string[t][v]))
								other_data2->_data_double[t][v] = k;
					_modified = true;
					k++;
				}
				else 
					// numeric - must be copy just like that
					_data_double[r][v] = atof(_data_string[r][v]);
				
			// search in the other dataset
			if (other_data1 && other_data1->data_type == MEP_DATA_STRING)
				for (int r = 0; r < other_data1->num_data; r++)
					if (strpbrk(other_data1->_data_string[r][v], "abcdfghijklmnopqrstyvwxyzABCDFGHIJKLMNOPQRSTUVWXYZ!$%^&*()_={}[]~#<>?/|")) {
					// search for it in the current set
						for (int t = r; t < other_data1->num_data; t++)
							if (!my_strcmp(other_data1->_data_string[r][v], other_data1->_data_string[t][v]))
								other_data1->_data_double[t][v] = k;
					// replace it in the other datasets too
					if (other_data2 && other_data2->data_type == MEP_DATA_STRING)
						for (int t = 0; t < other_data2->num_data; t++)
							if (!my_strcmp(other_data1->_data_string[r][v], other_data2->_data_string[t][v]))
								other_data2->_data_double[t][v] = k;
					_modified = true;
					k++;
				}
				else
					// numeric - must be copy just like that
					other_data1->_data_double[r][v] = atof(other_data1->_data_string[r][v]);

			// search in the other dataset
			if (other_data2 && other_data2->data_type == MEP_DATA_STRING)
				for (int r = 0; r < other_data2->num_data; r++)
					if (strpbrk(other_data2->_data_string[r][v], "abcdfghijklmnopqrstyvwxyzABCDFGHIJKLMNOPQRSTUVWXYZ!$%^&*()_={}[]~#<>?/|")) {
						// search for it in the current set
						for (int t = r; t < other_data2->num_data; t++)
							if (!my_strcmp(other_data2->_data_string[r][v], other_data2->_data_string[t][v]))
								other_data2->_data_double[t][v] = k;
						_modified = true;
						k++;
					}
					else
						// numeric - must be copy just like that
						other_data2->_data_double[r][v] = atof(other_data2->_data_string[r][v]);

		}

		data_type = MEP_DATA_DOUBLE;
		if (other_data1)
		  other_data1->data_type = MEP_DATA_DOUBLE;
		if (other_data2)
		  other_data2->data_type = MEP_DATA_DOUBLE;

		return MEP_OK;
	}
	else
		return E_NO_DATA;

	
}
//-----------------------------------------------------------------
int t_mep_data::to_interval_selected_col(double min, double max, int col, t_mep_data *other_data1, t_mep_data* other_data2)
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
			for (int t = 0; t < num_data; t++) {
				if (min_col > _data_double[t][col])
					min_col = _data_double[t][col];
				if (max_col < _data_double[t][col])
					max_col = _data_double[t][col];
			}

		if (other_data1->num_cols > col)
			for (int t = 0; t < other_data1->num_data; t++) {
				if (min_col > other_data1->_data_double[t][col])
					min_col = other_data1->_data_double[t][col];
				if (max_col < other_data1->_data_double[t][col])
					max_col = other_data1->_data_double[t][col];
			}

		if (other_data2->num_cols > col)
			for (int t = 0; t < other_data2->num_data; t++) {
				if (min_col > other_data2->_data_double[t][col])
					min_col = other_data2->_data_double[t][col];
				if (max_col < other_data2->_data_double[t][col])
					max_col = other_data2->_data_double[t][col];
			}
		// min, max found
		// now scale
		if (num_cols > col) {
			_modified = true;
			for (int t = 0; t < num_data; t++)
				_data_double[t][col] -= min_col;
			if (max_col > min_col)
				for (int t = 0; t < num_data; t++) {
					_data_double[t][col] /= (max_col - min_col);
					_data_double[t][col] *= (max - min);
				}
			for (int t = 0; t < num_data; t++)
				_data_double[t][col] += min;
		}

		if (other_data1->num_cols > col) {
			other_data1->_modified = true;
			for (int t = 0; t < other_data1->num_data; t++)
				other_data1->_data_double[t][col] -= min_col;
			if (max_col > min_col)
				for (int t = 0; t < other_data1->num_data; t++) {
					other_data1->_data_double[t][col] /= (max_col - min_col);
					other_data1->_data_double[t][col] *= (max - min);
				}
			for (int t = 0; t < other_data1->num_data; t++)
				other_data1->_data_double[t][col] += min;

		}

		if (other_data2->num_cols > col) {
			other_data2->_modified = true;
			for (int t = 0; t < other_data2->num_data; t++)
				other_data2->_data_double[t][col] -= min_col;
			if (max_col > min_col)
				for (int t = 0; t < other_data2->num_data; t++) {
					other_data2->_data_double[t][col] /= (max_col - min_col);
					other_data2->_data_double[t][col] *= (max - min);
				}
			for (int t = 0; t < other_data2->num_data; t++)
				other_data2->_data_double[t][col] += min;
		}

		return MEP_OK;
	}
	else
		return E_NO_DATA;

}
//-----------------------------------------------------------------
int t_mep_data::to_interval_everywhere(double min, double max, t_mep_data *other_data1, t_mep_data* other_data2)
{
	int result = to_interval_all_variables(min, max, other_data1, other_data2);
	if (result != MEP_OK)
		return result;
	if (num_outputs)
		return to_interval_selected_col(min, max, num_cols - 1, other_data1, other_data2);
	return MEP_OK;
}
//-----------------------------------------------------------------
int t_mep_data::to_interval_all_variables(double min, double max, t_mep_data *other_data1, t_mep_data* other_data2)
{
	for (int v = 0; v < num_cols - 1; v++) {
		int result = to_interval_selected_col(min, max, v, other_data1, other_data2);
		if (result != MEP_OK)
			return result;
		//is this numeric or alpha ?
	}
	return MEP_OK;
}
//-----------------------------------------------------------------
int t_mep_data::move_to(t_mep_data *dest, int count)
{
	if (!(data_type == dest->data_type || !dest->num_data))
		return E_CANNOT_MOVE_DATA_OF_DIFFERENT_TYPES; // can move only of the same type

	if (num_data && dest->num_data && num_cols != dest->num_cols)
		return E_DEST_AND_SOURCE_MUST_HAVE_THE_SAME_NUMBER_OF_COLUMNS; // 

	if (num_data >= count) {

		_modified = true;

		if (dest->num_data == 0) {
			dest->num_cols = num_cols;
			dest->num_outputs = num_outputs;
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
	else
		return E_NOT_ENOUGH_DATA_TO_MOVE; // not enough data
	return E_OK;
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
int t_mep_data::replace_symbol_from_selected_col(const char *s_find_what, const char* s_replace_with, int col, bool use_regular)
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

	_modified = true;
	return count_replaced;
}
//-----------------------------------------------------------------
int t_mep_data::find_symbol_from_selected_col(const char *s_find_what, int col, bool use_regular)
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
void t_mep_data::shuffle(void)
{
	if (data_type == MEP_DATA_DOUBLE) { // double
		for (int i = num_data - 1; i >= 1; i--) {
			int j = my_rand() % (i + 1);
			double *row = _data_double[i];
			_data_double[i] = _data_double[j];
			_data_double[j] = row;
		}
	}
	else {
		// string
		for (int i = num_data - 1; i >= 1; i--) {
			int j = my_rand() % (i + 1);
			char**row = _data_string[i];
			_data_string[i] = _data_string[j];
			_data_string[j] = row;
		}
	}
	_modified = true;
}
//-----------------------------------------------------------------
bool t_mep_data::is_classification_problem(void)
{
	for (int i = 0; i < num_data; i++)
		if (fabs(_data_double[i][num_cols - 1]) > 1E-6 &&  fabs(_data_double[i][num_cols - 1] - 1.0) > 1E-6)
			return false;
	return true;
}
//-----------------------------------------------------------------
int t_mep_data::get_num_rows(void)
{
	return num_data;
}
//-----------------------------------------------------------------
int t_mep_data::get_num_cols(void)
{
	return num_cols;
}
//-----------------------------------------------------------------
int t_mep_data::get_data_type(void)
{
	return data_type;
}
//-----------------------------------------------------------------
double* t_mep_data::get_row(int row)
{
	return _data_double[row];
}
//-----------------------------------------------------------------
double t_mep_data::get_value_double(int row, int col)
{
	return _data_double[row][col];
}
//-----------------------------------------------------------------
char* t_mep_data::get_value_string(int row, int col)
{
	return _data_string[row][col];
}
//-----------------------------------------------------------------
bool t_mep_data::is_modified(void)
{
	return _modified;
}
//-----------------------------------------------------------------
int t_mep_data::get_num_outputs(void)
{
	return num_outputs;
}
//-----------------------------------------------------------------
double** t_mep_data::get_data_matrix_double(void)
{
	return _data_double;
}
//-----------------------------------------------------------------
int t_mep_data::get_num_items_class_0(void)
{
	return num_class_0;
}
//-----------------------------------------------------------------
char*** t_mep_data::get_data_matrix_string(void)
{
	return _data_string;
}
//-----------------------------------------------------------------
void t_mep_data::set_num_outputs(int new_num)
{
	num_outputs = new_num;
}
//-----------------------------------------------------------------
int t_mep_data::get_num_classes(void)
{
	return num_classes;
}
//-----------------------------------------------------------------
void t_mep_data::count_num_classes(int target_col)
{
	if (num_outputs && num_data) {
		int max_value = _data_double[0][target_col];
		for (int i = 1; i < num_data; i++)
			if (max_value < _data_double[i][target_col])
				max_value = _data_double[i][target_col];
		num_classes = max_value + 1;
	}
	else
		num_classes = 0;
}
//-----------------------------------------------------------------