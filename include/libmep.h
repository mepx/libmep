#ifndef MEP_CLASS_H_INCLUDED
#define MEP_CLASS_H_INCLUDED

#include <thread>
#include <mutex>

#include "pugixml.hpp"

#include "parameters_class.h"
#include "data_class.h"
#include "operators_class.h"
#include "chromosome_class.h"
#include "mep_stats.h"
#include "utils_mep.h"

//-----------------------------------------------------------------
typedef void(*f_on_progress)(void);
//-----------------------------------------------------------------

//-----------------------------------------------------------------
struct t_sub_population{
	chromosome *individuals;
	chromosome offspring1, offspring2;
};
//-----------------------------------------------------------------

class t_mep
{

private:
	t_parameters parameters;
	t_sub_population* pop;
	double **cached_eval_matrix_double;
	double *cached_sum_of_errors, *cached_threashold;
	int best_individual_index;
	int best_subpopulation_index;

	int best_individual_index_for_test;
	int best_subpopulation_index_for_test;

	t_operators operators;

	int num_operators;

	int actual_operators[MAX_OPERATORS];

	bool _stopped;
	int last_run_index;

	t_data training_data;
	t_data validation_data;
	t_data test_data;

	char version[100];
	t_mep_stat *stats;

	bool get_error_double(chromosome &Individual, double *inputs, double *outputs);
	bool evaluate_double(chromosome &Individual, double *inputs, double *outputs);


	int num_total_variables;
	int target_col;

	int num_actual_variables;
	bool *variables_enabled;

	int *actual_enabled_variables;

	bool modified_project;

	char *problem_description;

	bool cache_results_for_all_training_data;

	bool start_steady_state(int seed, double ***, s_value_class **array_value_class, f_on_progress on_generation, f_on_progress on_new_evaluation);       // Steady-State MEP
	long tournament(t_sub_population &pop);

	void fitness_regression_double_cache_all_training_data(chromosome &Individual, double **);
	void fitness_classification_double_cache_all_training_data(chromosome &Individual, double**);
	void fitness_regression_double(chromosome &Individual, double* eval_array_double, double *fitness_array);

	void generate_random_individuals(void); // randomly initializes the individuals

	double compute_validation_error(int *, int*, double **eval_double);

	void allocate_sub_population(t_sub_population &pop);

	void allocate_values(double ****, s_value_class***);
	void delete_values(double ****, s_value_class***);

	void sort_by_fitness(t_sub_population &pop); // sort ascending the individuals in population
	void compute_best_and_average_error(double &best_error, double &mean_error);
	void compute_eval_matrix_double(chromosome &Individual, double **, int*);
	void compute_eval_vector_double(chromosome &Individual);
	void compute_cached_eval_matrix_double(void);
	void compute_cached_eval_matrix_double2(s_value_class *array_value_class);


	bool compute_regression_error_on_double_data(chromosome &individual, double **inputs, int num_data, double ** data, double *error);
	bool compute_classification_error_on_double_data(chromosome &individual, double **inputs, int num_data, double ** data, double *error);
	bool compute_regression_error_on_double_data_return_error(chromosome &individual, double **inputs, int num_data, double ** data, double *error);
	bool compute_classification_error_on_double_data_return_error(chromosome &individual, double **inputs, int num_data, double ** data, double *error);

	void delete_sub_population(t_sub_population &pop);

	void evolve_one_subpopulation_for_one_generation(int *current_subpop_index, std::mutex* mutex, t_sub_population * sub_populations, int generation_index, double ** eval_double, s_value_class *tmp_value_class);


	void fitness_regression(chromosome &Individual, double **);
	void fitness_classification(chromosome &individual, double **, s_value_class *);
	void fitness_classification_double_cache_all_training_data(chromosome &Individual, double **eval_double, s_value_class *);


public:

	t_mep();
	~t_mep();

	// returns the version of the library
	const char * get_version(void);

