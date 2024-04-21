// Multi Expression Programming library example
//-----------------------------------------------------------------
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
// last update on 2024.04.21
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

	t_mep_data* training_data = mep.get_training_data_ptr(); // store objects in some local variables for easier access
	t_mep_functions* mep_functions = mep.get_functions_ptr();
	t_mep_parameters* mep_parameters = mep.get_parameters_ptr();
	//t_mep_constants* mep_constants = mep.get_constants_ptr();

// take the input data from the https://github.com/mepx/libmep/tree/master/data folder

	if (!training_data->from_tabular_file("building1.csv", ' ', '.')) {
		printf("Cannot load training data! Please make sure that the path to file is correct!");
		printf("Press Enter...");
		getchar();
		return 1;
	}

	mep.init_enabled_variables();

	mep_functions->set_addition(true);
	mep_functions->set_subtraction(true);
	mep_functions->set_multiplication(true);
	mep_functions->set_division(true);

	mep_parameters->set_num_subpopulations(2);
	mep_parameters->set_subpopulation_size(200);
	mep_parameters->set_num_generations(100);
	mep_parameters->set_code_length(50);
	mep_parameters->set_problem_type(MEP_PROBLEM_REGRESSION);
	mep_parameters->set_random_subset_selection_size_percent(100);
	mep_parameters->set_num_threads(2);
	mep_parameters->set_error_measure(MEP_REGRESSION_MEAN_ABSOLUTE_ERROR);
	mep_parameters->set_num_runs(1);
	
	printf("Evolving ... \n");
	generation_index = 0;
	mep.start(on_generation, NULL, NULL);

	double error = mep.get_stats_ptr()->get_best_training_error(0, mep_parameters->get_num_generations() - 1);

	printf("\nFinal error = %lf\n", error);

	printf("Program = \n%s\n", mep.program_as_C(0, true, training_data->get_data_matrix_as_double(), NULL));

	printf("Press Enter...");
	
	getchar();

	return 0;
}
//-----------------------------------------------------------------
