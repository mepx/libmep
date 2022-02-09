// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef MEP_CHROMOSOME_CLASS_H
#define MEP_CHROMOSOME_CLASS_H
//-----------------------------------------------------------------
#include "pugixml.hpp"
#include "mep_constants.h"
#include "mep_parameters.h"
#include "mep_utils.h"
#include "mep_data.h"
#include "mep_error_functions.h"
//-----------------------------------------------------------------
struct t_code3{
	int op;            // either a variable or an operator; 
						// variables are positive 0,1,2,...; 
						// operators are negative -1, -2, -3...
	unsigned int addr1, addr2, addr3, addr4;    // pointers to arguments
};
//-----------------------------------------------------------------
class t_mep_chromosome{
private:
	unsigned int code_length;
	unsigned int num_total_variables;
	unsigned int num_constants;
	double *real_constants;
	unsigned int num_program_outputs; // do not confuse it with the number of data outputs!!!!!!!!

	t_code3 *prg;                   // a string of genes (instructions)
	t_code3 *simplified_prg;        // simplified program

	double fitness;							// the fitness
	unsigned int* index_best_genes;         // the index of the best expression in chromosome
	unsigned int max_index_best_genes;      // index of the max
	double best_class_threshold;			// only for binary classification
	unsigned int num_utilized_genes;		// number of utilized genes for regression and binary classification

	double num_incorrectly_classified;		// this is identical with fitness in some cases
	unsigned int num_classes;
	double* centers;						// center for each class;

	void mark(unsigned int position, bool* marked);
	void compute_eval_matrix_double(unsigned int num_training_data, double **cached_eval_matrix_double, 
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
		int *line_of_constants, double ** eval_double, t_seed& seed);
	

	void fitness_regression_double_cache_all_training_data(const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double** cached_eval_matrix, double * cached_sum_of_errors, 
			unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
			double **, t_mep_error_function, t_seed &seed);
	void fitness_regression_double_no_cache(const t_mep_data &mep_dataset, 
			unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double* eval_vect, double *sum_of_errors_array, t_mep_error_function);

	void fitness_binary_classification_double_cache_all_training_data(
				const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size,
				double **cached_eval_matrix, 
				double * cached_sum_of_errors, double * cached_threashold, 
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
				double **eval_matrix_double, s_value_class *tmp_value_class, t_seed &seed);

	void fitness_multi_class_classification_winner_takes_all_fixed_double_cache_all_training_data(
				const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size,
				double **cached_eval_matrix, 
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
				double **eval_matrix_double, t_seed &seed);

	void fitness_multi_class_classification_smooth_double_cache_all_training_data(
				const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size,
				double **cached_variables_eval_matrix, 
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
				double **eval_matrix_double, t_seed& seed);

	void fitness_multi_class_classification_winner_takes_all_dynamic_double_cache_all_training_data(
				const t_mep_data &mep_dataset, unsigned int *random_subset_indexes, unsigned int random_subset_selection_size,
				double **cached_eval_matrix,
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
				double **eval_matrix_double, t_seed &seed);


	void fitness_multi_class_classification_closest_center_double_cache_all_training_data(
		const t_mep_data& mep_dataset,
		unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
		double** cached_variables_eval_matrix,
		unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
		double** eval_matrix_double, t_seed& seed);
public:

	// constructor
	t_mep_chromosome();

	// destructor - deletes memory
	~t_mep_chromosome();

	void allocate_memory(unsigned int code_length, unsigned int num_total_vars, bool use_constants,
		const t_mep_constants* constants, unsigned int num_outputs, unsigned int num_classes);

	t_mep_chromosome& operator=(const t_mep_chromosome& source);
	void swap_pointers(t_mep_chromosome& source);

	//void init_and_allocate_memory(void);

	void clear(void);
	
	// set the operator inside a gene
	void set_gene_operation(unsigned int gene_index, int new_operation);


	// converts the MEP program into a C program
	// a row of data is also required because the main C program will also contain an example on how to run the obtained program
	char * to_C_double(bool simplified, double *data, 
				unsigned int problem_type, unsigned int error_measure);
	char* to_Excel_VBA_function_double(bool simplified, double* data, 
				unsigned int problem_type, unsigned int error_measure);

	double get_fitness(void);


    
	void generate_random(const t_mep_parameters &parameters, const t_mep_constants & mep_constants, 
		const int *actual_operators, unsigned int num_actual_operators,
		const unsigned int *actual_used_variables, unsigned int num_actual_used_variables, t_seed& seed);
    
	int compare(const t_mep_chromosome &other, bool minimize_operations_count);

	void mutation(const t_mep_parameters &parameters, 
			const t_mep_constants & mep_constants, const int *actual_operators, unsigned int num_actual_operators,
		const unsigned int *actual_used_variables, unsigned int num_actual_used_variables, t_seed& seed);

