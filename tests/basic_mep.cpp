#include "libmep.h"

//-----------------------------------------------------------------
int main(void)
{
	t_mep mep;

	if (!mep.load_training_data_from_csv("building1.csv")) {
		printf("Cannot load training data! Please make sure that the path to file is correct!");
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
	
	mep.start(NULL, NULL, NULL);

	double error = mep.get_best_training_error(0, mep.get_num_generations() - 1);
	
	printf("Error = %lf\n", error);

	printf("Program = \n%s\n", mep.program_as_C(0, 0, mep.get_training_data_row(0)));

	printf("Press Enter...");
	
	getchar();

	return 0;
}
//-----------------------------------------------------------------