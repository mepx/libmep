// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef MEP_CHROMOSOME_H
#define MEP_CHROMOSOME_H
//-----------------------------------------------------------------
#include "mep_constants.h"
#include "mep_parameters.h"
#include "mep_binary_classification_utils.h"
#include "mep_data.h"
#include "mep_loss_functions.h"
#include "utils/pugixml.hpp"
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
	char data_type;

	unsigned int code_length;
	unsigned int num_total_variables;
	unsigned int num_constants;

	double *real_constants;
	long long* long_constants;

	unsigned int num_program_outputs; // do not confuse it with the number of data outputs!!!!!!!!

	t_code3 *prg;                   // a string of genes (instructions)
	t_code3 *simplified_prg;        // simplified program

	double fitness;							// the fitness
	unsigned int* index_best_genes;         // the index of the best expression in chromosome
	unsigned int* index_best_genes_simplified;         // the index of the best expression in chromosome
	
	unsigned int max_index_best_genes;      // index of the max
	double best_class_threshold;			// only for binary classification
	int num_utilized_genes;		// number of utilized genes for regression and binary classification

	double num_incorrectly_classified;		// this is identical with fitness in some cases
	unsigned int num_classes;
	//int *class_index;
	double* centers;						// center for each class;
	
	unsigned int problem_type;
	unsigned int error_measure;
	
	int *class_labels;

	void mark(unsigned int position, char* marked);
	
	void compute_eval_matrix(unsigned int num_training_data,
							 double **cached_eval_matrix_double,
							 unsigned int num_actual_variables,
							 unsigned int * actual_enabled_variables,
							 int *line_of_constants,
							 double ** eval_double,
							 t_seed& seed);
	
	void compute_eval_matrix(unsigned int num_training_data,
							 long long **cached_eval_matrix,
							 unsigned int num_actual_variables,
							 unsigned int * actual_enabled_variables,
							 int *line_of_constants,
							 long long ** eval,
							 t_seed& seed);
	
	void fitness_regression_cache_all_training_data(const t_mep_data &mep_dataset,
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double** cached_eval_matrix,
													double ** cached_sum_of_errors,
			unsigned int num_actual_variables,
													unsigned int * actual_enabled_variables,
													
			double **, char *gene_used_for_output, t_mep_error_function_double,
													t_seed &seed);
	
	
	void fitness_regression_cache_all_training_data(const t_mep_data &mep_dataset,
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size,
			long long** cached_eval_matrix, double ** cached_sum_of_errors,
			unsigned int num_actual_variables,
													unsigned int * actual_enabled_variables,
			long long **,
													char *gene_used_for_output,
													t_mep_error_function_long_long,
													t_seed &seed);
	
	void fitness_regression_no_cache(const t_mep_data &mep_dataset,
			unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double* eval_vect, double *sum_of_errors_array,
									 t_mep_error_function_double);

	void fitness_binary_classification_double_cache_all_training_data(
				const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size,
				double **cached_eval_matrix, 
				double ** cached_sum_of_errors, double * cached_threashold, 
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
				double **eval_matrix_double, s_value_class *tmp_value_class,
				t_seed &seed);

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
				double **eval_matrix_double,
		t_seed& seed);

	void fitness_multi_class_classification_winner_takes_all_dynamic_double_cache_all_training_data(
				const t_mep_data &mep_dataset, unsigned int *random_subset_indexes, unsigned int random_subset_selection_size,
				double **cached_eval_matrix,
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
				double **eval_matrix_double,
		t_seed &seed);

	void fitness_multi_class_classification_closest_center_double_cache_all_training_data(
		const t_mep_data& mep_dataset,
		unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
		double** cached_variables_eval_matrix,
		unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
		double** eval_matrix_double,
		t_seed& seed);
	
	void code_to_infix_C(unsigned int mep_index, char* &tmp_s, size_t& capacity);
	void code_to_Latex(unsigned int mep_index, char* &tmp_s, size_t& capacity);
	
	void print_example_of_utilization_C(double** data_double,
										long long** data_long,
										char* prog);
	void print_MEP_code_C(char* s_prog,
						  const t_code3* mep_code,
						  unsigned int num_utilized_instructions);
	void print_output_calculation_C(char* s_prog,
									int num_utilized_instructions,
									const unsigned int *index_output_instructions);
	
	void print_output_calculation_Excel_VBA(char* s_prog,
											int num_utilized_instructions,
											const unsigned int *index_output_instructions);
	void print_MEP_code_Excel_VBA(char* s_prog,
								  const t_code3* mep_code,
								  unsigned int num_utilized_instructions);
	
	void print_example_of_utilization_Python(double** data_double,
										long long** data_long,
										char* prog);
	void print_MEP_code_Python(char* s_prog,
							   const t_code3* mep_code,
							   unsigned int num_utilized_instructions);

	void print_output_calculation_Python(char* s_prog,
										 int num_utilized_instructions,
										 const unsigned int *index_output_instructions);