	// returns the index of the last run (if multiple runs are performed)
	int get_last_run_index(void);

	// returns the number of variables
	int get_num_total_variables(void);

	// sets the number of variables
	void set_num_total_variables(int value);
	//---------------------------------------------------------------------------

	// loads the training data from a csv file
	int load_training_data_from_csv(const char* file_name);

	// saves the training data to a csv files
	int save_training_data_to_csv(const char* file_name, char list_separator);

	// returns the data type of the training data: 
	// 0 for real
	// 1 for string
	int get_training_data_type(void);

	// returns the number of columns of the training data
	int get_training_data_num_columns(void);

	// returns the number of training data
	int get_training_data_num_rows(void);

	// returns a training data as string (if data type is 1)
	// assumes that row and col are valid; no test for out of range are performed
	char *get_training_data_as_string(int row, int col);

	// returns a training data as double (if data type is 0)
	// assumes that row and col are valid; no test for out of range are performed
	double get_training_data_as_double(int row, int col);

	// clears the training data internal structures
	void clear_training_data(void);

	// transform string values from training data to real values
	void training_data_to_numeric(void);


	// loads the validation data from a csv file
	int load_validation_data_from_csv(const char* file_name);

	// saves the validation data to a csv files
	int save_validation_data_to_csv(const char* file_name, char list_separator);

	// returns the data type of the validation data: 
	// 0 for real
	// 1 for string
	int get_validation_data_type(void);

	// returns the number of columns of the validation data
	int get_validation_data_num_columns(void);

	// returns the number of validation data
	int get_validation_data_num_rows(void);

	// returns a validation data as string (if data type is 1)
	// assumes that row and col are valid; no test for out of range are performed
	char *get_validation_data_as_string(int row, int col);
	
	// returns a validation data as double (if data type is 0)
	// assumes that row and col are valid; no test for out of range are performed
	double get_validation_data_as_double(int row, int col);
	
	// clears the validation data internal structures
	void clear_validation_data(void);

	// transform string values from validation data to real values
	void validation_data_to_numeric(void);



	// loads the test data from a csv file
	int load_test_data_from_csv(const char* file_name);
	
	// saves the validation data to a csv files
	int save_test_data_to_csv(const char* file_name, char list_separator);

	// returns the data type of the test data: 
	// 0 for real
	// 1 for string
	int get_test_data_type(void);

	// returns the number of columns of the test data
	int get_test_data_num_columns(void);

	// returns the number of validation data
	int get_test_data_num_rows(void);
	
	// returns a test data as string (if data type is 1)
	// assumes that row and col are valid; no test for out of range are performed
	char *get_test_data_as_string(int row, int col);
	
	// returns a test data as double (if data type is 0)
	// assumes that row and col are valid; no test for out of range are performed
	double get_test_data_as_double(int row, int col);

	// clears the test data internal structurs
	void clear_test_data(void);

	// transform string values from validation data to real values
	void test_data_to_numeric(void);
	//---------------------------------------------------------------------------

	// returns true if the process is running, false otherwise
	bool is_running(void);

	// starts the optimization process
	int start(f_on_progress on_generation, f_on_progress on_new_evaluation, f_on_progress on_complete_run);

	// returns the best training error
	double get_best_training_error(int run, int generation);
	
	// returns the best validation error
	double get_best_validation_error(int run);

	// returns the average (over the entire population) training error
	double get_average_training_error(int run, int generation);

	// returns the running time
	double get_running_time(int run);

	// returns the last generation of a given run (useful when the run has been stopped earlier)
	int get_latest_generation(int run);

	// returns the error on the test data
	double get_test_error(int run);

	// stops the optimization process
	void stop(void);

	// gets the best chromosome
	void get_best(chromosome& dest);

	// save statistics to csv file
	int stats_to_csv(const char* file_name);

	// gets the output obtaining by running the best program in a given run against in input
	bool get_output(int run_index, double *inputs, double *outputs);

