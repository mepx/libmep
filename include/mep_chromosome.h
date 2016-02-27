#ifndef CHROMOSOME_CLASS_H
#define CHROMOSOME_CLASS_H

#include "pugixml.hpp"
#include "mep_constants.h"
#include "mep_parameters.h"
#include "mep_utils.h"
#include "mep_data.h"

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

	void fitness_regression_double_cache_all_training_data(t_mep_data *mep_dataset, double** cached_eval_matrix, double * cached_sum_of_errors, int num_actual_variables, int * actual_enabled_variables, double **);

	void fitness_binary_classification_double_cache_all_training_data(t_mep_data *mep_dataset, double **cached_eval_matrix, double * cached_sum_of_errors, double * cached_threashold, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double, s_value_class *tmp_value_class);

	double fit;        // the fitness

public:
	code3 *prg;        // a string of genes
	code3 *simplified_prg;      // simplified prg

	int best;          // the index of the best expression in chromosome
	int num_utilized;  // number of utilized genes
	double best_class_threshold;

public:

	t_mep_chromosome();
	~t_mep_chromosome();
	void clear(void);
	char * to_C_double(bool simplified, double *data, int problem_type);

	double get_fitness(void);
	void init_and_allocate_memory();

	t_mep_chromosome& operator=(const t_mep_chromosome &source);

	void allocate_memory(long code_length, int num_total_vars, bool use_constants, t_mep_constants *constants);
    
	void generate_random(t_mep_parameters *parameters, t_mep_constants * mep_constants, int *actual_operators, int num_actual_operators, int *actual_used_variables, int num_actual_used_variables);
    
	int compare(t_mep_chromosome *other, bool minimize_operations_count);

	void mutation(t_mep_parameters *parameters, t_mep_constants * mep_constants, int *actual_operators, int num_actual_operators, int *actual_used_variables, int num_actual_used_variables);
	void one_cut_point_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, t_mep_parameters *parameters, t_mep_constants * mep_constants);
	void uniform_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, t_mep_parameters *parameters, t_mep_constants * mep_constants);

	void fitness_regression_double(t_mep_data *mep_dataset, double* eval_vect, double *sum_of_errors_array);

	void fitness_regression(t_mep_data *mep_dataset, double** cached_eval_matrix, double * cached_sum_of_errors, int num_actual_variables, int * actual_enabled_variables, double **);
	void fitness_classification(t_mep_data *mep_dataset, double **cached_eval_matrix, double * cached_sum_of_errors, double * cached_threashold, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double, s_value_class *tmp_value_class);
	
	void fitness_multi_class_classification_double_cache_all_training_data(t_mep_data *mep_dataset, double **eval_matrix_double);

	bool compute_classification_error_on_double_data_return_error(double **inputs, int num_data, int output_col, double *error);
	bool compute_classification_error_on_double_data(double **data, int num_data, int output_col, double *error);

	bool compute_regression_error_on_double_data_return_error(double **inputs, int num_data, int output_col, double *error);
	bool compute_regression_error_on_double_data(double **data, int num_data, int output_col, double *error);

	bool evaluate_double(double *inputs, double *outputs);
	bool get_error_double(double *inputs, double *outputs);

	void simplify(void);

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent);
};
//-----------------------------------------------------------------
#endif