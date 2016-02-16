
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "mep_rands.h"
#include "libmep.h"

#include <errno.h>


#ifdef WIN32
#include <windows.h>
#endif


#define DIVISION_PROTECT 1E-10
//---------------------------------------------------------------------------
t_mep::t_mep()
{
	strcpy(version, "2016.02.16.1-beta");

	num_operators = 0;


	cached_eval_matrix_double = NULL;
	cached_sum_of_errors = NULL;

	stats = NULL;
	modified_project = false;
	_stopped = true;
	last_run_index = -1;

	variables_enabled = NULL;
	actual_enabled_variables = NULL;
	num_actual_variables = 0;
	num_total_variables = 0;
	target_col = -1;
	cache_results_for_all_training_data = true;

	problem_description = new char[100];
	strcpy(problem_description, "Problem description here ...");

	training_data = NULL;
	validation_data = NULL;
	test_data = NULL;
}
//---------------------------------------------------------------------------
t_mep::~t_mep()
{
	if (actual_enabled_variables) {
		delete[] actual_enabled_variables;
		actual_enabled_variables = NULL;
	}
	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}

	if (stats) {
		delete[] stats;
		stats = NULL;
	}

	if (problem_description) {
		delete[] problem_description;
		problem_description = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep::allocate_values(double ****eval_double, s_value_class ***array_value_class)
{
	*eval_double = new double**[parameters.num_threads];
	for (int c = 0; c < parameters.num_threads; c++) {
		(*eval_double)[c] = new double*[parameters.code_length];
		for (int i = 0; i < parameters.code_length; i++)
			(*eval_double)[c][i] = new double[training_data->get_num_rows()];
	}

	cached_eval_matrix_double = new double*[num_total_variables];
	for (int i = 0; i < num_total_variables; i++)
		cached_eval_matrix_double[i] = NULL;
	for (int i = 0; i < num_actual_variables; i++)
		cached_eval_matrix_double[actual_enabled_variables[i]] = new double[training_data->get_num_rows()];

	cached_sum_of_errors = new double[num_total_variables];
	cached_threashold = new double[num_total_variables];

	*array_value_class = new s_value_class*[parameters.num_threads];
	for (int c = 0; c < parameters.num_threads; c++)
		(*array_value_class)[c] = new s_value_class[training_data->get_num_rows()];
}
//---------------------------------------------------------------------------
void t_mep::allocate_sub_population(t_sub_population &pop)
{
	pop.offspring1.allocate_memory(parameters.code_length, num_total_variables, parameters.constants_probability > 1E-6, &parameters.constants);
	pop.offspring2.allocate_memory(parameters.code_length, num_total_variables, parameters.constants_probability > 1E-6, &parameters.constants);
	pop.individuals = new t_mep_chromosome[parameters.subpopulation_size];
	for (int j = 0; j < parameters.subpopulation_size; j++)
		pop.individuals[j].allocate_memory(parameters.code_length, num_total_variables, parameters.constants_probability > 1E-6, &parameters.constants);
}
//---------------------------------------------------------------------------
void t_mep::get_best(t_mep_chromosome& dest)
{
	dest = pop[best_subpopulation_index].individuals[best_individual_index];
}
//---------------------------------------------------------------------------
void t_mep::generate_random_individuals(void) // randomly initializes the individuals
{
	for (int i = 0; i < parameters.num_subpopulations; i++)
		for (int j = 0; j < parameters.subpopulation_size; j++)
			pop[i].individuals[j].generate_random(&parameters, actual_operators, num_operators, actual_enabled_variables, num_actual_variables);
}
//---------------------------------------------------------------------------
void t_mep::fitness_regression(t_mep_chromosome &individual, double **eval_matrix)
{
	fitness_regression_double_cache_all_training_data(individual, eval_matrix);
}
//---------------------------------------------------------------------------
void t_mep::fitness_classification(t_mep_chromosome &individual, double **eval, s_value_class *tmp_value_class)
{
	fitness_classification_double_cache_all_training_data(individual, eval, tmp_value_class);
}
//---------------------------------------------------------------------------
void t_mep::fitness_regression_double(t_mep_chromosome &Individual, double* eval_vect, double *sum_of_errors_array)
{
	double **data = training_data->get_data_matrix_double();

	Individual.fit = 1E+308;
	Individual.best = -1;

	for (int i = 0; i < parameters.code_length; i++)
		sum_of_errors_array[i] = 0;

	for (int k = 0; k < training_data->get_num_rows(); k++) {   // read the t_mep_chromosome from top to down
		for (int i = 0; i < parameters.code_length; i++) {    // read the t_mep_chromosome from top to down

			errno = 0;
			bool is_error_case = false;
			switch (Individual.prg[i].op) {
			case  O_ADDITION:  // +
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] + eval_vect[Individual.prg[i].adr2];
				break;
			case  O_SUBTRACTION:  // -
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] - eval_vect[Individual.prg[i].adr2];
				break;
			case  O_MULTIPLICATION:  // *
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] * eval_vect[Individual.prg[i].adr2];
				break;
			case  O_DIVISION:  //  /
				if (fabs(eval_vect[Individual.prg[i].adr2]) < DIVISION_PROTECT)
					is_error_case = true;
				else
					eval_vect[i] = eval_vect[Individual.prg[i].adr1] / eval_vect[Individual.prg[i].adr2];
				break;
			case O_POWER:
				eval_vect[i] = pow(eval_vect[Individual.prg[i].adr1], eval_vect[Individual.prg[i].adr2]);
				break;
			case O_SQRT:
				if (eval_vect[Individual.prg[i].adr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = sqrt(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_EXP:
				eval_vect[i] = exp(eval_vect[Individual.prg[i].adr1]);

				break;
			case O_POW10:
				eval_vect[i] = pow(10, eval_vect[Individual.prg[i].adr1]);
				break;
			case O_LN:
				if (eval_vect[Individual.prg[i].adr1] <= 0)
					is_error_case = true;
				else                // an exception occured !!!
					eval_vect[i] = log(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_LOG10:
				if (eval_vect[Individual.prg[i].adr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = log10(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_lOG2:
				if (eval_vect[Individual.prg[i].adr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = log2(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_FLOOR:
				eval_vect[i] = floor(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_CEIL:
				eval_vect[i] = ceil(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_ABS:
				eval_vect[i] = fabs(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_INV:
				eval_vect[i] = -eval_vect[Individual.prg[i].adr1];
				break;
			case O_X2:
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] * eval_vect[Individual.prg[i].adr1];
				break;
			case O_MIN:
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] < eval_vect[Individual.prg[i].adr2] ? eval_vect[Individual.prg[i].adr1] : eval_vect[Individual.prg[i].adr2];
				break;
			case O_MAX:
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] > eval_vect[Individual.prg[i].adr2] ? eval_vect[Individual.prg[i].adr1] : eval_vect[Individual.prg[i].adr2];
				break;

			case O_SIN:
				eval_vect[i] = sin(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_COS:
				eval_vect[i] = cos(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_TAN:
				eval_vect[i] = tan(eval_vect[Individual.prg[i].adr1]);
				break;

			case O_ASIN:
				eval_vect[i] = asin(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_ACOS:
				eval_vect[i] = acos(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_ATAN:
				eval_vect[i] = atan(eval_vect[Individual.prg[i].adr1]);
				break;
			case O_IFLZ:
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] < 0 ? eval_vect[Individual.prg[i].adr2] : eval_vect[Individual.prg[i].adr3];
				break;
			case O_IFALBCD:
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] < eval_vect[Individual.prg[i].adr2] ? eval_vect[Individual.prg[i].adr3] : eval_vect[Individual.prg[i].adr4];
				break;

			default:  // a variable
				if (Individual.prg[i].op < Individual.num_total_variables)
					eval_vect[i] = data[k][Individual.prg[i].op];
				else
					eval_vect[i] = Individual.constants_double[Individual.prg[i].op - Individual.num_total_variables];
				break;
			}
			if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
				delete[] eval_vect;
				// must redo everything again
			}
			else
				// everything ok - I must compute the difference between what I obtained and what I should obtain
				sum_of_errors_array[i] += fabs(eval_vect[i] - data[k][num_total_variables]);
		}
	}

	for (int i = 0; i < parameters.code_length; i++) {    // find the best gene
		if (Individual.fit > sum_of_errors_array[i] / training_data->get_num_rows()) {
			Individual.fit = sum_of_errors_array[i] / training_data->get_num_rows();
			Individual.best = i;
		}
	}
}
//---------------------------------------------------------------------------
void t_mep::fitness_regression_double_cache_all_training_data(t_mep_chromosome &Individual, double** eval_matrix_double)
{
	double **data = training_data->get_data_matrix_double();

	// evaluate Individual
	// partial results are stored and used later in other sub-expressions

	Individual.fit = 1E+308;
	Individual.best = -1;

	int *line_of_constants = NULL;
	double* cached_sum_of_errors_for_constants = NULL;
	if (Individual.num_constants) {
		line_of_constants = new int[Individual.num_constants];// line where a constant was firstly computed
		cached_sum_of_errors_for_constants = new double[Individual.num_constants];
		for (int i = 0; i < Individual.num_constants; i++) {
			line_of_constants[i] = -1;
			cached_sum_of_errors_for_constants[i] = -1;
		}
	}

	compute_eval_matrix_double(Individual, eval_matrix_double, line_of_constants);

	int num_training_data = training_data->get_num_rows();

	for (int i = 0; i < parameters.code_length; i++) {   // read the t_mep_chromosome from top to down
		double sum_of_errors;


		if (Individual.prg[i].op >= 0)
			if (Individual.prg[i].op < num_total_variables) // a variable, which is cached already
				sum_of_errors = cached_sum_of_errors[Individual.prg[i].op];

			else {// a constant
				sum_of_errors = 0;
				int cst_index = Individual.prg[i].op - num_total_variables;
				if (cached_sum_of_errors_for_constants[cst_index] < -0.5) {
					double *eval = eval_matrix_double[line_of_constants[cst_index]];
					for (int k = 0; k < num_training_data; k++)
						sum_of_errors += fabs(eval[k] - data[k][num_total_variables]);
				}
				else
					sum_of_errors = cached_sum_of_errors_for_constants[cst_index];
			}

		else {
			double *eval = eval_matrix_double[i];
			sum_of_errors = 0;
			for (int k = 0; k < num_training_data; k++)
				sum_of_errors += fabs(eval[k] - data[k][num_total_variables]);
		}
		if (Individual.fit > sum_of_errors / training_data->get_num_rows()) {
			Individual.fit = sum_of_errors / training_data->get_num_rows();
			Individual.best = i;
		}
	}

	if (line_of_constants)
		delete[] line_of_constants;

	if (cached_sum_of_errors_for_constants)
		delete[] cached_sum_of_errors_for_constants;
}
//---------------------------------------------------------------------------
/*
void t_mep::fitness_classification_double_cache_all_training_data(t_mep_chromosome &, double **)
{
	
	// evaluate Individual
	// partial results are stored and used later in other sub-expressions

	Individual.fit = 1E+308;
	Individual.best = -1;

	int *line_of_constants = NULL;
	if (Individual.num_constants) {
	line_of_constants = new int[Individual.num_constants];// line where a constant was firstly computed
	for (int i = 0; i < Individual.num_constants; i++)
	line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(Individual, eval_double, line_of_constants);

	for (int i = 0; i < parameters.code_length; i++) {   // read the t_mep_chromosome from top to down
	double sum_of_errors;
	if (Individual.prg[i].op >= 0)
	if (Individual.prg[i].op < training_data.num_vars) // a variable, which is cached already
	sum_of_errors = cached_sum_of_errors[Individual.prg[i].op];
	else {// a constant
	double *eval = eval_double[line_of_constants[Individual.prg[i].op - training_data.num_vars]];
	sum_of_errors = 0;
	for (int k = 0; k < training_data->get_num_rows(); k++)
	if (eval[k] <= parameters.classification_threshold)
	sum_of_errors += training_data._target_double[k];
	else
	sum_of_errors += 1 - training_data._target_double[k];
	}
	else {
	double *eval = eval_double[i];
	sum_of_errors = 0;
	for (int k = 0; k < training_data->get_num_rows(); k++)
	if (eval[k] <= parameters.classification_threshold)
	sum_of_errors += training_data._target_double[k];
	else
	sum_of_errors += 1 - training_data._target_double[k];
	}

	if (Individual.fit > sum_of_errors / training_data->get_num_rows()) {
	Individual.fit = sum_of_errors / training_data->get_num_rows();
	Individual.best = i;
	}
	}

	if (line_of_constants)
	delete[] line_of_constants;
	
}
*/
//---------------------------------------------------------------------------
void t_mep::fitness_classification_double_cache_all_training_data(t_mep_chromosome &Individual, double **eval_matrix_double, s_value_class *tmp_value_class)
{

	// evaluate Individual
	// partial results are stored and used later in other sub-expressions

	double **data = training_data->get_data_matrix_double();

	Individual.fit = 1E+308;
	Individual.best = -1;

	int *line_of_constants = NULL;
	if (Individual.num_constants) {
		line_of_constants = new int[Individual.num_constants];// line where a constant was firstly computed
		for (int i = 0; i < Individual.num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(Individual, eval_matrix_double, line_of_constants);

	double best_threshold;
	for (int i = 0; i < parameters.code_length; i++) {   // read the t_mep_chromosome from top to down
		double sum_of_errors;
		if (Individual.prg[i].op >= 0)
			if (Individual.prg[i].op < num_total_variables) { // a variable, which is cached already
				sum_of_errors = cached_sum_of_errors[Individual.prg[i].op];
				best_threshold = cached_threashold[Individual.prg[i].op];
			}
			else {// a constant
				if (training_data->get_num_items_class_0() < training_data->get_num_rows() - training_data->get_num_items_class_0()) {// i must classify everything as 1
					sum_of_errors = training_data->get_num_items_class_0();
					best_threshold = eval_matrix_double[line_of_constants[Individual.prg[i].op - num_total_variables]][0] - 1;
				}
				else {// less of 1, I must classify everything as class 0
					sum_of_errors = training_data->get_num_rows() - training_data->get_num_items_class_0();
					best_threshold = eval_matrix_double[line_of_constants[Individual.prg[i].op - num_total_variables]][0];
				}
			}
		else {// an operator
			double *eval = eval_matrix_double[i];


			for (int k = 0; k < training_data->get_num_rows(); k++) {
				tmp_value_class[k].value = eval[k];
				tmp_value_class[k].data_class = (int)data[k][num_total_variables];
			}
			qsort((void*)tmp_value_class, training_data->get_num_rows(), sizeof(s_value_class), sort_function_value_class);

			int num_0_incorrect = training_data->get_num_items_class_0();
			int num_1_incorrect = 0;
			best_threshold = tmp_value_class[0].value - 1;// all are classified to class 1 in this case
			sum_of_errors = num_0_incorrect;

			for (int t = 0; t < training_data->get_num_rows(); t++) {
				int j = t + 1;
				while (j < training_data->get_num_rows() && fabs(tmp_value_class[t].value - tmp_value_class[j].value) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
					j++;

				// le verific pe toate intre i si j si le cataloghez ca apartinant la clasa 0
				for (int k = t; k < j; k++)
					if (tmp_value_class[k].data_class == 0)
						num_0_incorrect--;
					else
						if (tmp_value_class[k].data_class == 1) {
							//num_0_incorrect--;
							num_1_incorrect++;
						}
				if (num_0_incorrect + num_1_incorrect < sum_of_errors) {
					sum_of_errors = num_0_incorrect + num_1_incorrect;
					best_threshold = tmp_value_class[t].value;
				}
				t = j;
				t--;
			}
		}

		if (Individual.fit > sum_of_errors / training_data->get_num_rows()) {
			Individual.fit = sum_of_errors / training_data->get_num_rows();
			Individual.best = i;
			Individual.best_class_threshold = best_threshold;
		}
	}

	if (line_of_constants)
		delete[] line_of_constants;
}
//---------------------------------------------------------------------------
bool t_mep::compute_regression_error_on_double_data(t_mep_chromosome &individual, double **inputs, int num_data, double ** data, double *error)
{
	*error = 0;
	double actual_output_double[1];
	int num_valid = 0;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(individual, inputs[k], actual_output_double)) {
			*error += fabs(data[k][target_col] - actual_output_double[0]);
			num_valid++;
		}
	}
	if (num_valid)
		*error /= num_valid;
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep::compute_classification_error_on_double_data(t_mep_chromosome &individual, double **inputs, int num_data, double ** data, double *error)
{
	(*error) = 0;
	double actual_output_double[1];

	//	int num_valid = 0;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(individual, inputs[k], actual_output_double)) {
			if (actual_output_double[0] <= individual.best_class_threshold)
				(*error) += data[k][target_col];
			else
				(*error) += 1 - data[k][target_col];
		}
		else
			(*error)++;
	}
	(*error) /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep::compute_regression_error_on_double_data_return_error(t_mep_chromosome &individual, double **inputs, int num_data, double ** data, double *error)
{
	*error = 0;
	double actual_output_double[1];
	int num_valid = 0;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(individual, inputs[k], actual_output_double)) {
			(*error) += fabs(data[k][target_col] - actual_output_double[0]);
			num_valid++;
		}
		else
			return false;
	}

	(*error) /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep::compute_classification_error_on_double_data_return_error(t_mep_chromosome &individual, double **inputs, int num_data, double ** data, double *error)
{
	(*error) = 0;
	double actual_output_double[1];

	//	int num_valid = 0;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(individual, inputs[k], actual_output_double)) {
			if (actual_output_double[0] <= individual.best_class_threshold)
				(*error) += data[k][target_col];
			else
				(*error) += 1 - data[k][target_col];
		}
		else
			return false;
	}
	(*error) /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep::get_output(int run_index, double *inputs, double *outputs)
{
	if (run_index > -1) {
		if (!evaluate_double(stats[run_index].prg, inputs, outputs))
			return false;
		if (parameters.problem_type == PROBLEM_CLASSIFICATION) {
			if (outputs[0] <= stats[run_index].prg.best_class_threshold)
				outputs[0] = 0;
			else
				outputs[0] = 1;
		}
	}
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep::get_error_double(t_mep_chromosome &Individual, double *inputs, double *outputs)
{
	if (!evaluate_double(Individual, inputs, outputs))
		return false;
	if (parameters.problem_type == PROBLEM_CLASSIFICATION) {
		if (outputs[0] <= Individual.best_class_threshold)
			outputs[0] = 0;
		else
			outputs[0] = 1;
	}

	return true;
}
//---------------------------------------------------------------------------
bool t_mep::evaluate_double(t_mep_chromosome &Individual, double *inputs, double *outputs)
{
	bool is_error_case;  // division by zero, other errors

	double *eval_vect = new double[Individual.best + 1];

	for (int i = 0; i <= Individual.best; i++)   // read the t_mep_chromosome from top to down
	{
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		is_error_case = false;
		switch (Individual.prg[i].op) {
		case  O_ADDITION:  // +
			eval_vect[i] = eval_vect[Individual.prg[i].adr1] + eval_vect[Individual.prg[i].adr2];
			break;
		case  O_SUBTRACTION:  // -
			eval_vect[i] = eval_vect[Individual.prg[i].adr1] - eval_vect[Individual.prg[i].adr2];
			break;
		case  O_MULTIPLICATION:  // *
			eval_vect[i] = eval_vect[Individual.prg[i].adr1] * eval_vect[Individual.prg[i].adr2];
			break;
		case  O_DIVISION:  //  /
			if (fabs(eval_vect[Individual.prg[i].adr2]) < DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = eval_vect[Individual.prg[i].adr1] / eval_vect[Individual.prg[i].adr2];
			break;
		case O_POWER:
			eval_vect[i] = pow(eval_vect[Individual.prg[i].adr1], eval_vect[Individual.prg[i].adr2]);
			break;
		case O_SQRT:
			if (eval_vect[Individual.prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = sqrt(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_EXP:
			eval_vect[i] = exp(eval_vect[Individual.prg[i].adr1]);

			break;
		case O_POW10:
			eval_vect[i] = pow(10, eval_vect[Individual.prg[i].adr1]);
			break;
		case O_LN:
			if (eval_vect[Individual.prg[i].adr1] <= 0)
				is_error_case = true;
			else                // an exception occured !!!
				eval_vect[i] = log(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_LOG10:
			if (eval_vect[Individual.prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log10(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_lOG2:
			if (eval_vect[Individual.prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log2(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_FLOOR:
			eval_vect[i] = floor(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_CEIL:
			eval_vect[i] = ceil(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_ABS:
			eval_vect[i] = fabs(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_INV:
			eval_vect[i] = -eval_vect[Individual.prg[i].adr1];
			break;
		case O_X2:
			eval_vect[i] = eval_vect[Individual.prg[i].adr1] * eval_vect[Individual.prg[i].adr1];
			break;
		case O_MIN:
			eval_vect[i] = eval_vect[Individual.prg[i].adr1] < eval_vect[Individual.prg[i].adr2] ? eval_vect[Individual.prg[i].adr1] : eval_vect[Individual.prg[i].adr2];
			break;
		case O_MAX:
			eval_vect[i] = eval_vect[Individual.prg[i].adr1] > eval_vect[Individual.prg[i].adr2] ? eval_vect[Individual.prg[i].adr1] : eval_vect[Individual.prg[i].adr2];
			break;

		case O_SIN:
			eval_vect[i] = sin(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_COS:
			eval_vect[i] = cos(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_TAN:
			eval_vect[i] = tan(eval_vect[Individual.prg[i].adr1]);
			break;

		case O_ASIN:
			eval_vect[i] = asin(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_ACOS:
			eval_vect[i] = acos(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_ATAN:
			eval_vect[i] = atan(eval_vect[Individual.prg[i].adr1]);
			break;
		case O_IFLZ:
			eval_vect[i] = eval_vect[Individual.prg[i].adr1] < 0 ? eval_vect[Individual.prg[i].adr2] : eval_vect[Individual.prg[i].adr3];
			break;
		case O_IFALBCD:
			eval_vect[i] = eval_vect[Individual.prg[i].adr1] < eval_vect[Individual.prg[i].adr2] ? eval_vect[Individual.prg[i].adr3] : eval_vect[Individual.prg[i].adr4];
			break;

		default:  // a variable
			if (Individual.prg[i].op < Individual.num_total_variables)
				eval_vect[i] = inputs[Individual.prg[i].op];
			else
				eval_vect[i] = Individual.constants_double[Individual.prg[i].op - Individual.num_total_variables];
			break;
		}
		if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
			delete[] eval_vect;
			return false;
		}
	}
	outputs[0] = eval_vect[Individual.best];
	delete[] eval_vect;

	return true;

}
//---------------------------------------------------------------------------
void t_mep::compute_cached_eval_matrix_double(void)
{
	/*
	if (parameters.problem_type == PROBLEM_REGRESSION)
	for (int v = 0; v < training_data.num_vars; v++) {
	cached_sum_of_errors[v] = 0;
	for (int k = 0; k < training_data->get_num_rows(); k++) {
	cached_eval_matrix_double[v][k] = training_data->_data_double[k][v];
	cached_sum_of_errors[v] += fabs(cached_eval_matrix_double[v][k] - training_data._target_double[k]);
	}
	}
	else
	for (int v = 0; v < training_data.num_vars; v++) {
	cached_sum_of_errors[v] = 0;
	for (int k = 0; k < training_data->get_num_rows(); k++) {
	cached_eval_matrix_double[v][k] = training_data->_data_double[k][v];
	if (cached_eval_matrix_double[v][k] <= parameters.classification_threshold)
	cached_sum_of_errors[v] += training_data._target_double[k];
	else
	cached_sum_of_errors[v] += 1 - training_data._target_double[k];
	}
	}
	*/
}
//---------------------------------------------------------------------------
void t_mep::compute_cached_eval_matrix_double2(s_value_class *array_value_class)
{
	double **data = training_data->get_data_matrix_double();

	if (parameters.problem_type == PROBLEM_REGRESSION)
		for (int v = 0; v < num_actual_variables; v++) {
			cached_sum_of_errors[actual_enabled_variables[v]] = 0;
			for (int k = 0; k < training_data->get_num_rows(); k++) {
				cached_eval_matrix_double[actual_enabled_variables[v]][k] = data[k][actual_enabled_variables[v]];
				cached_sum_of_errors[actual_enabled_variables[v]] += fabs(cached_eval_matrix_double[actual_enabled_variables[v]][k] - data[k][num_total_variables]);
			}
		}
	else
		for (int v = 0; v < num_actual_variables; v++) {

			cached_threashold[actual_enabled_variables[v]] = 0;
			for (int k = 0; k < training_data->get_num_rows(); k++) {
				cached_eval_matrix_double[actual_enabled_variables[v]][k] = data[k][actual_enabled_variables[v]];
				array_value_class[k].value = data[k][actual_enabled_variables[v]];
				array_value_class[k].data_class = (int)data[k][num_total_variables];
			}
			qsort((void*)array_value_class, training_data->get_num_rows(), sizeof(s_value_class), sort_function_value_class);

			int num_0_incorrect = training_data->get_num_items_class_0();
			int num_1_incorrect = 0;
			cached_threashold[actual_enabled_variables[v]] = array_value_class[0].value - 1;// all are classified to class 1 in this case
			cached_sum_of_errors[actual_enabled_variables[v]] = num_0_incorrect;

			for (int i = 0; i < training_data->get_num_rows(); i++) {
				int j = i + 1;
				//while (j < training_data->get_num_rows() && fabs(cached_eval_matrix_double[actual_enabled_variables[v]][i] - cached_eval_matrix_double[actual_enabled_variables[v]][j]) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
				while (j < training_data->get_num_rows() && fabs(array_value_class[i].value - array_value_class[j].value) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
					j++;

				// le verific pe toate intre i si j si le cataloghez ca apartinant la clasa 0
				for (int k = i; k < j; k++)
					if (array_value_class[k].data_class == 0)
						num_0_incorrect--;
					else
						if (array_value_class[k].data_class == 1) {
							//	num_0_incorrect--;
							num_1_incorrect++;
						}
				if (num_0_incorrect + num_1_incorrect < cached_sum_of_errors[actual_enabled_variables[v]]) {
					cached_sum_of_errors[actual_enabled_variables[v]] = num_0_incorrect + num_1_incorrect;
					cached_threashold[actual_enabled_variables[v]] = array_value_class[i].value;
				}
				i = j;
				i--;
			}
		}
}
//---------------------------------------------------------------------------
void t_mep::compute_eval_matrix_double(t_mep_chromosome &Individual, double **eval_double, int *line_of_constants)
{
	//	bool is_error_case;  // division by zero, other errors

	for (int i = 0; i < parameters.code_length; i++)   // read the t_mep_chromosome from top to down
	{
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		double *arg1, *arg2, *arg3, *arg4;
		double *eval = eval_double[i];
		int num_training_data = training_data->get_num_rows();

		if (Individual.prg[i].op < 0) {// an operator
			if (Individual.prg[Individual.prg[i].adr1].op >= 0)
				if (Individual.prg[Individual.prg[i].adr1].op < num_total_variables)
					arg1 = cached_eval_matrix_double[Individual.prg[Individual.prg[i].adr1].op];
				else
					arg1 = eval_double[line_of_constants[Individual.prg[Individual.prg[i].adr1].op - num_total_variables]];
			else
				arg1 = eval_double[Individual.prg[i].adr1];

			if (Individual.prg[Individual.prg[i].adr2].op >= 0)
				if (Individual.prg[Individual.prg[i].adr2].op < num_total_variables)
					arg2 = cached_eval_matrix_double[Individual.prg[Individual.prg[i].adr2].op];
				else
					arg2 = eval_double[line_of_constants[Individual.prg[Individual.prg[i].adr2].op - num_total_variables]];
			else
				arg2 = eval_double[Individual.prg[i].adr2];

			if (Individual.prg[Individual.prg[i].adr3].op >= 0)
				if (Individual.prg[Individual.prg[i].adr3].op < num_total_variables)
					arg3 = cached_eval_matrix_double[Individual.prg[Individual.prg[i].adr3].op];
				else
					arg3 = eval_double[line_of_constants[Individual.prg[Individual.prg[i].adr3].op - num_total_variables]];
			else
				arg3 = eval_double[Individual.prg[i].adr3];

			if (Individual.prg[Individual.prg[i].adr4].op >= 0)
				if (Individual.prg[Individual.prg[i].adr4].op < num_total_variables)
					arg4 = cached_eval_matrix_double[Individual.prg[Individual.prg[i].adr4].op];
				else
					arg4 = eval_double[line_of_constants[Individual.prg[Individual.prg[i].adr4].op - num_total_variables]];
			else
				arg4 = eval_double[Individual.prg[i].adr4];
		}

		switch (Individual.prg[i].op) {
		case  O_ADDITION:  // +
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] + arg2[k];
			break;
		case  O_SUBTRACTION:  // -
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] - arg2[k];
			break;
		case  O_MULTIPLICATION:  // *
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] * arg2[k];
			break;
		case  O_DIVISION:  //  /
			for (int k = 0; k < num_training_data; k++)
				if (fabs(arg2[k]) < DIVISION_PROTECT) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else
					eval[k] = arg1[k] / arg2[k];
			break;
		case O_POWER:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = pow(arg1[k], arg2[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_SQRT:
			for (int k = 0; k < num_training_data; k++) {
				if (arg1[k] <= 0) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = sqrt(arg1[k]);
			}
			break;
		case O_EXP:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = exp(arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
			}

			break;
		case O_POW10:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = pow(10, arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_LN:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = log(arg1[k]);

			break;
		case O_LOG10:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = log10(arg1[k]);

			break;
		case O_lOG2:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = log2(arg1[k]);
			break;
		case O_FLOOR:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = floor(arg1[k]);
			break;
		case O_CEIL:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = ceil(arg1[k]);
			break;
		case O_ABS:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = fabs(arg1[k]);
			break;
		case O_INV:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = -arg1[k];
			break;
		case O_X2:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] * arg1[k];
			break;
		case O_MIN:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < arg2[k] ? arg1[k] : arg2[k];
			break;
		case O_MAX:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] > arg2[k] ? arg1[k] : arg2[k];
			break;
		case O_SIN:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = sin(arg1[k]);
			break;
		case O_COS:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = cos(arg1[k]);
			break;
		case O_TAN:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = tan(arg1[k]);
			break;

		case O_ASIN:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = asin(arg1[k]);

			break;
		case O_ACOS:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					Individual.prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = acos(arg1[k]);

			break;
		case O_ATAN:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = atan(arg1[k]);
			}
			break;
		case O_IFLZ:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < 0 ? arg2[k] : arg3[k];
			break;
		case O_IFALBCD:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < arg2[k] ? arg3[k] : arg4[k];
			break;

		default:  // a constant
			if (Individual.prg[i].op >= Individual.num_total_variables)
				if (line_of_constants[Individual.prg[i].op - Individual.num_total_variables] == -1) {
					line_of_constants[Individual.prg[i].op - Individual.num_total_variables] = i;
					int constant_index = Individual.prg[i].op - Individual.num_total_variables;
					for (int k = 0; k < num_training_data; k++)
						eval[k] = Individual.constants_double[constant_index];
				}

			break;
		}

	}
}
//---------------------------------------------------------------------------
void t_mep::compute_eval_vector_double(t_mep_chromosome &)
{
}
//---------------------------------------------------------------------------
int sort_function_chromosomes(const void *a, const void *b)
{
	return ((t_mep_chromosome *)a)->compare((t_mep_chromosome *)b, false);
}
//---------------------------------------------------------------------------
void t_mep::sort_by_fitness(t_sub_population &pop) // sort ascending the individuals in population
{
	qsort((void *)pop.individuals, parameters.subpopulation_size, sizeof(pop.individuals[0]), sort_function_chromosomes);
}
//---------------------------------------------------------------------------
void t_mep::delete_sub_population(t_sub_population &pop)
{
	if (pop.individuals)
		delete[] pop.individuals;

	pop.offspring1.clear();
	pop.offspring2.clear();
}
//---------------------------------------------------------------------------
void t_mep::delete_values(double ****eval_double, s_value_class ***array_value_class)
{
	if (*eval_double) {
		for (int c = 0; c < parameters.num_threads; c++) {
			for (int i = 0; i < parameters.code_length; i++)
				delete[](*eval_double)[c][i];
			delete[](*eval_double)[c];
		}
		delete[] * eval_double;
		(*eval_double) = NULL;
	}
	if (cached_eval_matrix_double) {
		for (int i = 0; i < num_total_variables; i++)
			delete[] cached_eval_matrix_double[i];
		delete[] cached_eval_matrix_double;
		cached_eval_matrix_double = NULL;
	}

	if (cached_sum_of_errors) {
		delete[] cached_sum_of_errors;
		cached_sum_of_errors = NULL;
	}

	if (cached_threashold) {
		delete[] cached_threashold;
		cached_threashold = NULL;
	}

	if (*array_value_class) {
		for (int c = 0; c < parameters.num_threads; c++)
			delete[](*array_value_class)[c];
		delete[] * array_value_class;
		*array_value_class = NULL;
	}
}
//---------------------------------------------------------------------------
long t_mep::tournament(t_sub_population &pop)     // Size is the size of the tournament
{
	long r, p;
	p = my_rand() % parameters.subpopulation_size;
	for (int i = 1; i < parameters.tournament_size; i++) {
		r = my_rand() % parameters.subpopulation_size;
		p = pop.individuals[r].fit < pop.individuals[p].fit ? r : p;
	}
	return p;
}
//---------------------------------------------------------------------------
void t_mep::compute_best_and_average_error(double &best_error, double &mean_error)
{
	mean_error = 0;
	best_error = pop[0].individuals[0].fit;
	best_individual_index = 0;
	best_subpopulation_index = 0;
	for (int i = 0; i < parameters.num_subpopulations; i++)
		if (best_error > pop[i].individuals[0].fit) {
			best_error = pop[i].individuals[0].fit;
			best_individual_index = 0;
			best_subpopulation_index = i;
		}

	for (int i = 0; i < parameters.num_subpopulations; i++)
		for (int k = 0; k < parameters.subpopulation_size; k++)
			mean_error += pop[i].individuals[k].fit;

	mean_error /= parameters.num_subpopulations * parameters.subpopulation_size;
}
//---------------------------------------------------------------------------
double t_mep::compute_validation_error(int *best_subpopulation_index_for_validation, int *best_individual_index_for_validation, double **eval_double)
{
	double best_validation_error = -1;
	double validation_error;

	if (parameters.problem_type == PROBLEM_REGRESSION) {
		for (int k = 0; k < parameters.num_subpopulations; k++) {
			while (!compute_regression_error_on_double_data_return_error(pop[k].individuals[0], validation_data->get_data_matrix_double(), validation_data->get_num_rows(), validation_data->get_data_matrix_double(), &validation_error)) {
				// I have to mutate that individual.
				pop[k].individuals[0].prg[pop[k].individuals[0].best].op = actual_enabled_variables[my_rand() % num_actual_variables];
				// recompute its fitness on training;
				fitness_regression(pop[k].individuals[0], eval_double);
				// resort the population
				sort_by_fitness(pop[k]);
				// apply it again on validation
			}
			// now it is ok; no errors on
			if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
				best_validation_error = validation_error;
				*best_subpopulation_index_for_validation = k;
				*best_individual_index_for_validation = 0;
			}
		}
	}
	else
		if (parameters.problem_type == PROBLEM_CLASSIFICATION)
			for (int k = 0; k < parameters.num_subpopulations; k++) {
				while (!compute_classification_error_on_double_data_return_error(pop[k].individuals[0], validation_data->get_data_matrix_double(), validation_data->get_num_rows(), validation_data->get_data_matrix_double(), &validation_error)) {
					pop[k].individuals[0].prg[pop[k].individuals[0].best].op = actual_enabled_variables[my_rand() % num_actual_variables];
					// recompute its fitness on training;
					fitness_regression(pop[k].individuals[0], eval_double);
					// resort the population
					sort_by_fitness(pop[k]);
				}
				if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
					best_validation_error = validation_error;
					*best_subpopulation_index_for_validation = k;
					*best_individual_index_for_validation = 0;
				}
			}

	return best_validation_error;
}
//---------------------------------------------------------------------------
int t_mep::start(f_on_progress on_generation, f_on_progress on_new_evaluation, f_on_progress on_complete_run)
{
	_stopped = false;

	compute_list_of_enabled_variables();

	pop = new t_sub_population[parameters.num_subpopulations];
	for (int i = 0; i < parameters.num_subpopulations; i++)
		allocate_sub_population(pop[i]);

	num_operators = operators.get_list_of_operators(actual_operators);

	double ***eval_double;           // an array where the values of each expression are stored

	s_value_class **array_value_class;

	allocate_values(&eval_double, &array_value_class);

	if (parameters.problem_type == PROBLEM_CLASSIFICATION)
		training_data->count_0_class(target_col);

	compute_cached_eval_matrix_double2(array_value_class[0]);

	stats = new t_mep_statistics[parameters.num_runs];
	for (int run_index = 0; run_index < parameters.num_runs; run_index++) {
		stats[run_index].allocate(parameters.num_generations);
		last_run_index++;
		start_steady_state(run_index, eval_double, array_value_class, on_generation, on_new_evaluation);
		if (on_complete_run)
			on_complete_run();
		if (_stopped)
			break;
	}

	delete_values(&eval_double, &array_value_class);
	for (int i = 0; i < parameters.num_subpopulations; i++)
		delete_sub_population(pop[i]);
	delete[] pop;

	_stopped = true;
	return true;
}
//---------------------------------------------------------------------------
void t_mep::evolve_one_subpopulation_for_one_generation(int *current_subpop_index, std::mutex* mutex, t_sub_population * sub_populations, int generation_index, double ** eval_double, s_value_class *tmp_value_class)
{
	int pop_index = 0;
	while (*current_subpop_index < parameters.num_subpopulations) {
		// still more subpopulations to evolve?

		while (!mutex->try_lock()) {}// create a lock so that multiple threads will not evolve the same sub population
		pop_index = *current_subpop_index;
		(*current_subpop_index)++;
		mutex->unlock();

		// pop_index is the index of the subpopulation evolved by the current thread

		t_sub_population *a_sub_population = &sub_populations[pop_index];

		if (generation_index == 0) {
			if (parameters.problem_type == PROBLEM_REGRESSION)
				for (int i = 0; i < parameters.subpopulation_size; i++)
					fitness_regression(pop[pop_index].individuals[i], eval_double);
			else
				if (parameters.problem_type == PROBLEM_CLASSIFICATION)
					for (int i = 0; i < parameters.subpopulation_size; i++)
						fitness_classification(pop[pop_index].individuals[i], eval_double, tmp_value_class);

			sort_by_fitness(pop[pop_index]);
		}
		else // other generations, after initial
			for (int k = 0; k < parameters.subpopulation_size; k += 2) {
				// choose the parents using binary tournament
				long r1 = tournament(*a_sub_population);
				long r2 = tournament(*a_sub_population);
				// crossover
				double p = my_rand() / (RAND_MAX + 1.0);
				if (p < parameters.crossover_probability)
					if (parameters.crossover_type == UNIFORM_CROSSOVER)
						a_sub_population->individuals[r1].uniform_crossover(a_sub_population->individuals[r2], a_sub_population->offspring1, a_sub_population->offspring2, &parameters);
					else
						a_sub_population->individuals[r1].one_cut_point_crossover(a_sub_population->individuals[r2], a_sub_population->offspring1, a_sub_population->offspring2, &parameters);
				else {
					a_sub_population->offspring1 = a_sub_population->individuals[r1];
					a_sub_population->offspring2 = a_sub_population->individuals[r2];
				}
				// mutate the result and move the mutant in the new population
				a_sub_population->offspring1.mutation(&parameters, actual_operators, num_operators, actual_enabled_variables, num_actual_variables);
				if (parameters.problem_type == PROBLEM_REGRESSION)
					fitness_regression(a_sub_population->offspring1, eval_double);
				else
					fitness_classification(a_sub_population->offspring1, eval_double, tmp_value_class);

				a_sub_population->offspring2.mutation(&parameters, actual_operators, num_operators, actual_enabled_variables, num_actual_variables);
				if (parameters.problem_type == PROBLEM_REGRESSION)
					fitness_regression(a_sub_population->offspring2, eval_double);
				else
					fitness_classification(a_sub_population->offspring2, eval_double, tmp_value_class);

				if (a_sub_population->offspring1.fit < a_sub_population->offspring2.fit)   // the best offspring replaces the worst individual in the population
					if (a_sub_population->offspring1.fit < a_sub_population->individuals[parameters.subpopulation_size - 1].fit) {
						a_sub_population->individuals[parameters.subpopulation_size - 1] = a_sub_population->offspring1;
						sort_by_fitness(*a_sub_population);
					}
					else;
				else if (a_sub_population->offspring2.fit < a_sub_population->individuals[parameters.subpopulation_size - 1].fit) {
					a_sub_population->individuals[parameters.subpopulation_size - 1] = a_sub_population->offspring2;
					sort_by_fitness(*a_sub_population);
				}
			}

	}
}
//-----------------------------------------------------------------------
bool t_mep::start_steady_state(int run, double ***eval_double, s_value_class **array_value_class, f_on_progress on_generation, f_on_progress on_new_evaluation)       // Steady-State MEP
{
	my_srand(run + parameters.random_seed);

	//clock_t start_time = clock();
	time_t start_time;
	time(&start_time);

	generate_random_individuals();

	//wxLogDebug(wxString() << "generation " << gen_index << " ");
	// an array of threads. Each sub population is evolved by a thread
	std::thread **mep_threads = new std::thread*[parameters.num_threads];
	// we create a fixed number of threads and each thread will take and evolve one subpopulation, then it will take another one
	std::mutex mutex;
	// we need a mutex to make sure that the same subpopulation will not be evolved twice by different threads

	// initial population (generation 0)
	int current_subpop_index = 0;
	for (int t = 0; t < parameters.num_threads; t++)
		mep_threads[t] = new std::thread(&t_mep::evolve_one_subpopulation_for_one_generation, this, &current_subpop_index, &mutex, pop, 0, eval_double[t], array_value_class[t]);


	for (int t = 0; t < parameters.num_threads; t++) {
		mep_threads[t]->join(); // wait for all threads to execute
		delete mep_threads[t];
	}

	// now I have to apply this to the validation set

	stats[run].best_validation_error = -1;

	if (parameters.use_validation_data && validation_data->get_num_rows() > 0) {
		// I must run all solutions for the validation data and choose the best one
		stats[run].best_validation_error = compute_validation_error(&best_subpopulation_index_for_test, &best_individual_index_for_test, eval_double[0]);
		stats[run].prg = pop[best_subpopulation_index_for_test].individuals[best_individual_index_for_test];
	}

	double best_error_on_training, mean_error_on_training;
	compute_best_and_average_error(best_error_on_training, mean_error_on_training);
	stats[run].best_training_error[0] = best_error_on_training;
	stats[run].average_training_error[0] = mean_error_on_training;
	stats[run].last_gen = 0;
	modified_project = true;

	if (on_generation)
		on_generation();

	for (int gen_index = 1; gen_index < parameters.num_generations; gen_index++) {
		if (_stopped)
			break;

		int current_subpop_index = 0;
		for (int t = 0; t < parameters.num_threads; t++)
			mep_threads[t] = new std::thread(&t_mep::evolve_one_subpopulation_for_one_generation, this, &current_subpop_index, &mutex, pop, gen_index, eval_double[t], array_value_class[t]);

		for (int t = 0; t < parameters.num_threads; t++) {
			mep_threads[t]->join(); // wait for all threads to execute
			delete mep_threads[t];
		}

		// now copy the best from each deme to the next one
		for (int d = 0; d < parameters.num_subpopulations; d++) { // din d in d+1
			// aleg unul din dema d
			long w = tournament(pop[d]);
			if (pop[d].individuals[w].compare(&pop[(d + 1) % parameters.num_subpopulations].individuals[parameters.subpopulation_size - 1], false))
				pop[(d + 1) % parameters.num_subpopulations].individuals[parameters.subpopulation_size - 1] = pop[d].individuals[w];
		}
		for (int d = 0; d < parameters.num_subpopulations; d++) // din d in d+1
			sort_by_fitness(pop[d]);

		if (parameters.use_validation_data && validation_data->get_num_rows() > 0) {
			// I must run all solutions for the validation data and choose the best one
			int best_index_on_validation, best_subpop_index_on_validation;
			double validation_error = compute_validation_error(&best_subpop_index_on_validation, &best_index_on_validation, eval_double[0]);
			if ((validation_error < stats[run].best_validation_error) || (fabs(stats[run].best_validation_error + 1) <= 1E-6)) {
				stats[run].best_validation_error = validation_error;
				best_individual_index_for_test = best_index_on_validation;
				best_subpopulation_index_for_test = best_subpop_index_on_validation;
				stats[run].prg = pop[best_subpopulation_index_for_test].individuals[best_individual_index_for_test];
			}
		}

		compute_best_and_average_error(best_error_on_training, mean_error_on_training);
		stats[run].best_training_error[gen_index] = best_error_on_training;
		stats[run].average_training_error[gen_index] = mean_error_on_training;
		stats[run].last_gen = gen_index;
#ifdef _DEBUG
		//	wxLogDebug(wxString() << gen_index << " " << best_error_on_training << " " << pop[best_subpopulation_index].individuals[best_individual_index].best_class_threshold);
#endif
		if (on_generation)
			on_generation();
	}

	// DEBUG ONLY
	//	fitness_regression(pop[best_individual_index]);
	/*
	if (parameters.problem_type == PROBLEM_REGRESSION)
	fitness_regression(pop[0].individuals[0], eval_double[0]);
	else
	if (parameters.problem_type == PROBLEM_CLASSIFICATION)
	fitness_classification(pop[0].individuals[0], eval_double[0], array_value_class[0]);
	*/
	if (!(parameters.use_validation_data && validation_data->get_num_rows() > 0)) // if no validation data, the test is the best from all
		stats[run].prg = pop[best_subpopulation_index].individuals[best_individual_index];
	stats[run].prg.simplify();

	if (test_data && test_data->get_num_rows() && test_data->get_num_outputs()) {// has target
		// I must run all solutions for the test data and choose the best one
		if (parameters.problem_type == PROBLEM_REGRESSION) {
			if (compute_regression_error_on_double_data(stats[run].prg, test_data->get_data_matrix_double(), test_data->get_num_rows(), test_data->get_data_matrix_double(), &stats[run].test_error));
		}
		else
			if (parameters.problem_type == PROBLEM_CLASSIFICATION) {
				if (compute_classification_error_on_double_data(stats[run].prg, test_data->get_data_matrix_double(), test_data->get_num_rows(), test_data->get_data_matrix_double(), &stats[run].test_error));
			}
	}


	delete[] mep_threads;

	//clock_t end_time = clock();
	time_t end_time;
	time(&end_time);

	//stats[run].running_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	stats[run].running_time = difftime(end_time, start_time);

	return true;
}
//---------------------------------------------------------------------------
int t_mep::to_pugixml_node(pugi::xml_node parent)
{
	// utilized variables


	pugi::xml_node training_node = parent.append_child("training");
	training_data->to_xml(training_node);
	pugi::xml_node validation_node = parent.append_child("validation");
	validation_data->to_xml(validation_node);
	pugi::xml_node testing_node = parent.append_child("test");
	test_data->to_xml(testing_node);

	if (variables_enabled) {
        
        	char *tmp_str = new char[training_data->get_num_cols() * 2 + 10];
        
		pugi::xml_node utilized_variables_node = parent.append_child("variables_utilization");

		tmp_str[0] = 0;
		for (int v = 0; v < training_data->get_num_cols() - 1; v++) {
			char tmp_s[30];
			sprintf(tmp_s, "%d", variables_enabled[v]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		pugi::xml_node utilized_variables_data_node = utilized_variables_node.append_child(pugi::node_pcdata);
		utilized_variables_data_node.set_value(tmp_str);
        
        delete [] tmp_str;
	}
/*
	pugi::xml_node target_col_node = parent.append_child("target_col");
	pugi::xml_node target_col_data_node = target_col_node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", target_col);
	target_col_data_node.set_value(tmp_str);
*/
	pugi::xml_node parameters_node = parent.append_child("parameters");
	parameters.to_xml(parameters_node);
	pugi::xml_node operators_node = parent.append_child("operators");
	operators.to_xml(operators_node);

	pugi::xml_node results_node = parent.append_child("results");

	for (int r = 0; r <= last_run_index; r++) {
		pugi::xml_node run_node = results_node.append_child("run");
		stats[r].to_xml(run_node);
	}

	return true;
}
//---------------------------------------------------------------------------
int t_mep::from_pugixml_node(pugi::xml_node parent)
{
	//get the utilized variables first

	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}

	training_data->clear_data();

	if (training_data) {
		pugi::xml_node node = parent.child("training");
		if (node) {
			training_data->from_xml(node);
			num_total_variables = training_data->get_num_cols() - 1;
		}
		else
			num_total_variables = 0;
	}
	else
		num_total_variables = 0;
	

	if (training_data->get_num_rows()) {
		//actual_enabled_variables = new int[num_total_variables];
		variables_enabled = new bool[num_total_variables];

		pugi::xml_node node = parent.child("variables_utilization");
		if (node) {
			const char *value_as_cstring = node.child_value();
			int num_jumped_chars = 0;
			//num_actual_variables = 0;
			int i = 0;

			while (*(value_as_cstring + num_jumped_chars)) {
                int int_read;
				sscanf(value_as_cstring + num_jumped_chars, "%d", &int_read);
                variables_enabled[i] = int_read;
				/*
				if (variables_utilization[i]) {
				actual_enabled_variables[num_actual_variables] = i;
				num_actual_variables++;
				}
				*/
				long local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
				i++;
			}
		}
		else {// not found, use everything
			//num_actual_variables = num_total_variables;
			for (int i = 0; i < num_total_variables; i++) {
				variables_enabled[i] = 1;
				//actual_enabled_variables[i] = i;
			}
		}
	}

	pugi::xml_node node = parent.child("target_col");
	if (node) {
		const char *value_as_cstring = node.child_value();
		target_col = atoi(value_as_cstring);
	}
	else
		target_col = training_data->get_num_cols() - 1;

	validation_data->clear_data();

	if (validation_data) {
		node = parent.child("validation");
		if (node)
			validation_data->from_xml(node);
	}

	test_data->clear_data();

	if (test_data) {
		node = parent.child("test");
		if (node) {
			test_data->from_xml(node);
			if (test_data->get_num_cols() < training_data->get_num_cols())
				test_data->set_num_outputs(0);
		}
	}

	node = parent.child("parameters");
	if (node)
		parameters.from_xml(node);

	node = parent.child("operators");
	if (node)
		operators.from_xml(node);

	last_run_index = -1;
	pugi::xml_node node_results = parent.child("results");
	if (node_results)
		for (pugi::xml_node row = node_results.child("run"); row; row = row.next_sibling("run"), last_run_index++);

	if (last_run_index > -1) {
		stats = new t_mep_statistics[last_run_index + 1];
		last_run_index = 0;
		for (pugi::xml_node row = node_results.child("run"); row; row = row.next_sibling("run"), last_run_index++)
			stats[last_run_index].from_xml(row, parameters.num_generations, parameters.code_length);
		last_run_index--;
	}

	modified_project = false;

	return true;
}
//---------------------------------------------------------------------------
int t_mep::to_xml(const char *filename)
{
	pugi::xml_document doc;
	// add node with some name
	pugi::xml_node body = doc.append_child("project");

	pugi::xml_node problem_description_node = body.append_child("problem_description");
	pugi::xml_node data = problem_description_node.append_child(pugi::node_pcdata);
	data.set_value(problem_description);

	pugi::xml_node version_node = body.append_child("version");
	data = version_node.append_child(pugi::node_pcdata);
	data.set_value(version);

	pugi::xml_node alg_node = body.append_child("algorithm");
	to_pugixml_node(alg_node);

	modified_project = false;

#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	int result = doc.save_file(w_filename);
	delete[] w_filename;
	return result;
#else
	return doc.save_file(filename);
#endif

}
//-----------------------------------------------------------------
int t_mep::from_xml(const char* filename)
{
	pugi::xml_document doc;

	pugi::xml_parse_result result;

#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	result = doc.load_file(w_filename);
	delete[] w_filename;
#else
	result = doc.load_file(filename);
#endif

	if (result.status != pugi::status_ok)
		return false;

	pugi::xml_node body_node = doc.child("project");

	if (!body_node)
		return false;

	if (problem_description) {
		delete[] problem_description;
		problem_description = NULL;
	}
	pugi::xml_node node = body_node.child("problem_description");
	if (node) {
		const char *value_as_cstring = node.child_value();

		if (strlen(value_as_cstring)) {
			problem_description = new char[strlen(value_as_cstring) + 1];
			strcpy(problem_description, value_as_cstring);
		}
	}
	else {
		problem_description = new char[100];
		strcpy(problem_description, "Problem description here ...");
	}


	pugi::xml_node alg_node = body_node.child("algorithm");

	if (!alg_node)
		return false;

	from_pugixml_node(alg_node);

	return true;
}
//-----------------------------------------------------------------

double t_mep::get_best_training_error(int run, int gen)
{
	if (stats[run].best_training_error)
		if (gen <= stats[run].last_gen)
			return stats[run].best_training_error[gen];
		else
			return stats[run].best_training_error[stats[run].last_gen];
	else
		return -1;
}
//---------------------------------------------------------------------------
double t_mep::get_best_validation_error(int run)
{
	return stats[run].best_validation_error;
}
//---------------------------------------------------------------------------
double t_mep::get_test_error(int run)
{
	return stats[run].test_error;
}
//---------------------------------------------------------------------------
double t_mep::get_average_training_error(int run, int gen)
{
	if (stats[run].average_training_error)
		if (gen <= stats[run].last_gen)
			return stats[run].average_training_error[gen];
		else
			return stats[run].average_training_error[stats[run].last_gen];
	else
		return -1;
}
//---------------------------------------------------------------------------
int t_mep::get_latest_generation(int run)
{
	return stats[run].last_gen;
}
//---------------------------------------------------------------------------
double t_mep::get_running_time(int run)
{
	return stats[run].running_time;
}
//---------------------------------------------------------------------------
void t_mep::stop(void)
{
	_stopped = true;
}
//---------------------------------------------------------------------------
int t_mep::stats_to_csv(const char *filename)
{
	FILE *f = NULL;
#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"w");
	delete[] w_filename;

#else
	f = fopen(filename, "w");
#endif

	if (!f)
		return false;
	fprintf(f, "running time; training error; validation error\n");
	for (int r = 0; r <= last_run_index; r++)
		fprintf(f, "%lf;%lf;%lf\n", stats[r].running_time, stats[r].best_training_error[parameters.num_generations - 1], stats[r].best_validation_error);

	fclose(f);
	return true;
}
//---------------------------------------------------------------------------
int running_time_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->running_time < ((t_mep_statistics*)b)->running_time)
		return -1;
	else
		if (((t_mep_statistics*)a)->running_time > ((t_mep_statistics*)b)->running_time)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int running_time_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->running_time < ((t_mep_statistics*)b)->running_time)
		return 1;
	else
		if (((t_mep_statistics*)a)->running_time > ((t_mep_statistics*)b)->running_time)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep::sort_stats_by_running_time(bool ascending)
{
	if (ascending)
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), running_time_comparator_ascending);
	else
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), running_time_comparator_descending);
}
//---------------------------------------------------------------------------
int training_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->best_training_error[((t_mep_statistics*)a)->last_gen] < ((t_mep_statistics*)b)->best_training_error[((t_mep_statistics*)b)->last_gen])
		return -1;
	else
		if (((t_mep_statistics*)a)->best_training_error[((t_mep_statistics*)a)->last_gen] > ((t_mep_statistics*)b)->best_training_error[((t_mep_statistics*)b)->last_gen])
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int training_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->best_training_error[((t_mep_statistics*)a)->last_gen] < ((t_mep_statistics*)b)->best_training_error[((t_mep_statistics*)b)->last_gen])
		return 1;
	else
		if (((t_mep_statistics*)a)->best_training_error[((t_mep_statistics*)a)->last_gen] > ((t_mep_statistics*)b)->best_training_error[((t_mep_statistics*)b)->last_gen])
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep::sort_stats_by_training_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), training_error_comparator_ascending);
	else
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), training_error_comparator_descending);

}
//---------------------------------------------------------------------------
int validation_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->best_validation_error < ((t_mep_statistics*)b)->best_validation_error)
		return -1;
	else
		if (((t_mep_statistics*)a)->best_validation_error > ((t_mep_statistics*)b)->best_validation_error)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int validation_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->best_validation_error < ((t_mep_statistics*)b)->best_validation_error)
		return 1;
	else
		if (((t_mep_statistics*)a)->best_validation_error > ((t_mep_statistics*)b)->best_validation_error)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep::sort_stats_by_validation_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), validation_error_comparator_ascending);
	else
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), validation_error_comparator_descending);

}
//---------------------------------------------------------------------------
int test_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->test_error < ((t_mep_statistics*)b)->test_error)
		return -1;
	else
		if (((t_mep_statistics*)a)->test_error > ((t_mep_statistics*)b)->test_error)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int test_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->test_error < ((t_mep_statistics*)b)->test_error)
		return 1;
	else
		if (((t_mep_statistics*)a)->test_error > ((t_mep_statistics*)b)->test_error)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep::sort_stats_by_test_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), test_error_comparator_ascending);
	else
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), test_error_comparator_descending);
}
//---------------------------------------------------------------------------
void t_mep::compute_list_of_enabled_variables(void)
{
	if (actual_enabled_variables) {
		delete[](actual_enabled_variables);
		(actual_enabled_variables) = NULL;
	}

	num_actual_variables = 0;
	if (training_data->get_num_cols())
		num_total_variables = training_data->get_num_cols() - 1;
	else
		num_total_variables = 0;
	if (num_total_variables) {
		actual_enabled_variables = new int[num_total_variables];
		for (int i = 0; i < num_total_variables; i++)
			if (variables_enabled[i]) {
				actual_enabled_variables[num_actual_variables] = i;
				num_actual_variables++;
			}
	}
}
//---------------------------------------------------------------------------
double t_mep::get_mutation_probability(void)
{
	return parameters.mutation_probability;
}
//---------------------------------------------------------------------------
double t_mep::get_crossover_probability(void)
{
	return parameters.crossover_probability;
}
//---------------------------------------------------------------------------
long t_mep::get_code_length(void)
{
	return parameters.code_length;
}
//---------------------------------------------------------------------------
long t_mep::get_subpopulation_size(void)
{
	return parameters.subpopulation_size;
}
//---------------------------------------------------------------------------
long t_mep::get_num_threads(void)
{
	return parameters.num_threads;
}
//---------------------------------------------------------------------------
long t_mep::get_tournament_size(void)
{
	return parameters.tournament_size;
}
//---------------------------------------------------------------------------
long t_mep::get_num_generations(void)
{
	return parameters.num_generations;
}
//---------------------------------------------------------------------------
long t_mep::get_problem_type(void)
{
	return parameters.problem_type;
}
//---------------------------------------------------------------------------
long t_mep::get_num_subpopulations(void)
{
	return parameters.num_subpopulations;
}
//---------------------------------------------------------------------------
double t_mep::get_operators_probability(void)
{
	return parameters.operators_probability;
}
//---------------------------------------------------------------------------
double t_mep::get_variables_probability(void)
{
	return parameters.variables_probability;
}
//---------------------------------------------------------------------------
double t_mep::get_constants_probability(void)
{
	return parameters.constants_probability;
}
//---------------------------------------------------------------------------
bool t_mep::get_use_validation_data(void)
{
	return parameters.use_validation_data;
}
//---------------------------------------------------------------------------
int t_mep::get_crossover_type(void)
{
	return parameters.crossover_type;
}
//---------------------------------------------------------------------------
long t_mep::get_random_seed(void)
{
	return parameters.random_seed;
}
//---------------------------------------------------------------------------
long t_mep::get_num_runs(void)
{
	return parameters.num_runs;
}
//---------------------------------------------------------------------------
bool t_mep::get_simplified_programs(void)
{
	return parameters.simplified_programs;
}
//---------------------------------------------------------------------------
void t_mep::set_mutation_probability(double value)
{
	if (_stopped) {
		parameters.mutation_probability = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_crossover_probability(double value)
{
	if (_stopped) {
		parameters.crossover_probability = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_code_length(long value)
{
	if (_stopped) {
		parameters.code_length = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------

void t_mep::set_subpopulation_size(long value)
{
	if (_stopped) {
		parameters.subpopulation_size = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_num_threads(long value)
{
	if (_stopped) {
		parameters.num_threads = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_tournament_size(long value)
{
	if (_stopped) {
		parameters.tournament_size = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_num_generations(long value)
{
	if (_stopped) {
		parameters.num_generations = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_problem_type(long value)
{
	if (_stopped) {
		parameters.problem_type = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_num_subpopulations(long value)
{
	if (_stopped) {
		parameters.num_subpopulations = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_operators_probability(double value)
{
	if (_stopped) {
		parameters.operators_probability = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_variables_probability(double value)
{
	if (_stopped) {
		parameters.variables_probability = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_constants_probability(double value)
{
	if (_stopped) {
		parameters.constants_probability = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_use_validation_data(bool value)
{
	if (_stopped) {
		parameters.use_validation_data = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_crossover_type(int value)
{
	if (_stopped) {
		parameters.crossover_type = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_random_seed(long value)
{
	if (_stopped) {
		parameters.random_seed = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_num_runs(long value)
{
	if (_stopped) {
		parameters.num_runs = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_simplified_programs(bool value)
{
	if (_stopped) {
		parameters.simplified_programs = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
long t_mep::get_num_automatic_constants(void)
{
	return parameters.constants.num_automatic_constants;
}
//---------------------------------------------------------------------------
long t_mep::get_num_user_defined_constants(void)
{
	return parameters.constants.num_user_defined_constants;
}
//---------------------------------------------------------------------------
double t_mep::get_min_constants_interval_double(void)
{
	return parameters.constants.min_constants_interval_double;
}
//---------------------------------------------------------------------------
double t_mep::get_max_constants_interval_double(void)
{
	return parameters.constants.max_constants_interval_double;
}
//---------------------------------------------------------------------------
double t_mep::get_constants_double(long index)
{
	return parameters.constants.constants_double[index];
}
//---------------------------------------------------------------------------
long t_mep::get_constants_type(void)
{
	return parameters.constants.constants_type;
}
//---------------------------------------------------------------------------
bool t_mep::get_constants_can_evolve(void)
{
	return parameters.constants.constants_can_evolve;
}
//---------------------------------------------------------------------------
double t_mep::get_constants_mutation_max_deviation(void)
{
	return parameters.constants.constants_mutation_max_deviation;
}
//---------------------------------------------------------------------------
void t_mep::set_num_automatic_constants(long value)
{
	if (_stopped) {
		parameters.constants.num_automatic_constants = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_num_user_defined_constants(long value)
{
	if (_stopped) {
		if (parameters.constants.constants_double)
			delete[]parameters.constants.constants_double;

		parameters.constants.num_user_defined_constants = value;

		if (parameters.constants.num_user_defined_constants)
			parameters.constants.constants_double = new double[parameters.constants.num_user_defined_constants];
		else
			parameters.constants.constants_double = NULL;

		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_min_constants_interval_double(double value)
{
	if (_stopped) {
		parameters.constants.min_constants_interval_double = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_max_constants_interval_double(double value)
{
	if (_stopped) {
		parameters.constants.max_constants_interval_double = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_constants_double(long index, double value)
{
	if (_stopped) {
		parameters.constants.constants_double[index] = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_constants_type(long value)
{
	if (_stopped) {
		parameters.constants.constants_type = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_constants_can_evolve(bool value)
{
	if (_stopped) {
		parameters.constants.constants_can_evolve = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_constants_mutation_max_deviation(double value)
{
	if (_stopped) {
		parameters.constants.constants_mutation_max_deviation = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::init_parameters(void)
{
	if (_stopped) {
		parameters.init();
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
bool t_mep::get_addition(void)
{
	return operators.use_addition;
}
//---------------------------------------------------------------------------
bool t_mep::get_subtraction(void)
{
	return operators.use_subtraction;
}
//---------------------------------------------------------------------------
bool t_mep::get_multiplication(void)
{
	return operators.use_multiplication;
}
//---------------------------------------------------------------------------
bool t_mep::get_division(void)
{
	return operators.use_division;
}
//---------------------------------------------------------------------------
bool t_mep::get_power(void)
{
	return operators.use_power;
}
//---------------------------------------------------------------------------
bool t_mep::get_sqrt(void)
{
	return operators.use_sqrt;
}
//---------------------------------------------------------------------------
bool t_mep::get_exp(void)
{
	return operators.use_exp;
}
//---------------------------------------------------------------------------
bool t_mep::get_pow10(void)
{
	return operators.use_pow10;
}
//---------------------------------------------------------------------------
bool t_mep::get_ln(void)
{
	return operators.use_ln;
}
//---------------------------------------------------------------------------
bool t_mep::get_log10(void)
{
	return operators.use_log10;
}
//---------------------------------------------------------------------------
bool t_mep::get_log2(void)
{
	return operators.use_log2;
}
//---------------------------------------------------------------------------
bool t_mep::get_floor(void)
{
	return operators.use_floor;
}
//---------------------------------------------------------------------------
bool t_mep::get_ceil(void)
{
	return operators.use_ceil;
}
//---------------------------------------------------------------------------
bool t_mep::get_abs(void)
{
	return operators.use_abs;
}
//---------------------------------------------------------------------------
bool t_mep::get_inv(void)
{
	return operators.use_inv;
}
//---------------------------------------------------------------------------
bool t_mep::get_x2(void)
{
	return operators.use_x2;
}
//---------------------------------------------------------------------------
bool t_mep::get_min(void)
{
	return operators.use_min;
}
//---------------------------------------------------------------------------
bool t_mep::get_max(void)
{
	return operators.use_max;
}
//---------------------------------------------------------------------------
bool t_mep::get_sin(void)
{
	return operators.use_sin;
}
//---------------------------------------------------------------------------
bool t_mep::get_cos(void)
{
	return operators.use_cos;
}
//---------------------------------------------------------------------------
bool t_mep::get_tan(void)
{
	return operators.use_tan;
}
//---------------------------------------------------------------------------
bool t_mep::get_asin(void)
{
	return operators.use_asin;
}
//---------------------------------------------------------------------------
bool t_mep::get_acos(void)
{
	return operators.use_acos;
}
//---------------------------------------------------------------------------
bool t_mep::get_atan(void)
{
	return operators.use_atan;
}
//---------------------------------------------------------------------------
bool t_mep::get_iflz(void)
{
	return operators.use_iflz;
}
//---------------------------------------------------------------------------
bool t_mep::get_ifalbcd(void)
{
	return operators.use_ifalbcd;
}
//---------------------------------------------------------------------------
void t_mep::set_addition(bool value)
{
	if (_stopped) {
		operators.use_addition = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_subtraction(bool value)
{
	if (_stopped) {
		operators.use_subtraction = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_multiplication(bool value)
{
	if (_stopped) {
		operators.use_multiplication = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_division(bool value)
{
	if (_stopped) {
		operators.use_division = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_power(bool value)
{
	if (_stopped) {
		operators.use_power = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_sqrt(bool value)
{
	if (_stopped) {
		operators.use_sqrt = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_exp(bool value)
{
	if (_stopped) {
		operators.use_exp = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_pow10(bool value)
{
	if (_stopped) {
		operators.use_pow10 = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_ln(bool value)
{
	if (_stopped) {
		operators.use_ln = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_log10(bool value)
{
	if (_stopped) {
		operators.use_log10 = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_log2(bool value)
{
	if (_stopped) {
		operators.use_log2 = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_floor(bool value)
{
	if (_stopped) {
		operators.use_floor = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_ceil(bool value)
{
	if (_stopped) {
		operators.use_ceil = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_abs(bool value)
{
	if (_stopped) {
		operators.use_abs = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_inv(bool value)
{
	if (_stopped) {
		operators.use_inv = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_x2(bool value)
{
	if (_stopped) {
		operators.use_x2 = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_min(bool value)
{
	if (_stopped) {
		operators.use_min = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_max(bool value)
{
	if (_stopped) {
		operators.use_max = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_sin(bool value)
{
	if (_stopped) {
		operators.use_sin = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_cos(bool value)
{
	if (_stopped) {
		operators.use_cos = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_tan(bool value)
{
	if (_stopped) {
		operators.use_tan = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_asin(bool value)
{
	if (_stopped) {
		operators.use_asin = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_acos(bool value)
{
	if (_stopped) {
		operators.use_acos = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_atan(bool value)
{
	if (_stopped) {
		operators.use_atan = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_iflz(bool value)
{
	if (_stopped) {
		operators.use_iflz = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_ifalbcd(bool value)
{
	if (_stopped) {
		operators.use_ifalbcd = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::init_operators()
{
	if (_stopped) {
		operators.init();
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
bool t_mep::is_running(void)
{
	return !_stopped;
}
//---------------------------------------------------------------------------
int t_mep::get_last_run_index(void)
{
	return last_run_index;
}
//---------------------------------------------------------------------------
void t_mep::clear_stats(void)
{
	if (_stopped) {
		modified_project = true;

		last_run_index = -1;
		if (stats) {
			delete[] stats;
			stats = NULL;
		}
	}
}
//---------------------------------------------------------------------------
char* t_mep::program_as_C(int run_index, bool simplified, double *inputs)
{
	return stats[run_index].prg.to_C_double(simplified, inputs, parameters.problem_type);
}
//---------------------------------------------------------------------------
int t_mep::get_num_outputs(void)
{
	return 1;
}
//---------------------------------------------------------------------------
int t_mep::get_num_total_variables(void)
{
	return num_total_variables;
}
//---------------------------------------------------------------------------
void t_mep::set_num_total_variables(int value)
{
	if (_stopped) {
		num_total_variables = value;
		target_col = num_total_variables;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::init(void)
{
	if (_stopped) {
		init_parameters();
		init_operators();


		if (actual_enabled_variables) {
			delete[] actual_enabled_variables;
			actual_enabled_variables = NULL;
		}
		if (variables_enabled) {
			delete[] variables_enabled;
			variables_enabled = NULL;
		}

		num_actual_variables = 0;

		modified_project = false;
	}
}
//---------------------------------------------------------------------------
int t_mep::get_num_actual_variables(void)
{
	return num_actual_variables;
}
//---------------------------------------------------------------------------
bool t_mep::is_variable_enabled(int index)
{
	return variables_enabled[index];
}
//---------------------------------------------------------------------------
void t_mep::set_variable_enable(int index, bool new_state)
{
	if (_stopped) {
		variables_enabled[index] = new_state;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
bool t_mep::is_project_modified(void)
{
	return modified_project;
}
//---------------------------------------------------------------------------
void t_mep::set_problem_description(const char* value)
{
	if (_stopped) {

		if (problem_description) {
			delete[] problem_description;
			problem_description = NULL;
		}

		if (strlen(value)) {
			problem_description = new char[strlen(value) + 1];
			strcpy(problem_description, value);
		}

		modified_project = true;
	}
}
//---------------------------------------------------------------------------
char* t_mep::get_problem_description(void)
{
	return problem_description;
}
//---------------------------------------------------------------------------
void t_mep::set_enable_cache_results_for_all_training_data(bool value)
{
	if (_stopped)
	  cache_results_for_all_training_data = value;
}
//---------------------------------------------------------------------------
bool t_mep::get_enable_cache_results_for_all_training_data(void)
{
	return cache_results_for_all_training_data;
}
//---------------------------------------------------------------------------
long long t_mep::get_memory_consumption(void)
{
	// for chromosomes
	long long chromosomes_memory = 0;

	return 0;
}
//---------------------------------------------------------------------------
bool t_mep::validate_project(void)
{
	return true;
}
//---------------------------------------------------------------------------
void t_mep::set_training_data(t_mep_data *_data)
{
	if (_stopped) {
		training_data = _data;

		if (variables_enabled) {
			delete[] variables_enabled;
			variables_enabled = NULL;
		}

		if (actual_enabled_variables) {
			delete[] actual_enabled_variables;
			actual_enabled_variables = NULL;
		}

		if (training_data) {
			num_total_variables = training_data->get_num_cols();
			num_actual_variables = num_total_variables;

			variables_enabled = new bool[num_total_variables];

			for (int i = 0; i < num_total_variables; i++)
				variables_enabled[i] = 1;

			target_col = training_data->get_num_cols() - 1;
		}
	}
}
//---------------------------------------------------------------------------
void t_mep::set_validation_data(t_mep_data *_data)
{
	if (_stopped)
		validation_data = _data;
}
//---------------------------------------------------------------------------
void t_mep::set_test_data(t_mep_data *_data)
{
	if (_stopped)
		test_data = _data;
}
//---------------------------------------------------------------------------