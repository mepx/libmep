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
void t_mep_chromosome::fitness_multi_class_classification_winner_takes_all_dynamic(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double,
	t_seed& seed)
{
	fitness_multi_class_classification_winner_takes_all_dynamic_double_cache_all_training_data(
		mep_dataset, random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables, eval_matrix_double, seed);
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data(
	double** data, unsigned int num_data, unsigned int output_col, double& error)
{
	error = 0;

	unsigned int max_index;
	double max_value;
	unsigned int index_error_gene;
	unsigned int critical_errors = 0;
	double* values_for_output_genes = new double[num_classes];
	for (unsigned int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, max_value, index_error_gene, values_for_output_genes)) {
			unsigned int target_class = (unsigned int)data[k][output_col];
			unsigned int predicted_class = num_classes;
			for (unsigned int c = 0; c < num_classes; c++) {
				if (max_index == index_best_genes[c]) {
					predicted_class = c;
				}
			}

			if (predicted_class == num_classes) {
				double min_dist = fabs(max_value - values_for_output_genes[0]);
				unsigned int class_index = 0;
				for (unsigned int c = 1; c < num_classes; c++) {
					if (min_dist > fabs(max_value - values_for_output_genes[c])) {
						min_dist = fabs(max_value - values_for_output_genes[c]);
						class_index = c;
					}
				}
				if (class_index != target_class)
					error++;
			}
			else {
				if (predicted_class != target_class)
					error++;
			}

		}
		else {
			error++;
			critical_errors++;
		}
	}
	error /= num_data;
	error *= 100;

	delete[] values_for_output_genes;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data_return_error(
	double** data, unsigned int num_data, unsigned int output_col,
	double& error, unsigned int& index_error_gene, double& _num_incorrectly_classified)
{
	error = 0;
	//double actual_output_double[1];

	unsigned int max_index;
	double max_value;
	double* values_for_output_genes = new double[num_classes];

	for (unsigned int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, max_value, index_error_gene, values_for_output_genes)) {
			unsigned int target_class = (unsigned int)data[k][output_col];
			unsigned int predicted_class = num_classes;
			for (unsigned int c = 0; c < num_classes; c++) {
				if (max_index == index_best_genes[c]) {
					predicted_class = c;
				}
			}

			if (predicted_class == num_classes) {
				
				double min_dist = fabs(max_value - values_for_output_genes[0]);
				unsigned int class_index = 0;
				for (unsigned int c = 1; c < num_classes; c++) {
					if (min_dist > fabs(max_value - values_for_output_genes[c])) {
						min_dist = fabs(max_value - values_for_output_genes[c]);
						class_index = c;
					}
				}
				if (class_index != target_class)
					error++;
			}
			else {
				if (predicted_class != target_class) 
					error++;
			}
		}
		else {
			delete[] values_for_output_genes;
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
void t_mep_chromosome::fitness_multi_class_classification_winner_takes_all_dynamic_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double, t_seed& seed)
{

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double** data = mep_dataset.get_data_matrix_double();
	//unsigned int num_rows = mep_dataset.get_num_rows();

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (unsigned int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(random_subset_selection_size, cached_variables_eval_matrix, 
		num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double, seed);

	unsigned int* index_of_max = new unsigned int[random_subset_selection_size];
	double* max_value_for_each_data = new double[random_subset_selection_size];

	for (unsigned int t = 0; t < random_subset_selection_size; t++) {
		// find the maximal value
		unsigned int rs_index = random_subset_indexes[t];

		max_value_for_each_data[t] = -DBL_MAX;
		index_of_max[t] = 0;
		for (unsigned int i = 0; i < code_length; i++) {

			if (prg[i].op >= 0) {// variable or constant
				if (prg[i].op < (int)num_total_variables) {
					if (max_value_for_each_data[t] < cached_variables_eval_matrix[prg[i].op][rs_index]) {
						max_value_for_each_data[t] = cached_variables_eval_matrix[prg[i].op][rs_index];
						index_of_max[t] = i;

					}
				}
				else {// a constant
					if (max_value_for_each_data[t] < eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index]) {
						max_value_for_each_data[t] = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index];
						index_of_max[t] = i;
					}
				}
			}
			else// an operator
				if (max_value_for_each_data[t] < eval_matrix_double[i][rs_index]) {
					max_value_for_each_data[t] = eval_matrix_double[i][rs_index];
					index_of_max[t] = i;
				}
		}
	}

	int** errors = new int* [code_length];
	for (unsigned int i = 0; i < code_length; i++) {
		errors[i] = new int[num_classes];
		for (unsigned int c = 0; c < num_classes; c++)
			errors[i][c] = 0;
	}

	for (unsigned int t = 0; t < random_subset_selection_size; t++) {
		unsigned int rs_index = random_subset_indexes[t];
		unsigned int target_class = (unsigned int)data[rs_index][num_total_variables];
		for (unsigned int i = 0; i < code_length; i++)
			if (index_of_max[t] != i)
				errors[i][target_class]++;
	}
	//int* index_of_outputs = new int[num_classes];
	bool* used_genes = new bool[code_length];
	for (unsigned int i = 0; i < code_length; i++)
		used_genes[i] = false;

	bool* used_class = new bool[num_classes];
	for (unsigned int i = 0; i < num_classes; i++)
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
	max_index_best_genes = 0;
	for (unsigned int c = 0; c < num_classes; c++) {
		// find the gene with the minimal error
		int min_error = INT_MAX;
		unsigned int index_gene_min = 0;
		unsigned int index_class_min = 0;
		for (unsigned int i = 0; i < code_length; i++)
			for (unsigned int c2 = 0; c2 < num_classes; c2++)
				if (min_error > errors[i][c2] && !used_genes[i] && !used_class[c2]) {
					// check if this gene has not been used before
					min_error = errors[i][c2];
					index_gene_min = i;
					index_class_min = c2;
				}
		used_genes[index_gene_min] = true;
		used_class[index_class_min] = true;
		index_best_genes[index_class_min] = index_gene_min;
		if (max_index_best_genes < index_gene_min) // it makes no sense to evaluate all code since I can stop earlier
			max_index_best_genes = index_gene_min;
		num_incorrectly_classified += min_error;
	}

	num_incorrectly_classified = 0;
	double* values_for_each_best_gene = new double[num_classes];
	for (unsigned int t = 0; t < random_subset_selection_size; t++) {
		// find the maximal value
		unsigned int rs_index = random_subset_indexes[t];
		unsigned int target_class = (unsigned int)data[rs_index][num_total_variables];
		unsigned int predicted_class = num_classes;
		for (unsigned int c = 0; c < num_classes; c++) {
			if (index_of_max[t] == index_best_genes[c]) {
				predicted_class = c;
			}
		}
		if (predicted_class == num_classes) {// no class found
			// this is not correctly classified
			// so I try to classify it to the nearest maximum
			// fill in the values for each best index gene
			for (unsigned int c = 0; c < num_classes; c++) {
				unsigned int i = index_best_genes[c];
				if (prg[i].op >= 0) {// variable or constant
					if (prg[i].op < (int)num_total_variables)
						values_for_each_best_gene[c] = cached_variables_eval_matrix[prg[i].op][rs_index];
					else // a constant
						values_for_each_best_gene[c] = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][rs_index];
				}
				else// an operator
					values_for_each_best_gene[c] = eval_matrix_double[i][rs_index];
			}

			double min_dist = fabs(max_value_for_each_data[t] - values_for_each_best_gene[0]);
			unsigned int class_index = 0;
			for (unsigned int c = 1; c < num_classes; c++) {
				if (min_dist > fabs(max_value_for_each_data[t] - values_for_each_best_gene[c])) {
					min_dist = fabs(max_value_for_each_data[t] - values_for_each_best_gene[c]);
					class_index = c;
				}
			}
			if (class_index != target_class)
				num_incorrectly_classified++;
		}
		else {
			if (predicted_class != target_class)
				num_incorrectly_classified++;
		}
	}
	delete[] values_for_each_best_gene;

	fitness = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	num_incorrectly_classified = num_incorrectly_classified / (double)random_subset_selection_size * 100;

	if (line_of_constants)
		delete[] line_of_constants;

	for (unsigned int i = 0; i < code_length; i++)
		delete[] errors[i];
	delete[] errors;

	delete[] index_of_max;
	delete[] max_value_for_each_data;
	//delete[] index_of_outputs;
	delete[] used_genes;
	delete[] used_class;
}
//---------------------------------------------------------------------------