public:

	// constructor
	t_mep_chromosome();

	// destructor - deletes memory
	~t_mep_chromosome();

	void allocate_memory(unsigned int code_length, unsigned int num_total_vars,
						 bool use_constants,
						 const t_mep_constants* constants,
						 unsigned int num_outputs, unsigned int num_classes,
						 unsigned int problem_type, unsigned int error_measure,
						 const int* training_class_labels,
						 char _data_type);

	t_mep_chromosome& operator=(const t_mep_chromosome& source);
	void swap_pointers(t_mep_chromosome& source);

	//void init_and_allocate_memory(void);

	void clear(void);
	
	// set the operator inside a gene
	void set_gene_operation(unsigned int gene_index, int new_operation);

	// converts the MEP program into a C program
	// a row of data is also required because the main C program will also contain an example on how to run the obtained program
	char * to_C_code(bool simplified,
					 double **data_double, long long** data_long,
					 const char* libmep_version);

	char* to_Excel_VBA_function_code(bool simplified,
									const char* libmep_version);
	
	char* to_Python_code(bool simplified,
						 double **data_double,
						 long long** data_long,
						const char* libmep_version);

	char* to_C_infix_code(double **data_double,
						  long long** data_long,
						  const char* libmep_version);
	
	char* to_Latex_code(const char* libmep_version);

	double get_fitness(void) const;
    
	void generate_random(const t_mep_parameters &parameters, 
						 const t_mep_constants & mep_constants,
						 const int *actual_operators, unsigned int num_actual_operators,
						 const unsigned int *actual_used_variables, unsigned int num_actual_used_variables, 
						 t_seed& seed);
    
	// -1 if this is better than other, 1 if this is worse than other; 0 if are equal
	int compare(const t_mep_chromosome &other, double precision);

	void mutation(const t_mep_parameters &parameters, 
				  const t_mep_constants & mep_constants, 
				  const int *actual_operators, unsigned int num_actual_operators,
				  const unsigned int *actual_used_variables, 
				  unsigned int num_actual_used_variables, t_seed& seed);

	void one_cut_point_crossover(const t_mep_chromosome &parent2, 
								 t_mep_chromosome &offspring1, t_mep_chromosome &offspring2,
								 const t_mep_constants & mep_constants,
								 t_seed& seed);
	void uniform_crossover(const t_mep_chromosome &parent2,
						   t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, 
						   const t_mep_constants & mep_constants,
						   t_seed& seed);

	void compute_fitness(const t_mep_data& mep_dataset, const t_mep_data& mep_dataset_ts,
						 unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
						 double** cached_eval_matrix_double, long long** cached_eval_matrix_long,
						 double** cached_sum_of_errors,
						 double* cached_threashold, s_value_class* tmp_value_class,
						 unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
						 double** eval_double, long long** eval_long,
						 char* gene_used_for_output,
						 t_seed& seed);
	/*
	void compute_fitness(const t_mep_data& mep_dataset, const t_mep_data& mep_dataset_ts,
		unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
		long long** cached_eval_matrix, double* cached_sum_of_errors,
		double* cached_threashold, s_value_class* tmp_value_class,
		unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
						 long long**,
		t_seed& seed);
*/
	void fitness_regression(const t_mep_data &mep_dataset, 
							unsigned int *random_subset_indexes,
							unsigned int random_subset_selection_size,
							double** cached_variables_eval_matrix_double,
							long long** cached_variables_eval_matrix_long,
							double ** cached_sum_of_errors,
							unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
							double ** eval_matrix_double, long long** eval_matrix_long,
							char *gene_used_for_output,
							t_mep_error_function_double mep_error_function_double,
							t_mep_error_function_long_long mep_error_function_long_long,
							t_seed &seed);

	void fitness_binary_classification(const t_mep_data &mep_dataset, 
									   unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double **cached_eval_matrix, double ** cached_sum_of_errors,
									   double * cached_threashold,
		unsigned int num_actual_variables, unsigned int * actual_enabled_variables, 
		double **eval_matrix_double, s_value_class *tmp_value_class, 
		t_seed &seed);

	void fitness_multi_class_classification_smooth(const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
		double **cached_eval_matrix, unsigned int num_actual_variables,
												   unsigned int * actual_enabled_variables, 
		double **eval_matrix_double, 
		t_seed &seed);

	void fitness_multi_class_classification_winner_takes_all_fixed(const t_mep_data &mep_dataset, 
			unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double **cached_eval_matrix, 
			unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
			double **eval_matrix_double,
		 t_seed &seed);

	void fitness_multi_class_classification_winner_takes_all_dynamic(const t_mep_data &mep_dataset, 
		unsigned int *random_subset_indexes, unsigned int random_subset_selection_size, 
			double **cached_eval_matrix, unsigned int num_actual_variables,
			unsigned int * actual_enabled_variables,
		double **eval_matrix_double, 
		t_seed &seed);

	bool compute_binary_classification_error_on_double_data_return_error(
		const t_mep_data& mep_data, double &error, unsigned int &index_error_gene) const;

	bool compute_binary_classification_error_on_double_data(
                const t_mep_data &mep_data, double &error) const;
	
	bool compute_multi_class_classification_error_on_double_data_return_error(const t_mep_data &mep_data,
		double &error, unsigned int &index_error_gene, double &num_incorrectly_classified) const;
		
	bool compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data_return_error(const t_mep_data &mep_data,
		double &error, unsigned int &index_error_gene, double &_num_incorrectly_classified) const;

	bool compute_multi_class_classification_winner_takes_all_fixed_error_on_double_data(const t_mep_data &mep_data, double &error) const;

	bool compute_multi_class_classification_smooth_error_on_double_data(
							const t_mep_data &mep_data, double &error) const;

	bool compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data(
						const t_mep_data &mep_data, double &error) const;

	bool compute_regression_error_on_data_return_error(
		const t_mep_data& mep_data,
	   double *error_per_output, double &total_error,
	   unsigned int& index_error_gene,
		t_mep_error_function_double mep_error_function_double,
	   t_mep_error_function_long_long mep_error_function_long_long) const;
	
	bool compute_regression_error_on_data_return_error_double(const t_mep_data &mep_data,
			double *error_per_output, double &total_error,
			unsigned int &index_error_gene,
			const t_mep_error_function_double mep_error_function) const;
	
	bool compute_regression_error_on_data_return_error_long(const t_mep_data &mep_data,
															double *error_per_output, double &total_error,
															unsigned int &index_error_gene,
			const t_mep_error_function_long_long mep_error_function) const;
	
	bool compute_regression_error_on_data(
		const t_mep_data& mep_data,
										  double *error_per_output, double &total_error,
		t_mep_error_function_double mep_error_function_double,
		t_mep_error_function_long_long mep_error_function_long_long) const;
	
	bool compute_regression_error_on_data_double(const t_mep_data &mep_data,
												 double *error_per_output, double &total_error,
						const t_mep_error_function_double mep_error_function) const;
	bool compute_regression_error_on_data_long(const t_mep_data &mep_data,
											   double *error_per_output, double &total_error,
											   const t_mep_error_function_long_long mep_error_function) const;
	
	bool compute_time_series_error_on_data_return_error(double* previous_data,
														unsigned int window_size,
														const t_mep_data &mep_data,
														double *error_per_output,
														double& total_error,
		unsigned int& index_error_gene,
		const t_mep_error_function_double mep_error_function) const;
	
	bool compute_time_series_error_on_data_return_error(long long* previous_data,
														unsigned int window_size,
														const t_mep_data &mep_data,
														double *error_per_output,
														double& total_error,
														unsigned int& index_error_gene,
		const t_mep_error_function_long_long mep_error_function) const;

	bool compute_time_series_error_on_data(double* previous_data,
										   unsigned int window_size,
										   const t_mep_data &mep_data,
										   double *error_per_output,
										   double& total_error,
										   const t_mep_error_function_double mep_error_function) const;
	
	bool compute_time_series_error_on_data(long long* previous_data,
										   unsigned int window_size,
										   const t_mep_data &mep_data,
										   double *error_per_output,
										   double& total_error,
										   const t_mep_error_function_long_long mep_error_function) const;

	bool compute_time_series_output_on_given_data(
		double* previous_data, unsigned int window_size,
												double** data,
												  unsigned int num_data,
		double** output, char* valid_data) const;
	
	bool compute_time_series_output_on_given_data(
		long long* previous_data, unsigned int window_size,
												  long long** data,
												  unsigned int num_data,
		long long** output, char* valid_data) const;

	bool compute_time_series_output_on_predicted_data(
		double* previous_data, unsigned int window_size,
		unsigned int num_data, unsigned int& index_error_gene,
		double** output, char* valid_data) const;
	
	bool compute_time_series_output_on_predicted_data(
		long long* previous_data, unsigned int window_size,
		unsigned int num_data, unsigned int& index_error_gene,
		long long** output, char* valid_data) const;

	bool compute_multi_class_classification_closest_center_error_on_double_data_return_error(
		const t_mep_data &mep_data,
		double& error, unsigned int& index_error_gene,
		double& _num_incorrectly_classified) const;
	
	bool compute_multi_class_classification_closest_center_error_on_double_data(
		const t_mep_data &mep_data, double& error) const;

	void fitness_multi_class_classification_closest_center(
		const t_mep_data& mep_dataset,
		unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
		double** cached_variables_eval_matrix,
		unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
		double** eval_matrix_double,
		t_seed& seed);

	bool evaluate(double *inputs,
				  double *outputs,
				  unsigned int &index_error_gene) const;
	bool evaluate(long long *inputs,
				  long long *outputs,
				  unsigned int &index_error_gene) const;
	
	bool get_first_max_index(double *inputs, unsigned int &max_index, double& max_value,
							unsigned int &index_error_gene, 
							double* values_for_output_genes) const;
	
	bool get_error_double(double *inputs, double *outputs) const;

	unsigned int get_closest_class_index_from_center(double program_output) const;

	void simplify(void);

	unsigned int get_index_best_gene(unsigned int output_index) const;
	double get_best_class_threshold(void) const;

	double get_num_incorrectly_classified(void) const;

	bool get_class_index_for_winner_takes_all_dynamic(double* inputs, unsigned int& class_index) const;

	void to_xml_node(pugi::xml_node parent) const;
	int from_xml_node(pugi::xml_node parent,
					  unsigned int _problem_type,
					  unsigned int _error_measure,
					  unsigned int _training_num_classes,
					  unsigned int _code_length);
	
	unsigned int get_problem_type(void) const;
	
	int get_class_label(unsigned int class_index) const;
	int count_num_utilized_genes(void);
	int get_num_utilized_genes(void);
};
//-----------------------------------------------------------------
#endif
