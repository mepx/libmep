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
#include "mep_functions.h"
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_closest_center(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double,
	t_seed& seed)
{
	fitness_multi_class_classification_closest_center_double_cache_all_training_data(
		mep_dataset, random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables, eval_matrix_double,
		seed);
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multi_class_classification_closest_center_error_on_double_data(
	const t_mep_data& mep_dataset, double& error)
{
	error = 0;

	unsigned int index_error_gene;
	double out[1];
	
	double** data = mep_dataset.get_data_matrix_double();
	unsigned int *class_index = mep_dataset.get_class_label_index_as_array();
	unsigned int num_data = mep_dataset.get_num_rows();
	
	for (unsigned int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], out, index_error_gene)){
			double min_dist = DBL_MAX;
			unsigned int closest_class_index = 0;
			for (unsigned int c = 0; c < num_classes; c++) {
				if (min_dist > fabs(out[0] - centers[c])) {
					min_dist = fabs(out[0] - centers[c]);
					closest_class_index = c;
				}
			}
			if ((int)closest_class_index != class_index[k])
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
bool t_mep_chromosome::compute_multi_class_classification_closest_center_error_on_double_data_return_error(
const t_mep_data& mep_dataset,
	double& error, unsigned int& index_error_gene, double& _num_incorrectly_classified)
{
	error = 0;

//	unsigned int index_error_gene;
	double out[1];

	double** data = mep_dataset.get_data_matrix_double();
	unsigned int* class_index = mep_dataset.get_class_label_index_as_array();
	unsigned int num_data = mep_dataset.get_num_rows();

	for (unsigned int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], out, index_error_gene)) {
			double min_dist = DBL_MAX;
			unsigned int closest_class_index = 0;
			for (unsigned int c = 0; c < num_classes; c++) {
				if (min_dist > fabs(out[0] - centers[c])) {
					min_dist = fabs(out[0] - centers[c]);
					closest_class_index = c;
				}
			}
			if ((int)closest_class_index != class_index[k])
				error++;
		}
		else
			return false;
	}

	_num_incorrectly_classified = error;

	error /= num_data;
	error *= 100;

	_num_incorrectly_classified = _num_incorrectly_classified / (double)num_data * 100;

	return true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_closest_center_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double, 
	t_seed& seed)
{

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double** data = mep_dataset.get_data_matrix_double();
	//unsigned int num_rows = mep_dataset.get_num_rows();
	unsigned int* class_labels_index = mep_dataset.get_class_label_index_as_array();

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (unsigned int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(random_subset_selection_size,
							   cached_variables_eval_matrix,
							   num_actual_variables, actual_enabled_variables,
							   line_of_constants, eval_matrix_double, seed);

	double* local_centers = new double[num_classes];
	num_incorrectly_classified = UINT32_MAX;

	for (unsigned int i = 0; i < code_length; i++) {
		for (unsigned int c = 0; c < num_classes; c++) {
			local_centers[c] = 0;
			unsigned int count_class_c = 0;

			for (unsigned int t = 0; t < random_subset_selection_size; t++) {
				// find the maximal value
				unsigned int rs_index = random_subset_indexes[t];

				if (prg[i].op >= 0) {// variable or constant
					if (prg[i].op < (int)num_total_variables) {
						if (class_labels_index[rs_index] == c) { // class c
							local_centers[c] += cached_variables_eval_matrix[prg[i].op][rs_index];
							count_class_c++;
						}
					}
					else {
						if (fabs((unsigned int)data[rs_index][num_total_variables] - c) < 1e-6) { // class c
							local_centers[c] += eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index];
							count_class_c++;
						}
					}
				}
				else {// an operator
					if (fabs(class_labels_index[rs_index] - c) < 1e-6) { // class c
						local_centers[c] += eval_matrix_double[i][rs_index];
						count_class_c++;
					}
				}// end an operator
			}// end for t
			local_centers[c] /= count_class_c;
		}// end for c

		unsigned int tmp_num_incorrectly_classified = 0;

		for (unsigned int t = 0; t < random_subset_selection_size; t++) {
			unsigned int rs_index = random_subset_indexes[t];
			//unsigned int target_class = (unsigned int)data[rs_index][num_total_variables];
			double min_dist = DBL_MAX;
			unsigned int closest_class_index = 0;
			for (unsigned int c = 0; c < num_classes; c++) {
				if (prg[i].op >= 0) {// variable or constant
					if (prg[i].op < (int)num_total_variables) {
						if (min_dist > fabs(cached_variables_eval_matrix[prg[i].op][rs_index] - local_centers[c])) {
							min_dist = fabs(cached_variables_eval_matrix[prg[i].op][rs_index] - local_centers[c]);
							closest_class_index = c;
						}
					}
					else {
						if (min_dist > fabs(eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index] - local_centers[c])) {
							min_dist = fabs(eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index] - local_centers[c]);
							closest_class_index = c;
						}
					}
				}
				else {// an operator
					if (min_dist > fabs(eval_matrix_double[i][rs_index] - local_centers[c])) {
						min_dist = fabs(eval_matrix_double[i][rs_index] - local_centers[c]);
						closest_class_index = c;
					}
				}// end an operator
			} // end for c

			if (class_labels_index[rs_index] != closest_class_index)
				tmp_num_incorrectly_classified++;
		}

		if (num_incorrectly_classified > tmp_num_incorrectly_classified) {
			num_incorrectly_classified = tmp_num_incorrectly_classified;
			index_best_genes[0] = i;
			// must copy the class centers too
			for (unsigned int c = 0; c < num_classes; c++)
				centers[c] = local_centers[c];
		}

	} // end for i
	num_incorrectly_classified = num_incorrectly_classified / (double)random_subset_selection_size * 100;
	fitness = num_incorrectly_classified;

	if (line_of_constants)
		delete[] line_of_constants;

	delete[] local_centers;
	max_index_best_genes = index_best_genes[0];
}
//---------------------------------------------------------------------------
