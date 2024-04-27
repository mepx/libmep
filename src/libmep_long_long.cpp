// Multi Expression Programming library
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
//-----------------------------------------------------------------
#ifdef _WIN32
	#include <windows.h>
#endif
//-----------------------------------------------------------------
#include "libmep.h"
//---------------------------------------------------------------------------
bool t_mep::get_output(unsigned int run_index, long long* inputs, long long* outputs) const
{
	unsigned int index_error_gene;
	//unsigned int max_index;
	//double max_value;

	t_mep_chromosome* best_program = &statistics.get_stat_ptr(run_index)->best_program;
//	if (run_index > -1) {
		switch (mep_parameters.get_problem_type()) {
		case MEP_PROBLEM_REGRESSION:
			if (!best_program->evaluate(inputs, outputs, index_error_gene))
				return false;
			break;
		case MEP_PROBLEM_TIME_SERIE:
			if (!best_program->evaluate(inputs, outputs, index_error_gene))
				return false;
			break;
				/*
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			if (!best_program->evaluate(inputs, outputs, index_error_gene))
				return false;

			if (outputs[0] <= best_program->get_best_class_threshold())
				outputs[0] = best_program->get_class_label(0);
			else
				outputs[0] = best_program->get_class_label(1);

			break;
				
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			switch (mep_parameters.get_error_measure()) {
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
			case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
				//max_index = -1;
				if (!best_program->get_first_max_index(inputs, max_index, max_value, index_error_gene, NULL))
					return false;
				outputs[0] = best_program->get_class_label(max_index % training_data.get_num_classes());
				break;
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
				//max_index = -1;
				unsigned int class_index;
				if (!best_program->get_class_index_for_winner_takes_all_dynamic(inputs, class_index))
					return false;
				outputs[0] = best_program->get_class_label(class_index);

				break;
			case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
				
				if (!best_program->evaluate(inputs, outputs, index_error_gene))
					return false;

				outputs[0] = best_program->get_class_label(best_program->get_closest_class_index_from_center(outputs[0]));
				break;
			}// end switch error measure
			break;
				 */
		}// end switch problem type

	return true;
}
//---------------------------------------------------------------------------
void t_mep::compute_cached_eval_matrix_long_long2(s_value_class * array_value_class)
{
	long long** data;

	unsigned int num_rows;

	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE) {
		num_rows = training_data_ts.get_num_rows();
		data = training_data_ts.get_data_matrix_as_long_long();
	}
	else {
		num_rows = training_data.get_num_rows();
		data = training_data.get_data_matrix_as_long_long();
	}

	unsigned int num_problem_outputs = mep_parameters.get_num_outputs();
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_REGRESSION ||
		mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE) {
		for (unsigned int v = 0; v < num_actual_variables; v++) {
			unsigned int actual_variable = actual_enabled_variables[v];
			for (unsigned int o = 0; o < num_problem_outputs; o++)
				cached_sum_of_errors_for_variables[actual_variable][o] = 0;
			if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
				for (unsigned int k = 0; k < num_rows; k++) {
					cached_eval_variables_matrix_long_long[actual_enabled_variables[v]][k] =
							data[k][actual_enabled_variables[v]];
					for (unsigned int o = 0; o < num_problem_outputs; o++)
						cached_sum_of_errors_for_variables[actual_variable][o] +=
							mep_absolute_error_long_long(
									cached_eval_variables_matrix_long_long[actual_enabled_variables[v]][k],
									data[k][num_total_variables + o]);
				}
			else// MEP_REGRESSION_MEAN_SQUARED_ERROR
				for (unsigned int k = 0; k < num_rows; k++) {
					cached_eval_variables_matrix_long_long[actual_enabled_variables[v]][k] =
							data[k][actual_enabled_variables[v]];
					for (unsigned int o = 0; o < num_problem_outputs; o++)
						cached_sum_of_errors_for_variables[actual_variable][o] +=
							mep_squared_error_long_long(cached_eval_variables_matrix_long_long[actual_enabled_variables[v]][k],
									data[k][num_total_variables + o]);
				}

			for (unsigned int o = 0; o < num_problem_outputs; o++)
				cached_sum_of_errors_for_variables[actual_variable][o] /= (double)num_rows;
		}
	}
	else// classification
		for (unsigned int v = 0; v < num_actual_variables; v++) {
			unsigned int actual_variable = actual_enabled_variables[v];

			cached_threashold[actual_variable] = 0;
			for (unsigned int k = 0; k < num_rows; k++) {
				cached_eval_variables_matrix_long_long[actual_variable][k] =
						data[k][actual_enabled_variables[v]];
				array_value_class[k].value = (double)data[k][actual_variable];
				array_value_class[k].class_index = (int)data[k][num_total_variables];
			}
			qsort((void*)array_value_class, num_rows,
					sizeof(s_value_class), sort_function_value_class);

			unsigned int num_0_incorrect = training_data.get_num_items_class_0();
			unsigned int num_1_incorrect = 0;
			cached_threashold[actual_variable] = array_value_class[0].value - 1;// all are classified to class 1 in this case
			cached_sum_of_errors_for_variables[actual_variable][0] = num_0_incorrect;

			for (unsigned int i = 0; i < num_rows; i++) {
				unsigned int j = i + 1;
				while (j < num_rows &&
					   fabs(array_value_class[i].value - array_value_class[j].value) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
					j++;

				// le verific pe toate intre i si j si le cataloghez ca apartinant la clasa 0
				for (unsigned int k = i; k < j; k++)
					if (array_value_class[k].class_index == 0)
						num_0_incorrect--;
					else
						if (array_value_class[k].class_index == 1) {
							//	num_0_incorrect--;
							num_1_incorrect++;
						}
				if (num_0_incorrect + num_1_incorrect < cached_sum_of_errors_for_variables[actual_variable][0]) {
					cached_sum_of_errors_for_variables[actual_variable][0] = num_0_incorrect + num_1_incorrect;
					cached_threashold[actual_variable] = array_value_class[i].value;
				}
				i = j;
				i--;
			}
			cached_sum_of_errors_for_variables[actual_variable][0] /= (double)num_rows;
			cached_sum_of_errors_for_variables[actual_variable][0] *= 100;
		}
}
//---------------------------------------------------------------------------
void t_mep::allocate_values(long long**** eval_matrix_long)
{
	unsigned int num_rows;
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE)
		num_rows = training_data_ts.get_num_rows();
	else
		num_rows = training_data.get_num_rows();

	unsigned int num_threads = mep_parameters.get_num_threads();
	unsigned int code_length = mep_parameters.get_code_length();

	*eval_matrix_long = new long long** [num_threads];
	for (unsigned int c = 0; c < num_threads; c++) {
		(*eval_matrix_long)[c] = new long long* [code_length];
		for (unsigned int i = 0; i < code_length; i++)
			(*eval_matrix_long)[c][i] = new long long[num_rows];
	}

	cached_eval_variables_matrix_long_long = new long long* [num_total_variables];
	for (unsigned int i = 0; i < num_total_variables; i++)
		cached_eval_variables_matrix_long_long[i] = NULL;
	for (unsigned int i = 0; i < num_actual_variables; i++)
		cached_eval_variables_matrix_long_long[actual_enabled_variables[i]] = new long long[num_rows];

	cached_sum_of_errors_for_variables = new double*[num_total_variables];
	for (unsigned int i = 0; i < num_total_variables; i++)
		cached_sum_of_errors_for_variables[i] = new double[mep_parameters.get_num_outputs()];
	
	cached_threashold = new double[num_total_variables];
}
//---------------------------------------------------------------------------
void t_mep::delete_values(long long**** eval_matrix)
{
	if (*eval_matrix) {
		for (unsigned int c = 0; c < mep_parameters.get_num_threads(); c++) {
			for (unsigned int i = 0; i < mep_parameters.get_code_length(); i++)
				delete[](*eval_matrix)[c][i];
			delete[](*eval_matrix)[c];
		}
		delete[] * eval_matrix;
		(*eval_matrix) = NULL;
	}
	if (cached_eval_variables_matrix_long_long) {
		for (unsigned int i = 0; i < num_total_variables; i++)
			delete[] cached_eval_variables_matrix_long_long[i];
		delete[] cached_eval_variables_matrix_long_long;
		cached_eval_variables_matrix_long_long = NULL;
	}

	if (cached_sum_of_errors_for_variables) {
		for (unsigned int i = 0; i < num_total_variables; i++)
			delete[] cached_sum_of_errors_for_variables[i];

		delete[] cached_sum_of_errors_for_variables;
		cached_sum_of_errors_for_variables = NULL;
	}

	if (cached_threashold) {
		delete[] cached_threashold;
		cached_threashold = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep::compute_previous_data_for_time_series_validation(long long *previous_data)
{
	unsigned int window_size = mep_parameters.get_window_size();
	unsigned int num_outputs = mep_parameters.get_num_outputs();

	int num_training_data = training_data.get_num_rows();
	long long** training_matrix = training_data.get_data_matrix_as_long_long();
	for (unsigned int w = 0; w < window_size; w++)
		for (unsigned int c = 0; c < num_outputs; c++)
			previous_data[w * num_outputs + c] = training_matrix[num_training_data - window_size + w][c];
}
//---------------------------------------------------------------------------
void t_mep::compute_previous_data_for_time_series_training(long long* previous_data)
{
	unsigned int window_size = mep_parameters.get_window_size();
	long long** training_matrix = training_data.get_data_matrix_as_long_long();
	unsigned int num_outputs = mep_parameters.get_num_outputs();

	for (unsigned int w = 0; w < window_size; w++)
		for (unsigned int c = 0; c < num_outputs; c++)
		previous_data[w * num_outputs + c] = training_matrix[w][c];
}
//---------------------------------------------------------------------------
void t_mep::compute_previous_data_for_time_series_test(long long* previous_data)
{
	unsigned int window_size = mep_parameters.get_window_size();
	unsigned int num_outputs = mep_parameters.get_num_outputs();

	long long** validation_matrix = validation_data.get_data_matrix_as_long_long();
	unsigned int num_validation_data = validation_data.get_num_rows();

	long long** training_matrix = training_data.get_data_matrix_as_long_long();
	unsigned int num_training_data = training_data.get_num_rows();
	//unsigned int num_training_cols = training_data.get_num_cols();

	unsigned int window_size_from_validation = 0;
	unsigned int window_size_from_training = 0;
	if (window_size > num_validation_data) {
		window_size_from_validation = num_validation_data;
		window_size_from_training = window_size - num_validation_data;
	}
	else {
		window_size_from_validation = window_size;
		window_size_from_training = 0;
	}

	unsigned int index_in_previous_data = window_size - 1;

	for (unsigned int w = 0; w < window_size_from_validation; w++) {
		for (unsigned int c = 0; c < num_outputs; c++)
			previous_data[index_in_previous_data * num_outputs + c] = validation_matrix[num_validation_data - w - 1][c];
		index_in_previous_data--;
	}

	for (unsigned int w = 0; w < window_size_from_training; w++) {
		for (unsigned int c = 0; c < num_outputs; c++)
			previous_data[index_in_previous_data * num_outputs + c] = training_matrix[num_training_data - w - 1][c];
		index_in_previous_data--;
	}
}
//---------------------------------------------------------------------------
void t_mep::compute_previous_data_for_time_series_prediction(long long* previous_data)
{
	unsigned int window_size = mep_parameters.get_window_size();
	unsigned int num_outputs = mep_parameters.get_num_outputs();

	if (test_data.get_num_rows()) {
		long long** test_matrix = test_data.get_data_matrix_as_long_long();
		unsigned int num_test_data = test_data.get_num_rows();

		long long** validation_matrix = validation_data.get_data_matrix_as_long_long();
		unsigned int num_validation_data = validation_data.get_num_rows();

		long long** training_matrix = training_data.get_data_matrix_as_long_long();
		unsigned int num_training_data = training_data.get_num_rows();
		//unsigned int num_training_cols = training_data.get_num_cols();

		unsigned int window_size_from_test = window_size;
		unsigned int window_size_from_validation = 0;
		unsigned int window_size_from_training = 0;
		if (window_size > num_test_data) {
			window_size_from_test = num_test_data;
			window_size_from_validation = window_size - num_test_data;
			if (window_size_from_validation > num_validation_data) {
				window_size_from_validation = num_validation_data;
				window_size_from_training = window_size - num_test_data - num_validation_data;
			}
		}

		unsigned int index_in_previous_data = window_size - 1;
		for (unsigned int w = 0; w < window_size_from_test; w++) {
			for (unsigned int c = 0; c < num_outputs; c++)
				previous_data[index_in_previous_data * num_outputs + c] = test_matrix[num_test_data - w - 1][c];
			index_in_previous_data--;
		}

		for (unsigned int w = 0; w < window_size_from_validation; w++) {
			for (unsigned int c = 0; c < num_outputs; c++)
				previous_data[index_in_previous_data * num_outputs + c] = validation_matrix[num_validation_data - w - 1][c];
			index_in_previous_data--;
		}

		for (unsigned int w = 0; w < window_size_from_training; w++) {
			for (unsigned int c = 0; c < num_outputs; c++)
				previous_data[index_in_previous_data * num_outputs + c] = training_matrix[num_training_data - w - 1][c];
			index_in_previous_data--;
		}
	}
	else
		if (validation_data.get_num_rows()) {
			long long** validation_matrix = validation_data.get_data_matrix_as_long_long();
			unsigned int num_validation_data = validation_data.get_num_rows();
			long long** training_matrix = training_data.get_data_matrix_as_long_long();
			unsigned int num_training_data = training_data.get_num_rows();
			//unsigned int num_training_cols = training_data.get_num_cols();

			unsigned int window_size_from_validation = window_size;
			unsigned int window_size_from_training = 0;
			if (window_size > num_validation_data) {
				window_size_from_validation = num_validation_data;
				window_size_from_training = window_size - num_validation_data;
			}

			unsigned int index_in_previous_data = window_size - 1;
			for (unsigned int w = 0; w < window_size_from_validation; w++) {
				for (unsigned int c = 0; c < num_outputs; c++)
					previous_data[index_in_previous_data * num_outputs + c] = validation_matrix[num_validation_data - w - 1][c];
				index_in_previous_data--;
			}

			for (unsigned int w = 0; w < window_size_from_training; w++) {
				for (unsigned int c = 0; c < num_outputs; c++)
					previous_data[index_in_previous_data * num_outputs + c] = training_matrix[num_training_data - w - 1][c];
				index_in_previous_data--;
			}
		}
		else {// take data from training
			long long** training_matrix = training_data.get_data_matrix_as_long_long();
			unsigned int num_training_data = training_data.get_num_rows();
			for (unsigned int w = 0; w < window_size; w++)
				for (unsigned int c = 0; c < num_outputs; c++)
					previous_data[w * num_outputs + c] = training_matrix[num_training_data - window_size + w][c];

		}
}
//---------------------------------------------------------------------------
void t_mep::compute_output_on_training(int run_index,
									   long long** output,
									   char* valid_output)
{
	if (run_index == -1)
		return;

	switch (mep_parameters.get_problem_type()) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		{
			unsigned int num_data = training_data.get_num_rows();

			for (unsigned int i = 0; i < num_data; i++) {
				valid_output[i] = get_output(run_index,
											 training_data.get_row_as_long_long(i),
											 output[i]);
			}
		}
			break;
		case MEP_PROBLEM_TIME_SERIE:
		{
			unsigned int window_size = mep_parameters.get_window_size();
			unsigned int num_outputs = mep_parameters.get_num_outputs();

			long long* previous_data = new long long[window_size * num_outputs];

			compute_previous_data_for_time_series_training(previous_data);

			long long** training_matrix = training_data.get_data_matrix_as_long_long();
			unsigned int num_training_data = training_data.get_num_rows();

			for (unsigned int w = 0; w < window_size; w++) {
				for (unsigned int c = 0; c < num_outputs; c++)
					output[w][c] = previous_data[w * num_outputs + c];
				valid_output[w] = valid_output_NA; // this means NA (Not Apply)
			}

			statistics.get_stat_ptr(run_index)->best_program.compute_time_series_output_on_given_data(
				previous_data, window_size,
				training_matrix + window_size,
				num_training_data - window_size,
				output + window_size,
				valid_output + window_size);

			delete[] previous_data;

		}
			break;
	}
}
//---------------------------------------------------------------------------
void t_mep::compute_output_on_validation(int run_index, long long** output, char* valid_output)
{
	if (run_index == -1)
		return;

	switch (mep_parameters.get_problem_type()) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
	{
		unsigned int num_data = validation_data.get_num_rows();
		for (unsigned int i = 0; i < num_data; i++) {
			valid_output[i] = get_output(run_index,
										 validation_data.get_row_as_long_long(i),
										 output[i]);
		}
	}
		break;
	case MEP_PROBLEM_TIME_SERIE:
		unsigned int window_size = mep_parameters.get_window_size();
		unsigned int num_outputs = mep_parameters.get_num_outputs();

		long long* previous_data = new long long[window_size * num_outputs];
		long long** validation_matrix = validation_data.get_data_matrix_as_long_long();
		unsigned int num_validation_data = validation_data.get_num_rows();

		compute_previous_data_for_time_series_validation(previous_data);

		statistics.get_stat_ptr(run_index)->best_program.compute_time_series_output_on_given_data(
			previous_data, window_size, validation_matrix, num_validation_data,
			output, valid_output);

		delete[] previous_data;

		break;
	}
}
//---------------------------------------------------------------------------
void t_mep::compute_output_on_test(int run_index, long long** output, char* valid_output)
{
	if (run_index == -1)
		return;

	switch (mep_parameters.get_problem_type()) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
	{
		unsigned int num_data = test_data.get_num_rows();
		//long long output_long[1];
		for (unsigned int i = 0; i < num_data; i++) {
			valid_output[i] = get_output(run_index,
										 test_data.get_row_as_long_long(i),
										 output[i]);
			//if (valid_output[i])
				//output[i] = output_long[0];
		}
	}
		break;
	case MEP_PROBLEM_TIME_SERIE:
		unsigned int window_size = mep_parameters.get_window_size();
		unsigned int num_outputs = mep_parameters.get_num_outputs();

		long long* previous_data = new long long[window_size * num_outputs];

		long long** test_matrix = test_data.get_data_matrix_as_long_long();
		unsigned int num_test_data = test_data.get_num_rows();
		compute_previous_data_for_time_series_test(previous_data);

		//long long output_long[1];
		bool predict_on_test = mep_parameters.get_time_series_mode() == MEP_TIME_SERIES_TEST;

		if (predict_on_test) {
			unsigned int index_error_gene;
			
			statistics.get_stat_ptr(run_index)->best_program.compute_time_series_output_on_predicted_data(
				previous_data, window_size, num_test_data, index_error_gene,
				output, valid_output);
		}
		else {// analyze on test
			statistics.get_stat_ptr(run_index)->best_program.compute_time_series_output_on_given_data(
				previous_data, window_size, test_matrix, num_test_data,
				output, valid_output);
			 }

		delete[] previous_data;

		break;
	}
}
//---------------------------------------------------------------------------
bool t_mep::predict(int run_index, long long** output, char* valid_output)
{
	if (run_index == -1)
		return false;

	if (mep_parameters.get_problem_type() != MEP_PROBLEM_TIME_SERIE)
		return false;

	if (mep_parameters.get_time_series_mode() != MEP_TIME_SERIES_PREDICTION)
		return false;

	unsigned int window_size = mep_parameters.get_window_size();
	unsigned int num_outputs = mep_parameters.get_num_outputs();

	long long* previous_data = new long long[window_size * num_outputs];

	compute_previous_data_for_time_series_prediction(previous_data);

	//long long *tmp_out = new long long[num_outputs];
	unsigned int num_predictions = mep_parameters.get_num_predictions();

	for (unsigned int p = 0; p < num_predictions; p++) {
		if (get_output(run_index, previous_data, output[p])) {
			valid_output[p] = 1;
			for (unsigned int i = 0; i < (window_size - 1) * num_outputs; i++)
				previous_data[i] = previous_data[i + 1];
			for (unsigned int c = 0; c < num_outputs; c++)
				previous_data[(window_size - 1) * num_outputs + c] = output[p][c];
		}
		else {
			// fill the remaining with false
			for (unsigned int q = p; q < num_predictions; q++)
				valid_output[q] = false;
			break;
		}
	}

	delete[] previous_data;
	return true;
}
//---------------------------------------------------------------------------
bool t_mep::predict_on_test(int run_index, long long** output, char* valid_output)
{
	if (run_index == -1)
		return false;

	if (mep_parameters.get_problem_type() != MEP_PROBLEM_TIME_SERIE)
		return false;

	//unsigned int num_cols = training_data.get_num_cols();
	//double* data_row = new double[num_cols - 1];

	unsigned int window_size = mep_parameters.get_window_size();
	unsigned int num_outputs = mep_parameters.get_num_outputs();

	long long* previous_data = new long long[window_size * num_outputs];
	//int num_previous_data;
	//double** previous_matrix;
	if (validation_data.get_num_rows()) {
		long long** validation_matrix = validation_data.get_data_matrix_as_long_long();
		unsigned int num_validation_data = validation_data.get_num_rows();
		long long** training_matrix = training_data.get_data_matrix_as_long_long();
		unsigned int num_training_data = training_data.get_num_rows();
		//unsigned int num_training_cols = training_data.get_num_cols();

		unsigned int window_size_from_validation = window_size;
		unsigned int window_size_from_training = 0;
		if (window_size > num_validation_data) {
			window_size_from_validation = num_validation_data;
			window_size_from_training = window_size - num_validation_data;
		}

		unsigned int index_in_previous_data = window_size - 1;
		for (unsigned int w = 0; w < window_size_from_validation; w++) {
			for (unsigned int c = 0; c < num_outputs; c++)
				previous_data[index_in_previous_data * num_outputs + c] = validation_matrix[num_validation_data - w - 1][c];
			index_in_previous_data--;
		}

		for (unsigned int w = 0; w < window_size_from_training; w++) {
			for (unsigned int c = 0; c < num_outputs; c++)
				previous_data[index_in_previous_data * num_outputs + c] = training_matrix[num_training_data - w - 1][c];
			index_in_previous_data--;
		}
	}
	else {// take data from training
		long long** training_matrix = training_data.get_data_matrix_as_long_long();
		unsigned int num_training_data = training_data.get_num_rows();
		for (unsigned int w = 0; w < window_size; w++)
			for (unsigned int c = 0; c < num_outputs; c++)
				previous_data[w * num_outputs + c] = training_matrix[num_training_data - window_size + w][c];
	}

	unsigned int num_test_data = test_data.get_num_rows();
	unsigned int index_error_gene;
	
	statistics.get_stat_ptr(run_index)->best_program.compute_time_series_output_on_predicted_data(
		previous_data, window_size, num_test_data, index_error_gene,
		output, valid_output);

	delete[] previous_data;
	return true;
}
//---------------------------------------------------------------------------
void t_mep::compute_output_on_all_data_regression_and_classification(bool get_training,
																	 bool get_validation,
																	 bool get_test,
																	 int run_index,
																	 long long** output,
																	 char* valid_output,
																	 long long* min_data,
																	 long long* max_data)
{
	unsigned int num_data = 0;
	unsigned int num_outputs = mep_parameters.get_num_outputs();

	if (get_training) {
		unsigned int local_num_data = training_data.get_num_rows();
		compute_output_on_training(run_index,
								   output + num_data,
								   valid_output + num_data);
		for (unsigned int i = 0; i < local_num_data; i++) {
			if (valid_output[num_data + i] == 1) {
				// update min, max
				for (unsigned int o = 0; o < num_outputs; o++){
					if (min_data[o] > output[num_data + i][o])
						min_data[o] = output[num_data + i][o];
					if (max_data[o] < output[num_data + i][o])
						max_data[o] = output[num_data + i][o];
				}
			}
		}
		num_data += local_num_data;
	}
	if (get_validation) {
		unsigned int local_num_data = validation_data.get_num_rows();
		compute_output_on_validation(run_index, output + num_data, valid_output + num_data);
		for (unsigned int i = 0; i < local_num_data; i++) {
			if (valid_output[num_data + i] == 1) {
				// update min, max
				for (unsigned int o = 0; o < num_outputs; o++){
					if (min_data[o] > output[num_data + i][o])
						min_data[o] = output[num_data + i][o];
					if (max_data[o] < output[num_data + i][o])
						max_data[o] = output[num_data + i][o];
				}
			}
		}
		num_data += local_num_data;
	}
	if (get_test) {
		unsigned int local_num_data = test_data.get_num_rows();
		compute_output_on_test(run_index, output + num_data, valid_output + num_data);
		for (unsigned int i = 0; i < local_num_data; i++) {
			if (valid_output[num_data + i] == 1) {
				// update min, max
				for (unsigned int o = 0; o < num_outputs; o++){
					if (min_data[o] > output[num_data + i][o])
						min_data[o] = output[num_data + i][o];
					if (max_data[o] < output[num_data + i][o])
						max_data[o] = output[num_data + i][o];
				}
			}
		}
		num_data += local_num_data;
	}

}
//---------------------------------------------------------------------------
void t_mep::compute_output_on_all_data_time_series(bool get_training,
												   bool get_validation,
													bool get_test,
												   bool get_predictions,
													int run_index,
												   long long** output,
													   char* valid_output,
												   long long* min_data,
												   long long* max_data)
{
	unsigned int num_data = 0;
	unsigned int num_outputs = mep_parameters.get_num_outputs();
	
	unsigned int window_size = mep_parameters.get_window_size();
	long long *prev_data = new long long[window_size];

	if (get_training) {

		unsigned int local_num_data = training_data.get_num_rows();
		compute_output_on_training(run_index,
								   output + num_data,
								   valid_output + num_data);
		for (unsigned int i = 0; i < local_num_data; i++) {
			if (valid_output[num_data + i] == 1) {
				// update min, max
				for (unsigned int o = 0; o < num_outputs; o++){
					if (min_data[o] > output[num_data + i][o])
						min_data[o] = output[num_data + i][o];
					if (max_data[o] < output[num_data + i][o])
						max_data[o] = output[num_data + i][o];
				}
			}
		}
		num_data += local_num_data;
	}

	if (get_validation) {
	
		unsigned int local_num_data = validation_data.get_num_rows();
		compute_output_on_validation(run_index,
									 output + num_data,
									 valid_output + num_data);
		for (unsigned int i = 0; i < local_num_data; i++) {
			if (valid_output[num_data + i] == 1) {
				//at_least_one_valid = true;
				// update min, max
				for (unsigned int o = 0; o < num_outputs; o++){
					if (min_data[o] > output[num_data + i][o])
						min_data[o] = output[num_data + i][o];
					if (max_data[o] < output[num_data + i][o])
						max_data[o] = output[num_data + i][o];
				}
			}
		}

		num_data += local_num_data;
	}

	if (get_test) {

		unsigned int local_num_data = test_data.get_num_rows();

		compute_output_on_test(run_index,
							   output + num_data,
							   valid_output + num_data);
		for (unsigned int i = 0; i < local_num_data; i++) {
			if (valid_output[num_data + i] == 1) {
				//at_least_one_valid = true;
				// update min, max
				for (unsigned int o = 0; o < num_outputs; o++){
					if (min_data[o] > output[num_data + i][o])
						min_data[o] = output[num_data + i][o];
					if (max_data[o] < output[num_data + i][o])
						max_data[o] = output[num_data + i][o];
				}
			}
		}
		num_data += local_num_data;
	}
	// prediction
	if (get_predictions) {
		unsigned int local_num_data = mep_parameters.get_num_predictions();
		predict(run_index, output + num_data, valid_output + num_data);
		for (unsigned int i = 0; i < local_num_data; i++) {
			if (valid_output[num_data + i] == 1) {
				//at_least_one_valid = true;
				// update min, max
				for (unsigned int o = 0; o < num_outputs; o++){
					if (min_data[o] > output[num_data + i][o])
						min_data[o] = output[num_data + i][o];
					if (max_data[o] < output[num_data + i][o])
						max_data[o] = output[num_data + i][o];
				}
			}
		}

		num_data += local_num_data;
	}

	delete[] prev_data;
}
//---------------------------------------------------------------------
void t_mep::compute_output_on_all_data(bool get_training,
									   bool get_validation,
									bool get_test,
									bool get_predictions,
									int run_index,
									long long** output,
									char* valid_output,
									long long* min_data,
									   long long* max_data)
{
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE)
		compute_output_on_all_data_time_series(get_training,
												get_validation,
												get_test,
												get_predictions,
											   run_index,
											   output, valid_output,
											   min_data, max_data);
	else
		compute_output_on_all_data_regression_and_classification(get_training,
															 get_validation,
												 get_test,
												run_index, output, valid_output,
															 min_data, max_data);
}
//---------------------------------------------------------------------
void t_mep::get_target_for_all_data(bool get_training,
										bool get_validation,
										bool get_test,
										bool get_predictions,
										long long** target,
										char* has_target,
										long long *min_data,
										long long *max_data)
{
	unsigned int num_data = 0;
	unsigned int num_outputs = mep_parameters.get_num_outputs();

	if (get_training) {
		t_mep_data* mep_data_ptr = &training_data;
		unsigned int local_num_data = mep_data_ptr->get_num_rows();
		unsigned int num_cols = mep_data_ptr->get_num_cols();
		for (unsigned int i = 0; i < local_num_data; i++) {
			
			mep_data_ptr->get_range_values_long_long(i, num_cols - num_outputs, num_outputs, target[num_data + i]);
			has_target[num_data + i] = 1;
			
			for (unsigned int o = 0; o < num_outputs; o++){
				if (min_data[o] > target[num_data + i][o])
					min_data[o] = target[num_data + i][o];
				if (max_data[o] < target[num_data + i][o])
					max_data[o] = target[num_data + i][o];
			}
		}
		num_data += local_num_data;
	}

	if (get_validation) {
		t_mep_data* mep_data_ptr = &validation_data;
		unsigned int local_num_data = mep_data_ptr->get_num_rows();
		unsigned int num_cols = mep_data_ptr->get_num_cols();
		for (unsigned int i = 0; i < local_num_data; i++) {
			mep_data_ptr->get_range_values_long_long(i, num_cols - num_outputs,
												num_outputs,
												target[num_data + i]);
			has_target[num_data + i] = 1;

			for (unsigned int o = 0; o < num_outputs; o++){
				if (min_data[o] > target[num_data + i][o])
					min_data[o] = target[num_data + i][o];
				if (max_data[o] < target[num_data + i][o])
					max_data[o] = target[num_data + i][o];
			}
		}
		num_data += local_num_data;
	}

	if (get_test) {
		t_mep_data* mep_data_ptr = &test_data;
		unsigned int local_num_data = mep_data_ptr->get_num_rows();
		unsigned int num_cols = mep_data_ptr->get_num_cols();
		if (test_data.get_num_cols() == training_data.get_num_cols()) {// has target
			for (unsigned int i = 0; i < local_num_data; i++) {
				mep_data_ptr->get_range_values_long_long(i, num_cols - num_outputs,
													num_outputs,
													target[num_data + i]);
				has_target[num_data + i] = 1;

				for (unsigned int o = 0; o < num_outputs; o++){
					if (min_data[o] > target[num_data + i][o])
						min_data[o] = target[num_data + i][o];
					if (max_data[o] < target[num_data + i][o])
						max_data[o] = target[num_data + i][o];
				}
			}
		}
		else {
			for (unsigned int i = 0; i < local_num_data; i++)
				has_target[num_data + i] = 0;
		}
		num_data += local_num_data;
	}
	if (get_predictions) {
		unsigned int local_num_data = mep_parameters.get_num_predictions();
		for (unsigned int i = 0; i < local_num_data; i++)
			has_target[num_data + i] = false;
	}
}
//---------------------------------------------------------------------
