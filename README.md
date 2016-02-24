# libmep

# Multi Expression Programming library

Implements the Multi Expression Programming (MEP) technique for solving regression and binary classification problems. MEP is a Genetic Programming (GP) variant with linear representation of chromosomes.

MEP introduced a unique feature: the ability to encode multiple solutions in the same chromosome. This means that we can explore much more from the search space compared to other techniques which encode a single solution in the chromosome. In most cases this advantage comes with no penalty regarding the running time or the involved resources.

C++ 11 is required due to the use of C++ 11 threads.

# To compile: 

Create a new project and add all files from "src" and one file from "tests" folder (which contains the main function). Include paths must point to the "include" folder of this project.

# API
## Setting / getting parameters

Use t_mep_parameters class which has the following public methods:

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

## Setting operators
...
## Setting data
...
## Performing the analysis
...
## Reading the results
...

#More info:

http://www.mep.cs.ubbcluj.ro

#Discussion Group:

https://groups.google.com/d/forum/mepx

#Contact author:

mihai.oltean@gmail.com
