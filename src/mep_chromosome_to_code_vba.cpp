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
void print_instruction_to_Basic(int op, unsigned int adr1, unsigned int adr2, unsigned int adr3, unsigned int adr4, char* tmp_s)
{
	switch (op) {
	case O_ADDITION:
		sprintf(tmp_s, "prg(%u) + prg(%u)", adr1, adr2);
		break;
	case O_SUBTRACTION:
		sprintf(tmp_s, "prg(%u) - prg(%u)", adr1, adr2);
		break;
	case O_MULTIPLICATION:
		sprintf(tmp_s, "prg(%u) * prg(%u)", adr1, adr2);
		break;
	case O_DIVISION:
		sprintf(tmp_s, "prg(%u) / prg(%u)", adr1, adr2);
		break;
	case O_POWER:
		sprintf(tmp_s, "prg(%u) ^ prg(%u)", adr1, adr2);
		break;
	case O_SQRT:
		sprintf(tmp_s, "Sqr(prg(%u))", adr1);
		break;
	case O_EXP:
		sprintf(tmp_s, "Exp(prg(%u))", adr1);
		break;
	case O_POW10:
		sprintf(tmp_s, "10 ^prg(%u)", adr1);
		break;
	case O_LN:
		sprintf(tmp_s, "Log(prg(%u))", adr1);
		break;
	case O_LOG10:
		sprintf(tmp_s, "Log(prg(%u)) / Log(10#)", adr1);
		break;
	case O_lOG2:
		sprintf(tmp_s, "Log(prg(%u)) / Log(2#)", adr1);
		break;
	case O_FLOOR:
		sprintf(tmp_s, "Floor_Precise(prg(%u))", adr1);
		break;
	case O_CEIL:
		sprintf(tmp_s, "Ceil_Precise(prg(%u))", adr1);
		break;
	case O_ABS:
		sprintf(tmp_s, "Abs(prg(%u))", adr1);
		break;
	case O_INV:
		sprintf(tmp_s, "1/prg(%u)", adr1);
		break;
	case O_NEG:
		sprintf(tmp_s, "-prg(%u)", adr1);
		break;
	case O_X2:
		sprintf(tmp_s, "prg(%u) * prg(%u)", adr1, adr1);
		break;
	case O_MIN:
		sprintf(tmp_s, "IIf (prg(%u) < prg(%u), prg(%u), prg(%u)) ' min", adr1, adr2, adr1, adr2);
		break;
	case O_MAX:
		sprintf(tmp_s, "IIf (prg(%u) > prg(%u), prg(%u), prg(%u)) ' max", adr1, adr2, adr1, adr2);
		break;
	case O_SIN:
		sprintf(tmp_s, "Sin(prg(%u))", adr1);
		break;
	case O_COS:
		sprintf(tmp_s, "Cos(prg(%u))", adr1);
		break;
	case O_TAN:
		sprintf(tmp_s, "Tan(prg(%u))", adr1);
		break;
	case O_ASIN:
		sprintf(tmp_s, "Asin(prg(%u))", adr1);
		break;
	case O_ACOS:
		sprintf(tmp_s, "Acos(prg(%u))", adr1);
		break;
	case O_ATAN:
		sprintf(tmp_s, "Atan(prg(%u))", adr1);
		break;
	case O_IFLZ:
		sprintf(tmp_s, "IIf (prg(%u) < 0, prg(%u), prg(%u)) ' ifalzbc", adr1, adr2, adr3);
		break;
	case O_IFALBCD:
		sprintf(tmp_s, "IIf (prg(%u) < prg(%u), prg(%u), prg(%u)) ' ifalbcd", adr1, adr2, adr3, adr4);
		break;
	case O_IF_A_OR_B_CD:
		sprintf(tmp_s, "IIf (prg(%u) < 0 Or prg(%u) < 0, prg(%u), prg(%u)) ' if_a_or_b_cd", adr1, adr2, adr3, adr4);
		break;
	case O_IF_A_XOR_B_CD:
		sprintf(tmp_s, "IIf (prg(%u) < 0 <> prg(%u) < 0, prg(%u), prg(%u)) ' if_a_xor_b_cd", adr1, adr2, adr3, adr4);
		break;

	case O_NUM_INPUTS:
		sprintf(tmp_s, "%d ' num inputs", 0);
		break;

	case O_INPUTS_AVERAGE:
		sprintf(tmp_s, "%lf ' inputs average", 0.0);
		break;
	case O_FMOD:
		sprintf(tmp_s, "prg(%u) Mod prg(%u)", adr1, adr2);
		break;
	}
}
//---------------------------------------------------------------------------
char* t_mep_chromosome::to_Excel_VBA_function_double(bool simplified, double*,
	unsigned int problem_type, unsigned int error_measure)
{
	setlocale(LC_NUMERIC, "C");

	char* prog = new char[(code_length + num_constants + num_total_variables) * 100 + 2000];
	char tmp_s[100];
	prog[0] = 0;

	strcat(prog, "Function mepx(x As Range) As Double");
	strcat(prog, "\n");

	// here I have to declare the constants
	if (num_constants) {
		strcat(prog, "'constants ...");

		strcat(prog, "\n");

		sprintf(tmp_s, "  Dim constants(%u) As Double", num_constants - 1);// -1 because in VB the size is actual the last index
		strcat(prog, tmp_s);
		strcat(prog, "\n");
		for (unsigned int i = 0; i < num_constants; i++) {
			sprintf(tmp_s, "  constants(%u) = %lf", i, real_constants[i]);
			strcat(prog, tmp_s);
			strcat(prog, "\n");
		}
		strcat(prog, "\n");
	}

	if (simplified) {
		sprintf(tmp_s, "  Dim prg(%u) as Double\n", num_utilized_genes - 1);
		strcat(prog, tmp_s);

		for (unsigned int i = 0; i < num_utilized_genes; i++) {
			sprintf(tmp_s, "  prg(%u) = ", i);
			strcat(prog, tmp_s);

			if (simplified_prg[i].op < 0) {
				print_instruction_to_Basic(simplified_prg[i].op, simplified_prg[i].addr1, simplified_prg[i].addr2, simplified_prg[i].addr3, simplified_prg[i].addr4, tmp_s);
				strcat(prog, tmp_s);
				strcat(prog, "\n");
			}
			else { // a variable
				if (simplified_prg[i].op < (int)num_total_variables) {
					if (problem_type == MEP_PROBLEM_TIME_SERIE)
						sprintf(tmp_s, "x(%d,1).Value", simplified_prg[i].op + 1); // +1 because in VB all ranges are from 1
					else
						sprintf(tmp_s, "x(1,%d).Value", simplified_prg[i].op + 1); // +1 because in VB all ranges are from 1

					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
				else {
					sprintf(tmp_s, "constants(%u)", (unsigned int)simplified_prg[i].op - num_total_variables);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
			}
		}

		strcat(prog, "\n");
		switch (problem_type) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_TIME_SERIE:
			sprintf(tmp_s, "  mepx = prg(%u)", num_utilized_genes - 1);
			strcat(prog, tmp_s);
			break;
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			sprintf(tmp_s, "  If (prg(%u) <= %lf) Then\n   mepx = 0\n  Else\n    mepx = 1\n Endif\n", num_utilized_genes - 1, best_class_threshold);
			strcat(prog, tmp_s);
			break;
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			if (error_measure == MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR) {
				sprintf(tmp_s, "  Dim centers(%u) as Double\n", num_classes);
				strcat(prog, tmp_s);

				for (unsigned int c = 0; c < num_classes; c++) {
					sprintf(tmp_s, "  centers(%u) = %lf\n", c, centers[c]);
					strcat(prog, tmp_s);
				}

				strcat(prog, "  Dim min_dist as Double\n");
				sprintf(tmp_s, "  min_dist = Abs(prg(%u) - centers(0))\n", num_utilized_genes - 1);
				strcat(prog, tmp_s);

				strcat(prog, "  Dim closest_class_index as Integer\n");
				strcat(prog, "  closest_class_index = 0\n");
				sprintf(tmp_s, "  For c = 1 To %u\n", num_classes - 1);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "    If min_dist > Abs(prg(%u) - centers(c)) Then\n", num_utilized_genes - 1);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "      min_dist = Abs(prg(%u) - centers(c))\n", num_utilized_genes - 1);
				strcat(prog, tmp_s);

				strcat(prog, "      closest_class_index = c\n");
				strcat(prog, "    Endif\n");
				strcat(prog, "  Next\n");
				strcat(prog, "  mepx = closest_class_index\n");
			}
			else {
				// should never be here
			}
			break;
		}

	}
	else {// not simplified
		strcat(prog, "  Dim prg(");
		sprintf(tmp_s, "%u", code_length - 1);
		strcat(prog, tmp_s);
		strcat(prog, ") As Double");
		strcat(prog, "\n");

		for (unsigned int i = 0; i < code_length; i++) {
			sprintf(tmp_s, "  prg(%u) = ", i);
			strcat(prog, tmp_s);

			if (prg[i].op < 0) {
				print_instruction_to_Basic(prg[i].op, prg[i].addr1, prg[i].addr2, prg[i].addr3, prg[i].addr4, tmp_s);
				strcat(prog, tmp_s);
				strcat(prog, "\n");
			}
			else { // a variable
				if (prg[i].op < (int)num_total_variables) {
					if (problem_type == MEP_PROBLEM_TIME_SERIE)
						sprintf(tmp_s, "x(%d,1).Value", prg[i].op + 1); // +1 because all ranges in VB start from 1
					else
						sprintf(tmp_s, "x(1,%d).Value", prg[i].op + 1); // +1 because all ranges in VB start from 1

					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
				else {
					sprintf(tmp_s, "constants(%u)", (unsigned int)prg[i].op - num_total_variables);
					strcat(prog, tmp_s);
					strcat(prog, "\n");
				}
			}
		}

		strcat(prog, "\n");
		switch (problem_type) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_TIME_SERIE:
			sprintf(tmp_s, "  mepx = prg(%u)", index_best_genes[0]);
			strcat(prog, tmp_s);
			break;

		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			sprintf(tmp_s, "  If (prg(%u) <= %lg) Then\n    mepx = 0\n  Else\n    mepx = 1\n  End If\n", index_best_genes[0], best_class_threshold);
			strcat(prog, tmp_s);
			break;
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			switch (error_measure) {
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
			case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
				sprintf(tmp_s, "' find maximal value and divide index by num classes\n");
				strcat(prog, tmp_s);

				strcat(prog, "  max_value = prg(0)\n");
				strcat(prog, "  index_max_value = 0\n");
				sprintf(tmp_s, "  For i = 1 To %u\n", code_length - 1);
				strcat(prog, tmp_s);

				strcat(prog, "    If max_value < prg(i) Then\n      max_value = prg(i)\n      index_max_value = i\n    EndIf\n");
				strcat(prog, "  Next\n");

				sprintf(tmp_s, "  mepx = index_max_value Mod %u\n", num_classes);
				strcat(prog, tmp_s);
				break;
			case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:

				sprintf(tmp_s, "' index of genes holding the output for each class\n");
				strcat(prog, tmp_s);

				sprintf(tmp_s, "  Dim index_best_genes(%u) As Integer\n", num_classes - 1);
				strcat(prog, tmp_s);
				for (unsigned int c = 0; c < num_classes; c++) {
					sprintf(tmp_s, "  index_best_genes(%u) = %u\n", c, index_best_genes[c]);
					strcat(prog, tmp_s);
				}

				sprintf(tmp_s, "' find maximal value\n");
				strcat(prog, tmp_s);

				strcat(prog, "  max_value = prg(0)\n");
				strcat(prog, "  index_max_value = 0\n");
				sprintf(tmp_s, "  For i = 1 To %u\n", code_length - 1);
				strcat(prog, tmp_s);

				strcat(prog, "    If max_value < prg(i) Then\n      max_value = prg(i)\n      index_max_value = i\n    EndIf\n");
				strcat(prog, "  Next\n");

				strcat(prog, "  ' find the class\n");
				sprintf(tmp_s, "  class_index = %u\n", num_classes);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "  For c = 0 To %u\n", num_classes - 1);

				strcat(prog, tmp_s);

				strcat(prog, "    If index_best_genes(c) = index_max_value Then\n      class_index = c\n      Exit For\n    EndIf\n");
				strcat(prog, "  Next\n");

				sprintf(tmp_s, "  If class_index = %u Then\n", num_classes);
				strcat(prog, tmp_s);

				strcat(prog, "    ' find the closest max\n");
				strcat(prog, "    min_dist = Abs(max_value - prg(index_best_genes(0)))\n");
				strcat(prog, "    class_index = 0\n");
				sprintf(tmp_s, "    For c = 1 To %u\n", num_classes - 1);
				strcat(prog, tmp_s);

				strcat(prog, "      If min_dist > Abs(max_value - prg(index_best_genes(c))) Then\n");
				strcat(prog, "        min_dist = Abs(max_value - prg(index_best_genes(c)))\n");

				strcat(prog, "        class_index = c\n");
				strcat(prog, "      EndIf\n");
				strcat(prog, "    Next\n");

				strcat(prog, "  EndIf\n");
				sprintf(tmp_s, "  mepx = class_index\n");
				strcat(prog, tmp_s);
				break;
			case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:
				sprintf(tmp_s, "  Dim centers(%u) as Double\n", num_classes);
				strcat(prog, tmp_s);

				for (unsigned int c = 0; c < num_classes; c++) {
					sprintf(tmp_s, "  centers(%u) = %lf\n", c, centers[c]);
					strcat(prog, tmp_s);
				}

				strcat(prog, "  Dim min_dist as Double\n");
				sprintf(tmp_s, "  min_dist = Abs(prg(%u) - centers(0))\n", index_best_genes[0]);
				strcat(prog, tmp_s);

				strcat(prog, "  Dim closest_class_index as Integer\n");
				strcat(prog, "  closest_class_index = 0\n");
				sprintf(tmp_s, "  For c = 1 To %u\n", num_classes - 1);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "    If min_dist > Abs(prg(%u) - centers(c)) Then\n", index_best_genes[0]);
				strcat(prog, tmp_s);

				sprintf(tmp_s, "      min_dist = Abs(prg(%u) - centers(c))\n", index_best_genes[0]);
				strcat(prog, tmp_s);

				strcat(prog, "      closest_class_index = c\n");
				strcat(prog, "    Endif\n");
				strcat(prog, "  Next\n");
				strcat(prog, "  mepx = closest_class_index\n");

				break;
			}

			break;
		}// end switch error_measure

	}
	strcat(prog, "\n");
	strcat(prog, "End Function\n");

	setlocale(LC_NUMERIC, "");
	return prog;
}
//---------------------------------------------------------------------------