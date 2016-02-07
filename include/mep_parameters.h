#ifndef PARAMETERS_CLASS_H_INCLUDED
#define PARAMETERS_CLASS_H_INCLUDED

#include "pugixml.hpp"
#include "mep_constants.h"

#define PROBLEM_REGRESSION 0
#define PROBLEM_CLASSIFICATION 1


#define UNIFORM_CROSSOVER 0
#define ONE_CUTTING_POINT_CROSSOVER 1

//-----------------------------------------------------------------
class t_mep_parameters
{
public:
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

	long random_seed;
	long num_runs;

	bool simplified_programs;

	t_mep_constants constants;

	t_mep_parameters(void);
    void init (void);
    int to_xml(pugi::xml_node parent);
    int from_xml(pugi::xml_node parent);
};
//-----------------------------------------------------------------


#endif // PARAMETERS_CLASS_H_INCLUDED