	//void evolve_one_subpopulation_for_one_generation(t_sub_population &pop, double**, s_value_class *tmp_value_class);
	//void evolve_one_subpopulation_for_one_generation(int *current_subpop_index, std::mutex* mutex, t_sub_population * sub_populations, int generation_index, t_parameters &params, double **training_data, int num_training_data, int num_variables, double ** eval_double, s_value_class *tmp_value_class);


	void sort_stats_by_running_time(bool ascending);
	void sort_stats_by_training_error(bool ascending);
	void sort_stats_by_validation_error(bool ascending);
	void sort_stats_by_test_error(bool ascending);

	// saves everything to an xml file
	int to_xml(const char* file_name);
	
	// saves everything to an xml file
	int from_xml(const char* file_name);

	// saves everything to a pugixml node
	int to_pugixml_node(pugi::xml_node parent);
	
	// loads everything from a pugixml node
	int from_pugixml_node(pugi::xml_node parent);

public:

	// returns the mutation probability
	double get_mutation_probability(void);                   

	// returns crossover probability
	double get_crossover_probability(void);               

	// returns the length of a chromosome (the number of genes)
	long get_code_length(void);              

	// returns the number of individuals in a (sub)population
	long get_subpopulation_size(void);                 

	// returns the number of threads of the program. On each thread a subpopulation is evolved
	long get_num_threads(void);

	// returns the tournament size
	long get_tournament_size(void);

	// returns the number of generations
	long get_num_generations(void);

	// returns the problem type
	// 0 - symbolic regression, 
	// 1 - classification
	long get_problem_type(void); 

	// returns the number of sub-populations
	long get_num_subpopulations(void);

	// returns the probability of operators occurence
	double get_operators_probability(void);

	// returns the probability of variables occurence
	double get_variables_probability(void);

	// returns the probability of constants occurence
	double get_constants_probability(void);

	// returns true if the validation data is used
	bool get_use_validation_data(void);

	// returns the crossover type 
	// 0 UNIFORM_CROSSOVER
	// 1 ONE_CUTTING_POINT_CROSSOVER
	int get_crossover_type(void);

	// returns the seed for generating random numbers
	long get_random_seed(void);

	// returns the number of runs
	long get_num_runs(void);

	// returns true if the programs are returned in the simplified form (introns are removed)
	bool get_simplified_programs(void);

	// sets the mutation probability
	void set_mutation_probability(double value);                   

	// sets the crossover probability
	void set_crossover_probability(double value);               

	// sets the number of genes in a chromosome
	void set_code_length(long value);          

	// sets the number of individuals in population
	void set_subpopulation_size(long value);                 

	// sets the number of threads
	void set_num_threads(long value);

	// sets the tournament size
	void set_tournament_size(long value);

	// sets the number of generations
	void set_num_generations(long value);

	// sets the problem type
	// 0- regression, 
	// 1-classification
	void set_problem_type(long value); 

	// sets the number of subpopulations
	void set_num_subpopulations(long value);

	// sets the operators probability
	void set_operators_probability(double value);

	// sets the variables probability
	void set_variables_probability(double value);

	// sets the constants probability
	void set_constants_probability(double value);

	// sets the utilization of validation data
	void set_use_validation_data(bool value);

	// sets the crossover type
	void set_crossover_type(int value);

	// sets the random seed
	void set_random_seed(long value);

	// sets the number of runs
	void set_num_runs(long value);

	// sets the simplified programs parameters
	void set_simplified_programs(bool value);

	// returns the number of automatic constants
	long get_num_automatic_constants(void);

	// returns the number of user defined constants
	long get_num_user_defined_constants(void);

	// returns the min interval for automatic constants
	double get_min_constants_interval_double(void);

	// returns the max interval for automatic constants
	double get_max_constants_interval_double(void);

	// returns a constant
	double get_constants_double(long index);

	// returns the constants type
	// 0 - USER_DEFINED_CONSTANTS
	// 1- AUTOMATIC_CONSTANTS
	long get_constants_type(void);

