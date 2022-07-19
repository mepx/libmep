// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <math.h>
#include <float.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
#include "mep_functions.h"
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_binary_classification(const t_mep_data& mep_dataset, 
	unsigned int* random_subset_indexes,
	unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix, double* cached_sum_of_errors, double* cached_threashold,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double, s_value_class* tmp_value_class,
	t_seed& seed)
{
	fitness_binary_classification_double_cache_all_training_data(mep_dataset,
		random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix, cached_sum_of_errors, cached_threashold,
		num_actual_variables, actual_enabled_variables,
		eval_matrix_double, 
		tmp_value_class, seed);
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_binary_classification_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix, double* cached_sum_of_errors,
	double* cached_threashold,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double, s_value_class* tmp_value_class,
	t_seed& seed)
{

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	//double** data = mep_dataset.get_data_matrix_double();
	unsigned int *class_index = mep_dataset.get_class_label_index_as_array();
	unsigned int num_rows = mep_dataset.get_num_rows();

	fitness = DBL_MAX;
	//index_best_genes[0] = -1; //Do I need this?????
	num_incorrectly_classified = 100;// max

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (unsigned int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double, seed);

	double best_threshold;
	for (unsigned int i = 0; i < code_length; i++) {   // read the t_mep_chromosome from top to down
		double sum_of_errors;
		if (prg[i].op >= 0)// a variable
			if (prg[i].op < (int)num_total_variables) { // a variable, which is cached already
				sum_of_errors = cached_sum_of_errors[prg[i].op];
				best_threshold = cached_threashold[prg[i].op];
			}
			else {// a constant
				if (mep_dataset.get_num_items_class_0() < num_rows - mep_dataset.get_num_items_class_0()) {// i must classify everything as 1
					sum_of_errors = mep_dataset.get_num_items_class_0();
					best_threshold = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][0] - 1;
				}
				else {// less of 1, I must classify everything as class 0
					sum_of_errors = num_rows - mep_dataset.get_num_items_class_0();
					best_threshold = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][0];
				}
				sum_of_errors /= (double)num_rows;
				sum_of_errors *= 100;
			}
		else {// an operator
			double* eval = eval_matrix_double[i];

			int num_0_incorrect = 0;
			for (unsigned int k = 0; k < random_subset_selection_size; k++) {
				tmp_value_class[k].value = eval[random_subset_indexes[k]];
				tmp_value_class[k].data_class = class_index[random_subset_indexes[k]];
				if (tmp_value_class[k].data_class < 0.5)
					num_0_incorrect++;
			}
			qsort((void*)tmp_value_class, random_subset_selection_size, sizeof(s_value_class), sort_function_value_class);

			//			int num_0_incorrect = mep_dataset->get_num_items_class_0();
			int num_1_incorrect = 0;
			best_threshold = tmp_value_class[0].value - 1;// all are classified to class 1 in this case
			sum_of_errors = num_0_incorrect;

			for (unsigned int t = 0; t < random_subset_selection_size; t++) {
				unsigned int j = t + 1;
				while (j < random_subset_selection_size && 
					fabs(tmp_value_class[t].value - tmp_value_class[j].value) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
					j++;

				// check all between i and j and label them with class 0
				for (unsigned int k = t; k < j; k++)
					if (tmp_value_class[k].data_class == 0)
						num_0_incorrect--;
					else {
						if (tmp_value_class[k].data_class == 1) {
							//num_0_incorrect--;
							num_1_incorrect++;
						}
					}
				if (num_0_incorrect + num_1_incorrect < sum_of_errors) {
					sum_of_errors = num_0_incorrect + num_1_incorrect;
					best_threshold = tmp_value_class[t].value;
				}
				t = j;
				t--;
			}
			sum_of_errors /= (double)random_subset_selection_size;
			sum_of_errors *= 100;
		}

		if (fitness > sum_of_errors) {
			fitness = sum_of_errors;
			index_best_genes[0] = i;
			best_class_threshold = best_threshold;
			num_incorrectly_classified = fitness;
		}
	}

	max_index_best_genes = index_best_genes[0];
	if (line_of_constants)
		delete[] line_of_constants;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_binary_classification_error_on_double_data(
    const t_mep_data &mep_data, double& error)
{
	error = 0;
	double actual_output_double[1];

    unsigned int num_data = mep_data.get_num_rows();
    double **data = mep_data.get_data_matrix_double();
	unsigned int* class_labels_index = mep_data.get_class_label_index_as_array();

	unsigned int index_error_gene;
	for (unsigned int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			if (actual_output_double[0] <= best_class_threshold)
				error += (class_labels_index[k] > 0); // I do this because, maybe I have more than 2 classes in validation or test
			else
				error += (class_labels_index[k] != 1);
		}
		else
			error++;
	}
	error /= num_data;
	error *= 100;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_binary_classification_error_on_double_data_return_error(
	const t_mep_data &mep_data,
    double& error, unsigned int& index_error_gene)
{
	error = 0;
	double actual_output_double[1];

	unsigned int num_data = mep_data.get_num_rows();
	double **data = mep_data.get_data_matrix_double();
	unsigned int* class_labels_index = mep_data.get_class_label_index_as_array();

	for (unsigned int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			if (actual_output_double[0] <= best_class_threshold)
				error += (class_labels_index[k] > 0); // class 0; // I do this because, maybe I have more than 2 classes in validation or test
			else
				error += (class_labels_index[k] != 1);
		}
		else
			return false;
	}
	error /= num_data;
	error *= 100;

	return true;
}
//---------------------------------------------------------------------------
