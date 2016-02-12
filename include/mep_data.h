#ifndef DATA_CLASS_H_INCLUDED
#define DATA_CLASS_H_INCLUDED

#include "pugixml.hpp"

#define MEP_DATA_DOUBLE 0
#define MEP_DATA_STRING 1

//-----------------------------------------------------------------
class t_mep_data
{
private:
	bool from_csv_double(const char *file_name);
	bool from_csv_string(const char *file_name);

	void delete_double_data(void);
	void delete_string_data(void);

	bool _modified;

public:
    int num_cols;
    int num_data;

    double **_data_double;
    
	char* **_data_string;

	int num_targets;

	int num_class_0;

	int data_type; // 0-double, 1- string

	char list_separator;

	t_mep_data(void);
	~t_mep_data();

	int get_num_rows(void);
	int get_num_cols(void);

	int get_data_type(void);

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
};
//-----------------------------------------------------------------
#endif // DATA_CLASS_H_INCLUDED
