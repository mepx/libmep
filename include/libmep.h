#ifndef MEP_CLASS_H_INCLUDED
#define MEP_CLASS_H_INCLUDED

#include <thread>
#include <mutex>

#include "pugixml.hpp"

#include "mep_parameters.h"
#include "mep_data.h"
#include "mep_operators.h"
#include "mep_chromosome.h"
#include "mep_stats.h"
#include "mep_utils.h"

//-----------------------------------------------------------------
typedef void(*f_on_progress)(void);
//-----------------------------------------------------------------

//-----------------------------------------------------------------
struct t_sub_population{
	t_mep_chromosome *individuals;
	t_mep_chromosome offspring1, offspring2;
};
//-----------------------------------------------------------------

class t_mep
{

private:
	char version[100];

	t_mep_parameters *mep_parameters;
	t_mep_constants *mep_constants;
	t_sub_population* pop;
	double **cached_eval_matrix_double;
	double *cached_sum_of_errors, *cached_threashold;
	int best_individual_index;
	int best_subpopulation_index;

	int best_individual_index_for_test;
	int best_subpopulation_index_for_test;

	t_mep_operators *operators;

	int num_operators;

	int actual_operators[MAX_OPERATORS];

	bool _stopped;
	int last_run_index;

	t_mep_data* training_data;
	t_mep_data* validation_data;
	t_mep_data* test_data;

	t_mep_statistics *stats;

	bool get_error_double(t_mep_chromosome &Individual, double *inputs, double *outputs);
	bool evaluate_double(t_mep_chromosome &Individual, double *inputs, double *outputs);


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

	void fitness_regression_double_cache_all_training_data(t_mep_chromosome &Individual, double **);
	void fitness_classification_double_cache_all_training_data(t_mep_chromosome &Individual, double**);
	void fitness_regression_double(t_mep_chromosome &Individual, double* eval_array_double, double *fitness_array);

	void generate_random_individuals(void); // randomly initializes the individuals

	double compute_validation_error(int *, int*, double **eval_double);

	void allocate_sub_population(t_sub_population &pop);

	void allocate_values(double ****, s_value_class***);
	void delete_values(double ****, s_value_class***);

	void sort_by_fitness(t_sub_population &pop); // sort ascending the individuals in population
	void compute_best_and_average_error(double &best_error, double &mean_error);
	void compute_eval_matrix_double(t_mep_chromosome &Individual, double **, int*);
	void compute_eval_vector_double(t_mep_chromosome &Individual);
	void compute_cached_eval_matrix_double(void);
	void compute_cached_eval_matrix_double2(s_value_class *array_value_class);


	bool compute_regression_error_on_double_data(t_mep_chromosome &individual, double **inputs, int num_data, double ** data, double *error);
	bool compute_classification_error_on_double_data(t_mep_chromosome &individual, double **inputs, int num_data, double ** data, double *error);
	bool compute_regression_error_on_double_data_return_error(t_mep_chromosome &individual, double **inputs, int num_data, double ** data, double *error);
	bool compute_classification_error_on_double_data_return_error(t_mep_chromosome &individual, double **inputs, int num_data, double ** data, double *error);

	void delete_sub_population(t_sub_population &pop);

	void evolve_one_subpopulation_for_one_generation(int *current_subpop_index, std::mutex* mutex, t_sub_population * sub_populations, int generation_index, double ** eval_double, s_value_class *tmp_value_class);


	void fitness_regression(t_mep_chromosome &Individual, double **);
	void fitness_classification(t_mep_chromosome &individual, double **, s_value_class *);
	void fitness_classification_double_cache_all_training_data(t_mep_chromosome &Individual, double **eval_double, s_value_class *);


public:

	t_mep();
	~t_mep();

	void set_training_data(t_mep_data *_data);
	void set_validation_data(t_mep_data *_data);
	void set_test_data(t_mep_data *_data);

	// returns the version of the library
	const char * get_version(void);

	// returns the index of the last run (if multiple runs are performed)
	int get_last_run_index(void);

	// returns the number of variables
	int get_num_total_variables(void);

	// sets the number of variables
	void set_num_total_variables(int value);

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
	void get_best(t_mep_chromosome& dest);

	// save statistics to csv file
	int stats_to_csv(const char* file_name);

	// gets the output obtaining by running the best program in a given run against in input
	bool get_output(int run_index, double *inputs, double *outputs);

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



	void set_operators(t_mep_operators *mep_operators);
	void set_constants(t_mep_constants *mep_constants);
	void set_parameters(t_mep_parameters *mep_parameters);

	// clears everything
	void clear_stats(void);

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

	// returns the memory consumption in bytes
	long long get_memory_consumption(void);

	// returns true if parameters are correct
	bool validate_project(void);

	void compute_list_of_enabled_variables(void);
};
//-----------------------------------------------------------------
//extern t_mep mep_alg;

#endif // MEP_CLASS_H_INCLUDED
