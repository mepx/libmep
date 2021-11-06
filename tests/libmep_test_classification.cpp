// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include "libmep.h"

static int generation_index;
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
	if (!training_data->from_csv("../data/cancer1.csv")) {
		printf("Cannot load training data! Please make sure that the path to file is correct!");
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
	mep_functions->set_iflz(true);

	mep_parameters->set_num_subpopulations(4);// should be multiple of num_threads
	mep_parameters->set_num_generations(50);
	mep_parameters->set_subpopulation_size(100);
	mep_parameters->set_code_length(100);
	mep_parameters->set_problem_type(MEP_PROBLEM_MULTICLASS_CLASSIFICATION);
	mep_parameters->set_error_measure(MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR);
	mep_parameters->set_random_subset_selection_size(training_data->get_num_rows()); // all data are used for training
	mep_parameters->set_num_threads(4);
	mep_parameters->set_mutation_probability(0.01);
	mep_parameters->set_num_runs(1);


	printf("Evolving ... \n");
	generation_index = 0;
	mep.start(on_generation, NULL, NULL);

	double error = mep.get_stats_ptr()->get_best_training_error(0, mep_parameters->get_num_generations() - 1);

	double num_incorrectly_classified_percent = mep.get_stats_ptr()->get_best_training_num_incorrectly_classified(0, mep_parameters->get_num_generations() - 1);
	
	printf("\nFinal Error = %lf\n", error);

	printf("\nFinal Num incorreclty classified = %lf\n", num_incorrectly_classified_percent);

	printf("Program = \n%s\n", mep.program_as_C(0, 0, training_data->get_row(0)));
	
	printf("Press Enter...");
	
	getchar();

	return 0;
}
//-----------------------------------------------------------------