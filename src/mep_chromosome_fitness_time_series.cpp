#include "mep_chromosome.h"

//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_error_on_double_data_return_error(
	double* previous_data, unsigned int window_size,
	double** data, unsigned int num_data, double& error, unsigned int& index_error_gene,
	t_mep_error_function mep_error_function)
{
	error = 0;
	//double actual_output_double[1];

	double* output = new double[num_data];
	char* valid_output = new char[num_data];

	bool result = compute_time_series_output_on_predicted_double_data(
		previous_data, window_size, num_data, index_error_gene,
		output, valid_output);

	if (!result) {
		delete[] output;
		delete[] valid_output;
		return false;
	}

	for (unsigned int k = 0; k < num_data; k++) 
		error += mep_error_function(data[k][0], output[k]);
	
	error /= num_data;

	delete[] output;
	delete[] valid_output;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_error_on_double_data(
	double* previous_data, unsigned int window_size, 
	double** data,	unsigned int num_data,
	double& error, const t_mep_error_function mep_error_function)
{
	error = 0;
//	double actual_output_double[1];
	unsigned int num_valid = 0;
	unsigned int index_error_gene;

	double* output = new double[num_data];
	char* valid_output = new char[num_data];

	compute_time_series_output_on_predicted_double_data(
		previous_data, window_size, num_data, index_error_gene,
		output, valid_output);

	for (unsigned int k = 0; k < num_data; k++) {
		if (valid_output[k]){
			error += mep_error_function(data[k][0], output[k]);
			num_valid++;
		}
		else
			break; // it makes no sense to continue since I cannot compute the next value
	}

	delete[] output;
	delete[] valid_output;

	if (num_valid)
		error /= num_valid;
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_output_on_predicted_double_data(
	double* previous_data, unsigned int window_size,
	unsigned int num_data, unsigned int& index_error_gene,
	double *output, char* valid_output)
{
	double actual_output_double[1];

	//int num_valid = 0;
	for (unsigned int k = 0; k < num_data; k++) {
		valid_output[k] = evaluate_double(previous_data, actual_output_double, index_error_gene);
		if (valid_output[k])
			output[k] = actual_output_double[0];
		else {
			for (unsigned int q = k + 1; q < num_data; q++)
				valid_output[q] = false;
			return false;
		}
	
		// move to next; shift data left
		for (unsigned int i = 0; i < window_size - 1; i++)
			previous_data[i] = previous_data[i + 1];
		previous_data[window_size - 1] = actual_output_double[0];
	}

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_output_on_given_double_data(
	double* previous_data, unsigned int window_size,
	double** data, unsigned int num_data, 
	double* output, char* valid_output)
{
	double actual_output_double[1];

	unsigned int index_error_gene;
	for (unsigned int k = 0; k < num_data; k++) {
		valid_output[k] = evaluate_double(previous_data, actual_output_double, index_error_gene);
		if (valid_output[k])
			output[k] = actual_output_double[0];

		// move to next; shift data left
		for (unsigned int i = 0; i < window_size - 1; i++)
			previous_data[i] = previous_data[i + 1];
		previous_data[window_size - 1] = data[k][0];
	}

	return true;
}
//---------------------------------------------------------------------------
