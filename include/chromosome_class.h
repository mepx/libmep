#ifndef CHROMOSOME_CLASS_H
#define CHROMOSOME_CLASS_H

#include "pugixml.hpp"
#include "constants.h"
#include "parameters_class.h"


//-----------------------------------------------------------------
struct code3
{
	int op;            // either a variable or an operator; variables are 0,1,2,...; operators are -1, -2, -3...
	int adr1, adr2, adr3, adr4;    // pointers to arguments
};
//-----------------------------------------------------------------
struct chromosome
{
	code3 *prg;        // a string of genes
	code3 *s_prg;      // simplified prg
	double fit;        // the fitness
	int best;          // the index of the best expression in chromosome
	int num_utilized;  // number of utilized positions
	double best_class_threshold;

	long num_constants;
	double *constants_double;
	int num_total_variables;
	long code_length;

	chromosome();
	~chromosome();
	void clear(void);
	char * to_C_double(bool simplified, double *data, int problem_type);

	void init_and_allocate_memory();

	chromosome& operator=(const chromosome &source);

	int compare(chromosome *other, bool minimize_operations_count);
	void allocate_memory(long code_length, int num_total_vars, bool use_constants, t_constants *constants);
    
    void generate_random(t_parameters *parameters, int *actual_operators, int num_actual_operators, int *actual_used_variables, int num_actual_used_variables);
    
    void generate_random_individual(t_parameters *parameters);
	void mutation(t_parameters *parameters, int *actual_operators, int num_actual_operators, int *actual_used_variables, int num_actual_used_variables);
	void one_cut_point_crossover(const chromosome &parent2, chromosome &offspring1, chromosome &offspring2, t_parameters *parameters);
	void uniform_crossover(const chromosome &parent2, chromosome &offspring1, chromosome &offspring2, t_parameters *parameters);

	void simplify(void);
	void mark(int k, bool* marked);

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent);
};
//-----------------------------------------------------------------


#endif