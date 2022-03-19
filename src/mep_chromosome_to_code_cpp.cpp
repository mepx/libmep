// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
//---------------------------------------------------------------------------------
#include "mep_chromosome.h"
#include "mep_functions.h"
//---------------------------------------------------------------------------------
void print_instruction_to_C(int op, unsigned int adr1, unsigned int adr2, unsigned int adr3, unsigned int adr4, char* tmp_s)
{
	switch (op) {
	case O_ADDITION:
		sprintf(tmp_s, "prg[%u] + prg[%u];", adr1, adr2);
		break;
	case O_SUBTRACTION:
		sprintf(tmp_s, "prg[%u] - prg[%u];", adr1, adr2);
		break;
	case O_MULTIPLICATION:
		sprintf(tmp_s, "prg[%u] * prg[%u];", adr1, adr2);
		break;
	case O_DIVISION:
		sprintf(tmp_s, "prg[%u] / prg[%u];", adr1, adr2);
		break;
	case O_POWER:
		sprintf(tmp_s, "pow(prg[%u], prg[%u]);", adr1, adr2);
		break;
	case O_SQRT:
		sprintf(tmp_s, "sqrt(prg[%u]);", adr1);
		break;
	case O_EXP:
		sprintf(tmp_s, "exp(prg[%u]);", adr1);
		break;
	case O_POW10:
		sprintf(tmp_s, "pow(10, prg[%u]);", adr1);
		break;
	case O_LN:
		sprintf(tmp_s, "log(prg[%u]);", adr1);
		break;
	case O_LOG10:
		sprintf(tmp_s, "log10(prg[%u]);", adr1);
		break;
	case O_lOG2:
		sprintf(tmp_s, "log2(prg[%u]);", adr1);
		break;
	case O_FLOOR:
		sprintf(tmp_s, "floor(prg[%u]);", adr1);
		break;
	case O_CEIL:
		sprintf(tmp_s, "ceil(prg[%u]);", adr1);
		break;
	case O_ABS:
		sprintf(tmp_s, "fabs(prg[%u]);", adr1);
		break;
	case O_INV:
		sprintf(tmp_s, "1/prg[%u];", adr1);
		break;
	case O_NEG:
		sprintf(tmp_s, "-prg[%u];", adr1);
		break;
	case O_X2:
		sprintf(tmp_s, "prg[%u] * prg[%u];", adr1, adr1);
		break;
	case O_MIN:
		sprintf(tmp_s, "prg[%u] < prg[%u]?prg[%u] : prg[%u]; // min", adr1, adr2, adr1, adr2);
		break;
	case O_MAX:
		sprintf(tmp_s, "prg[%u] > prg[%u]?prg[%u] : prg[%u]; // max", adr1, adr2, adr1, adr2);
		break;
	case O_SIN:
		sprintf(tmp_s, "sin(prg[%u]);", adr1);
		break;
	case O_COS:
		sprintf(tmp_s, "cos(prg[%u]);", adr1);
		break;
	case O_TAN:
		sprintf(tmp_s, "tan(prg[%u]);", adr1);
		break;
	case O_ASIN:
		sprintf(tmp_s, "asin(prg[%u]);", adr1);
		break;
	case O_ACOS:
		sprintf(tmp_s, "acos(prg[%u]);", adr1);
		break;
	case O_ATAN:
		sprintf(tmp_s, "atan(prg[%u]);", adr1);
		break;
	case O_IFLZ:
		sprintf(tmp_s, "prg[%u] < 0?prg[%u] : prg[%u]; // ifalzbc", adr1, adr2, adr3);
		break;
	case O_IFALBCD:
		sprintf(tmp_s, "prg[%u] < prg[%u]?prg[%u] : prg[%u]; // ifalbcd", adr1, adr2, adr3, adr4);
		break;
	case O_IF_A_OR_B_CD:
		sprintf(tmp_s, "prg[%u] < 0 || prg[%u] < 0 ? prg[%u] : prg[%u]; // if_a_or_b_cd", adr1, adr2, adr3, adr4);
		break;
	case O_IF_A_XOR_B_CD:
		sprintf(tmp_s, "prg[%u] < 0 != prg[%u] < 0 ? prg[%u] : prg[%u]; // if_a_xor_b_cd", adr1, adr2, adr3, adr4);
		break;
	case O_FMOD:
		sprintf(tmp_s, "fmod(prg[%u], prg[%u]); // fmod", adr1, adr2);
		break;

	case O_NUM_INPUTS:
		sprintf(tmp_s, "%d; // num inputs", 0);
		break;

	case O_INPUTS_AVERAGE:
		sprintf(tmp_s, "%lf; // inputs average", 0.0);
		break;
	}
}
//---------------------------------------------------------------------------------
char* t_mep_chromosome::to_C_double(bool simplified, double* data,
	unsigned int problem_type, unsigned int error_measure, const char* libmep_version)
{
	setlocale(LC_NUMERIC, "C");

	char* prog = new char[(code_length + num_constants + num_total_variables) * 100 + 2000];
	char tmp_s[100];
	prog[0] = 0;
	sprintf(tmp_s, "//Generated by libmep version %s\n", libmep_version);
	strcat(prog, "#include <math.h>\n");
	strcat(prog, "#include <stdio.h>\n");
	strcat(prog, "\n");

	strcat(prog, "void mepx(double *x /*inputs*/, double *outputs)");
	strcat(prog, "\n");
	strcat(prog, "{");
	strcat(prog, "\n");

	if (simplified) {
		strcat(prog, "  double prg[");
		sprintf(tmp_s, "%u", num_utilized_genes);
		strcat(prog, tmp_s);
		strcat(prog, "];");
		strcat(prog, "\n");

		for (unsigned int i = 0; i < num_utilized_genes; i++) {
			sprintf(tmp_s, "  prg[%u] = ", i);
			strcat(prog, tmp_s);

			if (simplified_prg[i].op < 0) {
				print_instruction_to_C(simplified_prg[i].op, simplified_prg[i].addr1, simplified_prg[i].addr2, simplified_prg[i].addr3, simplified_prg[i].addr4, tmp_s);
				strcat(prog, tmp_s);
				strcat(prog, "\n");
			}
			else { // a variable
				if (simplified_prg[i].op < (int)num_total_variables) {
					sprintf(tmp_s, "x[%u];", (unsigned int)simplified_prg[i].op);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
				else {
					sprintf(tmp_s, "%lg;", real_constants[simplified_prg[i].op - num_total_variables]);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
			}
		}

		strcat(prog, "\n");
		switch (problem_type) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_TIME_SERIE:
			sprintf(tmp_s, "  outputs[0] = prg[%u];", num_utilized_genes - 1);
			strcat(prog, tmp_s);
			break;
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			sprintf(tmp_s, "  if (prg[%u] <= %lf)\n    outputs[0] = 0;\n  else\n    outputs[0] = 1;", num_utilized_genes - 1, best_class_threshold);
			strcat(prog, tmp_s);
			break;
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			if (error_measure == MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR) {
				sprintf(tmp_s, "  double centers[%u];\n", num_classes);
				strcat(prog, tmp_s);
				for (unsigned int c = 0; c < num_classes; c++) {
					sprintf(tmp_s, "  centers[%u] = %lf;\n", c, centers[c]);
					strcat(prog, tmp_s);
				}
				sprintf(tmp_s, "  double min_dist = fabs(prg[%u] - centers[0]);\n", num_utilized_genes - 1);
				strcat(prog, tmp_s);

				strcat(prog,   "  unsigned int closest_class_index = 0;\n");
				sprintf(tmp_s, "  for (unsigned int c = 1; c < %u; c++)\n", num_classes);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "    if (min_dist > fabs(prg[%u] - centers[c])) {\n", num_utilized_genes - 1);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "      min_dist = fabs(prg[%u] - centers[c]);\n", num_utilized_genes - 1);
				strcat(prog, tmp_s);

				strcat(prog,   "      closest_class_index = c;\n");
				strcat(prog,   "    }\n");
				strcat(prog,   "  outputs[0] = closest_class_index;\n");
			}
			else {
				// should never be here
			}
			break;
		}

		
	}
	else {// not simplified
		strcat(prog, "  double prg[");
		sprintf(tmp_s, "%u", code_length);
		strcat(prog, tmp_s);
		strcat(prog, "];");
		strcat(prog, "\n");

		for (unsigned int i = 0; i < code_length; i++) {
			sprintf(tmp_s, "  prg[%u] = ", i);
			strcat(prog, tmp_s);

			if (prg[i].op < 0) {
				print_instruction_to_C(prg[i].op, prg[i].addr1, prg[i].addr2, prg[i].addr3, prg[i].addr4, tmp_s);
				strcat(prog, tmp_s);
				strcat(prog, "\n");
			}
			else { // a variable
				if (prg[i].op < (int)num_total_variables) {
					sprintf(tmp_s, "x[%u];", (unsigned int)prg[i].op);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
				else {
					sprintf(tmp_s, "%lg;", real_constants[prg[i].op - num_total_variables]);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
			}
		}

		strcat(prog, "\n");
		switch (problem_type) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_TIME_SERIE:
			sprintf(tmp_s, "  outputs[0] = prg[%u];", index_best_genes[0]);
			strcat(prog, tmp_s);
			break;

		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			sprintf(tmp_s, "  if (prg[%u] <= %lg)\n    outputs[0] = 0;\n  else\n    outputs[0] = 1;", index_best_genes[0], best_class_threshold);
			strcat(prog, tmp_s);
			break;
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			switch (error_measure) {
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
			case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
				sprintf(tmp_s, "// find maximal value and divide index by num classes\n");
				strcat(prog, tmp_s);

				strcat(prog, "  double max_value = prg[0];\n");
				strcat(prog, "  int index_max_value = 0;\n");
				sprintf(tmp_s, "  for (unsigned int i = 1; i < %u; i++)\n", code_length);
				strcat(prog, tmp_s);

				strcat(prog, "    if (max_value < prg[i]){\n      max_value = prg[i];\n      index_max_value = i;\n    }\n");

				sprintf(tmp_s, "  outputs[0] = index_max_value %% %u;\n", num_classes);
				strcat(prog, tmp_s);
				break;
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:

				sprintf(tmp_s, "// index of genes holding the output for each class\n");
				strcat(prog, tmp_s);

				sprintf(tmp_s, "  int index_best_genes[%u];\n", num_classes);
				strcat(prog, tmp_s);
				for (unsigned int c = 0; c < num_classes; c++) {
					sprintf(tmp_s, "  index_best_genes[%u] = %u;\n", c, index_best_genes[c]);
					strcat(prog, tmp_s);
				}

				sprintf(tmp_s, "// find maximal value\n");
				strcat(prog, tmp_s);

				strcat(prog, "  double max_value = prg[0];\n");
				strcat(prog, "  int index_max_value = 0;\n");
				sprintf(tmp_s, "  for (unsigned int i = 1; i < %u; i++)\n", code_length);
				strcat(prog, tmp_s);

				strcat(prog, "    if (max_value < prg[i]){\n      max_value = prg[i];\n      index_max_value = i;\n    }\n");

				strcat(prog, "  //find the class\n");
				sprintf(tmp_s, "  unsigned int class_index = %u;\n", num_classes);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "  for (unsigned int c = 0; c < %u; c++){\n", num_classes);

				strcat(prog, tmp_s);

				strcat(prog, "    if (index_best_genes[c] == index_max_value) {\n      class_index = c;\n      break;\n    }\n");
				strcat(prog, "  }\n");

				sprintf(tmp_s, "  if (class_index == %u){\n", num_classes);
				strcat(prog, tmp_s);

				strcat(prog, "    // find the closest max\n");
				strcat(prog, "    double min_dist = fabs(max_value - prg[index_best_genes[0]]);\n");
				strcat(prog, "    class_index = 0;\n");
				sprintf(tmp_s, "    for (unsigned c = 1; c < %u; c++){\n", num_classes);
				strcat(prog, tmp_s);

				strcat(prog, "      if (min_dist > fabs(max_value - prg[index_best_genes[c]])){\n");
				strcat(prog, "        min_dist = fabs(max_value - prg[index_best_genes[c]]);\n");

				strcat(prog, "        class_index = c;\n");
				strcat(prog, "      }\n");
				strcat(prog, "    }\n");

				strcat(prog, "  }\n");

				sprintf(tmp_s, "  outputs[0] = class_index;\n");
				strcat(prog, tmp_s);
				break;

			case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
				sprintf(tmp_s, "  double centers[%u];\n", num_classes);
				strcat(prog, tmp_s);
				for (unsigned int c = 0; c < num_classes; c++) {
					sprintf(tmp_s, "  centers[%u] = %lf;\n", c, centers[c]);
					strcat(prog, tmp_s);
				}
				sprintf(tmp_s, "  double min_dist = fabs(prg[%u] - centers[0]);\n", index_best_genes[0]);
				strcat(prog, tmp_s);

				strcat(prog, "  unsigned int closest_class_index = 0;\n");
				sprintf(tmp_s, "  for (unsigned int c = 1; c < %u; c++)\n", num_classes);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "    if (min_dist > fabs(prg[%u] - centers[c])) {\n", index_best_genes[0]);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "      min_dist = fabs(prg[%u] - centers[c]);\n", index_best_genes[0]);
				strcat(prog, tmp_s);

				strcat(prog, "      closest_class_index = c;\n");
				strcat(prog, "    }\n");
				strcat(prog, "  outputs[0] = closest_class_index;\n");
				break;
			}// end switch error_measure
			break;
		} // end switch (problem_type)
	}// end else not simplified
	strcat(prog, "\n");
	strcat(prog, "}\n");

	strcat(prog, "\n");
	strcat(prog, "int main(void)\n");
	strcat(prog, "{\n");
	strcat(prog, "\n");
	strcat(prog, "//example of utilization ...\n");
	strcat(prog, "\n");
	sprintf(tmp_s, "  double x[%u];\n", num_total_variables);
	strcat(prog, tmp_s);
	for (unsigned int i = 0; i < num_total_variables; i++) {
		sprintf(tmp_s, "  x[%u] = %lf;\n", i, data[i]);
		strcat(prog, tmp_s);
	}

	strcat(prog, "\n");
	strcat(prog, "  double outputs[1];\n");
	strcat(prog, "\n");
	strcat(prog, "  mepx(x, outputs);\n");

	switch (problem_type) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_TIME_SERIE:
			strcat(prog, "  printf(\"%lf\", outputs[0]);\n");
			break;
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			strcat(prog, "  printf(\"class index = %d\", (unsigned int)outputs[0]);\n");
			break;
	}

	strcat(prog, "  getchar();\n");

	strcat(prog, "}\n");

	setlocale(LC_NUMERIC, "");
	return prog;
}
//---------------------------------------------------------------------------
