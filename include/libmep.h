#ifndef MEP_CLASS_H_INCLUDED
#define MEP_CLASS_H_INCLUDED

#include <thread>
#include <mutex>

#include "pugixml.hpp"


#include "parameters_class.h"
#include "data_class.h"
#include "operators_class.h"
#include "umystring.h"
#include "chromosome_class.h"
#include "mep_stats.h"
#include "utils_mep.h"

//-----------------------------------------------------------------
typedef void (*f_on_progress)(void);
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

public:
	int get_last_run_index(void);
	char * get_version(void);
	int *actual_used_variables;
	int num_actual_variables;

	int *variables_utilization;
	bool modified_project;
	
	t_mep_stat *stats;


	int num_total_vars;
	int target_col;
    
public:
	//---------------------------------------------------------------------------
	int load_training_data_from_csv(const char* file_name);
	int save_training_data_to_csv(const char* file_name, char list_separator);
	int get_training_data_type(void);
	int get_training_data_num_columns(void);
	int get_training_data_num_rows(void);
	// assumes that row and col are valid; no test for out of range are performed
	char *get_training_data_as_string(int row, int col);
	// assumes that row and col are valid; no test for out of range are performed
	double get_training_data_as_double(int row, int col);
	// clears the training data internal structurs
	void clear_training_data(void);

	void training_data_to_numeric(void);
	

	//---------------------------------------------------------------------------
	int load_validation_data_from_csv(const char* file_name);
	int save_validation_data_to_csv(const char* file_name, char list_separator);
	int get_validation_data_type(void);
	int get_validation_data_num_columns(void);
	int get_validation_data_num_rows(void);
	// assumes that row and col are valid; no test for out of range are performed
	char *get_validation_data_as_string(int row, int col);
	// assumes that row and col are valid; no test for out of range are performed
	double get_validation_data_as_double(int row, int col);
	// clears the training data internal structurs
	void clear_validation_data(void);

	void validation_data_to_numeric(void);

	//---------------------------------------------------------------------------
	int load_test_data_from_csv(const char* file_name);
	int save_test_data_to_csv(const char* file_name, char list_separator);
	int get_test_data_type(void);
	int get_test_data_num_columns(void);
	int get_test_data_num_rows(void);
	// assumes that row and col are valid; no test for out of range are performed
	char *get_test_data_as_string(int row, int col);
	// assumes that row and col are valid; no test for out of range are performed
	double get_test_data_as_double(int row, int col);
	// clears the training data internal structurs
	void clear_test_data(void);

	void test_data_to_numeric(void);
	//---------------------------------------------------------------------------

	// returns true if the process is running, false otherwise
	bool is_running(void);

	// starts the optimization process
	int start(f_on_progress on_generation, f_on_progress on_new_evaluation, f_on_progress on_complete_run);    
	double get_best_training_error(int run, int gen);
	double get_best_validation_error(int run);
	double get_average_training_error(int run, int gen);
	double get_running_time(int run);
	int get_latest_generation(int run);
	double get_test_error(int run);

	t_mep();
	~t_mep();

	void stop(void);

	void delete_sub_population(t_sub_population &pop);
	void get_best(chromosome& dest);

	int stats_to_csv(const char* file_name);

	bool evaluate_double(chromosome &Individual, double *inputs, double *outputs);
	bool get_error_double(chromosome &Individual, double *inputs, double *outputs);
	//void evolve_one_subpopulation_for_one_generation(t_sub_population &pop, double**, s_value_class *tmp_value_class);
	//void evolve_one_subpopulation_for_one_generation(int *current_subpop_index, std::mutex* mutex, t_sub_population * sub_populations, int generation_index, t_parameters &params, double **training_data, int num_training_data, int num_variables, double ** eval_double, s_value_class *tmp_value_class);
	void evolve_one_subpopulation_for_one_generation(int *current_subpop_index, std::mutex* mutex, t_sub_population * sub_populations, int generation_index, double ** eval_double, s_value_class *tmp_value_class);


	void fitness_regression(chromosome &Individual, double **);
	void fitness_classification(chromosome &individual, double **, s_value_class *);
	void fitness_classification_double2(chromosome &Individual, double **eval_double, s_value_class *);

	void sort_stats_by_running_time(bool ascending);
	void sort_stats_by_training_error(bool ascending);
	void sort_stats_by_validation_error(bool ascending);
	void sort_stats_by_test_error(bool ascending);

	void get_list_of_used_variables(void);

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent);

private:


	bool start_steady_state(int seed, double ***, s_value_class **array_value_class, f_on_progress on_generation, f_on_progress on_new_evaluation);       // Steady-State MEP
	long tournament(t_sub_population &pop);

 //   void uniform_crossover(const chromosome &parent1, const chromosome &parent2, chromosome &offspring1, chromosome &offspring2);
	//void one_cut_point_crossover(const chromosome &parent1, const chromosome &parent2, chromosome &offspring1, chromosome &offspring2);
    //void mutation(chromosome &Individual); // mutate the individual

	void fitness_regression_double(chromosome &Individual, double **);
	void fitness_classification_double(chromosome &Individual, double**);

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

	public:

		double get_mutation_probability(void);                   // mutation probability
		double get_crossover_probability(void);               // crossover probability
		long get_code_length(void);              // the number of genes

		long get_subpopulation_size(void);                 // the number of individuals in population  (must be an odd number!!!)
		long get_num_threads(void);
		long get_tournament_size(void);
		long get_num_generations(void);
		long get_problem_type(void); // 0- regression, 1-classification

		long get_num_subpopulations(void);
		double get_operators_probability(void);
		double get_variables_probability(void);
		double get_constants_probability(void);
		long get_use_validation_data(void);
		long get_crossover_type(void);

		long get_random_seed(void);
		long get_num_runs(void);

		long get_simplified_programs(void);

		void set_mutation_probability(double value);                   // mutation probability
		void set_crossover_probability(double value);               // crossover probability
		void set_code_length(long value);              // the number of genes

		void set_subpopulation_size(long value);                 // the number of individuals in population  (must be an odd number!!!)
		void set_num_threads(long value);
		void set_tournament_size(long value);
		void set_num_generations(long value);
		void set_problem_type(long value); // 0- regression, 1-classification

		void set_num_subpopulations(long value);
		void set_operators_probability(double value);
		void set_variables_probability(double value);
		void set_constants_probability(double value);
		void set_use_validation_data(long value);
		void set_crossover_type(long value);

		void set_random_seed(long value);
		void set_num_runs(long value);

		void set_simplified_programs(long value);


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

		 void clear(void);

		 double* get_training_data_row(int row);
		 double* get_validation_data_row(int row);
		 double* get_test_data_row(int row);
		 //double** get_training_data_matrix(void);

		 char* prg_to_C(int run_index, bool simplified, double *inputs);

		 int get_num_outputs(void);
};
//-----------------------------------------------------------------
//extern t_mep mep_alg;

#endif // MEP_CLASS_H_INCLUDED
