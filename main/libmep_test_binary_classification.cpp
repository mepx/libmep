// Multi Expression Programming library example
//-----------------------------------------------------------------
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
// last update on 2024.4.21
//-----------------------------------------------------------------
#include "libmep.h"
//-----------------------------------------------------------------
static unsigned int generation_index;
t_mep mep;
//-----------------------------------------------------------------
void on_generation(void)
{
	printf("Generation %d fitness = %lf num incorrectly classified percent = %lf\n", generation_index, mep.get_stats_ptr()->get_best_training_error(0, generation_index), mep.get_stats_ptr()->get_best_training_num_incorrectly_classified(0, generation_index));
	generation_index++;
}
//-----------------------------------------------------------------
int main(void)
{
	printf("libmep version = %s\n", mep.get_version());

	t_mep_data* training_data = mep.get_training_data_ptr(); // store objects in some local variables for easier access
	t_mep_functions* mep_functions = mep.get_functions_ptr();
	t_mep_parameters* mep_parameters = mep.get_parameters_ptr();
	//t_mep_constants* mep_constants = mep.get_constants_ptr();
	
	printf("Loading data ...\n");
	// sample input file taken from the https://github.com/mepx/libmep/tree/master/data folder
	//if (!training_data->from_csv_file("cancer1.csv", ' ', '.')) {
	if (!training_data->from_tabular_file("cancer1_output1-1.csv", ';', '.')) {
		printf("Cannot load training data! Please make sure that the path to file is correct!\n");
		printf("Press Enter...");
		getchar();
		return 1;
	}
	else
		printf("Loading data done\n");

	
	mep.init_enabled_variables();

	mep_functions->set_addition(true);
	mep_functions->set_subtraction(true);
	mep_functions->set_multiplication(true);
	mep_functions->set_division(true);
	mep_functions->set_sin(true);
	mep_functions->set_iflz(true);

	mep_parameters->set_num_subpopulations(4);// should be multiple of num_threads
	mep_parameters->set_num_generations(20);
	mep_parameters->set_subpopulation_size(200);
	mep_parameters->set_code_length(100);
	mep_parameters->set_problem_type(MEP_PROBLEM_BINARY_CLASSIFICATION);
	mep_parameters->set_error_measure(MEP_BINARY_CLASSIFICATION_AUTOMATIC_THRESHOLD);
	mep_parameters->set_random_subset_selection_size_percent(100); // all data are used for training
	mep_parameters->set_num_threads(4);
	mep_parameters->set_mutation_probability(0.005);
	mep_parameters->set_num_runs(1);


	printf("Evolving ... \n");
	generation_index = 0;
	mep.start(on_generation, NULL, NULL);

	double error = mep.get_stats_ptr()->get_best_training_error(0, mep_parameters->get_num_generations() - 1);

	double num_incorrectly_classified_percent = mep.get_stats_ptr()->get_best_training_num_incorrectly_classified(0, mep_parameters->get_num_generations() - 1);
	
	printf("\nFinal Error = %lf\n", error);

	printf("\nFinal Num incorrectly classified = %lf\n", num_incorrectly_classified_percent);

	printf("Program = \n%s\n", mep.program_as_C(0, true, training_data->get_data_matrix_as_double(), NULL));
	
	printf("Press Enter...");
	
	getchar();

	return 0;
}
//-----------------------------------------------------------------
