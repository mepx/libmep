// Multi Expression Programming library
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
void t_mep_chromosome::fitness_regression_cache_all_training_data(
	const t_mep_data& mep_dataset,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	long long** cached_variables_eval_matrix,
	double** cached_sum_of_errors,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	long long** eval_matrix,
	char* gene_used_for_output,
	t_mep_error_function_long mep_error_function,
	t_seed& seed)
{
	long long** data = mep_dataset.get_data_matrix_as_long();
	unsigned int num_rows = mep_dataset.get_num_rows();

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	fitness = 0;
	//index_best_genes[0] = -1;

	int* line_of_constants = NULL;
	long long* cached_sum_of_errors_for_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		cached_sum_of_errors_for_constants = new long long[num_constants];
		for (unsigned int i = 0; i < num_constants; i++) {
			line_of_constants[i] = -1;
			cached_sum_of_errors_for_constants[i] = -1;
		}
	}

	compute_eval_matrix(num_rows,
		cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables,
		line_of_constants, eval_matrix, seed);

	for (unsigned int o = 0; o < code_length; o++)
		gene_used_for_output[o] = 0;

	for (unsigned int o = 0; o < num_program_outputs; o++){    //for each output
		double best_error_for_output = DBL_MAX;
		for (unsigned int i = 0; i < code_length; i++) {   // read the t_mep_chromosome from top to down
			double sum_of_errors;
			
			if (prg[i].op >= 0) {// variable or constant
				if (prg[i].op < (int)num_total_variables){ // a variable, which is cached already
					sum_of_errors = cached_sum_of_errors[prg[i].op][o];
				}
				else {// a constant
					sum_of_errors = 0;
					unsigned int constant_index = (unsigned int)prg[i].op - num_total_variables;
					if (cached_sum_of_errors_for_constants[constant_index] < -0.5) {// this is not cached?
						long long* eval = eval_matrix[line_of_constants[constant_index]];
						for (unsigned int k = 0; k < random_subset_selection_size; k++)
							sum_of_errors += mep_error_function(eval[random_subset_indexes[k]],
																data[random_subset_indexes[k]][num_total_variables + o]);
						sum_of_errors /= double(random_subset_selection_size);
					}
					else{
						sum_of_errors = (double)cached_sum_of_errors_for_constants[constant_index];
					}
				}
			}
			else {// operator
				long long* eval = eval_matrix[i];
				sum_of_errors = 0;
				for (unsigned int k = 0; k < random_subset_selection_size; k++)
					sum_of_errors += mep_error_function(eval[random_subset_indexes[k]],
														data[random_subset_indexes[k]][num_total_variables + o]);
				sum_of_errors /= double(random_subset_selection_size);
			}
			if (best_error_for_output > sum_of_errors &&
				!gene_used_for_output[i]) {
				best_error_for_output = sum_of_errors;
				index_best_genes[o] = i;
			}
		}// end for i
		gene_used_for_output[index_best_genes[o]] = 1;// mark that gene as output
		fitness += best_error_for_output; // Fitness is the sum of errors for each output

	}// end for o
	
	if (line_of_constants)
		delete[] line_of_constants;

	if (cached_sum_of_errors_for_constants)
		delete[] cached_sum_of_errors_for_constants;
		
	max_index_best_genes = index_best_genes[0];
	for (unsigned int o = 1; o < num_program_outputs; o++)
		if (max_index_best_genes < index_best_genes[o])
			max_index_best_genes = index_best_genes[o];
}
//---------------------------------------------------------------------------
/*
void t_mep_chromosome::fitness_regression_double_no_cache(
 const t_mep_data& mep_dataset,
	unsigned int*,// random_subset_indexes
unsigned int, //random_subset_selection_size
	double* eval_vect, double* sum_of_errors_array,
t_mep_error_function_double mep_error_function)
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
				eval_vect[i] = eval_vect[prg[i].addr1] < 0 != eval_vect[prg[i].addr2] < 0 ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
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
*/
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_regression_error_on_data_return_error_long(
					const t_mep_data &mep_data,
								double* error_per_output,
								double& total_error,
							unsigned int& index_error_gene,
	t_mep_error_function_long mep_error_function) const
{
	long long** data = mep_data.get_data_matrix_as_long();
	unsigned int num_data = mep_data.get_num_rows();
	
	long long *actual_output_long = new long long[num_program_outputs];

	for (unsigned int o = 0; o < num_program_outputs; o++)
		error_per_output[o] = 0;

	for (unsigned int o = 0; o < num_program_outputs; o++) {
		int num_valid = 0;
		
		for (unsigned int k = 0; k < num_data; k++) {
			if (evaluate(data[k], actual_output_long, index_error_gene)) {
				error_per_output[o] += mep_error_function(data[k][num_total_variables + o],
											actual_output_long[o]);
				num_valid++;
			}
			else{
				delete[] actual_output_long;
				return false;
			}
		}
		
		error_per_output[o] /= num_data;
	}
	total_error = 0;
	for (unsigned int o = 0; o < num_program_outputs; o++)
		total_error += error_per_output[o];
	delete[] actual_output_long;
	
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_regression_error_on_data_long(
	const t_mep_data &mep_data,
	double* error_per_output,
	double& total_error,
	t_mep_error_function_long mep_error_function) const
{
	long long** data = mep_data.get_data_matrix_as_long();
	unsigned int num_data = mep_data.get_num_rows();

	for (unsigned int o = 0; o < num_program_outputs; o++)
		error_per_output[o] = 0;

	long long *actual_output_long = new long long[num_program_outputs];

	unsigned int index_error_gene;

	for (unsigned int o = 0; o < num_program_outputs; o++) {
		unsigned int num_valid = 0;
		for (unsigned int k = 0; k < num_data; k++) {
			if (evaluate(data[k], actual_output_long, index_error_gene)) {
				error_per_output[o] += mep_error_function(data[k][num_total_variables + o],
											actual_output_long[o]);
				num_valid++;
			}
		}
		if (num_valid)
			error_per_output[o] /= num_valid;
		else{
			delete[] actual_output_long;
			return false;
		}
	}
	total_error = 0;
	for (unsigned int o = 0; o < num_program_outputs; o++)
		total_error += error_per_output[o];
	delete[] actual_output_long;
	
	return true;
}
//---------------------------------------------------------------------------
