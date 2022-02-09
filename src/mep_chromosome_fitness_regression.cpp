// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <math.h>
#include <errno.h>
#include <float.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
#include "mep_functions.h"
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_regression(const t_mep_data& mep_dataset, 
	unsigned int* random_subset_indexes,
	unsigned int random_subset_selection_size, double** cached_variables_eval_matrix,
	double* cached_sum_of_errors, unsigned int num_actual_variables,
	unsigned int* actual_enabled_variables, double** eval_matrix,
	t_mep_error_function mep_error_function, t_seed& seed)
{
	fitness_regression_double_cache_all_training_data(mep_dataset,
		random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix, cached_sum_of_errors,
		num_actual_variables, actual_enabled_variables,
		eval_matrix, mep_error_function, seed);
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_regression_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix, double* cached_sum_of_errors,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_matrix_double, t_mep_error_function mep_error_function,
	t_seed& seed)
{
	double** data = mep_dataset.get_data_matrix_double();
	unsigned int num_rows = mep_dataset.get_num_rows();

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	fitness = DBL_MAX;
	//index_best_genes[0] = -1;

	int* line_of_constants = NULL;
	double* cached_sum_of_errors_for_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		cached_sum_of_errors_for_constants = new double[num_constants];
		for (unsigned int i = 0; i < num_constants; i++) {
			line_of_constants[i] = -1;
			cached_sum_of_errors_for_constants[i] = -1;
		}
	}

	compute_eval_matrix_double(num_rows,
		cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables,
		line_of_constants, eval_matrix_double, seed);

	//	int num_training_data = mep_dataset->get_num_rows();

	for (unsigned int i = 0; i < code_length; i++) {   // read the t_mep_chromosome from top to down
		double sum_of_errors;

		if (prg[i].op >= 0) {// variable or constant
			if (prg[i].op < (int)num_total_variables) // a variable, which is cached already
				sum_of_errors = cached_sum_of_errors[prg[i].op];
			else {// a constant
				sum_of_errors = 0;
				unsigned int constant_index = (unsigned int)prg[i].op - num_total_variables;
				if (cached_sum_of_errors_for_constants[constant_index] < -0.5) {// this is not cached?
					double* eval = eval_matrix_double[line_of_constants[constant_index]];
					for (unsigned int k = 0; k < random_subset_selection_size; k++)
						sum_of_errors += mep_error_function(eval[random_subset_indexes[k]], data[random_subset_indexes[k]][num_total_variables]);
					sum_of_errors /= double(random_subset_selection_size);
				}
				else
					sum_of_errors = cached_sum_of_errors_for_constants[constant_index];
			}
		}
		else {// operator
			double* eval = eval_matrix_double[i];
			sum_of_errors = 0;
			for (unsigned int k = 0; k < random_subset_selection_size; k++)
				sum_of_errors += mep_error_function(eval[random_subset_indexes[k]], data[random_subset_indexes[k]][num_total_variables]);
			sum_of_errors /= double(random_subset_selection_size);
		}
		if (fitness > sum_of_errors) {
			fitness = sum_of_errors;
			index_best_genes[0] = i;
		}
	}

	if (line_of_constants)
		delete[] line_of_constants;

	if (cached_sum_of_errors_for_constants)
		delete[] cached_sum_of_errors_for_constants;

	max_index_best_genes = index_best_genes[0];
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_regression_double_no_cache(const t_mep_data& mep_dataset,
	unsigned int* /*random_subset_indexes*/, unsigned int /*random_subset_selection_size*/,
	double* eval_vect, double* sum_of_errors_array, t_mep_error_function mep_error_function)
{
	double** data = mep_dataset.get_data_matrix_double();

	fitness = DBL_MAX;
	//index_best_genes[0] = -1;

	for (unsigned int i = 0; i < code_length; i++)
		sum_of_errors_array[i] = 0;

	for (unsigned int k = 0; k < mep_dataset.get_num_rows(); k++) {   // read the t_mep_chromosome from top to down
		for (unsigned int i = 0; i < code_length; i++) {    // read the t_mep_chromosome from top to down

			errno = 0;
			bool is_error_case = false;
			switch (prg[i].op) {
			case  O_ADDITION:  // +
				eval_vect[i] = eval_vect[prg[i].addr1] + eval_vect[prg[i].addr2];
				break;
			case  O_SUBTRACTION:  // -
				eval_vect[i] = eval_vect[prg[i].addr1] - eval_vect[prg[i].addr2];
				break;
			case  O_MULTIPLICATION:  // *
				eval_vect[i] = eval_vect[prg[i].addr1] * eval_vect[prg[i].addr2];
				break;
			case  O_DIVISION:  //  /
				if (fabs(eval_vect[prg[i].addr2]) < MEP_DIVISION_PROTECT)
					is_error_case = true;
				else
					eval_vect[i] = eval_vect[prg[i].addr1] / eval_vect[prg[i].addr2];
				break;
			case O_POWER:
				eval_vect[i] = pow(eval_vect[prg[i].addr1], eval_vect[prg[i].addr2]);
				break;
			case O_SQRT:
				if (eval_vect[prg[i].addr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = sqrt(eval_vect[prg[i].addr1]);
				break;
			case O_EXP:
				eval_vect[i] = exp(eval_vect[prg[i].addr1]);

				break;
			case O_POW10:
				eval_vect[i] = pow(10, eval_vect[prg[i].addr1]);
				break;
			case O_LN:
				if (eval_vect[prg[i].addr1] <= 0)
					is_error_case = true;
				else                // an exception occured !!!
					eval_vect[i] = log(eval_vect[prg[i].addr1]);
				break;
			case O_LOG10:
				if (eval_vect[prg[i].addr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = log10(eval_vect[prg[i].addr1]);
				break;
			case O_lOG2:
				if (eval_vect[prg[i].addr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = log2(eval_vect[prg[i].addr1]);
				break;
			case O_FLOOR:
				eval_vect[i] = floor(eval_vect[prg[i].addr1]);
				break;
			case O_CEIL:
				eval_vect[i] = ceil(eval_vect[prg[i].addr1]);
				break;
			case O_ABS:
				eval_vect[i] = fabs(eval_vect[prg[i].addr1]);
				break;
			case O_INV:
				if (fabs(eval_vect[prg[i].addr1]) < MEP_DIVISION_PROTECT)
					is_error_case = true;
				else
					eval_vect[i] = 1.0 / eval_vect[prg[i].addr1];
				break;
			case O_NEG:
				eval_vect[i] = -eval_vect[prg[i].addr1];
				break;
			case O_X2:
				eval_vect[i] = eval_vect[prg[i].addr1] * eval_vect[prg[i].addr1];
				break;
			case O_MIN:
				eval_vect[i] = eval_vect[prg[i].addr1] < eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr1] : eval_vect[prg[i].addr2];
				break;
			case O_MAX:
				eval_vect[i] = eval_vect[prg[i].addr1] > eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr1] : eval_vect[prg[i].addr2];
				break;

			case O_SIN:
				eval_vect[i] = sin(eval_vect[prg[i].addr1]);
				break;
			case O_COS:
				eval_vect[i] = cos(eval_vect[prg[i].addr1]);
				break;
			case O_TAN:
				eval_vect[i] = tan(eval_vect[prg[i].addr1]);
				break;

			case O_ASIN:
				eval_vect[i] = asin(eval_vect[prg[i].addr1]);
				break;
			case O_ACOS:
				eval_vect[i] = acos(eval_vect[prg[i].addr1]);
				break;
			case O_ATAN:
				eval_vect[i] = atan(eval_vect[prg[i].addr1]);
				break;
			case O_IFLZ:
				eval_vect[i] = eval_vect[prg[i].addr1] < 0 ? eval_vect[prg[i].addr2] : eval_vect[prg[i].addr3];
				break;
			case O_IFALBCD:
				eval_vect[i] = eval_vect[prg[i].addr1] < eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
				break;
			case O_IF_A_OR_B_CD:
				eval_vect[i] = (eval_vect[prg[i].addr1] < 0) || (eval_vect[prg[i].addr2] < 0) ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
				break;

			case O_IF_A_XOR_B_CD:
				eval_vect[i] = (eval_vect[prg[i].addr1] < 0) != (eval_vect[prg[i].addr2] < 0) ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
				break;

			case O_FMOD:
				if (fabs(eval_vect[prg[i].addr2]) < MEP_DIVISION_PROTECT)
					is_error_case = true;
				else
					eval_vect[i] = fmod(eval_vect[prg[i].addr1], eval_vect[prg[i].addr2]);
				break;

			default:  // a variable
				if (prg[i].op < (int)num_total_variables)
					eval_vect[i] = data[k][prg[i].op];
				else
					eval_vect[i] = real_constants[prg[i].op - num_total_variables];
				break;
			}
			if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
				delete[] eval_vect;
				// must redo everything again
			}
			else
				// everything ok - I must compute the difference between what I obtained and what I should obtain
				sum_of_errors_array[i] += mep_error_function(eval_vect[i], data[k][num_total_variables]);
		}
	}

	unsigned int num_data = mep_dataset.get_num_rows();
	for (unsigned int i = 0; i < code_length; i++) {    // find the best gene
		if (fitness > sum_of_errors_array[i] / num_data) {
			fitness = sum_of_errors_array[i] / num_data;
			index_best_genes[0] = i;
		}
	}
	max_index_best_genes = index_best_genes[0];
	errno = 0;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_regression_error_on_double_data_return_error(
	double** data, unsigned int num_data, unsigned int output_col, double& error, unsigned int& index_error_gene,
	t_mep_error_function mep_error_function)
{
	error = 0;
	double actual_output_double[1];

	int num_valid = 0;
	for (unsigned int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			error += mep_error_function(data[k][output_col], actual_output_double[0]);
			num_valid++;
		}
		else
			return false;
	}

	error /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_regression_error_on_double_data(double** data, unsigned int num_data,
	unsigned int output_col, double& error, t_mep_error_function mep_error_function)
{
	error = 0;
	double actual_output_double[1];
	unsigned int num_valid = 0;
	unsigned int index_error_gene;
	for (unsigned int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			error += mep_error_function(data[k][output_col], actual_output_double[0]);
			num_valid++;
		}
	}
	if (num_valid)
		error /= num_valid;
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
