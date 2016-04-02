#ifndef CHROMOSOME_CLASS_H
#define CHROMOSOME_CLASS_H

#include "pugixml.hpp"
#include "mep_constants.h"
#include "mep_parameters.h"
#include "mep_utils.h"
#include "mep_data.h"

typedef double (*t_mep_error_function)(double, double);

double mep_absolute_error(double x, double y);
double mep_squared_error(double x, double y);

//-----------------------------------------------------------------
struct code3
{
	int op;            // either a variable or an operator; variables are 0,1,2,...; operators are -1, -2, -3...
	int adr1, adr2, adr3, adr4;    // pointers to arguments
};
//-----------------------------------------------------------------
class t_mep_chromosome
{
private:
	long code_length;
	int num_total_variables;
	long num_constants;
	double *constants_double;

	void mark(int k, bool* marked);
	void compute_eval_matrix_double(int num_training_data, double **cached_eval_matrix_double, int num_actual_variables, int * actual_enabled_variables, int *line_of_constants, double ** eval_double);
	

	void fitness_regression_double_cache_all_training_data(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double** cached_eval_matrix, double * cached_sum_of_errors, int num_actual_variables, int * actual_enabled_variables, double **, t_mep_error_function);
	void fitness_regression_double_no_cache(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double* eval_vect, double *sum_of_errors_array, t_mep_error_function);

	void fitness_binary_classification_double_cache_all_training_data(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double **cached_eval_matrix, double * cached_sum_of_errors, double * cached_threashold, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double, s_value_class *tmp_value_class);
	void fitness_multi_class_classification_double_cache_all_training_data(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double **cached_eval_matrix, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double);

	double fitness;               // the fitness
	int index_best_gene;          // the index of the best expression in chromosome
	double best_class_threshold;  // only for binary classification
	int num_utilized;             // number of utilized genes for regression and binary classification

	code3 *prg;                   // a string of genes
	code3 *simplified_prg;        // simplified prg

public:

	t_mep_chromosome();
	~t_mep_chromosome();

	// set the operator inside a gene
	void set_gene_operation(int gene_index, int new_operation);

	void clear(void);
	char * to_C_double(bool simplified, double *data, int problem_type, int num_classes);

	double get_fitness(void);
	void init_and_allocate_memory();

	t_mep_chromosome& operator=(const t_mep_chromosome &source);

	void allocate_memory(long code_length, int num_total_vars, bool use_constants, t_mep_constants *constants);
    
	void generate_random(t_mep_parameters *parameters, t_mep_constants * mep_constants, int *actual_operators, int num_actual_operators, int *actual_used_variables, int num_actual_used_variables);
    
	int compare(t_mep_chromosome *other, bool minimize_operations_count);

	void mutation(t_mep_parameters *parameters, t_mep_constants * mep_constants, int *actual_operators, int num_actual_operators, int *actual_used_variables, int num_actual_used_variables);
	void one_cut_point_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, t_mep_parameters *parameters, t_mep_constants * mep_constants);
	void uniform_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, t_mep_parameters *parameters, t_mep_constants * mep_constants);

	void fitness_regression(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double** cached_eval_matrix, double * cached_sum_of_errors, int num_actual_variables, int * actual_enabled_variables, double **, t_mep_error_function);
	void fitness_binary_classification(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double **cached_eval_matrix, double * cached_sum_of_errors, double * cached_threashold, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double, s_value_class *tmp_value_class);
	void fitness_multiclass_classification(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double **cached_eval_matrix, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double);
	
	bool compute_binary_classification_error_on_double_data_return_error(double **inputs, int num_data, int output_col, double &error, int &index_error_gene);
	bool compute_binary_classification_error_on_double_data(double **data, int num_data, int output_col, double &error);

	bool compute_multiclass_classification_error_on_double_data_return_error(double **inputs, int num_data, int output_col, int num_classes, double &error, int &index_error_gene);
	bool compute_multiclass_classification_error_on_double_data(double **data, int num_data, int output_col, int num_classes, double &error);

	bool compute_regression_error_on_double_data_return_error(double **inputs, int num_data, int output_col, double &error, int &index_error_gene, t_mep_error_function mep_error_function);
	bool compute_regression_error_on_double_data(double **data, int num_data, int output_col, double &error, t_mep_error_function mep_error_function);

	bool evaluate_double(double *inputs, double *outputs, int &index_error_gene);
	bool get_first_max_index(double *inputs, int &max_index, int &index_error_gene);
	bool get_error_double(double *inputs, double *outputs);

	void simplify(void);

	int get_index_best_gene(void);
	double get_best_class_threshold(void);

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent);
};
//-----------------------------------------------------------------
#endif