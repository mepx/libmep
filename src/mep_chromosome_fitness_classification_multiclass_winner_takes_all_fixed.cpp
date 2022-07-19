// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <limits.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_winner_takes_all_fixed(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix, unsigned int num_actual_variables,
	unsigned int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{
	fitness_multi_class_classification_winner_takes_all_fixed_double_cache_all_training_data(
		mep_dataset, random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables, eval_matrix_double, seed);
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multi_class_classification_winner_takes_all_fixed_error_on_double_data(
	const t_mep_data& mep_data, double& error)
{
	error = 0;
	//	double actual_output_double[1];

	unsigned int max_index;
	unsigned int index_error_gene;
	double max_value;

	unsigned int num_data = mep_data.get_num_rows();
	double** data = mep_data.get_data_matrix_double();
	unsigned int* class_labels_index = mep_data.get_class_label_index_as_array();

	for (unsigned int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, max_value, index_error_gene, NULL)) {
			if (fabs(max_index % num_classes - class_labels_index[k]) > 1E-6)
				error++;
		}
		else
			error++;
	}
	error /= num_data;
	error *= 100;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multi_class_classification_error_on_double_data_return_error(
const t_mep_data& mep_data,
	double& error, unsigned int& index_error_gene, double& _num_incorrectly_classified)
{
	// this function is the same for winner takes all fixed and smooth
	error = 0;
	//double actual_output_double[1];

	unsigned int max_index;
	double max_value;

	unsigned int num_data = mep_data.get_num_rows();
	double** data = mep_data.get_data_matrix_double();
	unsigned int* class_labels_index = mep_data.get_class_label_index_as_array();

	for (unsigned int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, max_value, index_error_gene, NULL)) {
			if (fabs(max_index % num_classes - class_labels_index[k]) > 1E-6)
				error++;
		}
		else {
			return false;
		}
	}

	_num_incorrectly_classified = error;

	error /= num_data;
	error *= 100;

	_num_incorrectly_classified = _num_incorrectly_classified / (double)num_data * 100;

	return true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_winner_takes_all_fixed_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{
	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	//double** data = mep_dataset.get_data_matrix_double();
	unsigned int num_rows = mep_dataset.get_num_rows();
	unsigned int* class_labels_index = mep_dataset.get_class_label_index_as_array();

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (unsigned int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables,
		line_of_constants, eval_matrix_double, seed);

	num_incorrectly_classified = 0;
	for (unsigned int t = 0; t < random_subset_selection_size; t++) {
		// find the maximal value
		unsigned int rs_index = random_subset_indexes[t];

		double max_val = -DBL_MAX;
		unsigned int max_index = 0; // this cannot be less than 0
		for (unsigned int i = 0; i < code_length; i++) {
			int op = prg[i].op;
			if (op >= 0) {// variable or constant
				if (op < (int)num_total_variables) {
					if (max_val < cached_variables_eval_matrix[op][rs_index]) {
						max_val = cached_variables_eval_matrix[op][rs_index];
						max_index = i;
					}
				}
				else {
					if (max_val < eval_matrix_double[line_of_constants[op - num_total_variables]][rs_index]) {
						max_val = eval_matrix_double[line_of_constants[op - num_total_variables]][rs_index];
						max_index = i;
					}
				}
			}
			else// an operator
				if (max_val < eval_matrix_double[i][rs_index]) {
					max_val = eval_matrix_double[i][rs_index];
					max_index = i;
				}
		}
		if (max_index % num_classes != class_labels_index[rs_index])
			num_incorrectly_classified++;
	}

	fitness = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	num_incorrectly_classified = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	if (line_of_constants)
		delete[] line_of_constants;
}
//---------------------------------------------------------------------------
