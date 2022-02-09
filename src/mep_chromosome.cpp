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
	max_index_best_genes = 0;
	code_length = 0;
	num_total_variables = 0;
	num_utilized_genes = 0;
	num_program_outputs = 1;
	num_classes = 0;

	centers = NULL;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::allocate_memory(unsigned int _code_length, unsigned int num_vars,
		bool use_constants, const t_mep_constants *  constants,
	unsigned int _num_program_outputs, unsigned int _num_classes)
{
	this->code_length = _code_length;
	prg = new t_code3[code_length];
	simplified_prg = NULL;
	this->num_total_variables = num_vars;
	this->num_program_outputs = _num_program_outputs;
	this->num_classes = _num_classes;

	if (use_constants) {
		if (constants->get_constants_type() == MEP_USER_DEFINED_CONSTANTS) {
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
	if (num_program_outputs)
		index_best_genes = new unsigned int[num_program_outputs];

	if (num_classes)
		centers = new double[num_classes];
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

	if (centers) {
		delete[] centers;
		centers = NULL;
	}

	num_incorrectly_classified = 0;
	max_index_best_genes = 0;
}
//---------------------------------------------------------------------------
t_mep_chromosome& t_mep_chromosome::operator = (const t_mep_chromosome &source)
{
	if (this != &source) {

		code_length = source.code_length;
		num_total_variables = source.num_total_variables;
		num_program_outputs = source.num_program_outputs;

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

		if (source.real_constants) {
			if (!real_constants)
				real_constants = new double[num_constants];
			//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
			for (unsigned int i = 0; i < num_constants; i++)
				real_constants[i] = source.real_constants[i];
		}

		num_incorrectly_classified = source.num_incorrectly_classified;
		num_classes = source.num_classes;
		if (source.centers) {
			if (!centers)
				centers = new double[num_classes];
			for (unsigned int i = 0; i < num_classes; i++)
				centers[i] = source.centers[i];          // the index of the best expression in t_mep_chromosome
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
	num_constants = source.num_constants;
	best_class_threshold = source.best_class_threshold;

	double* tmp_real_constants = real_constants;
	real_constants = source.real_constants;
	source.real_constants = tmp_real_constants;

	num_incorrectly_classified = source.num_incorrectly_classified;

	double *tmp_centers_ptr = centers;
	centers = source.centers;
	source.centers = tmp_centers_ptr;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::to_xml(pugi::xml_node parent)
{
	char tmp_str[100];

	pugi::xml_node node = parent.append_child("code_length");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", code_length);
	data.set_value(tmp_str);

	node = parent.append_child("num_variables");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", num_total_variables);
	data.set_value(tmp_str);

	node = parent.append_child("num_outputs");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", num_program_outputs);
	data.set_value(tmp_str);

	pugi::xml_node node_code = parent.append_child("code");

	for (unsigned int i = 0; i < code_length; i++) {
		node = node_code.append_child("i");
		data = node.append_child(pugi::node_pcdata);
		if (prg[i].op < 0) // operator
			sprintf(tmp_str, "%u %u %u %u %u", prg[i].op, prg[i].addr1, prg[i].addr2, prg[i].addr3, prg[i].addr4);
		else
			sprintf(tmp_str, "%d", prg[i].op);
		data.set_value(tmp_str);
	}
	node = parent.append_child("best_genes");
	data = node.append_child(pugi::node_pcdata);

	if (num_program_outputs) {
		char tmp_s[30];
		char *tmp_genes_str = new char[num_program_outputs * 12 + 1]; // 30 digits for each constant !!!
		tmp_genes_str[0] = 0;
		for (unsigned int c = 0; c < num_program_outputs; c++) {
			sprintf(tmp_s, "%u", index_best_genes[c]);
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
	sprintf(tmp_str, "%u", num_constants);
	data.set_value(tmp_str);

	if (num_constants) {
		node = parent.append_child("constants");
		data = node.append_child(pugi::node_pcdata);

		char *tmp_cst_str = NULL;

		if (real_constants) {
			char tmp_s[30];
			tmp_cst_str = new char[num_constants * 30]; // 30 digits for each constant !!!
			tmp_cst_str[0] = 0;
			for (unsigned int c = 0; c < num_constants; c++) {
				sprintf(tmp_s, "%lg", real_constants[c]);
				strcat(tmp_cst_str, tmp_s);
				strcat(tmp_cst_str, " ");
			}
		}
		data.set_value(tmp_cst_str);
		if (tmp_cst_str)
		delete[] tmp_cst_str;
	}

	node = parent.append_child("num_classes");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", num_classes);
	data.set_value(tmp_str);

	if (centers) {
		node = parent.append_child("class_centers");
		data = node.append_child(pugi::node_pcdata);

		char* tmp_cst_str = NULL;

		char tmp_s[30];
		tmp_cst_str = new char[num_classes * 30]; // 30 digits for each constant !!!
		tmp_cst_str[0] = 0;
		for (unsigned int c = 0; c < num_classes; c++) {
			sprintf(tmp_s, "%lg", centers[c]);
			strcat(tmp_cst_str, tmp_s);
			strcat(tmp_cst_str, " ");
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
		code_length = (unsigned int)atoi(value_as_cstring);
	}
	else
		code_length = 0;

	node = parent.child("num_variables");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_total_variables = (unsigned int)atoi(value_as_cstring);
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
					int num_read = sscanf(value_as_cstring, "%d%u%u%u%u", &prg[i].op, &prg[i].addr1, &prg[i].addr2, &prg[i].addr3, &prg[i].addr4);
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
						int num_read = sscanf(value_as_cstring, "%d%u%u%u%u", &prg[i].op, &prg[i].addr1, &prg[i].addr2, &prg[i].addr3, &prg[i].addr4);
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
		num_program_outputs = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_program_outputs = 1;

	if (num_program_outputs)
		index_best_genes = new unsigned int[num_program_outputs];

	node = parent.child("best_genes");
	if (node) {
		const char *value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		for (unsigned int c = 0; c < num_program_outputs; c++) {
			sscanf(value_as_cstring + num_jumped_chars, "%u", &index_best_genes[c]);
			size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}
	else {
// old versions
		node = parent.child("best");
		if (node) {
			const char *value_as_cstring = node.child_value();
			index_best_genes[0] = (unsigned int)atoi(value_as_cstring);
			max_index_best_genes = index_best_genes[0];
		}
	}

	node = parent.child("max_index_best_genes");
	if (node) {
		const char *value_as_cstring = node.child_value();
		max_index_best_genes = (unsigned int)atoi(value_as_cstring);
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
		num_constants = (unsigned int)atoi(value_as_cstring);
	}
	node = parent.child("constants");
	if (node) {
		const char *value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		real_constants = new double[num_constants];
		for (unsigned int c = 0; c < num_constants; c++) {
			sscanf(value_as_cstring + num_jumped_chars, "%lf", &real_constants[c]);
			size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}

	node = parent.child("num_classes");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_classes = (unsigned int)atoi(value_as_cstring);
	}

	node = parent.child("class_centers");
	if (node) {
		const char* value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		centers = new double[num_classes];
		for (unsigned int c = 0; c < num_classes; c++) {
			sscanf(value_as_cstring + num_jumped_chars, "%lf", &centers[c]);
			size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::mark(unsigned int position, bool* marked)
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
		case O_FMOD:
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
	for (unsigned int i = 0; i < code_length; marked[i++] = false);

	unsigned int *skipped = new unsigned int[code_length];
	for (unsigned int o = 0; o < num_program_outputs; o++) {
		mark(index_best_genes[o], marked);

		if (!marked[0])
			skipped[0] = 1;
		else
			skipped[0] = 0;
		for (unsigned int i = 1; i <= index_best_genes[o]; i++)
			if (!marked[i])
				skipped[i] = skipped[i - 1] + 1;
			else
				skipped[i] = skipped[i - 1];
	}
	if (simplified_prg)
		delete[] simplified_prg;
	simplified_prg = new t_code3[code_length];

	num_utilized_genes = 0;
	for (unsigned int o = 0; o < num_program_outputs; o++)
	for (unsigned int i = 0; i <= index_best_genes[o]; i++)
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
		const int *actual_operators, unsigned int num_actual_operators,
	const unsigned int *actual_variables, unsigned int num_actual_variables, t_seed& seed)
// randomly initializes the individuals
{
	// I have to generate the constants for this individuals
	if (parameters.get_constants_probability() > 1E-6) {
		if (mep_constants.get_constants_type() == MEP_USER_DEFINED_CONSTANTS) {
			for (unsigned int c = 0; c < num_constants; c++)
				real_constants[c] = mep_constants.get_constants_double(c);
		}
		else {// automatic constants
			for (unsigned int c = 0; c < num_constants; c++)
				real_constants[c] = mep_real_rand(seed, mep_constants.get_min_constants_interval_double(),
											mep_constants.get_max_constants_interval_double());
		}
	}

	double sum = parameters.get_variables_probability() + parameters.get_constants_probability();
	double p = mep_real_rand(seed, 0, sum);

	if (p <= parameters.get_variables_probability())
		prg[0].op = (int)actual_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];
	else
		prg[0].op = (int)num_total_variables + (int)mep_unsigned_int_rand(seed, 0, num_constants - 1);

	for (unsigned int i = 1; i < parameters.get_code_length(); i++) {
		p = mep_real_rand(seed, 0, 1);

		if (p <= parameters.get_operators_probability())
			prg[i].op = (int)actual_operators[mep_unsigned_int_rand(seed, 0, num_actual_operators - 1)];
		else
			if (p <= parameters.get_operators_probability() + parameters.get_variables_probability())
				prg[i].op = (int)actual_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];
			else
				prg[i].op = (int)num_total_variables + (int)mep_unsigned_int_rand(seed, 0, num_constants - 1);

		prg[i].addr1 = mep_unsigned_int_rand(seed, 0, i - 1);
		prg[i].addr2 = mep_unsigned_int_rand(seed, 0, i - 1);
		prg[i].addr3 = mep_unsigned_int_rand(seed, 0, i - 1);
		prg[i].addr4 = mep_unsigned_int_rand(seed, 0, i - 1);
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
	double p = mep_real_rand(seed, 0, 1);
	if (p < parameters.get_mutation_probability()) {
		double sum = parameters.get_variables_probability() + parameters.get_constants_probability();
		double q = mep_real_rand(seed, 0, sum);

		if (q <= parameters.get_variables_probability())
			prg[0].op = (int)actual_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];
		else
			prg[0].op = (int)num_total_variables + (int)mep_unsigned_int_rand(seed, 0, num_constants - 1);
	}
	// mutate the other instructions
	for (unsigned int i = 1; i < code_length; i++) {
		p = mep_real_rand(seed, 0, 1);      // mutate the operator
		if (p < parameters.get_mutation_probability()) {
			double q = mep_real_rand(seed, 0, 1);

			if (q <= parameters.get_operators_probability())
				prg[i].op = actual_operators[mep_unsigned_int_rand(seed, 0, num_actual_operators - 1)];
			else
				if (q <= parameters.get_operators_probability() + parameters.get_variables_probability())
					prg[i].op = (int)actual_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];
				else
					prg[i].op = (int)num_total_variables + (int)mep_unsigned_int_rand(seed, 0, num_constants - 1);
		}

		p = mep_real_rand(seed, 0, 1);      // mutate the first address  (adr1)
		if (p < parameters.get_mutation_probability())
			prg[i].addr1 = mep_unsigned_int_rand(seed, 0, i - 1);

		p = mep_real_rand(seed, 0, 1);      // mutate the second address   (adr2)
		if (p < parameters.get_mutation_probability())
			prg[i].addr2 = mep_unsigned_int_rand(seed, 0, i - 1);
		p = mep_real_rand(seed, 0, 1);      // mutate the 3rd address   (adr3)
		if (p < parameters.get_mutation_probability())
			prg[i].addr3 = mep_unsigned_int_rand(seed, 0, i - 1);
		p = mep_real_rand(seed, 0, 1);      // mutate the 4th address   (adr4)
		if (p < parameters.get_mutation_probability())
			prg[i].addr4 = mep_unsigned_int_rand(seed, 0, i - 1);
	}
	// lets see if I can evolve constants

	if (mep_constants.get_constants_can_evolve() && 
		mep_constants.get_constants_type() == MEP_AUTOMATIC_CONSTANTS)
		for (unsigned int c = 0; c < num_constants; c++) {
			p = mep_real_rand(seed, 0, 1);      // mutate the operator
			double tmp_cst_d = mep_real_rand(seed, 0, mep_constants.get_constants_mutation_max_deviation());
			
			if (p < parameters.get_mutation_probability()) {
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
	pct = 1 + mep_unsigned_int_rand(seed, 0, code_length - 3);
	for (i = 0; i < pct; i++) {
		offspring1.prg[i] = prg[i];
		offspring2.prg[i] = parent2.prg[i];
	}
	for (i = pct; i < code_length; i++) {
		offspring1.prg[i] = parent2.prg[i];
		offspring2.prg[i] = prg[i];
	}

	if (num_constants && mep_constants.get_constants_can_evolve() &&
			mep_constants.get_constants_type() == MEP_AUTOMATIC_CONSTANTS) {
		pct = 1 + mep_unsigned_int_rand(seed, 0, num_constants - 3);
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
//---------------------------------------------------------------------------
void t_mep_chromosome::uniform_crossover(const t_mep_chromosome &parent2,
	t_mep_chromosome &offspring1, t_mep_chromosome &offspring2,
	const t_mep_constants & mep_constants, t_seed& seed)
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
        mep_constants.get_constants_type() == MEP_AUTOMATIC_CONSTANTS)
        for (unsigned int c = 0; c < num_constants; c++) {
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
bool t_mep_chromosome::evaluate_double(double *inputs, double *outputs, unsigned int &index_error_gene)
{
	double *eval_vect = new double[max_index_best_genes + 1];

	for (unsigned int i = 0; i <= max_index_best_genes; i++){   // read the t_mep_chromosome from top to down
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		bool is_error_case = false;// division by zero, other errors
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
			if (fabs(eval_vect[prg[i].addr2]) < MEP_DIVISION_PROTECT)
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
			if (fabs(eval_vect[prg[i].addr1]) < MEP_DIVISION_PROTECT)
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
			if (eval_vect[prg[i].addr1] < -1 || eval_vect[prg[i].addr1] > 1)
				is_error_case = true;
			else
				eval_vect[i] = asin(eval_vect[prg[i].addr1]);
			break;
		case O_ACOS:
			if (eval_vect[prg[i].addr1] < -1 || eval_vect[prg[i].addr1] > 1)
				is_error_case = true;
			else
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
		case O_FMOD:
			if (fabs(eval_vect[prg[i].addr2]) < MEP_DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = fmod(eval_vect[prg[i].addr1], eval_vect[prg[i].addr2]);
			break;
		case O_INPUTS_AVERAGE:
			eval_vect[i] = 0;
			break;
		case O_NUM_INPUTS:
			eval_vect[i] = 0;
			break;

		default:  // a variable
			if (prg[i].op < (int)num_total_variables)
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
	errno = 0;
	return true;

}
//---------------------------------------------------------------------------
bool t_mep_chromosome::get_first_max_index(double *inputs, unsigned int &max_index, double & max_value,
					unsigned int &index_error_gene,	double *values_for_output_genes)
{
	double *eval_vect = new double[code_length];

	max_index = 0;
	max_value = -DBL_MAX;

	for (unsigned int i = 0; i < code_length; i++){   // read the t_mep_chromosome from top to down
	
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		bool is_error_case = false;// division by zero, other errors
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
			if (fabs(eval_vect[prg[i].addr2]) < MEP_DIVISION_PROTECT)
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
			if (fabs(eval_vect[prg[i].addr1]) < MEP_DIVISION_PROTECT)
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
			if (eval_vect[prg[i].addr1] < -1 || eval_vect[prg[i].addr1] > 1)
				is_error_case = true;
			else
				eval_vect[i] = asin(eval_vect[prg[i].addr1]);
			break;
		case O_ACOS:
			if (eval_vect[prg[i].addr1] < -1 || eval_vect[prg[i].addr1] > 1)
				is_error_case = true;
			else
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
		case O_FMOD:
			if (fabs(eval_vect[prg[i].addr2]) < MEP_DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = fmod(eval_vect[prg[i].addr1], eval_vect[prg[i].addr2]);
			break;

		default:  // a variable
			if (prg[i].op < (int)num_total_variables)
				eval_vect[i] = inputs[prg[i].op];
			else
				eval_vect[i] = real_constants[prg[i].op - num_total_variables];
			break;
		} // end switch

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

	if (values_for_output_genes) {
		for (unsigned int c = 0; c < num_classes; c++)
			values_for_output_genes[c] = eval_vect[index_best_genes[c]];
	}

	delete[] eval_vect;
	errno = 0;
	return true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::compute_eval_matrix_double(unsigned int num_training_data,
			double **cached_variables_eval_matrix, 
			unsigned int num_actual_variables, unsigned int * actual_enabled_variables,
			int *line_of_constants, double ** eval_double, t_seed & seed)
{
	//	bool is_error_case;  // division by zero, other errors

	for (unsigned int i = 0; i < code_length; i++){   // read the t_mep_chromosome from top to down
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		double *arg1, *arg2, *arg3, *arg4;
		double *eval = eval_double[i];
		//int num_training_data = mep_dataset->get_num_rows();

		if (prg[i].op < 0) {// an operator
			if (prg[prg[i].addr1].op >= 0)
				if (prg[prg[i].addr1].op < (int)num_total_variables)
					arg1 = cached_variables_eval_matrix[prg[prg[i].addr1].op];
				else
					arg1 = eval_double[line_of_constants[prg[prg[i].addr1].op - num_total_variables]];
			else
				arg1 = eval_double[prg[i].addr1];

			if (prg[prg[i].addr2].op >= 0)
				if (prg[prg[i].addr2].op < (int)num_total_variables)
					arg2 = cached_variables_eval_matrix[prg[prg[i].addr2].op];
				else
					arg2 = eval_double[line_of_constants[prg[prg[i].addr2].op - num_total_variables]];
			else
				arg2 = eval_double[prg[i].addr2];

			if (prg[prg[i].addr3].op >= 0)
				if (prg[prg[i].addr3].op < (int)num_total_variables)
					arg3 = cached_variables_eval_matrix[prg[prg[i].addr3].op];
				else
					arg3 = eval_double[line_of_constants[prg[prg[i].addr3].op - num_total_variables]];
			else
				arg3 = eval_double[prg[i].addr3];

			if (prg[prg[i].addr4].op >= 0)
				if (prg[prg[i].addr4].op < (int)num_total_variables)
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
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = arg1[k] + arg2[k];
				/*
				if (fabs(eval[k]) > MEP_MAX_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				*/
			}
		}
			break;
		case  O_SUBTRACTION: {  // -
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = arg1[k] - arg2[k];
				/*
				if (fabs(eval[k]) > MEP_MAX_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				*/
			}
		}
			break;
		case  O_MULTIPLICATION: {  // *
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = arg1[k] * arg2[k];
				
				if (isinf(fabs(eval[k]))) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				
			}
		}
			break;
		case  O_DIVISION:  //  /
			for (unsigned int k = 0; k < num_training_data; k++)
				if (fabs(arg2[k]) < MEP_DIVISION_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else {
					eval[k] = arg1[k] / arg2[k];
					
					if (isinf(fabs(eval[k]))) {
						prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
						break;
					}
					
				}
			break;
		case O_POWER:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = pow(arg1[k], arg2[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_SQRT:
			for (unsigned int k = 0; k < num_training_data; k++) {
				if (arg1[k] <= 0) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else {
					eval[k] = sqrt(arg1[k]);
					/*
					if (fabs(eval[k]) > MEP_MAX_PROTECT) {
						prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
						break;
					}
					*/
				}
			}
			break;
		case O_EXP:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = exp(arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
			}

			break;
		case O_POW10:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = pow(10, arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_LN:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else {
					eval[k] = log(arg1[k]);
				}

			break;
		case O_LOG10:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else {
					eval[k] = log10(arg1[k]);
				}

			break;
		case O_lOG2:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else {
					eval[k] = log2(arg1[k]);
				}
			break;
		case O_FLOOR:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = floor(arg1[k]);
			break;
		case O_CEIL:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = ceil(arg1[k]);
			break;
		case O_ABS:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = fabs(arg1[k]);
			break;
		case O_INV:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (fabs(arg1[k]) < MEP_DIVISION_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else {
					eval[k] = 1.0 / arg1[k];
					
					if (isinf(fabs(eval[k]))) {
						prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
						break;
					}
					
				}
			break;
		case O_NEG:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = -arg1[k];
			break;
		case O_X2:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = arg1[k] * arg1[k];
				
				if (isinf(fabs(eval[k]))) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				
			}
			break;
		case O_MIN:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < arg2[k] ? arg1[k] : arg2[k];
			break;
		case O_MAX:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] > arg2[k] ? arg1[k] : arg2[k];
			break;
		case O_SIN:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = sin(arg1[k]);
			break;
		case O_COS:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = cos(arg1[k]);
			break;
		case O_TAN:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = tan(arg1[k]);
			break;

		case O_ASIN:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = asin(arg1[k]);

			break;
		case O_ACOS:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = acos(arg1[k]);

			break;
		case O_ATAN:
			for (unsigned int k = 0; k < num_training_data; k++) {
				eval[k] = atan(arg1[k]);
			}
			break;
		case O_IFLZ:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < 0 ? arg2[k] : arg3[k];
			break;
		case O_IFALBCD:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < arg2[k] ? arg3[k] : arg4[k];
			break;
		case O_IF_A_OR_B_CD:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] < 0 || arg2[k] < 0 ? arg3[k] : arg4[k];
			break;
		case O_IF_A_XOR_B_CD:
			for (unsigned int k = 0; k < num_training_data; k++)
				eval[k] = (arg1[k] < 0) != (arg2[k] < 0) ? arg3[k] : arg4[k];
			break;
		case O_FMOD:
			for (unsigned int k = 0; k < num_training_data; k++)
				if (fabs(arg2[k]) < MEP_DIVISION_PROTECT) {
					prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else {
					eval[k] = fmod(arg1[k], arg2[k]);
					/*
					if (fabs(eval[k]) > MEP_MAX_PROTECT) {
						prg[i].op = (int)actual_enabled_variables[mep_unsigned_int_rand(seed, 0, num_actual_variables - 1)];   // the gene is mutated into a terminal
						break;
					}
					*/
				}
			
			break;

		default:  // a constant
			if (prg[i].op >= (int)num_total_variables)
				if (line_of_constants[prg[i].op - num_total_variables] == -1) {
					line_of_constants[prg[i].op - num_total_variables] = (int)i;
					unsigned int constant_index = (unsigned int)prg[i].op - num_total_variables;
					for (unsigned int k = 0; k < num_training_data; k++)
						eval[k] = real_constants[constant_index];// why do I keep the same constant for an entire row??????
				}

			break;
		}

	}
	errno = 0;
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
unsigned int t_mep_chromosome::get_index_best_gene(unsigned int output_index)
{
	return index_best_genes[output_index];
}
//---------------------------------------------------------------------------
double t_mep_chromosome::get_best_class_threshold(void)
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
double t_mep_chromosome::get_num_incorrectly_classified(void)
{
	return num_incorrectly_classified;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::compute_fitness(const t_mep_data& mep_dataset, const t_mep_data& mep_dataset_ts,
	unsigned int* random_subset_indexes, unsigned int random_subset_selection_size,
	double** cached_eval_matrix, double* cached_sum_of_errors, double* cached_threashold, 
	s_value_class* tmp_value_class,
	unsigned int num_actual_variables, unsigned int* actual_enabled_variables,
	double** eval_double, unsigned int problem_type, unsigned int error_measure, 
	t_seed& seed)
{
	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
		if (error_measure == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
			fitness_regression(mep_dataset,
				random_subset_indexes, random_subset_selection_size,
				cached_eval_matrix, cached_sum_of_errors,
				num_actual_variables, actual_enabled_variables,
				eval_double, mep_absolute_error, seed);
		else
			fitness_regression(mep_dataset,
				random_subset_indexes, random_subset_selection_size,
				cached_eval_matrix, cached_sum_of_errors,
				num_actual_variables, actual_enabled_variables,
				eval_double, mep_squared_error, seed);
		break;
	case MEP_PROBLEM_TIME_SERIE:
		if (error_measure == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR)
			fitness_regression(mep_dataset_ts,
				random_subset_indexes, random_subset_selection_size,
				cached_eval_matrix, cached_sum_of_errors,
				num_actual_variables, actual_enabled_variables,
				eval_double, mep_absolute_error, seed);
		else
			fitness_regression(mep_dataset_ts,
				random_subset_indexes, random_subset_selection_size,
				cached_eval_matrix, cached_sum_of_errors,
				num_actual_variables, actual_enabled_variables,
				eval_double, mep_squared_error, seed);
		break;
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
		fitness_binary_classification(mep_dataset,
			random_subset_indexes, random_subset_selection_size,
			cached_eval_matrix, cached_sum_of_errors, cached_threashold,
			num_actual_variables, actual_enabled_variables,
			eval_double, tmp_value_class, seed);
		break;
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		switch (error_measure) {
		case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
			fitness_multi_class_classification_winner_takes_all_fixed(
				mep_dataset, random_subset_indexes,
				random_subset_selection_size,
				cached_eval_matrix, num_actual_variables,
				actual_enabled_variables, eval_double, seed);
			break;
		case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
			fitness_multi_class_classification_smooth(
				mep_dataset, random_subset_indexes,
				random_subset_selection_size,
				cached_eval_matrix, num_actual_variables,
				actual_enabled_variables, eval_double, seed);
			break;
		case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
			fitness_multi_class_classification_winner_takes_all_dynamic(
				mep_dataset, random_subset_indexes,
				random_subset_selection_size,
				cached_eval_matrix, num_actual_variables,
				actual_enabled_variables, eval_double, seed);
			break;
		case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
			fitness_multi_class_classification_closest_center(
				mep_dataset, random_subset_indexes,
				random_subset_selection_size,
				cached_eval_matrix, num_actual_variables,
				actual_enabled_variables, eval_double, seed);
			break;
		}
	}
}
//---------------------------------------------------------------------------
unsigned int t_mep_chromosome::get_closest_class_index(double program_output)
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
bool t_mep_chromosome::get_class_index_for_winner_takes_all_dynamic(double *inputs, unsigned int& class_index)
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
