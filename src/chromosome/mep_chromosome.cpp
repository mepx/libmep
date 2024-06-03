// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
#include "mep_functions.h"
#include "mep_data.h"
#include "utils/rands_generator_utils.h"
//---------------------------------------------------------------------------
t_mep_chromosome::t_mep_chromosome()
{
	prg = NULL;
	simplified_prg = NULL;
	index_best_genes = NULL;
	index_best_genes_simplified = NULL;
	fitness = 0;

	num_constants = 0;
	real_constants = NULL;
	long_constants = NULL;
	best_class_threshold = 0;

	num_incorrectly_classified = 0;
	max_index_best_genes = 0;
	code_length = 0;
	num_total_variables = 0;
	num_utilized_genes = -1;
	num_program_outputs = 1;
	num_classes = 0;
	//class_index = NULL;
	problem_type = MEP_PROBLEM_REGRESSION;
	data_type = MEP_DATA_DOUBLE;
	error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;

	centers = NULL;
	class_labels = NULL;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::allocate_memory(unsigned int _code_length,
									   unsigned int num_vars,
									   bool use_constants,
									   const t_mep_constants *  constants,
									   unsigned int _num_program_outputs,
									   unsigned int _num_classes,
									   unsigned int _problem_type,
									   unsigned int _error_measure,
									   const int* training_class_labels,
									   char _data_type)
{
	this->code_length = _code_length;
	prg = new t_code3[code_length];
	simplified_prg = NULL;
	index_best_genes_simplified = NULL;
	this->num_total_variables = num_vars;
	this->num_program_outputs = _num_program_outputs;
	this->num_classes = _num_classes;
	problem_type = _problem_type;
	error_measure = _error_measure;
	data_type = _data_type;
	real_constants = NULL;
	long_constants = NULL;
	num_utilized_genes = -1;

	if (use_constants) {
		if (constants->get_constants_type() == MEP_CONSTANTS_USER_DEFINED) {
			num_constants = constants->get_num_user_defined_constants();
			if (num_constants){
				if (data_type == MEP_DATA_DOUBLE)
					real_constants = new double[num_constants];
				else{
					if (data_type == MEP_DATA_LONG_LONG)
						long_constants = new long long [num_constants];
				}
			}
		}
		else {// automatic constants
			num_constants = constants->get_num_automatic_constants();
			if (num_constants){
				if (data_type == MEP_DATA_DOUBLE)
					real_constants = new double[num_constants];
				else{
					if (data_type == MEP_DATA_LONG_LONG)
						long_constants = new long long [num_constants];
				}
			}
		}
	}
	else
		num_constants = 0;
	
	if (num_program_outputs){
		index_best_genes = new unsigned int[num_program_outputs];
		for (unsigned int i = 0; i < num_program_outputs; i++)
			index_best_genes[i] = 0;
	}

	if (num_classes) {
		centers = new double[num_classes];
		for (unsigned int i = 0; i < num_classes; i++)
			centers[i] = 0;
		class_labels = new int[num_classes];
		if (!training_class_labels)
			for (unsigned int i = 0; i < num_classes; i++)
				class_labels[i] = i;
		else
			for (unsigned int i = 0; i < num_classes; i++)
				class_labels[i] = training_class_labels[i];
	}
}
//---------------------------------------------------------------------------
t_mep_chromosome::~t_mep_chromosome()
{
	clear();
}
//---------------------------------------------------------------------------
void t_mep_chromosome::clear(void)
{
	if (prg) {
		delete[] prg;
		prg = NULL;
	}
	if (simplified_prg) {
		delete[] simplified_prg;
		simplified_prg = NULL;
	}
	
	
	if (index_best_genes) {
		delete[] index_best_genes;
		index_best_genes = NULL;
	}
	if (index_best_genes_simplified) {
		delete[] index_best_genes_simplified;
		index_best_genes_simplified = NULL;
	}

	fitness = 0;

	num_constants = 0;
	code_length = 0;
	num_total_variables = 0;

	if (real_constants) {
		delete[] real_constants;
		real_constants = NULL;
	}
	
	if (long_constants) {
		delete[] long_constants;
		long_constants = NULL;
	}

	if (centers) {
		delete[] centers;
		centers = NULL;
	}
	
	if (class_labels) {
		delete[] class_labels;
	 class_labels = NULL;
	}

	num_incorrectly_classified = 0;
	max_index_best_genes = 0;
	num_utilized_genes = -1;
}
//---------------------------------------------------------------------------
t_mep_chromosome& t_mep_chromosome::operator = (const t_mep_chromosome &source)
{
	if (this != &source) {

		code_length = source.code_length;
		num_total_variables = source.num_total_variables;
		num_program_outputs = source.num_program_outputs;
		problem_type = source.problem_type;
		error_measure = source.error_measure;

		if (!prg)// I do this for stats only
			prg = new t_code3[code_length];        // a string of genes
		//		if (!simplified_prg)// I do this for stats only
		//			simplified_prg = new code3[code_length];        // a string of genes
		//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
		
		for (unsigned int i = 0; i < code_length; i++) {
			prg[i] = source.prg[i];
			//			simplified_prg[i] = source.simplified_prg[i];
		}
		fitness = source.fitness;        // the fitness

		if (!index_best_genes)
			index_best_genes = new unsigned int[num_program_outputs];

		for (unsigned int i = 0; i < num_program_outputs; i++)
			index_best_genes[i] = source.index_best_genes[i];          // the index of the best expression in t_mep_chromosome

		max_index_best_genes = source.max_index_best_genes;
		num_constants = source.num_constants;
		best_class_threshold = source.best_class_threshold;
		data_type = source.data_type;
		num_utilized_genes = source.num_utilized_genes;

		if (source.real_constants) {
			if (!real_constants)
				real_constants = new double[num_constants];
			//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
			for (unsigned int i = 0; i < num_constants; i++)
				real_constants[i] = source.real_constants[i];
		}
		
		if (source.long_constants) {
			if (!long_constants)
				long_constants = new long long[num_constants];
			//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
			for (unsigned int i = 0; i < num_constants; i++)
				long_constants[i] = source.long_constants[i];
		}

		num_incorrectly_classified = source.num_incorrectly_classified;
		num_classes = source.num_classes;
		if (source.centers) {
			if (!centers)
				centers = new double[num_classes];
			for (unsigned int i = 0; i < num_classes; i++)
				centers[i] = source.centers[i];
			// the index of the best expression in t_mep_chromosome
		}
		
		if (source.class_labels) {
			if (!class_labels){
				class_labels = new int[num_classes];
				for (unsigned int i = 0; i < num_classes; i++)
					class_labels[i] = source.class_labels[i];
			}
		}
	}
	return *this;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::swap_pointers(t_mep_chromosome& source)
{
	if (this == &source)
		return;

	code_length = source.code_length;
	num_total_variables = source.num_total_variables;
	num_program_outputs = source.num_program_outputs;

	t_code3* tmp_prg = prg;
	prg = source.prg;
	source.prg = tmp_prg;

	fitness = source.fitness; // the fitness

	unsigned int* tmp_index_best_genes = index_best_genes;
	index_best_genes = source.index_best_genes;
	source.index_best_genes = tmp_index_best_genes;

	max_index_best_genes = source.max_index_best_genes;
	//num_constants = source.num_constants;
	best_class_threshold = source.best_class_threshold;

	double* tmp_real_constants = real_constants;
	real_constants = source.real_constants;
	source.real_constants = tmp_real_constants;

	long long* tmp_long_constants = long_constants;
	long_constants = source.long_constants;
	source.long_constants = tmp_long_constants;
	
	num_incorrectly_classified = source.num_incorrectly_classified;

	double *tmp_centers_ptr = centers;
	centers = source.centers;
	source.centers = tmp_centers_ptr;
	
	num_utilized_genes = source.num_utilized_genes;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::mark(unsigned int position, char* marked)
{
	if ((prg[position].op < 0) && !marked[position]) {
		mark(prg[position].addr[0], marked);

		switch (prg[position].op) {
		case O_ADDITION:
			mark(prg[position].addr[1], marked);
			break;
		case O_SUBTRACTION:
			mark(prg[position].addr[1], marked);
			break;
		case O_MULTIPLICATION:
			mark(prg[position].addr[1], marked);
			break;
		case O_DIVISION:
			mark(prg[position].addr[1], marked);
			break;
		case O_POWER:
			mark(prg[position].addr[1], marked);
			break;
		case O_MIN:
			mark(prg[position].addr[1], marked);
			break;
		case O_MAX:
			mark(prg[position].addr[1], marked);
			break;
		case O_FMOD:
			mark(prg[position].addr[1], marked);
			break;
		case O_IFLZ:
			mark(prg[position].addr[1], marked);
			mark(prg[position].addr[2], marked);
			break;
		case O_IFALBCD:
		case O_IF_A_OR_B_CD:
		case O_IF_A_XOR_B_CD:
			mark(prg[position].addr[1], marked);
			mark(prg[position].addr[2], marked);
			mark(prg[position].addr[3], marked);
			break;
		}
	}
	marked[position] = 1;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::count_num_utilized_genes(void)
{
	if (problem_type == MEP_PROBLEM_MULTICLASS_CLASSIFICATION &&
		error_measure != MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR){
		num_utilized_genes = code_length;
		return num_utilized_genes;
	}
			
	char *marked = new char[code_length];
	for (unsigned int i = 0; i < code_length; marked[i++] = 0);

	for (unsigned int o = 0; o < num_program_outputs; o++)
		mark(index_best_genes[o], marked);

	num_utilized_genes = 0;
	for (unsigned int i = 0; i <= max_index_best_genes; i++)
		num_utilized_genes += marked[i];

	delete[] marked;
	return num_utilized_genes;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::get_num_utilized_genes(void)
{
	if (num_utilized_genes == -1)
		return count_num_utilized_genes();
	else
		return num_utilized_genes;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::simplify(void)
{
	char *marked = new char[code_length];
	for (unsigned int i = 0; i < code_length; marked[i++] = 0);

	unsigned int *skipped = new unsigned int[code_length];
	for (unsigned int o = 0; o < num_program_outputs; o++)
		mark(index_best_genes[o], marked);

	if (!marked[0])
		skipped[0] = 1;
	else
		skipped[0] = 0;
	for (unsigned int i = 1; i <= max_index_best_genes; i++)
		if (!marked[i])
			skipped[i] = skipped[i - 1] + 1;
		else
			skipped[i] = skipped[i - 1];

	if (simplified_prg)
		delete[] simplified_prg;
	if (index_best_genes_simplified)
		delete[] index_best_genes_simplified;
	
	simplified_prg = new t_code3[code_length];
	index_best_genes_simplified = new unsigned int[num_program_outputs];

	num_utilized_genes = 0;
	for (unsigned int i = 0; i <= max_index_best_genes; i++){
		if (marked[i]) {
			for (unsigned int o = 0; o < num_program_outputs; o++)
				if (index_best_genes[o] == i){
					index_best_genes_simplified[o] = index_best_genes[o] - skipped[i];
					break;
				}
			simplified_prg[num_utilized_genes] = prg[i];
			if (prg[i].op < 0) {
				simplified_prg[num_utilized_genes].addr[0] -= skipped[prg[i].addr[0]];
				simplified_prg[num_utilized_genes].addr[1] -= skipped[prg[i].addr[1]];
				simplified_prg[num_utilized_genes].addr[2] -= skipped[prg[i].addr[2]];
				simplified_prg[num_utilized_genes].addr[3] -= skipped[prg[i].addr[3]];
			}
			num_utilized_genes++;
		}
	}

	delete[] skipped;
	delete[] marked;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::compare(const t_mep_chromosome &other, double precision)
{
	// -1 if this is better than other, 
	// +1 if this is worse than other
	if (fitness < other.fitness - precision)
		return -1;

	if (fitness > other.fitness + precision)
		return 1;

	if (num_utilized_genes < other.num_utilized_genes)
		return -1;

	if (num_utilized_genes > other.num_utilized_genes)
		return 1;

	return 0;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::generate_random(const t_mep_parameters &parameters,
		const t_mep_constants & mep_constants,
		const int *actual_operators, unsigned int num_actual_operators,
	const unsigned int *actual_variables, unsigned int num_actual_variables,
									   t_seed& seed)
// randomly initializes the individuals
{
	// I have to generate the constants for this individuals
	if (parameters.get_constants_probability() > 1E-6) {
		if (mep_constants.get_constants_type() == MEP_CONSTANTS_USER_DEFINED) {
			if (data_type == MEP_DATA_LONG_LONG)
				for (unsigned int c = 0; c < num_constants; c++)
					long_constants[c] = mep_constants.get_constants_long_long(c);
			else// it is double
				for (unsigned int c = 0; c < num_constants; c++)
					real_constants[c] = mep_constants.get_constants_double(c);
		}
		else {// automatic constants
			if (data_type == MEP_DATA_LONG_LONG){
				for (unsigned int c = 0; c < num_constants; c++)
					long_constants[c] = mep_long_long_rand(seed,
												  mep_constants.get_min_constants_interval_long_long(),
											mep_constants.get_max_constants_interval_long_long() + 1);
			}
			else
				for (unsigned int c = 0; c < num_constants; c++)
					real_constants[c] = mep_real_rand(seed,
												  mep_constants.get_min_constants_interval_double(),
											mep_constants.get_max_constants_interval_double());
		}
	}

	double sum = parameters.get_variables_probability() + parameters.get_constants_probability();
	double p = mep_real_rand(seed, 0, sum);

	if (p <= parameters.get_variables_probability())
		prg[0].op = (int)actual_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];
	else
		prg[0].op = (int)num_total_variables + (int)mep_unsigned_int_rand(seed, 0, num_constants);

	for (unsigned int i = 1; i < parameters.get_code_length(); i++) {
		p = mep_real_rand(seed, 0, 1);

		if (p <= parameters.get_operators_probability())
			prg[i].op = (int)actual_operators[mep_unsigned_int_rand(seed, 0, num_actual_operators)];
		else
			if (p <= parameters.get_operators_probability() + parameters.get_variables_probability())
				prg[i].op = (int)actual_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];
			else
				prg[i].op = (int)num_total_variables + (int)mep_unsigned_int_rand(seed, 0, num_constants);

		prg[i].addr[0] = mep_unsigned_int_rand(seed, 0, i);
		prg[i].addr[1] = mep_unsigned_int_rand(seed, 0, i);
		prg[i].addr[2] = mep_unsigned_int_rand(seed, 0, i);
		prg[i].addr[3] = mep_unsigned_int_rand(seed, 0, i);
	}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::mutation(const t_mep_parameters &parameters, 
	const t_mep_constants & mep_constants,
	const int *actual_operators, unsigned int num_actual_operators,
	const unsigned int *actual_variables, unsigned int num_actual_variables,
	t_seed& seed)
// mutate the individual
{

	// mutate each symbol with the same pm probability
	// mutate the first instruction
	double pm = parameters.get_mutation_probability();
	double p = mep_real_rand(seed, 0, 1);
	if (p < pm) {
		double sum = parameters.get_variables_probability() + parameters.get_constants_probability();
		double q = mep_real_rand(seed, 0, sum);

		if (q <= parameters.get_variables_probability())
			prg[0].op = (int)actual_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];
		else
			prg[0].op = (int)num_total_variables + (int)mep_unsigned_int_rand(seed, 0, num_constants);
	}
	// mutate the other instructions
	for (unsigned int i = 1; i < code_length; i++) {
		p = mep_real_rand(seed, 0, 1);      // mutate the operator
		if (p < pm) {
			double q = mep_real_rand(seed, 0, 1);

			if (q <= parameters.get_operators_probability())
				prg[i].op = actual_operators[mep_unsigned_int_rand(seed, 0, num_actual_operators)];
			else
				if (q <= parameters.get_operators_probability() + parameters.get_variables_probability())
					prg[i].op = (int)actual_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables)];
				else
					prg[i].op = (int)num_total_variables + (int)mep_unsigned_int_rand(seed, 0, num_constants);
		}

		p = mep_real_rand(seed, 0, 1);      // mutate the first address  (adr1)
		if (p < pm)
			prg[i].addr[0] = mep_unsigned_int_rand(seed, 0, i);

		p = mep_real_rand(seed, 0, 1);      // mutate the second address   (adr2)
		if (p < pm)
			prg[i].addr[1] = mep_unsigned_int_rand(seed, 0, i);
		p = mep_real_rand(seed, 0, 1);      // mutate the 3rd address   (adr3)
		if (p < pm)
			prg[i].addr[2] = mep_unsigned_int_rand(seed, 0, i);
		p = mep_real_rand(seed, 0, 1);      // mutate the 4th address   (adr4)
		if (p < pm)
			prg[i].addr[3] = mep_unsigned_int_rand(seed, 0, i);
	}
	// lets see if I can evolve constants

	if (mep_constants.get_constants_can_evolve() && 
		mep_constants.get_constants_type() == MEP_CONSTANTS_AUTOMATIC){
		if (data_type == MEP_DATA_LONG_LONG)
		for (unsigned int c = 0; c < num_constants; c++) {
			p = mep_real_rand(seed, 0, 1);      // mutate the operator
			long long tmp_cst_d = mep_long_long_rand(seed, 0,
													 mep_constants.get_constants_mutation_max_deviation_long_long() + 1);
			
			if (p < pm) {
				if (mep_int_01_rand(seed)) {// coin
					if (mep_constants.get_constants_can_evolve_outside_initial_interval())
						long_constants[c] += tmp_cst_d;
					else
						if (long_constants[c] + tmp_cst_d <= mep_constants.get_max_constants_interval_long_long())
							long_constants[c] += tmp_cst_d;
				}
				else {
					if (mep_constants.get_constants_can_evolve_outside_initial_interval())
						long_constants[c] -= tmp_cst_d;
					else
						if (long_constants[c] - tmp_cst_d >= mep_constants.get_min_constants_interval_long_long())
							long_constants[c] -= tmp_cst_d;
				}
				break;// 1 mutation for constants only??
			}
		}
		else// it is double
			for (unsigned int c = 0; c < num_constants; c++) {
				p = mep_real_rand(seed, 0, 1);      // mutate the operator
				double tmp_cst_d = mep_real_rand(seed, 0, mep_constants.get_constants_mutation_max_deviation_double());
				
				if (p < pm) {
					if (mep_int_01_rand(seed)) {// coin
						if (mep_constants.get_constants_can_evolve_outside_initial_interval())
							real_constants[c] += tmp_cst_d;
						else
							if (real_constants[c] + tmp_cst_d <= mep_constants.get_max_constants_interval_double())
								real_constants[c] += tmp_cst_d;
					}
					else {
						if (mep_constants.get_constants_can_evolve_outside_initial_interval())
							real_constants[c] -= tmp_cst_d;
						else
							if (real_constants[c] - tmp_cst_d >= mep_constants.get_min_constants_interval_double())
								real_constants[c] -= tmp_cst_d;
					}
					break;// 1 mutation for constants only??
				}
			}
	}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::one_cut_point_crossover(const t_mep_chromosome &parent2,
		t_mep_chromosome &offspring1, t_mep_chromosome &offspring2,
	const t_mep_constants & mep_constants, t_seed& seed)
{
	offspring1.code_length = code_length;
	offspring2.code_length = code_length;

	offspring1.num_total_variables = num_total_variables;
	offspring2.num_total_variables = num_total_variables;

	unsigned int pct, i;
	pct = 1 + mep_unsigned_int_rand(seed, 0, code_length - 1);
	for (i = 0; i < pct; i++) {
		offspring1.prg[i] = prg[i];
		offspring2.prg[i] = parent2.prg[i];
	}
	for (i = pct; i < code_length; i++) {
		offspring1.prg[i] = parent2.prg[i];
		offspring2.prg[i] = prg[i];
	}

	if (num_constants && mep_constants.get_constants_can_evolve() &&
			mep_constants.get_constants_type() == MEP_CONSTANTS_AUTOMATIC) {
		pct = 1 + mep_unsigned_int_rand(seed, 0, num_constants - 1);
		
		if (data_type == MEP_DATA_LONG_LONG){
			for (unsigned int c = 0; c < pct; c++) {
				offspring1.long_constants[c] = long_constants[c];
				offspring2.long_constants[c] = parent2.long_constants[c];
			}
			for (unsigned int c = pct; c < num_constants; c++) {
				offspring1.long_constants[c] = parent2.long_constants[c];
				offspring2.long_constants[c] = long_constants[c];
			}
		}
		else{// it is double
			for (unsigned int c = 0; c < pct; c++) {
				offspring1.real_constants[c] = real_constants[c];
				offspring2.real_constants[c] = parent2.real_constants[c];
			}
			for (unsigned int c = pct; c < num_constants; c++) {
				offspring1.real_constants[c] = parent2.real_constants[c];
				offspring2.real_constants[c] = real_constants[c];
			}
		}
	}

}
//---------------------------------------------------------------------------
void t_mep_chromosome::uniform_crossover(
	const t_mep_chromosome &parent2,
	t_mep_chromosome &offspring1, t_mep_chromosome &offspring2,
	const t_mep_constants & mep_constants,
										 t_seed& seed)
{
	offspring1.code_length = code_length;
	offspring2.code_length = code_length;

	offspring1.num_total_variables = num_total_variables;
	offspring2.num_total_variables = num_total_variables;

	for (unsigned int i = 0; i < code_length; i++)
		if (mep_int_01_rand(seed)) {
			offspring1.prg[i] = prg[i];
			offspring2.prg[i] = parent2.prg[i];
		}
		else {
			offspring1.prg[i] = parent2.prg[i];
			offspring2.prg[i] = prg[i];
		}

    if (mep_constants.get_constants_can_evolve() &&
		mep_constants.get_constants_type() == MEP_CONSTANTS_AUTOMATIC){
		if (data_type == MEP_DATA_LONG_LONG){
			for (unsigned int c = 0; c < num_constants; c++) {
				offspring1.long_constants[c] = long_constants[c];
				offspring2.long_constants[c] = parent2.long_constants[c];
			}
		}
		else{// double
			for (unsigned int c = 0; c < num_constants; c++) {
				offspring1.real_constants[c] = real_constants[c];
				offspring2.real_constants[c] = parent2.real_constants[c];
			}
		}
	}
}
//---------------------------------------------------------------------------
/*
void t_mep_chromosome::fitness_classification_double_cache_all_training_data(t_mep_chromosome &, double **)
{

// evaluate a_chromosome
// partial results are stored and used later in other sub-expressions

fit = 1E+308;
best = -1;

int *line_of_constants = NULL;
if (num_constants) {
line_of_constants = new int[num_constants];// line where a constant was firstly computed
for (int i = 0; i < num_constants; i++)
line_of_constants[i] = -1;
}

compute_eval_matrix_double(a_chromosome, eval_double, line_of_constants);

for (int i = 0; i < mep_parameters->get_code_length(); i++) {   // read the t_mep_chromosome from top to down
double sum_of_errors;
if (prg[i].op >= 0)
if (prg[i].op < training_data.num_vars) // a variable, which is cached already
sum_of_errors = cached_sum_of_errors[prg[i].op];
else {// a constant
double *eval = eval_double[line_of_constants[prg[i].op - training_data.num_vars]];
sum_of_errors = 0;
for (int k = 0; k < training_data->get_num_rows(); k++)
if (eval[k] <= mep_parameters->classification_threshold)
sum_of_errors += training_data._target_double[k];
else
sum_of_errors += 1 - training_data._target_double[k];
}
else {
double *eval = eval_double[i];
sum_of_errors = 0;
for (int k = 0; k < training_data->get_num_rows(); k++)
if (eval[k] <= mep_parameters->classification_threshold)
sum_of_errors += training_data._target_double[k];
else
sum_of_errors += 1 - training_data._target_double[k];
}

if (fit > sum_of_errors / training_data->get_num_rows()) {
fit = sum_of_errors / training_data->get_num_rows();
best = i;
}
}

if (line_of_constants)
delete[] line_of_constants;

}
*/
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*
bool t_mep_chromosome::get_error_double(double *inputs, double *outputs)
{
if (!evaluate_double(inputs, outputs))
return false;
if (mep_parameters->get_problem_type() == PROBLEM_CLASSIFICATION) {
if (outputs[0] <= best_class_threshold)
outputs[0] = 0;
else
outputs[0] = 1;
}

return true;
}
//---------------------------------------------------------------------------
*/
double t_mep_chromosome::get_fitness(void) const
{
	return fitness;
}
//---------------------------------------------------------------------------
unsigned int t_mep_chromosome::get_index_best_gene(unsigned int output_index) const
{
	return index_best_genes[output_index];
}
//---------------------------------------------------------------------------
double t_mep_chromosome::get_best_class_threshold(void) const
{
	return best_class_threshold;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::set_gene_operation(unsigned int gene_index, int new_operation)
{
	if (prg)
		prg[gene_index].op = new_operation;
}
//---------------------------------------------------------------------------
double t_mep_chromosome::get_num_incorrectly_classified(void) const
{
	return num_incorrectly_classified;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::compute_fitness(const t_mep_data& mep_dataset,
									   const t_mep_data& mep_dataset_ts,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_variables_eval_matrix_double,
									   long long** cached_variables_eval_matrix_long,
									   double** cached_sum_of_errors,
									   double* cached_threashold,
	s_value_class* tmp_value_class,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_double, long long** eval_long,
									   char *gene_used_for_output,
    t_seed& seed)
{
	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
		if (error_measure == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
			fitness_regression(mep_dataset,
				random_subset_indexes, random_subset_selection_size,
							   cached_variables_eval_matrix_double,
							   cached_variables_eval_matrix_long,
							   cached_sum_of_errors,
				num_actual_variables, actual_enabled_variables,
				eval_double, eval_long, gene_used_for_output,
							   mep_absolute_error_double,
							   mep_absolute_error_long_long, seed);
		else
			fitness_regression(mep_dataset,
				random_subset_indexes, random_subset_selection_size,
							   cached_variables_eval_matrix_double,
							   cached_variables_eval_matrix_long,
							   cached_sum_of_errors,
				num_actual_variables, actual_enabled_variables,
				eval_double, eval_long, gene_used_for_output,
							   mep_squared_error_double,
							   mep_absolute_error_long_long, seed);
		break;
	case MEP_PROBLEM_TIME_SERIE:
		if (error_measure == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
			fitness_regression(mep_dataset_ts,
				random_subset_indexes, random_subset_selection_size,
							   cached_variables_eval_matrix_double,
							   cached_variables_eval_matrix_long,
							   cached_sum_of_errors,
				num_actual_variables, actual_enabled_variables,
				eval_double, eval_long, gene_used_for_output,
							   mep_absolute_error_double,
							   mep_absolute_error_long_long, seed);
		else
			fitness_regression(mep_dataset_ts,
				random_subset_indexes, random_subset_selection_size,
							   cached_variables_eval_matrix_double,
							   cached_variables_eval_matrix_long,
							   cached_sum_of_errors,
				num_actual_variables, actual_enabled_variables,
				eval_double, eval_long, gene_used_for_output,
							   mep_squared_error_double,
							   mep_absolute_error_long_long, seed);
		break;
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
		fitness_binary_classification(mep_dataset,
			random_subset_indexes, random_subset_selection_size,
			cached_variables_eval_matrix_double, cached_sum_of_errors,
			cached_threashold,
			num_actual_variables, actual_enabled_variables,
			eval_double, tmp_value_class, seed);
		break;
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		switch (error_measure) {
		case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
			fitness_multi_class_classification_winner_takes_all_fixed(
				mep_dataset, random_subset_indexes,
				random_subset_selection_size,
				cached_variables_eval_matrix_double, num_actual_variables,
				actual_enabled_variables, eval_double, seed);
			break;
		case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
			fitness_multi_class_classification_smooth(
				mep_dataset, random_subset_indexes,
				random_subset_selection_size,
				cached_variables_eval_matrix_double, num_actual_variables,
				actual_enabled_variables, eval_double, seed);
			break;
		case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
			fitness_multi_class_classification_winner_takes_all_dynamic(
				mep_dataset, random_subset_indexes,
				random_subset_selection_size,
				cached_variables_eval_matrix_double, num_actual_variables,
				actual_enabled_variables, eval_double, seed);
			break;
		case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
			fitness_multi_class_classification_closest_center(
				mep_dataset, random_subset_indexes,
				random_subset_selection_size,
				cached_variables_eval_matrix_double, num_actual_variables,
				actual_enabled_variables, eval_double, seed);
			break;
		}
	}
	count_num_utilized_genes();
}
//---------------------------------------------------------------------------
unsigned int t_mep_chromosome::get_closest_class_index_from_center(double program_output) const
{
	double min_dist = DBL_MAX;
	unsigned int closest_class_index = 0;
	for (unsigned int c = 0; c < num_classes; c++) {
		if (min_dist > fabs(program_output - centers[c])) {
			min_dist = fabs(program_output - centers[c]);
			closest_class_index = c;
		}
	}
	return closest_class_index;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::get_class_index_for_winner_takes_all_dynamic(
	double *inputs,
	unsigned int& class_index) const
{
	unsigned int index_error_gene;
	unsigned int max_index;
	double max_value;

	class_index = num_classes;

	double* values_for_output_genes = new double[num_classes];
	if (!get_first_max_index(inputs, max_index, max_value, index_error_gene, values_for_output_genes)) {
		delete[] values_for_output_genes;
		return false;
	}
	for (unsigned int c = 0; c < num_classes; c++)
		if (index_best_genes[c] == max_index) {
			class_index = c;
			break;
		}
	if (class_index == num_classes) {
		// find the closest max
		double min_dist = fabs(max_value - values_for_output_genes[0]);
		class_index = 0;
		for (unsigned int c = 1; c < num_classes; c++) {
			if (min_dist > fabs(max_value - values_for_output_genes[c])) {
				min_dist = fabs(max_value - values_for_output_genes[c]);
				class_index = c;
			}
		}
	}
	delete[] values_for_output_genes;

	return true;
}
//---------------------------------------------------------------------------
unsigned int t_mep_chromosome::get_problem_type(void) const
{
	return problem_type;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::get_class_label(unsigned int class_index) const
{
	return class_labels[class_index];
}
//---------------------------------------------------------------------------
