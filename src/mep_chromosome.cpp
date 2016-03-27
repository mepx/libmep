#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <float.h>


#include "mep_chromosome.h"
#include "mep_operators.h"
#include "mep_constants.h"
#include "mep_data.h"
#include "mep_rands.h"


#define DIVISION_PROTECT 1E-10
//---------------------------------------------------------------------------
t_mep_chromosome::t_mep_chromosome()
{
	prg = NULL;
	simplified_prg = NULL;
	index_best_gene = -1;
	fitness = 0;

	num_constants = 0;
	constants_double = NULL;
	best_class_threshold = 0;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::allocate_memory(long code_length, int num_vars, bool use_constants, t_mep_constants *  constants)
{
	this->code_length = code_length;
	prg = new code3[code_length];
	simplified_prg = NULL;
	this->num_total_variables = num_vars;

	if (use_constants) {
		if (constants->get_constants_type() == USER_DEFINED_CONSTANTS) {
			num_constants = constants->get_num_user_defined_constants();
			constants_double = new double[num_constants];
		}
		else {// automatic constants
			num_constants = constants->get_num_automatic_constants();
			if (num_constants)
				constants_double = new double[num_constants];
		}
	}
	else
		num_constants = 0;
}
//---------------------------------------------------------------------------
t_mep_chromosome::~t_mep_chromosome()
{
	clear();
}
//---------------------------------------------------------------------------
void t_mep_chromosome::clear(void)
{
	if (prg){
		delete[] prg;
		prg = NULL;
	}
	if (simplified_prg){
		delete[] simplified_prg;
		simplified_prg = NULL;
	}
	index_best_gene = -1;
	fitness = 0;

	num_constants = 0;
	code_length = 0;
	num_total_variables = 0;

	if (constants_double){
		delete[] constants_double;
		constants_double = NULL;
	}
}
//---------------------------------------------------------------------------
t_mep_chromosome& t_mep_chromosome::operator = (const t_mep_chromosome &source)
{
	if (this != &source){
		//		clear();

		code_length = source.code_length;
		num_total_variables = source.num_total_variables;

		if (!prg)// I do this for stats only
			prg = new code3[code_length];        // a string of genes
		//		if (!simplified_prg)// I do this for stats only
		//			simplified_prg = new code3[code_length];        // a string of genes
		//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
		for (int i = 0; i < code_length; i++){
			prg[i] = source.prg[i];
			//			simplified_prg[i] = source.simplified_prg[i];
		}
		fitness = source.fitness;        // the fitness
		index_best_gene = source.index_best_gene;          // the index of the best expression in t_mep_chromosome

		num_constants = source.num_constants;
		best_class_threshold = source.best_class_threshold;

		if (source.constants_double){
			if (!constants_double)
				constants_double = new double[num_constants];
			//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
			for (int i = 0; i < num_constants; i++)
				constants_double[i] = source.constants_double[i];
		}
	}
	return *this;
}
//---------------------------------------------------------------------------
void print_instruction(int op, int adr1, int adr2, int adr3, int adr4, char * tmp_s)
{
	switch (op){
	case O_ADDITION:
		sprintf(tmp_s, "prg[%d] + prg[%d];", adr1, adr2);
		break;
	case O_SUBTRACTION:
		sprintf(tmp_s, "prg[%d] - prg[%d];", adr1, adr2);
		break;
	case O_MULTIPLICATION:
		sprintf(tmp_s, "prg[%d] * prg[%d];", adr1, adr2);
		break;
	case O_DIVISION:
		sprintf(tmp_s, "prg[%d] / prg[%d];", adr1, adr2);
		break;
	case O_POWER:
		sprintf(tmp_s, "pow(prg[%d], prg[%d]);", adr1, adr2);
		break;
	case O_SQRT:
		sprintf(tmp_s, "sqrt(prg[%d]);", adr1);
		break;
	case O_EXP:
		sprintf(tmp_s, "exp(prg[%d]);", adr1);
		break;
	case O_POW10:
		sprintf(tmp_s, "pow(10, prg[%d]);", adr1);
		break;
	case O_LN:
		sprintf(tmp_s, "log(prg[%d]);", adr1);
		break;
	case O_LOG10:
		sprintf(tmp_s, "log10(prg[%d]);", adr1);
		break;
	case O_lOG2:
		sprintf(tmp_s, "log2(prg[%d]);", adr1);
		break;
	case O_FLOOR:
		sprintf(tmp_s, "floor(prg[%d]);", adr1);
		break;
	case O_CEIL:
		sprintf(tmp_s, "ceil(prg[%d]);", adr1);
		break;
	case O_ABS:
		sprintf(tmp_s, "fabs(prg[%d]);", adr1);
		break;
	case O_INV:
		sprintf(tmp_s, "-prg[%d];", adr1);
		break;
	case O_X2:
		sprintf(tmp_s, "prg[%d] * prg[%d];", adr1, adr1);
		break;
	case O_MIN:
		sprintf(tmp_s, "prg[%d] < prg[%d]?prg[%d] : prg[%d]; // min", adr1, adr2, adr1, adr2);
		break;
	case O_MAX:
		sprintf(tmp_s, "prg[%d] > prg[%d]?prg[%d] : prg[%d]; // max", adr1, adr2, adr1, adr2);
		break;
	case O_SIN:
		sprintf(tmp_s, "sin(prg[%d]);", adr1);
		break;
	case O_COS:
		sprintf(tmp_s, "cos(prg[%d]);", adr1);
		break;
	case O_TAN:
		sprintf(tmp_s, "tan(prg[%d]);", adr1);
		break;
	case O_ASIN:
		sprintf(tmp_s, "asin(prg[%d]);", adr1);
		break;
	case O_ACOS:
		sprintf(tmp_s, "acos(prg[%d]);", adr1);
		break;
	case O_ATAN:
		sprintf(tmp_s, "atan(prg[%d]);", adr1);
		break;
	case O_IFLZ:
		sprintf(tmp_s, "prg[%d] < 0?prg[%d] : prg[%d]; // ifalzbc", adr1, adr2, adr3);
		break;
	case O_IFALBCD:
		sprintf(tmp_s, "prg[%d] < prg[%d]?prg[%d] : prg[%d]; // ifalbcd", adr1, adr2, adr3, adr4);
		break;
	}
}
//---------------------------------------------------------------------------------
char * t_mep_chromosome::to_C_double(bool simplified, double *data, int problem_type, int num_classes)
{
	char *prog = new char[(code_length + num_constants + num_total_variables) * 100 + 1000];
	char tmp_s[100];
	prog[0] = 0;
	strcat(prog, "#include <math.h>\n");
	strcat(prog, "#include <stdio.h>\n");
	strcat(prog, "\n");

	strcat(prog, "void mepx(double *x /*inputs*/, double *outputs)");
	strcat(prog, "\n");
	strcat(prog, "{");
	strcat(prog, "\n");

	// here I have to declare the constants
	if (num_constants){
		strcat(prog, "//constants ...");

		strcat(prog, "\n");

		sprintf(tmp_s, "  double constants[%ld];", num_constants);
		strcat(prog, tmp_s);
		strcat(prog, "\n");
		for (int i = 0; i < num_constants; i++){
			sprintf(tmp_s, "  constants[%d] = %lf;", i, constants_double[i]);
			strcat(prog, tmp_s);
			strcat(prog, "\n");
		}
		strcat(prog, "\n");
	}



	if (simplified){
		strcat(prog, "  double prg[");
		sprintf(tmp_s, "%d", num_utilized);
		strcat(prog, tmp_s);
		strcat(prog, "];");
		strcat(prog, "\n");

		for (int i = 0; i < num_utilized; i++){
			sprintf(tmp_s, "  prg[%d] = ", i);
			strcat(prog, tmp_s);

			if (simplified_prg[i].op < 0) {
				print_instruction(simplified_prg[i].op, simplified_prg[i].adr1, simplified_prg[i].adr2, simplified_prg[i].adr3, simplified_prg[i].adr4, tmp_s);
				strcat(prog, tmp_s);
				strcat(prog, "\n");
			}
			else
			{ // a variable
				if (simplified_prg[i].op < num_total_variables) {
					sprintf(tmp_s, "x[%d];", simplified_prg[i].op);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
				else{
					sprintf(tmp_s, "constants[%d];", simplified_prg[i].op - num_total_variables);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
			}
		}

		strcat(prog, "\n");
		if (problem_type == MEP_PROBLEM_REGRESSION)
			sprintf(tmp_s, "  outputs[0] = prg[%d];", num_utilized - 1);
		else
			if (problem_type == MEP_PROBLEM_BINARY_CLASSIFICATION)
			  sprintf(tmp_s, "  if (prg[%d] <= %lf)\n    outputs[0] = 0;\n  else\n    outputs[0] = 1;", num_utilized - 1, best_class_threshold);
			else {
// cannot be here
			}

		strcat(prog, tmp_s);
	}
	else{// not simplified
		strcat(prog, "  double prg[");
		sprintf(tmp_s, "%ld", code_length);
		strcat(prog, tmp_s);
		strcat(prog, "];");
		strcat(prog, "\n");

		for (int i = 0; i < code_length; i++){
			sprintf(tmp_s, "  prg[%d] = ", i);
			strcat(prog, tmp_s);

			if (prg[i].op < 0){
				print_instruction(prg[i].op, prg[i].adr1, prg[i].adr2, prg[i].adr3, prg[i].adr4, tmp_s);
				strcat(prog, tmp_s);
				strcat(prog, "\n");
			}
			else
			{ // a variable
				if (prg[i].op < num_total_variables){
					sprintf(tmp_s, "x[%d];", prg[i].op);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
				else{
					sprintf(tmp_s, "constants[%d];", prg[i].op - num_total_variables);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
			}
		}

		strcat(prog, "\n");
		if (problem_type == MEP_PROBLEM_REGRESSION) {
			sprintf(tmp_s, "  outputs[0] = prg[%d];", index_best_gene);
			strcat(prog, tmp_s);
		}
		else
			if (problem_type == MEP_PROBLEM_BINARY_CLASSIFICATION) {
				sprintf(tmp_s, "  if (prg[%d] <= %lg)\n    outputs[0] = 0;\n  else\n    outputs[0] = 1;", index_best_gene, best_class_threshold);
				strcat(prog, tmp_s);
			}
			else {
				sprintf(tmp_s, "// find maximal value and divide index by num classes\n");
				strcat(prog, tmp_s);
				
				strcat(prog, "  double max_value = prg[0];\n");
				strcat(prog, "  int index_max_value = 0;\n");
				sprintf(tmp_s, "  for (int i = 1; i < %d; i++)\n", code_length);
				strcat(prog, tmp_s);

				strcat(prog, "    if (max_value > prg[i] + 1e-6){\n      max_value = prg[i];\n      index_max_value = i;\n    }\n");

				sprintf(tmp_s, "  outputs[0] = index_max_value %% %d;\n", num_classes);
				strcat(prog, tmp_s);
			}
	}
	strcat(prog, "\n");
	strcat(prog, "}\n");

	strcat(prog, "\n");
	strcat(prog, "int main(void)\n");
	strcat(prog, "{\n");
	strcat(prog, "\n");
	strcat(prog, "//example of utilization ...\n");
	strcat(prog, "\n");
	sprintf(tmp_s, "  double x[%d];\n", num_total_variables);
	strcat(prog, tmp_s);
	for (int i = 0; i < num_total_variables; i++){
		sprintf(tmp_s, "  x[%d] = %lf;\n", i, data[i]);
		strcat(prog, tmp_s);
	}

	strcat(prog, "\n");
	strcat(prog, "  double outputs[1];\n");
	strcat(prog, "\n");
	strcat(prog, "  mepx(x, outputs);\n");
	strcat(prog, "  printf(\"%lf\", outputs[0]);\n");

	strcat(prog, "  getchar();\n");

	strcat(prog, "}\n");

	return prog;
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

	pugi::xml_node node_code = parent.append_child("code");

	for (int i = 0; i < code_length; i++){
		pugi::xml_node node = node_code.append_child("i");
		pugi::xml_node data = node.append_child(pugi::node_pcdata);
		if (prg[i].op < 0) // operator
			sprintf(tmp_str, "%d %d %d %d %d", prg[i].op, prg[i].adr1, prg[i].adr2, prg[i].adr3, prg[i].adr4);
		else
			sprintf(tmp_str, "%d", prg[i].op);
		data.set_value(tmp_str);
	}
	node = parent.append_child("best");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", index_best_gene);
	data.set_value(tmp_str);

	node = parent.append_child("error");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", fitness);
	data.set_value(tmp_str);

	node = parent.append_child("binary_classification_threshold");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", best_class_threshold);
	data.set_value(tmp_str);

	node = parent.append_child("num_constants");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", num_constants);
	data.set_value(tmp_str);

	if (num_constants){
		node = parent.append_child("constants");
		data = node.append_child(pugi::node_pcdata);

		char *tmp_cst_str = NULL;

		if (constants_double){
			char tmp_s[30];
			tmp_cst_str = new char[num_constants * 30]; // 30 digits for each constant !!!
			tmp_cst_str[0] = 0;
			for (int c = 0; c < num_constants; c++){
				sprintf(tmp_s, "%lg", constants_double[c]);
				strcat(tmp_cst_str, tmp_s);
				strcat(tmp_cst_str, " ");
			}
		}
		data.set_value(tmp_cst_str);
		delete[] tmp_cst_str;
	}
	return true;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::from_xml(pugi::xml_node parent)
{
	clear();

	pugi::xml_node node = parent.child("code_length");
	if (node)
	{
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

	if (code_length){
		prg = new code3[code_length];
		pugi::xml_node node_code = parent.child("code");
		if (node_code)
		{
			int i = 0;
			for (pugi::xml_node row = node_code.child("i"); row; row = row.next_sibling("i"), i++)
			{
				const char *value_as_cstring = row.child_value();
				sscanf(value_as_cstring, "%d", &prg[i].op);
				if (prg[i].op < 0) { // operator
					int num_read = sscanf(value_as_cstring, "%d%d%d%d%d", &prg[i].op, &prg[i].adr1, &prg[i].adr2, &prg[i].adr3, &prg[i].adr4);
					if (num_read < 4) {
						prg[i].adr3 = 0;
						prg[i].adr4 = 0;
					}

				}

			}
			if (!i) {
				for (pugi::xml_node row = node_code.child("instruction"); row; row = row.next_sibling("instruction"), i++) {
					const char *value_as_cstring = row.child_value();
					sscanf(value_as_cstring, "%d", &prg[i].op);
					if (prg[i].op < 0) { // operator
						int num_read = sscanf(value_as_cstring, "%d%d%d%d%d", &prg[i].op, &prg[i].adr1, &prg[i].adr2, &prg[i].adr3, &prg[i].adr4);
						if (num_read < 4) {
							prg[i].adr3 = 0;
							prg[i].adr4 = 0;
						}

					}

				}

			}
		}
	}

	node = parent.child("best");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		index_best_gene = atoi(value_as_cstring);
	}

	node = parent.child("error");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		fitness = atof(value_as_cstring);
	}

	node = parent.child("binary_classification_threshold");
	if (node) {
		const char *value_as_cstring = node.child_value();
		best_class_threshold = atof(value_as_cstring);
	}
	else
		best_class_threshold = 0;

	node = parent.child("num_constants");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		num_constants = atoi(value_as_cstring);
	}
	node = parent.child("constants");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		int num_jumped_chars = 0;


		constants_double = new double[num_constants];
		for (int c = 0; c < num_constants; c++){
			sscanf(value_as_cstring + num_jumped_chars, "%lf", &constants_double[c]);
			long local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::mark(int k, bool* marked)
{
	if ((prg[k].op < 0) && !marked[k]){
		mark(prg[k].adr1, marked);

		switch (prg[k].op){
		case O_ADDITION:
			mark(prg[k].adr2, marked);
			break;
		case O_SUBTRACTION:
			mark(prg[k].adr2, marked);
			break;
		case O_MULTIPLICATION:
			mark(prg[k].adr2, marked);
			break;
		case O_DIVISION:
			mark(prg[k].adr2, marked);
			break;
		case O_POWER:
			mark(prg[k].adr2, marked);
			break;
		case O_MIN:
			mark(prg[k].adr2, marked);
			break;
		case O_MAX:
			mark(prg[k].adr2, marked);
			break;
		case O_IFLZ:
			mark(prg[k].adr2, marked);
			mark(prg[k].adr3, marked);
			break;
		case O_IFALBCD:
			mark(prg[k].adr2, marked);
			mark(prg[k].adr3, marked);
			mark(prg[k].adr4, marked);
			break;
		}
	}
	marked[k] = true;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::simplify(void)
{
	bool *marked = new bool[code_length];
	for (int i = 0; i < code_length; marked[i++] = false);
	mark(index_best_gene, marked);
	int *skipped = new int[index_best_gene + 1];
	if (!marked[0])
		skipped[0] = 1;
	else
		skipped[0] = 0;
	for (int i = 1; i <= index_best_gene; i++)
		if (!marked[i])
			skipped[i] = skipped[i - 1] + 1;
		else
			skipped[i] = skipped[i - 1];

	if (simplified_prg)
		delete[] simplified_prg;
	simplified_prg = new code3[index_best_gene + 1];

	num_utilized = 0;
	for (int i = 0; i <= index_best_gene; i++)
		if (marked[i]){
			simplified_prg[num_utilized] = prg[i];
			if (prg[i].op < 0){
				simplified_prg[num_utilized].adr1 -= skipped[prg[i].adr1];
				simplified_prg[num_utilized].adr2 -= skipped[prg[i].adr2];
				simplified_prg[num_utilized].adr3 -= skipped[prg[i].adr3];
				simplified_prg[num_utilized].adr4 -= skipped[prg[i].adr4];
			}
			num_utilized++;
		}

	delete[] skipped;
	delete[] marked;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::compare(t_mep_chromosome *other, bool minimize_operations_count)
{
	if (fitness > other->fitness)
		return 1;
	else if (fitness < other->fitness)
		return -1;
	else
		return 0;

}
//---------------------------------------------------------------------------
void t_mep_chromosome::generate_random(t_mep_parameters *parameters, t_mep_constants * mep_constants, int *actual_operators, int num_actual_operators, int *actual_variables, int num_actual_variables) // randomly initializes the individuals
{
	// I have to generate the constants for this individuals
	if (parameters->get_constants_probability() > 1E-6) {
		if (mep_constants->get_constants_type() == USER_DEFINED_CONSTANTS) {
			for (int c = 0; c < num_constants; c++)
				constants_double[c] = mep_constants->get_constants_double(c);
		}
		else {// automatic constants
			for (int c = 0; c < num_constants; c++)
				constants_double[c] = my_rand() / double(RAND_MAX) * (mep_constants->get_max_constants_interval_double() - mep_constants->get_min_constants_interval_double()) + mep_constants->get_min_constants_interval_double();
		}
	}

	double sum = parameters->get_variables_probability() + parameters->get_constants_probability();
	double p = my_rand() / (double)RAND_MAX * sum;

	if (p <= parameters->get_variables_probability())
		prg[0].op = actual_variables[my_rand() % num_actual_variables];
	else
		prg[0].op = num_total_variables + my_rand() % num_constants;

	for (int i = 1; i < parameters->get_code_length(); i++) {
		double p = my_rand() / (double)RAND_MAX;

		if (p <= parameters->get_operators_probability())
			prg[i].op = actual_operators[my_rand() % num_actual_operators];
		else
			if (p <= parameters->get_operators_probability() + parameters->get_variables_probability())
				prg[i].op = actual_variables[my_rand() % num_actual_variables];
			else
				prg[i].op = num_total_variables + my_rand() % num_constants;

		prg[i].adr1 = my_rand() % i;
		prg[i].adr2 = my_rand() % i;
		prg[i].adr3 = my_rand() % i;
		prg[i].adr4 = my_rand() % i;
	}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::mutation(t_mep_parameters *parameters, t_mep_constants * mep_constants, int *actual_operators, int num_actual_operators, int *actual_variables, int num_actual_variables) // mutate the individual
{

	// mutate each symbol with the same pm probability
	double p = my_rand() / (double)RAND_MAX;
	if (p < parameters->get_mutation_probability()) {
		double sum = parameters->get_variables_probability() + parameters->get_constants_probability();
		double q = my_rand() / (double)RAND_MAX * sum;

		if (q <= parameters->get_variables_probability())
			prg[0].op = actual_variables[my_rand() % num_actual_variables];
		else
			prg[0].op = num_total_variables + my_rand() % num_constants;
	}

	for (int i = 1; i < code_length; i++) {
		p = my_rand() / (double)RAND_MAX;      // mutate the operator
		if (p < parameters->get_mutation_probability()) {
			double q = my_rand() / (double)RAND_MAX;

			if (q <= parameters->get_operators_probability())
				prg[i].op = actual_operators[my_rand() % num_actual_operators];
			else
				if (q <= parameters->get_operators_probability() + parameters->get_variables_probability())
					prg[i].op = actual_variables[my_rand() % num_actual_variables];
				else
					prg[i].op = num_total_variables + my_rand() % num_constants;
		}

		p = my_rand() / (double)RAND_MAX;      // mutate the first address  (adr1)
		if (p < parameters->get_mutation_probability())
			prg[i].adr1 = my_rand() % i;

		p = my_rand() / (double)RAND_MAX;      // mutate the second address   (adr2)
		if (p < parameters->get_mutation_probability())
			prg[i].adr2 = my_rand() % i;
		p = my_rand() / (double)RAND_MAX;      // mutate the second address   (adr2)
		if (p < parameters->get_mutation_probability())
			prg[i].adr3 = my_rand() % i;
		p = my_rand() / (double)RAND_MAX;      // mutate the second address   (adr2)
		if (p < parameters->get_mutation_probability())
			prg[i].adr4 = my_rand() % i;
	}
	// lets see if I can evolve constants

	if (mep_constants->get_constants_can_evolve() && mep_constants->get_constants_type() == AUTOMATIC_CONSTANTS)
		for (int c = 0; c < num_constants; c++) {
			p = my_rand() / (double)RAND_MAX;      // mutate the operator
			double tmp_cst_d = my_rand() / double(RAND_MAX) * mep_constants->get_constants_mutation_max_deviation();

			if (p < parameters->get_mutation_probability()) {
				if (my_rand() % 2) {// coin
					if (constants_double[c] + tmp_cst_d <= mep_constants->get_max_constants_interval_double())
						constants_double[c] += tmp_cst_d;
				}
				else
					if (constants_double[c] - tmp_cst_d >= mep_constants->get_min_constants_interval_double())
						constants_double[c] -= tmp_cst_d;
				break;
			}
		}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::one_cut_point_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, t_mep_parameters *parameters, t_mep_constants * mep_constants)
{
	offspring1.code_length = code_length;
	offspring2.code_length = code_length;

	offspring1.num_total_variables = num_total_variables;
	offspring2.num_total_variables = num_total_variables;

	int pct, i;
	pct = 1 + my_rand() % (code_length - 2);
	for (i = 0; i < pct; i++) {
		offspring1.prg[i] = prg[i];
		offspring2.prg[i] = parent2.prg[i];
	}
	for (i = pct; i < code_length; i++) {
		offspring1.prg[i] = parent2.prg[i];
		offspring2.prg[i] = prg[i];
	}

	if (mep_constants->get_constants_can_evolve() && mep_constants->get_constants_type() == AUTOMATIC_CONSTANTS) {
		pct = 1 + my_rand() % (num_constants - 2);
		for (int c = 0; c < pct; c++) {
			offspring1.constants_double[c] = constants_double[c];
			offspring2.constants_double[c] = parent2.constants_double[c];
		}
		for (int c = pct; c < num_constants; c++) {
			offspring1.constants_double[c] = parent2.constants_double[c];
			offspring2.constants_double[c] = constants_double[c];
		}
	}

}
//---------------------------------------------------------------------------
void t_mep_chromosome::uniform_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, t_mep_parameters *parameters, t_mep_constants * mep_constants)
{
	offspring1.code_length = code_length;
	offspring2.code_length = code_length;

	offspring1.num_total_variables = num_total_variables;
	offspring2.num_total_variables = num_total_variables;

	for (int i = 0; i < code_length; i++)
		if (my_rand() % 2) {
			offspring1.prg[i] = prg[i];
			offspring2.prg[i] = parent2.prg[i];
		}
		else {
			offspring1.prg[i] = parent2.prg[i];
			offspring2.prg[i] = prg[i];
		}

		if (mep_constants->get_constants_can_evolve() && mep_constants->get_constants_type() == AUTOMATIC_CONSTANTS)
			for (int c = 0; c < num_constants; c++) {
				offspring1.constants_double[c] = constants_double[c];
				offspring2.constants_double[c] = parent2.constants_double[c];
			}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_regression_double_no_cache(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double* eval_vect, double *sum_of_errors_array)
{
	double **data = mep_dataset->get_data_matrix_double();

	fitness = 1E+308;
	index_best_gene = -1;

	for (int i = 0; i < code_length; i++)
		sum_of_errors_array[i] = 0;

	for (int k = 0; k < mep_dataset->get_num_rows(); k++) {   // read the t_mep_chromosome from top to down
		for (int i = 0; i < code_length; i++) {    // read the t_mep_chromosome from top to down

			errno = 0;
			bool is_error_case = false;
			switch (prg[i].op) {
			case  O_ADDITION:  // +
				eval_vect[i] = eval_vect[prg[i].adr1] + eval_vect[prg[i].adr2];
				break;
			case  O_SUBTRACTION:  // -
				eval_vect[i] = eval_vect[prg[i].adr1] - eval_vect[prg[i].adr2];
				break;
			case  O_MULTIPLICATION:  // *
				eval_vect[i] = eval_vect[prg[i].adr1] * eval_vect[prg[i].adr2];
				break;
			case  O_DIVISION:  //  /
				if (fabs(eval_vect[prg[i].adr2]) < DIVISION_PROTECT)
					is_error_case = true;
				else
					eval_vect[i] = eval_vect[prg[i].adr1] / eval_vect[prg[i].adr2];
				break;
			case O_POWER:
				eval_vect[i] = pow(eval_vect[prg[i].adr1], eval_vect[prg[i].adr2]);
				break;
			case O_SQRT:
				if (eval_vect[prg[i].adr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = sqrt(eval_vect[prg[i].adr1]);
				break;
			case O_EXP:
				eval_vect[i] = exp(eval_vect[prg[i].adr1]);

				break;
			case O_POW10:
				eval_vect[i] = pow(10, eval_vect[prg[i].adr1]);
				break;
			case O_LN:
				if (eval_vect[prg[i].adr1] <= 0)
					is_error_case = true;
				else                // an exception occured !!!
					eval_vect[i] = log(eval_vect[prg[i].adr1]);
				break;
			case O_LOG10:
				if (eval_vect[prg[i].adr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = log10(eval_vect[prg[i].adr1]);
				break;
			case O_lOG2:
				if (eval_vect[prg[i].adr1] <= 0)
					is_error_case = true;
				else
					eval_vect[i] = log2(eval_vect[prg[i].adr1]);
				break;
			case O_FLOOR:
				eval_vect[i] = floor(eval_vect[prg[i].adr1]);
				break;
			case O_CEIL:
				eval_vect[i] = ceil(eval_vect[prg[i].adr1]);
				break;
			case O_ABS:
				eval_vect[i] = fabs(eval_vect[prg[i].adr1]);
				break;
			case O_INV:
				eval_vect[i] = -eval_vect[prg[i].adr1];
				break;
			case O_X2:
				eval_vect[i] = eval_vect[prg[i].adr1] * eval_vect[prg[i].adr1];
				break;
			case O_MIN:
				eval_vect[i] = eval_vect[prg[i].adr1] < eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr1] : eval_vect[prg[i].adr2];
				break;
			case O_MAX:
				eval_vect[i] = eval_vect[prg[i].adr1] > eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr1] : eval_vect[prg[i].adr2];
				break;

			case O_SIN:
				eval_vect[i] = sin(eval_vect[prg[i].adr1]);
				break;
			case O_COS:
				eval_vect[i] = cos(eval_vect[prg[i].adr1]);
				break;
			case O_TAN:
				eval_vect[i] = tan(eval_vect[prg[i].adr1]);
				break;

			case O_ASIN:
				eval_vect[i] = asin(eval_vect[prg[i].adr1]);
				break;
			case O_ACOS:
				eval_vect[i] = acos(eval_vect[prg[i].adr1]);
				break;
			case O_ATAN:
				eval_vect[i] = atan(eval_vect[prg[i].adr1]);
				break;
			case O_IFLZ:
				eval_vect[i] = eval_vect[prg[i].adr1] < 0 ? eval_vect[prg[i].adr2] : eval_vect[prg[i].adr3];
				break;
			case O_IFALBCD:
				eval_vect[i] = eval_vect[prg[i].adr1] < eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr3] : eval_vect[prg[i].adr4];
				break;

			default:  // a variable
				if (prg[i].op < num_total_variables)
					eval_vect[i] = data[k][prg[i].op];
				else
					eval_vect[i] = constants_double[prg[i].op - num_total_variables];
				break;
			}
			if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
				delete[] eval_vect;
				// must redo everything again
			}
			else
				// everything ok - I must compute the difference between what I obtained and what I should obtain
				sum_of_errors_array[i] += fabs(eval_vect[i] - data[k][num_total_variables]);
		}
	}

	for (int i = 0; i < code_length; i++) {    // find the best gene
		if (fitness > sum_of_errors_array[i] / mep_dataset->get_num_rows()) {
			fitness = sum_of_errors_array[i] / mep_dataset->get_num_rows();
			index_best_gene = i;
		}
	}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_regression(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double** cached_eval_matrix, double * cached_sum_of_errors, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix)
{
	fitness_regression_double_cache_all_training_data(mep_dataset, random_subset_indexes, random_subset_selection_size, cached_eval_matrix, cached_sum_of_errors, num_actual_variables, actual_enabled_variables, eval_matrix);
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_binary_classification(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double **cached_eval_matrix, double * cached_sum_of_errors, double * cached_threashold, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double, s_value_class *tmp_value_class)
{
	fitness_binary_classification_double_cache_all_training_data(mep_dataset, random_subset_indexes, random_subset_selection_size, cached_eval_matrix, cached_sum_of_errors, cached_threashold, num_actual_variables, actual_enabled_variables, eval_matrix_double, tmp_value_class);
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multiclass_classification(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double **cached_eval_matrix, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double)
{
	fitness_multi_class_classification_double_cache_all_training_data(mep_dataset, random_subset_indexes, random_subset_selection_size, cached_eval_matrix, num_actual_variables, actual_enabled_variables, eval_matrix_double);
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_regression_double_cache_all_training_data(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double** cached_eval_matrix, double *cached_sum_of_errors, int num_actual_variables, int * actual_enabled_variables, double** eval_matrix_double)
{
	double **data = mep_dataset->get_data_matrix_double();
	int num_rows = mep_dataset->get_num_rows();

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	fitness = 1E+308;
	index_best_gene = -1;

	int *line_of_constants = NULL;
	double* cached_sum_of_errors_for_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		cached_sum_of_errors_for_constants = new double[num_constants];
		for (int i = 0; i < num_constants; i++) {
			line_of_constants[i] = -1;
			cached_sum_of_errors_for_constants[i] = -1;
		}
	}

	compute_eval_matrix_double(num_rows, cached_eval_matrix, num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double);

	int num_training_data = mep_dataset->get_num_rows();

	for (int i = 0; i < code_length; i++) {   // read the t_mep_chromosome from top to down
		double sum_of_errors;


		if (prg[i].op >= 0)// variable or constant
			if (prg[i].op < num_total_variables) // a variable, which is cached already
				sum_of_errors = cached_sum_of_errors[prg[i].op];
			else {// a constant
				sum_of_errors = 0;
				int constant_index = prg[i].op - num_total_variables;
				if (cached_sum_of_errors_for_constants[constant_index] < -0.5) {// this is not cached?
					double *eval = eval_matrix_double[line_of_constants[constant_index]];
					for (int k = 0; k < random_subset_selection_size; k++)
						sum_of_errors += fabs(eval[random_subset_indexes[k]] - data[random_subset_indexes[k]][num_total_variables]);
					sum_of_errors /= double(random_subset_selection_size);
				}
				else
					sum_of_errors = cached_sum_of_errors_for_constants[constant_index];
			}
		else {// operator
			double *eval = eval_matrix_double[i];
			sum_of_errors = 0;
			for (int k = 0; k < random_subset_selection_size; k++)
				sum_of_errors += fabs(eval[random_subset_indexes[k]] - data[random_subset_indexes[k]][num_total_variables]);
			sum_of_errors /= double(random_subset_selection_size);
		}
		if (fitness > sum_of_errors) {
			fitness = sum_of_errors;
			index_best_gene = i;
		}
	}

	if (line_of_constants)
		delete[] line_of_constants;

	if (cached_sum_of_errors_for_constants)
		delete[] cached_sum_of_errors_for_constants;
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
void t_mep_chromosome::fitness_binary_classification_double_cache_all_training_data(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double **cached_eval_matrix, double * cached_sum_of_errors, double * cached_threashold, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double, s_value_class *tmp_value_class)
{

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double **data = mep_dataset->get_data_matrix_double();
	int num_rows = mep_dataset->get_num_rows();

	fitness = DBL_MAX;
	index_best_gene = -1;

	int *line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_eval_matrix, num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double);

	double best_threshold;
	for (int i = 0; i < code_length; i++) {   // read the t_mep_chromosome from top to down
		double sum_of_errors;
		if (prg[i].op >= 0)// a vairable 
			if (prg[i].op < num_total_variables) { // a variable, which is cached already
				sum_of_errors = cached_sum_of_errors[prg[i].op];
				best_threshold = cached_threashold[prg[i].op];
			}
			else {// a constant
				if (mep_dataset->get_num_items_class_0() < num_rows - mep_dataset->get_num_items_class_0()) {// i must classify everything as 1
					sum_of_errors = mep_dataset->get_num_items_class_0();
					best_threshold = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][0] - 1;
				}
				else {// less of 1, I must classify everything as class 0
					sum_of_errors = num_rows - mep_dataset->get_num_items_class_0();
					best_threshold = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][0];
				}
				sum_of_errors /= (double)num_rows;
			}
		else {// an operator
			double *eval = eval_matrix_double[i];

			int num_0_incorrect = 0;
			for (int k = 0; k < random_subset_selection_size; k++) {
				tmp_value_class[k].value = eval[random_subset_indexes[k]];
				tmp_value_class[k].data_class = (int)data[random_subset_indexes[k]][num_total_variables];
				if (data[random_subset_indexes[k]][num_total_variables] < 0.5)
					num_0_incorrect++;
			}
			qsort((void*)tmp_value_class, random_subset_selection_size, sizeof(s_value_class), sort_function_value_class);

//			int num_0_incorrect = mep_dataset->get_num_items_class_0();
			int num_1_incorrect = 0;
			best_threshold = tmp_value_class[0].value - 1;// all are classified to class 1 in this case
			sum_of_errors = num_0_incorrect;

			for (int t = 0; t < random_subset_selection_size; t++) {
				int j = t + 1;
				while (j < random_subset_selection_size && fabs(tmp_value_class[t].value - tmp_value_class[j].value) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
					j++;

				// le verific pe toate intre i si j si le cataloghez ca apartinant la clasa 0
				for (int k = t; k < j; k++)
					if (tmp_value_class[k].data_class == 0)
						num_0_incorrect--;
					else
						if (tmp_value_class[k].data_class == 1) {
							//num_0_incorrect--;
							num_1_incorrect++;
						}
				if (num_0_incorrect + num_1_incorrect < sum_of_errors) {
					sum_of_errors = num_0_incorrect + num_1_incorrect;
					best_threshold = tmp_value_class[t].value;
				}
				t = j;
				t--;
			}
			sum_of_errors /= (double)random_subset_selection_size;
		}

		if (fitness > sum_of_errors) {
			fitness = sum_of_errors;
			index_best_gene = i;
			best_class_threshold = best_threshold;
		}
	}

	if (line_of_constants)
		delete[] line_of_constants;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_multi_class_classification_double_cache_all_training_data(t_mep_data *mep_dataset, int *random_subset_indexes, int random_subset_selection_size, double **cached_eval_matrix, int num_actual_variables, int * actual_enabled_variables, double **eval_matrix_double)
{


	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double **data = mep_dataset->get_data_matrix_double();
	int num_rows = mep_dataset->get_num_rows();
	int num_classes = mep_dataset->get_num_classes();

	int *line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_eval_matrix, num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double);

	int count_incorrect_classified = 0;
	for (int t = 0; t < random_subset_selection_size; t++) {
		// find the maximal value
		double max_val = eval_matrix_double[0][random_subset_indexes[t]];
		int max_index = 0;
		for (int i = 1; i < code_length; i++)
			if (prg[i].op >= 0)// variable or constant
				if (prg[i].op < num_total_variables) {
					if (max_val < cached_eval_matrix[prg[i].op][random_subset_indexes[t]]) {
						max_val = cached_eval_matrix[prg[i].op][random_subset_indexes[t]];
						max_index = i;
					}
				}
				else {
					if (max_val < eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][random_subset_indexes[t]]) {
						max_val = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][random_subset_indexes[t]];
						max_index = i;
					}
				}
			else// an operator
				if (max_val < eval_matrix_double[i][random_subset_indexes[t]]) {
					max_val = eval_matrix_double[i][random_subset_indexes[t]];
				max_index = i;
			  }
		if (max_index % num_classes != (int)data[random_subset_indexes[t]][num_total_variables])
			count_incorrect_classified++;
	}
	fitness = count_incorrect_classified / (double)random_subset_selection_size;

	if (line_of_constants)
		delete[] line_of_constants;
}
//---------------------------------------------------------------------------

bool t_mep_chromosome::compute_regression_error_on_double_data(double **data, int num_data, int output_col, double &error)
{
	error = 0;
	double actual_output_double[1];
	int num_valid = 0;
	int index_error_gene;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			error += fabs(data[k][output_col] - actual_output_double[0]);
			num_valid++;
		}
	}
	if (num_valid)
		error /= num_valid;
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_binary_classification_error_on_double_data(double **data, int num_data, int output_col, double &error)
{
	error = 0;
	double actual_output_double[1];

	//	int num_valid = 0;
	int index_error_gene;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			if (actual_output_double[0] <= best_class_threshold)
				error += data[k][output_col];
			else
				error += 1 - data[k][output_col];
		}
		else
			error++;
	}
	error /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multiclass_classification_error_on_double_data(double **data, int num_data, int output_col, int num_classes, double &error)
{
	error = 0;
//	double actual_output_double[1];

	int max_index = -1;
	int index_error_gene;
	for (int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, index_error_gene)) {
			if (fabs(max_index % num_classes - data[k][output_col]) > 1E-6)
				error++;
		}
		else
			error++;
	}
	error /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_regression_error_on_double_data_return_error(double **data, int num_data, int output_col, double &error, int &index_error_gene)
{
	error = 0;
	double actual_output_double[1];

	int num_valid = 0;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			error += fabs(data[k][output_col] - actual_output_double[0]);
			num_valid++;
		}
		else
			return false;
	}

	error /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_binary_classification_error_on_double_data_return_error(double **data, int num_data, int output_col, double &error, int &index_error_gene)
{
	error = 0;
	double actual_output_double[1];

	//	int num_valid = 0;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			if (actual_output_double[0] <= best_class_threshold)
				error += data[k][output_col];
			else
				error += 1 - data[k][output_col];
		}
		else
			return false;
	}
	error /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_multiclass_classification_error_on_double_data_return_error(double **data, int num_data, int output_col, int num_classes, double &error, int &index_error_gene)
{
	error = 0;
	//double actual_output_double[1];

	int max_index = -1;
	for (int k = 0; k < num_data; k++) {
		if (get_first_max_index(data[k], max_index, index_error_gene)) {
			if (fabs(max_index % num_classes - data[k][output_col]) > 1E-6)
				error++;
		}
		else
			return false;
	}
	error /= num_data;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::evaluate_double(double *inputs, double *outputs, int &index_error_gene)
{
	bool is_error_case;  // division by zero, other errors

	double *eval_vect = new double[index_best_gene + 1];

	for (int i = 0; i <= index_best_gene; i++)   // read the t_mep_chromosome from top to down
	{
		// and compute the fitness of each expression by dynamic programming
		errno = 0;
		is_error_case = false;
		switch (prg[i].op) {
		case  O_ADDITION:  // +
			eval_vect[i] = eval_vect[prg[i].adr1] + eval_vect[prg[i].adr2];
			break;
		case  O_SUBTRACTION:  // -
			eval_vect[i] = eval_vect[prg[i].adr1] - eval_vect[prg[i].adr2];
			break;
		case  O_MULTIPLICATION:  // *
			eval_vect[i] = eval_vect[prg[i].adr1] * eval_vect[prg[i].adr2];
			break;
		case  O_DIVISION:  //  /
			if (fabs(eval_vect[prg[i].adr2]) < DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = eval_vect[prg[i].adr1] / eval_vect[prg[i].adr2];
			break;
		case O_POWER:
			eval_vect[i] = pow(eval_vect[prg[i].adr1], eval_vect[prg[i].adr2]);
			break;
		case O_SQRT:
			if (eval_vect[prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = sqrt(eval_vect[prg[i].adr1]);
			break;
		case O_EXP:
			eval_vect[i] = exp(eval_vect[prg[i].adr1]);

			break;
		case O_POW10:
			eval_vect[i] = pow(10, eval_vect[prg[i].adr1]);
			break;
		case O_LN:
			if (eval_vect[prg[i].adr1] <= 0)
				is_error_case = true;
			else                // an exception occured !!!
				eval_vect[i] = log(eval_vect[prg[i].adr1]);
			break;
		case O_LOG10:
			if (eval_vect[prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log10(eval_vect[prg[i].adr1]);
			break;
		case O_lOG2:
			if (eval_vect[prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log2(eval_vect[prg[i].adr1]);
			break;
		case O_FLOOR:
			eval_vect[i] = floor(eval_vect[prg[i].adr1]);
			break;
		case O_CEIL:
			eval_vect[i] = ceil(eval_vect[prg[i].adr1]);
			break;
		case O_ABS:
			eval_vect[i] = fabs(eval_vect[prg[i].adr1]);
			break;
		case O_INV:
			eval_vect[i] = -eval_vect[prg[i].adr1];
			break;
		case O_X2:
			eval_vect[i] = eval_vect[prg[i].adr1] * eval_vect[prg[i].adr1];
			break;
		case O_MIN:
			eval_vect[i] = eval_vect[prg[i].adr1] < eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr1] : eval_vect[prg[i].adr2];
			break;
		case O_MAX:
			eval_vect[i] = eval_vect[prg[i].adr1] > eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr1] : eval_vect[prg[i].adr2];
			break;

		case O_SIN:
			eval_vect[i] = sin(eval_vect[prg[i].adr1]);
			break;
		case O_COS:
			eval_vect[i] = cos(eval_vect[prg[i].adr1]);
			break;
		case O_TAN:
			eval_vect[i] = tan(eval_vect[prg[i].adr1]);
			break;

		case O_ASIN:
			eval_vect[i] = asin(eval_vect[prg[i].adr1]);
			break;
		case O_ACOS:
			eval_vect[i] = acos(eval_vect[prg[i].adr1]);
			break;
		case O_ATAN:
			eval_vect[i] = atan(eval_vect[prg[i].adr1]);
			break;
		case O_IFLZ:
			eval_vect[i] = eval_vect[prg[i].adr1] < 0 ? eval_vect[prg[i].adr2] : eval_vect[prg[i].adr3];
			break;
		case O_IFALBCD:
			eval_vect[i] = eval_vect[prg[i].adr1] < eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr3] : eval_vect[prg[i].adr4];
			break;

		default:  // a variable
			if (prg[i].op < num_total_variables)
				eval_vect[i] = inputs[prg[i].op];
			else
				eval_vect[i] = constants_double[prg[i].op - num_total_variables];
			break;
		}
		if (errno || is_error_case || isnan(eval_vect[i]) || isinf(eval_vect[i])) {
			delete[] eval_vect;
            index_error_gene = i;
			return false;
		}
	}
	outputs[0] = eval_vect[index_best_gene];
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
			eval_vect[i] = eval_vect[prg[i].adr1] + eval_vect[prg[i].adr2];
			break;
		case  O_SUBTRACTION:  // -
			eval_vect[i] = eval_vect[prg[i].adr1] - eval_vect[prg[i].adr2];
			break;
		case  O_MULTIPLICATION:  // *
			eval_vect[i] = eval_vect[prg[i].adr1] * eval_vect[prg[i].adr2];
			break;
		case  O_DIVISION:  //  /
			if (fabs(eval_vect[prg[i].adr2]) < DIVISION_PROTECT)
				is_error_case = true;
			else
				eval_vect[i] = eval_vect[prg[i].adr1] / eval_vect[prg[i].adr2];
			break;
		case O_POWER:
			eval_vect[i] = pow(eval_vect[prg[i].adr1], eval_vect[prg[i].adr2]);
			break;
		case O_SQRT:
			if (eval_vect[prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = sqrt(eval_vect[prg[i].adr1]);
			break;
		case O_EXP:
			eval_vect[i] = exp(eval_vect[prg[i].adr1]);

			break;
		case O_POW10:
			eval_vect[i] = pow(10, eval_vect[prg[i].adr1]);
			break;
		case O_LN:
			if (eval_vect[prg[i].adr1] <= 0)
				is_error_case = true;
			else                // an exception occured !!!
				eval_vect[i] = log(eval_vect[prg[i].adr1]);
			break;
		case O_LOG10:
			if (eval_vect[prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log10(eval_vect[prg[i].adr1]);
			break;
		case O_lOG2:
			if (eval_vect[prg[i].adr1] <= 0)
				is_error_case = true;
			else
				eval_vect[i] = log2(eval_vect[prg[i].adr1]);
			break;
		case O_FLOOR:
			eval_vect[i] = floor(eval_vect[prg[i].adr1]);
			break;
		case O_CEIL:
			eval_vect[i] = ceil(eval_vect[prg[i].adr1]);
			break;
		case O_ABS:
			eval_vect[i] = fabs(eval_vect[prg[i].adr1]);
			break;
		case O_INV:
			eval_vect[i] = -eval_vect[prg[i].adr1];
			break;
		case O_X2:
			eval_vect[i] = eval_vect[prg[i].adr1] * eval_vect[prg[i].adr1];
			break;
		case O_MIN:
			eval_vect[i] = eval_vect[prg[i].adr1] < eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr1] : eval_vect[prg[i].adr2];
			break;
		case O_MAX:
			eval_vect[i] = eval_vect[prg[i].adr1] > eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr1] : eval_vect[prg[i].adr2];
			break;

		case O_SIN:
			eval_vect[i] = sin(eval_vect[prg[i].adr1]);
			break;
		case O_COS:
			eval_vect[i] = cos(eval_vect[prg[i].adr1]);
			break;
		case O_TAN:
			eval_vect[i] = tan(eval_vect[prg[i].adr1]);
			break;

		case O_ASIN:
			eval_vect[i] = asin(eval_vect[prg[i].adr1]);
			break;
		case O_ACOS:
			eval_vect[i] = acos(eval_vect[prg[i].adr1]);
			break;
		case O_ATAN:
			eval_vect[i] = atan(eval_vect[prg[i].adr1]);
			break;
		case O_IFLZ:
			eval_vect[i] = eval_vect[prg[i].adr1] < 0 ? eval_vect[prg[i].adr2] : eval_vect[prg[i].adr3];
			break;
		case O_IFALBCD:
			eval_vect[i] = eval_vect[prg[i].adr1] < eval_vect[prg[i].adr2] ? eval_vect[prg[i].adr3] : eval_vect[prg[i].adr4];
			break;

		default:  // a variable
			if (prg[i].op < num_total_variables)
				eval_vect[i] = inputs[prg[i].op];
			else
				eval_vect[i] = constants_double[prg[i].op - num_total_variables];
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
void t_mep_chromosome::compute_eval_matrix_double(int num_training_data, double **cached_eval_matrix_double, int num_actual_variables, int * actual_enabled_variables, int *line_of_constants, double ** eval_double)
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
			if (prg[prg[i].adr1].op >= 0)
				if (prg[prg[i].adr1].op < num_total_variables)
					arg1 = cached_eval_matrix_double[prg[prg[i].adr1].op];
				else
					arg1 = eval_double[line_of_constants[prg[prg[i].adr1].op - num_total_variables]];
			else
				arg1 = eval_double[prg[i].adr1];

			if (prg[prg[i].adr2].op >= 0)
				if (prg[prg[i].adr2].op < num_total_variables)
					arg2 = cached_eval_matrix_double[prg[prg[i].adr2].op];
				else
					arg2 = eval_double[line_of_constants[prg[prg[i].adr2].op - num_total_variables]];
			else
				arg2 = eval_double[prg[i].adr2];

			if (prg[prg[i].adr3].op >= 0)
				if (prg[prg[i].adr3].op < num_total_variables)
					arg3 = cached_eval_matrix_double[prg[prg[i].adr3].op];
				else
					arg3 = eval_double[line_of_constants[prg[prg[i].adr3].op - num_total_variables]];
			else
				arg3 = eval_double[prg[i].adr3];

			if (prg[prg[i].adr4].op >= 0)
				if (prg[prg[i].adr4].op < num_total_variables)
					arg4 = cached_eval_matrix_double[prg[prg[i].adr4].op];
				else
					arg4 = eval_double[line_of_constants[prg[prg[i].adr4].op - num_total_variables]];
			else
				arg4 = eval_double[prg[i].adr4];
		}
		else {
			arg1 = arg2 = arg3 = arg4 = NULL; // just to silence some compiler warnings
		}

		switch (prg[i].op) {
		case  O_ADDITION:  // +
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] + arg2[k];
			break;
		case  O_SUBTRACTION:  // -
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] - arg2[k];
			break;
		case  O_MULTIPLICATION:  // *
			for (int k = 0; k < num_training_data; k++)
				eval[k] = arg1[k] * arg2[k];
			break;
		case  O_DIVISION:  //  /
			for (int k = 0; k < num_training_data; k++)
				if (fabs(arg2[k]) < DIVISION_PROTECT) {
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal, I can also put a constant here!!!!!!!!!!!!!!!
					break;
				}
				else
					eval[k] = arg1[k] / arg2[k];
			break;
		case O_POWER:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = pow(arg1[k], arg2[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_SQRT:
			for (int k = 0; k < num_training_data; k++) {
				if (arg1[k] <= 0) {
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
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
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
			}

			break;
		case O_POW10:
			for (int k = 0; k < num_training_data; k++) {
				eval[k] = pow(10, arg1[k]);
				if (errno || isnan(eval[k]) || isinf(eval[k])) {
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
			}
			break;
		case O_LN:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = log(arg1[k]);

			break;
		case O_LOG10:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = log10(arg1[k]);

			break;
		case O_lOG2:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] <= 0) {
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
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
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
					break;
				}
				else
					eval[k] = asin(arg1[k]);

			break;
		case O_ACOS:
			for (int k = 0; k < num_training_data; k++)
				if (arg1[k] < -1 || arg1[k] > 1) {
					prg[i].op = actual_enabled_variables[my_rand() % num_actual_variables];   // the gene is mutated into a terminal
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

		default:  // a constant
			if (prg[i].op >= num_total_variables)
				if (line_of_constants[prg[i].op - num_total_variables] == -1) {
					line_of_constants[prg[i].op - num_total_variables] = i;
					int constant_index = prg[i].op - num_total_variables;
					for (int k = 0; k < num_training_data; k++)
						eval[k] = constants_double[constant_index];
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
int t_mep_chromosome::get_index_best_gene(void)
{
	return index_best_gene;
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
