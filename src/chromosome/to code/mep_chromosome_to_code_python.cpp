// Multi Expression Programming library
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
void print_instruction_to_Python(int op,
								 unsigned int adr1,
								 unsigned int adr2,
								 unsigned int adr3,
								 unsigned int adr4,
								 char data_type,
								 char* tmp_s)
{
	switch (op) {
	case O_ADDITION:
		snprintf(tmp_s, 100, "prg[%u] + prg[%u]", adr1, adr2);
		break;
	case O_SUBTRACTION:
		snprintf(tmp_s, 100, "prg[%u] - prg[%u]", adr1, adr2);
		break;
	case O_MULTIPLICATION:
		snprintf(tmp_s, 100, "prg[%u] * prg[%u]", adr1, adr2);
		break;
	case O_DIVISION:
		snprintf(tmp_s, 100, "prg[%u] / prg[%u]", adr1, adr2);
		break;
	case O_POWER:
		snprintf(tmp_s, 100, "math.pow(prg[%u], prg[%u])", adr1, adr2);
		break;
	case O_SQRT:
		snprintf(tmp_s, 100, "math.sqrt(prg[%u])", adr1);
		break;
	case O_EXP:
		snprintf(tmp_s, 100, "math.exp(prg[%u])", adr1);
		break;
	case O_POW10:
		snprintf(tmp_s, 100, "math.pow(10, prg[%u])", adr1);
		break;
	case O_LN:
		snprintf(tmp_s, 100, "math.log(prg[%u])", adr1);
		break;
	case O_LOG10:
		snprintf(tmp_s, 100, "math.log10(prg[%u])", adr1);
		break;
	case O_LOG2:
		snprintf(tmp_s, 100, "math.log2(prg[%u])", adr1);
		break;
	case O_FLOOR:
		snprintf(tmp_s, 100, "math.floor(prg[%u])", adr1);
		break;
	case O_CEIL:
		snprintf(tmp_s, 100, "math.ceil(prg[%u])", adr1);
		break;
	case O_ABS:
		snprintf(tmp_s, 100, "abs(prg[%u])", adr1);
		break;
	case O_INV:
		snprintf(tmp_s, 100, "1/prg[%u]", adr1);
		break;
	case O_NEG:
		snprintf(tmp_s, 100, "-prg[%u]", adr1);
		break;
	case O_X2:
		snprintf(tmp_s, 100, "prg[%u] * prg[%u]", adr1, adr1);
		break;
	case O_MIN:
		snprintf(tmp_s, 100, "min(prg[%u], prg[%u])", adr1, adr2);
		break;
	case O_MAX:
		snprintf(tmp_s, 100, "max(prg[%u], prg[%u])", adr1, adr2);
		break;
	case O_SIN:
		snprintf(tmp_s, 100, "math.sin(prg[%u])", adr1);
		break;
	case O_COS:
		snprintf(tmp_s, 100, "math.cos(prg[%u])", adr1);
		break;
	case O_TAN:
		snprintf(tmp_s, 100, "math.tan(prg[%u])", adr1);
		break;
	case O_ASIN:
		snprintf(tmp_s, 100, "math.asin(prg[%u])", adr1);
		break;
	case O_ACOS:
		snprintf(tmp_s, 100, "math.acos(prg[%u])", adr1);
		break;
	case O_ATAN:
		snprintf(tmp_s, 100, "math.atan(prg[%u])", adr1);
		break;
	case O_IFLZ:
		snprintf(tmp_s, 100, "prg[%u] if prg[%u] < 0 else prg[%u] # ifalzbc", adr2, adr1, adr3);
		break;
	case O_IFALBCD:
		snprintf(tmp_s, 100, "prg[%u] if prg[%u] < prg[%u] else prg[%u] # ifalbcd", adr3, adr1, adr2, adr4);
		break;
	case O_IF_A_OR_B_CD:
		snprintf(tmp_s, 100, "prg[%u] if prg[%u] < 0 or prg[%u] < 0 else prg[%u] # if_a_or_b_cd", adr3, adr1, adr2, adr4);
		break;
	case O_IF_A_XOR_B_CD:
		snprintf(tmp_s, 100, "prg[%u] if prg[%u] < 0 != prg[%u] < 0 else prg[%u] # if_a_xor_b_cd", adr3, adr1, adr2, adr4);
		break;
	case O_FMOD:
		if (data_type == MEP_DATA_LONG_LONG)
			snprintf(tmp_s, 100, "prg[%u] %% prg[%u]", adr1, adr2);
		else
			snprintf(tmp_s, 100, "math.fmod(prg[%u], prg[%u]) # fmod", adr1, adr2);
		break;

	case O_NUM_INPUTS:
		snprintf(tmp_s, 100, "%d # num. inputs", 0);
		break;

	case O_INPUTS_AVERAGE:
		snprintf(tmp_s, 100, "%lf # inputs average", 0.0);
		break;
	}
}
//---------------------------------------------------------------------------------
void t_mep_chromosome::print_example_of_utilization_Python(double** data_double,
													  long long** data_long,
													  char* prog)
{
	char tmp_s[100];
	strcat(prog, "#example of utilization ...\n");
	strcat(prog, "\n");
	snprintf(tmp_s, 100, "x = [\n");
	strcat(prog, tmp_s);
	if (data_type == MEP_DATA_LONG_LONG){
		if (problem_type == MEP_PROBLEM_TIME_SERIE){
			if (num_program_outputs > 1){
				unsigned int window_size = num_total_variables / num_program_outputs;
				
				for (unsigned int w = 0; w < window_size; w++){
					strcat(prog, "[");
					for (unsigned int v = 0; v < num_program_outputs - 1; v++) {
						snprintf(tmp_s, 100, "%lld, ", data_long[w][v]);
						strcat(prog, tmp_s);
					}
					snprintf(tmp_s, 100, "%lld]", data_long[w][num_program_outputs - 1]);
					strcat(prog, tmp_s);
					if (w < window_size - 1)
						strcat(prog, ", ");
				}
			}
			else{// univariate
				for (unsigned int i = 0; i < num_total_variables - 1; i++) {
					snprintf(tmp_s, 100, "%lld, ", data_long[i][0]);
					strcat(prog, tmp_s);
				}
				snprintf(tmp_s, 100, "%lld", data_long[num_total_variables - 1][0]);
			}
		}
		else{// not time series
			for (unsigned int i = 0; i < num_total_variables - 1; i++) {
				snprintf(tmp_s, 100, "%lld, ", data_long[i][0]);
				strcat(prog, tmp_s);
			}
			snprintf(tmp_s, 100, "%lld", data_long[num_total_variables - 1][0]);
			strcat(prog, tmp_s);
		}
	}
	else{// double data type
		if (problem_type == MEP_PROBLEM_TIME_SERIE){
			if (num_program_outputs > 1){
				unsigned int window_size = num_total_variables / num_program_outputs;
				
				for (unsigned int w = 0; w < window_size; w++){
					strcat(prog, "[");
					for (unsigned int v = 0; v < num_program_outputs - 1; v++) {
						snprintf(tmp_s, 100, "%lf, ", data_double[w][v]);
						strcat(prog, tmp_s);
					}
					snprintf(tmp_s, 100, "%lf]", data_double[w][num_program_outputs - 1]);
					strcat(prog, tmp_s);
					if (w < window_size - 1)
						strcat(prog, ", ");
				}
			}
			else{// univariate
				for (unsigned int i = 0; i < num_total_variables - 1; i++) {
					snprintf(tmp_s, 100, "  %lf,\n", data_double[i][0]);
					strcat(prog, tmp_s);
				}
				snprintf(tmp_s, 100, "  %lf\n", data_double[num_total_variables - 1][0]);
			}
		}
		else{// not time series
			for (unsigned int i = 0; i < num_total_variables - 1; i++) {
				snprintf(tmp_s, 100, "%lf, ", data_double[0][i]);
				strcat(prog, tmp_s);
			}
			snprintf(tmp_s, 100, "%lf", data_double[0][num_total_variables - 1]);
			strcat(prog, tmp_s);
		}
	}
	
	strcat(prog, "]\n");

	strcat(prog, "\n");
	if ((problem_type == MEP_PROBLEM_TIME_SERIE ||
		problem_type == MEP_PROBLEM_REGRESSION) &&
					num_program_outputs > 1){
		snprintf(tmp_s, 100, "outputs = [0] * %u\n", num_program_outputs);
		strcat(prog, tmp_s);
	}
	else{
		strcat(prog, "outputs = [0]\n");
	}
	strcat(prog, "\n");
	strcat(prog, "mepx(x, outputs)\n\n");

	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_TIME_SERIE:
		if (num_program_outputs > 1){
			snprintf(tmp_s, 100, "for c in range(%u):\n", num_program_outputs);
			strcat(prog, tmp_s);
			strcat(prog, "  print(outputs[c])\n");
		}
		else{ // 1 output
			strcat(prog, "print(outputs[0])\n");
		}
		break;
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		strcat(prog, "print(\"class = \", int(outputs[0]))\n");
		break;
	}

}
//---------------------------------------------------------------------------------
void t_mep_chromosome::print_MEP_code_Python(char* s_prog,
										const t_code3* mep_code,
										unsigned int num_utilized_instructions)
{
	char tmp_s[100];
	snprintf(tmp_s, 100, "  prg = [0] * %u\n", num_utilized_instructions);
	strcat(s_prog, tmp_s);
	for (unsigned int i = 0; i < num_utilized_instructions; i++) {
		snprintf(tmp_s, 100, "  prg[%u] = ", i);
		strcat(s_prog, tmp_s);

		if (mep_code[i].op < 0) {
			print_instruction_to_Python(mep_code[i].op,
										mep_code[i].addr1,
										mep_code[i].addr2,
										mep_code[i].addr3,
										mep_code[i].addr4,
										data_type,
										tmp_s);
			strcat(s_prog, tmp_s);
			strcat(s_prog, "\n");
		}
		else { // a variable
			if (mep_code[i].op < (int)num_total_variables) {
				if (problem_type == MEP_PROBLEM_TIME_SERIE &&
					num_program_outputs > 1)
					snprintf(tmp_s, 100, "x[%u][%u]",
							 (unsigned int)mep_code[i].op / num_program_outputs,
							 (unsigned int)mep_code[i].op % num_program_outputs
							 );
				else
					snprintf(tmp_s, 100, "x[%u]", (unsigned int)mep_code[i].op);
				strcat(s_prog, tmp_s);
				strcat(s_prog, "\n");
			}
			else {
				if (data_type == MEP_DATA_LONG_LONG)
					snprintf(tmp_s, 100, "%lld", long_constants[mep_code[i].op - num_total_variables]);
				else
					snprintf(tmp_s, 100, "%lg", real_constants[mep_code[i].op - num_total_variables]);
				strcat(s_prog, tmp_s);
				strcat(s_prog, "\n");
			}
		}
	}
}
//---------------------------------------------------------------------------------
void t_mep_chromosome::print_output_calculation_Python(char* prog,
											int num_utilized_instructions,
											const unsigned int *index_output_instructions)
{
	char tmp_s[100];
	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_TIME_SERIE:
		for (unsigned int c = 0; c < num_program_outputs; c++){
			snprintf(tmp_s, 100, "  outputs[%u] = prg[%u]\n", c, index_output_instructions[c]);
			strcat(prog, tmp_s);
		}
		break;
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
		snprintf(tmp_s, 100, "  if prg[%u] <= %lf:\n    outputs[0] = %d\n  else:\n    outputs[0] = %d",
				 index_output_instructions[0], best_class_threshold, class_labels[0], class_labels[1]);
		strcat(prog, tmp_s);
		break;
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		switch (error_measure){
			case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
			// both
			strcat(prog, "  centers = [\n");
			for (unsigned int c = 0; c < num_classes; c++) {
				snprintf(tmp_s, 100, "%lf", centers[c]);
				strcat(prog, tmp_s);
				if (c < num_classes - 1)
					strcat(prog, ", ");
			}
			strcat(prog, "]\n");
			
			snprintf(tmp_s, 100, "  min_dist = abs(prg[%u] - centers[0])\n",
					 index_output_instructions[0]);
			strcat(prog, tmp_s);

			strcat(prog, "  closest_class_index = 0;\n");
			snprintf(tmp_s, 100, "  for c in range(%u):\n", num_classes);
			strcat(prog, tmp_s);

			snprintf(tmp_s, 100, "    if min_dist > abs(prg[%u] - centers[c]):\n",
						index_output_instructions[0]);
			strcat(prog, tmp_s);

			snprintf(tmp_s, 100, "      min_dist = abs(prg[%u] - centers[c])\n",
						index_output_instructions[0]);
			strcat(prog, tmp_s);

			strcat(prog, "      closest_class_index = c\n");

			strcat(prog, "  outputs[0] = class_labels[closest_class_index]\n");
				break;
			// not-simplified only
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
			case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
						snprintf(tmp_s, 100, "# find maximal value and divide index by num classes\n");
						strcat(prog, tmp_s);

						strcat(prog, "  max_value = prg[0]\n");
						strcat(prog, "  index_max_value = 0\n");
						snprintf(tmp_s, 100, "  for i in range(%u):\n", code_length);
						strcat(prog, tmp_s);

						strcat(prog, "    if max_value < prg[i]:\n      max_value = prg[i]\n      index_max_value = i\n    \n");

						snprintf(tmp_s, 100, "  outputs[0] = class_labels[index_max_value %% %u]\n", num_classes);
						strcat(prog, tmp_s);
						break;
					case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:

						snprintf(tmp_s, 100, "# index of genes holding the output for each class\n");
						strcat(prog, tmp_s);

						snprintf(tmp_s, 100, "  index_best_genes = [");
						strcat(prog, tmp_s);
						for (unsigned int c = 0; c < num_classes; c++) {
							snprintf(tmp_s, 100, "%u\n", index_best_genes[c]);
							strcat(prog, tmp_s);
							if (c < num_classes - 1)
								strcat(prog, ", ");
						}
						
						strcat(prog, "]\n");

						snprintf(tmp_s, 100, "# find maximal value\n");
						strcat(prog, tmp_s);

						strcat(prog, "  max_value = prg[0]\n");
						strcat(prog, "  index_max_value = 0\n");
						snprintf(tmp_s, 100, "  for i in range(%u):\n", code_length);
						strcat(prog, tmp_s);

						strcat(prog, "    if max_value < prg[i]:\n      max_value = prg[i]\n      index_max_value = i\n    \n");

						strcat(prog, "  #find the class\n");
						snprintf(tmp_s, 100, "  class_index = %u\n", num_classes);
						strcat(prog, tmp_s);

						snprintf(tmp_s, 100, "  for c in range(%u):\n", num_classes);

						strcat(prog, tmp_s);

						strcat(prog, "    if index_best_genes[c] == index_max_value:\n      class_index = c\n      break\n    \n");
						strcat(prog, "  \n");

						snprintf(tmp_s, 100, "  if class_index == %u:\n", num_classes);
						strcat(prog, tmp_s);

						strcat(prog, "    # find the closest max\n");
						strcat(prog, "    min_dist = abs(max_value - prg[index_best_genes[0]])\n");
						strcat(prog, "    class_index = 0\n");
						snprintf(tmp_s, 100, "    for c in range(%u):\n", num_classes);
						strcat(prog, tmp_s);

						strcat(prog, "      if min_dist > abs(max_value - prg[index_best_genes[c]]):\n");
						strcat(prog, "        min_dist = abs(max_value - prg[index_best_genes[c]])\n");

						strcat(prog, "        class_index = c\n");

						snprintf(tmp_s, 100, "  outputs[0] = class_labels[class_index]\n");
						strcat(prog, tmp_s);
						break;

		}// end switch error_measure
	}
}
//---------------------------------------------------------------------------------
char* t_mep_chromosome::to_Python_code(bool simplified,
										 double **data_double,
										 long long** data_long,
	const char* libmep_version)
{
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	char* saved_locale = strdup (old_locale);
	
	setlocale(LC_NUMERIC, "C");

	char* prog = new char[(code_length + num_constants + num_total_variables) * 100 + 5000];
	char tmp_s[100];
	prog[0] = 0;
	//strcat(prog, "#include <math.h>\n");
	//strcat(prog, "#include <stdio.h>\n");
	//strcat(prog, "\n");

	snprintf(tmp_s, 100, "#Generated by libmep version %s\n\n", libmep_version);
	strcat(prog, tmp_s);
	strcat(prog, "import math\n\n");
	strcat(prog, "def mepx(x, outputs):");
	strcat(prog, "\n");
	
	if (problem_type == MEP_PROBLEM_MULTICLASS_CLASSIFICATION){
		// here print the class labels
		strcat(prog, "\n");
		snprintf(tmp_s, 100, "  class_labels = [");
		strcat(prog, tmp_s);
		for (unsigned int i = 0; i < num_classes; i++) {
			snprintf(tmp_s, 100, "%d", class_labels[i]);
			strcat(prog, tmp_s);
			if (i < num_classes - 1)
				strcat(prog, ", ");
		}
		strcat(prog, "]\n");
		strcat(prog, "\n");
	}

	if (simplified && simplified_prg){
		print_MEP_code_Python(prog, simplified_prg, num_utilized_genes);
		
		strcat(prog, "\n");
		print_output_calculation_Python(prog,
								   num_utilized_genes,
								   index_best_genes_simplified);
	}
	else {// not simplified or could not be simplified
		print_MEP_code_Python(prog, prg, code_length);
		
		strcat(prog, "\n");
		print_output_calculation_Python(prog,
								   code_length,
								   index_best_genes);
		
	}// end else not simplified
/*
	if (simplified) {
		

		strcat(prog, "\n");
	}
	else {// not simplified
		snprintf(tmp_s, 100, "  prg = [0] * %u\n", code_length);
		strcat(prog, tmp_s);

		for (unsigned int i = 0; i < code_length; i++) {
			snprintf(tmp_s, 100, "  prg[%u] = ", i);
			strcat(prog, tmp_s);

			if (prg[i].op < 0) {
				print_instruction_to_Python(prg[i].op,
											prg[i].addr1, prg[i].addr2,
											prg[i].addr3, prg[i].addr4,
											data_type,
											tmp_s);
				strcat(prog, tmp_s);
				strcat(prog, "\n");
			}
			else { // a variable
				if (prg[i].op < (int)num_total_variables) {
					snprintf(tmp_s, 100, "x[%u]", (unsigned int)prg[i].op);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
				else {
					if (data_type == MEP_DATA_LONG_LONG)
						snprintf(tmp_s, 100, "%lld", long_constants[prg[i].op - num_total_variables]);
					else
						snprintf(tmp_s, 100, "%lg", real_constants[prg[i].op - num_total_variables]);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
			}
		}

		strcat(prog, "\n");
		switch (problem_type) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_TIME_SERIE:
			snprintf(tmp_s, 100, "  outputs[0] = prg[%u]", index_best_genes[0]);
			strcat(prog, tmp_s);
			break;

		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			snprintf(tmp_s, 100, "  if prg[%u] <= %lg:\n    outputs[0] = %d\n  else:\n    outputs[0] = %d", index_best_genes[0], best_class_threshold, class_labels[0], class_labels[1]);
			strcat(prog, tmp_s);
			break;
			break;
		} // end switch (problem_type)
	}// end else not simplified
	*/
	strcat(prog, "\n");
	//strcat(prog, "}\n");

	//strcat(prog, "\n");
	//strcat(prog, "int main(void)\n");
	//strcat(prog, "{\n");
	strcat(prog, "\n");
	print_example_of_utilization_Python(data_double, data_long, prog);
	setlocale(LC_NUMERIC, saved_locale);
	free (saved_locale);

	return prog;
}
//---------------------------------------------------------------------------
