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
void print_instruction_to_Basic(int op,
								unsigned int adr1,
								unsigned int adr2,
								unsigned int adr3,
								unsigned int adr4,
								char data_type,
								char* tmp_s)
{
	switch (op) {
	case O_ADDITION:
		snprintf(tmp_s, 100, "prg(%u) + prg(%u)", adr1, adr2);
		break;
	case O_SUBTRACTION:
		snprintf(tmp_s, 100, "prg(%u) - prg(%u)", adr1, adr2);
		break;
	case O_MULTIPLICATION:
		snprintf(tmp_s, 100, "prg(%u) * prg(%u)", adr1, adr2);
		break;
	case O_DIVISION:
		if (data_type == MEP_DATA_LONG_LONG)
			snprintf(tmp_s, 100, "prg(%u) \\ prg(%u)", adr1, adr2);
		else
			snprintf(tmp_s, 100, "prg(%u) / prg(%u)", adr1, adr2);
		break;
	case O_POWER:
		snprintf(tmp_s, 100, "prg(%u) ^ prg(%u)", adr1, adr2);
		break;
	case O_SQRT:
		snprintf(tmp_s, 100, "Sqr(prg(%u))", adr1);
		break;
	case O_EXP:
		snprintf(tmp_s, 100, "Exp(prg(%u))", adr1);
		break;
	case O_POW10:
		snprintf(tmp_s, 100, "10 ^prg(%u)", adr1);
		break;
	case O_LN:
		snprintf(tmp_s, 100, "Log(prg(%u))", adr1);
		break;
	case O_LOG10:
		snprintf(tmp_s, 100, "Log(prg(%u)) / Log(10#)", adr1);
		break;
	case O_LOG2:
		snprintf(tmp_s, 100, "Log(prg(%u)) / Log(2#)", adr1);
		break;
	case O_FLOOR:
		snprintf(tmp_s, 100, "Int(prg(%u))", adr1);// floor
		break;
	case O_CEIL:
		snprintf(tmp_s, 100, "-Int(-prg(%u))", adr1);// ceil
		break;
	case O_ABS:
		snprintf(tmp_s, 100, "Abs(prg(%u))", adr1);
		break;
	case O_INV:
		snprintf(tmp_s, 100, "1/prg(%u)", adr1);
		break;
	case O_NEG:
		snprintf(tmp_s, 100, "-prg(%u)", adr1);
		break;
	case O_X2:
		snprintf(tmp_s, 100, "prg(%u) * prg(%u)", adr1, adr1);
		break;
	case O_MIN:
		snprintf(tmp_s, 100, "IIf (prg(%u) < prg(%u), prg(%u), prg(%u)) ' min", adr1, adr2, adr1, adr2);
		break;
	case O_MAX:
		snprintf(tmp_s, 100, "IIf (prg(%u) > prg(%u), prg(%u), prg(%u)) ' max", adr1, adr2, adr1, adr2);
		break;
	case O_SIN:
		snprintf(tmp_s, 100, "Sin(prg(%u))", adr1);
		break;
	case O_COS:
		snprintf(tmp_s, 100, "Cos(prg(%u))", adr1);
		break;
	case O_TAN:
		snprintf(tmp_s, 100, "Tan(prg(%u))", adr1);
		break;
	case O_ASIN:
		snprintf(tmp_s, 100, "WorksheetFunction.Asin(prg(%u))", adr1);
		break;
	case O_ACOS:
		snprintf(tmp_s, 100, "WorksheetFunction.Acos(prg(%u))", adr1);
		break;
	case O_ATAN:
		snprintf(tmp_s, 100, "Atn(prg(%u))", adr1);
		break;
	case O_IFLZ:
		snprintf(tmp_s, 100, "IIf (prg(%u) < 0, prg(%u), prg(%u)) ' ifalzbc", adr1, adr2, adr3);
		break;
	case O_IFALBCD:
		snprintf(tmp_s, 100, "IIf (prg(%u) < prg(%u), prg(%u), prg(%u)) ' ifalbcd", adr1, adr2, adr3, adr4);
		break;
	case O_IF_A_OR_B_CD:
		snprintf(tmp_s, 100, "IIf (prg(%u) < 0 Or prg(%u) < 0, prg(%u), prg(%u)) ' if_a_or_b_cd", adr1, adr2, adr3, adr4);
		break;
	case O_IF_A_XOR_B_CD:
		snprintf(tmp_s, 100, "IIf (prg(%u) < 0 Xor prg(%u) < 0, prg(%u), prg(%u)) ' if_a_xor_b_cd", adr1, adr2, adr3, adr4);
		break;
	case O_FMOD:
		if (data_type == MEP_DATA_LONG_LONG)
			snprintf(tmp_s, 100, "prg(%u) Mod prg(%u)", adr1, adr2);
		else // for double
			snprintf(tmp_s, 100, "Evaluate(\"MOD(\" & prg(%u) & \",\" & prg(%u) & \")\")", adr1, adr2);

		break;
	case O_NUM_INPUTS:
		snprintf(tmp_s, 100, "%d ' num. inputs", 0);
		break;

	case O_INPUTS_AVERAGE:
		snprintf(tmp_s, 100, "%lf ' inputs average", 0.0);
		break;

	}
}
//---------------------------------------------------------------------------
void t_mep_chromosome::print_MEP_code_Excel_VBA(char* s_prog,
										const t_code3* mep_code,
										unsigned int num_utilized_instructions)
{
	char tmp_s[100];
	
	if (data_type == MEP_DATA_LONG_LONG)
		snprintf(tmp_s, 100, "  Dim prg(%u) as LongLong\n", num_utilized_instructions - 1);
	else// double
		snprintf(tmp_s, 100, "  Dim prg(%u) as Double\n", num_utilized_instructions - 1);
	strcat(s_prog, tmp_s);
	strcat(s_prog, "\n");

	for (unsigned int i = 0; i < num_utilized_instructions; i++) {
		snprintf(tmp_s, 100, "  prg(%u) = ", i);
		strcat(s_prog, tmp_s);

		if (mep_code[i].op < 0) {
			print_instruction_to_Basic(mep_code[i].op,
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
				if (problem_type == MEP_PROBLEM_TIME_SERIE)
					snprintf(tmp_s, 100, "x(%d, %d).Value",
							 mep_code[i].op / num_program_outputs + 1,
							 mep_code[i].op % num_program_outputs + 1); // +1 because in VB all ranges are from 1
				else
					snprintf(tmp_s, 100, "x(1, %d).Value", mep_code[i].op + 1); // +1 because in VB all ranges are from 1

				strcat(s_prog, tmp_s);
				strcat(s_prog, "\n");
			}
			else {
				if (data_type == MEP_DATA_LONG_LONG)
					snprintf(tmp_s, 100, "%lld", long_constants[mep_code[i].op - num_total_variables]);
				else// double
					snprintf(tmp_s, 100, "%lg", real_constants[mep_code[i].op - num_total_variables]);
				strcat(s_prog, tmp_s);
				strcat(s_prog, "\n");
			}
		}
	}

}
//---------------------------------------------------------------------------
void t_mep_chromosome::print_output_calculation_Excel_VBA(char* s_prog,
											int num_utilized_instructions,
											const unsigned int *index_output_instructions)
{
	char tmp_s[100];
	
	strcat(s_prog, "\n");
	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_TIME_SERIE:
		if (num_program_outputs > 1){
			if (data_type == MEP_DATA_LONG_LONG)
				snprintf(tmp_s, 100, "  Dim output(%u) As LongLong\n\n", num_program_outputs - 1);
			else
				snprintf(tmp_s, 100, "  Dim output(%u) As Double\n\n", num_program_outputs - 1);
			strcat(s_prog, tmp_s);
			
			for (unsigned int c = 0; c < num_program_outputs; c++){
				snprintf(tmp_s, 100, "  output(%u) = prg(%u)\n",
						 c,
						 index_output_instructions[c]);
				strcat(s_prog, tmp_s);
			}
			strcat(s_prog, "\n");
			strcat(s_prog, "  mepx = output\n");
		}
		else{
			snprintf(tmp_s, 100, "  mepx = prg(%u)", index_output_instructions[0]);
			strcat(s_prog, tmp_s);
		}
		break;
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
		snprintf(tmp_s, 100, "  If (prg(%u) <= %lf) Then\n   mepx = %d\n  Else\n    mepx = %d\n Endif\n",
				 index_output_instructions[0],
				 best_class_threshold,
				 class_labels[0], class_labels[1]);
		strcat(s_prog, tmp_s);
		break;
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		switch (error_measure){
			case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
				// both
			snprintf(tmp_s, 100, "  Dim centers(%u) as Double\n", num_classes);
			strcat(s_prog, tmp_s);

			for (unsigned int c = 0; c < num_classes; c++) {
				snprintf(tmp_s, 100, "  centers(%u) = %lf\n", c, centers[c]);
				strcat(s_prog, tmp_s);
			}

			strcat(s_prog, "  Dim min_dist as Double\n");
			snprintf(tmp_s, 100, "  min_dist = Abs(prg(%u) - centers(0))\n", index_output_instructions[0]);
			strcat(s_prog, tmp_s);

			strcat(s_prog, "  Dim closest_class_index as Integer\n");
			strcat(s_prog, "  closest_class_index = 0\n");
			snprintf(tmp_s, 100, "  For c = 1 To %u\n", num_classes - 1);
			strcat(s_prog, tmp_s);

			snprintf(tmp_s, 100, "    If min_dist > Abs(prg(%u) - centers(c)) Then\n", index_output_instructions[0]);
			strcat(s_prog, tmp_s);

			snprintf(tmp_s, 100, "      min_dist = Abs(prg(%u) - centers(c))\n", index_output_instructions[0]);
			strcat(s_prog, tmp_s);

			strcat(s_prog, "      closest_class_index = c\n");
			strcat(s_prog, "    Endif\n");
			strcat(s_prog, "  Next\n");
			strcat(s_prog, "  mepx = class_labels(closest_class_index)\n");

			// should never be here
		case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
		case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
				// not simplified only
			snprintf(tmp_s, 100, "' find maximal value and divide index by num classes\n");
			strcat(s_prog, tmp_s);

			strcat(s_prog, "  max_value = prg(0)\n");
			strcat(s_prog, "  index_max_value = 0\n");
			snprintf(tmp_s, 100, "  For i = 1 To %u\n", code_length - 1);
			strcat(s_prog, tmp_s);

			strcat(s_prog, "    If max_value < prg(i) Then\n      max_value = prg(i)\n      index_max_value = i\n    EndIf\n");
			strcat(s_prog, "  Next\n");

			snprintf(tmp_s, 100, "  mepx = class_labels(index_max_value Mod %u)\n", num_classes);
			strcat(s_prog, tmp_s);
			break;
		case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
				// not simplified only
			snprintf(tmp_s, 100, "' index of genes holding the output for each class\n");
			strcat(s_prog, tmp_s);

			snprintf(tmp_s, 100, "  Dim index_best_genes(%u) As Integer\n", num_classes - 1);
			strcat(s_prog, tmp_s);
			for (unsigned int c = 0; c < num_classes; c++) {
				snprintf(tmp_s, 100, "  index_best_genes(%u) = %u\n", c, index_best_genes[c]);
				strcat(s_prog, tmp_s);
			}

			snprintf(tmp_s, 100, "' find maximal value\n");
			strcat(s_prog, tmp_s);

			strcat(s_prog, "  max_value = prg(0)\n");
			strcat(s_prog, "  index_max_value = 0\n");
			snprintf(tmp_s, 100, "  For i = 1 To %u\n", code_length - 1);
			strcat(s_prog, tmp_s);

			strcat(s_prog, "    If max_value < prg(i) Then\n      max_value = prg(i)\n      index_max_value = i\n    EndIf\n");
			strcat(s_prog, "  Next\n");

			strcat(s_prog, "  ' find the class\n");
			snprintf(tmp_s, 100, "  class_index = %u\n", num_classes);
			strcat(s_prog, tmp_s);

			snprintf(tmp_s, 100, "  For c = 0 To %u\n", num_classes - 1);

			strcat(s_prog, tmp_s);

			strcat(s_prog, "    If index_best_genes(c) = index_max_value Then\n      class_index = c\n      Exit For\n    EndIf\n");
			strcat(s_prog, "  Next\n");

			snprintf(tmp_s, 100, "  If class_index = %u Then\n", num_classes);
			strcat(s_prog, tmp_s);

			strcat(s_prog, "    ' find the closest max\n");
			strcat(s_prog, "    min_dist = Abs(max_value - prg(index_best_genes(0)))\n");
			strcat(s_prog, "    class_index = 0\n");
			snprintf(tmp_s, 100, "    For c = 1 To %u\n", num_classes - 1);
			strcat(s_prog, tmp_s);

			strcat(s_prog, "      If min_dist > Abs(max_value - prg(index_best_genes(c))) Then\n");
			strcat(s_prog, "        min_dist = Abs(max_value - prg(index_best_genes(c)))\n");

			strcat(s_prog, "        class_index = c\n");
			strcat(s_prog, "      EndIf\n");
			strcat(s_prog, "    Next\n");

			strcat(s_prog, "  EndIf\n");
			snprintf(tmp_s, 100, "  mepx = class_labels(class_index)\n");
			strcat(s_prog, tmp_s);
			break;

		}
		break;
	}

}
//---------------------------------------------------------------------------
char* t_mep_chromosome::to_Excel_VBA_function_code(bool simplified,
												const char* libmep_version)
{
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	char* saved_locale = strdup (old_locale);
	
	setlocale(LC_NUMERIC, "C");

	char* prog = new char[(code_length + num_constants + num_total_variables) * 100 + 2000];
	char tmp_s[100];
	prog[0] = 0;
	snprintf(tmp_s, 100, "'Generated by libmep version %s\n\n", libmep_version);
	strcat(prog, tmp_s);
	if (data_type == MEP_DATA_LONG_LONG){
		if ((problem_type == MEP_PROBLEM_TIME_SERIE ||
			problem_type == MEP_PROBLEM_REGRESSION) && num_program_outputs > 1)
			strcat(prog, "Function mepx(x As Range) As LongLong()");
		else
			strcat(prog, "Function mepx(x As Range) As LongLong");
	}
	else// double
		if ((problem_type == MEP_PROBLEM_TIME_SERIE ||
			problem_type == MEP_PROBLEM_REGRESSION) && num_program_outputs > 1)
			strcat(prog, "Function mepx(x As Range) As Double()");
		else
			strcat(prog, "Function mepx(x As Range) As Double");
	
	strcat(prog, "\n");

	if (problem_type == MEP_PROBLEM_MULTICLASS_CLASSIFICATION){
		// here print the class labels
		strcat(prog, "\n");
		snprintf(tmp_s, 100, "  Dim class_labels(%u) as Integer\n", num_classes - 1);
		strcat(prog, tmp_s);
		for (unsigned int i = 0; i < num_classes; i++) {
			snprintf(tmp_s, 100, "  class_labels(%u) = %d\n", i, class_labels[i]);
			strcat(prog, tmp_s);
		}
		strcat(prog, "\n");
	}
	

	if (simplified) {
// code
		print_MEP_code_Excel_VBA(prog, simplified_prg, num_utilized_genes);
		// output
		print_output_calculation_Excel_VBA(prog, num_utilized_genes, index_best_genes_simplified);
	}
	else {// not simplified
		print_MEP_code_Excel_VBA(prog, prg, code_length);
		// output
		print_output_calculation_Excel_VBA(prog, code_length, index_best_genes);
	}
	strcat(prog, "\n");
	strcat(prog, "End Function\n");

	setlocale(LC_NUMERIC, saved_locale);
	free (saved_locale);

	return prog;
}
//---------------------------------------------------------------------------