	// returns true if the constants can evolve
	bool get_constants_can_evolve(void);

	// returns the max deviation within which the constants can evolve
	double get_constants_mutation_max_deviation(void);

	// sets the number of automatically generated constants
	void set_num_automatic_constants(long value);

	// sets the number of user defined constants
	void set_num_user_defined_constants(long value);

	// sets the lower bound of the constants interval
	void set_min_constants_interval_double(double value);
	
	// sets the upper bound of the constants interval
	void set_max_constants_interval_double(double value);

	// sets a particular user-defined constant
	void set_constants_double(long index, double value);

	// sets the constants type
	// 0 - USER_DEFINED_CONSTANTS
	// 1- AUTOMATIC_CONSTANTS
	void set_constants_type(long value);

	// sets if the constants can evolve
	void  set_constants_can_evolve(bool value);

	// sets the max deviation when constants can evolve
	void set_constants_mutation_max_deviation(double value);

	// initialize the parameters
	void init_parameters(void);

	// returns true if the addition operator is enabled
	bool get_addition(void);
	
	// returns true if the subtraction operator is enabled
	bool get_subtraction(void);
	
	// returns true if the multiplication operator is enabled
	bool get_multiplication(void);
	
	// returns true if the division operator is enabled
	bool get_division(void);
	
	// returns true if the power operator is enabled
	bool get_power(void);
	
	// returns true if the sqrt operator is enabled
	bool get_sqrt(void);
	
	// returns true if the exp operator is enabled
	bool get_exp(void);
	
	// returns true if the pow10 operator is enabled
	bool get_pow10(void);
	
	// returns true if the ln (natural logarithm) operator is enabled
	bool get_ln(void);
	
	// returns true if the log10 (log in base 10) operator is enabled
	bool get_log10(void);

	// returns true if the log2 (log in base 2) operator is enabled
	bool get_log2(void);
	
	// returns true if the floor operator is enabled
	bool get_floor(void);
	
	// returns true if the ceil operator is enabled
	bool get_ceil(void);
	
	// returns true if the absolute operator is enabled
	bool get_abs(void);
	
	// returns true if the inv (1/x) operator is enabled
	bool get_inv(void);
	
	// returns true if the x^2 operator is enabled
	bool get_x2(void);
	
	// returns true if the min (x, y) operator is enabled
	bool get_min(void);
	
	// returns true if the max(x, y) operator is enabled
	bool get_max(void);
	
	// returns true if the sin operator is enabled
	bool get_sin(void);
	
	// returns true if the cos operator is enabled
	bool get_cos(void);
	
	// returns true if the tan operator is enabled
	bool get_tan(void);
	
	// returns true if the asin operator is enabled
	bool get_asin(void);
	
	// returns true if the acos operator is enabled
	bool get_acos(void);
	
	// returns true if the atan operator is enabled
	bool get_atan(void);
	
	// returns true if the "if lower than zero" operator is enabled
	bool get_iflz(void);
	
	// returns true if the "if a lower than b returns c else returns d" operator is enabled
	bool get_ifalbcd(void);


	// sets if the addition operator is enabled or not
	void set_addition(bool value);
	
	// sets if the subtraction operator is enabled or not
	void set_subtraction(bool value);

	// sets if the multiplication operator is enabled or not
	void set_multiplication(bool value);

	// sets if the division operator is enabled or not
	void set_division(bool value);

	// sets if the power operator is enabled or not
	void set_power(bool value);

	// sets if the sqrt operator is enabled or not
	void set_sqrt(bool value);

	// sets if the exp operator is enabled or not
	void set_exp(bool value);

	// sets if the pow10 operator is enabled or not
	void set_pow10(bool value);

	// sets if the ln operator is enabled or not
	void set_ln(bool value);

	// sets if the log10 operator is enabled or not
	void set_log10(bool value);

