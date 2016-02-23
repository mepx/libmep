#include "libmep.h"

static int generation_index;
//-----------------------------------------------------------------
void on_generation(void)
{
	printf("Generation %d - done\n", generation_index);
	generation_index++;
}
//-----------------------------------------------------------------
int main(void)
{
	t_mep mep;
	t_mep_data training_data;
	t_mep_operators mep_operators;
	t_mep_parameters mep_parameters;
	t_mep_constants mep_constants;

	if (!training_data.from_csv("building1.csv")) {
		printf("Cannot load training data! Please make sure that the path to file is correct!");
		printf("Press Enter...");
		getchar();
		return 1;
	}

	mep.set_training_data(&training_data);
	mep_operators.set_addition(true);
	mep_operators.set_subtraction(true);
	mep_operators.set_multiplication(true);

	mep.set_operators(&mep_operators);

	mep_parameters.set_num_subpopulations(1);
	mep_parameters.set_num_generations(50);
	mep_parameters.set_code_length(20);
	mep_parameters.set_problem_type(0); // regression

	mep.set_parameters(&mep_parameters);
	mep.set_constants(&mep_constants);

	printf("Evolving ... \n");
	generation_index = 0;
	mep.start(on_generation, NULL, NULL);

	double error = mep.get_best_training_error(0, mep_parameters.get_num_generations() - 1);
	
	printf("\nError = %lf\n", error);

	printf("Program = \n%s\n", mep.program_as_C(0, 0, training_data.get_row(0)));

	printf("Press Enter...");
	
	getchar();

	return 0;
}
//-----------------------------------------------------------------