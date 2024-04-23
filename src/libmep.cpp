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
#include "utils/rands_generator_utils.h"
//---------------------------------------------------------------------------
t_mep::t_mep()
{
	strcpy(version, "2024.4.23.0-beta");

	num_selected_operators = 0;

	cached_eval_variables_matrix_double = NULL;
	cached_sum_of_errors_for_variables = NULL;

	modified_project = false;
	_stopped = true;
	_stopped_signal_sent = false;
	last_run_index = -1;

	variables_enabled = NULL;
	actual_enabled_variables = NULL;
	num_actual_variables = 0;
	num_total_variables = 0;
	cache_results_for_all_training_data = true;

	problem_description = new char[100];
	strcpy(problem_description, "Problem description here ...");

	random_subset_indexes = NULL;
}
//---------------------------------------------------------------------------
t_mep::~t_mep()
{
	clear();
}
//---------------------------------------------------------------------------
void t_mep::clear(void)
{
	if (actual_enabled_variables) {
		delete[] actual_enabled_variables;
		actual_enabled_variables = NULL;
	}
	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}

	statistics.delete_memory();

	if (problem_description) {
		delete[] problem_description;
		problem_description = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep::allocate_sub_population(t_sub_population & a_pop)
{
	unsigned int num_program_outputs = 1;// do not confuse it with the number of data outputs!!!!!!!!
	
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_REGRESSION ||
		mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE ||
		mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION)
		num_program_outputs = mep_parameters.get_num_outputs();
	else { // MEP_PROBLEM_MULTICLASS
		if (mep_parameters.get_error_measure() == MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR ||
			mep_parameters.get_error_measure() == MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR)
			num_program_outputs = 0;
		else {
			if (mep_parameters.get_error_measure() == MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR)
				num_program_outputs = training_data.get_num_classes();
			else
				if (mep_parameters.get_error_measure() == MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR)
					num_program_outputs = 1;
		}
	}
	
	a_pop.offspring1.allocate_memory(mep_parameters.get_code_length(),
			num_total_variables, mep_parameters.get_constants_probability() > 1E-6,
			&mep_constants, num_program_outputs, training_data.get_num_classes(),
									 mep_parameters.get_problem_type(),
									 mep_parameters.get_error_measure(),
									 training_data.get_class_labels_ptr(),
									 mep_parameters.get_data_type());
	a_pop.offspring2.allocate_memory(mep_parameters.get_code_length(),
			num_total_variables, mep_parameters.get_constants_probability() > 1E-6,
			&mep_constants, num_program_outputs, training_data.get_num_classes(),
									 mep_parameters.get_problem_type(),
									 mep_parameters.get_error_measure(),
									 training_data.get_class_labels_ptr(),
									 mep_parameters.get_data_type());
	a_pop.individuals = new t_mep_chromosome[mep_parameters.get_subpopulation_size()];
	for (unsigned int j = 0; j < mep_parameters.get_subpopulation_size(); j++)
		a_pop.individuals[j].allocate_memory(mep_parameters.get_code_length(),
				num_total_variables, mep_parameters.get_constants_probability() > 1E-6,
				&mep_constants, num_program_outputs, training_data.get_num_classes(),
											 mep_parameters.get_problem_type(),
									   mep_parameters.get_error_measure(),
											 training_data.get_class_labels_ptr(),
											 mep_parameters.get_data_type());
}
//---------------------------------------------------------------------------
void t_mep::get_best(t_mep_chromosome & dest) const
{
	dest = population[best_subpopulation_index].individuals[best_individual_index];
}
//---------------------------------------------------------------------------
t_mep_chromosome* t_mep::get_best_ptr() const
{
	return &population[best_subpopulation_index].individuals[best_individual_index];
}
//---------------------------------------------------------------------------
/*
int sort_function_chromosomes(const void* a, const void* b)
{
	return ((t_mep_chromosome*)a)->compare(*(t_mep_chromosome*)b);
}
//---------------------------------------------------------------------------
void t_mep::sort_by_fitness(t_sub_population & a_pop) // sort ascending the individuals in population
{
	qsort((void*)a_pop.individuals,
		  mep_parameters.get_subpopulation_size(),
		  sizeof(a_pop.individuals[0]),
		  sort_function_chromosomes);
}
*/
//---------------------------------------------------------------------------
void t_mep::delete_sub_population(t_sub_population & a_pop)
{
	if (a_pop.individuals)
		delete[] a_pop.individuals;

	a_pop.offspring1.clear();
	a_pop.offspring2.clear();
}
//---------------------------------------------------------------------------

