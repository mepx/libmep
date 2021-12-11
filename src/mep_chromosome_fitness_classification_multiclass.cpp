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
void t_mep_chromosome::fitness_multi_class_classification_winner_takes_all_fixed(
	const t_mep_data& mep_dataset,
	int* random_subset_indexes, int random_subset_selection_size,
	double** cached_variables_eval_matrix, int num_actual_variables,
	int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{
	fitness_multi_class_classification_winner_takes_all_fixed_double_cache_all_training_data(
		mep_dataset, random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables, eval_matrix_double, seed);
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_smooth(
	const t_mep_data& mep_dataset,
	int* random_subset_indexes, int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	int num_actual_variables, int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{
	fitness_multi_class_classification_smooth_double_cache_all_training_data(mep_dataset,
		random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix, num_actual_variables,
		actual_enabled_variables, eval_matrix_double, seed);
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_winner_takes_all_dynamic(
	const t_mep_data& mep_dataset,
	int* random_subset_indexes, int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	int num_actual_variables, int* actual_enabled_variables,
	double** eval_matrix_double,
	t_seed& seed)
{
	fitness_multi_class_classification_winner_takes_all_dynamic_double_cache_all_training_data(
		mep_dataset, random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables, eval_matrix_double, seed);
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multi_class_classification_winner_takes_all_fixed_error_on_double_data(
	double** data, int num_data, int output_col, int num_classes, double& error)
{
	error = 0;
	//	double actual_output_double[1];

	int max_index = -1;
	int index_error_gene;
	for (int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, index_error_gene)) {
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
bool t_mep_chromosome::compute_multi_class_classification_smooth_error_on_double_data(
	double** data, int num_data, int output_col, int num_classes, double& error)
{
	error = 0;

	int max_index = -1;
	int index_error_gene;
	for (int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, index_error_gene)) {
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
bool t_mep_chromosome::compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data(
	double** data, int num_data, int output_col, int /*num_classes*/, double& error)
{
	error = 0;

	int max_index = -1;
	int index_error_gene;
	for (int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, index_error_gene)) {
			if (max_index != index_best_genes[(int)data[k][output_col]])
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
	double** data, int num_data, int output_col, int num_classes,
	double& error, int& index_error_gene, double& _num_incorrectly_classified)
{
	error = 0;
	//double actual_output_double[1];

	int max_index = -1;
	for (int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, index_error_gene)) {
			if (fabs(max_index % num_classes - data[k][output_col]) > 1E-6)
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
bool t_mep_chromosome::compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data_return_error(
	double** data, int num_data, int output_col,
	int /*num_classes*/, double& error, int& index_error_gene, double& _num_incorrectly_classified)
{
	error = 0;
	//double actual_output_double[1];

	int max_index = -1;
	for (int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, index_error_gene)) {
			if (max_index != index_best_genes[(int)data[k][output_col]])
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
void t_mep_chromosome::fitness_multi_class_classification_winner_takes_all_dynamic_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	int* random_subset_indexes, int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	int num_actual_variables, int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double** data = mep_dataset.get_data_matrix_double();
	int num_rows = mep_dataset.get_num_rows();
	int num_classes = mep_dataset.get_num_classes();

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_variables_eval_matrix, num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double, seed);

	int* index_of_max = new int[num_rows];

	num_incorrectly_classified = 0;
	for (int t = 0; t < random_subset_selection_size; t++) {
		// find the maximal value
		int rs_index = random_subset_indexes[t];

		double max_val = -DBL_MAX;
		index_of_max[t] = -1;
		for (int i = 0; i < code_length; i++) {

			if (prg[i].op >= 0)// variable or constant
				if (prg[i].op < num_total_variables) {
					if (max_val < cached_variables_eval_matrix[prg[i].op][rs_index]) {
						max_val = cached_variables_eval_matrix[prg[i].op][rs_index];
						index_of_max[t] = i;
					}
				}
				else {
					if (max_val < eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index]) {
						max_val = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index];
						index_of_max[t] = i;
					}
				}
			else// an operator
				if (max_val < eval_matrix_double[i][rs_index]) {
					max_val = eval_matrix_double[i][rs_index];
					index_of_max[t] = i;
				}
		}
	}

	int** errors = new int* [code_length];
	for (int i = 0; i < code_length; i++) {
		errors[i] = new int[num_classes];
		for (int c = 0; c < num_classes; c++)
			errors[i][c] = 0;
	}

	for (int t = 0; t < random_subset_selection_size; t++) {
		int rs_index = random_subset_indexes[t];
		int target_class = (int)data[rs_index][num_total_variables];
		for (int i = 0; i < code_length; i++)
			if (index_of_max[t] != i)
				errors[i][target_class]++;
	}
	int* index_of_outputs = new int[num_classes];
	bool* used_genes = new bool[code_length];
	for (int i = 0; i < code_length; i++)
		used_genes[i] = false;

	bool* used_class = new bool[num_classes];
	for (int i = 0; i < num_classes; i++)
		used_class[i] = false;

	num_incorrectly_classified = 0;

	// assign genes to outputs
	/*
	// method 1
	for (int c = 0; c < num_classes; c++) {
		// find the gene with the minimal error
		int min_error = INT_MAX;
		int index_min = -1;
		for (int i = 0; i < code_length; i++)
			if (min_error > errors[i][c] && !used_genes[i]){
				// check if this gene has not been used before
				min_error = errors[i][c];
				index_min = i;
			}
		used_genes[index_min] = true;
		num_incorrectly_classified += min_error;
	}
	*/
	// method 2
	max_index_best_genes = -1;
	for (int c = 0; c < num_classes; c++) {
		// find the gene with the minimal error
		int min_error = INT_MAX;
		int index_gene_min = -1;
		int index_class_min = -1;
		for (int i = 0; i < code_length; i++)
			for (int c2 = 0; c2 < num_classes; c2++)
				if (min_error > errors[i][c2] && !used_genes[i] && !used_class[c2]) {
					// check if this gene has not been used before
					min_error = errors[i][c2];
					index_gene_min = i;
					index_class_min = c2;
				}
		used_genes[index_gene_min] = true;
		used_class[index_class_min] = true;
		index_best_genes[index_class_min] = index_gene_min;
		if (max_index_best_genes < index_gene_min)
			max_index_best_genes = index_gene_min;
		num_incorrectly_classified += min_error;
	}

	fitness = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	num_incorrectly_classified = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	if (line_of_constants)
		delete[] line_of_constants;

	for (int i = 0; i < code_length; i++)
		delete[] errors[i];
	delete[] errors;

	delete[] index_of_max;
	delete[] index_of_outputs;
	delete[] used_genes;
	delete[] used_class;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_winner_takes_all_fixed_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	int* random_subset_indexes, int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	int num_actual_variables, int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double** data = mep_dataset.get_data_matrix_double();
	int num_rows = mep_dataset.get_num_rows();
	int num_classes = mep_dataset.get_num_classes();

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables,
		line_of_constants, eval_matrix_double, seed);

	num_incorrectly_classified = 0;
	for (int t = 0; t < random_subset_selection_size; t++) {
		// find the maximal value
		int rs_index = random_subset_indexes[t];

		double max_val = -DBL_MAX;
		int max_index = -1;
		for (int i = 0; i < code_length; i++) {
			int op = prg[i].op;
			if (op >= 0) {// variable or constant
				if (op < num_total_variables) {
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
		if (max_index % num_classes != (int)data[rs_index][num_total_variables])
			num_incorrectly_classified++;
	}

	fitness = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	num_incorrectly_classified = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	if (line_of_constants)
		delete[] line_of_constants;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_smooth_double_cache_all_training_data(
	const t_mep_data& mep_dataset, int* random_subset_indexes, int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	int num_actual_variables, int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{
	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double** data = mep_dataset.get_data_matrix_double();
	int num_rows = mep_dataset.get_num_rows();
	int num_classes = mep_dataset.get_num_classes();

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_variables_eval_matrix, num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double, seed);

	double* max_value_per_class = new double[num_classes];
	int* max_index_of_class = new int[num_classes];

	fitness = 0;

	num_incorrectly_classified = 0;
	for (int t = 0; t < random_subset_selection_size; t++) {
		// find the maximal value
		int rs_index = random_subset_indexes[t];
		//		double max_val = eval_matrix_double[0][rs_index];

		for (int i = 0; i < num_classes; i++)
			if (prg[i].op >= 0) // variable or constant
				if (prg[i].op < num_total_variables) {
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

		for (int i = num_classes; i < code_length; i++)
			if (prg[i].op >= 0) // variable or constant
				if (prg[i].op < num_total_variables) {
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

		// now I have to scale them between 0 and 1
		double _max = max_value_per_class[0];
		double _min = max_value_per_class[0];
		int max_index = 0;

		for (int i = 1; i < num_classes; i++)
			if (_max < max_value_per_class[i]) {
				_max = max_value_per_class[i];
				max_index = max_index_of_class[i];
			}
			else
				if (_min > max_value_per_class[i])
					_min = max_value_per_class[i];

		if (fabs(_min - _max) < 1E-6) // constant
			//fitness += num_classes - 1;
			;
		else {
			// make them between 0 and 1
			for (int i = 0; i < num_classes; i++) {
				max_value_per_class[i] -= _min;
				max_value_per_class[i] /= (_max - _min);
			}
		}

		int target = (int)data[rs_index][num_total_variables];

		for (int i = 0; i < target; i++)
			if (max_value_per_class[i] >= max_value_per_class[target])
				fitness += 1 + max_value_per_class[i] - max_value_per_class[target];
		// add 1 for each other class which has a bigger maximal value than the expected one; also add distance to that max.

		for (int i = target + 1; i < num_classes; i++)
			if (max_value_per_class[i] >= max_value_per_class[target])
				fitness += 1 + max_value_per_class[i] - max_value_per_class[target];



		if (fabs(_max - _min) < 1E-6) {
			// all are equal
			fitness += num_classes - 1;
		}
		else {// some are sifferent
			fitness += (1 - max_value_per_class[target]) * (num_classes - 1);

			for (int i = 0; i < target; i++)
				fitness += max_value_per_class[i];

			for (int i = target + 1; i < num_classes; i++)
				fitness += max_value_per_class[i];
		}
		if (max_index % num_classes != (int)data[rs_index][num_total_variables])
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
