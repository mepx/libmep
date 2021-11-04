// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <string.h>
#include <stdlib.h>

//-----------------------------------------------------------------
#include "mep_data.h"
//-----------------------------------------------------------------
int t_mep_data::from_xml(pugi::xml_node parent)
{
	clear_data();

	pugi::xml_node node = parent.child("num_data");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_data = atoi(value_as_cstring);
	}
	else
		num_data = 0;

	node = parent.child("num_variables");
	if (node) {
		const char* value_as_cstring = node.child_value();
		int num_variables = atoi(value_as_cstring);
		if (num_variables)
			num_cols = num_variables + 1;
		else
			num_cols = 0;
	}
	else {
		node = parent.child("num_cols");
		if (node) {
			const char* value_as_cstring = node.child_value();
			num_cols = atoi(value_as_cstring);
		}
		else
			num_cols = 0;
	}

	node = parent.child("num_outputs");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_outputs = atoi(value_as_cstring);
	}
	else
		num_outputs = 1;


	node = parent.child("num_classes");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_classes = atoi(value_as_cstring);
	}
	else
		num_classes = 0;

	node = parent.child("data_type");
	if (node) {
		const char* value_as_cstring = node.child_value();
		data_type = (char)atoi(value_as_cstring);
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
		const char* value_as_cstring = node.child_value();
		list_separator = value_as_cstring[0];
	}
	else
		list_separator = ' ';// blank space by default

	if (data_type == MEP_DATA_DOUBLE) {
		if (num_data) {
			_data_double = new double* [num_data];
			for (int r = 0; r < num_data; r++)
				_data_double[r] = new double[num_cols];
		}
	}
	else {
		if (num_data) {
			_data_string = new char** [num_data];
			for (int r = 0; r < num_data; r++) {
				_data_string[r] = new char* [num_cols];
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
			const char* value_as_cstring = row.child_value();
			size_t num_jumped_chars = 0;

			for (int c = 0; c < num_cols; c++) {
				_data_double[r][c] = 0;
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &_data_double[r][c]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
	}
	else {// string
		for (pugi::xml_node row = node_data.child("row"); row; row = row.next_sibling("row"), r++) {
			const char* value_as_cstring = row.child_value();
			//char* buf = (char*)value_as_cstring;
			//_data_string[r][c]
			get_csv_info_from_string_to_row(value_as_cstring, ' ', r, this);
		}
	}

	if (r == 0) {
		clear_data();
	}

	_modified = false;
	return true;
}
//-----------------------------------------------------------------
int t_mep_data::to_xml(pugi::xml_node parent)
{
	char temp_str[30];

	pugi::xml_node node = parent.append_child("num_data");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	sprintf(temp_str, "%d", num_data);
	data.set_value(temp_str);

	node = parent.append_child("num_cols");
	data = node.append_child(pugi::node_pcdata);
	sprintf(temp_str, "%d", num_cols);
	data.set_value(temp_str);

	node = parent.append_child("num_outputs");
	data = node.append_child(pugi::node_pcdata);
	sprintf(temp_str, "%d", num_outputs);
	data.set_value(temp_str);

	node = parent.append_child("num_classes");
	data = node.append_child(pugi::node_pcdata);
	sprintf(temp_str, "%d", num_classes);
	data.set_value(temp_str);

	node = parent.append_child("data_type");
	data = node.append_child(pugi::node_pcdata);
	sprintf(temp_str, "%d", data_type);
	data.set_value(temp_str);

	node = parent.append_child("list_separator");
	data = node.append_child(pugi::node_pcdata);
	temp_str[0] = list_separator;
	temp_str[1] = 0;
	data.set_value(temp_str);

	if (!num_data || !num_cols) {
		return true;
	}

	for (int c = 0; c < num_cols; c++) {
	}

	pugi::xml_node node_data = parent.append_child("data");

	if (_data_double) {
		size_t s_CAPACITY = num_cols * 10; // how to test?: set this to 1; not 0 because I should not do new[0]
		char* s_tmp_row = new char[s_CAPACITY];
		
		char tmp_s[300];
		for (int r = 0; r < num_data; r++) {
			node = node_data.append_child("row");
			data = node.append_child(pugi::node_pcdata);
			s_tmp_row[0] = 0;
			size_t s_tmp_row_len = 0;

			for (int c = 0; c < num_cols; c++) {
				size_t tmp_s_len = sprintf(tmp_s, "%lg", this->_data_double[r][c]);
				if (tmp_s_len + s_tmp_row_len + 2 > s_CAPACITY) {
					// must increase the string capacity
					s_CAPACITY += (num_cols - c) * tmp_s_len + 2;
					char* s_new_tmp_row = new char [s_CAPACITY];
					strcpy(s_new_tmp_row, s_tmp_row);
					delete[] s_tmp_row;
					s_tmp_row = s_new_tmp_row;
				}
				strcat(s_tmp_row, tmp_s);
				if (c < num_cols - 1)
					strcat(s_tmp_row, " ");
				s_tmp_row_len += tmp_s_len + 1;
			}
			//now the target if there is one...
			data.set_value(s_tmp_row);
		}
		delete[] s_tmp_row;
	}
	else
		if (_data_string) {
			size_t s_CAPACITY = num_cols * 10; // how to test?: set this to 1; not 0 because I should not do new[0]
			char* s_tmp_row = new char[s_CAPACITY];

			for (int r = 0; r < num_data; r++) {
				node = node_data.append_child("row");
				data = node.append_child(pugi::node_pcdata);
				s_tmp_row[0] = 0;
				size_t s_tmp_row_len = 0;
				for (int c = 0; c < num_cols; c++) 
					if (_data_string[r][c]){
						char* str_ptr = NULL;
						char* tmp_str = NULL;
						if (strchr(_data_string[r][c], '\n') ||
							strchr(_data_string[r][c], '\r') ||
							strchr(_data_string[r][c], ' ') ||
							strchr(_data_string[r][c], '"')) {
							// put it inside ""
							size_t len = strlen(_data_string[r][c]);
							tmp_str = new char[2 + 2 * len + 1];
							size_t tmp_pos = 0;

							tmp_str[tmp_pos++] = '"';
							for (size_t i = 0; i < len; i++) {
								if (_data_string[r][c][i] == '"')
									tmp_str[tmp_pos++] = '"';

								tmp_str[tmp_pos++] = _data_string[r][c][i];
							}
							tmp_str[tmp_pos++] = '"';
							tmp_str[tmp_pos] = '\0';

							str_ptr = tmp_str;
						}
						else
							str_ptr = _data_string[r][c];// no "", just data

						size_t tmp_s_len = strlen(str_ptr);
						if (tmp_s_len + s_tmp_row_len + 2 > s_CAPACITY) {
							// must increase the string capacity
							s_CAPACITY += (num_cols - c) * tmp_s_len + 2;
							char* s_new_tmp_row = new char[s_CAPACITY];
							strcpy(s_new_tmp_row, s_tmp_row);
							delete[] s_tmp_row;
							s_tmp_row = s_new_tmp_row;
						}

						strcat(s_tmp_row, str_ptr); 
						if (c < num_cols - 1)
							strcat(s_tmp_row, " ");

						if (tmp_str)
							delete[] tmp_str;
					}
				//now the target if there is one...

				data.set_value(s_tmp_row);
			}
			delete[] s_tmp_row;
		}
	_modified = false;
	return true;
}
//-----------------------------------------------------------------
