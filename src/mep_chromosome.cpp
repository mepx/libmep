
#include "mep_chromosome.h"
#include "mep_operators.h"
#include "mep_data.h"
#include "mep_rands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------
t_mep_chromosome::t_mep_chromosome()
{
	prg = NULL;
	s_prg = NULL;
	best = -1;
	fit = 0;

	num_constants = 0;
	constants_double = NULL;
	best_class_threshold = 0;
}
//---------------------------------------------------------------------------
void t_mep_chromosome::allocate_memory(long code_length, int num_vars, bool use_constants, t_mep_constants *  constants)
{
	this->code_length = code_length;
	prg = new code3[code_length];
	s_prg = NULL;
	this->num_total_variables = num_vars;

	if (use_constants) {
		if (constants->constants_type == USER_DEFINED_CONSTANTS) {
			num_constants = constants->num_user_defined_constants;
			constants_double = new double[num_constants];
		}
		else {// automatic constants
			num_constants = constants->num_automatic_constants;
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
	if (s_prg){
		delete[] s_prg;
		s_prg = NULL;
	}
	best = -1;
	fit = 0;

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
		//		if (!s_prg)// I do this for stats only
		//			s_prg = new code3[code_length];        // a string of genes
		//!!!!!!!!!!!!!!!!!!!! this is not efficient. I must delete memory only when it is needed, at the end of all runs.!!!!!!!!!!!!!!!!!!!!!!!
		for (int i = 0; i < code_length; i++){
			prg[i] = source.prg[i];
			//			s_prg[i] = source.s_prg[i];
		}
		fit = source.fit;        // the fitness
		best = source.best;          // the index of the best expression in t_mep_chromosome

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
		sprintf(tmp_s, "prg[%d] < prg[%d]?prg[%d] : prg[%d];", adr1, adr2, adr1, adr2);
		break;
	case O_MAX:
		sprintf(tmp_s, "prg[%d] > prg[%d]?prg[%d] : prg[%d];", adr1, adr2, adr1, adr2);
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
		sprintf(tmp_s, "prg[%d] < 0?prg[%d] : prg[%d];", adr1, adr2, adr3);
		break;
	case O_IFALBCD:
		sprintf(tmp_s, "prg[%d] < prg[%d]?prg[%d] : prg[%d];", adr1, adr2, adr3, adr4);
		break;
	}
}
//---------------------------------------------------------------------------------
char * t_mep_chromosome::to_C_double(bool simplified, double *data, int problem_type)
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

			if (s_prg[i].op < 0){
				print_instruction(s_prg[i].op, s_prg[i].adr1, s_prg[i].adr2, s_prg[i].adr3, s_prg[i].adr4, tmp_s);
				strcat(prog, tmp_s);
				strcat(prog, "\n");
			}
			else
			{ // a variable
				if (s_prg[i].op < num_total_variables){
					sprintf(tmp_s, "x[%d];", s_prg[i].op);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
				else{
					sprintf(tmp_s, "constants[%d];", s_prg[i].op - num_total_variables);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
			}
		}

		strcat(prog, "\n");
		if (problem_type == PROBLEM_REGRESSION)
			sprintf(tmp_s, "  outputs[0] = prg[%d];", num_utilized - 1);
		else
			sprintf(tmp_s, "  if (prg[%d] <= %lf)\n    outputs[0] = 0;\n  else\n    outputs[0] = 1;", num_utilized - 1, best_class_threshold);

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
		if (problem_type == PROBLEM_REGRESSION)
			sprintf(tmp_s, "  outputs[0] = prg[%d];", best);
		else{
			//            wxLogDebug(wxString() << best << " " << best_class_threshold);
			sprintf(tmp_s, "  if (prg[%d] <= %lg)\n    outputs[0] = 0;\n  else\n    outputs[0] = 1;", best, best_class_threshold);
			//          wxLogDebug(wxString(tmp_s));
		}

		strcat(prog, tmp_s);
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
	sprintf(tmp_str, "%d", best);
	data.set_value(tmp_str);

	node = parent.append_child("error");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", fit);
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
		best = atoi(value_as_cstring);
	}

	node = parent.child("error");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		fit = atof(value_as_cstring);
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
	mark(best, marked);
	int *skipped = new int[best + 1];
	if (!marked[0])
		skipped[0] = 1;
	else
		skipped[0] = 0;
	for (int i = 1; i <= best; i++)
		if (!marked[i])
			skipped[i] = skipped[i - 1] + 1;
		else
			skipped[i] = skipped[i - 1];

	if (s_prg)
		delete[] s_prg;
	s_prg = new code3[best + 1];

	num_utilized = 0;
	for (int i = 0; i <= best; i++)
		if (marked[i]){
			s_prg[num_utilized] = prg[i];
			if (prg[i].op < 0){
				s_prg[num_utilized].adr1 -= skipped[prg[i].adr1];
				s_prg[num_utilized].adr2 -= skipped[prg[i].adr2];
				s_prg[num_utilized].adr3 -= skipped[prg[i].adr3];
				s_prg[num_utilized].adr4 -= skipped[prg[i].adr4];
			}
			num_utilized++;
		}

	delete[] skipped;
	delete[] marked;
}
//---------------------------------------------------------------------------
int t_mep_chromosome::compare(t_mep_chromosome *other, bool minimize_operations_count)
{
	if (fit > other->fit)
		return 1;
	else if (fit < other->fit)
		return -1;
	else
		return 0;

}
//---------------------------------------------------------------------------
void t_mep_chromosome::generate_random(t_parameters *parameters, int *actual_operators, int num_actual_operators, int *actual_variables, int num_actual_variables) // randomly initializes the individuals
{
	// I have to generate the constants for this individuals
	if (parameters->constants_probability > 1E-6) {
		if (parameters->constants.constants_type == USER_DEFINED_CONSTANTS) {
			for (int c = 0; c < num_constants; c++)
				constants_double[c] = parameters->constants.constants_double[c];
		}
		else {// automatic constants
			for (int c = 0; c < num_constants; c++)
				constants_double[c] = my_rand() / double(RAND_MAX) * (parameters->constants.max_constants_interval_double - parameters->constants.min_constants_interval_double) + parameters->constants.min_constants_interval_double;
		}
	}

	double sum = parameters->variables_probability + parameters->constants_probability;
	double p = my_rand() / (double)RAND_MAX * sum;

	if (p <= parameters->variables_probability)
		prg[0].op = actual_variables[my_rand() % num_actual_variables];
	else
		prg[0].op = num_total_variables + my_rand() % num_constants;

	for (int i = 1; i < parameters->code_length; i++) {
		double p = my_rand() / (double)RAND_MAX;

		if (p <= parameters->operators_probability)
			prg[i].op = actual_operators[my_rand() % num_actual_operators];
		else
			if (p <= parameters->operators_probability + parameters->variables_probability)
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
void t_mep_chromosome::mutation(t_parameters *parameters, int *actual_operators, int num_actual_operators, int *actual_variables, int num_actual_variables) // mutate the individual
{

	// mutate each symbol with the same pm probability
	double p = my_rand() / (double)RAND_MAX;
	if (p < parameters->mutation_probability) {
		double sum = parameters->variables_probability + parameters->constants_probability;
		double q = my_rand() / (double)RAND_MAX * sum;

		if (q <= parameters->variables_probability)
			prg[0].op = actual_variables[my_rand() % num_actual_variables];
		else
			prg[0].op = num_total_variables + my_rand() % num_constants;
	}

	for (int i = 1; i < code_length; i++) {
		p = my_rand() / (double)RAND_MAX;      // mutate the operator
		if (p < parameters->mutation_probability) {
			double q = my_rand() / (double)RAND_MAX;

			if (q <= parameters->operators_probability)
				prg[i].op = actual_operators[my_rand() % num_actual_operators];
			else
				if (q <= parameters->operators_probability + parameters->variables_probability)
					prg[i].op = actual_variables[my_rand() % num_actual_variables];
				else
					prg[i].op = num_total_variables + my_rand() % num_constants;
		}

		p = my_rand() / (double)RAND_MAX;      // mutate the first address  (adr1)
		if (p < parameters->mutation_probability)
			prg[i].adr1 = my_rand() % i;

		p = my_rand() / (double)RAND_MAX;      // mutate the second address   (adr2)
		if (p < parameters->mutation_probability)
			prg[i].adr2 = my_rand() % i;
		p = my_rand() / (double)RAND_MAX;      // mutate the second address   (adr2)
		if (p < parameters->mutation_probability)
			prg[i].adr3 = my_rand() % i;
		p = my_rand() / (double)RAND_MAX;      // mutate the second address   (adr2)
		if (p < parameters->mutation_probability)
			prg[i].adr4 = my_rand() % i;
	}
	// lets see if I can evolve constants

	if (parameters->constants.constants_can_evolve && parameters->constants.constants_type == AUTOMATIC_CONSTANTS)
		for (int c = 0; c < num_constants; c++) {
			p = my_rand() / (double)RAND_MAX;      // mutate the operator
			double tmp_cst_d = my_rand() / double(RAND_MAX) * parameters->constants.constants_mutation_max_deviation;

			if (p < parameters->mutation_probability) {
				if (my_rand() % 2) {// coin
					if (constants_double[c] + tmp_cst_d <= parameters->constants.max_constants_interval_double)
						constants_double[c] += tmp_cst_d;
				}
				else
					if (constants_double[c] - tmp_cst_d >= parameters->constants.min_constants_interval_double)
						constants_double[c] -= tmp_cst_d;
				break;
			}
		}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::one_cut_point_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, t_parameters *parameters)
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

	if (parameters->constants.constants_can_evolve && parameters->constants.constants_type == AUTOMATIC_CONSTANTS) {
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
void t_mep_chromosome::uniform_crossover(const t_mep_chromosome &parent2, t_mep_chromosome &offspring1, t_mep_chromosome &offspring2, t_parameters *parameters)
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

		if (parameters->constants.constants_can_evolve && parameters->constants.constants_type == AUTOMATIC_CONSTANTS)
			for (int c = 0; c < num_constants; c++) {
				offspring1.constants_double[c] = constants_double[c];
				offspring2.constants_double[c] = parent2.constants_double[c];
			}
}
//---------------------------------------------------------------------------