	// sets if the log2 operator is enabled or not
	void set_log2(bool value);

	// sets if the floor operator is enabled or not
	void set_floor(bool value);

	// sets if the ceil operator is enabled or not
	void set_ceil(bool value);

	// sets if the abs operator is enabled or not
	void set_abs(bool value);

	// sets if the inv (1/x) operator is enabled or not
	void set_inv(bool value);

	// sets if the x^2 operator is enabled or not
	void set_x2(bool value);

	// sets if the min operator is enabled or not
	void set_min(bool value);

	// sets if the max operator is enabled or not
	void set_max(bool value);

	// sets if the sin operator is enabled or not
	void set_sin(bool value);
	
	// sets if the cos operator is enabled or not
	void set_cos(bool value);
	
	// sets if the tan operator is enabled or not
	void set_tan(bool value);
	
	// sets if the asin operator is enabled or not
	void set_asin(bool value);
	
	// sets if the acos operator is enabled or not
	void set_acos(bool value);
	
	// sets if the atan operator is enabled or not
	void set_atan(bool value);
	
	// sets if the "if less than zero" operator is enabled or not
	void set_iflz(bool value);
	
	// sets if the "if a lower than b returns c else returns d" operator is enabled or not
	void set_ifalbcd(bool value);

	// initialize operators
	void init_operators();

	// returns true if the training data is a classification problem (last column has only 0 and 1)
	bool is_training_data_a_classification_problem(void);

	// returns true if the test data is a classification problem (last column has only 0 and 1)
	bool is_test_data_a_classification_problem(void);

	// returns true if the validation data is a classification problem (last column has only 0 and 1)
	bool is_validation_data_a_classification_problem(void);

	// scale all training data to [min, max] interval
	void training_data_scale_to_interval_everywhere(double min, double max);

	// scale all training data variables to [min, max] interval
	void training_data_scale_to_interval_all_variables(double min, double max);

	// scale the selected training data column to [min, max] interval
	void training_data_scale_to_interval_selected_col(double min, double max, int col);

	// replace a given string in the training data column
	// returns the number of replacements
	int training_data_replace_symbol_from_selected_col(const char *find_what, const char* replace_with, int col, bool use_regular);
	
	// replace a given string in all variables of the training data matrix
	// returns the number of replacements
	int training_data_replace_symbol_from_all_variables(const char *find_what, const char* replace_with, bool use_regular);

	// replace a given string everywhere in the training data matrix
	// returns the number of replacements
	int training_data_replace_symbol_everywhere(const char *find_what, const char* replace_with, bool use_regular);

	// searches for a string in a particular col of the training matrix
	// returns the number of findings
	int training_data_find_symbol_from_selected_col(const char *find_what, int col, bool use_regular);
	
	// searches for a string in all variables of the training matrix
	// returns the number of findings
	int training_data_find_symbol_from_all_variables(const char *find_what, bool use_regular);
	
	// searches for a string in all training matrix
	// returns the number of findings
	int training_data_find_symbol_everywhere(const char *find_what, bool use_regular);


	// scale all validation data to [min, max] interval
	void validation_data_scale_to_interval_everywhere(double min, double max);

	// scale all validation data variables to [min, max] interval
	void validation_data_scale_to_interval_all_variables(double min, double max);

	// scale the selected validation data column to [min, max] interval
	void validation_data_scale_to_interval_selected_col(double min, double max, int col);

	// replace a given string in the validation data column
	// returns the number of replacements
	int validation_data_replace_symbol_from_selected_col(const char *find_what, const char* replace_with, int col, bool use_regular);

	// replace a given string in all variables of the validation data matrix
	// returns the number of replacements
	int validation_data_replace_symbol_from_all_variables(const char *find_what, const char* replace_with, bool use_regular);

	// replace a given string everywhere in the validation data matrix
	// returns the number of replacements
	int validation_data_replace_symbol_everywhere(const char *find_what, const char* replace_with, bool use_regular);

