#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <stdio.h>
//-----------------------------------------------------------------
class t_setter_data_base {
public:
	virtual void add_string_data(int row, int col, const char* data) = 0;
	virtual void add_string_data_to_row(int row, int col, const char* data) = 0;
};

//-----------------------------------------------------------------
bool get_csv_info_from_file(FILE* f, const char list_separator, t_setter_data_base * obj);
bool get_csv_info_from_string_to_row(const char* input_string, const char list_separator, int R, t_setter_data_base* obj);
bool get_csv_info_from_string(const char* input_string, const char list_separator, t_setter_data_base* obj);

//-----------------------------------------------------------------
bool get_next_field(char* start_sir, char list_separator, char* dest, int& size, int& skipped);
//-----------------------------------------------------------------
char* trim_and_strcpy(char* destination, const char* source);
//-----------------------------------------------------------------

#endif
