// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "mep_rands.h"
#include "libmep.h"

//---------------------------------------------------------------------------
t_mep::t_mep()
{
	strcpy(version, "2021.12.7.0-beta");

	num_selected_operators = 0;

	cached_eval_variables_matrix_double = NULL;
	cached_sum_of_errors = NULL;

	modified_project = false;
	_stopped = true;
	_stopped_signal_sent = false;
	last_run_index = -1;

	variables_enabled = NULL;
	actual_enabled_variables = NULL;
	num_actual_variables = 0;
	num_total_variables = 0;
	target_col = -1;
	cache_results_for_all_training_data = true;

	problem_description = new char[100];
	strcpy(problem_description, "Problem description here ...");

	random_subset_indexes = NULL;
}
//---------------------------------------------------------------------------
t_mep::~t_mep()
{
	clear();
}
//---------------------------------------------------------------------------
void t_mep::clear(void)
{
	if (actual_enabled_variables) {
		delete[] actual_enabled_variables;
		actual_enabled_variables = NULL;
	}
	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}

	statistics.delete_memory();

	if (problem_description) {
		delete[] problem_description;
		problem_description = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep::allocate_values(double**** eval_double, s_value_class*** array_value_class)
{
	*eval_double = new double** [mep_parameters.get_num_threads()];
	for (int c = 0; c < mep_parameters.get_num_threads(); c++) {
		(*eval_double)[c] = new double* [mep_parameters.get_code_length()];
		for (int i = 0; i < mep_parameters.get_code_length(); i++)
			(*eval_double)[c][i] = new double[training_data.get_num_rows()];
	}

	cached_eval_variables_matrix_double = new double* [num_total_variables];
	for (int i = 0; i < num_total_variables; i++)
		cached_eval_variables_matrix_double[i] = NULL;
	for (int i = 0; i < num_actual_variables; i++)
		cached_eval_variables_matrix_double[actual_enabled_variables[i]] = new double[training_data.get_num_rows()];

	cached_sum_of_errors = new double[num_total_variables];
	cached_threashold = new double[num_total_variables];

	*array_value_class = new s_value_class * [mep_parameters.get_num_threads()];
	for (int c = 0; c < mep_parameters.get_num_threads(); c++)
		(*array_value_class)[c] = new s_value_class[training_data.get_num_rows()];
}
//---------------------------------------------------------------------------
void t_mep::allocate_sub_population(t_sub_population & a_pop)
{
	int num_outputs = 0;
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_REGRESSION || mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION)
		num_outputs = 1;
	else // MEP_PROBLEM_MULTICLASS
		if (mep_parameters.get_error_measure() == MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR || mep_parameters.get_error_measure() == MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR)
			num_outputs = 0;
		else
			num_outputs = training_data.get_num_classes();

	a_pop.offspring1.allocate_memory(mep_parameters.get_code_length(), 
			num_total_variables, mep_parameters.get_constants_probability() > 1E-6, 
			&mep_constants, num_outputs);
	a_pop.offspring2.allocate_memory(mep_parameters.get_code_length(), 
			num_total_variables, mep_parameters.get_constants_probability() > 1E-6, 
			&mep_constants, num_outputs);
	a_pop.individuals = new t_mep_chromosome[mep_parameters.get_subpopulation_size()];
	for (int j = 0; j < mep_parameters.get_subpopulation_size(); j++)
		a_pop.individuals[j].allocate_memory(mep_parameters.get_code_length(), 
				num_total_variables, mep_parameters.get_constants_probability() > 1E-6, 
				&mep_constants, num_outputs);
}
//---------------------------------------------------------------------------
void t_mep::get_best(t_mep_chromosome & dest)
{
	dest = pop[best_subpopulation_index].individuals[best_individual_index];
}
//---------------------------------------------------------------------------
bool t_mep::get_output(int run_index, double* inputs, double* outputs)
{
	int index_error_gene;
	int max_index;
	if (run_index > -1) {
		switch (mep_parameters.get_problem_type()) {
		case MEP_PROBLEM_REGRESSION:
			if (!statistics.get_stat_ptr(run_index)->best_program.evaluate_double(inputs, outputs, index_error_gene))
				return false;
			break;
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			if (!statistics.get_stat_ptr(run_index)->best_program.evaluate_double(inputs, outputs, index_error_gene))
				return false;

			if (outputs[0] <= statistics.get_stat_ptr(run_index)->best_program.get_best_class_threshold())
				outputs[0] = 0;
			else
				outputs[0] = 1;

			break;
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			switch (mep_parameters.get_error_measure()) {
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
			case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
				max_index = -1;
				if (!statistics.get_stat_ptr(run_index)->best_program.get_first_max_index(inputs, max_index, index_error_gene))
					return false;
				outputs[0] = max_index % training_data.get_num_classes();
				break;
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
				max_index = -1;
				outputs[0] = -1;
				if (!statistics.get_stat_ptr(run_index)->best_program.get_first_max_index(inputs, max_index, index_error_gene))
					return false;
				for (int c = 0; c < training_data.get_num_classes(); c++)
					if (statistics.get_stat_ptr(run_index)->best_program.get_index_best_gene(c) == max_index) {
						outputs[0] = c;
						break;
					}

				break;
			}// end switch error measure
			break;
		}// end switch problem type
	}
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
/*
void t_mep::compute_cached_eval_matrix_double(void)
{

if (mep_parameters.get_problem_type() == PROBLEM_REGRESSION)
for (int v = 0; v < training_data.num_vars; v++) {
cached_sum_of_errors[v] = 0;
for (int k = 0; k < training_data.get_num_rows(); k++) {
cached_eval_variables_matrix_double[v][k] = training_data._data_double[k][v];
cached_sum_of_errors[v] += fabs(cached_eval_variables_matrix_double[v][k] - training_data._target_double[k]);
}
}
else
for (int v = 0; v < training_data.num_vars; v++) {
cached_sum_of_errors[v] = 0;
for (int k = 0; k < training_data.get_num_rows(); k++) {
cached_eval_variables_matrix_double[v][k] = training_data._data_double[k][v];
if (cached_eval_variables_matrix_double[v][k] <= mep_parameters.classification_threshold)
cached_sum_of_errors[v] += training_data._target_double[k];
else
cached_sum_of_errors[v] += 1 - training_data._target_double[k];
}
}

}
*/
//---------------------------------------------------------------------------
void t_mep::compute_cached_eval_matrix_double2(s_value_class * array_value_class)
{
	double** data = training_data.get_data_matrix_double();

	if (mep_parameters.get_problem_type() == MEP_PROBLEM_REGRESSION) {
		for (int v = 0; v < num_actual_variables; v++) {
			cached_sum_of_errors[actual_enabled_variables[v]] = 0;
			if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
				for (int k = 0; k < training_data.get_num_rows(); k++) {
					cached_eval_variables_matrix_double[actual_enabled_variables[v]][k] = 
							data[k][actual_enabled_variables[v]];
					cached_sum_of_errors[actual_enabled_variables[v]] += 
							mep_absolute_error(cached_eval_variables_matrix_double[actual_enabled_variables[v]][k], 
									data[k][num_total_variables]);
				}
			else// MEP_REGRESSION_MEAN_SQUARED_ERROR
				for (int k = 0; k < training_data.get_num_rows(); k++) {
					cached_eval_variables_matrix_double[actual_enabled_variables[v]][k] = 
							data[k][actual_enabled_variables[v]];
					cached_sum_of_errors[actual_enabled_variables[v]] += 
							mep_squared_error(cached_eval_variables_matrix_double[actual_enabled_variables[v]][k], 
									data[k][num_total_variables]);
				}

			cached_sum_of_errors[actual_enabled_variables[v]] /= (double)training_data.get_num_rows();
		}
	}
	else// classification
		for (int v = 0; v < num_actual_variables; v++) {

			cached_threashold[actual_enabled_variables[v]] = 0;
			for (int k = 0; k < training_data.get_num_rows(); k++) {
				cached_eval_variables_matrix_double[actual_enabled_variables[v]][k] = 
						data[k][actual_enabled_variables[v]];
				array_value_class[k].value = data[k][actual_enabled_variables[v]];
				array_value_class[k].data_class = (int)data[k][num_total_variables];
			}
			qsort((void*)array_value_class, training_data.get_num_rows(), 
					sizeof(s_value_class), sort_function_value_class);

			int num_0_incorrect = training_data.get_num_items_class_0();
			int num_1_incorrect = 0;
			cached_threashold[actual_enabled_variables[v]] = array_value_class[0].value - 1;// all are classified to class 1 in this case
			cached_sum_of_errors[actual_enabled_variables[v]] = num_0_incorrect;

			for (int i = 0; i < training_data.get_num_rows(); i++) {
				int j = i + 1;
				//while (j < training_data.get_num_rows() && fabs(cached_eval_variables_matrix_double[actual_enabled_variables[v]][i] - cached_eval_variables_matrix_double[actual_enabled_variables[v]][j]) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
				while (j < training_data.get_num_rows() && fabs(array_value_class[i].value - array_value_class[j].value) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
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
			cached_sum_of_errors[actual_enabled_variables[v]] /= (double)training_data.get_num_rows();
			cached_sum_of_errors[actual_enabled_variables[v]] *= 100;
		}
}
//---------------------------------------------------------------------------
/*
void t_mep::compute_eval_vector_double(t_mep_chromosome &)
{
}
*/
//---------------------------------------------------------------------------
int sort_function_chromosomes(const void* a, const void* b)
{
	return ((t_mep_chromosome*)a)->compare(*(t_mep_chromosome*)b, false);
}
//---------------------------------------------------------------------------
void t_mep::sort_by_fitness(t_sub_population & a_pop) // sort ascending the individuals in population
{
	qsort((void*)a_pop.individuals, mep_parameters.get_subpopulation_size(),
			sizeof(a_pop.individuals[0]), sort_function_chromosomes);
}
//---------------------------------------------------------------------------
void t_mep::delete_sub_population(t_sub_population & a_pop)
{
	if (a_pop.individuals)
		delete[] a_pop.individuals;

	a_pop.offspring1.clear();
	a_pop.offspring2.clear();
}
//---------------------------------------------------------------------------
void t_mep::delete_values(double**** eval_double, s_value_class * **array_value_class)
{
	if (*eval_double) {
		for (int c = 0; c < mep_parameters.get_num_threads(); c++) {
			for (int i = 0; i < mep_parameters.get_code_length(); i++)
				delete[](*eval_double)[c][i];
			delete[](*eval_double)[c];
		}
		delete[] * eval_double;
		(*eval_double) = NULL;
	}
	if (cached_eval_variables_matrix_double) {
		for (int i = 0; i < num_total_variables; i++)
			delete[] cached_eval_variables_matrix_double[i];
		delete[] cached_eval_variables_matrix_double;
		cached_eval_variables_matrix_double = NULL;
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
		for (int c = 0; c < mep_parameters.get_num_threads(); c++)
			delete[](*array_value_class)[c];
		delete[] * array_value_class;
		*array_value_class = NULL;
	}
}
//---------------------------------------------------------------------------
long t_mep::tournament(const t_sub_population & a_pop, t_seed & seed) 
{
	long r, p;
	p = mep_int_rand(seed, 0, mep_parameters.get_subpopulation_size() - 1);
	for (int i = 1; i < mep_parameters.get_tournament_size(); i++) {
		r = mep_int_rand(seed, 0, mep_parameters.get_subpopulation_size() - 1);
		p = a_pop.individuals[r].get_fitness() < a_pop.individuals[p].get_fitness() ? r : p;
	}
	return p;
}
//---------------------------------------------------------------------------
void t_mep::compute_best_and_average_error(double& best_error, double& mean_error, 
		double& num_incorrectly_classified, double& average_incorrectly_classified)
{
	mean_error = 0;
	average_incorrectly_classified = 0;
	pop[0].compute_best_index(mep_parameters.get_subpopulation_size());
	best_error = pop[0].individuals[pop[0].best_index].get_fitness();
	num_incorrectly_classified = pop[0].individuals[pop[0].best_index].get_num_incorrectly_classified();

	best_individual_index = 0;
	best_subpopulation_index = 0;
	for (int i = 0; i < mep_parameters.get_num_subpopulations(); i++) {
		pop[i].compute_best_index(mep_parameters.get_subpopulation_size());
		if (best_error > pop[i].individuals[pop[i].best_index].get_fitness()) {
			best_error = pop[i].individuals[pop[i].best_index].get_fitness();
			num_incorrectly_classified = pop[i].individuals[pop[i].best_index].get_num_incorrectly_classified();
			best_individual_index = pop[i].best_index;
			best_subpopulation_index = i;
		}
	}

	for (int i = 0; i < mep_parameters.get_num_subpopulations(); i++)
		for (int k = 0; k < mep_parameters.get_subpopulation_size(); k++) {
			mean_error += pop[i].individuals[k].get_fitness();
			average_incorrectly_classified += pop[i].individuals[pop[i].best_index].get_num_incorrectly_classified();
		}

	mean_error /= mep_parameters.get_num_subpopulations() * mep_parameters.get_subpopulation_size();
	average_incorrectly_classified /= mep_parameters.get_num_subpopulations() * mep_parameters.get_subpopulation_size();
}
//---------------------------------------------------------------------------
double t_mep::compute_validation_error(int& best_subpopulation_index_for_validation, 
		int& best_individual_index_for_validation, double** eval_double, 
		s_value_class * tmp_value_class, t_seed * seeds, double& best_num_incorrectly_classified)
{
	double best_validation_error = -1;
	double validation_error;
	int index_error_gene = -1;
	double num_incorrectly_classified = 100;
	//int k = 0, i;
	bool result;

	switch (mep_parameters.get_problem_type()){
		case MEP_PROBLEM_REGRESSION:
					result = true;
					if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
						result = pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].compute_regression_error_on_double_data_return_error(
							validation_data.get_data_matrix_double(),
							validation_data.get_num_rows(), validation_data.get_num_cols() - 1,
							validation_error, index_error_gene, mep_absolute_error);
					else
						result = pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].compute_regression_error_on_double_data_return_error(
							validation_data.get_data_matrix_double(),
							validation_data.get_num_rows(), validation_data.get_num_cols() - 1,
							validation_error, index_error_gene, mep_squared_error);
					while (!result) {
						// I have to mutate that a_chromosome.
						pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].set_gene_operation(index_error_gene,
							actual_enabled_variables[mep_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables - 1)]);
						// recompute its fitness on training;
						if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
							pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].fitness_regression(training_data,
								random_subset_indexes, mep_parameters.get_random_subset_selection_size(),
								cached_eval_variables_matrix_double, cached_sum_of_errors,
								num_actual_variables, actual_enabled_variables,
								eval_double, mep_absolute_error, seeds[best_subpopulation_index]);
						else
							pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].fitness_regression(training_data,
								random_subset_indexes, mep_parameters.get_random_subset_selection_size(),
								cached_eval_variables_matrix_double, cached_sum_of_errors,
								num_actual_variables, actual_enabled_variables,
								eval_double, mep_squared_error, seeds[best_subpopulation_index]);
						// resort the population
						//sort_by_fitness(pop[best_subpopulation_index]);
						pop[best_subpopulation_index].compute_best_index(mep_parameters.get_subpopulation_size());
						if (pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].get_fitness() >
							pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].worst_index].get_fitness())
							pop[best_subpopulation_index].compute_worst_index(mep_parameters.get_subpopulation_size());
						// apply it again on validation
						if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
							result = pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].compute_regression_error_on_double_data_return_error(
								validation_data.get_data_matrix_double(),
								validation_data.get_num_rows(), validation_data.get_num_cols() - 1,
								validation_error, index_error_gene, mep_absolute_error);
						else
							result = pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].compute_regression_error_on_double_data_return_error(
								validation_data.get_data_matrix_double(),
								validation_data.get_num_rows(), validation_data.get_num_cols() - 1,
								validation_error, index_error_gene, mep_squared_error);
					}
					// now it is ok; no errors on
					if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
						best_validation_error = validation_error;
						best_subpopulation_index_for_validation = best_subpopulation_index;
						best_individual_index_for_validation = 0;
					}
			break;

		case MEP_PROBLEM_BINARY_CLASSIFICATION:
					while (!pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].compute_binary_classification_error_on_double_data_return_error(
								validation_data.get_data_matrix_double(), 
								validation_data.get_num_rows(), validation_data.get_num_cols() - 1, 
								validation_error, index_error_gene)) {
						pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].set_gene_operation(index_error_gene,
									actual_enabled_variables[mep_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables - 1)]);
						// recompute its fitness on training;
						pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].fitness_binary_classification(training_data,
								random_subset_indexes, mep_parameters.get_random_subset_selection_size(), 
								cached_eval_variables_matrix_double, cached_sum_of_errors, 
								cached_threashold, num_actual_variables, 
								actual_enabled_variables, eval_double, tmp_value_class, seeds[best_subpopulation_index]);
						// resort the population
						//sort_by_fitness(pop[best_subpopulation_index]);
						pop[best_subpopulation_index].compute_best_index(mep_parameters.get_subpopulation_size());
						if (pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].get_fitness() >
							pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].worst_index].get_fitness())
							pop[best_subpopulation_index].compute_worst_index(mep_parameters.get_subpopulation_size());

					}
					if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
						best_validation_error = validation_error;
						best_subpopulation_index_for_validation = best_subpopulation_index;
						best_individual_index_for_validation = 0;
						best_num_incorrectly_classified = validation_error;
					}
			break;

		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:

			switch (mep_parameters.get_error_measure()) {
				case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
							while (!pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].compute_multi_class_classification_error_on_double_data_return_error(validation_data.get_data_matrix_double(), validation_data.get_num_rows(), validation_data.get_num_cols() - 1, validation_data.get_num_classes(), validation_error, index_error_gene, num_incorrectly_classified)) {
								pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].set_gene_operation(index_error_gene,
										actual_enabled_variables[mep_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables - 1)]);
								// recompute its fitness on training;
								pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].fitness_multi_class_classification_winner_takes_all_fixed(
										training_data, random_subset_indexes, 
										mep_parameters.get_random_subset_selection_size(), 
										cached_eval_variables_matrix_double, num_actual_variables, 
										actual_enabled_variables, eval_double, seeds[best_subpopulation_index]);
								// resort the population
								//sort_by_fitness(pop[best_subpopulation_index]);
								pop[best_subpopulation_index].compute_best_index(mep_parameters.get_subpopulation_size());
								if (pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].get_fitness() >
									pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].worst_index].get_fitness())
									pop[best_subpopulation_index].compute_worst_index(mep_parameters.get_subpopulation_size());

							}
							if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
								best_validation_error = validation_error;
								best_num_incorrectly_classified = num_incorrectly_classified;
								best_subpopulation_index_for_validation = best_subpopulation_index;
								best_individual_index_for_validation = 0;
							}
						
					break;
				case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
							while (!pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].compute_multi_class_classification_error_on_double_data_return_error(validation_data.get_data_matrix_double(), validation_data.get_num_rows(), validation_data.get_num_cols() - 1, validation_data.get_num_classes(), validation_error, index_error_gene, num_incorrectly_classified)) {
								pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].set_gene_operation(index_error_gene,
										actual_enabled_variables[mep_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables - 1)]);
								// recompute its fitness on training;
								pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].fitness_multi_class_classification_smooth(training_data,
										random_subset_indexes, 
										mep_parameters.get_random_subset_selection_size(), 
										cached_eval_variables_matrix_double, num_actual_variables, 
										actual_enabled_variables, eval_double, seeds[best_subpopulation_index]);
								// resort the population
								//sort_by_fitness(pop[best_subpopulation_index]);
								pop[best_subpopulation_index].compute_best_index(mep_parameters.get_subpopulation_size());
								if (pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].get_fitness() >
									pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].worst_index].get_fitness())
									pop[best_subpopulation_index].compute_worst_index(mep_parameters.get_subpopulation_size());

							}
							if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
								best_validation_error = validation_error;
								best_num_incorrectly_classified = num_incorrectly_classified;
								best_subpopulation_index_for_validation = best_subpopulation_index;
								best_individual_index_for_validation = 0;
							}
						
					break;
				case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
							while (!pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data_return_error(validation_data.get_data_matrix_double(), validation_data.get_num_rows(), validation_data.get_num_cols() - 1, validation_data.get_num_classes(), validation_error, index_error_gene, num_incorrectly_classified)) {
								pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].set_gene_operation(index_error_gene,
										actual_enabled_variables[mep_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables - 1)]);
								// recompute its fitness on training;
								pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].fitness_multi_class_classification_winner_takes_all_dynamic(
										training_data, random_subset_indexes, 
										mep_parameters.get_random_subset_selection_size(), 
										cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[best_subpopulation_index]);
								// resort the population
								//sort_by_fitness(pop[best_subpopulation_index]);
								pop[best_subpopulation_index].compute_best_index(mep_parameters.get_subpopulation_size());
								if (pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index].get_fitness() >
									pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].worst_index].get_fitness())
									pop[best_subpopulation_index].compute_worst_index(mep_parameters.get_subpopulation_size());

							}
							if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
								best_validation_error = validation_error;
								best_num_incorrectly_classified = num_incorrectly_classified;
								best_subpopulation_index_for_validation = best_subpopulation_index;
								best_individual_index_for_validation = 0;
							}
						
					break;
				}// end switch (mep_parameters.get_error_measure()) {
			break;
	}
	return best_validation_error;
}
//---------------------------------------------------------------------------
int t_mep::start(f_on_progress on_generation, f_on_progress on_new_evaluation, f_on_progress on_complete_run)
{
	_stopped = false;
	_stopped_signal_sent = false;

	compute_list_of_enabled_variables();
	random_subset_indexes = new int[training_data.get_num_rows()];

	if (mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION)
		training_data.count_0_class(target_col);
	else
		if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION)
			training_data.count_num_classes(target_col);

	pop = new t_sub_population[mep_parameters.get_num_subpopulations()];
	for (int i = 0; i < mep_parameters.get_num_subpopulations(); i++)
		allocate_sub_population(pop[i]);

	num_selected_operators = mep_operators.get_list_of_operators(actual_operators);

	double*** eval_double;           // an array where the values of each expression are stored

	s_value_class * *array_value_class;

	allocate_values(&eval_double, &array_value_class);

	compute_cached_eval_matrix_double2(array_value_class[0]);

	t_seed * seeds = new t_seed[mep_parameters.get_num_subpopulations()];
	for (int p = 0; p < mep_parameters.get_num_subpopulations(); p++)
		seeds[p].init(p);

	last_run_index = -1;

	statistics.create(mep_parameters.get_num_runs());
	for (int run_index = 0; run_index < mep_parameters.get_num_runs(); run_index++) {
		statistics.append(mep_parameters.get_num_generations());
		last_run_index++;
		start_steady_state(run_index, seeds, eval_double, array_value_class, on_generation, on_new_evaluation);
		if (on_complete_run)
			on_complete_run();
		if (_stopped_signal_sent)
			break;
	}

	statistics.compute_mean_stddev(
			mep_parameters.get_use_validation_data() && validation_data.get_num_rows(), 
			test_data.get_num_rows(), mep_parameters.get_problem_type());

	delete_values(&eval_double, &array_value_class);
	for (int i = 0; i < mep_parameters.get_num_subpopulations(); i++)
		delete_sub_population(pop[i]);
	delete[] pop;

	delete[] random_subset_indexes;

	delete[] seeds;

	_stopped = true;
	return true;
}
//---------------------------------------------------------------------------
void t_mep::evolve_one_subpopulation_for_one_generation(int* current_subpop_index, 
		std::mutex * mutex, t_sub_population * sub_populations, 
		int generation_index, bool recompute_fitness, 
		double** eval_double, s_value_class * tmp_value_class, t_seed * seeds)
{
	int pop_index = 0;
	int subpopulation_size = mep_parameters.get_subpopulation_size();
	while (*current_subpop_index < mep_parameters.get_num_subpopulations()) {
		// still more subpopulations to evolve?

		while (!mutex->try_lock()) {}// create a lock so that multiple threads will not evolve the same sub population
		pop_index = *current_subpop_index;
		(*current_subpop_index)++;
		mutex->unlock();

		// pop_index is the index of the subpopulation evolved by the current thread

		if (pop_index < mep_parameters.get_num_subpopulations()) {
			t_sub_population* a_sub_population = &sub_populations[pop_index];

			if (!generation_index) { // first generation
				for (int j = 0; j < subpopulation_size; j++)
					pop[pop_index].individuals[j].generate_random(
							mep_parameters, mep_constants, 
							actual_operators, num_selected_operators, 
							actual_enabled_variables, num_actual_variables, seeds[pop_index]);
				// compute fitness
				switch (mep_parameters.get_problem_type()) {
				case MEP_PROBLEM_REGRESSION:
					if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
						for (int i = 0; i < subpopulation_size; i++)
							pop[pop_index].individuals[i].fitness_regression(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, cached_sum_of_errors, 
									num_actual_variables, actual_enabled_variables, 
									eval_double, mep_absolute_error, seeds[pop_index]);
					else
						for (int i = 0; i < subpopulation_size; i++)
							pop[pop_index].individuals[i].fitness_regression(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, cached_sum_of_errors, 
									num_actual_variables, actual_enabled_variables, 
									eval_double, mep_squared_error, seeds[pop_index]);
					break;
				case MEP_PROBLEM_BINARY_CLASSIFICATION:
					for (int i = 0; i < subpopulation_size; i++)
						pop[pop_index].individuals[i].fitness_binary_classification(
								training_data, random_subset_indexes, 
								mep_parameters.get_random_subset_selection_size(), 
								cached_eval_variables_matrix_double, cached_sum_of_errors, cached_threashold, 
								num_actual_variables, actual_enabled_variables, 
								eval_double, tmp_value_class, seeds[pop_index]);
					break;
				case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
					switch (mep_parameters.get_error_measure()) {
					case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
						for (int i = 0; i < subpopulation_size; i++)
							pop[pop_index].individuals[i].fitness_multi_class_classification_winner_takes_all_fixed(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, 
									num_actual_variables, actual_enabled_variables, 
									eval_double, seeds[pop_index]);
						break;
					case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
						for (int i = 0; i < subpopulation_size; i++)
							pop[pop_index].individuals[i].fitness_multi_class_classification_smooth(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
						break;
					case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
						for (int i = 0; i < subpopulation_size; i++)
							pop[pop_index].individuals[i].fitness_multi_class_classification_winner_takes_all_dynamic(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
						break;

					}
					break;
				}
				//sort_by_fitness(pop[pop_index]);
				a_sub_population->compute_worst_index(subpopulation_size);
			}
			else {// other generations after the first one
				if (recompute_fitness) {
					switch (mep_parameters.get_problem_type()) {
					case MEP_PROBLEM_REGRESSION:
						if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
							for (int i = 0; i < subpopulation_size; i++)
								pop[pop_index].individuals[i].fitness_regression(
										training_data, random_subset_indexes, 
										mep_parameters.get_random_subset_selection_size(), 
										cached_eval_variables_matrix_double, cached_sum_of_errors, 
										num_actual_variables, actual_enabled_variables, 
										eval_double, mep_absolute_error, seeds[pop_index]);
						else
							for (int i = 0; i < subpopulation_size; i++)
								pop[pop_index].individuals[i].fitness_regression(
										training_data, random_subset_indexes, 
										mep_parameters.get_random_subset_selection_size(), 
										cached_eval_variables_matrix_double, cached_sum_of_errors, 
										num_actual_variables, actual_enabled_variables, 
										eval_double, mep_squared_error, seeds[pop_index]);
						break;
					case MEP_PROBLEM_BINARY_CLASSIFICATION:
						for (int i = 0; i < subpopulation_size; i++)
							pop[pop_index].individuals[i].fitness_binary_classification(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, 
									cached_sum_of_errors, cached_threashold, 
									num_actual_variables, actual_enabled_variables, 
									eval_double, tmp_value_class, seeds[pop_index]);
						break;
					case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
						switch (mep_parameters.get_error_measure()) {
						case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
							for (int i = 0; i < subpopulation_size; i++)
								pop[pop_index].individuals[i].fitness_multi_class_classification_winner_takes_all_fixed(
										training_data, random_subset_indexes, 
										mep_parameters.get_random_subset_selection_size(), 
										cached_eval_variables_matrix_double, num_actual_variables, 
										actual_enabled_variables, eval_double, seeds[pop_index]);
							break;
						case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
							for (int i = 0; i < subpopulation_size; i++)
								pop[pop_index].individuals[i].fitness_multi_class_classification_smooth(
										training_data, random_subset_indexes, 
										mep_parameters.get_random_subset_selection_size(), 
										cached_eval_variables_matrix_double, num_actual_variables, 
										actual_enabled_variables, eval_double, seeds[pop_index]);
							break;
						case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
							for (int i = 0; i < subpopulation_size; i++)
								pop[pop_index].individuals[i].fitness_multi_class_classification_winner_takes_all_dynamic(
										training_data, random_subset_indexes, 
										mep_parameters.get_random_subset_selection_size(), 
										cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
							break;

						}
						break;
					}
					//sort_by_fitness(pop[pop_index]);
					a_sub_population->compute_worst_index(subpopulation_size);
				}
				// continue
				for (int k = 0; k < subpopulation_size; k += 2) {
					// choose the parents using binary tournament
					long r1 = tournament(*a_sub_population, seeds[pop_index]);
					long r2 = tournament(*a_sub_population, seeds[pop_index]);
					// crossover
					double p = mep_real_rand(seeds[pop_index], 0, 1);
					if (p < mep_parameters.get_crossover_probability()) {
						if (mep_parameters.get_crossover_type() == MEP_UNIFORM_CROSSOVER)
							a_sub_population->individuals[r1].uniform_crossover(
								a_sub_population->individuals[r2],
								a_sub_population->offspring1, a_sub_population->offspring2,
								mep_constants, seeds[pop_index]);
						else
							a_sub_population->individuals[r1].one_cut_point_crossover(
								a_sub_population->individuals[r2],
								a_sub_population->offspring1, a_sub_population->offspring2,
								mep_constants, seeds[pop_index]);
					}
					else {
						// cannot do swap pointers here!

						a_sub_population->offspring1 = a_sub_population->individuals[r1];
						a_sub_population->offspring2 = a_sub_population->individuals[r2];
					}
					// mutate the result and move the mutant in the new population
					a_sub_population->offspring1.mutation(mep_parameters, mep_constants, 
							actual_operators, num_selected_operators, 
							actual_enabled_variables, num_actual_variables, seeds[pop_index]);
					switch (mep_parameters.get_problem_type()) {
					case MEP_PROBLEM_REGRESSION:
						if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
							a_sub_population->offspring1.fitness_regression(training_data, 
									random_subset_indexes, mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, cached_sum_of_errors, 
								num_actual_variables, actual_enabled_variables, 
								eval_double, mep_absolute_error, seeds[pop_index]);
						else
							a_sub_population->offspring1.fitness_regression(training_data, 
									random_subset_indexes, mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, cached_sum_of_errors, 
									num_actual_variables, actual_enabled_variables, 
									eval_double, mep_squared_error, seeds[pop_index]);
						break;
					case MEP_PROBLEM_BINARY_CLASSIFICATION:
						a_sub_population->offspring1.fitness_binary_classification(training_data, 
								random_subset_indexes, mep_parameters.get_random_subset_selection_size(), 
								cached_eval_variables_matrix_double, cached_sum_of_errors, cached_threashold, 
								num_actual_variables, actual_enabled_variables, 
								eval_double, tmp_value_class, seeds[pop_index]);
						break;
					case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
						switch (mep_parameters.get_error_measure()) {
						case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
							a_sub_population->offspring1.fitness_multi_class_classification_winner_takes_all_fixed(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
							break;
						case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
							a_sub_population->offspring1.fitness_multi_class_classification_smooth(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
							break;
						case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
							a_sub_population->offspring1.fitness_multi_class_classification_winner_takes_all_dynamic(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
							break;
						}
						break;
					}

					a_sub_population->offspring2.mutation(mep_parameters, 
							mep_constants, actual_operators, num_selected_operators, 
							actual_enabled_variables, num_actual_variables, seeds[pop_index]);

					switch (mep_parameters.get_problem_type()) {
					case MEP_PROBLEM_REGRESSION:
						if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
							a_sub_population->offspring2.fitness_regression(training_data, 
									random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, cached_sum_of_errors, 
									num_actual_variables, actual_enabled_variables, 
									eval_double, mep_absolute_error, seeds[pop_index]);
						else
							a_sub_population->offspring2.fitness_regression(training_data, 
									random_subset_indexes, mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, cached_sum_of_errors, 
									num_actual_variables, actual_enabled_variables, 
									eval_double, mep_squared_error, seeds[pop_index]);
						break;

					case MEP_PROBLEM_BINARY_CLASSIFICATION:
						a_sub_population->offspring2.fitness_binary_classification(
								training_data, random_subset_indexes, 
								mep_parameters.get_random_subset_selection_size(), 
								cached_eval_variables_matrix_double, 
								cached_sum_of_errors, cached_threashold, 
								num_actual_variables, actual_enabled_variables, 
								eval_double, tmp_value_class, seeds[pop_index]);
						break;
					case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
						switch (mep_parameters.get_error_measure()) {
						case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
							a_sub_population->offspring2.fitness_multi_class_classification_winner_takes_all_fixed(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
							break;
						case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
							a_sub_population->offspring2.fitness_multi_class_classification_smooth(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
							break;
						case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
							a_sub_population->offspring2.fitness_multi_class_classification_winner_takes_all_dynamic(
									training_data, random_subset_indexes, 
									mep_parameters.get_random_subset_selection_size(), 
									cached_eval_variables_matrix_double, num_actual_variables, 
									actual_enabled_variables, eval_double, seeds[pop_index]);
							break;
						}
						break;
					}

					if (a_sub_population->offspring1.get_fitness() < a_sub_population->offspring2.get_fitness()) {   // the best offspring replaces the worst a_chromosome in the population
						if (a_sub_population->offspring1.get_fitness() < a_sub_population->individuals[a_sub_population->worst_index].get_fitness()) {
							a_sub_population->individuals[a_sub_population->worst_index].swap_pointers(a_sub_population->offspring1);
							a_sub_population->compute_worst_index(subpopulation_size);
						}
						else;
					}
					else {
						if (a_sub_population->offspring2.get_fitness() < a_sub_population->individuals[a_sub_population->worst_index].get_fitness()) {
							a_sub_population->individuals[a_sub_population->worst_index].swap_pointers(a_sub_population->offspring2);
							a_sub_population->compute_worst_index(subpopulation_size);
						}
					}
				}
			}// end other generations
		}// end of if (pop_index < mep_parameters.get_num_subpopulations())
	}
}
//-----------------------------------------------------------------------
bool t_mep::start_steady_state(int run, t_seed * seeds, double*** eval_double, 
		s_value_class * *array_value_class, 
		f_on_progress on_generation, f_on_progress /*on_new_evaluation*/)       // Steady-State MEP
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	//wxLogDebug(wxString() << "generation " << gen_index << " ");
	// an array of threads. Each sub population is evolved by a thread
	std::thread* mep_threads = new std::thread [mep_parameters.get_num_threads()];
	// we create a fixed number of threads and each thread will take and evolve one subpopulation, then it will take another one
	std::mutex mutex;
	// we need a mutex to make sure that the same subpopulation will not be evolved twice by different threads

	unsigned int num_generations_for_which_random_subset_is_kept_fixed_counter = mep_parameters.get_num_generations_for_which_random_subset_is_kept_fixed();

	for (int generation_index = 0; generation_index < mep_parameters.get_num_generations(); generation_index++) {
		if (_stopped_signal_sent)
			break;

		if (num_generations_for_which_random_subset_is_kept_fixed_counter >= mep_parameters.get_num_generations_for_which_random_subset_is_kept_fixed()) {
			get_random_subset(mep_parameters.get_random_subset_selection_size(), random_subset_indexes, seeds[0]);
			num_generations_for_which_random_subset_is_kept_fixed_counter = 0;
		}
		else
			num_generations_for_which_random_subset_is_kept_fixed_counter++;

		int current_subpop_index = 0;
		for (int t = 0; t < mep_parameters.get_num_threads(); t++) {
			bool recompute_fitness;
			if (mep_parameters.get_random_subset_selection_size() == training_data.get_num_rows())
				recompute_fitness = generation_index == 0;
			else
				recompute_fitness = num_generations_for_which_random_subset_is_kept_fixed_counter == 0;
			mep_threads[t] = std::thread(&t_mep::evolve_one_subpopulation_for_one_generation, 
												this, &current_subpop_index, &mutex, pop, 
												generation_index, recompute_fitness, 
												eval_double[t], array_value_class[t], seeds);
		}

		for (int t = 0; t < mep_parameters.get_num_threads(); t++) {
			mep_threads[t].join(); // wait for all threads to execute
		}

		// now copy one from each subpopulation to the next one
		for (int d = 0; d < mep_parameters.get_num_subpopulations(); d++) { // din d in d+1
			// choose a random individual from subpopulation d and copy over the worst in d + 1
			long w = mep_int_rand(seeds[d], 0, mep_parameters.get_subpopulation_size() - 1);
			if (pop[d].individuals[w].compare(pop[(d + 1) % mep_parameters.get_num_subpopulations()].individuals[pop[(d + 1) % mep_parameters.get_num_subpopulations()].worst_index], false)) {
				pop[(d + 1) % mep_parameters.get_num_subpopulations()].individuals[pop[(d + 1) % mep_parameters.get_num_subpopulations()].worst_index] = pop[d].individuals[w];
				pop[(d + 1) % mep_parameters.get_num_subpopulations()].compute_worst_index(mep_parameters.get_subpopulation_size());
			}
		}
		//for (int d = 0; d < mep_parameters.get_num_subpopulations(); d++) // din d in d+1
		//	sort_by_fitness(pop[d]);

		double best_error_on_training, mean_error_on_training;
		double num_incorrectly_classified_on_training;
		double average_incorrectly_classified_on_training;

		compute_best_and_average_error(best_error_on_training, mean_error_on_training, 
					num_incorrectly_classified_on_training, average_incorrectly_classified_on_training);
		statistics.get_stat_ptr(run)->best_training_error[generation_index] = best_error_on_training;
		statistics.get_stat_ptr(run)->average_training_error[generation_index] = mean_error_on_training;

		statistics.get_stat_ptr(run)->best_training_num_incorrect[generation_index] = num_incorrectly_classified_on_training;
		statistics.get_stat_ptr(run)->average_training_num_incorrect[generation_index] = average_incorrectly_classified_on_training;

		if (mep_parameters.get_use_validation_data() && validation_data.get_num_rows() > 0) {
			// I must run all solutions for the validation data and choose the best one
			int best_index_on_validation, best_subpop_index_on_validation;
			double num_incorrectly_classified_on_validation;
			double validation_error = compute_validation_error(best_subpop_index_on_validation, 
					best_index_on_validation, eval_double[0], array_value_class[0], 
					seeds, num_incorrectly_classified_on_validation);
			if ((validation_error < statistics.get_stat_ptr(run)->best_validation_error) || 
					(fabs(statistics.get_stat_ptr(run)->best_validation_error + 1) <= 1E-6)) {
				statistics.get_stat_ptr(run)->best_validation_error = validation_error;
				statistics.get_stat_ptr(run)->best_validation_num_incorrect = num_incorrectly_classified_on_validation;
				best_individual_index_for_test = best_index_on_validation;
				best_subpopulation_index_for_test = best_subpop_index_on_validation;
				statistics.get_stat_ptr(run)->best_program = pop[best_subpopulation_index_for_test].individuals[best_individual_index_for_test];
			}
		}
		else
			statistics.get_stat_ptr(run)->best_program = pop[best_subpopulation_index].individuals[pop[best_subpopulation_index].best_index];

		statistics.get_stat_ptr(run)->last_generation = generation_index;
#ifdef _DEBUG

#endif
		if (on_generation)
			on_generation();
	}

	// DEBUG ONLY
	//	fitness_regression(pop[best_individual_index]);
	/*
	if (mep_parameters.get_problem_type() == PROBLEM_REGRESSION)
	fitness_regression(pop[0].individuals[0], eval_double[0]);
	else
	if (mep_parameters.get_problem_type() == PROBLEM_CLASSIFICATION)
	fitness_classification(pop[0].individuals[0], eval_double[0], array_value_class[0]);
	*/
	/**/
	//if (!(mep_parameters.get_use_validation_data() && validation_data.get_num_rows() > 0)) // if no validation data, the test is the best from all
	//	stats[run].prg = pop[best_subpopulation_index].individuals[best_individual_index];
	if (mep_parameters.get_problem_type() != MEP_PROBLEM_MULTICLASS_CLASSIFICATION)
		statistics.get_stat_ptr(run)->best_program.simplify();

	//double num_incorrectly_classified_test;
	//int gene_error_index_test;
	if (test_data.get_num_rows() && test_data.get_num_outputs()) {// has target
		// I must run all solutions for the test data and choose the best one
		switch (mep_parameters.get_problem_type()) {
		case MEP_PROBLEM_REGRESSION:
			if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR) {
				if (statistics.get_stat_ptr(run)->best_program.compute_regression_error_on_double_data(
					test_data.get_data_matrix_double(),
					test_data.get_num_rows(), test_data.get_num_cols() - 1,
					statistics.get_stat_ptr(run)->test_error, mep_absolute_error)) {}
					;
			}
			else {
				if (statistics.get_stat_ptr(run)->best_program.compute_regression_error_on_double_data(
						test_data.get_data_matrix_double(), 
						test_data.get_num_rows(), test_data.get_num_cols() - 1, 
						statistics.get_stat_ptr(run)->test_error, mep_squared_error)){ }
					;
			}
			break;
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			if (statistics.get_stat_ptr(run)->best_program.compute_binary_classification_error_on_double_data(
					test_data.get_data_matrix_double(), 
					test_data.get_num_rows(), test_data.get_num_cols() - 1, 
					statistics.get_stat_ptr(run)->test_error)) {
				statistics.get_stat_ptr(run)->test_num_incorrect = statistics.get_stat_ptr(run)->test_error;
			}
			break;

		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			switch (mep_parameters.get_error_measure()) {
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
				if (statistics.get_stat_ptr(run)->best_program.compute_multi_class_classification_winner_takes_all_fixed_error_on_double_data(
						test_data.get_data_matrix_double(), 
						test_data.get_num_rows(), test_data.get_num_cols() - 1, test_data.get_num_classes(), 
						statistics.get_stat_ptr(run)->test_error)) {
					statistics.get_stat_ptr(run)->test_num_incorrect = statistics.get_stat_ptr(run)->test_error;// num_incorrectly_classified_test;
				}
				break;

			case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
				if (statistics.get_stat_ptr(run)->best_program.compute_multi_class_classification_smooth_error_on_double_data(
						test_data.get_data_matrix_double(), 
						test_data.get_num_rows(), test_data.get_num_cols() - 1, 
						test_data.get_num_classes(), statistics.get_stat_ptr(run)->test_error)) {
					statistics.get_stat_ptr(run)->test_num_incorrect = statistics.get_stat_ptr(run)->test_error;
				}
				break;
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
				if (statistics.get_stat_ptr(run)->best_program.compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data(
						test_data.get_data_matrix_double(), 
						test_data.get_num_rows(), test_data.get_num_cols() - 1, 
						test_data.get_num_classes(), 
						statistics.get_stat_ptr(run)->test_error)) {
					statistics.get_stat_ptr(run)->test_num_incorrect = statistics.get_stat_ptr(run)->test_error;// num_incorrectly_classified_test;
				}

				break;
			}
			break;
		}
	}
	else {
		// does not have target ... here I have to propose the error, class
		// ????????????????????
	}

	delete[] mep_threads;

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;

	statistics.get_stat_ptr(run)->running_time = elapsed_seconds.count();

	return true;
}
//---------------------------------------------------------------------------
void t_mep::get_random_subset(int requested_size, int* indexes, t_seed& seed)
{
	int num_data = training_data.get_num_rows();
	if (num_data == requested_size) {
		// do it faster if you want all data
		for (int i = 0; i < num_data; i++)
			indexes[i] = i;
	}
	else {
		double p_selection = requested_size / (double)num_data;
		int count_real = 0;
		for (int i = 0; i < num_data; i++) {
			double p = mep_real_rand(seed, 0, 1);
			if (p <= p_selection) {
				indexes[count_real] = i;
				count_real++;
				if (count_real == requested_size)
					break;
			}
		}
		while (count_real < requested_size) {
			indexes[count_real] = mep_int_rand(seed, 0, num_data - 1);
			count_real++;
		}
	}
}
//-----------------------------------------------------------------------
int t_mep::to_pugixml_node(pugi::xml_node parent)
{
	// utilized variables

	pugi::xml_node training_node = parent.append_child("training");
	training_data.to_xml(training_node);
	pugi::xml_node validation_node = parent.append_child("validation");
	validation_data.to_xml(validation_node);
	pugi::xml_node testing_node = parent.append_child("test");
	test_data.to_xml(testing_node);

	if (variables_enabled) {

		char* tmp_str = new char[training_data.get_num_cols() * 2 + 10];

		pugi::xml_node utilized_variables_node = parent.append_child("variables_utilization");

		tmp_str[0] = 0;
		for (int v = 0; v < training_data.get_num_cols() - 1; v++) {
			char tmp_s[30];
			sprintf(tmp_s, "%d", variables_enabled[v]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		pugi::xml_node utilized_variables_data_node = utilized_variables_node.append_child(pugi::node_pcdata);
		utilized_variables_data_node.set_value(tmp_str);

		delete[] tmp_str;
	}

	pugi::xml_node parameters_node = parent.append_child("parameters");
	mep_parameters.to_xml(parameters_node);

	pugi::xml_node constants_node = parent.append_child("constants");
	mep_constants.to_xml(constants_node);

	pugi::xml_node operators_node = parent.append_child("operators");
	mep_operators.to_xml(operators_node);

	pugi::xml_node results_node = parent.append_child("results");

	statistics.to_xml(results_node);

	modified_project = false;

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

	training_data.clear_data();
	pugi::xml_node node = parent.child("training");
	if (node) {
		training_data.from_xml(node);
		num_total_variables = training_data.get_num_cols() - 1;
	}
	else
		num_total_variables = 0;

	if (training_data.get_num_rows()) {
		//actual_enabled_variables = new int[num_total_variables];
		variables_enabled = new bool[num_total_variables];

		node = parent.child("variables_utilization");
		if (node) {
			const char* value_as_cstring = node.child_value();
			size_t num_jumped_chars = 0;
			num_actual_variables = 0;
			int i = 0;

			while (*(value_as_cstring + num_jumped_chars)) {
				int int_read;
				sscanf(value_as_cstring + num_jumped_chars, "%d", &int_read);
				variables_enabled[i] = int_read;

				if (variables_enabled[i]) {
					//	actual_enabled_variables[num_actual_variables] = i;
					num_actual_variables++;
				}

				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
				i++;
			}
		}
		else {// not found, use everything
			num_actual_variables = num_total_variables;
			for (int i = 0; i < num_total_variables; i++) {
				variables_enabled[i] = 1;
				//actual_enabled_variables[i] = i;
			}
		}
	}

	node = parent.child("target_col");
	if (node) {
		const char* value_as_cstring = node.child_value();
		target_col = atoi(value_as_cstring);
	}
	else
		target_col = training_data.get_num_cols() - 1;

	validation_data.clear_data();
	node = parent.child("validation");
	if (node)
		validation_data.from_xml(node);

	test_data.clear_data();
	node = parent.child("test");
	if (node) {
		test_data.from_xml(node);
		if (test_data.get_num_cols() < training_data.get_num_cols())
			test_data.set_num_outputs(0);
	}


	node = parent.child("parameters");
	if (node) {
		mep_parameters.from_xml(node);
		if (mep_parameters.get_random_subset_selection_size() == 0) {
			mep_parameters.set_random_subset_selection_size(training_data.get_num_rows());
		}
	}
	else
		mep_parameters.init();

	node = parent.child("operators");
	if (node)
		mep_operators.from_xml(node);
	else
		mep_operators.init();

	node = parent.child("constants");
	if (node)
		mep_constants.from_xml(node);
	else
		mep_constants.init();


	if ((mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION) &&
		training_data.get_num_classes() == 0) {
		training_data.count_num_classes(target_col);
		validation_data.count_num_classes(target_col);
		if (test_data.get_num_cols() == training_data.get_num_cols())
			test_data.count_num_classes(target_col);
	}

	statistics.from_xml(parent, mep_parameters.get_problem_type());

	last_run_index = statistics.num_runs - 1;

	statistics.compute_mean_stddev(mep_parameters.get_use_validation_data() && 
			validation_data.get_num_rows(), test_data.get_num_rows(), 
			mep_parameters.get_problem_type());

	modified_project = false;

	return true;
}
//---------------------------------------------------------------------------
int t_mep::to_xml(const char* filename)
{
	setlocale(LC_NUMERIC, "C");

	pugi::xml_document doc;
	// add node with some name
	pugi::xml_node body = doc.append_child("project");

	pugi::xml_node version_node = body.append_child("version");
	pugi::xml_node data = version_node.append_child(pugi::node_pcdata);
	data.set_value(version);

	pugi::xml_node problem_description_node = body.append_child("problem_description");
	data = problem_description_node.append_child(pugi::node_pcdata);
	data.set_value(problem_description);

	pugi::xml_node alg_node = body.append_child("algorithm");
	to_pugixml_node(alg_node);

	modified_project = false;

	setlocale(LC_NUMERIC, "");

#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
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
	setlocale(LC_NUMERIC, "C");

	pugi::xml_document doc;

	pugi::xml_parse_result result;

#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	result = doc.load_file(w_filename);
	delete[] w_filename;
#else
	result = doc.load_file(filename);
#endif

	if (result.status != pugi::status_ok) {
		setlocale(LC_NUMERIC, "");

		return false;
	}

	pugi::xml_node body_node = doc.child("project");

	if (!body_node) {
		setlocale(LC_NUMERIC, "");

		return false;
	}

	if (problem_description) {
		delete[] problem_description;
		problem_description = NULL;
	}
	pugi::xml_node node = body_node.child("problem_description");
	if (node) {
		const char* value_as_cstring = node.child_value();

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

	if (!alg_node) {
		setlocale(LC_NUMERIC, "");
		return false;
	}

	from_pugixml_node(alg_node);

	setlocale(LC_NUMERIC, "");

	return true;
}
//-----------------------------------------------------------------
void t_mep::compute_list_of_enabled_variables(void)
{
	if (actual_enabled_variables) {
		delete[] actual_enabled_variables;
		actual_enabled_variables = NULL;
	}

	num_actual_variables = 0;
	if (training_data.get_num_cols())
		num_total_variables = training_data.get_num_cols() - 1;
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
		if (last_run_index != -1) {
			modified_project = true;

			last_run_index = -1;
			statistics.delete_memory();
		}
	}
}
//---------------------------------------------------------------------------
char* t_mep::program_as_C(int run_index, bool simplified, double* inputs)
{
	return get_stats_ptr()->get_stat_ptr(run_index)->best_program.to_C_double(
			simplified, inputs, mep_parameters.get_problem_type(), 
			mep_parameters.get_error_measure(), training_data.get_num_classes()
		);
}
//---------------------------------------------------------------------------
char* t_mep::program_as_Excel_function(int run_index, bool simplified, double* inputs)
{
	return get_stats_ptr()->get_stat_ptr(run_index)->best_program.to_Excel_function_double(
		simplified, inputs, mep_parameters.get_problem_type(),
		mep_parameters.get_error_measure(), training_data.get_num_classes()
	);
}
//---------------------------------------------------------------------------
int t_mep::get_num_outputs(void)
{
	return 1;
}
//---------------------------------------------------------------------------
int t_mep::get_num_total_variables(void)
{
	if (!variables_enabled) {
		num_total_variables = training_data.get_num_cols() - 1;
		if (num_total_variables < 0)
			num_total_variables = 0;
		num_actual_variables = num_total_variables;
		
		target_col = training_data.get_num_cols() - 1;

		if (num_total_variables == 0)
			return 0;

		variables_enabled = new bool[num_total_variables];

		for (int i = 0; i < num_total_variables; i++)
			variables_enabled[i] = 1;

		target_col = training_data.get_num_cols() - 1;
	}
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
		mep_parameters.init();
		mep_operators.init();
		mep_constants.init();

		if (actual_enabled_variables) {
			delete[] actual_enabled_variables;
			actual_enabled_variables = NULL;
		}
		if (variables_enabled) {
			delete[] variables_enabled;
			variables_enabled = NULL;
		}

		num_actual_variables = 0;

		if (problem_description) {
			delete[] problem_description;
			problem_description = NULL;
		}

		problem_description = new char[100];
		strcpy(problem_description, "Problem description here ...");

		modified_project = false;
	}
}
//---------------------------------------------------------------------------
void t_mep::init_enabled_variables(void)
{
	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}
	num_total_variables = training_data.get_num_cols() - 1;
	if (num_total_variables < 0) 
		num_total_variables = 0;
	
	num_actual_variables = num_total_variables;

	target_col = training_data.get_num_cols() - 1;

	if (num_total_variables == 0)
		return;

	variables_enabled = new bool[num_total_variables];

	for (int i = 0; i < num_total_variables; i++)
		variables_enabled[i] = 1;
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
	//	long long chromosomes_memory = 0;

	return 0;
}
//---------------------------------------------------------------------------
bool t_mep::validate_project(char* error_message)
{
	if (mep_parameters.get_random_subset_selection_size() > training_data.get_num_rows()) {
		sprintf(error_message, "Random subset size cannot be larger than the number of training data!");
		return false;
	}

	if (mep_parameters.get_random_subset_selection_size() < 1) {
		sprintf(error_message, "Random subset size cannot be less than 1!");
		return false;
	}

	if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION) {
		training_data.count_num_classes(target_col);

		if (mep_parameters.get_code_length() < training_data.get_num_classes()) {
			sprintf(error_message, "Code length cannot be less than the number of classes!");
			return false;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
t_mep_data* t_mep::get_training_data_ptr(void)
{
	return &training_data;
}
//---------------------------------------------------------------------------
t_mep_data* t_mep::get_validation_data_ptr(void)
{
	return &validation_data;

}
//---------------------------------------------------------------------------
t_mep_data* t_mep::get_test_data_ptr(void)
{
	return &test_data;
}
//---------------------------------------------------------------------------
t_mep_functions* t_mep::get_functions_ptr(void)
{
	return &mep_operators;
}
//---------------------------------------------------------------------------
t_mep_constants* t_mep::get_constants_ptr(void)
{
	return &mep_constants;
}
//---------------------------------------------------------------------------
t_mep_parameters* t_mep::get_parameters_ptr(void)
{
	return &mep_parameters;
}
//---------------------------------------------------------------------------
void t_mep::stop(void)
{
	_stopped_signal_sent = true;
}
//---------------------------------------------------------------------------
t_mep_statistics* t_mep::get_stats_ptr(void)
{
	return &statistics;
}
//---------------------------------------------------------------------------
const char* t_mep::get_version(void)
{
	return version;
}
//---------------------------------------------------------------------------