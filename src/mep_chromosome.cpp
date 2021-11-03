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
#include "mep_constants.h"
#include "mep_data.h"
#include "mep_rands.h"
//---------------------------------------------------------------------------
double mep_absolute_error(double x, double y)
{
	return fabs(x - y);
}
//---------------------------------------------------------------------------
double mep_squared_error(double x, double y)
{
	return (x - y) * (x - y);
}
//---------------------------------------------------------------------------
t_mep_chromosome::t_mep_chromosome()
{
	prg = NULL;
	simplified_prg = NULL;
	index_best_genes = NULL;
	fitness = 0;

	num_constants = 0;
	real_constants = NULL;
	best_class_threshold = 0;

	num_incorrectly_classified = 0;
	max_index_best_genes = -1;
	code_length = 0;
	num_total_variables = 0;
	num_utilized_genes = 0;
	num_outputs = 0;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::allocate_memory(long _code_length, int num_vars,
		bool use_constants, t_mep_constants *  constants,
		int _num_outputs)
{
	this->code_length = _code_length;
	prg = new t_code3[code_length];
	simplified_prg = NULL;
	this->num_total_variables = num_vars;
	this->num_outputs = _num_outputs;

	if (use_constants) {
		if (constants->get_constants_type() == USER_DEFINED_CONSTANTS) {
			num_constants = constants->get_num_user_defined_constants();
			real_constants = new double[num_constants];
		}
		else {// automatic constants
			num_constants = constants->get_num_automatic_constants();
			if (num_constants)
				real_constants = new double[num_constants];
		}
	}
	else
		num_constants = 0;
	if (_num_outputs)
		index_best_genes = new int[num_outputs];
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
	fitness = 0;

	num_constants = 0;
	code_length = 0;
	num_total_variables = 0;

	if (real_constants) {
		delete[] real_constants;
		real_constants = NULL;
	}

	num_incorrectly_classified = 0;
	max_index_best_genes = -1;
}
//---------------------------------------------------------------------------
t_mep_chromosome& t_mep_chromosome::operator = (const t_mep_chromosome &source)
{
	if (this != &source) {
		//		clear();

		code_length = source.code_length;
		num_total_variables = source.num_total_variables;
		num_outputs = source.num_outputs;

		if (!prg)// I do this for stats only
			prg = new t_code3[code_length];        // a string of genes
		//		if (!simplified_prg)// I do this for stats only
		//			simplified_prg = new code3[code_length];        // a string of genes
		//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
		for (int i = 0; i < code_length; i++) {
			prg[i] = source.prg[i];
			//			simplified_prg[i] = source.simplified_prg[i];
		}
		fitness = source.fitness;        // the fitness

		if (!index_best_genes)
			index_best_genes = new int[num_outputs];

		for (int i = 0; i < num_outputs; i++)
			index_best_genes[i] = source.index_best_genes[i];          // the index of the best expression in t_mep_chromosome

		max_index_best_genes = source.max_index_best_genes;
		num_constants = source.num_constants;
		best_class_threshold = source.best_class_threshold;

		if (source.real_constants) {
			if (!real_constants)
				real_constants = new double[num_constants];
			//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
			for (int i = 0; i < num_constants; i++)
				real_constants[i] = source.real_constants[i];
		}

		num_incorrectly_classified = source.num_incorrectly_classified;
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
	num_outputs = source.num_outputs;

	t_code3* tmp_prg = prg;
	prg = source.prg;
	source.prg = tmp_prg;

	fitness = source.fitness; // the fitness

	int* tmp_index_best_genes = index_best_genes;
	index_best_genes = source.index_best_genes;
	source.index_best_genes = tmp_index_best_genes;

	max_index_best_genes = source.max_index_best_genes;
	num_constants = source.num_constants;
	best_class_threshold = source.best_class_threshold;

	double* tmp_real_constants = real_constants;
	real_constants = source.real_constants;
	source.real_constants = tmp_real_constants;

	num_incorrectly_classified = source.num_incorrectly_classified;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::to_xml(pugi::xml_node parent)
{
	char tmp_str[100];

	pugi::xml_node node = parent.append_child("code_length");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", code_length);
	data.set_value(tmp_str);

	node = parent.append_child("num_variables");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", num_total_variables);
	data.set_value(tmp_str);

	node = parent.append_child("num_outputs");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", num_outputs);
	data.set_value(tmp_str);

	pugi::xml_node node_code = parent.append_child("code");

	for (int i = 0; i < code_length; i++) {
		node = node_code.append_child("i");
		data = node.append_child(pugi::node_pcdata);
		if (prg[i].op < 0) // operator
			sprintf(tmp_str, "%d %d %d %d %d", prg[i].op, prg[i].addr1, prg[i].addr2, prg[i].addr3, prg[i].addr4);
		else
			sprintf(tmp_str, "%d", prg[i].op);
		data.set_value(tmp_str);
	}
	node = parent.append_child("best_genes");
	data = node.append_child(pugi::node_pcdata);

	if (num_outputs) {
		char tmp_s[30];
		char *tmp_genes_str = new char[num_outputs * 12 + 1]; // 30 digits for each constant !!!
		tmp_genes_str[0] = 0;
		for (int c = 0; c < num_outputs; c++) {
			sprintf(tmp_s, "%d", index_best_genes[c]);
			strcat(tmp_genes_str, tmp_s);
			strcat(tmp_genes_str, " ");
		}

		data.set_value(tmp_genes_str);
		delete[] tmp_genes_str;
	}

	node = parent.append_child("max_index_best_genes");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", max_index_best_genes);
	data.set_value(tmp_str);

	node = parent.append_child("error");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", fitness);
	data.set_value(tmp_str);

	node = parent.append_child("num_incorrectly_classified");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lf", num_incorrectly_classified);
	data.set_value(tmp_str);

	node = parent.append_child("binary_classification_threshold");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", best_class_threshold);
	data.set_value(tmp_str);

	node = parent.append_child("num_constants");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", num_constants);
	data.set_value(tmp_str);

	if (num_constants) {
		node = parent.append_child("constants");
		data = node.append_child(pugi::node_pcdata);

		char *tmp_cst_str = NULL;

		if (real_constants) {
			char tmp_s[30];
			tmp_cst_str = new char[num_constants * 30]; // 30 digits for each constant !!!
			tmp_cst_str[0] = 0;
			for (int c = 0; c < num_constants; c++) {
				sprintf(tmp_s, "%lg", real_constants[c]);
				strcat(tmp_cst_str, tmp_s);
				strcat(tmp_cst_str, " ");
			}
		}
		data.set_value(tmp_cst_str);
		if (tmp_cst_str)
		delete[] tmp_cst_str;
	}
	return true;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::from_xml(pugi::xml_node parent)
{
	clear();

	pugi::xml_node node = parent.child("code_length");
	if (node) {
		const char *value_as_cstring = node.child_value();
		code_length = atoi(value_as_cstring);
	}
	else
		code_length = 0;

	node = parent.child("num_variables");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_total_variables = atoi(value_as_cstring);
	}
	else
		num_total_variables = 1;

	if (code_length) {
		prg = new t_code3[code_length];
		pugi::xml_node node_code = parent.child("code");
		if (node_code) {
			int i = 0;
			for (pugi::xml_node row = node_code.child("i"); row; row = row.next_sibling("i"), i++) {
				const char *value_as_cstring = row.child_value();
				sscanf(value_as_cstring, "%d", &prg[i].op);
				if (prg[i].op < 0) { // operator
					int num_read = sscanf(value_as_cstring, "%d%d%d%d%d", &prg[i].op, &prg[i].addr1, &prg[i].addr2, &prg[i].addr3, &prg[i].addr4);
					if (num_read < 4) {
						prg[i].addr3 = 0;
						prg[i].addr4 = 0;
					}

				}

			}
			if (!i) {
				for (pugi::xml_node row = node_code.child("instruction"); row; row = row.next_sibling("instruction"), i++) {
					const char *value_as_cstring = row.child_value();
					sscanf(value_as_cstring, "%d", &prg[i].op);
					if (prg[i].op < 0) { // operator
						int num_read = sscanf(value_as_cstring, "%d%d%d%d%d", &prg[i].op, &prg[i].addr1, &prg[i].addr2, &prg[i].addr3, &prg[i].addr4);
						if (num_read < 4) {
							prg[i].addr3 = 0;
							prg[i].addr4 = 0;
						}

					}

				}

			}
		}
	}

	node = parent.child("num_outputs");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_outputs = atoi(value_as_cstring);
	}
	else
		num_outputs = 1;

	if (num_outputs)
		index_best_genes = new int[num_outputs];

	node = parent.child("best_genes");
	if (node) {
		const char *value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		for (int c = 0; c < num_outputs; c++) {
			sscanf(value_as_cstring + num_jumped_chars, "%d", &index_best_genes[c]);
			size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}
	else {
// old versions
		node = parent.child("best");
		if (node) {
			const char *value_as_cstring = node.child_value();
			index_best_genes[0] = atoi(value_as_cstring);
			max_index_best_genes = index_best_genes[0];
		}
	}

	node = parent.child("max_index_best_genes");
	if (node) {
		const char *value_as_cstring = node.child_value();
		max_index_best_genes = atoi(value_as_cstring);
	}

	node = parent.child("error");
	if (node) {
		const char *value_as_cstring = node.child_value();
		fitness = atof(value_as_cstring);
	}

	node = parent.child("num_incorrectly_classified");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_incorrectly_classified = atof(value_as_cstring);
	}
	else
		num_incorrectly_classified = 0;

	node = parent.child("binary_classification_threshold");
	if (node) {
		const char *value_as_cstring = node.child_value();
		best_class_threshold = atof(value_as_cstring);
	}
	else
		best_class_threshold = 0;

	node = parent.child("num_constants");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_constants = atoi(value_as_cstring);
	}
	node = parent.child("constants");
	if (node) {
		const char *value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		real_constants = new double[num_constants];
		for (int c = 0; c < num_constants; c++) {
			sscanf(value_as_cstring + num_jumped_chars, "%lf", &real_constants[c]);
			size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::mark(int position, bool* marked)
{
	if ((prg[position].op < 0) && !marked[position]) {
		mark(prg[position].addr1, marked);

		switch (prg[position].op) {
		case O_ADDITION:
			mark(prg[position].addr2, marked);
			break;
		case O_SUBTRACTION:
			mark(prg[position].addr2, marked);
			break;
		case O_MULTIPLICATION:
			mark(prg[position].addr2, marked);
			break;
		case O_DIVISION:
			mark(prg[position].addr2, marked);
			break;
		case O_POWER:
			mark(prg[position].addr2, marked);
			break;
		case O_MIN:
			mark(prg[position].addr2, marked);
			break;
		case O_MAX:
			mark(prg[position].addr2, marked);
			break;
		case O_IFLZ:
			mark(prg[position].addr2, marked);
			mark(prg[position].addr3, marked);
			break;
		case O_IFALBCD:
		case O_IF_A_OR_B_CD:
		case O_IF_A_XOR_B_CD:
			mark(prg[position].addr2, marked);
			mark(prg[position].addr3, marked);
			mark(prg[position].addr4, marked);
			break;
		}
	}
	marked[position] = true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::simplify(void)
{
	bool *marked = new bool[code_length];
	for (int i = 0; i < code_length; marked[i++] = false);

	int *skipped = new int[code_length];
	for (int o = 0; o < num_outputs; o++) {
		mark(index_best_genes[o], marked);

		if (!marked[0])
			skipped[0] = 1;
		else
			skipped[0] = 0;
		for (int i = 1; i <= index_best_genes[o]; i++)
			if (!marked[i])
				skipped[i] = skipped[i - 1] + 1;
			else
				skipped[i] = skipped[i - 1];
	}
	if (simplified_prg)
		delete[] simplified_prg;
	simplified_prg = new t_code3[code_length];

	num_utilized_genes = 0;
	for (int o = 0; o < num_outputs; o++)
	for (int i = 0; i <= index_best_genes[o]; i++)
		if (marked[i]) {
			simplified_prg[num_utilized_genes] = prg[i];
			if (prg[i].op < 0) {
				simplified_prg[num_utilized_genes].addr1 -= skipped[prg[i].addr1];
				simplified_prg[num_utilized_genes].addr2 -= skipped[prg[i].addr2];
				simplified_prg[num_utilized_genes].addr3 -= skipped[prg[i].addr3];
				simplified_prg[num_utilized_genes].addr4 -= skipped[prg[i].addr4];
			}
			num_utilized_genes++;
		}

	delete[] skipped;
	delete[] marked;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::compare(const t_mep_chromosome &other, bool /*minimize_operations_count*/)
{// -1 if this is better than other, 1 if this is worse than oher
	if (fitness > other.fitness)
		return 1;
	else if (fitness < other.fitness)
		return -1;
	else
		return 0;

}
//---------------------------------------------------------------------------
void t_mep_chromosome::generate_random(const t_mep_parameters &parameters,
		const t_mep_constants & mep_constants,
		const int *actual_operators, int num_actual_operators,
	const int *actual_variables, int num_actual_variables, t_seed& seed)
// randomly initializes the individuals
{
	// I have to generate the constants for this individuals
	if (parameters.get_constants_probability() > 1E-6) {
		if (mep_constants.get_constants_type() == USER_DEFINED_CONSTANTS) {
			for (int c = 0; c < num_constants; c++)
				real_constants[c] = mep_constants.get_constants_double(c);
		}
		else {// automatic constants
			for (int c = 0; c < num_constants; c++)
				real_constants[c] = mep_real_rand(seed, mep_constants.get_min_constants_interval_double(),
											mep_constants.get_max_constants_interval_double());
		}
	}

	double sum = parameters.get_variables_probability() + parameters.get_constants_probability();
	double p = mep_real_rand(seed, 0, sum);

	if (p <= parameters.get_variables_probability())
		prg[0].op = actual_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];
	else
		prg[0].op = num_total_variables + mep_int_rand(seed, 0, num_constants - 1);

	for (int i = 1; i < parameters.get_code_length(); i++) {
		p = mep_real_rand(seed, 0, 1);

		if (p <= parameters.get_operators_probability())
			prg[i].op = actual_operators[mep_int_rand(seed, 0, num_actual_operators - 1)];
		else
			if (p <= parameters.get_operators_probability() + parameters.get_variables_probability())
				prg[i].op = actual_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];
			else
				prg[i].op = num_total_variables + mep_int_rand(seed, 0, num_constants - 1);

		prg[i].addr1 = mep_int_rand(seed, 0, i - 1);
		prg[i].addr2 = mep_int_rand(seed, 0, i - 1);
		prg[i].addr3 = mep_int_rand(seed, 0, i - 1);
		prg[i].addr4 = mep_int_rand(seed, 0, i - 1);
	}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::mutation(const t_mep_parameters &parameters, const t_mep_constants & mep_constants,
		const int *actual_operators, int num_actual_operators,
	const int *actual_variables, int num_actual_variables,
	t_seed& seed)
// mutate the individual
{

	// mutate each symbol with the same pm probability
	double p = mep_real_rand(seed, 0, 1);
	if (p < parameters.get_mutation_probability()) {
		double sum = parameters.get_variables_probability() + parameters.get_constants_probability();
		double q = mep_real_rand(seed, 0, sum);

		if (q <= parameters.get_variables_probability())
			prg[0].op = actual_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];
		else
			prg[0].op = num_total_variables + mep_int_rand(seed, 0, num_constants - 1);
	}

	for (int i = 1; i < code_length; i++) {
		p = mep_real_rand(seed, 0, 1);      // mutate the operator
		if (p < parameters.get_mutation_probability()) {
			double q = mep_real_rand(seed, 0, 1);

			if (q <= parameters.get_operators_probability())
				prg[i].op = actual_operators[mep_int_rand(seed, 0, num_actual_operators - 1)];
			else
				if (q <= parameters.get_operators_probability() + parameters.get_variables_probability())
					prg[i].op = actual_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];
				else
					prg[i].op = num_total_variables + mep_int_rand(seed, 0, num_constants - 1);
		}

		p = mep_real_rand(seed, 0, 1);      // mutate the first address  (adr1)
		if (p < parameters.get_mutation_probability())
			prg[i].addr1 = mep_int_rand(seed, 0, i - 1);

		p = mep_real_rand(seed, 0, 1);      // mutate the second address   (adr2)
		if (p < parameters.get_mutation_probability())
			prg[i].addr2 = mep_int_rand(seed, 0, i - 1);
		p = mep_real_rand(seed, 0, 1);      // mutate the 3rd address   (adr3)
		if (p < parameters.get_mutation_probability())
			prg[i].addr3 = mep_int_rand(seed, 0, i - 1);
		p = mep_real_rand(seed, 0, 1);      // mutate the 4th address   (adr4)
		if (p < parameters.get_mutation_probability())
			prg[i].addr4 = mep_int_rand(seed, 0, i - 1);
	}
	// lets see if I can evolve constants

	if (mep_constants.get_constants_can_evolve() && mep_constants.get_constants_type() == AUTOMATIC_CONSTANTS)
		for (int c = 0; c < num_constants; c++) {
			p = mep_real_rand(seed, 0, 1);      // mutate the operator
			double tmp_cst_d = mep_real_rand(seed, 0, mep_constants.get_constants_mutation_max_deviation());

			if (p < parameters.get_mutation_probability()) {
				if (mep_int_rand(seed, 0, 1)) {// coin
					if (real_constants[c] + tmp_cst_d <= mep_constants.get_max_constants_interval_double())
						real_constants[c] += tmp_cst_d;
				}
				else
					if (real_constants[c] - tmp_cst_d >= mep_constants.get_min_constants_interval_double())
						real_constants[c] -= tmp_cst_d;
				break;
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

	int pct, i;
	pct = 1 + mep_int_rand(seed, 0, code_length - 3);
	for (i = 0; i < pct; i++) {
		offspring1.prg[i] = prg[i];
		offspring2.prg[i] = parent2.prg[i];
	}
	for (i = pct; i < code_length; i++) {
		offspring1.prg[i] = parent2.prg[i];
		offspring2.prg[i] = prg[i];
	}

	if (num_constants && mep_constants.get_constants_can_evolve() &&
			mep_constants.get_constants_type() == AUTOMATIC_CONSTANTS) {
		pct = 1 + mep_int_rand(seed, 0, num_constants - 3);
		for (int c = 0; c < pct; c++) {
			offspring1.real_constants[c] = real_constants[c];
			offspring2.real_constants[c] = parent2.real_constants[c];
		}
		for (int c = pct; c < num_constants; c++) {
			offspring1.real_constants[c] = parent2.real_constants[c];
			offspring2.real_constants[c] = real_constants[c];
		}
	}

}
//---------------------------------------------------------------------------
void t_mep_chromosome::uniform_crossover(const t_mep_chromosome &parent2,
	t_mep_chromosome &offspring1, t_mep_chromosome &offspring2,
	const t_mep_constants & mep_constants, t_seed& seed)
{
	offspring1.code_length = code_length;
	offspring2.code_length = code_length;

	offspring1.num_total_variables = num_total_variables;
	offspring2.num_total_variables = num_total_variables;

	for (int i = 0; i < code_length; i++)
		if (mep_int_rand(seed, 0, 1)) {
			offspring1.prg[i] = prg[i];
			offspring2.prg[i] = parent2.prg[i];
		}
		else {
			offspring1.prg[i] = parent2.prg[i];
			offspring2.prg[i] = prg[i];
		}

    if (mep_constants.get_constants_can_evolve() &&
        mep_constants.get_constants_type() == AUTOMATIC_CONSTANTS)
        for (int c = 0; c < num_constants; c++) {
            offspring1.real_constants[c] = real_constants[c];
            offspring2.real_constants[c] = parent2.real_constants[c];
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
bool t_mep_chromosome::evaluate_double(double *inputs, double *outputs, int &index_error_gene)
{
	bool is_error_case;  // division by zero, other errors

	double *eval_vect = new double[max_index_best_genes + 1];

	for (int i = 0; i <= max_index_best_genes; i++)   // read the t_mep_chromosome from top to down
	{
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		is_error_case = false;
		switch (prg[i].op) {
		case  O_ADDITION:  // +
			eval_vect[i] = eval_vect[prg[i].addr1] + eval_vect[prg[i].addr2];
			break;
		case  O_SUBTRACTION:  // -
			eval_vect[i] = eval_vect[prg[i].addr1] - eval_vect[prg[i].addr2];
			break;
		case  O_MULTIPLICATION:  // *
			eval_vect[i] = eval_vect[prg[i].addr1] * eval_vect[prg[i].addr2];
			break;
		case  O_DIVISION:  //  /
			if (fabs(eval_vect[prg[i].addr2]) < DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = eval_vect[prg[i].addr1] / eval_vect[prg[i].addr2];
			break;
		case O_POWER:
			eval_vect[i] = pow(eval_vect[prg[i].addr1], eval_vect[prg[i].addr2]);
			break;
		case O_SQRT:
			if (eval_vect[prg[i].addr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = sqrt(eval_vect[prg[i].addr1]);
			break;
		case O_EXP:
			eval_vect[i] = exp(eval_vect[prg[i].addr1]);

			break;
		case O_POW10:
			eval_vect[i] = pow(10, eval_vect[prg[i].addr1]);
			break;
		case O_LN:
			if (eval_vect[prg[i].addr1] <= 0)
				is_error_case = true;
			else                // an exception occured !!!
				eval_vect[i] = log(eval_vect[prg[i].addr1]);
			break;
		case O_LOG10:
			if (eval_vect[prg[i].addr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log10(eval_vect[prg[i].addr1]);
			break;
		case O_lOG2:
			if (eval_vect[prg[i].addr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log2(eval_vect[prg[i].addr1]);
			break;
		case O_FLOOR:
			eval_vect[i] = floor(eval_vect[prg[i].addr1]);
			break;
		case O_CEIL:
			eval_vect[i] = ceil(eval_vect[prg[i].addr1]);
			break;
		case O_ABS:
			eval_vect[i] = fabs(eval_vect[prg[i].addr1]);
			break;
		case O_INV:
			if (fabs(eval_vect[prg[i].addr1]) < DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = 1 / eval_vect[prg[i].addr1];
			break;
		case O_NEG:
			eval_vect[i] = -eval_vect[prg[i].addr1];
			break;
		case O_X2:
			eval_vect[i] = eval_vect[prg[i].addr1] * eval_vect[prg[i].addr1];
			break;
		case O_MIN:
			eval_vect[i] = eval_vect[prg[i].addr1] < eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr1] : eval_vect[prg[i].addr2];
			break;
		case O_MAX:
			eval_vect[i] = eval_vect[prg[i].addr1] > eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr1] : eval_vect[prg[i].addr2];
			break;

		case O_SIN:
			eval_vect[i] = sin(eval_vect[prg[i].addr1]);
			break;
		case O_COS:
			eval_vect[i] = cos(eval_vect[prg[i].addr1]);
			break;
		case O_TAN:
			eval_vect[i] = tan(eval_vect[prg[i].addr1]);
			break;

		case O_ASIN:
			eval_vect[i] = asin(eval_vect[prg[i].addr1]);
			break;
		case O_ACOS:
			eval_vect[i] = acos(eval_vect[prg[i].addr1]);
			break;
		case O_ATAN:
			eval_vect[i] = atan(eval_vect[prg[i].addr1]);
			break;
		case O_IFLZ:
			eval_vect[i] = eval_vect[prg[i].addr1] < 0 ? eval_vect[prg[i].addr2] : eval_vect[prg[i].addr3];
			break;
		case O_IFALBCD:
			eval_vect[i] = eval_vect[prg[i].addr1] < eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
			break;
		case O_IF_A_OR_B_CD:
			eval_vect[i] = eval_vect[prg[i].addr1] < 0 || eval_vect[prg[i].addr2] < 0 ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
			break;
		case O_IF_A_XOR_B_CD:
			eval_vect[i] = (eval_vect[prg[i].addr1] < 0) != (eval_vect[prg[i].addr2] < 0) ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
			break;
		case O_INPUTS_AVERAGE:
			eval_vect[i] = 0;
			break;
		case O_NUM_INPUTS:
			eval_vect[i] = 0;
			break;

		default:  // a variable
			if (prg[i].op < num_total_variables)
				eval_vect[i] = inputs[prg[i].op];
			else
				eval_vect[i] = real_constants[prg[i].op - num_total_variables];
			break;
		}
		if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
			delete[] eval_vect;
			index_error_gene = i;
			return false;
		}
	}
	outputs[0] = eval_vect[max_index_best_genes];
	delete[] eval_vect;

	return true;

}
//---------------------------------------------------------------------------
bool t_mep_chromosome::get_first_max_index(double *inputs, int &max_index, int &index_error_gene)
{
	bool is_error_case;  // division by zero, other errors

	double *eval_vect = new double[code_length];

	max_index = -1;
	double max_value = -DBL_MAX;

	for (int i = 0; i < code_length; i++)   // read the t_mep_chromosome from top to down
	{
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		is_error_case = false;
		switch (prg[i].op) {
		case  O_ADDITION:  // +
			eval_vect[i] = eval_vect[prg[i].addr1] + eval_vect[prg[i].addr2];
			break;
		case  O_SUBTRACTION:  // -
			eval_vect[i] = eval_vect[prg[i].addr1] - eval_vect[prg[i].addr2];
			break;
		case  O_MULTIPLICATION:  // *
			eval_vect[i] = eval_vect[prg[i].addr1] * eval_vect[prg[i].addr2];
			break;
		case  O_DIVISION:  //  /
			if (fabs(eval_vect[prg[i].addr2]) < DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = eval_vect[prg[i].addr1] / eval_vect[prg[i].addr2];
			break;
		case O_POWER:
			eval_vect[i] = pow(eval_vect[prg[i].addr1], eval_vect[prg[i].addr2]);
			break;
		case O_SQRT:
			if (eval_vect[prg[i].addr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = sqrt(eval_vect[prg[i].addr1]);
			break;
		case O_EXP:
			eval_vect[i] = exp(eval_vect[prg[i].addr1]);

			break;
		case O_POW10:
			eval_vect[i] = pow(10, eval_vect[prg[i].addr1]);
			break;
		case O_LN:
			if (eval_vect[prg[i].addr1] <= 0)
				is_error_case = true;
			else                // an exception occured !!!
				eval_vect[i] = log(eval_vect[prg[i].addr1]);
			break;
		case O_LOG10:
			if (eval_vect[prg[i].addr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log10(eval_vect[prg[i].addr1]);
			break;
		case O_lOG2:
			if (eval_vect[prg[i].addr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log2(eval_vect[prg[i].addr1]);
			break;
		case O_FLOOR:
			eval_vect[i] = floor(eval_vect[prg[i].addr1]);
			break;
		case O_CEIL:
			eval_vect[i] = ceil(eval_vect[prg[i].addr1]);
			break;
		case O_ABS:
			eval_vect[i] = fabs(eval_vect[prg[i].addr1]);
			break;
		case O_INV:
			if (fabs(eval_vect[prg[i].addr1]) < DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = 1.0 / eval_vect[prg[i].addr1];
			break;
		case O_NEG:
			eval_vect[i] = -eval_vect[prg[i].addr1];
			break;
		case O_X2:
			eval_vect[i] = eval_vect[prg[i].addr1] * eval_vect[prg[i].addr1];
			break;
		case O_MIN:
			eval_vect[i] = eval_vect[prg[i].addr1] < eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr1] : eval_vect[prg[i].addr2];
			break;
		case O_MAX:
			eval_vect[i] = eval_vect[prg[i].addr1] > eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr1] : eval_vect[prg[i].addr2];
			break;

		case O_SIN:
			eval_vect[i] = sin(eval_vect[prg[i].addr1]);
			break;
		case O_COS:
			eval_vect[i] = cos(eval_vect[prg[i].addr1]);
			break;
		case O_TAN:
			eval_vect[i] = tan(eval_vect[prg[i].addr1]);
			break;

		case O_ASIN:
			eval_vect[i] = asin(eval_vect[prg[i].addr1]);
			break;
		case O_ACOS:
			eval_vect[i] = acos(eval_vect[prg[i].addr1]);
			break;
		case O_ATAN:
			eval_vect[i] = atan(eval_vect[prg[i].addr1]);
			break;
		case O_IFLZ:
			eval_vect[i] = eval_vect[prg[i].addr1] < 0 ? eval_vect[prg[i].addr2] : eval_vect[prg[i].addr3];
			break;
		case O_IFALBCD:
			eval_vect[i] = eval_vect[prg[i].addr1] < eval_vect[prg[i].addr2] ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
			break;
		case O_IF_A_OR_B_CD:
			eval_vect[i] = eval_vect[prg[i].addr1] < 0 || eval_vect[prg[i].addr2] < 0 ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
			break;
		case O_IF_A_XOR_B_CD:
			eval_vect[i] = (eval_vect[prg[i].addr1] < 0) != (eval_vect[prg[i].addr2] < 0) ? eval_vect[prg[i].addr3] : eval_vect[prg[i].addr4];
			break;


		default:  // a variable
			if (prg[i].op < num_total_variables)
				eval_vect[i] = inputs[prg[i].op];
			else
				eval_vect[i] = real_constants[prg[i].op - num_total_variables];
			break;
		}
		if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
			delete[] eval_vect;
			index_error_gene = i;
			return false;
		}
		else
			if (max_value < eval_vect[i]) {
				max_value = eval_vect[i];
				max_index = i;
			}
	}

	delete[] eval_vect;

	return true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::compute_eval_matrix_double(int num_training_data,
			double **cached_variables_eval_matrix, int num_actual_variables, int * actual_enabled_variables,
			int *line_of_constants, double ** eval_double, t_seed & seed)
{
	//	bool is_error_case;  // division by zero, other errors

	for (int i = 0; i < code_length; i++)   // read the t_mep_chromosome from top to down
	{
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		double *arg1, *arg2, *arg3, *arg4;
		double *eval = eval_double[i];
		//int num_training_data = mep_dataset->get_num_rows();

		if (prg[i].op < 0) {// an operator
			if (prg[prg[i].addr1].op >= 0)
				if (prg[prg[i].addr1].op < num_total_variables)
					arg1 = cached_variables_eval_matrix[prg[prg[i].addr1].op];
				else
					arg1 = eval_double[line_of_constants[prg[prg[i].addr1].op - num_total_variables]];
			else
				arg1 = eval_double[prg[i].addr1];

			if (prg[prg[i].addr2].op >= 0)
				if (prg[prg[i].addr2].op < num_total_variables)
					arg2 = cached_variables_eval_matrix[prg[prg[i].addr2].op];
				else
					arg2 = eval_double[line_of_constants[prg[prg[i].addr2].op - num_total_variables]];
			else
				arg2 = eval_double[prg[i].addr2];

			if (prg[prg[i].addr3].op >= 0)
				if (prg[prg[i].addr3].op < num_total_variables)
					arg3 = cached_variables_eval_matrix[prg[prg[i].addr3].op];
				else
					arg3 = eval_double[line_of_constants[prg[prg[i].addr3].op - num_total_variables]];
			else
				arg3 = eval_double[prg[i].addr3];

			if (prg[prg[i].addr4].op >= 0)
				if (prg[prg[i].addr4].op < num_total_variables)
					arg4 = cached_variables_eval_matrix[prg[prg[i].addr4].op];
				else
					arg4 = eval_double[line_of_constants[prg[prg[i].addr4].op - num_total_variables]];
			else
				arg4 = eval_double[prg[i].addr4];
		}
		else {
			arg1 = arg2 = arg3 = arg4 = NULL; // just to silence some compiler warnings
		}

		switch (prg[i].op) {
		case  O_ADDITION: {  // +
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] + arg2[k];
		}
			break;
		case  O_SUBTRACTION: {  // -
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] - arg2[k];
		}
			break;
		case  O_MULTIPLICATION: {  // *
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] * arg2[k];
		}
			break;
		case  O_DIVISION:  //  /
			for (int k = 0; k < num_training_data; k++)
				if (fabs(arg2[k]) < DIVISION_PROTECT) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else
					eval[k] = arg1[k] / arg2[k];
			break;
		case O_POWER:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = pow(arg1[k], arg2[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_SQRT:
			for (int k = 0; k < num_training_data; k++) {
				if (arg1[k] <= 0) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = sqrt(arg1[k]);
			}
			break;
		case O_EXP:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = exp(arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
			}

			break;
		case O_POW10:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = pow(10, arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_LN:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = log(arg1[k]);

			break;
		case O_LOG10:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = log10(arg1[k]);

			break;
		case O_lOG2:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = log2(arg1[k]);
			break;
		case O_FLOOR:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = floor(arg1[k]);
			break;
		case O_CEIL:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = ceil(arg1[k]);
			break;
		case O_ABS:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = fabs(arg1[k]);
			break;
		case O_INV:
			for (int k = 0; k < num_training_data; k++)
				if (fabs(arg1[k]) < DIVISION_PROTECT) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else
					eval[k] = 1.0 / arg1[k];
			break;
		case O_NEG:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = -arg1[k];
			break;
		case O_X2:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] * arg1[k];
			break;
		case O_MIN:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < arg2[k] ? arg1[k] : arg2[k];
			break;
		case O_MAX:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] > arg2[k] ? arg1[k] : arg2[k];
			break;
		case O_SIN:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = sin(arg1[k]);
			break;
		case O_COS:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = cos(arg1[k]);
			break;
		case O_TAN:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = tan(arg1[k]);
			break;

		case O_ASIN:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = asin(arg1[k]);

			break;
		case O_ACOS:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					prg[i].op = actual_enabled_variables[mep_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = acos(arg1[k]);

			break;
		case O_ATAN:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = atan(arg1[k]);
			}
			break;
		case O_IFLZ:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < 0 ? arg2[k] : arg3[k];
			break;
		case O_IFALBCD:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < arg2[k] ? arg3[k] : arg4[k];
			break;
		case O_IF_A_OR_B_CD:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < 0 || arg2[k] < 0 ? arg3[k] : arg4[k];
			break;
		case O_IF_A_XOR_B_CD:
			for (int k = 0; k < num_training_data; k++)
				eval[k] = (arg1[k] < 0) != (arg2[k] < 0) ? arg3[k] : arg4[k];
			break;

		default:  // a constant
			if (prg[i].op >= num_total_variables)
				if (line_of_constants[prg[i].op - num_total_variables] == -1) {
					line_of_constants[prg[i].op - num_total_variables] = i;
					int constant_index = prg[i].op - num_total_variables;
					for (int k = 0; k < num_training_data; k++)
						eval[k] = real_constants[constant_index];
				}

			break;
		}

	}
}
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
double t_mep_chromosome::get_fitness(void)
{
	return fitness;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::get_index_best_gene(int output_index)
{
	return index_best_genes[output_index];
}
//---------------------------------------------------------------------------
double t_mep_chromosome::get_best_class_threshold(void)
{
	return best_class_threshold;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::set_gene_operation(int gene_index, int new_operation)
{
	if (prg)
		prg[gene_index].op = new_operation;
}
//---------------------------------------------------------------------------
double t_mep_chromosome::get_num_incorrectly_classified(void)
{
	return num_incorrectly_classified;
}
//---------------------------------------------------------------------------
