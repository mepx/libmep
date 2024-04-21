// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
#include "mep_functions.h"
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_regression(const t_mep_data &mep_dataset,
		unsigned int *random_subset_indexes,
						unsigned int random_subset_selection_size,
		double** cached_variables_eval_matrix_double,
						long long** cached_variables_eval_matrix_long,
						double ** cached_sum_of_errors,
	unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
		double ** eval_matrix_double, long long** eval_matrix_long,
										  char *gene_used_for_output,
						t_mep_error_function_double mep_error_function_double,
						t_mep_error_function_long mep_error_function_long,
						t_seed &seed)
{
	if (mep_dataset.get_data_type() == MEP_DATA_LONG_LONG)
		fitness_regression_cache_all_training_data(mep_dataset,
			random_subset_indexes, random_subset_selection_size,
			cached_variables_eval_matrix_long, cached_sum_of_errors,
			num_actual_variables, actual_enabled_variables,
			eval_matrix_long, gene_used_for_output,
												   mep_error_function_long, seed);
	else
		fitness_regression_cache_all_training_data(mep_dataset,
			random_subset_indexes, random_subset_selection_size,
			cached_variables_eval_matrix_double, cached_sum_of_errors,
			num_actual_variables, actual_enabled_variables,
			eval_matrix_double, gene_used_for_output,
												   mep_error_function_double, seed);
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_regression_error_on_data_return_error(
	const t_mep_data& mep_data,
	double* error_per_output, double& total_error,
	unsigned int& index_error_gene,
	t_mep_error_function_double mep_error_function_double,
	t_mep_error_function_long mep_error_function_long) const
{
	if (mep_data.get_data_type() == MEP_DATA_LONG_LONG)
		return compute_regression_error_on_data_return_error_long(mep_data,
																  error_per_output,
																  total_error,
																  index_error_gene,
																  mep_error_function_long);
	else
		return compute_regression_error_on_data_return_error_double(mep_data, error_per_output, total_error, index_error_gene, mep_error_function_double);
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_regression_error_on_data(
	const t_mep_data& mep_data,
	double* error_per_output, double& total_error,
	t_mep_error_function_double mep_error_function_double,
	t_mep_error_function_long mep_error_function_long) const
{
	if (mep_data.get_data_type() == MEP_DATA_LONG_LONG)
		return compute_regression_error_on_data_long(mep_data,
													 error_per_output,
													 total_error,
													 mep_error_function_long);
	else
		return compute_regression_error_on_data_double(mep_data,
													   error_per_output,
													   total_error,
													   mep_error_function_double);
}
//---------------------------------------------------------------------------
