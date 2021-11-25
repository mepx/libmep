#include <string.h>
#include <ctype.h>

#include "csv_utils.h"

//--------------------------------------------------------------------------
void append_to_string(char* &Field, char Ch, int L, int &CAPACITY_L)
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
/*
void AddData(const char* Field, int R, int C)
{
	printf("R = %d C = %d D = %s\n", R, C, Field);
}
//--------------------------------------------------------------------------

void AddData(t_setter_data_base *obj, const char* Field, int R, int C)
{
	obj->add_string_data(R, C, Field);
	//printf("R = %d C = %d D = %s\n", R, C, Field);
}
//--------------------------------------------------------------------------
*/
bool get_csv_info_from_file(FILE* f, const char list_separator, t_setter_data_base* obj)
{
	int max_R = 0;
	int max_C = 0;
	int max_L = 0;

	char buf[4];
	fgets(buf, 4, f);

	if (strcmp(buf, "\xef\xbb\xbf")) {
		rewind(f);
	}

	bool Quoted = false;
	char *Field = NULL;
	int CAPACITY_L = 0;

	int R = 0;
	int C = 0;
	int Ch = getc(f);
	int L = 0;
	while (Ch != EOF){

		if (Ch == '"') {
			if (!Quoted)
				Quoted = true;
			else {
				int ch2 = getc(f);
				if (ch2 == '"') {
					append_to_string(Field, Ch, L, CAPACITY_L);

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
		Ch = getc(f);
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
bool get_csv_info_from_string_to_row(const char* input_string, const char list_separator, int R, t_setter_data_base* obj)
{
	int max_R = 0;
	int max_C = 0;
	int max_L = 0;

	bool Quoted = false;
	char* Field = NULL;
	int CAPACITY_L = 0;

	int index = 0;

	int C = 0;
	char Ch = input_string[index++];
	int L = 0;
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
		Ch = input_string[index++];
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
bool get_csv_info_from_string(const char* input_string, const char list_separator, t_setter_data_base* obj)
{
	int max_R = 0;
	int max_C = 0;
	int max_L = 0;

	bool Quoted = false;
	char* Field = NULL;
	int CAPACITY_L = 0;

	int index = 0;

	int C = 0;
	int R = 0;
	char Ch = input_string[index++];
	int L = 0;
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
		Ch = input_string[index++];
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


bool get_next_field(char* start_sir, char list_separator, char* dest, int& size, int& skipped)
{
	skipped = 0;
	char* tmp_start = start_sir;
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
