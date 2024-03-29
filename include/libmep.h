// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef lib_mep_H
#define lib_mep_H
//-----------------------------------------------------------------
#include <thread>
#include <mutex>
//-----------------------------------------------------------------
#include "pugixml.hpp"
//-----------------------------------------------------------------
#include "mep_parameters.h"
#include "mep_data.h"
#include "mep_functions.h"
#include "mep_subpopulation.h"
#include "mep_stats.h"
#include "mep_utils.h"

//-----------------------------------------------------------------
typedef void(*f_on_progress)(void);
//-----------------------------------------------------------------
class t_mep{

private:
	char version[100];

	t_mep_parameters mep_parameters;
	t_mep_constants mep_constants;

	t_sub_population* population; // array of subpopulations
	double **cached_eval_variables_matrix_double;
	double *cached_sum_of_errors, *cached_threashold;
	unsigned int best_individual_index;
	unsigned int best_subpopulation_index;

	unsigned int best_individual_index_for_test;
	unsigned int best_subpopulation_index_for_test;

	t_mep_functions mep_operators;

	unsigned int num_selected_operators;

	int actual_operators[MEP_MAX_OPERATORS];

	bool _stopped;
	bool _stopped_signal_sent;
	int last_run_index;

	t_mep_data training_data;
	t_mep_data validation_data;
	t_mep_data test_data;

	// data transformed for time series
	t_mep_data training_data_ts;

	t_mep_statistics statistics;

	unsigned int num_total_variables;
	unsigned int target_col;

	unsigned int num_actual_variables;
	bool *variables_enabled;

	unsigned int *actual_enabled_variables;

	bool modified_project;

	char *problem_description;

	bool cache_results_for_all_training_data;

	unsigned int *random_subset_indexes;

	bool start_steady_state(unsigned int run, t_seed *seeds, double ***,
			s_value_class **array_value_class, 
			f_on_progress on_generation, f_on_progress on_new_evaluation);       // Steady-State MEP
	unsigned int tournament(const t_sub_population &pop, t_seed &seed);

	double compute_validation_error(unsigned int &, unsigned int&,
			double **eval_double, s_value_class *tmp_value_class, 
			t_seed *seeds, double &num_incorrectly_classified);

	void allocate_sub_population(t_sub_population &pop);

	void allocate_values(double ****, s_value_class***);
	void delete_values(double ****, s_value_class***);

	void sort_by_fitness(t_sub_population &pop); // sort ascending the individuals in population
	void compute_best_and_average_error(double &best_error, double &mean_error, 
			double &num_incorrectly_classified, double &average_incorrectly_classified);
	void compute_cached_eval_matrix_double2(s_value_class *array_value_class);

	void delete_sub_population(t_sub_population &pop);

	void evolve_one_subpopulation_for_one_generation(unsigned int *current_subpop_index, 
			std::mutex* mutex, t_sub_population * sub_populations, 
			int generation_index, bool recompute_fitness, 
			double ** eval_double, s_value_class *tmp_value_class, t_seed* seeds);
	void get_random_subset(unsigned int count, unsigned int *indecses, t_seed& seed);

	// transform from single column arrays to matrices
	//bool create_time_series_data(void);

	unsigned int random_subset_selection_size;
	void compute_previous_data_for_training(double* previous_data);
	void compute_previous_data_for_validation(double* previous_data);
	void compute_previous_data_for_test(double* previous_data);
	void compute_previous_data_for_prediction(double* previous_data);
    
public:

	t_mep();
	~t_mep();

	t_mep_data *get_training_data_ptr(void);
	t_mep_data *get_validation_data_ptr(void);
	t_mep_data *get_test_data_ptr(void);

	// returns the version of the library
	const char * get_version(void) const;

	// returns the index of the last run (if multiple runs are performed)
	int get_last_run_index(void) const;

	// returns the number of variables
	unsigned int get_num_total_variables(void);

	// sets the number of variables
	void set_num_total_variables(unsigned int value);

	// returns true if the process is running, false otherwise
	bool is_running(void) const;

	// starts the optimization process
	int start(f_on_progress on_generation, f_on_progress on_new_evaluation, f_on_progress on_complete_run);

	// stops the optimization process
	void stop(void);

	// gets the best chromosome
	void get_best(t_mep_chromosome& dest) const;

	// gets the output obtaining by running the best program in a given run against in input
	bool get_output(unsigned int run_index, double *inputs, double *outputs) const;

	// saves everything to an xml file
	int to_xml(const char* file_name);
	
	// saves everything to an xml file
	int from_xml(const char* file_name);

	// saves everything to a pugixml node
	int to_pugixml_node(pugi::xml_node parent);
	
	// loads everything from a pugixml node
	int from_pugixml_node(pugi::xml_node parent);

	t_mep_functions* get_functions_ptr(void);
	t_mep_constants* get_constants_ptr(void);
	t_mep_parameters* get_parameters_ptr(void);

	// clears everything
	void clear_stats(void);

	// returns the chromosome as a C program
	char* program_as_C(unsigned int run_index, bool simplified, double *inputs) const;

	char* program_as_C_infix(unsigned int run_index, double* inputs) const;
		
	// returns the chromosome as an Excel function
	char* program_as_Excel_function(unsigned int run_index, bool simplified, double* inputs) const;

	// returns the chromosome as an Python function
	char* program_as_Python(unsigned int run_index, bool simplified, double* inputs) const;

	// returns the number of outputs of the program/
	// currently only problems with 1 output are handled
	unsigned int get_num_outputs(void) const;

	// init operators, parameters and clears all data
	void init(void);

	// returns the number of enabled variables
	unsigned int get_num_actual_variables(void) const;

	// returns true if a particular variable is enable
	bool is_variable_enabled(unsigned int index) const;

	// sets if a particular variable is enabled or not
	void set_variable_enable(unsigned int index, bool new_state);

	// returns true if the project has been modified
	bool is_project_modified(void) const;

	// sets the problem description
	void set_problem_description(const char* value);

	// returns the problem description
	char* get_problem_description(void) const;

	// sets if the results are cached for all training data
	// caching will speed up the process, but will take much more memmory
	void set_enable_cache_results_for_all_training_data(bool value);

	// gets if the results are cached for all training data
	bool get_enable_cache_results_for_all_training_data(void) const;

	// returns the memory consumption in bytes
	long long get_memory_consumption(void) const;

	// returns true if parameters are correct
	bool validate_project(char* error_str);

	void compute_list_of_enabled_variables(void);

	const t_mep_statistics* get_stats_ptr(void) const;

	void init_enabled_variables(void);

	void clear(void);

	// is this a univariate time serie? we do not take window size now into account
	bool could_be_univariate_time_serie(void);

	// transform 1 column of training data to a time serie
//	bool to_time_serie(unsigned int window_size);

	// predict new values for a time serie
	bool predict(int run_index, double* output, char* valid_output);

	bool predict_on_test(int run_index, double* output, char* valid_output);

	void compute_output_on_training(int run_index, double* output, char* valid_output);
	void compute_output_on_validation(int run_index, double* output, char* valid_output);
	void compute_output_on_test(int run_index, double* output, char* valid_output);

};
//-----------------------------------------------------------------
#endif 
