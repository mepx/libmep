// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
// last update on 2024.04.16
//-----------------------------------------------------------------
// Shows how to discover the Fibonacci formula and to predict next 10 values of it
//-----------------------------------------------------------------
#include "libmep.h"
//-----------------------------------------------------------------
static int generation_index;
t_mep mep;
//-----------------------------------------------------------------
void on_generation(void)
{
	double error = mep.get_stats_ptr()->get_best_training_error(0, generation_index);
	printf("Generation %d; Best error = %lf\n", generation_index, error);
	generation_index++;
}
//-----------------------------------------------------------------
int main(void)
{
	printf("libmep version = %s\n", mep.get_version());

	t_mep_parameters* mep_parameters = mep.get_parameters_ptr();
	mep_parameters->set_data_type(MEP_DATA_LONG_LONG);

	t_mep_data* training_data = mep.get_training_data_ptr(); // store objects in some local variables for easier access

	t_mep_functions* mep_functions = mep.get_functions_ptr();

	//t_mep_constants* mep_constants = mep.get_constants_ptr();

// take the input data from the https://github.com/mepx/libmep/tree/master/data folder

	if (!training_data->from_csv_file("data/fibonacci.txt", ' ', '.')) {
		printf("Cannot load training data! Please make sure that the path to file is correct!");
		printf("Press Enter...");
		getchar();
		return 1;
	}

	training_data->set_data_type(MEP_DATA_LONG_LONG); // must convert them to LONG LONG

	mep.init_enabled_variables();

	mep_functions->set_addition(true);
	mep_functions->set_subtraction(true);
	mep_functions->set_multiplication(true);
	mep_functions->set_division(true);

	mep_parameters->set_num_subpopulations(2);
	mep_parameters->set_subpopulation_size(10);
	mep_parameters->set_num_generations(10);
	mep_parameters->set_code_length(10);
	mep_parameters->set_problem_type(MEP_PROBLEM_TIME_SERIE);
	mep_parameters->set_time_series_mode(MEP_TIME_SERIES_PREDICTION); // to predict future values; NOT to predict on test data
	mep_parameters->set_window_size(2);
	mep_parameters->set_random_subset_selection_size_percent(100);
	mep_parameters->set_num_threads(2);
	mep_parameters->set_error_measure(MEP_REGRESSION_MEAN_ABSOLUTE_ERROR);
	mep_parameters->set_num_runs(1);
	
	printf("Training / Evolving ... \n");
	generation_index = 0;
	mep.start(on_generation, NULL, NULL);

	double error = mep.get_stats_ptr()->get_best_training_error(0, mep_parameters->get_num_generations() - 1);

	printf("\nFinal error = %lf\n", error);

	printf("Program = \n%s\n", mep.program_as_C(0, 
												true,
												NULL, // no double data
												training_data->get_data_matrix_as_long())
		);

	// lets predict 10 future values
#define NUM_PREDICTIONS 10
	mep_parameters->set_num_predictions(NUM_PREDICTIONS);
// allocate some memory
	long long *predictions[NUM_PREDICTIONS];
	for (int i = 0; i < NUM_PREDICTIONS; i++)
		predictions[i] = new long long [1];
	char valid_output[NUM_PREDICTIONS];
	
	// predict them with the formula from run 0
	mep.predict(0, (long long**)predictions, valid_output);
	printf("Future predictions:\n");
	for (int i = 0; i < NUM_PREDICTIONS; i++)
		printf("%lld\n", predictions[i][0]); // asume all predictions are valid numbers; otherwise check valid_output vector
	
// delete memory
	for (int i = 0; i < NUM_PREDICTIONS; i++)
		delete[] predictions[i];

	// done
	printf("Press Enter...");
	
	getchar();

	return 0;
}
//-----------------------------------------------------------------