unsigned int t_mep::tournament(const t_sub_population & a_pop, t_seed & seed) const
{
	unsigned int p;
	p = mep_unsigned_int_rand(seed, 0, mep_parameters.get_subpopulation_size());
	for (unsigned int i = 1; i < mep_parameters.get_tournament_size(); i++) {
		unsigned int r = mep_unsigned_int_rand(seed, 0, mep_parameters.get_subpopulation_size());
		if (a_pop.individuals[r].compare(a_pop.individuals[p], mep_parameters.get_precision()) < 0)
			p = r;
	}
	return p;
}
//---------------------------------------------------------------------------
void t_mep::compute_best_and_average_error(double& best_error, double& mean_error,
		double& num_incorrectly_classified, double& average_incorrectly_classified)
{
	mean_error = 0;
	average_incorrectly_classified = 0;
	population[0].compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
											mep_parameters.get_precision());
	best_error = population[0].individuals[population[0].best_index].get_fitness();
	num_incorrectly_classified = population[0].individuals[population[0].best_index].get_num_incorrectly_classified();

	best_individual_index = population[0].best_index;
	best_subpopulation_index = 0;
	
	for (unsigned int i = 1; i < mep_parameters.get_num_subpopulations(); i++) {
		population[i].compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
												mep_parameters.get_precision());
		if (population[i].individuals[population[i].best_index].compare(
				population[best_subpopulation_index].individuals[population[best_subpopulation_index].best_index],
																		mep_parameters.get_precision()) < 0) {
			best_error = population[i].individuals[population[i].best_index].get_fitness();
			num_incorrectly_classified = population[i].individuals[population[i].best_index].get_num_incorrectly_classified();
			best_individual_index = population[i].best_index;
			best_subpopulation_index = i;
		}
	}

	for (unsigned int i = 0; i < mep_parameters.get_num_subpopulations(); i++)
		for (unsigned int k = 0; k < mep_parameters.get_subpopulation_size(); k++) {
			mean_error += population[i].individuals[k].get_fitness();
			average_incorrectly_classified += population[i].individuals[population[i].best_index].get_num_incorrectly_classified();
		}

	mean_error /= mep_parameters.get_num_subpopulations() * mep_parameters.get_subpopulation_size();
	average_incorrectly_classified /= mep_parameters.get_num_subpopulations() * mep_parameters.get_subpopulation_size();
}
//---------------------------------------------------------------------------
void t_mep::allocate_extra_arrays(s_value_class ***array_value_class,
									   char *** gene_used_for_output)
{
	unsigned int num_threads = mep_parameters.get_num_threads();
	unsigned int code_length = mep_parameters.get_code_length();
	unsigned int num_rows;
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE)
		num_rows = training_data_ts.get_num_rows();
	else
		num_rows = training_data.get_num_rows();
	
	*array_value_class = new s_value_class* [num_threads];
	*gene_used_for_output = new char*[num_threads];
	for (unsigned int t = 0; t < num_threads; t++){
		(*array_value_class)[t] = new s_value_class[num_rows];
		(*gene_used_for_output)[t] = new char[code_length];
	}
}
//---------------------------------------------------------------------------
void t_mep::delete_extra_arrays(s_value_class ***array_value_class,
									 char *** gene_used_for_output)
{
	unsigned int num_threads = mep_parameters.get_num_threads();
	
	if (*array_value_class) {
		for (unsigned int c = 0; c < num_threads; c++){
			delete[](*array_value_class)[c];
			delete[](*gene_used_for_output)[c];
		}
		delete[] *array_value_class;
		delete[] *gene_used_for_output;
		*array_value_class = NULL;
		*gene_used_for_output = NULL;
	}
}
//---------------------------------------------------------------------------
int t_mep::start(f_on_progress on_generation,
				 f_on_progress on_new_evaluation,
				 f_on_progress on_complete_run)
{
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE) {
		training_data_ts.clear_data();
		if (!training_data_ts.to_time_serie_from_multivariate(
			training_data,
			mep_parameters.get_window_size())) { // I should do validation of data first before calling start
			return false;
		}
		mep_parameters.set_num_outputs(training_data.get_num_cols());
	}

	_stopped = false;
	_stopped_signal_sent = false;

	compute_list_of_enabled_variables();

	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE)
		random_subset_selection_size = training_data_ts.get_num_rows() *
				mep_parameters.get_random_subset_selection_size_percent() / 100;
	else
		random_subset_selection_size = training_data.get_num_rows() *
			mep_parameters.get_random_subset_selection_size_percent() / 100;

	random_subset_indexes = new unsigned int[random_subset_selection_size];

	if ((mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION ||
		mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION) &&
		!training_data.get_class_labels_ptr()){ // the labels have not been extracted yet
		training_data.compute_class_labels(mep_parameters.get_num_outputs());
		validation_data.assign_class_index_from_training_class_labels(training_data.get_class_labels_ptr(), training_data.get_num_classes() );
		test_data.assign_class_index_from_training_class_labels(training_data.get_class_labels_ptr(), training_data.get_num_classes());
	}
	
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION)
		training_data.count_0_class();

	population = new t_sub_population[mep_parameters.get_num_subpopulations()];
	for (unsigned int i = 0; i < mep_parameters.get_num_subpopulations(); i++)
		allocate_sub_population(population[i]);

	num_selected_operators = mep_operators.get_list_of_operators(actual_operators);

	double*** eval_double = NULL;           // an array where the values of each expression are stored
	long long*** eval_long = NULL;
	
	s_value_class * *array_value_class;
	char** gene_used_for_output;

	if (mep_parameters.get_data_type() == MEP_DATA_LONG_LONG)
		allocate_values(&eval_long);
	else
		allocate_values(&eval_double);
	
	allocate_extra_arrays(&array_value_class, &gene_used_for_output);

	if (mep_parameters.get_data_type() == MEP_DATA_LONG_LONG)
		compute_cached_eval_matrix_long2(array_value_class[0]);
	else
		compute_cached_eval_matrix_double2(array_value_class[0]);

	t_seed* seeds = new t_seed[mep_parameters.get_num_subpopulations()];

	last_run_index = -1;

	statistics.create(mep_parameters.get_num_runs());
	for (unsigned int run_index = 0; run_index < mep_parameters.get_num_runs(); run_index++) {
		statistics.append(mep_parameters.get_num_generations());
		last_run_index++;
		for (unsigned int p = 0; p < mep_parameters.get_num_subpopulations(); p++)
			seeds[p].init((run_index + mep_parameters.get_random_seed()) * mep_parameters.get_num_subpopulations() + p);

		start_steady_state(run_index, seeds, eval_double, eval_long,
						   array_value_class, gene_used_for_output,
						   on_generation, on_new_evaluation);
		if (on_complete_run)
			on_complete_run();
		if (_stopped_signal_sent)
			break;
	}

	statistics.compute_mean_stddev(
			mep_parameters.get_use_validation_data() &&
			(validation_data.get_num_rows() > 0),
			(test_data.get_num_rows() > 0) &&
			(test_data.get_num_cols() == training_data.get_num_cols()));

	if (mep_parameters.get_data_type() == MEP_DATA_LONG_LONG)
		delete_values(&eval_long);
	else
		delete_values(&eval_double);
	
	delete_extra_arrays(&array_value_class, &gene_used_for_output);
	
	for (unsigned int i = 0; i < mep_parameters.get_num_subpopulations(); i++)
		delete_sub_population(population[i]);
	delete[] population;

	delete[] random_subset_indexes;

	delete[] seeds;

	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE) {
		training_data_ts.clear_data();
	}

	_stopped = true;
	return true;
}
//---------------------------------------------------------------------------
void t_mep::evolve_one_subpopulation_for_one_generation(
		unsigned int* current_subpop_index,
		std::mutex * mutex, t_sub_population * sub_populations,
		int generation_index, bool recompute_fitness_due_to_change_of_subset,
		double** eval_double, long long** eval_long,
		s_value_class * tmp_value_class,
		char *gene_used_for_output,
		t_seed * seeds)
{
	unsigned int pop_index = 0;
	unsigned int subpopulation_size = mep_parameters.get_subpopulation_size();
	while (*current_subpop_index < mep_parameters.get_num_subpopulations()) {
		// still more subpopulations to evolve?

		while (!mutex->try_lock()) {}// create a lock so that multiple threads will not evolve the same sub population
		pop_index = *current_subpop_index;
		(*current_subpop_index)++;
		mutex->unlock();

		// pop_index is the index of the subpopulation evolved by the current thread

		if (pop_index < mep_parameters.get_num_subpopulations()) {
			t_sub_population* a_sub_population = &sub_populations[pop_index];

			if (generation_index == 0) { // first generation; generate random
				for (unsigned int j = 0; j < subpopulation_size; j++)
					population[pop_index].individuals[j].generate_random(
							mep_parameters, mep_constants,
							actual_operators, num_selected_operators,
							actual_enabled_variables, num_actual_variables,
							seeds[pop_index]);
				// compute fitness
				for (unsigned int i = 0; i < subpopulation_size; i++)
					population[pop_index].individuals[i].compute_fitness(
														training_data,
														training_data_ts,
						random_subset_indexes, random_subset_selection_size,
						cached_eval_variables_matrix_double,
						cached_eval_variables_matrix_long,
						cached_sum_of_errors_for_variables,
						cached_threashold, tmp_value_class,
						num_actual_variables, actual_enabled_variables,
						eval_double, eval_long,
																		 gene_used_for_output,
                        seeds[pop_index]);
				//sort_by_fitness(pop[pop_index]);
				a_sub_population->compute_index_of_the_worst(subpopulation_size,
															 mep_parameters.get_precision());
			}// end if generation == 0
			else {// other generations after the first one
				if (recompute_fitness_due_to_change_of_subset) {
					for (unsigned int i = 0; i < subpopulation_size; i++)
						population[pop_index].individuals[i].compute_fitness(
												training_data,
												training_data_ts,
							random_subset_indexes, random_subset_selection_size,
							cached_eval_variables_matrix_double,
							cached_eval_variables_matrix_long,
						cached_sum_of_errors_for_variables,
							cached_threashold,
							 tmp_value_class,
							num_actual_variables, actual_enabled_variables,
							eval_double, eval_long, gene_used_for_output,
																			 seeds[pop_index]);
					//sort_by_fitness(pop[pop_index]);
					a_sub_population->compute_index_of_the_worst(subpopulation_size,
																 mep_parameters.get_precision());
				}
				// continue
				for (unsigned int k = 0; k < subpopulation_size; k += 2) {
					// choose the parents using binary tournament
					unsigned int r1 = tournament(*a_sub_population, seeds[pop_index]);
					unsigned int r2 = tournament(*a_sub_population, seeds[pop_index]);
					// crossover
					double p = mep_real_rand(seeds[pop_index], 0, 1);
					if (p < mep_parameters.get_crossover_probability()) {
						if (mep_parameters.get_crossover_type() == MEP_CROSSOVER_UNIFORM)
							a_sub_population->individuals[r1].uniform_crossover(
								a_sub_population->individuals[r2],
								a_sub_population->offspring1, a_sub_population->offspring2,
								mep_constants, seeds[pop_index]);
						else
							a_sub_population->individuals[r1].one_cut_point_crossover(
								a_sub_population->individuals[r2],
								a_sub_population->offspring1, a_sub_population->offspring2,
								mep_constants, seeds[pop_index]);
					}
					else {
						// cannot do swap pointers here!

						a_sub_population->offspring1 = a_sub_population->individuals[r1];
						a_sub_population->offspring2 = a_sub_population->individuals[r2];
					}
					// mutate the result and move the mutant in the new population
					a_sub_population->offspring1.mutation(mep_parameters, mep_constants,
							actual_operators, num_selected_operators,
							actual_enabled_variables, num_actual_variables, seeds[pop_index]);

					a_sub_population->offspring1.compute_fitness(training_data, training_data_ts,
						random_subset_indexes, random_subset_selection_size,
						cached_eval_variables_matrix_double,
								cached_eval_variables_matrix_long,
								cached_sum_of_errors_for_variables,
						cached_threashold, tmp_value_class,
						num_actual_variables, actual_enabled_variables,
						eval_double, eval_long, gene_used_for_output,
									seeds[pop_index]);

					a_sub_population->offspring2.mutation(mep_parameters,
							mep_constants, actual_operators, num_selected_operators,
							actual_enabled_variables, num_actual_variables,
										seeds[pop_index]);

					a_sub_population->offspring2.compute_fitness(training_data, training_data_ts,
						random_subset_indexes, random_subset_selection_size,
						cached_eval_variables_matrix_double,
										cached_eval_variables_matrix_long,
										cached_sum_of_errors_for_variables,
						cached_threashold, tmp_value_class,
						num_actual_variables, actual_enabled_variables,
						eval_double, eval_long, gene_used_for_output,
										seeds[pop_index]);

					if (a_sub_population->offspring1.compare(a_sub_population->offspring2, mep_parameters.get_precision()) < 0) {   // the best offspring replaces the worst a_chromosome in the population
						// offspring 1 is better
						if (a_sub_population->offspring1.compare(a_sub_population->individuals[a_sub_population->worst_index],
																 mep_parameters.get_precision()) < 0) {
							a_sub_population->individuals[a_sub_population->worst_index].swap_pointers(a_sub_population->offspring1);
							a_sub_population->compute_index_of_the_worst(subpopulation_size,
																		 mep_parameters.get_precision());
						}
						else;
					}
					else { // offspring 2 is better
						if (a_sub_population->offspring2.compare(a_sub_population->individuals[a_sub_population->worst_index],
																 mep_parameters.get_precision()) < 0) {
							a_sub_population->individuals[a_sub_population->worst_index].swap_pointers(a_sub_population->offspring2);
							a_sub_population->compute_index_of_the_worst(subpopulation_size,
																		 mep_parameters.get_precision());
						}
					}
				}
			}// end other generations
		}// end of if (pop_index < mep_parameters.get_num_subpopulations())
	}
}
//-----------------------------------------------------------------------
bool t_mep::start_steady_state(unsigned int run_index,
							   t_seed * seeds,
							   double*** eval_double,
							   long long*** eval_long,
		s_value_class * *array_value_class,
							   char** gene_used_for_output,
		f_on_progress on_generation, f_on_progress /*on_new_evaluation*/)       // Steady-State MEP
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	//wxLogDebug(wxString() << "generation " << gen_index << " ");
	// an array of threads. Each sub population is evolved by a thread
	std::thread* mep_threads = new std::thread [mep_parameters.get_num_threads()];
	// we create a fixed number of threads and each thread will take and evolve one subpopulation, then it will take another one
	std::mutex mutex;
	// we need a mutex to make sure that the same subpopulation will not be evolved twice by different threads

	unsigned int num_generations_for_which_random_subset_is_kept_fixed_counter = mep_parameters.get_num_generations_for_which_random_subset_is_kept_fixed();

	for (unsigned int generation_index = 0; generation_index < mep_parameters.get_num_generations(); generation_index++) {
		if (_stopped_signal_sent)
			break;

		if (num_generations_for_which_random_subset_is_kept_fixed_counter >= mep_parameters.get_num_generations_for_which_random_subset_is_kept_fixed()) {
			get_random_subset(random_subset_selection_size, random_subset_indexes, seeds[0]);
			num_generations_for_which_random_subset_is_kept_fixed_counter = 0;
		}
		else
			num_generations_for_which_random_subset_is_kept_fixed_counter++;

		unsigned int current_subpop_index = 0;
		//unsigned int training_num_rows = training_data.get_num_rows();
		//if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE)
		//	training_num_rows = training_data_ts.get_num_rows();

		for (unsigned int t = 0; t < mep_parameters.get_num_threads(); t++) {
			bool recompute_fitness_due_to_change_of_subset;
			if (mep_parameters.get_random_subset_selection_size_percent() == 100)
				recompute_fitness_due_to_change_of_subset = generation_index == 0;
			else
				recompute_fitness_due_to_change_of_subset = num_generations_for_which_random_subset_is_kept_fixed_counter == 0;
			mep_threads[t] = std::thread(&t_mep::evolve_one_subpopulation_for_one_generation,
												this, &current_subpop_index, &mutex,
										 population,
										generation_index,
										 recompute_fitness_due_to_change_of_subset,
										 eval_double?eval_double[t]:NULL,
										 eval_long?eval_long[t]:NULL,
										 array_value_class[t],
										 gene_used_for_output[t],
										 seeds);
		}

		for (unsigned int t = 0; t < mep_parameters.get_num_threads(); t++) {
			mep_threads[t].join(); // wait for all threads to execute
		}

		double best_error_on_training, mean_error_on_training;
		double num_incorrectly_classified_on_training;
		double average_incorrectly_classified_on_training;

		compute_best_and_average_error(best_error_on_training,
									   mean_error_on_training,
									   num_incorrectly_classified_on_training,
									   average_incorrectly_classified_on_training);
		statistics.get_stat_ptr(run_index)->best_training_error[generation_index] = best_error_on_training;
		statistics.get_stat_ptr(run_index)->average_training_error[generation_index] = mean_error_on_training;

		statistics.get_stat_ptr(run_index)->best_training_num_incorrect[generation_index] = num_incorrectly_classified_on_training;
		statistics.get_stat_ptr(run_index)->average_training_num_incorrect[generation_index] = average_incorrectly_classified_on_training;

		if (mep_parameters.get_use_validation_data() &&
			(validation_data.get_num_rows() > 0)) {
			// I must run all solutions for the validation data and choose the best one
			unsigned int best_index_on_validation, best_subpop_index_on_validation;
			double num_incorrectly_classified_on_validation;
			
			double validation_error = compute_validation_error(
										best_subpop_index_on_validation,
										best_index_on_validation,
										eval_double?eval_double[0]:NULL,
										eval_long?eval_long[0]:NULL,
										array_value_class[0],
										gene_used_for_output[0],
					seeds, num_incorrectly_classified_on_validation);
			
			if ((validation_error < statistics.get_stat_ptr(run_index)->best_validation_error) ||
				statistics.get_stat_ptr(run_index)->best_validation_error < 0) {
				
				statistics.get_stat_ptr(run_index)->best_validation_error = validation_error;
				statistics.get_stat_ptr(run_index)->best_validation_num_incorrect = num_incorrectly_classified_on_validation;
				statistics.get_stat_ptr(run_index)->validation_error[generation_index] = validation_error;
				statistics.get_stat_ptr(run_index)->validation_num_incorrect[generation_index] = num_incorrectly_classified_on_validation;
				
				best_individual_index_for_test = best_index_on_validation;
				best_subpopulation_index_for_test = best_subpop_index_on_validation;
				statistics.get_stat_ptr(run_index)->best_program = population[best_subpopulation_index_for_test].individuals[best_individual_index_for_test];
			}
			else{
				
				// copy the validation error from previous generation
				statistics.get_stat_ptr(run_index)->validation_error[generation_index] = statistics.get_stat_ptr(run_index)->validation_error[generation_index - 1];
				statistics.get_stat_ptr(run_index)->validation_num_incorrect[generation_index] = statistics.get_stat_ptr(run_index)->validation_num_incorrect[generation_index - 1];
			}
		}
		else
			statistics.get_stat_ptr(run_index)->best_program = population[best_subpopulation_index].individuals[population[best_subpopulation_index].best_index];

		statistics.get_stat_ptr(run_index)->last_generation = (int)generation_index;
/*
		char tmp_s[50];
		snprintf(tmp_s, 50, "run_%d_gen_%d.txt", run_index, generation_index);
		to_xml_file_current_generation(tmp_s);
*/
		// now copy one from each subpopulation to the next one if better than worst
		unsigned int num_subpops = mep_parameters.get_num_subpopulations();
		
		for (unsigned int d = 0; d < num_subpops; d++) { // din d in d+1
			// choose a random individual from subpopulation d and copy over the worst in d + 1
			unsigned int next_pop_index = (d + 1) % num_subpops;
			unsigned int w = mep_unsigned_int_rand(seeds[d], 0, mep_parameters.get_subpopulation_size());
			if (population[d].individuals[w].compare(
					population[next_pop_index].individuals[population[next_pop_index].worst_index],
													 mep_parameters.get_precision()) < 0) {
				population[next_pop_index].individuals[population[next_pop_index].worst_index] = population[d].individuals[w];
				population[next_pop_index].compute_index_of_the_worst(mep_parameters.get_subpopulation_size(),
																	  mep_parameters.get_precision());
			}
		}

		if (on_generation)
			on_generation();
	}

	if (mep_parameters.get_problem_type() != MEP_PROBLEM_MULTICLASS_CLASSIFICATION || 
		(mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION &&
		mep_parameters.get_error_measure() == MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR))
		statistics.get_stat_ptr(run_index)->best_program.simplify();

	//double num_incorrectly_classified_test;
	//int gene_error_index_test;
	if (test_data.get_num_rows() &&
		test_data.get_num_cols() == training_data.get_num_cols()) {// has target
		//double num_incorrectly_classified;
		double total_test_error;
		double *test_error_per_output = new double[mep_parameters.get_num_outputs()];
		compute_test_error(statistics.get_stat_ptr(run_index)->best_program,
						   test_error_per_output, total_test_error);
		
		statistics.get_stat_ptr(run_index)->test_num_incorrect = total_test_error;
		statistics.get_stat_ptr(run_index)->test_error = total_test_error;
		delete[] test_error_per_output;
	}
	else {
		// does not have target ... here I have to propose the error, class
		// ????????????????????
	}

	delete[] mep_threads;

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;

	statistics.get_stat_ptr(run_index)->running_time = elapsed_seconds.count();

	return true;
}
//---------------------------------------------------------------------------
void t_mep::get_random_subset(unsigned int requested_size,
							  unsigned int* indexes,
							  t_seed& seed) const
{
	unsigned int num_data;
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE)
		num_data = training_data_ts.get_num_rows();
	else
		num_data = training_data.get_num_rows();

	if (num_data == requested_size) {
		// do it faster if you want all data
		for (unsigned int i = 0; i < num_data; i++)
			indexes[i] = i;
	}
	else {
		double p_selection = requested_size / (double)num_data;
		unsigned int count_real = 0;
		for (unsigned int i = 0; i < num_data; i++) {
			double p = mep_real_rand(seed, 0, 1);
			if (p <= p_selection) {
				indexes[count_real] = i;
				count_real++;
				if (count_real == requested_size)
					break;
			}
		}
		while (count_real < requested_size) {
			indexes[count_real] = mep_unsigned_int_rand(seed, 0, num_data);
			count_real++;
		}
	}
}
//-----------------------------------------------------------------------
double t_mep::compute_validation_error(
	unsigned int& best_subpopulation_index_for_validation,
	unsigned int& best_individual_index_for_validation,
	double** eval_double,
	long long** eval_long,
	s_value_class * tmp_value_class,
	char* gene_used_for_output,
	t_seed * seeds,
	double& best_num_incorrectly_classified)
{
	double best_validation_error = -1;
	double validation_error;
	double *validation_error_per_output = new double[mep_parameters.get_num_outputs()];
	unsigned int index_error_gene;
	double num_incorrectly_classified = 100;
	bool result;
	
	t_sub_population &best_sub_pop = population[best_subpopulation_index];

	switch (mep_parameters.get_problem_type()){
		case MEP_PROBLEM_REGRESSION:

			if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
				result = best_sub_pop.individuals[best_sub_pop.best_index].compute_regression_error_on_data_return_error(
					validation_data,
				 validation_error_per_output,
					validation_error,
					index_error_gene, mep_absolute_error_double, mep_absolute_error_long);
			else
				result = best_sub_pop.individuals[best_sub_pop.best_index].compute_regression_error_on_data_return_error(
																	validation_data,
																	validation_error_per_output,
																	validation_error,
																	index_error_gene,
																	mep_squared_error_double, mep_squared_error_long);
			while (!result) {
				// I have to mutate that a_chromosome.
				best_sub_pop.individuals[best_sub_pop.best_index].set_gene_operation(index_error_gene,
					actual_enabled_variables[mep_unsigned_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables)]);
				// recompute its fitness on training;
				if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
					best_sub_pop.individuals[best_sub_pop.best_index].fitness_regression(training_data,
						random_subset_indexes, random_subset_selection_size,
						cached_eval_variables_matrix_double, cached_eval_variables_matrix_long,
						cached_sum_of_errors_for_variables,
						num_actual_variables, actual_enabled_variables,
						eval_double, eval_long, gene_used_for_output,
						mep_absolute_error_double, mep_absolute_error_long,
						seeds[best_subpopulation_index]);
				else
					best_sub_pop.individuals[best_sub_pop.best_index].fitness_regression(training_data,
						random_subset_indexes, random_subset_selection_size,
						cached_eval_variables_matrix_double,
						cached_eval_variables_matrix_long,
																						 cached_sum_of_errors_for_variables,
						num_actual_variables, actual_enabled_variables,
						eval_double, eval_long, gene_used_for_output,
						mep_squared_error_double, mep_squared_error_long, seeds[best_subpopulation_index]);
				best_sub_pop.individuals[best_sub_pop.best_index].count_num_utilized_genes();
				// resort the population
				//sort_by_fitness(pop[best_subpopulation_index]);
				if (best_sub_pop.individuals[best_sub_pop.best_index].compare(
							best_sub_pop.individuals[best_sub_pop.worst_index],
																			  mep_parameters.get_precision()) > 0)// old best could be now worse than the worst
					best_sub_pop.compute_index_of_the_worst(mep_parameters.get_subpopulation_size(),
															mep_parameters.get_precision());
				best_sub_pop.compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
													   mep_parameters.get_precision());
				// apply it again on validation
				if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
					result = best_sub_pop.individuals[best_sub_pop.best_index].compute_regression_error_on_data_return_error(
						validation_data,
					 validation_error_per_output,
						validation_error,
						index_error_gene, mep_absolute_error_double, mep_absolute_error_long);
				else
					result = best_sub_pop.individuals[best_sub_pop.best_index].compute_regression_error_on_data_return_error(
						validation_data,
						 validation_error_per_output,
						validation_error,
						index_error_gene, mep_squared_error_double, mep_squared_error_long);
			}
			// now it is ok; no errors on
			if ((validation_error < best_validation_error) ||
				(fabs(best_validation_error + 1) <= 1E-6)) {
				best_validation_error = validation_error;
				best_subpopulation_index_for_validation = best_subpopulation_index;
				best_individual_index_for_validation = best_sub_pop.best_index;
			}
			break;

		case MEP_PROBLEM_TIME_SERIE: {
			unsigned int window_size = mep_parameters.get_window_size();
			double* previous_data_double = NULL;
			long long* previous_data_long = NULL;
			unsigned int num_outputs = mep_parameters.get_num_outputs();
			if (mep_parameters.get_data_type() == MEP_DATA_LONG_LONG){
				previous_data_long = new long long[window_size * num_outputs];

				compute_previous_data_for_time_series_validation(previous_data_long);

				if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
					result = best_sub_pop.individuals[best_sub_pop.best_index].compute_time_series_error_on_data_return_error(
						previous_data_long, window_size,
						validation_data,
						validation_error_per_output, validation_error, index_error_gene, mep_absolute_error_long);
				else
					result = best_sub_pop.individuals[best_sub_pop.best_index].compute_time_series_error_on_data_return_error(
						previous_data_long, window_size,
						validation_data,
						validation_error_per_output,
						validation_error, index_error_gene, mep_squared_error_long);
			}
			else{// double
				previous_data_double = new double[window_size * num_outputs];

				compute_previous_data_for_time_series_validation(previous_data_double);

				if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
					result = best_sub_pop.individuals[best_sub_pop.best_index].compute_time_series_error_on_data_return_error(
						previous_data_double, window_size,
						validation_data,
						validation_error_per_output,
						validation_error, index_error_gene, mep_absolute_error_double);
				else
					result = best_sub_pop.individuals[best_sub_pop.best_index].compute_time_series_error_on_data_return_error(
						previous_data_double, window_size,
						validation_data,
						validation_error_per_output,
						validation_error, index_error_gene, mep_squared_error_double);
			}
			while (!result) {
				// I have to mutate that a_chromosome.
				best_sub_pop.individuals[best_sub_pop.best_index].set_gene_operation(index_error_gene,
					actual_enabled_variables[mep_unsigned_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables)]);
				// recompute its fitness on training;
				if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
					best_sub_pop.individuals[best_sub_pop.best_index].fitness_regression(
						training_data_ts,
						random_subset_indexes, random_subset_selection_size,
						cached_eval_variables_matrix_double,
						cached_eval_variables_matrix_long,
																						 cached_sum_of_errors_for_variables,
						num_actual_variables, actual_enabled_variables,
						eval_double, eval_long, gene_used_for_output,
						mep_absolute_error_double, mep_absolute_error_long,
						seeds[best_subpopulation_index]);
				else
					best_sub_pop.individuals[best_sub_pop.best_index].fitness_regression(training_data_ts,
						random_subset_indexes, random_subset_selection_size,
						cached_eval_variables_matrix_double, cached_eval_variables_matrix_long,
																						 cached_sum_of_errors_for_variables,
						num_actual_variables, actual_enabled_variables,
						eval_double, eval_long, gene_used_for_output,
						mep_squared_error_double, mep_squared_error_long,
						seeds[best_subpopulation_index]);
				best_sub_pop.individuals[best_sub_pop.best_index].count_num_utilized_genes();
				// resort the population
				//sort_by_fitness(pop[best_subpopulation_index]);
				if (best_sub_pop.individuals[best_sub_pop.best_index].compare(
					best_sub_pop.individuals[best_sub_pop.worst_index],
																			  mep_parameters.get_precision()) > 0)// old best could be now worse than the worst
					best_sub_pop.compute_index_of_the_worst(mep_parameters.get_subpopulation_size(),
															mep_parameters.get_precision());
				best_sub_pop.compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
													   mep_parameters.get_precision());
				// apply it again on validation
				if (mep_parameters.get_data_type() == MEP_DATA_LONG_LONG){
					compute_previous_data_for_time_series_validation(previous_data_long); // I have to recompute it again because it is overwritten!

					if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
						result = best_sub_pop.individuals[best_sub_pop.best_index].compute_time_series_error_on_data_return_error(
																																  previous_data_long, window_size,
							validation_data, validation_error_per_output,
							validation_error, index_error_gene, mep_absolute_error_long);
					else
						result = best_sub_pop.individuals[best_sub_pop.best_index].compute_time_series_error_on_data_return_error(
																																  previous_data_long, window_size,
							validation_data,validation_error_per_output,
							validation_error, index_error_gene, mep_squared_error_long);

				}
				else{
					compute_previous_data_for_time_series_validation(previous_data_double); // I have to recompute it again because it is overwritten!

					if (mep_parameters.get_error_measure() == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
						result = best_sub_pop.individuals[best_sub_pop.best_index].compute_time_series_error_on_data_return_error(
								previous_data_double, window_size,
							validation_data,validation_error_per_output,
							validation_error, index_error_gene, mep_absolute_error_double);
					else
						result = best_sub_pop.individuals[best_sub_pop.best_index].compute_time_series_error_on_data_return_error(
																																  previous_data_double, window_size,
							validation_data,validation_error_per_output,
							validation_error, index_error_gene, mep_squared_error_double);
				}
			}
			if (previous_data_double)
				delete[] previous_data_double;
			if (previous_data_long)
				delete[] previous_data_long;
			
			// now it is ok; no errors on
			if ((validation_error < best_validation_error) ||
				(fabs(best_validation_error + 1) <= 1E-6)) {
				best_validation_error = validation_error;
				best_subpopulation_index_for_validation = best_subpopulation_index;
				best_individual_index_for_validation = best_sub_pop.best_index;
			}
		}
		break;

		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			while (!best_sub_pop.individuals[best_sub_pop.best_index].compute_binary_classification_error_on_double_data_return_error(
						validation_data, validation_error, index_error_gene)) {
							best_sub_pop.individuals[best_sub_pop.best_index].set_gene_operation(index_error_gene,
							actual_enabled_variables[mep_unsigned_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables)]);
				// recompute its fitness on training;
							best_sub_pop.individuals[best_sub_pop.best_index].fitness_binary_classification(training_data,
						random_subset_indexes, random_subset_selection_size,
						cached_eval_variables_matrix_double, cached_sum_of_errors_for_variables,
						cached_threashold, num_actual_variables,
						actual_enabled_variables, eval_double, tmp_value_class,
						seeds[best_subpopulation_index]);
				best_sub_pop.individuals[best_sub_pop.best_index].count_num_utilized_genes();
				// resort the population
				//sort_by_fitness(pop[best_subpopulation_index]);
				if (best_sub_pop.individuals[best_sub_pop.best_index].compare(
					best_sub_pop.individuals[best_sub_pop.worst_index],
																			  mep_parameters.get_precision()) > 0)
					best_sub_pop.compute_index_of_the_worst(mep_parameters.get_subpopulation_size(),
															mep_parameters.get_precision());
				best_sub_pop.compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
													   mep_parameters.get_precision());

			}
			if ((validation_error < best_validation_error) ||
				(fabs(best_validation_error + 1) <= 1E-6)) {
				best_validation_error = validation_error;
				best_subpopulation_index_for_validation = best_subpopulation_index;
				best_individual_index_for_validation = best_sub_pop.best_index;
				best_num_incorrectly_classified = validation_error;
			}
			break;

		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:

			switch (mep_parameters.get_error_measure()) {
				case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
					while (!best_sub_pop.individuals[best_sub_pop.best_index].compute_multi_class_classification_error_on_double_data_return_error(
						validation_data, validation_error, index_error_gene, num_incorrectly_classified)) {
						best_sub_pop.individuals[best_sub_pop.best_index].set_gene_operation(index_error_gene,
								actual_enabled_variables[mep_unsigned_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables)]);
						// recompute its fitness on training;
						best_sub_pop.individuals[best_sub_pop.best_index].fitness_multi_class_classification_winner_takes_all_fixed(
								training_data, random_subset_indexes,
							random_subset_selection_size,
								cached_eval_variables_matrix_double, num_actual_variables,
								actual_enabled_variables, eval_double,
								seeds[best_subpopulation_index]);
						best_sub_pop.individuals[best_sub_pop.best_index].count_num_utilized_genes();

						// resort the population
						//sort_by_fitness(pop[best_subpopulation_index]);
						best_sub_pop.compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
															   mep_parameters.get_precision());
						if (best_sub_pop.individuals[best_sub_pop.best_index].compare(
							best_sub_pop.individuals[best_sub_pop.worst_index],
																					  mep_parameters.get_precision()) > 0)
							best_sub_pop.compute_index_of_the_worst(mep_parameters.get_subpopulation_size(),
																	mep_parameters.get_precision());

					}
					if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
						best_validation_error = validation_error;
						best_num_incorrectly_classified = num_incorrectly_classified;
						best_subpopulation_index_for_validation = best_subpopulation_index;
						best_individual_index_for_validation = best_sub_pop.best_index;
					}

					break;
				case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
					while (!best_sub_pop.individuals[best_sub_pop.best_index].compute_multi_class_classification_error_on_double_data_return_error(
						validation_data, validation_error, index_error_gene, num_incorrectly_classified)) {
						best_sub_pop.individuals[best_sub_pop.best_index].set_gene_operation(index_error_gene,
								actual_enabled_variables[mep_unsigned_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables)]);
						// recompute its fitness on training;
						best_sub_pop.individuals[best_sub_pop.best_index].fitness_multi_class_classification_smooth(training_data,
								random_subset_indexes,
							random_subset_selection_size,
								cached_eval_variables_matrix_double, num_actual_variables,
								actual_enabled_variables, eval_double, seeds[best_subpopulation_index]);
						best_sub_pop.individuals[best_sub_pop.best_index].count_num_utilized_genes();
							// resort the population
						//sort_by_fitness(pop[best_subpopulation_index]);
						if (best_sub_pop.individuals[best_sub_pop.best_index].compare(
							best_sub_pop.individuals[best_sub_pop.worst_index],
																					  mep_parameters.get_precision()) > 0)
							best_sub_pop.compute_index_of_the_worst(mep_parameters.get_subpopulation_size(),
																	mep_parameters.get_precision());
						best_sub_pop.compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
															   mep_parameters.get_precision());
					}
					if ((validation_error < best_validation_error) ||
						(fabs(best_validation_error + 1) <= 1E-6)) {
						best_validation_error = validation_error;
						best_num_incorrectly_classified = num_incorrectly_classified;
						best_subpopulation_index_for_validation = best_subpopulation_index;
						best_individual_index_for_validation = best_sub_pop.best_index;
					}

					break;
				case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
					while (!best_sub_pop.individuals[best_sub_pop.best_index].compute_multi_class_classification_winner_takes_all_dynamic_error_on_double_data_return_error(
						validation_data, validation_error, index_error_gene, num_incorrectly_classified)) {
						best_sub_pop.individuals[best_sub_pop.best_index].set_gene_operation(index_error_gene,
								actual_enabled_variables[mep_unsigned_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables)]);
						// recompute its fitness on training;
						best_sub_pop.individuals[best_sub_pop.best_index].fitness_multi_class_classification_winner_takes_all_dynamic(
								training_data, random_subset_indexes,
							random_subset_selection_size,
								cached_eval_variables_matrix_double, num_actual_variables,
							actual_enabled_variables, eval_double,
							seeds[best_subpopulation_index]);
						best_sub_pop.individuals[best_sub_pop.best_index].count_num_utilized_genes();
						// resort the population
						//sort_by_fitness(pop[best_subpopulation_index]);
						if (best_sub_pop.individuals[best_sub_pop.best_index].compare(
							best_sub_pop.individuals[best_sub_pop.worst_index],
																					  mep_parameters.get_precision()) > 0)
							best_sub_pop.compute_index_of_the_worst(mep_parameters.get_subpopulation_size(),
																	mep_parameters.get_precision());
						best_sub_pop.compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
															   mep_parameters.get_precision());

					}
					if ((validation_error < best_validation_error) || (fabs(best_validation_error + 1) <= 1E-6)) {
						best_validation_error = validation_error;
						best_num_incorrectly_classified = num_incorrectly_classified;
						best_subpopulation_index_for_validation = best_subpopulation_index;
						best_individual_index_for_validation = best_sub_pop.best_index;
					}

					break;
				case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
					while (!best_sub_pop.individuals[best_sub_pop.best_index].compute_multi_class_classification_closest_center_error_on_double_data_return_error(
						validation_data, validation_error, index_error_gene, num_incorrectly_classified)) {
						best_sub_pop.individuals[best_sub_pop.best_index].set_gene_operation(index_error_gene,
							actual_enabled_variables[mep_unsigned_int_rand(seeds[best_subpopulation_index], 0, num_actual_variables)]);
						// recompute its fitness on training;
						best_sub_pop.individuals[best_sub_pop.best_index].fitness_multi_class_classification_closest_center(
							training_data, random_subset_indexes,
							random_subset_selection_size,
							cached_eval_variables_matrix_double, num_actual_variables,
							actual_enabled_variables, eval_double, seeds[best_subpopulation_index]);
						// resort the population
						//sort_by_fitness(pop[best_subpopulation_index]);
						if (best_sub_pop.individuals[best_sub_pop.best_index].compare(
							best_sub_pop.individuals[best_sub_pop.worst_index],
																					  mep_parameters.get_precision()) > 0)
							best_sub_pop.compute_index_of_the_worst(mep_parameters.get_subpopulation_size(),
																	mep_parameters.get_precision());
						best_sub_pop.compute_index_of_the_best(mep_parameters.get_subpopulation_size(),
															   mep_parameters.get_precision());
					}
					if ((validation_error < best_validation_error) ||
						(fabs(best_validation_error + 1) <= 1E-6)) {
						best_validation_error = validation_error;
						best_num_incorrectly_classified = num_incorrectly_classified;
						best_subpopulation_index_for_validation = best_subpopulation_index;
						best_individual_index_for_validation = best_sub_pop.best_index;
					}

					break;
			}// end switch (mep_parameters.get_error_measure()) {
			break;
	}
	delete[] validation_error_per_output;
	return best_validation_error;
}
//---------------------------------------------------------------------------

