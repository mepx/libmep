
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "mep_rands.h"
#include "libmep.h"

#include <errno.h>


#ifdef WIN32
#include <windows.h>
#endif



//---------------------------------------------------------------------------
t_mep::t_mep()
{
	strcpy(version, "2016.02.27.1-beta");

	num_operators = 0;
	

	cached_eval_matrix_double = NULL;
	cached_sum_of_errors = NULL;

	stats = NULL;
	modified_project = false;
	_stopped = true;
	last_run_index = -1;

	variables_enabled = NULL;
	actual_enabled_variables = NULL;
	num_actual_variables = 0;
	num_total_variables = 0;
	target_col = -1;
	cache_results_for_all_training_data = true;

	problem_description = new char[100];
	strcpy(problem_description, "Problem description here ...");

	training_data = NULL;
	validation_data = NULL;
	test_data = NULL;
}
//---------------------------------------------------------------------------
t_mep::~t_mep()
{
	if (actual_enabled_variables) {
		delete[] actual_enabled_variables;
		actual_enabled_variables = NULL;
	}
	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}

	if (stats) {
		delete[] stats;
		stats = NULL;
	}

	if (problem_description) {
		delete[] problem_description;
		problem_description = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep::allocate_values(double ****eval_double, s_value_class ***array_value_class)
{
	*eval_double = new double**[mep_parameters->get_num_threads()];
	for (int c = 0; c < mep_parameters->get_num_threads(); c++) {
		(*eval_double)[c] = new double*[mep_parameters->get_code_length()];
		for (int i = 0; i < mep_parameters->get_code_length(); i++)
			(*eval_double)[c][i] = new double[training_data->get_num_rows()];
	}

	cached_eval_matrix_double = new double*[num_total_variables];
	for (int i = 0; i < num_total_variables; i++)
		cached_eval_matrix_double[i] = NULL;
	for (int i = 0; i < num_actual_variables; i++)
		cached_eval_matrix_double[actual_enabled_variables[i]] = new double[training_data->get_num_rows()];

	cached_sum_of_errors = new double[num_total_variables];
	cached_threashold = new double[num_total_variables];

	*array_value_class = new s_value_class*[mep_parameters->get_num_threads()];
	for (int c = 0; c < mep_parameters->get_num_threads(); c++)
		(*array_value_class)[c] = new s_value_class[training_data->get_num_rows()];
}
//---------------------------------------------------------------------------
void t_mep::allocate_sub_population(t_sub_population &pop)
{
	pop.offspring1.allocate_memory(mep_parameters->get_code_length(), num_total_variables, mep_parameters->get_constants_probability() > 1E-6, mep_constants);
	pop.offspring2.allocate_memory(mep_parameters->get_code_length(), num_total_variables, mep_parameters->get_constants_probability() > 1E-6, mep_constants);
	pop.individuals = new t_mep_chromosome[mep_parameters->get_subpopulation_size()];
	for (int j = 0; j < mep_parameters->get_subpopulation_size(); j++)
		pop.individuals[j].allocate_memory(mep_parameters->get_code_length(), num_total_variables, mep_parameters->get_constants_probability() > 1E-6, mep_constants);
}
//---------------------------------------------------------------------------
void t_mep::get_best(t_mep_chromosome& dest)
{
	dest = pop[best_subpopulation_index].individuals[best_individual_index];
}
//---------------------------------------------------------------------------
void t_mep::generate_random_individuals(void) // randomly initializes the individuals
{
	for (int i = 0; i < mep_parameters->get_num_subpopulations(); i++)
		for (int j = 0; j < mep_parameters->get_subpopulation_size(); j++)
			pop[i].individuals[j].generate_random(mep_parameters, mep_constants, actual_operators, num_operators, actual_enabled_variables, num_actual_variables);
}
//---------------------------------------------------------------------------
bool t_mep::get_output(int run_index, double *inputs, double *outputs)
{
	if (run_index > -1) {
		if (!stats[run_index].prg.evaluate_double(inputs, outputs))
			return false;
		if (mep_parameters->get_problem_type() == PROBLEM_CLASSIFICATION) {
			if (outputs[0] <= stats[run_index].prg.get_best_class_threshold())
				outputs[0] = 0;
			else
				outputs[0] = 1;
		}
	}
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
void t_mep::compute_cached_eval_matrix_double(void)
{
	/*
	if (mep_parameters->get_problem_type() == PROBLEM_REGRESSION)
	for (int v = 0; v < training_data.num_vars; v++) {
	cached_sum_of_errors[v] = 0;
	for (int k = 0; k < training_data->get_num_rows(); k++) {
	cached_eval_matrix_double[v][k] = training_data->_data_double[k][v];
	cached_sum_of_errors[v] += fabs(cached_eval_matrix_double[v][k] - training_data._target_double[k]);
	}
	}
	else
	for (int v = 0; v < training_data.num_vars; v++) {
	cached_sum_of_errors[v] = 0;
	for (int k = 0; k < training_data->get_num_rows(); k++) {
	cached_eval_matrix_double[v][k] = training_data->_data_double[k][v];
	if (cached_eval_matrix_double[v][k] <= mep_parameters->classification_threshold)
	cached_sum_of_errors[v] += training_data._target_double[k];
	else
	cached_sum_of_errors[v] += 1 - training_data._target_double[k];
	}
	}
	*/
}
//---------------------------------------------------------------------------
void t_mep::compute_cached_eval_matrix_double2(s_value_class *array_value_class)
{
	double **data = training_data->get_data_matrix_double();

	if (mep_parameters->get_problem_type() == PROBLEM_REGRESSION)
		for (int v = 0; v < num_actual_variables; v++) {
			cached_sum_of_errors[actual_enabled_variables[v]] = 0;
			for (int k = 0; k < training_data->get_num_rows(); k++) {
				cached_eval_matrix_double[actual_enabled_variables[v]][k] = data[k][actual_enabled_variables[v]];
				cached_sum_of_errors[actual_enabled_variables[v]] += fabs(cached_eval_matrix_double[actual_enabled_variables[v]][k] - data[k][num_total_variables]);
			}
		}
	else
		for (int v = 0; v < num_actual_variables; v++) {

			cached_threashold[actual_enabled_variables[v]] = 0;
			for (int k = 0; k < training_data->get_num_rows(); k++) {
				cached_eval_matrix_double[actual_enabled_variables[v]][k] = data[k][actual_enabled_variables[v]];
				array_value_class[k].value = data[k][actual_enabled_variables[v]];
				array_value_class[k].data_class = (int)data[k][num_total_variables];
			}
			qsort((void*)array_value_class, training_data->get_num_rows(), sizeof(s_value_class), sort_function_value_class);

			int num_0_incorrect = training_data->get_num_items_class_0();
			int num_1_incorrect = 0;
			cached_threashold[actual_enabled_variables[v]] = array_value_class[0].value - 1;// all are classified to class 1 in this case
			cached_sum_of_errors[actual_enabled_variables[v]] = num_0_incorrect;

			for (int i = 0; i < training_data->get_num_rows(); i++) {
				int j = i + 1;
				//while (j < training_data->get_num_rows() && fabs(cached_eval_matrix_double[actual_enabled_variables[v]][i] - cached_eval_matrix_double[actual_enabled_variables[v]][j]) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
				while (j < training_data->get_num_rows() && fabs(array_value_class[i].value - array_value_class[j].value) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
					j++;

				// le verific pe toate intre i si j si le cataloghez ca apartinant la clasa 0
				for (int k = i; k < j; k++)
					if (array_value_class[k].data_class == 0)
						num_0_incorrect--;
					else
						if (array_value_class[k].data_class == 1) {
							//	num_0_incorrect--;
							num_1_incorrect++;
						}
				if (num_0_incorrect + num_1_incorrect < cached_sum_of_errors[actual_enabled_variables[v]]) {
					cached_sum_of_errors[actual_enabled_variables[v]] = num_0_incorrect + num_1_incorrect;
					cached_threashold[actual_enabled_variables[v]] = array_value_class[i].value;
				}
				i = j;
				i--;
			}
		}
}
//---------------------------------------------------------------------------
/*
void t_mep::compute_eval_vector_double(t_mep_chromosome &)
{
}
*/
//---------------------------------------------------------------------------
int sort_function_chromosomes(const void *a, const void *b)
{
	return ((t_mep_chromosome *)a)->compare((t_mep_chromosome *)b, false);
}
//---------------------------------------------------------------------------
void t_mep::sort_by_fitness(t_sub_population &pop) // sort ascending the individuals in population
{
	qsort((void *)pop.individuals, mep_parameters->get_subpopulation_size(), sizeof(pop.individuals[0]), sort_function_chromosomes);
}
//---------------------------------------------------------------------------
void t_mep::delete_sub_population(t_sub_population &pop)
{
	if (pop.individuals)
		delete[] pop.individuals;

	pop.offspring1.clear();
	pop.offspring2.clear();
}
//---------------------------------------------------------------------------
void t_mep::delete_values(double ****eval_double, s_value_class ***array_value_class)
{
	if (*eval_double) {
		for (int c = 0; c < mep_parameters->get_num_threads(); c++) {
			for (int i = 0; i < mep_parameters->get_code_length(); i++)
				delete[](*eval_double)[c][i];
			delete[](*eval_double)[c];
		}
		delete[] * eval_double;
		(*eval_double) = NULL;
	}
	if (cached_eval_matrix_double) {
		for (int i = 0; i < num_total_variables; i++)
			delete[] cached_eval_matrix_double[i];
		delete[] cached_eval_matrix_double;
		cached_eval_matrix_double = NULL;
	}

	if (cached_sum_of_errors) {
		delete[] cached_sum_of_errors;
		cached_sum_of_errors = NULL;
	}

	if (cached_threashold) {
		delete[] cached_threashold;
		cached_threashold = NULL;
	}

	if (*array_value_class) {
		for (int c = 0; c < mep_parameters->get_num_threads(); c++)
			delete[](*array_value_class)[c];
		delete[] * array_value_class;
		*array_value_class = NULL;
	}
}
//---------------------------------------------------------------------------
long t_mep::tournament(t_sub_population &pop)     // Size is the size of the tournament
{
	long r, p;
	p = my_rand() % mep_parameters->get_subpopulation_size();
	for (int i = 1; i < mep_parameters->get_tournament_size(); i++) {
		r = my_rand() % mep_parameters->get_subpopulation_size();
		p = pop.individuals[r].get_fitness() < pop.individuals[p].get_fitness() ? r : p;
	}
	return p;
}
//---------------------------------------------------------------------------
void t_mep::compute_best_and_average_error(double &best_error, double &mean_error)
{
	mean_error = 0;
	best_error = pop[0].individuals[0].get_fitness();
	best_individual_index = 0;
	best_subpopulation_index = 0;
	for (int i = 0; i < mep_parameters->get_num_subpopulations(); i++)
		if (best_error > pop[i].individuals[0].get_fitness()) {
			best_error = pop[i].individuals[0].get_fitness();
			best_individual_index = 0;
			best_subpopulation_index = i;
		}

	for (int i = 0; i < mep_parameters->get_num_subpopulations(); i++)
		for (int k = 0; k < mep_parameters->get_subpopulation_size(); k++)
			mean_error += pop[i].individuals[k].get_fitness();

	mean_error /= mep_parameters->get_num_subpopulations() * mep_parameters->get_subpopulation_size();
}
//---------------------------------------------------------------------------
double t_mep::compute_validation_error(int *best_subpopulation_index_for_validation, int *best_individual_index_for_validation, double **eval_double, s_value_class *tmp_value_class)
{
	double best_validation_error = -1;
	double validation_error;

	if (mep_parameters->get_problem_type() == PROBLEM_REGRESSION) {
		for (int k = 0; k < mep_parameters->get_num_subpopulations(); k++) {
			while (!pop[k].individuals[0].compute_regression_error_on_double_data_return_error(validation_data->get_data_matrix_double(), validation_data->get_num_rows(), validation_data->get_num_cols() - 1, &validation_error)) {
				// I have to mutate that a_chromosome.
				pop[k].individuals[0].set_gene_operation(pop[k].individuals[0].get_index_best_gene(), actual_enabled_variables[my_rand() % num_actual_variables]);
				// recompute its fitness on training;
				pop[k].individuals[0].fitness_regression(training_data, cached_eval_matrix_double, cached_sum_of_errors, num_actual_variables, actual_enabled_variables, eval_double);
				// resort the population
				sort_by_fitness(pop[k]);
				// apply it again on validation
			}
			// now it is ok; no errors on
			if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
				best_validation_error = validation_error;
				*best_subpopulation_index_for_validation = k;
				*best_individual_index_for_validation = 0;
			}
		}
	}
	else
		if (mep_parameters->get_problem_type() == PROBLEM_CLASSIFICATION)
			for (int k = 0; k < mep_parameters->get_num_subpopulations(); k++) {
				while (!pop[k].individuals[0].compute_classification_error_on_double_data_return_error(validation_data->get_data_matrix_double(), validation_data->get_num_rows(), validation_data->get_num_cols() - 1, &validation_error)) {
					pop[k].individuals[0].set_gene_operation(pop[k].individuals[0].get_index_best_gene(), actual_enabled_variables[my_rand() % num_actual_variables]);
					// recompute its fitness on training;
					pop[k].individuals[0].fitness_classification(training_data, cached_eval_matrix_double, cached_sum_of_errors, cached_threashold, num_actual_variables, actual_enabled_variables, eval_double, tmp_value_class);
					// resort the population
					sort_by_fitness(pop[k]);
				}
				if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
					best_validation_error = validation_error;
					*best_subpopulation_index_for_validation = k;
					*best_individual_index_for_validation = 0;
				}
			}

	return best_validation_error;
}
//---------------------------------------------------------------------------
int t_mep::start(f_on_progress on_generation, f_on_progress on_new_evaluation, f_on_progress on_complete_run)
{
	_stopped = false;

	compute_list_of_enabled_variables();

	pop = new t_sub_population[mep_parameters->get_num_subpopulations()];
	for (int i = 0; i < mep_parameters->get_num_subpopulations(); i++)
		allocate_sub_population(pop[i]);

	num_operators = operators->get_list_of_operators(actual_operators);

	double ***eval_double;           // an array where the values of each expression are stored

	s_value_class **array_value_class;

	allocate_values(&eval_double, &array_value_class);

	if (mep_parameters->get_problem_type() == PROBLEM_CLASSIFICATION)
		training_data->count_0_class(target_col);

	compute_cached_eval_matrix_double2(array_value_class[0]);

	stats = new t_mep_statistics[mep_parameters->get_num_runs()];
	for (int run_index = 0; run_index < mep_parameters->get_num_runs(); run_index++) {
		stats[run_index].allocate(mep_parameters->get_num_generations());
		last_run_index++;
		start_steady_state(run_index, eval_double, array_value_class, on_generation, on_new_evaluation);
		if (on_complete_run)
			on_complete_run();
		if (_stopped)
			break;
	}

	delete_values(&eval_double, &array_value_class);
	for (int i = 0; i < mep_parameters->get_num_subpopulations(); i++)
		delete_sub_population(pop[i]);
	delete[] pop;

	_stopped = true;
	return true;
}
//---------------------------------------------------------------------------
void t_mep::evolve_one_subpopulation_for_one_generation(int *current_subpop_index, std::mutex* mutex, t_sub_population * sub_populations, int generation_index, double ** eval_double, s_value_class *tmp_value_class)
{
	int pop_index = 0;
	while (*current_subpop_index < mep_parameters->get_num_subpopulations()) {
		// still more subpopulations to evolve?

		while (!mutex->try_lock()) {}// create a lock so that multiple threads will not evolve the same sub population
		pop_index = *current_subpop_index;
		(*current_subpop_index)++;
		mutex->unlock();

		// pop_index is the index of the subpopulation evolved by the current thread

		t_sub_population *a_sub_population = &sub_populations[pop_index];

		if (generation_index == 0) {
			if (mep_parameters->get_problem_type() == PROBLEM_REGRESSION)
				for (int i = 0; i < mep_parameters->get_subpopulation_size(); i++)
					pop[pop_index].individuals[i].fitness_regression(training_data, cached_eval_matrix_double, cached_sum_of_errors, num_actual_variables, actual_enabled_variables, eval_double);
			else
				if (mep_parameters->get_problem_type() == PROBLEM_CLASSIFICATION)
					for (int i = 0; i < mep_parameters->get_subpopulation_size(); i++)
						pop[pop_index].individuals[i].fitness_classification(training_data, cached_eval_matrix_double, cached_sum_of_errors, cached_threashold, num_actual_variables, actual_enabled_variables, eval_double, tmp_value_class);

			sort_by_fitness(pop[pop_index]);
		}
		else // other generations, after initial
			for (int k = 0; k < mep_parameters->get_subpopulation_size(); k += 2) {
				// choose the parents using binary tournament
				long r1 = tournament(*a_sub_population);
				long r2 = tournament(*a_sub_population);
				// crossover
				double p = my_rand() / (RAND_MAX + 1.0);
				if (p < mep_parameters->get_crossover_probability())
					if (mep_parameters->get_crossover_type() == UNIFORM_CROSSOVER)
						a_sub_population->individuals[r1].uniform_crossover(a_sub_population->individuals[r2], a_sub_population->offspring1, a_sub_population->offspring2, mep_parameters, mep_constants);
					else
						a_sub_population->individuals[r1].one_cut_point_crossover(a_sub_population->individuals[r2], a_sub_population->offspring1, a_sub_population->offspring2, mep_parameters, mep_constants);
				else {
					a_sub_population->offspring1 = a_sub_population->individuals[r1];
					a_sub_population->offspring2 = a_sub_population->individuals[r2];
				}
				// mutate the result and move the mutant in the new population
				a_sub_population->offspring1.mutation(mep_parameters, mep_constants, actual_operators, num_operators, actual_enabled_variables, num_actual_variables);
				if (mep_parameters->get_problem_type() == PROBLEM_REGRESSION)
					a_sub_population->offspring1.fitness_regression(training_data, cached_eval_matrix_double, cached_sum_of_errors, num_actual_variables, actual_enabled_variables, eval_double);
				else
					a_sub_population->offspring1.fitness_classification(training_data, cached_eval_matrix_double, cached_sum_of_errors, cached_threashold, num_actual_variables, actual_enabled_variables, eval_double, tmp_value_class);

				a_sub_population->offspring2.mutation(mep_parameters, mep_constants, actual_operators, num_operators, actual_enabled_variables, num_actual_variables);
				if (mep_parameters->get_problem_type() == PROBLEM_REGRESSION)
					a_sub_population->offspring2.fitness_regression(training_data, cached_eval_matrix_double, cached_sum_of_errors, num_actual_variables, actual_enabled_variables, eval_double);
				else
					a_sub_population->offspring2.fitness_classification(training_data, cached_eval_matrix_double, cached_sum_of_errors, cached_threashold, num_actual_variables, actual_enabled_variables, eval_double, tmp_value_class);

				if (a_sub_population->offspring1.get_fitness() < a_sub_population->offspring2.get_fitness())   // the best offspring replaces the worst a_chromosome in the population
					if (a_sub_population->offspring1.get_fitness() < a_sub_population->individuals[mep_parameters->get_subpopulation_size() - 1].get_fitness()) {
						a_sub_population->individuals[mep_parameters->get_subpopulation_size() - 1] = a_sub_population->offspring1;
						sort_by_fitness(*a_sub_population);
					}
					else;
				else if (a_sub_population->offspring2.get_fitness() < a_sub_population->individuals[mep_parameters->get_subpopulation_size() - 1].get_fitness()) {
					a_sub_population->individuals[mep_parameters->get_subpopulation_size() - 1] = a_sub_population->offspring2;
					sort_by_fitness(*a_sub_population);
				}
			}

	}
}
//-----------------------------------------------------------------------
bool t_mep::start_steady_state(int run, double ***eval_double, s_value_class **array_value_class, f_on_progress on_generation, f_on_progress on_new_evaluation)       // Steady-State MEP
{
	my_srand(run + mep_parameters->get_random_seed());

	//clock_t start_time = clock();
	time_t start_time;
	time(&start_time);

	generate_random_individuals();

	//wxLogDebug(wxString() << "generation " << gen_index << " ");
	// an array of threads. Each sub population is evolved by a thread
	std::thread **mep_threads = new std::thread*[mep_parameters->get_num_threads()];
	// we create a fixed number of threads and each thread will take and evolve one subpopulation, then it will take another one
	std::mutex mutex;
	// we need a mutex to make sure that the same subpopulation will not be evolved twice by different threads

	// initial population (generation 0)
	int current_subpop_index = 0;
	for (int t = 0; t < mep_parameters->get_num_threads(); t++)
		mep_threads[t] = new std::thread(&t_mep::evolve_one_subpopulation_for_one_generation, this, &current_subpop_index, &mutex, pop, 0, eval_double[t], array_value_class[t]);


	for (int t = 0; t < mep_parameters->get_num_threads(); t++) {
		mep_threads[t]->join(); // wait for all threads to execute
		delete mep_threads[t];
	}

	// now I have to apply this to the validation set

	stats[run].best_validation_error = -1;

	if (mep_parameters->get_use_validation_data() && validation_data->get_num_rows() > 0) {
		// I must run all solutions for the validation data and choose the best one
		stats[run].best_validation_error = compute_validation_error(&best_subpopulation_index_for_test, &best_individual_index_for_test, eval_double[0], array_value_class[0]);
		stats[run].prg = pop[best_subpopulation_index_for_test].individuals[best_individual_index_for_test];
	}

	double best_error_on_training, mean_error_on_training;
	compute_best_and_average_error(best_error_on_training, mean_error_on_training);
	stats[run].best_training_error[0] = best_error_on_training;
	stats[run].average_training_error[0] = mean_error_on_training;
	stats[run].last_gen = 0;
	modified_project = true;

	if (on_generation)
		on_generation();

	for (int gen_index = 1; gen_index < mep_parameters->get_num_generations(); gen_index++) {
		if (_stopped)
			break;

		int current_subpop_index = 0;
		for (int t = 0; t < mep_parameters->get_num_threads(); t++)
			mep_threads[t] = new std::thread(&t_mep::evolve_one_subpopulation_for_one_generation, this, &current_subpop_index, &mutex, pop, gen_index, eval_double[t], array_value_class[t]);

		for (int t = 0; t < mep_parameters->get_num_threads(); t++) {
			mep_threads[t]->join(); // wait for all threads to execute
			delete mep_threads[t];
		}

		// now copy the best from each deme to the next one
		for (int d = 0; d < mep_parameters->get_num_subpopulations(); d++) { // din d in d+1
			// aleg unul din dema d
			long w = tournament(pop[d]);
			if (pop[d].individuals[w].compare(&pop[(d + 1) % mep_parameters->get_num_subpopulations()].individuals[mep_parameters->get_subpopulation_size() - 1], false))
				pop[(d + 1) % mep_parameters->get_num_subpopulations()].individuals[mep_parameters->get_subpopulation_size() - 1] = pop[d].individuals[w];
		}
		for (int d = 0; d < mep_parameters->get_num_subpopulations(); d++) // din d in d+1
			sort_by_fitness(pop[d]);

		if (mep_parameters->get_use_validation_data() && validation_data->get_num_rows() > 0) {
			// I must run all solutions for the validation data and choose the best one
			int best_index_on_validation, best_subpop_index_on_validation;
			double validation_error = compute_validation_error(&best_subpop_index_on_validation, &best_index_on_validation, eval_double[0], array_value_class[0]);
			if ((validation_error < stats[run].best_validation_error) || (fabs(stats[run].best_validation_error + 1) <= 1E-6)) {
				stats[run].best_validation_error = validation_error;
				best_individual_index_for_test = best_index_on_validation;
				best_subpopulation_index_for_test = best_subpop_index_on_validation;
				stats[run].prg = pop[best_subpopulation_index_for_test].individuals[best_individual_index_for_test];
			}
		}

		compute_best_and_average_error(best_error_on_training, mean_error_on_training);
		stats[run].best_training_error[gen_index] = best_error_on_training;
		stats[run].average_training_error[gen_index] = mean_error_on_training;
		stats[run].last_gen = gen_index;
#ifdef _DEBUG
		
#endif
		if (on_generation)
			on_generation();
	}

	// DEBUG ONLY
	//	fitness_regression(pop[best_individual_index]);
	/*
	if (mep_parameters->get_problem_type() == PROBLEM_REGRESSION)
	fitness_regression(pop[0].individuals[0], eval_double[0]);
	else
	if (mep_parameters->get_problem_type() == PROBLEM_CLASSIFICATION)
	fitness_classification(pop[0].individuals[0], eval_double[0], array_value_class[0]);
	*/
	if (!(mep_parameters->get_use_validation_data() && validation_data->get_num_rows() > 0)) // if no validation data, the test is the best from all
		stats[run].prg = pop[best_subpopulation_index].individuals[best_individual_index];
	stats[run].prg.simplify();

	if (test_data && test_data->get_num_rows() && test_data->get_num_outputs()) {// has target
		// I must run all solutions for the test data and choose the best one
		if (mep_parameters->get_problem_type() == PROBLEM_REGRESSION) {
			if (stats[run].prg.compute_regression_error_on_double_data(test_data->get_data_matrix_double(), test_data->get_num_rows(), test_data->get_num_cols() - 1, &stats[run].test_error));
		}
		else
			if (mep_parameters->get_problem_type() == PROBLEM_CLASSIFICATION) {
				if (stats[run].prg.compute_classification_error_on_double_data(test_data->get_data_matrix_double(), test_data->get_num_rows(), test_data->get_num_cols() - 1, &stats[run].test_error));
			}
	}


	delete[] mep_threads;

	//clock_t end_time = clock();
	time_t end_time;
	time(&end_time);

	//stats[run].running_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	stats[run].running_time = difftime(end_time, start_time);

	return true;
}
//---------------------------------------------------------------------------
int t_mep::to_pugixml_node(pugi::xml_node parent)
{
	// utilized variables


	pugi::xml_node training_node = parent.append_child("training");
	training_data->to_xml(training_node);
	pugi::xml_node validation_node = parent.append_child("validation");
	validation_data->to_xml(validation_node);
	pugi::xml_node testing_node = parent.append_child("test");
	test_data->to_xml(testing_node);

	if (variables_enabled) {
        
        	char *tmp_str = new char[training_data->get_num_cols() * 2 + 10];
        
		pugi::xml_node utilized_variables_node = parent.append_child("variables_utilization");

		tmp_str[0] = 0;
		for (int v = 0; v < training_data->get_num_cols() - 1; v++) {
			char tmp_s[30];
			sprintf(tmp_s, "%d", variables_enabled[v]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		pugi::xml_node utilized_variables_data_node = utilized_variables_node.append_child(pugi::node_pcdata);
		utilized_variables_data_node.set_value(tmp_str);
        
        delete [] tmp_str;
	}
/*
	pugi::xml_node target_col_node = parent.append_child("target_col");
	pugi::xml_node target_col_data_node = target_col_node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", target_col);
	target_col_data_node.set_value(tmp_str);
*/
	pugi::xml_node parameters_node = parent.append_child("parameters");
	mep_parameters->to_xml(parameters_node);

	pugi::xml_node constants_node = parent.append_child("constants");
	mep_constants->to_xml(constants_node);

	pugi::xml_node operators_node = parent.append_child("operators");
	operators->to_xml(operators_node);

	pugi::xml_node results_node = parent.append_child("results");

	for (int r = 0; r <= last_run_index; r++) {
		pugi::xml_node run_node = results_node.append_child("run");
		stats[r].to_xml(run_node);
	}

	return true;
}
//---------------------------------------------------------------------------
int t_mep::from_pugixml_node(pugi::xml_node parent)
{
	//get the utilized variables first

	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}

	training_data->clear_data();

	if (training_data) {
		pugi::xml_node node = parent.child("training");
		if (node) {
			training_data->from_xml(node);
			num_total_variables = training_data->get_num_cols() - 1;
		}
		else
			num_total_variables = 0;
	}
	else
		num_total_variables = 0;
	

	if (training_data->get_num_rows()) {
		//actual_enabled_variables = new int[num_total_variables];
		variables_enabled = new bool[num_total_variables];

		pugi::xml_node node = parent.child("variables_utilization");
		if (node) {
			const char *value_as_cstring = node.child_value();
			int num_jumped_chars = 0;
			//num_actual_variables = 0;
			int i = 0;

			while (*(value_as_cstring + num_jumped_chars)) {
                int int_read;
				sscanf(value_as_cstring + num_jumped_chars, "%d", &int_read);
                variables_enabled[i] = int_read;
				/*
				if (variables_utilization[i]) {
				actual_enabled_variables[num_actual_variables] = i;
				num_actual_variables++;
				}
				*/
				long local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
				i++;
			}
		}
		else {// not found, use everything
			//num_actual_variables = num_total_variables;
			for (int i = 0; i < num_total_variables; i++) {
				variables_enabled[i] = 1;
				//actual_enabled_variables[i] = i;
			}
		}
	}

	pugi::xml_node node = parent.child("target_col");
	if (node) {
		const char *value_as_cstring = node.child_value();
		target_col = atoi(value_as_cstring);
	}
	else
		target_col = training_data->get_num_cols() - 1;

	validation_data->clear_data();

	if (validation_data) {
		node = parent.child("validation");
		if (node)
			validation_data->from_xml(node);
	}

	test_data->clear_data();

	if (test_data) {
		node = parent.child("test");
		if (node) {
			test_data->from_xml(node);
			if (test_data->get_num_cols() < training_data->get_num_cols())
				test_data->set_num_outputs(0);
		}
	}

	node = parent.child("parameters");
	if (node)
		mep_parameters->from_xml(node);
	else
		mep_parameters->init();

	node = parent.child("operators");
	if (node)
		operators->from_xml(node);
	else
		operators->init();

	node = parent.child("constants");
	if (node)
		mep_constants->from_xml(node);
	else
		mep_constants->init();

	last_run_index = -1;
	pugi::xml_node node_results = parent.child("results");
	if (node_results)
		for (pugi::xml_node row = node_results.child("run"); row; row = row.next_sibling("run"), last_run_index++);

	if (last_run_index > -1) {
		stats = new t_mep_statistics[last_run_index + 1];
		last_run_index = 0;
		for (pugi::xml_node row = node_results.child("run"); row; row = row.next_sibling("run"), last_run_index++)
			stats[last_run_index].from_xml(row, mep_parameters->get_num_generations(), mep_parameters->get_code_length());
		last_run_index--;
	}

	modified_project = false;

	return true;
}
//---------------------------------------------------------------------------
int t_mep::to_xml(const char *filename)
{
	pugi::xml_document doc;
	// add node with some name
	pugi::xml_node body = doc.append_child("project");

	pugi::xml_node problem_description_node = body.append_child("problem_description");
	pugi::xml_node data = problem_description_node.append_child(pugi::node_pcdata);
	data.set_value(problem_description);

	pugi::xml_node version_node = body.append_child("version");
	data = version_node.append_child(pugi::node_pcdata);
	data.set_value(version);

	pugi::xml_node alg_node = body.append_child("algorithm");
	to_pugixml_node(alg_node);

	modified_project = false;

#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	int result = doc.save_file(w_filename);
	delete[] w_filename;
	return result;
#else
	return doc.save_file(filename);
#endif

}
//-----------------------------------------------------------------
int t_mep::from_xml(const char* filename)
{
	pugi::xml_document doc;

	pugi::xml_parse_result result;

#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	result = doc.load_file(w_filename);
	delete[] w_filename;
#else
	result = doc.load_file(filename);
#endif

	if (result.status != pugi::status_ok)
		return false;

	pugi::xml_node body_node = doc.child("project");

	if (!body_node)
		return false;

	if (problem_description) {
		delete[] problem_description;
		problem_description = NULL;
	}
	pugi::xml_node node = body_node.child("problem_description");
	if (node) {
		const char *value_as_cstring = node.child_value();

		if (strlen(value_as_cstring)) {
			problem_description = new char[strlen(value_as_cstring) + 1];
			strcpy(problem_description, value_as_cstring);
		}
	}
	else {
		problem_description = new char[100];
		strcpy(problem_description, "Problem description here ...");
	}


	pugi::xml_node alg_node = body_node.child("algorithm");

	if (!alg_node)
		return false;

	from_pugixml_node(alg_node);

	return true;
}
//-----------------------------------------------------------------

double t_mep::get_best_training_error(int run, int gen)
{
	if (stats[run].best_training_error)
		if (gen <= stats[run].last_gen)
			return stats[run].best_training_error[gen];
		else
			return stats[run].best_training_error[stats[run].last_gen];
	else
		return -1;
}
//---------------------------------------------------------------------------
double t_mep::get_best_validation_error(int run)
{
	return stats[run].best_validation_error;
}
//---------------------------------------------------------------------------
double t_mep::get_test_error(int run)
{
	return stats[run].test_error;
}
//---------------------------------------------------------------------------
double t_mep::get_average_training_error(int run, int gen)
{
	if (stats[run].average_training_error)
		if (gen <= stats[run].last_gen)
			return stats[run].average_training_error[gen];
		else
			return stats[run].average_training_error[stats[run].last_gen];
	else
		return -1;
}
//---------------------------------------------------------------------------
int t_mep::get_latest_generation(int run)
{
	return stats[run].last_gen;
}
//---------------------------------------------------------------------------
double t_mep::get_running_time(int run)
{
	return stats[run].running_time;
}
//---------------------------------------------------------------------------
void t_mep::stop(void)
{
	_stopped = true;
}
//---------------------------------------------------------------------------
int t_mep::stats_to_csv(const char *filename)
{
	FILE *f = NULL;
#ifdef WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"w");
	delete[] w_filename;

#else
	f = fopen(filename, "w");
#endif

	if (!f)
		return false;
	fprintf(f, "running time; training error; validation error\n");
	for (int r = 0; r <= last_run_index; r++)
		fprintf(f, "%lf;%lf;%lf\n", stats[r].running_time, stats[r].best_training_error[mep_parameters->get_num_generations() - 1], stats[r].best_validation_error);

	fclose(f);
	return true;
}
//---------------------------------------------------------------------------
int running_time_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->running_time < ((t_mep_statistics*)b)->running_time)
		return -1;
	else
		if (((t_mep_statistics*)a)->running_time > ((t_mep_statistics*)b)->running_time)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int running_time_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->running_time < ((t_mep_statistics*)b)->running_time)
		return 1;
	else
		if (((t_mep_statistics*)a)->running_time > ((t_mep_statistics*)b)->running_time)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep::sort_stats_by_running_time(bool ascending)
{
	if (ascending)
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), running_time_comparator_ascending);
	else
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), running_time_comparator_descending);
}
//---------------------------------------------------------------------------
int training_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->best_training_error[((t_mep_statistics*)a)->last_gen] < ((t_mep_statistics*)b)->best_training_error[((t_mep_statistics*)b)->last_gen])
		return -1;
	else
		if (((t_mep_statistics*)a)->best_training_error[((t_mep_statistics*)a)->last_gen] > ((t_mep_statistics*)b)->best_training_error[((t_mep_statistics*)b)->last_gen])
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int training_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->best_training_error[((t_mep_statistics*)a)->last_gen] < ((t_mep_statistics*)b)->best_training_error[((t_mep_statistics*)b)->last_gen])
		return 1;
	else
		if (((t_mep_statistics*)a)->best_training_error[((t_mep_statistics*)a)->last_gen] > ((t_mep_statistics*)b)->best_training_error[((t_mep_statistics*)b)->last_gen])
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep::sort_stats_by_training_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), training_error_comparator_ascending);
	else
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), training_error_comparator_descending);

}
//---------------------------------------------------------------------------
int validation_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->best_validation_error < ((t_mep_statistics*)b)->best_validation_error)
		return -1;
	else
		if (((t_mep_statistics*)a)->best_validation_error > ((t_mep_statistics*)b)->best_validation_error)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int validation_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->best_validation_error < ((t_mep_statistics*)b)->best_validation_error)
		return 1;
	else
		if (((t_mep_statistics*)a)->best_validation_error > ((t_mep_statistics*)b)->best_validation_error)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep::sort_stats_by_validation_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), validation_error_comparator_ascending);
	else
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), validation_error_comparator_descending);

}
//---------------------------------------------------------------------------
int test_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->test_error < ((t_mep_statistics*)b)->test_error)
		return -1;
	else
		if (((t_mep_statistics*)a)->test_error > ((t_mep_statistics*)b)->test_error)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int test_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_statistics*)a)->test_error < ((t_mep_statistics*)b)->test_error)
		return 1;
	else
		if (((t_mep_statistics*)a)->test_error > ((t_mep_statistics*)b)->test_error)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep::sort_stats_by_test_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), test_error_comparator_ascending);
	else
		qsort((void*)stats, last_run_index + 1, sizeof(t_mep_statistics), test_error_comparator_descending);
}
//---------------------------------------------------------------------------
void t_mep::compute_list_of_enabled_variables(void)
{
	if (actual_enabled_variables) {
		delete[](actual_enabled_variables);
		(actual_enabled_variables) = NULL;
	}

	num_actual_variables = 0;
	if (training_data->get_num_cols())
		num_total_variables = training_data->get_num_cols() - 1;
	else
		num_total_variables = 0;
	if (num_total_variables) {
		actual_enabled_variables = new int[num_total_variables];
		for (int i = 0; i < num_total_variables; i++)
			if (variables_enabled[i]) {
				actual_enabled_variables[num_actual_variables] = i;
				num_actual_variables++;
			}
	}
}
//---------------------------------------------------------------------------
bool t_mep::is_running(void)
{
	return !_stopped;
}
//---------------------------------------------------------------------------
int t_mep::get_last_run_index(void)
{
	return last_run_index;
}
//---------------------------------------------------------------------------
void t_mep::clear_stats(void)
{
	if (_stopped) {
		modified_project = true;

		last_run_index = -1;
		if (stats) {
			delete[] stats;
			stats = NULL;
		}
	}
}
//---------------------------------------------------------------------------
char* t_mep::program_as_C(int run_index, bool simplified, double *inputs)
{
	return stats[run_index].prg.to_C_double(simplified, inputs, mep_parameters->get_problem_type());
}
//---------------------------------------------------------------------------
int t_mep::get_num_outputs(void)
{
	return 1;
}
//---------------------------------------------------------------------------
int t_mep::get_num_total_variables(void)
{
	return num_total_variables;
}
//---------------------------------------------------------------------------
void t_mep::set_num_total_variables(int value)
{
	if (_stopped) {
		num_total_variables = value;
		target_col = num_total_variables;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::init(void)
{
	if (_stopped) {
		mep_parameters->init();
		operators->init();
		mep_constants->init();

		if (actual_enabled_variables) {
			delete[] actual_enabled_variables;
			actual_enabled_variables = NULL;
		}
		if (variables_enabled) {
			delete[] variables_enabled;
			variables_enabled = NULL;
		}

		num_actual_variables = 0;

		modified_project = false;
	}
}
//---------------------------------------------------------------------------
int t_mep::get_num_actual_variables(void)
{
	return num_actual_variables;
}
//---------------------------------------------------------------------------
bool t_mep::is_variable_enabled(int index)
{
	return variables_enabled[index];
}
//---------------------------------------------------------------------------
void t_mep::set_variable_enable(int index, bool new_state)
{
	if (_stopped) {
		variables_enabled[index] = new_state;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
bool t_mep::is_project_modified(void)
{
	return modified_project;
}
//---------------------------------------------------------------------------
void t_mep::set_problem_description(const char* value)
{
	if (_stopped) {

		if (problem_description) {
			delete[] problem_description;
			problem_description = NULL;
		}

		if (strlen(value)) {
			problem_description = new char[strlen(value) + 1];
			strcpy(problem_description, value);
		}

		modified_project = true;
	}
}
//---------------------------------------------------------------------------
char* t_mep::get_problem_description(void)
{
	return problem_description;
}
//---------------------------------------------------------------------------
void t_mep::set_enable_cache_results_for_all_training_data(bool value)
{
	if (_stopped)
	  cache_results_for_all_training_data = value;
}
//---------------------------------------------------------------------------
bool t_mep::get_enable_cache_results_for_all_training_data(void)
{
	return cache_results_for_all_training_data;
}
//---------------------------------------------------------------------------
long long t_mep::get_memory_consumption(void)
{
	// for chromosomes
	long long chromosomes_memory = 0;

	return 0;
}
//---------------------------------------------------------------------------
bool t_mep::validate_project(void)
{
	return true;
}
//---------------------------------------------------------------------------
void t_mep::set_training_data(t_mep_data *_data)
{
	if (_stopped) {
		training_data = _data;

		if (variables_enabled) {
			delete[] variables_enabled;
			variables_enabled = NULL;
		}

		if (actual_enabled_variables) {
			delete[] actual_enabled_variables;
			actual_enabled_variables = NULL;
		}

		if (training_data) {
			num_total_variables = training_data->get_num_cols();
			num_actual_variables = num_total_variables;

			variables_enabled = new bool[num_total_variables];

			for (int i = 0; i < num_total_variables; i++)
				variables_enabled[i] = 1;

			target_col = training_data->get_num_cols() - 1;
		}
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_validation_data(t_mep_data *_data)
{
	if (_stopped) {
		validation_data = _data;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_test_data(t_mep_data *_data)
{
	if (_stopped) {
		test_data = _data;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_operators(t_mep_operators *_mep_operators)
{
	if (_stopped) {
		operators = _mep_operators;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_constants(t_mep_constants *_mep_constants)
{
	if (_stopped) {
		mep_constants = _mep_constants;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::set_parameters(t_mep_parameters *_mep_parameters)
{
	if (_stopped) {
		mep_parameters = _mep_parameters;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------