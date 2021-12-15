#include "mep_chromosome.h"

//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_error_on_double_data_return_error(
	double* previous_data, unsigned int window_size,
	double** data, unsigned int num_data, double& error, unsigned int& index_error_gene,
	t_mep_error_function mep_error_function)
{
	error = 0;
	double actual_output_double[1];

	int num_valid = 0;
	for (unsigned int k = 0; k < num_data; k++) {
		if (evaluate_double(previous_data, actual_output_double, index_error_gene)) {
			error += mep_error_function(data[k][0], actual_output_double[0]);
			num_valid++;
		}
		else
			return false;
		// move to next; shift data left
		for (unsigned int i = 0; i < window_size - 1; i++)
			previous_data[i] = previous_data[i + 1];
		previous_data[window_size - 1] = actual_output_double[0];
	}

	error /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_error_on_double_data(
	double* previous_data, unsigned int window_size, 
	double** data,	unsigned int num_data,
	double& error, const t_mep_error_function mep_error_function)
{
	error = 0;
	double actual_output_double[1];
	unsigned int num_valid = 0;
	unsigned int index_error_gene;
	for (unsigned int k = 0; k < num_data; k++) {
		if (evaluate_double(previous_data, actual_output_double, index_error_gene)) {
			error += mep_error_function(data[k][0], actual_output_double[0]);
			num_valid++;

			// move to next; shift data left
			for (unsigned int i = 0; i < window_size - 1; i++)
				previous_data[i] = previous_data[i + 1];
			previous_data[window_size - 1] = actual_output_double[0];
		}
		else
			break; // it makes no sense to continue since I cannot compute the next value
	}
	if (num_valid)
		error /= num_valid;
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