bool t_mep::is_running(void) const
{
	return !_stopped;
}
//---------------------------------------------------------------------------
int t_mep::get_last_run_index(void) const
{
	return last_run_index;
}
//---------------------------------------------------------------------------
void t_mep::clear_stats(void)
{
	if (_stopped) {
		if (last_run_index != -1) {
			modified_project = true;

			last_run_index = -1;
			statistics.delete_memory();
		}
	}
}
//---------------------------------------------------------------------------
char* t_mep::program_as_C(unsigned int run_index,
						  bool simplified,
						  double** inputs_double,
						  long long** inputs_long) const
{
	return get_stats_ptr()->get_stat_ptr(run_index)->best_program.to_C_code(
			simplified, inputs_double, inputs_long, version);
}
//---------------------------------------------------------------------------
char* t_mep::program_as_C_infix(unsigned int run_index,
								double** inputs_double,
								long long** inputs_long) const
{
	return get_stats_ptr()->get_stat_ptr(run_index)->best_program.to_C_infix_code(
			inputs_double, inputs_long,
		version);
}
//---------------------------------------------------------------------------
char* t_mep::program_as_Latex(unsigned int run_index) const
{
	return get_stats_ptr()->get_stat_ptr(run_index)->best_program.to_Latex_code(version);
}
//---------------------------------------------------------------------------
char* t_mep::program_as_Excel_function(unsigned int run_index,
									   bool simplified)const
{
	return get_stats_ptr()->get_stat_ptr(run_index)->best_program.to_Excel_VBA_function_code(
		simplified, version);
}
//---------------------------------------------------------------------------
char* t_mep::program_as_Python(unsigned int run_index,
							   bool simplified, double** inputs_double,
							   long long** inputs_long)const
{
	return get_stats_ptr()->get_stat_ptr(run_index)->best_program.to_Python_code(
								simplified,
								inputs_double, inputs_long,
								version
	);
}
//---------------------------------------------------------------------------
void t_mep::set_num_total_variables(unsigned int value)
{
	if (_stopped) {
		num_total_variables = value;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
void t_mep::init(void)
{
	if (_stopped) {
		mep_parameters.init();
		mep_operators.init();
		mep_constants.init();

		if (actual_enabled_variables) {
			delete[] actual_enabled_variables;
			actual_enabled_variables = NULL;
		}
		if (variables_enabled) {
			delete[] variables_enabled;
			variables_enabled = NULL;
		}

		num_actual_variables = 0;

		if (problem_description) {
			delete[] problem_description;
			problem_description = NULL;
		}

		problem_description = new char[100];
		strcpy(problem_description, "Problem description here ...");

		modified_project = false;
	}
}
//---------------------------------------------------------------------------
void t_mep::compute_list_of_enabled_variables(void)
{
	if (actual_enabled_variables) {
		delete[] actual_enabled_variables;
		actual_enabled_variables = NULL;
	}

	num_actual_variables = 0;

	if (training_data.get_num_cols() == 0) {
		num_total_variables = 0;
		return;
	}
	
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE) {
		if (training_data_ts.get_num_cols() == 0)
			num_total_variables = 0;
		else
			num_total_variables = training_data_ts.get_num_cols() - mep_parameters.get_num_outputs();

		if (variables_enabled) { // from a previous run
			delete[] variables_enabled;
			variables_enabled = NULL;
		}

		if (num_total_variables == 0)
			return;

		variables_enabled = new bool[num_total_variables];
		//num_actual_variables = num_total_variables;
		for (unsigned int i = 0; i < num_total_variables; i++) {
			variables_enabled[i] = 1;
			//actual_enabled_variables[i] = i;
		}
	}
	else{
		if (mep_parameters.get_problem_type() == MEP_PROBLEM_REGRESSION) {
			num_total_variables = training_data.get_num_cols() - mep_parameters.get_num_outputs();
		}
		else{// classification problems
			num_total_variables = training_data.get_num_cols() - 1;
		}
	}
		
	if (num_total_variables) {
		actual_enabled_variables = new unsigned int[num_total_variables];
		for (unsigned int i = 0; i < num_total_variables; i++)
			if (variables_enabled[i]) {
				actual_enabled_variables[num_actual_variables] = i;
				num_actual_variables++;
			}
	}
}
//---------------------------------------------------------------------------
void t_mep::init_enabled_variables(void)
{
	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}

	if (training_data.get_num_cols() < 1)
		num_total_variables = 0;
	else {
		num_total_variables = training_data.get_num_cols() - 1;
	}

	num_actual_variables = num_total_variables;

	if (num_total_variables == 0)
		return;

	variables_enabled = new bool[num_total_variables];

	for (unsigned int i = 0; i < num_total_variables; i++)
		variables_enabled[i] = 1;
}

