// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_smooth(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{
	fitness_multi_class_classification_smooth_double_cache_all_training_data(mep_dataset,
		random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix, num_actual_variables,
		actual_enabled_variables, eval_matrix_double, seed);
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multi_class_classification_smooth_error_on_double_data(
	double** data, unsigned int num_data, unsigned int output_col, double& error)
{
	error = 0;

	unsigned int max_index;
	unsigned int index_error_gene;
	double max_value;
	for (unsigned int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, max_value, index_error_gene, NULL)) {
			if (fabs(max_index % num_classes - data[k][output_col]) > 1E-6)
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
void t_mep_chromosome::fitness_multi_class_classification_smooth_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{
	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double** data = mep_dataset.get_data_matrix_double();
	unsigned int num_rows = mep_dataset.get_num_rows();

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (unsigned int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_variables_eval_matrix, num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double, seed);

	double* max_value_per_class = new double[num_classes];
	unsigned int* max_index_of_class = new unsigned int[num_classes];

	fitness = 0;

	num_incorrectly_classified = 0;
	for (unsigned int t = 0; t < random_subset_selection_size; t++) {
		// find the maximal value
		unsigned int rs_index = random_subset_indexes[t];
		//		double max_val = eval_matrix_double[0][rs_index];

		for (unsigned int i = 0; i < num_classes; i++) {
			if (prg[i].op >= 0) // variable or constant
				if (prg[i].op < (int)num_total_variables) {
					max_value_per_class[i] = cached_variables_eval_matrix[prg[i].op][rs_index];
					max_index_of_class[i] = i;
				}
				else { // constant
					max_value_per_class[i] = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index];
					max_index_of_class[i] = i;
				}
			else {// an operator
				max_value_per_class[i] = eval_matrix_double[i][rs_index];
				max_index_of_class[i] = i;
			}
			/*
			if (errno || isnan(max_value_per_class[i]) || isinf(max_value_per_class[i])) {
				printf("bug!");
			}
			*/
		}
		for (unsigned int i = num_classes; i < code_length; i++) {
			if (prg[i].op >= 0) // variable or constant
				if (prg[i].op < (int)num_total_variables) {
					if (max_value_per_class[i % num_classes] < cached_variables_eval_matrix[prg[i].op][rs_index]) {
						max_value_per_class[i % num_classes] = cached_variables_eval_matrix[prg[i].op][rs_index];
						max_index_of_class[i % num_classes] = i;
					}
				}
				else {
					if (max_value_per_class[i % num_classes] < eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index]) {
						max_value_per_class[i % num_classes] = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index];
						max_index_of_class[i % num_classes] = i;
					}
				}
			else// an operator
				if (max_value_per_class[i % num_classes] < eval_matrix_double[i][rs_index]) {
					max_value_per_class[i % num_classes] = eval_matrix_double[i][rs_index];
					max_index_of_class[i % num_classes] = i;
				}
			/*
			if (errno || isnan(max_value_per_class[i]) || isinf(max_value_per_class[i])) {
				printf("bug!");
			}
			*/
		}

		// now I have to scale them between 0 and 1
		double _max = max_value_per_class[0];
		double _min = max_value_per_class[0];
		/*
		if (errno || isnan(_max) || isinf(_max)) {
			printf("bug!");
		}
		*/

		unsigned int max_index = 0;

		for (unsigned int c = 1; c < num_classes; c++) {
			if (_max < max_value_per_class[c]) {
				_max = max_value_per_class[c];
				max_index = max_index_of_class[c];
			}
			else
				if (_min > max_value_per_class[c])
					_min = max_value_per_class[c];
			/*
			if (errno || isnan(_max) || isinf(_max)) {
				printf("bug!");
			}
			*/
		}
		bool is_error_case = false;
		if (fabs(_min - _max) < 1E-6) { // a constant
			//fitness += num_classes - 1;
			;
			is_error_case = true;
		}
		else {
			// make them between 0 and 1
			errno = 0;
			for (unsigned int c = 0; c < num_classes; c++) {
				max_value_per_class[c] -= _min;
				max_value_per_class[c] /= (_max - _min);
				if (errno || isnan(max_value_per_class[c]) || isinf(max_value_per_class[c])) {
					//printf("bug!");
					// this must be handled
					is_error_case = true;
					break;
				}
			}
			errno = 0;
		}

		unsigned int target = (unsigned int)data[rs_index][num_total_variables];

		for (unsigned int i = 0; i < target; i++)
			if (max_value_per_class[i] >= max_value_per_class[target])
				fitness += 1 + max_value_per_class[i] - max_value_per_class[target];
		// add 1 for each other class which has a bigger maximal value than the expected one; also add distance to that max.

		for (unsigned int i = target + 1; i < num_classes; i++)
			if (max_value_per_class[i] >= max_value_per_class[target])
				fitness += 1 + max_value_per_class[i] - max_value_per_class[target];

		if (is_error_case) {
			// all are equal
			fitness += num_classes - 1;
		}
		else {// some are sifferent
			fitness += (1 - max_value_per_class[target]) * (num_classes - 1);

			for (unsigned int i = 0; i < target; i++)
				fitness += max_value_per_class[i];

			for (unsigned int i = target + 1; i < num_classes; i++)
				fitness += max_value_per_class[i];
		}
		if (max_index % num_classes != (unsigned int)data[rs_index][num_total_variables])
			num_incorrectly_classified++;
	}

	//fitness += num_incorrectly_classified;
	fitness /= (double)random_subset_selection_size;

	num_incorrectly_classified = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	if (line_of_constants)
		delete[] line_of_constants;

	delete[] max_value_per_class;
	delete[] max_index_of_class;
}
//---------------------------------------------------------------------------
