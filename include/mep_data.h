#ifndef DATA_CLASS_H_INCLUDED
#define DATA_CLASS_H_INCLUDED

#include "pugixml.hpp"

#define MEP_DATA_DOUBLE 0
#define MEP_DATA_STRING 1

#define E_OK 0
#define E_CANNOT_MOVE_DATA_OF_DIFFERENT_TYPES 1
#define E_NOT_ENOUGH_DATA_TO_MOVE 2
#define E_DEST_AND_SOURCE_MUST_HAVE_THE_SAME_NUMBER_OF_COLUMNS 3


//-----------------------------------------------------------------
class t_mep_data
{
private:
	int num_cols;
	int num_data;

	double **_data_double;

	char* **_data_string;

	int num_outputs;

	int num_class_0;

	int data_type; // 0-double, 1- string

	char list_separator;

	bool _modified;

	bool from_csv_double(const char *file_name);
	bool from_csv_string(const char *file_name);

	void delete_double_data(void);
	void delete_string_data(void);
public:

	t_mep_data(void);
	~t_mep_data();

	int get_num_rows(void);
	int get_num_cols(void);

	int get_data_type(void);

	int get_num_outputs(void);
	void set_num_outputs(int new_num);

	double** get_data_matrix_double(void);
	char*** get_data_matrix_string(void);
	
	// returns an entire row as a pointer to double
	// data type must be 0,
	double* get_row(int row);

	double get_value_double(int row, int col);
	char* get_value_string(int row, int col);

	void clear_data(void);
	void delete_data(void);
    void init(void);

    int from_xml(pugi::xml_node parent);
    int to_xml(pugi::xml_node parent);
	bool to_csv(const char *file_name, char ListSeparator);
	bool from_csv(const char *filename);

	void to_numeric(void);
	int to_interval_everywhere(double min, double max);
	int to_interval_all_variables(double min, double max);
	int to_interval_selected_col(double min, double max, int col);
    
	int replace_symbol_from_selected_col(const char *find_what, const char* replace_with, int col, bool use_regular);
	int replace_symbol_from_all_variables(const char *find_what, const char* replace_with, bool use_regular);
	int replace_symbol_everywhere(const char *s_find_what, const char* s_replace_with, bool use_regular);

	int find_symbol_from_selected_col(const char *find_what, int col, bool use_regular);
	int find_symbol_from_all_variables(const char *find_what, bool use_regular);
	int find_symbol_everywhere(const char *s_find_what, bool use_regular);

	void shuffle(void);

	bool detect_list_separator(const char *file_name);

	int move_to(t_mep_data *, int count);

	void count_0_class(int target_col);

	bool is_classification_problem(void);

	bool is_modified(void);

	int get_num_items_class_0(void);
};
//-----------------------------------------------------------------
#endif // DATA_CLASS_H_INCLUDED