	// searches for a string in a particular col of the validation matrix
	// returns the number of findings
	int validation_data_find_symbol_from_selected_col(const char *find_what, int col, bool use_regular);

	// searches for a string in all variables of the validation matrix
	// returns the number of findings
	int validation_data_find_symbol_from_all_variables(const char *find_what, bool use_regular);

	// searches for a string in all validation matrix
	// returns the number of findings
	int validation_data_find_symbol_everywhere(const char *find_what, bool use_regular);

	// scale all test data to [min, max] interval
	void test_data_scale_to_interval_everywhere(double min, double max);

	// scale all test data variables to [min, max] interval
	void test_data_scale_to_interval_all_variables(double min, double max);

	// scale the selected test data column to [min, max] interval
	void test_data_scale_to_interval_selected_col(double min, double max, int col);

	// replace a given string in the test data column
	// returns the number of replacements
	int test_data_replace_symbol_from_selected_col(const char *find_what, const char* replace_with, int col, bool use_regular);

	// replace a given string in all variables of the test data matrix
	// returns the number of replacements
	int test_data_replace_symbol_from_all_variables(const char *find_what, const char* replace_with, bool use_regular);

	// replace a given string everywhere in the training data matrix
	// returns the number of replacements
	int test_data_replace_symbol_everywhere(const char *find_what, const char* replace_with, bool use_regular);

	// searches for a string in a particular col of the test matrix
	// returns the number of findings
	int test_data_find_symbol_from_selected_col(const char *find_what, int col, bool use_regular);

	// searches for a string in all variables of the test matrix
	// returns the number of findings
	int test_data_find_symbol_from_all_variables(const char *find_what, bool use_regular);

	// searches for a string in all test matrix
	// returns the number of findings
	int test_data_find_symbol_everywhere(const char *find_what, bool use_regular);

	// randomly re-arrange rows of the training matrix
	void training_data_shuffle(void);

	// moves rows_count rows from training matrix to validation matrix
	int move_training_data_to_validation(int rows_count);
	
	// moves rows_count rows from training matrix to test matrix
	int move_training_data_to_test(int rows_count);
	
	// moves rows_count rows from test matrix to training matrix
	int move_test_data_to_training(int rows_count);
	
	// moves rows_count rows from validation matrix to training matrix
	int move_validation_data_to_training(int rows_count);

	// clears everything
	void clear_stats(void);

	// returns an entire row of the training matrix
	double* get_training_data_row(int row);

	// returns an entire row of the validation matrix
	double* get_validation_data_row(int row);

	// returns an entire row of the test matrix
	double* get_test_data_row(int row);

	//double** get_training_data_matrix(void);

	// returns the chromosome as a C program
	char* program_as_C(int run_index, bool simplified, double *inputs);

	// returns the number of outputs of the program/
	// currently only problems with 1 output are handled
	int get_num_outputs(void);

	// init operators, parameters and clears all data
	void init(void);

	// returns the number of enabled variables
	int get_num_actual_variables(void);

	// returns true if a particular variable is enable
	bool is_variable_enabled(int index);

	// sets if a particular variable is enabled or not
	void set_variable_enable(int index, bool new_state);

	// returns true if the project has been modified
	bool is_project_modified(void);

	// sets the problem description
	void set_problem_description(const char* value);

	// returns the problem description
	char* get_problem_description(void);

	// sets if the results are cached for all training data
	// caching will speed up the process, but will take much more memmory
	void set_enable_cache_results_for_all_training_data(bool value);

	// gets if the results are cached for all training data
	bool get_enable_cache_results_for_all_training_data(void);

	// returns the memmory consumption in bytes
	long long get_memory_consumption(void);

	// returns true if parameters are correct
	bool validate_project(void);

	void compute_list_of_enabled_variables(void);
};
//-----------------------------------------------------------------
//extern t_mep mep_alg;

#endif // MEP_CLASS_H_INCLUDED
