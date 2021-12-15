// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef mep_parameters_H
#define mep_parameters_H

//-----------------------------------------------------------------
#include "pugixml.hpp"
#include "mep_constants.h"

//-----------------------------------------------------------------
#define MEP_PROBLEM_REGRESSION 0
#define MEP_PROBLEM_BINARY_CLASSIFICATION 1
#define MEP_PROBLEM_MULTICLASS_CLASSIFICATION 2
#define MEP_PROBLEM_TIME_SERIE 3
//-----------------------------------------------------------------
#define MEP_UNIFORM_CROSSOVER 0
#define MEP_ONE_CUTTING_POINT_CROSSOVER 1
//-----------------------------------------------------------------
#define MEP_REGRESSION_MEAN_ABSOLUTE_ERROR 0
#define MEP_REGRESSION_MEAN_SQUARED_ERROR 1
#define MEP_BINARY_CLASSIFICATION_BEST_THRESHOLD 2
#define MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR 3
#define MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR 4
#define MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR 5
//-----------------------------------------------------------------
#define MEP_TIME_SERIES_TEST 0
#define MEP_TIME_SERIES_PREDICTION 1
//-----------------------------------------------------------------
class t_mep_parameters{
private:
    double mutation_probability;                // mutation probability
    double crossover_probability;               // crossover probability
	unsigned int code_length;							// the number of genes
    
	unsigned int subpopulation_size;                 // the number of individuals in population  (must be an odd number!!!)
	unsigned int num_threads;
	unsigned int tournament_size;
	unsigned int num_generations;
	unsigned int problem_type; // 0- regression, 1-binary classification, 2- multi class, 3- time series
    
	unsigned int num_subpopulations;
	double operators_probability, variables_probability, constants_probability;
	bool use_validation_data;
	unsigned int crossover_type;

	unsigned int random_subset_selection_size;
	unsigned int num_generations_for_which_random_subset_is_kept_fixed;

	unsigned int random_seed;
	unsigned int num_runs;

	bool simplified_programs;

	bool modified;

	unsigned int error_measure;
	unsigned int num_predictions;

	unsigned int time_series_mode;

public:

	t_mep_parameters(void);
    void init (void);
    int to_xml(pugi::xml_node parent);
    int from_xml(pugi::xml_node parent);

	bool operator ==(const t_mep_parameters&);

	// returns the mutation probability
	double get_mutation_probability(void) const;

	// returns crossover probability
	double get_crossover_probability(void) const;

	// returns the length of a chromosome (the number of genes)
	unsigned int get_code_length(void) const;

	// returns the number of individuals in a (sub)population
	unsigned int get_subpopulation_size(void) const;

	// returns the number of threads of the program. On each thread a subpopulation is evolved
	unsigned int get_num_threads(void) const;

	// returns the tournament size
	unsigned int get_tournament_size(void) const;

	// returns the number of generations
	unsigned int get_num_generations(void) const;

	// returns the problem type
	// 0 - symbolic regression, 
	// 1 - classification
	unsigned int get_problem_type(void) const;

	// returns the number of sub-populations
	unsigned int get_num_subpopulations(void) const;

	// returns the probability of operators occurence
	double get_operators_probability(void) const;

	// returns the probability of variables occurence
	double get_variables_probability(void) const;

	// returns the probability of constants occurence
	double get_constants_probability(void) const;

	// returns true if the validation data is used
	bool get_use_validation_data(void) const;

	// returns the crossover type 
	// 0 UNIFORM_CROSSOVER
	// 1 ONE_CUTTING_POINT_CROSSOVER
	unsigned int get_crossover_type(void) const;

	// returns the seed for generating random numbers
	unsigned int get_random_seed(void) const;

	// returns the number of runs
	unsigned int get_num_runs(void) const;

	// returns true if the programs are returned in the simplified form (introns are removed)
	bool get_simplified_programs(void) const;

	// sets the mutation probability
	void set_mutation_probability(double value);

	// sets the crossover probability
	void set_crossover_probability(double value);

	// sets the number of genes in a chromosome
	void set_code_length(unsigned int value);

	// sets the number of individuals in population
	void set_subpopulation_size(unsigned int value);

	// sets the number of threads
	void set_num_threads(unsigned int value);

	// sets the tournament size
	void set_tournament_size(unsigned int value);

	// sets the number of generations
	void set_num_generations(unsigned int value);

	// sets the problem type
	// 0- regression, 
	// 1- binary classification
	// 2- multi class classification
	// 3- time series 
	void set_problem_type(unsigned int value);

	// sets the number of subpopulations
	void set_num_subpopulations(unsigned int value);

	// sets the operators probability
	void set_operators_probability(double value);

	// sets the variables probability
	void set_variables_probability(double value);

	// sets the constants probability
	void set_constants_probability(double value);

	// sets the utilization of validation data
	void set_use_validation_data(bool value);

	// sets the crossover type
	void set_crossover_type(unsigned int value);

	// sets the random seed
	void set_random_seed(unsigned int value);

	// sets the number of runs
	void set_num_runs(unsigned int value);

	// sets the simplified programs parameters
	void set_simplified_programs(bool value);

	// set the size of the random subset on which the training is performed.
	// this must be called after calling set_training_data from the t_mep class because set_training_data sets this value to the size of the training data
	void set_random_subset_selection_size(unsigned int value);

	// returns the size of the random subset on which the training is performed
	unsigned int get_random_subset_selection_size(void) const;

	// sets the error measure type
	// for regression and time series it can be: MEP_REGRESSION_MEAN_ABSOLUTE_ERROR 0 or MEP_REGRESSION_MEAN_SQUARED_ERROR 1
	// for classification it can be: MEP_CLASSIFICATION_MEAN_ERROR 2
	void set_error_measure(unsigned int value);

	// returns the error measure type
	// for regression and time series it can be: MEP_REGRESSION_MEAN_ABSOLUTE_ERROR 0 or MEP_REGRESSION_MEAN_SQUARED_ERROR 1
	// for classification it can be: MEP_CLASSIFICATION_MEAN_ERROR 2
	unsigned int get_error_measure(void) const;

	// get the number of generations for which the random subset of data is kept fixed
	unsigned int get_num_generations_for_which_random_subset_is_kept_fixed(void) const;

	// set the number of generations for which the random subset of data is kept fixed
	void set_num_generations_for_which_random_subset_is_kept_fixed(unsigned int);

	// get the number of generations for which the random subset of data is kept fixed
	unsigned int get_num_predictions(void) const;

	// set the number of generations for which the random subset of data is kept fixed
	void set_num_predictions(unsigned int);

	// get time series mode
	unsigned int get_time_series_mode(void) const;

	// set time series mode
	void set_time_series_mode(unsigned int new_value);
};
//-----------------------------------------------------------------
#endif // PARAMETERS_CLASS_H_INCLUDED