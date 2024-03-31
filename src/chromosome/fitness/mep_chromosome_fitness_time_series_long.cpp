// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <string.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_error_on_data_return_error(
	long long* previous_data, unsigned int window_size,
	const t_mep_data& mep_data,
	double* error_per_output,
	double& total_error,
	unsigned int& index_error_gene,
	t_mep_error_function_long mep_error_function) const
{
	long long** data = mep_data.get_data_matrix_as_long();
	unsigned int num_data = mep_data.get_num_rows();
	total_error = 0;
	//double actual_output_double[1];

	long long** output = new long long*[num_data];
	for (unsigned int d = 0; d < num_data; d++)
		output[d] = new long long[num_program_outputs];
	char* valid_output = new char[num_data];

	bool result = compute_time_series_output_on_predicted_data(
		previous_data, window_size, num_data, index_error_gene,
		output, valid_output);

	if (!result) {
		delete[] output;
		delete[] valid_output;
		return false;
	}

	for (unsigned int o = 0; o < num_program_outputs; o++){
		error_per_output[o] = 0;
		for (unsigned int k = 0; k < num_data; k++){
			error_per_output[o] += mep_error_function(data[k][o], output[k][o]);
		}
		total_error += error_per_output[o];
	}
	
	total_error /= num_data;

	for (unsigned int d = 0; d < num_data; d++)
		delete[] output[d];
	delete[] output;
	delete[] valid_output;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_error_on_data(
	long long* previous_data, unsigned int window_size,
	const t_mep_data& mep_data,
	double* error_per_output,
	double& total_error,
	const t_mep_error_function_long mep_error_function) const
{
	long long** data = mep_data.get_data_matrix_as_long();
	unsigned int num_data = mep_data.get_num_rows();
	total_error = 0;
//	double actual_output_double[1];
	unsigned int num_valid = 0;
	unsigned int index_error_gene;

	long long** output = new long long*[num_data];
	for (unsigned int d = 0; d < num_data; d++)
		output[d] = new long long[num_program_outputs];
	char* valid_output = new char[num_data];

	compute_time_series_output_on_predicted_data(
		previous_data, window_size, num_data, index_error_gene,
		output, valid_output);
	
	for (unsigned int o = 0; o < num_program_outputs; o++)
		error_per_output[o] = 0;

	for (unsigned int k = 0; k < num_data; k++) {
		if (valid_output[k]){
			for (unsigned int o = 0; o < num_program_outputs; o++)
				error_per_output[o] += mep_error_function(data[k][o], output[k][o]);
			num_valid++;
		}
		else
			break; // it makes no sense to continue since I cannot compute the next value
	}

	for (unsigned int d = 0; d < num_data; d++)
		delete[] output[d];
	delete[] output;
	delete[] valid_output;

	for (unsigned int o = 0; o < num_program_outputs; o++)
		total_error += error_per_output[o];
	
	if (num_valid)
		total_error /= num_valid;
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_output_on_predicted_data(
	long long* previous_data, unsigned int window_size,
	unsigned int num_data, unsigned int& index_error_gene,
	long long **output, char* valid_output) const
{
	long long* actual_output_long = new long long[num_program_outputs];

	//int num_valid = 0;
	for (unsigned int k = 0; k < num_data; k++) {
		valid_output[k] = evaluate(previous_data, actual_output_long,
								   index_error_gene);
		if (valid_output[k])
			memcpy(output[k], actual_output_long,
				   num_program_outputs*sizeof(long long));
		else {
			for (unsigned int q = k + 1; q < num_data; q++)
				valid_output[q] = false;
			delete[] actual_output_long;
			return false;
		}
	
		// move to next; shift data left
		for (unsigned int i = 0; i < (window_size - 1) * num_program_outputs; i++)
			previous_data[i] = previous_data[i + num_program_outputs];
		for (unsigned int c = 0; c < num_program_outputs; c++)
			previous_data[(window_size - 1)* num_program_outputs + c] = actual_output_long[c];

	}
	delete[] actual_output_long;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_time_series_output_on_given_data(
	long long* previous_data, unsigned int window_size,
	long long** data,
	unsigned int num_data,
	long long** output, char* valid_output) const
{
	long long* actual_output_long = new long long[num_program_outputs];

	unsigned int index_error_gene;
	for (unsigned int k = 0; k < num_data; k++) {
		valid_output[k] = evaluate(previous_data, actual_output_long,
								   index_error_gene);
		if (valid_output[k])
			memcpy(output[k], actual_output_long,
				   num_program_outputs * sizeof(long long));

		// move to next; shift data left
		for (unsigned int i = 0; i < (window_size - 1) * num_program_outputs; i++)
			previous_data[i] = previous_data[i + num_program_outputs];
		for (unsigned int c = 0; c < num_program_outputs; c++)
			previous_data[(window_size - 1)* num_program_outputs + c] = actual_output_long[c];
	}
	delete[] actual_output_long;

	return true;
}
//---------------------------------------------------------------------------
