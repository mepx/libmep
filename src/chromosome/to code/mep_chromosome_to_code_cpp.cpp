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
void print_instruction_to_C(int op,
							unsigned int adr1, unsigned int adr2,
							unsigned int adr3, unsigned int adr4,
							char data_type,
							char* tmp_s)
{
	switch (op) {
	case O_ADDITION:
		snprintf(tmp_s, 100, "prg[%u] + prg[%u];", adr1, adr2);
		break;
	case O_SUBTRACTION:
		snprintf(tmp_s, 100, "prg[%u] - prg[%u];", adr1, adr2);
		break;
	case O_MULTIPLICATION:
		snprintf(tmp_s, 100, "prg[%u] * prg[%u];", adr1, adr2);
		break;
	case O_DIVISION:
		snprintf(tmp_s, 100, "prg[%u] / prg[%u];", adr1, adr2);
		break;
	case O_POWER:
		snprintf(tmp_s, 100, "pow(prg[%u], prg[%u]);", adr1, adr2);
		break;
	case O_SQRT:
		snprintf(tmp_s, 100, "sqrt(prg[%u]);", adr1);
		break;
	case O_EXP:
		snprintf(tmp_s, 100, "exp(prg[%u]);", adr1);
		break;
	case O_POW10:
		snprintf(tmp_s, 100, "pow(10, prg[%u]);", adr1);
		break;
	case O_LN:
		snprintf(tmp_s, 100, "log(prg[%u]);", adr1);
		break;
	case O_LOG10:
		snprintf(tmp_s, 100, "log10(prg[%u]);", adr1);
		break;
	case O_LOG2:
		snprintf(tmp_s, 100, "log2(prg[%u]);", adr1);
		break;
	case O_FLOOR:
		snprintf(tmp_s, 100, "floor(prg[%u]);", adr1);
		break;
	case O_CEIL:
		snprintf(tmp_s, 100, "ceil(prg[%u]);", adr1);
		break;
	case O_ABS:
		snprintf(tmp_s, 100, "fabs(prg[%u]);", adr1);
		break;
	case O_INV:
		snprintf(tmp_s, 100, "1/prg[%u];", adr1);
		break;
	case O_NEG:
		snprintf(tmp_s, 100, "-prg[%u];", adr1);
		break;
	case O_X2:
		snprintf(tmp_s, 100, "prg[%u] * prg[%u];", adr1, adr1);
		break;
	case O_MIN:
		snprintf(tmp_s, 100, "prg[%u] < prg[%u]?prg[%u] : prg[%u]; // min", adr1, adr2, adr1, adr2);
		break;
	case O_MAX:
		snprintf(tmp_s, 100, "prg[%u] > prg[%u]?prg[%u] : prg[%u]; // max", adr1, adr2, adr1, adr2);
		break;
	case O_SIN:
		snprintf(tmp_s, 100, "sin(prg[%u]);", adr1);
		break;
	case O_COS:
		snprintf(tmp_s, 100, "cos(prg[%u]);", adr1);
		break;
	case O_TAN:
		snprintf(tmp_s, 100, "tan(prg[%u]);", adr1);
		break;
	case O_ASIN:
		snprintf(tmp_s, 100, "asin(prg[%u]);", adr1);
		break;
	case O_ACOS:
		snprintf(tmp_s, 100, "acos(prg[%u]);", adr1);
		break;
	case O_ATAN:
		snprintf(tmp_s, 100, "atan(prg[%u]);", adr1);
		break;
	case O_IFLZ:
		snprintf(tmp_s, 100, "prg[%u] < 0?prg[%u] : prg[%u]; // ifalzbc", adr1, adr2, adr3);
		break;
	case O_IFALBCD:
		snprintf(tmp_s, 100, "prg[%u] < prg[%u]?prg[%u] : prg[%u]; // ifalbcd", adr1, adr2, adr3, adr4);
		break;
	case O_IF_A_OR_B_CD:
		snprintf(tmp_s, 100, "prg[%u] < 0 || prg[%u] < 0 ? prg[%u] : prg[%u]; // if_a_or_b_cd", adr1, adr2, adr3, adr4);
		break;
	case O_IF_A_XOR_B_CD:
		snprintf(tmp_s, 100, "prg[%u] < 0 != prg[%u] < 0 ? prg[%u] : prg[%u]; // if_a_xor_b_cd", adr1, adr2, adr3, adr4);
		break;
	case O_FMOD:
		if (data_type == MEP_DATA_LONG_LONG)
			snprintf(tmp_s, 100, "prg[%u] %% prg[%u]; // mod", adr1, adr2);
		else
			snprintf(tmp_s, 100, "fmod(prg[%u], prg[%u]); // fmod", adr1, adr2);
		break;

	case O_NUM_INPUTS:
		snprintf(tmp_s, 100, "%d; // num. inputs", 0);
		break;

	case O_INPUTS_AVERAGE:
		snprintf(tmp_s, 100, "%lf; // inputs average", 0.0);
		break;
	}
}
//---------------------------------------------------------------------------------
void t_mep_chromosome::print_example_of_utilization_C(double** data_double,
													  long long** data_long,
													  char* prog)
{
	char tmp_s[100];
	
	strcat(prog, "\n");
	strcat(prog, "int main(void)\n");
	strcat(prog, "{\n");
	strcat(prog, "\n");
	strcat(prog, "//example of utilization ...\n");
	strcat(prog, "\n");
	if (data_type == MEP_DATA_LONG_LONG){
		if (problem_type == MEP_PROBLEM_TIME_SERIE){
			if (num_program_outputs > 1){
				unsigned int window_size = num_total_variables / num_program_outputs;
				snprintf(tmp_s, 100, "  long long x[%u][%u];\n", window_size, num_program_outputs);
				strcat(prog, tmp_s);
				
				for (unsigned int w = 0; w < window_size; w++)
					for (unsigned int v = 0; v < num_program_outputs; v++) {
						snprintf(tmp_s, 100, "  x[%u][%u] = %lld;\n", w, v, data_long[w][v]);
						strcat(prog, tmp_s);
					}
			}
			else{// univariate time series
				snprintf(tmp_s, 100, "  long long x[%u];\n", num_total_variables);
				strcat(prog, tmp_s);
				
				for (unsigned int w = 0; w < num_total_variables; w++){
					snprintf(tmp_s, 100, "  x[%u] = %lld;\n", w, data_long[w][0]);
					strcat(prog, tmp_s);
				}
			}
		}
		else{ // not time series
			snprintf(tmp_s, 100, "  long long x[%u];\n", num_total_variables);
			strcat(prog, tmp_s);
			for (unsigned int i = 0; i < num_total_variables; i++) {
				snprintf(tmp_s, 100, "  x[%u] = %lld;\n", i, data_long[0][i]);
				strcat(prog, tmp_s);
			}
		}

		strcat(prog, "\n");
		switch (problem_type) {
			case MEP_PROBLEM_REGRESSION:
			case MEP_PROBLEM_TIME_SERIE:
				snprintf(tmp_s, 100, "  long long outputs[%u];\n", num_program_outputs);
				strcat(prog, tmp_s);
				break;
			case MEP_PROBLEM_BINARY_CLASSIFICATION:
			case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
				strcat(prog, "  long long outputs[1];\n");
				break;
		}
		strcat(prog, "\n\n");
		strcat(prog, "  mepx(x, outputs);\n\n");

		switch (problem_type) {
			case MEP_PROBLEM_REGRESSION:
			case MEP_PROBLEM_TIME_SERIE:
				if (num_program_outputs > 1){
					snprintf(tmp_s, 100, "  for (unsigned int i = 0; i < %u; i++)\n", num_program_outputs);
					strcat(prog, tmp_s);
					strcat(prog, "  printf(\"%lld\\n\", outputs[i]);\n");
				}
				else{
					strcat(prog, "  printf(\"%d\", outputs[0]);\n");
				}
				break;
			case MEP_PROBLEM_BINARY_CLASSIFICATION:
			case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
				strcat(prog, "  printf(\"class = %d\", (int)outputs[0]);\n");
				break;
		}
	}
	else{
		// double data type
		if (problem_type == MEP_PROBLEM_TIME_SERIE){
			if (num_program_outputs > 1){
				unsigned int window_size = num_total_variables / num_program_outputs;
				snprintf(tmp_s, 100, "  double x[%u][%u];\n", window_size, num_program_outputs);
				strcat(prog, tmp_s);
				
				for (unsigned int w = 0; w < window_size; w++)
					for (unsigned int v = 0; v < num_program_outputs; v++) {
						snprintf(tmp_s, 100, "  x[%u][%u] = %lf;\n", w, v, data_double[w][v]);
						strcat(prog, tmp_s);
					}
			}
			else{ // univariate time series
				snprintf(tmp_s, 100, "  double x[%u];\n", num_total_variables);
				strcat(prog, tmp_s);
				
				for (unsigned int w = 0; w < num_total_variables; w++){
					snprintf(tmp_s, 100, "  x[%u] = %lf;\n", w, data_double[w][0]);
					strcat(prog, tmp_s);
				}

			}
		}
		else{// not time series
			snprintf(tmp_s, 100, "  double x[%u];\n", num_total_variables);
			strcat(prog, tmp_s);
			for (unsigned int i = 0; i < num_total_variables; i++) {
				snprintf(tmp_s, 100, "  x[%u] = %lf;\n", i, data_double[0][i]);
				strcat(prog, tmp_s);
			}
		}

		strcat(prog, "\n");
		switch (problem_type) {
			case MEP_PROBLEM_REGRESSION:
			case MEP_PROBLEM_TIME_SERIE:
				snprintf(tmp_s, 100, "  double outputs[%u];\n", num_program_outputs);
				strcat(prog, tmp_s);
				break;
			case MEP_PROBLEM_BINARY_CLASSIFICATION:
			case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
				strcat(prog, "  double outputs[1];\n");
				break;
		}
		strcat(prog, "\n");
		strcat(prog, "  mepx(x, outputs);\n\n");
// print the results
		switch (problem_type) {
			case MEP_PROBLEM_REGRESSION:
			case MEP_PROBLEM_TIME_SERIE:
				if (num_program_outputs > 1){
					snprintf(tmp_s, 100, "  for (unsigned int i = 0; i < %u; i++)\n", num_program_outputs);
					strcat(prog, tmp_s);
					strcat(prog, "    printf(\"%lf\\n\", outputs[i]);\n");
				}
				else{
					strcat(prog, "  printf(\"%lf\", outputs[0]);\n");
				}
				break;
			case MEP_PROBLEM_BINARY_CLASSIFICATION:
			case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
				strcat(prog, "  printf(\"class = %d\", (int)outputs[0]);\n");
				break;
		}
	}

	strcat(prog, "  getchar();\n");

	strcat(prog, "}\n");
}
//------------------------------------------------------------------
void t_mep_chromosome::print_MEP_code_C(char* s_prog,
										const t_code3* mep_code,
										unsigned int num_utilized_instructions)
{
	char tmp_s[100];
	
	if (data_type == MEP_DATA_LONG_LONG)
		strcat(s_prog, "  long long prg[");
	else
		strcat(s_prog, "  double prg[");
	
	snprintf(tmp_s, 100, "%u", num_utilized_instructions);
	strcat(s_prog, tmp_s);
	strcat(s_prog, "];");
	strcat(s_prog, "\n\n");

	for (unsigned int i = 0; i < num_utilized_instructions; i++) {
		snprintf(tmp_s, 100, "  prg[%u] = ", i);
		strcat(s_prog, tmp_s);

		if (mep_code[i].op < 0) {
			print_instruction_to_C(mep_code[i].op,
								   mep_code[i].addr1, mep_code[i].addr2,
								   mep_code[i].addr3, mep_code[i].addr4,
								   data_type, tmp_s);
			strcat(s_prog, tmp_s);
			strcat(s_prog, "\n");
		}
		else { // a variable
			if (mep_code[i].op < (int)num_total_variables) {
				if (problem_type == MEP_PROBLEM_TIME_SERIE &&
					num_program_outputs > 1)
					snprintf(tmp_s, 100, "x[%u][%u];",
							 (unsigned int)mep_code[i].op / num_program_outputs,
							 (unsigned int)mep_code[i].op % num_program_outputs);
				else
					snprintf(tmp_s, 100, "x[%u];", (unsigned int)mep_code[i].op);
				strcat(s_prog, tmp_s);
				strcat(s_prog, "\n");
			}
			else {
				if (data_type == MEP_DATA_LONG_LONG)
					snprintf(tmp_s, 100, "%lld;", long_constants[mep_code[i].op - num_total_variables]);
				else
					snprintf(tmp_s, 100, "%lg;", real_constants[mep_code[i].op - num_total_variables]);
				strcat(s_prog, tmp_s);
				strcat(s_prog, "\n");
			}
		}
	}
}
//------------------------------------------------------------------
void t_mep_chromosome::print_output_calculation_C(char* s_prog,
											int num_utilized_instructions,
											const unsigned int *index_output_instructions)
{
	char tmp_s[100];
	
	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_TIME_SERIE:
		for (unsigned int c = 0; c < num_program_outputs; c++){
			snprintf(tmp_s, 100, "  outputs[%u] = prg[%u];\n",
					 c,
					 index_output_instructions[c]);
			strcat(s_prog, tmp_s);
		}
		break;
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
		snprintf(tmp_s, 100, "  if (prg[%u] <= %lf)\n    outputs[0] = %d;\n  else\n    outputs[0] = %d;",
				 index_output_instructions[0],
				 best_class_threshold,
				 class_labels[0], class_labels[1]);
		strcat(s_prog, tmp_s);
		break;
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		switch (error_measure) {
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
			case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
				// not simplified only
				snprintf(tmp_s, 100, "// find maximal value and divide index by num classes\n");
				strcat(s_prog, tmp_s);
				
				strcat(s_prog, "  double max_value = prg[0];\n");
				strcat(s_prog, "  int index_max_value = 0;\n");
				snprintf(tmp_s, 100, "  for (unsigned int i = 1; i < %u; i++)\n",
								code_length);
				strcat(s_prog, tmp_s);
				
				strcat(s_prog, "    if (max_value < prg[i]){\n      max_value = prg[i];\n      index_max_value = i;\n    }\n");
				
				snprintf(tmp_s, 100, "  outputs[0] = class_labels[index_max_value %% %u];\n",
						 num_classes);
				strcat(s_prog, tmp_s);
				break;
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
				// not simplified only
				snprintf(tmp_s, 100, "// index of genes holding the output for each class\n");
				strcat(s_prog, tmp_s);
				
				snprintf(tmp_s, 100, "  int index_best_genes[%u];\n",
						 num_classes);
				strcat(s_prog, tmp_s);
				for (unsigned int c = 0; c < num_classes; c++) {
					snprintf(tmp_s, 100, "  index_best_genes[%u] = %u;\n", c,
							 index_output_instructions[c]);
					strcat(s_prog, tmp_s);
				}
				
				snprintf(tmp_s, 100, "// find maximal value\n");
				strcat(s_prog, tmp_s);
				
				strcat(s_prog, "  double max_value = prg[0];\n");
				strcat(s_prog, "  int index_max_value = 0;\n");
				snprintf(tmp_s, 100, "  for (unsigned int i = 1; i < %u; i++)\n",
						 code_length);
				strcat(s_prog, tmp_s);
				
				strcat(s_prog, "    if (max_value < prg[i]){\n      max_value = prg[i];\n      index_max_value = i;\n    }\n");
				
				strcat(s_prog, "  //find the class\n");
				snprintf(tmp_s, 100, "  unsigned int class_index = %u;\n",
						 num_classes);
				strcat(s_prog, tmp_s);
				
				snprintf(tmp_s, 100, "  for (unsigned int c = 0; c < %u; c++){\n",
						 num_classes);
				
				strcat(s_prog, tmp_s);
				
				strcat(s_prog, "    if (index_best_genes[c] == index_max_value) {\n      class_index = c;\n      break;\n    }\n");
				strcat(s_prog, "  }\n");
				
				snprintf(tmp_s, 100, "  if (class_index == %u){\n",
						 num_classes);
				strcat(s_prog, tmp_s);
				
				strcat(s_prog, "    // find the closest max\n");
				strcat(s_prog, "    double min_dist = fabs(max_value - prg[index_best_genes[0]]);\n");
				strcat(s_prog, "    class_index = 0;\n");
				snprintf(tmp_s, 100, "    for (unsigned c = 1; c < %u; c++){\n",
								num_classes);
				strcat(s_prog, tmp_s);
				
				strcat(s_prog, "      if (min_dist > fabs(max_value - prg[index_best_genes[c]])){\n");
				strcat(s_prog, "        min_dist = fabs(max_value - prg[index_best_genes[c]]);\n");
				
				strcat(s_prog, "        class_index = c;\n");
				strcat(s_prog, "      }\n");
				strcat(s_prog, "    }\n");
				
				strcat(s_prog, "  }\n");
				
				snprintf(tmp_s, 100, "  outputs[0] = class_labels[class_index];\n");
				strcat(s_prog, tmp_s);
				break;
				
			case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
				// simplified + not simplified
				snprintf(tmp_s, 100, "  double centers[%u];\n",
						 num_classes);
				strcat(s_prog, tmp_s);
				for (unsigned int c = 0; c < num_classes; c++) {
					snprintf(tmp_s, 100, "  centers[%u] = %lf;\n",
							 c, centers[c]);
					strcat(s_prog, tmp_s);
				}
				snprintf(tmp_s, 100, "  double min_dist = fabs(prg[%u] - centers[0]);\n",
						 index_output_instructions[0]);
				strcat(s_prog, tmp_s);
				
				strcat(s_prog,   "  unsigned int closest_class_index = 0;\n");
				snprintf(tmp_s, 100, "  for (unsigned int c = 1; c < %u; c++)\n",
						 num_classes);
				strcat(s_prog, tmp_s);
				
				snprintf(tmp_s, 100, "    if (min_dist > fabs(prg[%u] - centers[c])) {\n",
						 index_output_instructions[0]);
				strcat(s_prog, tmp_s);
				
				snprintf(tmp_s, 100, "      min_dist = fabs(prg[%u] - centers[c]);\n",
						 index_output_instructions[0]);
				strcat(s_prog, tmp_s);
				
				strcat(s_prog,   "      closest_class_index = c;\n");
				strcat(s_prog,   "    }\n");
				strcat(s_prog,   "  outputs[0] = class_labels[closest_class_index];\n");
				break;
		} // end switch (error_measure)
		break;
	} // end switch (problem_type)
}
//------------------------------------------------------------------
char* t_mep_chromosome::to_C_code(bool simplified,
									double** data_double,
									long long** data_long,
									const char* libmep_version)
{
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	char* saved_locale = strdup (old_locale);
	setlocale(LC_NUMERIC, "C");
	
	char* prog = new char[(code_length + num_constants + num_total_variables) * 100 + 2000];
	char tmp_s[100];
	prog[0] = 0;
	snprintf(tmp_s, 100, "//Generated by libmep version %s\n\n", libmep_version);
	strcat(prog, tmp_s);
	strcat(prog, "#include <math.h>\n");
	strcat(prog, "#include <stdio.h>\n");
	strcat(prog, "\n");
	
	if (data_type == MEP_DATA_LONG_LONG){
		if (problem_type == MEP_PROBLEM_TIME_SERIE &&
			num_program_outputs > 1) {
			unsigned int window_size = num_total_variables / num_program_outputs;
			snprintf(tmp_s, 100, "void mepx(long long x[%u][%u] /*inputs*/, long long *outputs)", 
				window_size, num_program_outputs);
			strcat(prog, tmp_s);
		}
		else {
			strcat(prog, "void mepx(long long *x /*inputs*/, long long *outputs)");
		}
	}
	else{// double
		if (problem_type == MEP_PROBLEM_TIME_SERIE && num_program_outputs > 1) {
			unsigned int window_size = num_total_variables / num_program_outputs;
			snprintf(tmp_s, 100, "void mepx(double x[%u][%u] /*inputs*/, double *outputs)",
				window_size, num_program_outputs);
			strcat(prog, tmp_s);

		}
		else {
			strcat(prog, "void mepx(double *x /*inputs*/, double *outputs)");
		}
	}
	
	strcat(prog, "\n");
	strcat(prog, "{");
	strcat(prog, "\n");
	
	if (problem_type == MEP_PROBLEM_MULTICLASS_CLASSIFICATION){
		// here print the class labels
		strcat(prog, "\n");
		snprintf(tmp_s, 100, "  int class_labels[%d] = {", num_classes);
		strcat(prog, tmp_s);
		for (unsigned int i = 0; i < num_classes; i++) {
			snprintf(tmp_s, 100, "%d", class_labels[i]);
			strcat(prog, tmp_s);
			if (i < num_classes - 1)
				strcat(prog, ", ");
		}
		strcat(prog, "};\n");
		strcat(prog, "\n");
	}

	if (simplified && simplified_prg){
		print_MEP_code_C(prog, simplified_prg, num_utilized_genes);
		
		strcat(prog, "\n");
		print_output_calculation_C(prog,
								   num_utilized_genes,
								   index_best_genes_simplified);
	}
	else {// not simplified or could not be simplified
		print_MEP_code_C(prog, prg, code_length);
		
		strcat(prog, "\n");
		print_output_calculation_C(prog,
								   code_length,
								   index_best_genes);
		
	}// end else not simplified

	strcat(prog, "\n");
	strcat(prog, "}\n");

	print_example_of_utilization_C(data_double, data_long, prog);

	setlocale(LC_NUMERIC, saved_locale);
	free (saved_locale);

	return prog;
}
//---------------------------------------------------------------------------
