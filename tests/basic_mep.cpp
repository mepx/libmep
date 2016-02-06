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

	if (!mep.load_training_data_from_csv("building1.csv")) {
		printf("Cannot load training data! Please make sure that the path to file is correct!");
		printf("Press Enter...");
		getchar();
		return 1;
	}

	mep.set_addition(true);
	mep.set_subtraction(true);
	mep.set_multiplication(true);

	mep.set_num_subpopulations(1);
	mep.set_num_generations(50);
	mep.set_code_length(20);
	mep.set_problem_type(0); // regression

	printf("Evolving ... \n");
	generation_index = 0;
	mep.start(on_generation, NULL, NULL);

	double error = mep.get_best_training_error(0, mep.get_num_generations() - 1);
	
	printf("\nError = %lf\n", error);

	printf("Program = \n%s\n", mep.program_as_C(0, 0, mep.get_training_data_row(0)));

	printf("Press Enter...");
	
	getchar();

	return 0;
}
//-----------------------------------------------------------------