//---------------------------------------------------------------------------
unsigned int t_mep::get_num_actual_variables(void) const
{
	return num_actual_variables;
}
//---------------------------------------------------------------------------
bool t_mep::is_variable_enabled(unsigned int index) const
{
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE)
		return true;
	else
		return variables_enabled[index];
}
//---------------------------------------------------------------------------
void t_mep::set_variable_enable(unsigned int index, bool new_state)
{
	if (_stopped) {
		variables_enabled[index] = new_state;
		modified_project = true;
	}
}
//---------------------------------------------------------------------------
bool t_mep::is_project_modified(void) const
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
char* t_mep::get_problem_description(void) const
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
bool t_mep::get_enable_cache_results_for_all_training_data(void) const
{
	return cache_results_for_all_training_data;
}
//---------------------------------------------------------------------------
long long t_mep::get_memory_consumption(void) const
{
	// for chromosomes
	//	long long chromosomes_memory = 0;

	return 0;
}
//---------------------------------------------------------------------------
bool t_mep::validate_project(char* error_message, size_t buffer_size)
{
	if (training_data.get_num_rows() == 0) {
		strcpy(error_message, "There are no training data! Please enter some data!");
		return false;
	}
	
	int tmp_num_selected_operators = mep_operators.count_operators();
	if (tmp_num_selected_operators == 0) {
		strcpy(error_message, "No function is selected!");
		return false;
	}
	
	if (training_data.get_data_type() == MEP_DATA_STRING) {
		strcpy(error_message, "Training data: some data are alphanumeric. Please convert them to numeric values first (by pressing the <<Advanced commands|To numeric>> button).");
		return false;
	}

	if (validation_data.get_data_type() == MEP_DATA_STRING &&
		validation_data.get_num_rows()) {
		strcpy(error_message, "Validation data: some data are alphanumeric. Please convert them to numeric values first (by pressing the <<Advanced commands|To numeric>> button).");
		return false;
	}

	if (test_data.get_data_type() == MEP_DATA_STRING &&
		test_data.get_num_rows()) {
		strcpy(error_message, "Test data: some data are alphanumeric. Please convert them to numeric values first (by pressing the <<Advanced commands|To numeric>> button).");
		return false;
	}
	
	if (mep_parameters.get_data_type() == MEP_DATA_LONG_LONG &&
		(mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION ||
		 mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION)){
		strcpy(error_message, "Integer data type cannot be used for classification problems! Please switch to Real data type.");
		return false;
	}

	// compute num. classes
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION ||
			mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION) {

		if (!training_data.get_class_labels_ptr()){ // the labels have not been extracted yet
			training_data.compute_class_labels(mep_parameters.get_num_outputs());
			validation_data.assign_class_index_from_training_class_labels(training_data.get_class_labels_ptr(), training_data.get_num_classes() );
			test_data.assign_class_index_from_training_class_labels(training_data.get_class_labels_ptr(), training_data.get_num_classes());
		}
			
		//mep_alg.get_training_data_ptr()->count_num_classes(mep_alg.get_training_data_ptr()->get_num_cols() - 1);
		//mep_alg.get_validation_data_ptr()->count_num_classes(mep_alg.get_validation_data_ptr()->get_num_cols() - 1);
		//mep_alg.test_data.count_num_classes(mep_alg.test_data.get_num_cols() - 1);
	}
		
	// if the problem is of classification, the target must have 2 classes only
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION) {
		//training_data.
		if (training_data.get_num_rows() &&
			training_data.get_num_classes() != 2) {
			strcpy(error_message, "For binary classification problems the training data must have 2 classes !");
			return false;
		}
/*
		if (mep_alg.get_validation_data_ptr()->get_num_rows() &&
			mep_alg.get_validation_data_ptr()->get_num_classes() > 2) {

			strcpy(error_message, "The target for validation data target must be 0 or 1 for binary classification problems!");
			return false;
		}

		if (test_data.get_num_cols() == mep_alg.get_training_data_ptr()->get_num_cols()) {
			if (test_data.get_num_rows() &&
				test_data.get_num_classes() > 2) {
				strcpy(error_message, "The target for test data must be 0 or 1 for binary classification problems!");
				return false;
			}
		}
		*/
	}
	else
		if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION) {

			/*
			if (get_training_data_ptr()->get_num_rows() &&
				!get_training_data_ptr()->is_multi_class_classification_problem()) {
				// this looks like repeated from above
				strcpy(error_message, "The target for training data must be integer for multi-class classification problems!");
				return false;
			}
			
			if (.get_validation_data_ptr()->get_num_rows() &&
				!.get_validation_data_ptr()->is_multi_class_classification_problem_within_range(mep_alg.get_training_data_ptr()->get_num_classes() - 1)) {

				strcpy(error_message, "The target for validation data must be 0, 1... num_classes - 1 for multi-class classification problems!");
				return false;
			}

			if (.test_data.get_num_cols() == mep_alg.get_training_data_ptr()->get_num_cols()) {
				if (.test_data.get_num_rows() &&
					!.test_data.is_multi_class_classification_problem_within_range(mep_alg.get_training_data_ptr()->get_num_classes() - 1)) {
					strcpy(error_message, "The target for test data must be 0, 1... num_classes - 1 for multi-class classification problems!");
					return false;
				}
			}
			*/
			// this part must be replaced by another part where I should test for
		}

	if (mep_parameters.get_num_subpopulations() < mep_parameters.get_num_threads()) {
		strcpy(error_message, "Number of sub-populations must be greater or equal to the number of threads.");
		return false;
	}

	if (mep_parameters.get_constants_probability() > 0) {
		if (mep_constants.get_constants_type() == MEP_CONSTANTS_USER_DEFINED &&
			!mep_constants.get_num_user_defined_constants()) {
			strcpy(error_message, "If constant's type is user defined, you must enter some constants!");
			return false;
		}
		if (mep_constants.get_constants_type() == MEP_CONSTANTS_AUTOMATIC &&
			!mep_constants.get_num_automatic_constants()) {
			strcpy(error_message, "If constant's type is program generated, you must set the number of constants to a value greater than 0!");
			return false;
		}

		if (mep_constants.get_constants_type() == MEP_CONSTANTS_AUTOMATIC &&
			mep_constants.get_min_constants_interval_double() >= mep_constants.get_max_constants_interval_double()) {
			strcpy(error_message, "The lower bound for constant's interval must be smaller than upper bound!");
			return false;
		}
	}


	// num. vars in training, validation and testing must be the same.

	if (mep_parameters.get_problem_type() != MEP_PROBLEM_TIME_SERIE) {
		if (get_validation_data_ptr()->get_num_cols() &&
			(get_training_data_ptr()->get_num_cols() != get_validation_data_ptr()->get_num_cols())) {
			strcpy(error_message, "Validation data and training data must have the same number of variables!");
			return false;
		}

		if (get_training_data_ptr()->get_num_cols() < test_data.get_num_cols()) {
			strcpy(error_message, "Testing data cannot have more variables than the training data!");
			return false;
		}
		// in testing can be 1 less column compared to the training.
		if (test_data.get_num_cols() &&
			(get_training_data_ptr()->get_num_cols() != test_data.get_num_cols()) &&
			 (get_training_data_ptr()->get_num_cols() - mep_parameters.get_num_outputs() != test_data.get_num_cols())
			) {
			strcpy(error_message, "Incorrect test data! The number of columns must be equal to the number of columns of the training data or equal to the number of columns of the training data minus the number of outputs!");
			return false;
		}
	}
	/*
	else {// time series
		if (get_validation_data_ptr()->get_num_cols() != 0 &&
			get_validation_data_ptr()->get_num_cols() != 1) {
			strcpy(error_message, "Validation data can have 0 or 1 columns!");
			return false;
		}
		// in testing can be 1 less column compared to the training.
		if (test_data.get_num_cols() != 0 &&
			test_data.get_num_cols() != 1) {
			strcpy(error_message, "Testing data can have 0 or 1 columns!");
			return false;
		}
	}
*/
	if (mep_parameters.get_random_subset_selection_size_percent() > 100) {
		snprintf(error_message, buffer_size, "Random subset size cannot be larger than 100 percent!");
		return false;
	}

	if (mep_parameters.get_random_subset_selection_size_percent() < 1) {
		snprintf(error_message, buffer_size, "Random subset size cannot be less than 1 percent!");
		return false;
	}
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION ||
		mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION) {
		if (!training_data.are_all_output_int()){
			snprintf(error_message, buffer_size, "Training data output must be integer!");
			return false;
		}
		if (!validation_data.are_all_output_int()){
			snprintf(error_message, buffer_size, "Validation data output must be integer!");
			return false;
		}
		if (training_data.get_num_cols() == test_data.get_num_cols()){
			if (!test_data.are_all_output_int()){
				snprintf(error_message, buffer_size, "Test data output must be integer!");
				return false;
			}
		}
	}
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION ||
		mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION) {
		if (mep_parameters.get_num_outputs() != 1){
			snprintf(error_message, buffer_size, "The number of outputs for classification problems must be 1!");
			return false;
		}
	}
	
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION ||
		mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION) {
		training_data.compute_class_labels(mep_parameters.get_num_outputs());
		validation_data.assign_class_index_from_training_class_labels(training_data.get_class_labels_ptr(), training_data.get_num_classes() );
		test_data.assign_class_index_from_training_class_labels(training_data.get_class_labels_ptr(), training_data.get_num_classes());
	}
	
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION) {
		if (mep_parameters.get_code_length() < training_data.get_num_classes()) {
			snprintf(error_message, buffer_size, "Code length cannot be less than the number of classes!");
			return false;
		}
	}
	else
		if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE) {
			if (mep_parameters.get_num_outputs() != training_data.get_num_cols()) {
				snprintf(error_message, buffer_size, "For time series all columns must be set as targets (outputs)! Currently, the number of outputs is set to %d", mep_parameters.get_num_outputs());
				return false;
			}
			else
				if (training_data.get_num_rows() <= mep_parameters.get_window_size()) {
					snprintf(error_message, buffer_size, "Window size must be smaller than number of training data!");
					return false;
				}
		}
	
	// now try to do some tests related to data type
	if (mep_parameters.get_data_type() == MEP_DATA_LONG_LONG){
		// try to translate everything into long long
		if (!mep_constants.set_data_type(MEP_DATA_LONG_LONG)){
			snprintf(error_message, buffer_size, "Constants cannot be converted into integer numbers!");
			return false;
		}
		if (!training_data.set_data_type(MEP_DATA_LONG_LONG)){
			snprintf(error_message, buffer_size, "Some training data cannot be converted into integer numbers!");
			return false;
		}
		if (!validation_data.set_data_type(MEP_DATA_LONG_LONG)){
			snprintf(error_message, buffer_size, "Some validation data cannot be converted into integer numbers!");
			return false;
		}
		if (!test_data.set_data_type(MEP_DATA_LONG_LONG)){
			snprintf(error_message, buffer_size, "Some test data cannot be converted into integer numbers!");
			return false;
		}
	}
	else{
		mep_constants.set_data_type(MEP_DATA_DOUBLE);
		training_data.set_data_type(MEP_DATA_DOUBLE);
		validation_data.set_data_type(MEP_DATA_DOUBLE);
		test_data.set_data_type(MEP_DATA_DOUBLE);
	}
	
	if (mep_parameters.get_code_length() < mep_parameters.get_num_outputs()){
		snprintf(error_message, buffer_size, "Code length cannot be less than the number of outputs!");
		return false;
	}
		
	return true;
}
//---------------------------------------------------------------------------
t_mep_data* t_mep::get_training_data_ptr(void)
{
	return &training_data;
}
//---------------------------------------------------------------------------
t_mep_data* t_mep::get_validation_data_ptr(void)
{
	return &validation_data;
}
//---------------------------------------------------------------------------
t_mep_data* t_mep::get_test_data_ptr(void)
{
	return &test_data;
}
//---------------------------------------------------------------------------
t_mep_functions* t_mep::get_functions_ptr(void)
{
	return &mep_operators;
}
//---------------------------------------------------------------------------
t_mep_constants* t_mep::get_constants_ptr(void)
{
	return &mep_constants;
}
//---------------------------------------------------------------------------
t_mep_parameters* t_mep::get_parameters_ptr(void)
{
	return &mep_parameters;
}
//---------------------------------------------------------------------------
void t_mep::stop(void)
{
	_stopped_signal_sent = true;
}
//---------------------------------------------------------------------------
const t_mep_all_runs_statistics* t_mep::get_stats_ptr(void)const
{
	return &statistics;
}
//---------------------------------------------------------------------------
const char* t_mep::get_version(void) const
{
	return version;
}
//---------------------------------------------------------------------------
bool t_mep::could_be_univariate_time_serie(void) const
{
	return training_data.could_be_univariate_time_serie() &&
		(!validation_data.get_num_rows() ||
		(validation_data.get_num_rows() && validation_data.get_num_cols () == 1))	&&
		(!test_data.get_num_rows() ||
		(test_data.get_num_rows() && test_data.get_num_cols() == 1));
}
//---------------------------------------------------------------------------
unsigned int t_mep::get_num_total_variables(void) const
{
	return num_total_variables;
}
//---------------------------------------------------------------------------
// 1653
// 1452
// 2177
// 2054
// 2314
