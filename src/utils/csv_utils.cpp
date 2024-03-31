// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//--------------------------------------------------------------------------
#include <string.h>
#include <ctype.h>
//--------------------------------------------------------------------------
#include "utils/csv_utils.h"
//--------------------------------------------------------------------------
void append_to_string(char* &Field, char Ch, size_t L, size_t &CAPACITY_L)
{
	if (L + 1 > CAPACITY_L) {
		char* new_Field = new char[L + 10];
		CAPACITY_L = L + 10;
		if (L)
			memcpy(new_Field, Field, L);
		if (Field)
			delete[] Field;
		Field = new_Field;
	}
	Field[L] = Ch;
}
//--------------------------------------------------------------------------
bool get_csv_info_from_file(FILE* f,
							const char list_separator,
							t_setter_data_base* obj)
{
	unsigned int max_R = 0;
	unsigned int max_C = 0;
	size_t max_L = 0;

	char buf[4];
	fgets(buf, 4, f);

	if (strcmp(buf, "\xef\xbb\xbf") != 0) {// are not equal
		rewind(f);
	}

	bool Quoted = false;
	char *Field = NULL;
	size_t CAPACITY_L = 0;

	unsigned int R = 0;
	unsigned int C = 0;

	int Ch = getc(f);
	while (Ch != EOF && Ch == list_separator)// skip consecutive separators
		Ch = getc(f);

	size_t L = 0;

	while (Ch != EOF){

		if (Ch == '"') {
			if (!Quoted)
				Quoted = true;
			else {
				int ch2 = getc(f);
				if (ch2 == '"') {
					append_to_string(Field, (char)Ch, L, CAPACITY_L);

					L++;
					if (max_L < L)
						max_L = L;
				}
				else {
					ungetc(ch2, f);
					Quoted = false;
				}
			}
		}
		else
			if (Ch == list_separator) {
				if (!Quoted) {
					append_to_string(Field, 0, L, CAPACITY_L);

					obj->add_string_data(R, C, Field);
					if (max_L < L)
						max_L = L;
					Field[0] = 0;
					L = 0;
					C++;
					if (max_C < C)
						max_C = C;
				}
				else {
					append_to_string(Field, (char)Ch, L, CAPACITY_L);
					L++;
					if (max_L < L)
						max_L = L;
				}
				;
			}
			else {
				if (Ch == 10) {
					if (!Quoted) {
						append_to_string(Field, 0, L, CAPACITY_L);
						obj->add_string_data(R, C, Field);

						C++;
						if (max_C < C)
							max_C = C;

						R++;
						if (max_R < R)
							max_R = R;
						Field[0] = 0;
						C = 0;
						L = 0;
					}
					else {// quoted
						append_to_string(Field, 0xD, L, CAPACITY_L);
						append_to_string(Field, 0xA, L, CAPACITY_L);
						L += 2;
						if (max_L < L)
							max_L = L;
					}
				}
				else {
					if (Ch == 13) {
						//just skip
					}
					else {
						append_to_string(Field, (char)Ch, L, CAPACITY_L);
						L++;
						if (max_L < L)
							max_L = L;
					}
				}
			}
		if (Ch == list_separator) { 
			while (Ch != EOF && Ch == list_separator)// skip consecutive separators
				Ch = getc(f);
		}
		else
			if (Ch == 10 || Ch == 13) {
				Ch = getc(f);
				while (Ch != EOF && Ch == list_separator)// skip consecutive separators from begining of the line
					Ch = getc(f);
			}
			else {
				Ch = getc(f);
				// if is list separator, then try to find if the next symbol is new line
				// if is new line, then use that one as current character
				// if not, continue as normal
				int count_skipped = 0;
				while (Ch != EOF && Ch == list_separator) {// skip consecutive separators from begining of the line
					Ch = getc(f);
					count_skipped++;
				}
				if (count_skipped && Ch != 10 && Ch != 13) {
					ungetc(Ch, f);
					count_skipped--;
					while (count_skipped) {
						count_skipped--;
						ungetc(list_separator, f);
					}
					Ch = list_separator;
				}
			}
	}

	if (Field && Field[0]) {
		append_to_string(Field, 0, L, CAPACITY_L);
		obj->add_string_data(R, C, Field);
		C++;
		if (max_C < C)
			max_C = C;

		R++;
		if (max_R < R)
			max_R = R;
	}
	if (Field)
		delete[] Field;
	return true;
}
//--------------------------------------------------------------------------
bool get_csv_info_from_string_to_row(const char* input_string, 
									 const char list_separator,
									 unsigned int R,
									 t_setter_data_base* obj)
{
	unsigned int max_R = 0;
	unsigned int max_C = 0;
	size_t max_L = 0;

	bool Quoted = false;
	char* Field = NULL;
	size_t CAPACITY_L = 0;

	int index = 0;

	unsigned int C = 0;
	char Ch = input_string[index++];
	while (Ch && Ch == list_separator)// skip consecutive separators
		Ch = input_string[index++];

	size_t L = 0;
	while (Ch) {

		if (Ch == '"') {
			if (!Quoted)
				Quoted = true;
			else {
				char ch2 = input_string[index++];
				if (ch2 == '"') {
					append_to_string(Field, Ch, L, CAPACITY_L);

					L++;
					if (max_L < L)
						max_L = L;
				}
				else {
					//ungetc(ch2, f);
					index--;
					Quoted = false;
				}
			}
		}
		else
			if (Ch == list_separator) {
				if (!Quoted) {
					append_to_string(Field, 0, L, CAPACITY_L);

					obj->add_string_data_to_row(R, C, Field);
					if (max_L < L)
						max_L = L;
					Field[0] = 0;
					L = 0;
					C++;
					if (max_C < C)
						max_C = C;
				}
				else {
					append_to_string(Field, Ch, L, CAPACITY_L);
					L++;
					if (max_L < L)
						max_L = L;
				}
				;
			}
			else {
				if (Ch == 10) {
					if (!Quoted) {
						append_to_string(Field, 0, L, CAPACITY_L);
						obj->add_string_data_to_row(R, C, Field);

						C++;
						if (max_C < C)
							max_C = C;

						R++;
						if (max_R < R)
							max_R = R;
						Field[0] = 0;
						C = 0;
						L = 0;
					}
					else {
						append_to_string(Field, 0xD, L, CAPACITY_L);
						append_to_string(Field, 0xA, L, CAPACITY_L);
						L += 2;
						if (max_L < L)
							max_L = L;
					}
				}
				else {
					if (Ch == 13) {
						//just skip
					}
					else {
						append_to_string(Field, Ch, L, CAPACITY_L);
						L++;
						if (max_L < L)
							max_L = L;
					}
				}
			}
		//Ch = getc(f);
		if (Ch == list_separator) { 
			while (Ch && Ch == list_separator)// skip consecutive separators
				Ch = input_string[index++];
		}
		else
			if (Ch == 10 || Ch == 13) {
				Ch = input_string[index++];
				while (Ch && Ch == list_separator)// skip consecutive separators
					Ch = input_string[index++];
			}
			else {
				Ch = input_string[index++];
				// if is list separator, then try to find if the next symbol is new line
				// if is new line, then use that one as current character
				// if not, continue as normal
				int count_skipped = 0;
				while (Ch != EOF && Ch == list_separator) {// skip consecutive separators from begining of the line
					Ch = input_string[index++];
					count_skipped++;
				}
				if (count_skipped && Ch != 10 && Ch != 13) {
					index--;
					count_skipped--;
					while (count_skipped) {
						count_skipped--;
						index--;
					}
					Ch = input_string[index-1];
				}
			}
	}

	if (Field && Field[0]) {
		append_to_string(Field, 0, L, CAPACITY_L);
		obj->add_string_data_to_row(R, C, Field);
		C++;
		if (max_C < C)
			max_C = C;

		R++;
		if (max_R < R)
			max_R = R;
	}
	if (Field)
		delete[] Field;
	return true;
}
//--------------------------------------------------------------------------
bool get_csv_info_from_string(const char* input_string, 
							  const char list_separator,
							  t_setter_data_base* obj)
{
	unsigned int max_R = 0;
	unsigned int max_C = 0;
	size_t max_L = 0;

	bool Quoted = false;
	char* Field = NULL;
	size_t CAPACITY_L = 0;

	int index = 0;

	unsigned int C = 0;
	unsigned int R = 0;
	char Ch = input_string[index++];
	while (Ch && Ch == list_separator)// skip consecutive separators
		Ch = input_string[index++];

	size_t L = 0;
	while (Ch) {

		if (Ch == '"') {
			if (!Quoted)
				Quoted = true;
			else {
				char ch2 = input_string[index++];
				if (ch2 == '"') {
					append_to_string(Field, Ch, L, CAPACITY_L);

					L++;
					if (max_L < L)
						max_L = L;
				}
				else {
					//ungetc(ch2, f);
					index--;
					Quoted = false;
				}
			}
		}
		else
			if (Ch == list_separator) {
				if (!Quoted) {
					append_to_string(Field, 0, L, CAPACITY_L);

					obj->add_string_data(R, C, Field);
					if (max_L < L)
						max_L = L;
					Field[0] = 0;
					L = 0;
					C++;
					if (max_C < C)
						max_C = C;
				}
				else {
					append_to_string(Field, Ch, L, CAPACITY_L);
					L++;
					if (max_L < L)
						max_L = L;
				}
				;
			}
			else {
				if (Ch == 10) {
					if (!Quoted) {
						append_to_string(Field, 0, L, CAPACITY_L);
						obj->add_string_data(R, C, Field);

						C++;
						if (max_C < C)
							max_C = C;

						R++;
						if (max_R < R)
							max_R = R;
						Field[0] = 0;
						C = 0;
						L = 0;
					}
					else {// quoted
						append_to_string(Field, 0xD, L, CAPACITY_L);
						append_to_string(Field, 0xA, L, CAPACITY_L);
						L += 2;
						if (max_L < L)
							max_L = L;
					}
				}
				else {
					if (Ch == 13) {
						//just skip
					}
					else {
						append_to_string(Field, Ch, L, CAPACITY_L);
						L++;
						if (max_L < L)
							max_L = L;
					}
				}
			}
		//Ch = getc(f);
		if (Ch == list_separator) { // skip consecutive separators
			while (Ch && Ch == list_separator)
				Ch = input_string[index++];
		}
		else
			if (Ch == 10 || Ch == 13) {
				Ch = input_string[index++];
				while (Ch && Ch == list_separator)// skip consecutive separators
					Ch = input_string[index++];
			}
			else {
				Ch = input_string[index++];
				int count_skipped = 0;
				while (Ch != EOF && Ch == list_separator) {// skip consecutive separators from begining of the line
					Ch = input_string[index++];
					count_skipped++;
				}
				if (count_skipped && Ch != 10 && Ch != 13) {
					index--;
					count_skipped--;
					while (count_skipped) {
						count_skipped--;
						index--;
					}
					Ch = input_string[index-1];
				}
			}
	}

	if (Field && Field[0]) {
		append_to_string(Field, 0, L, CAPACITY_L);
		obj->add_string_data(R, C, Field);
		C++;
		if (max_C < C)
			max_C = C;

		R++;
		if (max_R < R)
			max_R = R;
	}
	if (Field)
		delete[] Field;
	return true;
}
//--------------------------------------------------------------------------
char* trim_and_strcpy(char* destination, const char* source)
{
	// return if no memory is allocated to the destination
	if (destination == NULL) {
		return NULL;
	}

	// take a pointer pointing to the beginning of the destination string
	char* saved_destination = destination;

	// trim left
	while (*source && isspace ((unsigned char)(*source)))
		source++;
	// copy the C-string pointed by source into the array
	// pointed by destination

	while (*source){
		*destination = *source;
		destination++;
		source++;
	}
	// trim right
	while (destination != saved_destination && isspace((unsigned char)(*(destination - 1))))
		destination--;

	// include the terminating null character
	*destination = '\0';

	// the destination is returned by standard `strcpy()`
	return saved_destination;
}
// ---------------------------------------------------------------------------
