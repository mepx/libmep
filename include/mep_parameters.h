#ifndef PARAMETERS_CLASS_H_INCLUDED
#define PARAMETERS_CLASS_H_INCLUDED

#include "pugixml.hpp"
#include "mep_constants.h"

#define MEP_PROBLEM_REGRESSION 0
#define MEP_PROBLEM_BINARY_CLASSIFICATION 1
#define MEP_PROBLEM_MULTICLASS_CLASSIFICATION 2


#define MEP_UNIFORM_CROSSOVER 0
#define MEP_ONE_CUTTING_POINT_CROSSOVER 1

#define MEP_REGRESSION_MEAN_ABSOLUTE_ERROR 0
#define MEP_REGRESSION_MEAN_SQUARED_ERROR 1
#define MEP_CLASSIFICATION_MEAN_ERROR 2


//-----------------------------------------------------------------
class t_mep_parameters
{
private:
    double mutation_probability;                   // mutation probability
    double crossover_probability;               // crossover probability
    long code_length;              // the number of genes
    
	long subpopulation_size;                 // the number of individuals in population  (must be an odd number!!!)
    long num_threads;
	long tournament_size;
	long num_generations;
    int problem_type; // 0- regression, 1-classification
    
	long num_subpopulations;
	double operators_probability, variables_probability, constants_probability;
	bool use_validation_data;
	int crossover_type;

	int random_subset_selection_size;
	unsigned int num_generations_for_which_random_subset_is_kept_fixed;

	long random_seed;
	unsigned int num_runs;

	bool simplified_programs;

	bool modified;

	int error_measure;

public:

	t_mep_parameters(void);
    void init (void);
    int to_xml(pugi::xml_node parent);
    int from_xml(pugi::xml_node parent);

	// returns the mutation probability
	double get_mutation_probability(void);

	// returns crossover probability
	double get_crossover_probability(void);

	// returns the length of a chromosome (the number of genes)
	long get_code_length(void);

	// returns the number of individuals in a (sub)population
	long get_subpopulation_size(void);

	// returns the number of threads of the program. On each thread a subpopulation is evolved
	long get_num_threads(void);

	// returns the tournament size
	long get_tournament_size(void);

	// returns the number of generations
	long get_num_generations(void);

	// returns the problem type
	// 0 - symbolic regression, 
	// 1 - classification
	long get_problem_type(void);

	// returns the number of sub-populations
	long get_num_subpopulations(void);

	// returns the probability of operators occurence
	double get_operators_probability(void);

	// returns the probability of variables occurence
	double get_variables_probability(void);

	// returns the probability of constants occurence
	double get_constants_probability(void);

	// returns true if the validation data is used
	bool get_use_validation_data(void);

	// returns the crossover type 
	// 0 UNIFORM_CROSSOVER
	// 1 ONE_CUTTING_POINT_CROSSOVER
	int get_crossover_type(void);

	// returns the seed for generating random numbers
	long get_random_seed(void);

	// returns the number of runs
	long get_num_runs(void);

	// returns true if the programs are returned in the simplified form (introns are removed)
	bool get_simplified_programs(void);

	// sets the mutation probability
	void set_mutation_probability(double value);

	// sets the crossover probability
	void set_crossover_probability(double value);

	// sets the number of genes in a chromosome
	void set_code_length(long value);

	// sets the number of individuals in population
	void set_subpopulation_size(long value);

	// sets the number of threads
	void set_num_threads(long value);

	// sets the tournament size
	void set_tournament_size(long value);

	// sets the number of generations
	void set_num_generations(long value);

	// sets the problem type
	// 0- regression, 
	// 1-classification
	void set_problem_type(long value);

	// sets the number of subpopulations
	void set_num_subpopulations(long value);

	// sets the operators probability
	void set_operators_probability(double value);

	// sets the variables probability
	void set_variables_probability(double value);

	// sets the constants probability
	void set_constants_probability(double value);

	// sets the utilization of validation data
	void set_use_validation_data(bool value);

	// sets the crossover type
	void set_crossover_type(int value);

	// sets the random seed
	void set_random_seed(long value);

	// sets the number of runs
	void set_num_runs(long value);

	// sets the simplified programs parameters
	void set_simplified_programs(bool value);

	// set the size of the random subset on which the training is performed.
	// this must be called after calling set_training_data from the t_mep class because set_training_data sets this value to the size of the training data
	void set_random_subset_selection_size(int value);

	// returns the size of the random subset on which the training is performed
	int get_random_subset_selection_size(void);

	// sets the error measure type
	// for regression it can be: MEP_REGRESSION_MEAN_ABSOLUTE_ERROR 0 or MEP_REGRESSION_MEAN_SQUARED_ERROR 1
	// for classification it can be: MEP_CLASSIFICATION_MEAN_ERROR 2
	void set_error_measure(int value);

	// returns the error measure type
	// for regression it can be: MEP_REGRESSION_MEAN_ABSOLUTE_ERROR 0 or MEP_REGRESSION_MEAN_SQUARED_ERROR 1
	// for classification it can be: MEP_CLASSIFICATION_MEAN_ERROR 2
	int get_error_measure(void);

	// get the number of generations for which the random subset of data is kept fixed
	unsigned int get_num_generations_for_which_random_subset_is_kept_fixed(void);

	// set the number of generations for which the random subset of data is kept fixed
	void set_num_generations_for_which_random_subset_is_kept_fixed(unsigned int);
};
//-----------------------------------------------------------------
#endif // PARAMETERS_CLASS_H_INCLUDED