	void one_cut_point_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, 
							const t_mep_constants & mep_constants, t_seed& seed);
	void uniform_crossover(const t_mep_chromosome &parent2, 
					t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, const t_mep_constants & mep_constants, t_seed& seed);

	void compute_fitness(const t_mep_data& mep_dataset, const t_mep_data& mep_dataset_ts,
		unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
		double** cached_eval_matrix, double* cached_sum_of_errors, 
		double* cached_threashold, s_value_class* tmp_value_class,
		unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
		double**, unsigned int problem_type, unsigned int error_measure, 
		t_seed& seed);

	void fitness_regression(const t_mep_data &mep_dataset, 
			unsigned int *random_subset_indexes, unsigned int random_subset_selection_size,
			double** cached_eval_matrix, double * cached_sum_of_errors, 
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
			double **, t_mep_error_function, t_seed &seed);

	void fitness_binary_classification(const t_mep_data &mep_dataset, 
			unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double **cached_eval_matrix, double * cached_sum_of_errors, double * cached_threashold, 
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
		double **eval_matrix_double, s_value_class *tmp_value_class, t_seed &seed);

	void fitness_multi_class_classification_smooth(const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
		double **cached_eval_matrix, unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
		double **eval_matrix_double, t_seed &seed);

	void fitness_multi_class_classification_winner_takes_all_fixed(const t_mep_data &mep_dataset, 
			unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double **cached_eval_matrix, unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
			double **eval_matrix_double, t_seed &seed);

	void fitness_multi_class_classification_winner_takes_all_dynamic(const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double **cached_eval_matrix, unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
		double **eval_matrix_double, t_seed &seed);

	bool compute_binary_classification_error_on_double_data_return_error(double **inputs, 
		unsigned int num_data, unsigned int output_col, double &error, unsigned int &index_error_gene);

	bool compute_binary_classification_error_on_double_data(double **data, unsigned int num_data, 
				unsigned int output_col, double &error);
	
	bool compute_multi_class_classification_error_on_double_data_return_error(double **inputs, 
		unsigned int num_data, unsigned int output_col,
		double &error, unsigned int &index_error_gene, double &num_incorrectly_classified);
		
	bool compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data_return_error(double **data, 
		unsigned int num_data, unsigned int output_col, 
		double &error, unsigned int &index_error_gene, double &_num_incorrectly_classified);

	bool compute_multi_class_classification_winner_takes_all_fixed_error_on_double_data(double **data, 
		unsigned int num_data, unsigned int output_col, double &error);

	bool compute_multi_class_classification_smooth_error_on_double_data(double **data, 
		unsigned int num_data, unsigned int output_col,double &error);

	bool compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data(double **data, 
		unsigned int num_data, unsigned int output_col, double &error);

	bool compute_regression_error_on_double_data_return_error(double **inputs, 
			unsigned int num_data, unsigned int output_col, 
			double &error, unsigned int &index_error_gene, const t_mep_error_function mep_error_function);

	bool compute_regression_error_on_double_data(double **data, unsigned int num_data, unsigned int output_col, 
			double &error, const t_mep_error_function mep_error_function);

	bool compute_time_series_error_on_double_data_return_error(double* previous_data, unsigned int window_size,
		double** inputs,
		unsigned int num_data,
		double& error, unsigned int& index_error_gene, const t_mep_error_function mep_error_function);

	bool compute_time_series_error_on_double_data(double* previous_data, unsigned int window_size,
		double** data,
		unsigned int num_data,
		double& error, const t_mep_error_function mep_error_function);

	bool compute_time_series_output_on_given_double_data(
		double* previous_data, unsigned int window_size,
		double** data, unsigned int num_data, 
		double* output, char* valid_data);

	bool compute_time_series_output_on_predicted_double_data(
		double* previous_data, unsigned int window_size,
		unsigned int num_data, unsigned int& index_error_gene,
		double* output, char* valid_data);

	bool compute_multi_class_classification_closest_center_error_on_double_data_return_error(
		double** data, unsigned int num_data, unsigned int output_col,
		double& error, unsigned int& index_error_gene, double& _num_incorrectly_classified);
	bool compute_multi_class_classification_closest_center_error_on_double_data(
		double** data, unsigned int num_data, unsigned int output_col, double& error);

	void fitness_multi_class_classification_closest_center(
		const t_mep_data& mep_dataset,
		unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
		double** cached_variables_eval_matrix,
		unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
		double** eval_matrix_double,
		t_seed& seed);

	bool evaluate_double(double *inputs, double *outputs, unsigned int &index_error_gene);
	bool get_first_max_index(double *inputs, unsigned int &max_index, double& max_value,
							unsigned int &index_error_gene, 
							double* values_for_output_genes);
	bool get_error_double(double *inputs, double *outputs);

	unsigned int get_closest_class_index(double program_output);

	void simplify(void);

	unsigned int get_index_best_gene(unsigned int output_index);
	double get_best_class_threshold(void);

	double get_num_incorrectly_classified(void);

	bool get_class_index_for_winner_takes_all_dynamic(double* inputs, unsigned int& class_index);

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent);
};
//-----------------------------------------------------------------
#endif