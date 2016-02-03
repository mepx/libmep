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
	bool *variables_utilization;

	int *actual_used_variables;

	bool modified_project;

	char *problem_description;

	bool cache_results_for_all_training_data;

	bool start_steady_state(int seed, double ***, s_value_class **array_value_class, f_on_progress on_generation, f_on_progress on_new_evaluation);       // Steady-State MEP
	long tournament(t_sub_population &pop);

	//   void uniform_crossover(const chromosome &parent1, const chromosome &parent2, chromosome &offspring1, chromosome &offspring2);
	//void one_cut_point_crossover(const chromosome &parent1, const chromosome &parent2, chromosome &offspring1, chromosome &offspring2);
	//void mutation(chromosome &Individual); // mutate the individual

	void fitness_regression_double_cache_all_training_data(chromosome &Individual, double **);
	void fitness_classification_double_cache_all_training_data(chromosome &Individual, double**);
	void fitness_regression_double(chromosome &Individual, double* eval_array_double, double *fitness_array);

	void generate_random_individuals(void); // randomly initializes the individuals

	double compute_validation_error(int *, int*, double **eval_double);

	void allocate_sub_population(t_sub_population &pop);
	void ReadTrainingData(void);

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

	void get_list_of_used_variables(void);

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


	long get_num_automatic_constants(void);
	long get_num_user_defined_constants(void);
	double get_min_constants_interval_double(void);
	double get_max_constants_interval_double(void);

	double get_constants_double(long index);

	long get_constants_type(void);
	long get_constants_can_evolve(void);
	double get_constants_mutation_max_deviation(void);

	void set_num_automatic_constants(long value);
	void set_num_user_defined_constants(long value);
	void set_min_constants_interval_double(double value);
	void set_max_constants_interval_double(double value);

	void set_constants_double(long index, double value);

	void set_constants_type(long value);
	void  set_constants_can_evolve(long value);
	void set_constants_mutation_max_deviation(double value);

	void init_parameters(void);

	bool get_addition(void);
	bool get_subtraction(void);
	bool get_multiplication(void);
	bool get_division(void);
	bool get_power(void);
	bool get_sqrt(void);
	bool get_exp(void);
	bool get_pow10(void);
	bool get_ln(void);
	bool get_log10(void);
	bool get_log2(void);
	bool get_floor(void);
	bool get_ceil(void);
	bool get_abs(void);
	bool get_inv(void);
	bool get_x2(void);
	bool get_min(void);
	bool get_max(void);
	bool get_sin(void);
	bool get_cos(void);
	bool get_tan(void);
	bool get_asin(void);
	bool get_acos(void);
	bool get_atan(void);
	bool get_iflz(void);
	bool get_ifalbcd(void);



	void set_addition(bool value);
	void set_subtraction(bool value);
	void set_multiplication(bool value);
	void set_division(bool value);
	void set_power(bool value);
	void set_sqrt(bool value);
	void set_exp(bool value);
	void set_pow10(bool value);
	void set_ln(bool value);
	void set_log10(bool value);
	void set_log2(bool value);
	void set_floor(bool value);
	void set_ceil(bool value);
	void set_abs(bool value);
	void set_inv(bool value);
	void set_x2(bool value);
	void set_min(bool value);
	void set_max(bool value);
	void set_sin(bool value);
	void set_cos(bool value);
	void set_tan(bool value);
	void set_asin(bool value);
	void set_acos(bool value);
	void set_atan(bool value);
	void set_iflz(bool value);
	void set_ifalbcd(bool value);

	void init_operators();

	bool is_training_data_a_classification_problem(void);
	bool is_test_data_a_classification_problem(void);
	bool is_validation_data_a_classification_problem(void);

	void training_data_scale_to_interval_everywhere(double min, double max);
	void training_data_scale_to_interval_all_variables(double min, double max);
	void training_data_scale_to_interval_selected_col(double min, double max, int col);

	int training_data_replace_symbol_from_selected_col(const char *find_what, const char* replace_with, int col, bool use_regular);
	int training_data_replace_symbol_from_all_variables(const char *find_what, const char* replace_with, bool use_regular);
	int training_data_replace_symbol_everywhere(const char *find_what, const char* replace_with, bool use_regular);

	int training_data_find_symbol_from_selected_col(const char *find_what, int col, bool use_regular);
	int training_data_find_symbol_from_all_variables(const char *find_what, bool use_regular);
	int training_data_find_symbol_everywhere(const char *find_what, bool use_regular);



	void validation_data_scale_to_interval_everywhere(double min, double max);
	void validation_data_scale_to_interval_all_variables(double min, double max);
	void validation_data_scale_to_interval_selected_col(double min, double max, int col);

	int validation_data_replace_symbol_from_selected_col(const char *find_what, const char* replace_with, int col, bool use_regular);
	int validation_data_replace_symbol_from_all_variables(const char *find_what, const char* replace_with, bool use_regular);
	int validation_data_replace_symbol_everywhere(const char *find_what, const char* replace_with, bool use_regular);

	int validation_data_find_symbol_from_selected_col(const char *find_what, int col, bool use_regular);
	int validation_data_find_symbol_from_all_variables(const char *find_what, bool use_regular);
	int validation_data_find_symbol_everywhere(const char *find_what, bool use_regular);

	void test_data_scale_to_interval_everywhere(double min, double max);
	void test_data_scale_to_interval_all_variables(double min, double max);
	void test_data_scale_to_interval_selected_col(double min, double max, int col);

	int test_data_replace_symbol_from_selected_col(const char *find_what, const char* replace_with, int col, bool use_regular);
	int test_data_replace_symbol_from_all_variables(const char *find_what, const char* replace_with, bool use_regular);
	int test_data_replace_symbol_everywhere(const char *find_what, const char* replace_with, bool use_regular);

	int test_data_find_symbol_from_selected_col(const char *find_what, int col, bool use_regular);
	int test_data_find_symbol_from_all_variables(const char *find_what, bool use_regular);
	int test_data_find_symbol_everywhere(const char *find_what, bool use_regular);

	void training_data_shuffle(void);

	int move_training_data_to_validation(int count);
	int move_training_data_to_test(int count);
	int move_test_data_to_training(int count);
	int move_validation_data_to_training(int count);

	// clears everything
	void clear_stats(void);

	double* get_training_data_row(int row);
	double* get_validation_data_row(int row);
	double* get_test_data_row(int row);
	//double** get_training_data_matrix(void);

	// returns the chromosome as a C program
	char* program_as_C(int run_index, bool simplified, double *inputs);


	int get_num_outputs(void);
	void init(void);
	int get_num_actual_variables(void);
	bool is_variable_utilized(int index);
	void set_variable_utilization(int index, bool value);
	bool is_project_modified(void);

	void set_problem_description(const char* value);
	char* get_problem_description(void);

	void set_cache_results_for_all_training_data(bool value);
	bool get_cache_results_for_all_training_data(void);

	long long get_memory_consumption(void);
};
//-----------------------------------------------------------------
//extern t_mep mep_alg;

#endif // MEP_CLASS_H_INCLUDED
