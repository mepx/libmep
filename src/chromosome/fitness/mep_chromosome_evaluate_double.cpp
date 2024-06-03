// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <math.h>
#include <errno.h>
#include <float.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
#include "mep_functions.h"
//---------------------------------------------------------------------------
bool t_mep_chromosome::evaluate(double *inputs,
								double *outputs,
								unsigned int &index_error_gene) const
{
	double *eval_vect = new double[max_index_best_genes + 1];

	for (unsigned int i = 0; i <= max_index_best_genes; i++){   // read the t_mep_chromosome from top to down
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		bool is_error_case = false;// division by zero, other errors
		switch (prg[i].op) {
		case  O_ADDITION:  // +
			eval_vect[i] = eval_vect[prg[i].addr[0]] + eval_vect[prg[i].addr[1]];
			break;
		case  O_SUBTRACTION:  // -
			eval_vect[i] = eval_vect[prg[i].addr[0]] - eval_vect[prg[i].addr[1]];
			break;
		case  O_MULTIPLICATION:  // *
			eval_vect[i] = eval_vect[prg[i].addr[0]] * eval_vect[prg[i].addr[1]];
			break;
		case  O_DIVISION:  //  /
			if (fabs(eval_vect[prg[i].addr[1]]) < MEP_DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = eval_vect[prg[i].addr[0]] / eval_vect[prg[i].addr[1]];
			break;
		case O_POWER:
			eval_vect[i] = pow(eval_vect[prg[i].addr[0]], eval_vect[prg[i].addr[1]]);
			break;
		case O_SQRT:
			if (eval_vect[prg[i].addr[0]] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = sqrt(eval_vect[prg[i].addr[0]]);
			break;
		case O_EXP:
			eval_vect[i] = exp(eval_vect[prg[i].addr[0]]);

			break;
		case O_POW10:
			eval_vect[i] = pow(10, eval_vect[prg[i].addr[0]]);
			break;
		case O_LN:
			if (eval_vect[prg[i].addr[0]] <= 0)
				is_error_case = true;
			else                // an exception occured !!!
				eval_vect[i] = log(eval_vect[prg[i].addr[0]]);
			break;
		case O_LOG10:
			if (eval_vect[prg[i].addr[0]] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log10(eval_vect[prg[i].addr[0]]);
			break;
		case O_LOG2:
			if (eval_vect[prg[i].addr[0]] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log2(eval_vect[prg[i].addr[0]]);
			break;
		case O_FLOOR:
			eval_vect[i] = floor(eval_vect[prg[i].addr[0]]);
			break;
		case O_CEIL:
			eval_vect[i] = ceil(eval_vect[prg[i].addr[0]]);
			break;
		case O_ABS:
			eval_vect[i] = fabs(eval_vect[prg[i].addr[0]]);
			break;
		case O_INV:
			if (fabs(eval_vect[prg[i].addr[0]]) < MEP_DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = 1 / eval_vect[prg[i].addr[0]];
			break;
		case O_NEG:
			eval_vect[i] = -eval_vect[prg[i].addr[0]];
			break;
		case O_X2:
			eval_vect[i] = eval_vect[prg[i].addr[0]] * eval_vect[prg[i].addr[0]];
			break;
		case O_MIN:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < eval_vect[prg[i].addr[1]] ? eval_vect[prg[i].addr[0]] : eval_vect[prg[i].addr[1]];
			break;
		case O_MAX:
			eval_vect[i] = eval_vect[prg[i].addr[0]] > eval_vect[prg[i].addr[1]] ? eval_vect[prg[i].addr[0]] : eval_vect[prg[i].addr[1]];
			break;

		case O_SIN:
			eval_vect[i] = sin(eval_vect[prg[i].addr[0]]);
			break;
		case O_COS:
			eval_vect[i] = cos(eval_vect[prg[i].addr[0]]);
			break;
		case O_TAN:
			eval_vect[i] = tan(eval_vect[prg[i].addr[0]]);
			break;

		case O_ASIN:
			if (eval_vect[prg[i].addr[0]] < -1 || eval_vect[prg[i].addr[0]] > 1)
				is_error_case = true;
			else
				eval_vect[i] = asin(eval_vect[prg[i].addr[0]]);
			break;
		case O_ACOS:
			if (eval_vect[prg[i].addr[0]] < -1 || eval_vect[prg[i].addr[0]] > 1)
				is_error_case = true;
			else
				eval_vect[i] = acos(eval_vect[prg[i].addr[0]]);
			break;
		case O_ATAN:
			eval_vect[i] = atan(eval_vect[prg[i].addr[0]]);
			break;
		case O_IFLZ:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < 0 ? eval_vect[prg[i].addr[1]] : eval_vect[prg[i].addr[2]];
			break;
		case O_IFALBCD:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < eval_vect[prg[i].addr[1]] ? eval_vect[prg[i].addr[2]] : eval_vect[prg[i].addr[3]];
			break;
		case O_IF_A_OR_B_CD:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < 0 || eval_vect[prg[i].addr[1]] < 0 ? eval_vect[prg[i].addr[2]] : eval_vect[prg[i].addr[3]];
			break;
		case O_IF_A_XOR_B_CD:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < 0 != eval_vect[prg[i].addr[1]] < 0 ? eval_vect[prg[i].addr[2]] : eval_vect[prg[i].addr[3]];
			break;
		case O_FMOD:
			if (fabs(eval_vect[prg[i].addr[1]]) < MEP_DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = fmod(eval_vect[prg[i].addr[0]], eval_vect[prg[i].addr[1]]);
			break;
		case O_INPUTS_AVERAGE:
			eval_vect[i] = 0;
			break;
		case O_NUM_INPUTS:
			eval_vect[i] = 0;
			break;

		default:  // a variable
			if (prg[i].op < (int)num_total_variables)
				eval_vect[i] = inputs[prg[i].op];
			else
				eval_vect[i] = real_constants[prg[i].op - num_total_variables];
			break;
		}
		if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
			delete[] eval_vect;
			index_error_gene = i;
			return false;
		}
	}
	for (unsigned int o = 0; o < num_program_outputs; o++)
		outputs[o] = eval_vect[index_best_genes[o]];
	
	delete[] eval_vect;
	errno = 0;
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::get_first_max_index(double *inputs,
										   unsigned int &max_index,
										   double & max_value,
										   unsigned int &index_error_gene,
										   double *values_for_output_genes) const
{
	double *eval_vect = new double[code_length];

	max_index = 0;
	max_value = -DBL_MAX;

	for (unsigned int i = 0; i < code_length; i++){   // read the t_mep_chromosome from top to down
	
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		bool is_error_case = false;// division by zero, other errors
		switch (prg[i].op) {
		case  O_ADDITION:  // +
			eval_vect[i] = eval_vect[prg[i].addr[0]] + eval_vect[prg[i].addr[1]];
			break;
		case  O_SUBTRACTION:  // -
			eval_vect[i] = eval_vect[prg[i].addr[0]] - eval_vect[prg[i].addr[1]];
			break;
		case  O_MULTIPLICATION:  // *
			eval_vect[i] = eval_vect[prg[i].addr[0]] * eval_vect[prg[i].addr[1]];
			break;
		case  O_DIVISION:  //  /
			if (fabs(eval_vect[prg[i].addr[1]]) < MEP_DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = eval_vect[prg[i].addr[0]] / eval_vect[prg[i].addr[1]];
			break;
		case O_POWER:
			eval_vect[i] = pow(eval_vect[prg[i].addr[0]], eval_vect[prg[i].addr[1]]);
			break;
		case O_SQRT:
			if (eval_vect[prg[i].addr[0]] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = sqrt(eval_vect[prg[i].addr[0]]);
			break;
		case O_EXP:
			eval_vect[i] = exp(eval_vect[prg[i].addr[0]]);

			break;
		case O_POW10:
			eval_vect[i] = pow(10, eval_vect[prg[i].addr[0]]);
			break;
		case O_LN:
			if (eval_vect[prg[i].addr[0]] <= 0)
				is_error_case = true;
			else                // an exception occured !!!
				eval_vect[i] = log(eval_vect[prg[i].addr[0]]);
			break;
		case O_LOG10:
			if (eval_vect[prg[i].addr[0]] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log10(eval_vect[prg[i].addr[0]]);
			break;
		case O_LOG2:
			if (eval_vect[prg[i].addr[0]] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log2(eval_vect[prg[i].addr[0]]);
			break;
		case O_FLOOR:
			eval_vect[i] = floor(eval_vect[prg[i].addr[0]]);
			break;
		case O_CEIL:
			eval_vect[i] = ceil(eval_vect[prg[i].addr[0]]);
			break;
		case O_ABS:
			eval_vect[i] = fabs(eval_vect[prg[i].addr[0]]);
			break;
		case O_INV:
			if (fabs(eval_vect[prg[i].addr[0]]) < MEP_DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = 1.0 / eval_vect[prg[i].addr[0]];
			break;
		case O_NEG:
			eval_vect[i] = -eval_vect[prg[i].addr[0]];
			break;
		case O_X2:
			eval_vect[i] = eval_vect[prg[i].addr[0]] * eval_vect[prg[i].addr[0]];
			break;
		case O_MIN:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < eval_vect[prg[i].addr[1]] ? eval_vect[prg[i].addr[0]] : eval_vect[prg[i].addr[1]];
			break;
		case O_MAX:
			eval_vect[i] = eval_vect[prg[i].addr[0]] > eval_vect[prg[i].addr[1]] ? eval_vect[prg[i].addr[0]] : eval_vect[prg[i].addr[1]];
			break;

		case O_SIN:
			eval_vect[i] = sin(eval_vect[prg[i].addr[0]]);
			break;
		case O_COS:
			eval_vect[i] = cos(eval_vect[prg[i].addr[0]]);
			break;
		case O_TAN:
			eval_vect[i] = tan(eval_vect[prg[i].addr[0]]);
			break;

		case O_ASIN:
			if (eval_vect[prg[i].addr[0]] < -1 || eval_vect[prg[i].addr[0]] > 1)
				is_error_case = true;
			else
				eval_vect[i] = asin(eval_vect[prg[i].addr[0]]);
			break;
		case O_ACOS:
			if (eval_vect[prg[i].addr[0]] < -1 || eval_vect[prg[i].addr[0]] > 1)
				is_error_case = true;
			else
				eval_vect[i] = acos(eval_vect[prg[i].addr[0]]);
			break;
		case O_ATAN:
			eval_vect[i] = atan(eval_vect[prg[i].addr[0]]);
			break;
		case O_IFLZ:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < 0 ? eval_vect[prg[i].addr[1]] : eval_vect[prg[i].addr[2]];
			break;
		case O_IFALBCD:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < eval_vect[prg[i].addr[1]] ? eval_vect[prg[i].addr[2]] : eval_vect[prg[i].addr[3]];
			break;
		case O_IF_A_OR_B_CD:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < 0 || eval_vect[prg[i].addr[1]] < 0 ? eval_vect[prg[i].addr[2]] : eval_vect[prg[i].addr[3]];
			break;
		case O_IF_A_XOR_B_CD:
			eval_vect[i] = eval_vect[prg[i].addr[0]] < 0 != eval_vect[prg[i].addr[1]] < 0 ? eval_vect[prg[i].addr[2]] : eval_vect[prg[i].addr[3]];
			break;
		case O_FMOD:
			if (fabs(eval_vect[prg[i].addr[1]]) < MEP_DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = fmod(eval_vect[prg[i].addr[0]], eval_vect[prg[i].addr[1]]);
			break;

		default:  // a variable
			if (prg[i].op < (int)num_total_variables)
				eval_vect[i] = inputs[prg[i].op];
			else
				eval_vect[i] = real_constants[prg[i].op - num_total_variables];
			break;
		} // end switch

		if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
			delete[] eval_vect;
			index_error_gene = i;
			return false;
		}
		else
			if (max_value < eval_vect[i]) {
				max_value = eval_vect[i];
				max_index = i;
			}
	}

	if (values_for_output_genes) {
		for (unsigned int c = 0; c < num_classes; c++)
			values_for_output_genes[c] = eval_vect[index_best_genes[c]];
	}

	delete[] eval_vect;
	errno = 0;
	return true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::compute_eval_matrix(
										   unsigned int num_training_data,
			double **cached_variables_eval_matrix,
			unsigned int num_actual_variables,
												  unsigned int * actual_enabled_variables,
			int *line_of_constants, double ** eval_double,
										   t_seed & seed)
{
	//	bool is_error_case;  // division by zero, other errors

	for (unsigned int i = 0; i < code_length; i++){   // read the t_mep_chromosome from top to down
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		double *arg1, *arg2, *arg3, *arg4;
		double *eval = eval_double[i];
		//int num_training_data = mep_dataset->get_num_rows();

		if (prg[i].op < 0) {// an operator
			if (prg[prg[i].addr[0]].op >= 0)
				if (prg[prg[i].addr[0]].op < (int)num_total_variables)
					arg1 = cached_variables_eval_matrix[prg[prg[i].addr[0]].op];
				else
					arg1 = eval_double[line_of_constants[prg[prg[i].addr[0]].op - num_total_variables]];
			else
				arg1 = eval_double[prg[i].addr[0]];

			if (prg[prg[i].addr[1]].op >= 0)
				if (prg[prg[i].addr[1]].op < (int)num_total_variables)
					arg2 = cached_variables_eval_matrix[prg[prg[i].addr[1]].op];
				else
					arg2 = eval_double[line_of_constants[prg[prg[i].addr[1]].op - num_total_variables]];
			else
				arg2 = eval_double[prg[i].addr[1]];

			if (prg[prg[i].addr[2]].op >= 0)
				if (prg[prg[i].addr[2]].op < (int)num_total_variables)
					arg3 = cached_variables_eval_matrix[prg[prg[i].addr[2]].op];
				else
					arg3 = eval_double[line_of_constants[prg[prg[i].addr[2]].op - num_total_variables]];
			else
				arg3 = eval_double[prg[i].addr[2]];

			if (prg[prg[i].addr[3]].op >= 0)
				if (prg[prg[i].addr[3]].op < (int)num_total_variables)
					arg4 = cached_variables_eval_matrix[prg[prg[i].addr[3]].op];
				else
					arg4 = eval_double[line_of_constants[prg[prg[i].addr[3]].op - num_total_variables]];
			else
				arg4 = eval_double[prg[i].addr[3]];
		}
		else {
			arg1 = arg2 = arg3 = arg4 = NULL; // just to silence some compiler warnings
		}

		switch (prg[i].op) {
		case  O_ADDITION: {  // +
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = arg1[k] + arg2[k];
				/*
				if (fabs(eval[k]) > MEP_MAX_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				*/
			}
		}
			break;
		case  O_SUBTRACTION: {  // -
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = arg1[k] - arg2[k];
				/*
				if (fabs(eval[k]) > MEP_MAX_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				*/
			}
		}
			break;
		case  O_MULTIPLICATION: {  // *
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = arg1[k] * arg2[k];
				
				if (isinf(fabs(eval[k]))) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				
			}
		}
			break;
		case  O_DIVISION:  //  /
			for (unsigned int k = 0; k < num_training_data; k++)
				if (fabs(arg2[k]) < MEP_DIVISION_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else {
					eval[k] = arg1[k] / arg2[k];
					
					if (isinf(fabs(eval[k]))) {
						prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
						break;
					}
					
				}
			break;
		case O_POWER:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = pow(arg1[k], arg2[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_SQRT:
			for (unsigned int k = 0; k < num_training_data; k++) {
				if (arg1[k] <= 0) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				else {
					eval[k] = sqrt(arg1[k]);
					/*
					if (fabs(eval[k]) > MEP_MAX_PROTECT) {
						prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
						break;
					}
					*/
				}
			}
			break;
		case O_EXP:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = exp(arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
			}

			break;
		case O_POW10:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = pow(10, arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_LN:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				else {
					eval[k] = log(arg1[k]);
				}

			break;
		case O_LOG10:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				else {
					eval[k] = log10(arg1[k]);
				}

			break;
		case O_LOG2:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				else {
					eval[k] = log2(arg1[k]);
				}
			break;
		case O_FLOOR:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = floor(arg1[k]);
			break;
		case O_CEIL:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = ceil(arg1[k]);
			break;
		case O_ABS:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = fabs(arg1[k]);
			break;
		case O_INV:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (fabs(arg1[k]) < MEP_DIVISION_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else {
					eval[k] = 1.0 / arg1[k];
					
					if (isinf(fabs(eval[k]))) {
						prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
						break;
					}
					
				}
			break;
		case O_NEG:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = -arg1[k];
			break;
		case O_X2:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = arg1[k] * arg1[k];
				
				if (isinf(fabs(eval[k]))) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				
			}
			break;
		case O_MIN:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < arg2[k] ? arg1[k] : arg2[k];
			break;
		case O_MAX:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] > arg2[k] ? arg1[k] : arg2[k];
			break;
		case O_SIN:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = sin(arg1[k]);
			break;
		case O_COS:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = cos(arg1[k]);
			break;
		case O_TAN:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = tan(arg1[k]);
			break;

		case O_ASIN:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = asin(arg1[k]);

			break;
		case O_ACOS:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = acos(arg1[k]);

			break;
		case O_ATAN:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = atan(arg1[k]);
			}
			break;
		case O_IFLZ:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < 0 ? arg2[k] : arg3[k];
			break;
		case O_IFALBCD:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < arg2[k] ? arg3[k] : arg4[k];
			break;
		case O_IF_A_OR_B_CD:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < 0 || arg2[k] < 0 ? arg3[k] : arg4[k];
			break;
		case O_IF_A_XOR_B_CD:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < 0 != arg2[k] < 0 ? arg3[k] : arg4[k];
			break;
		case O_FMOD:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (fabs(arg2[k]) < MEP_DIVISION_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else {
					eval[k] = fmod(arg1[k], arg2[k]);
					/*
					if (fabs(eval[k]) > MEP_MAX_PROTECT) {
						prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];   // the gene is mutated into a terminal
						break;
					}
					*/
				}
			
			break;

		default:  // a constant
			if (prg[i].op >= (int)num_total_variables)
				if (line_of_constants[prg[i].op - num_total_variables] == -1) {
					line_of_constants[prg[i].op - num_total_variables] = (int)i;
					unsigned int constant_index = (unsigned int)prg[i].op - num_total_variables;
					for (unsigned int k = 0; k < num_training_data; k++)
						eval[k] = real_constants[constant_index];// why do I keep the same constant for an entire row??????
				}

			break;
		}

	}
	errno = 0;
}
//---------------------------------------------------------------------------
// 661
