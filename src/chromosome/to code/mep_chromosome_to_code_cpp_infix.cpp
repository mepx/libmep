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
#include "utils/func_utils.h"
//---------------------------------------------------------------------------------
void t_mep_chromosome::code_to_infix_C(unsigned int mep_index,
									   char*& s_prg,
									   size_t &capacity)
{
	switch (prg[mep_index].op) {
	case O_ADDITION:
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 1);
		strcat(s_prg, "+");
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
		break;
	case O_SUBTRACTION:
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 1);
		strcat(s_prg, "-");
		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
		if (prg[prg[mep_index].addr2].op < 0) {
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		break;
	case O_MULTIPLICATION:
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "*");
		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		break;
	case O_DIVISION:
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "/");
		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		break;
	case O_POWER:
		increase_string_capacity2(s_prg, capacity, 5);
		strcat(s_prg, "pow(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ",");
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_SQRT:
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "sqrt(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_EXP:
		increase_string_capacity2(s_prg, capacity, 5);
		strcat(s_prg, "exp(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_POW10:
		increase_string_capacity2(s_prg, capacity, 8);
		strcat(s_prg, "pow(10,");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_LN:
		increase_string_capacity2(s_prg, capacity, 5);
		strcat(s_prg, "log(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_LOG10:
		increase_string_capacity2(s_prg, capacity, 7);
		strcat(s_prg, "log10(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_LOG2:
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "log2(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_FLOOR:
		increase_string_capacity2(s_prg, capacity, 7);
		strcat(s_prg, "floor(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_CEIL:
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "ceil(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_ABS:
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "fabs(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_INV:
		increase_string_capacity2(s_prg, capacity, 3);
		strcat(s_prg, "1/");
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		break;
	case O_NEG:
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "-");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		break;
	case O_X2:
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "*");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		break;
	case O_MIN:
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "(");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "<");

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "?");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ":");

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");

		break;
	case O_MAX:
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "(");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ">");

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "?");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ":");

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");

		break;
	case O_SIN:
		increase_string_capacity2(s_prg, capacity, 5);
		strcat(s_prg, "sin(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_COS:
		increase_string_capacity2(s_prg, capacity, 5);
		strcat(s_prg, "cos(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_TAN:
		increase_string_capacity2(s_prg, capacity, 5);
		strcat(s_prg, "tan(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_ASIN:
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "asin(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_ACOS:
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "acos(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_ATAN:
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "atan(");
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_IFLZ:
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "(");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		increase_string_capacity2(s_prg, capacity, 4);
		strcat(s_prg, "<0?");

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ":");

		if (prg[prg[mep_index].addr3].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr3, s_prg, capacity);
		if (prg[prg[mep_index].addr3].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");

		break;
	case O_IFALBCD:
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "(");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "<");

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "?");

		if (prg[prg[mep_index].addr3].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr3, s_prg, capacity);

		if (prg[prg[mep_index].addr3].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ":");

		if (prg[prg[mep_index].addr4].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr4, s_prg, capacity);
		if (prg[prg[mep_index].addr4].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");

		break;
	case O_IF_A_OR_B_CD:
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "(");

		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "<0 ||");

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		increase_string_capacity2(s_prg, capacity, 4);
		strcat(s_prg, "<0?");

		if (prg[prg[mep_index].addr3].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr3, s_prg, capacity);

		if (prg[prg[mep_index].addr3].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ":");

		if (prg[prg[mep_index].addr4].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr4, s_prg, capacity);
		if (prg[prg[mep_index].addr4].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");

		break;
	case O_IF_A_XOR_B_CD:
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, "(");
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
		if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 6);
		strcat(s_prg, "<0 !=");

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}
		code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);

		if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		increase_string_capacity2(s_prg, capacity, 4);
		strcat(s_prg, "<0?");

		if (prg[prg[mep_index].addr3].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr3, s_prg, capacity);

		if (prg[prg[mep_index].addr3].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ":");

		if (prg[prg[mep_index].addr4].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "(");
		}

		code_to_infix_C(prg[mep_index].addr4, s_prg, capacity);
		if (prg[prg[mep_index].addr4].op < 0) { // it is an operator
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}
		increase_string_capacity2(s_prg, capacity, 2);
		strcat(s_prg, ")");
		break;
	case O_FMOD:
			
		if (data_type == MEP_DATA_LONG_LONG){
			if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
				increase_string_capacity2(s_prg, capacity, 2);
				strcat(s_prg, "(");
			}
			code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
			if (prg[prg[mep_index].addr1].op < 0) { // it is an operator
				increase_string_capacity2(s_prg, capacity, 2);
				strcat(s_prg, ")");
			}
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, "%");
			if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
				increase_string_capacity2(s_prg, capacity, 2);
				strcat(s_prg, "(");
			}
			code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
			if (prg[prg[mep_index].addr2].op < 0) { // it is an operator
				increase_string_capacity2(s_prg, capacity, 2);
				strcat(s_prg, ")");
			}
		}
		else{
			increase_string_capacity2(s_prg, capacity, 6);
			strcat(s_prg, "fmod(");
			code_to_infix_C(prg[mep_index].addr1, s_prg, capacity);
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ",");
			code_to_infix_C(prg[mep_index].addr2, s_prg, capacity);
			increase_string_capacity2(s_prg, capacity, 2);
			strcat(s_prg, ")");
		}

		break;

	case O_NUM_INPUTS:
		break;

	case O_INPUTS_AVERAGE:
		break;
	default:
		if (prg[mep_index].op >= 0) { // a variable or constant
			if (prg[mep_index].op < (int)num_total_variables) {
				char tmp_s[100];
				if (problem_type == MEP_PROBLEM_TIME_SERIE && num_program_outputs > 1)
					snprintf(tmp_s, 100, "x[%u][%u]",
							 (unsigned int)prg[mep_index].op / num_program_outputs,
							 (unsigned int)prg[mep_index].op % num_program_outputs);
				else
					snprintf(tmp_s, 100, "x[%u]", (unsigned int)prg[mep_index].op);
				size_t str_len = strlen(tmp_s);
				increase_string_capacity2(s_prg, capacity, str_len);
				strcat(s_prg, tmp_s);
			}
			else {
				char tmp_s[100];
				if (data_type == MEP_DATA_LONG_LONG)
					snprintf(tmp_s, 100, "%lld", long_constants[prg[mep_index].op - num_total_variables]);
				else
					snprintf(tmp_s, 100, "%lg", real_constants[prg[mep_index].op - num_total_variables]);

				size_t str_len = strlen(tmp_s);
				increase_string_capacity2(s_prg, capacity, str_len);
				strcat(s_prg, tmp_s);
			}
		}
	}
}
//---------------------------------------------------------------------------------
char* t_mep_chromosome::to_C_infix_code(
										  double **data_double,
										  long long** data_long,
										  const char* libmep_version)
{
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	char* saved_locale = strdup (old_locale);
	
	setlocale(LC_NUMERIC, "C");

	char* prog = new char[1000];
	char tmp_s[100];
	prog[0] = 0;
	snprintf(tmp_s, 100, "//Generated by libmep version %s\n\n", libmep_version);
	strcat(prog, tmp_s);
	strcat(prog, "#include <math.h>\n");
	strcat(prog, "#include <stdio.h>\n");
	strcat(prog, "\n");

	if (data_type == MEP_DATA_LONG_LONG){
		if (problem_type == MEP_PROBLEM_TIME_SERIE && num_program_outputs > 1)
			strcat(prog, "void mepx(long long **x /*inputs*/, long long *outputs)");
		else
			strcat(prog, "void mepx(long long *x /*inputs*/, long long *outputs)");
	}
	else{// double
		if (problem_type == MEP_PROBLEM_TIME_SERIE && num_program_outputs > 1)
			strcat(prog, "void mepx(double **x /*inputs*/, double *outputs)");
		else
			strcat(prog, "void mepx(double *x /*inputs*/, double *outputs)");
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

	size_t string_capacity = 1000;

	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_TIME_SERIE:
		for (unsigned int o = 0; o < num_program_outputs; o++){
			snprintf(tmp_s, 100, "outputs[%u] = ", o);

			strcat(prog, tmp_s);
			
			code_to_infix_C(index_best_genes[o], prog, string_capacity);
			strcat(prog, ";\n");
		}
		break;

	case MEP_PROBLEM_BINARY_CLASSIFICATION:
		strcat(prog, "  double result = ");

		code_to_infix_C(index_best_genes[0], prog, string_capacity);
		increase_string_capacity2(prog, string_capacity, 100);
		strcat(prog, ";\n");
		snprintf(tmp_s, 100, "  if (result <= %lg)\n    outputs[0] = %d;\n  else\n    outputs[0] = %d;",
				 best_class_threshold, class_labels[0], class_labels[1]);
		strcat(prog, tmp_s);
		break;
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		switch (error_measure) {
		case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR:
		case MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR:
		case MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR:
			strcat(prog, "// Currently there is no code generated for this case! Use the standard C++ code for this strategy.\n");
			break;

		case MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR:

			strcat(prog, "  double result = ");

			code_to_infix_C(index_best_genes[0], prog, string_capacity);

			increase_string_capacity2(prog, string_capacity,  100 + num_classes * 50);
			strcat(prog, ";\n");

			snprintf(tmp_s, 100, "  double centers[%u];\n", num_classes);
			strcat(prog, tmp_s);

			for (unsigned int c = 0; c < num_classes; c++) {
				snprintf(tmp_s, 100, "  centers[%u] = %lg;\n", c, centers[c]);
				strcat(prog, tmp_s);
			}
			increase_string_capacity2(prog, string_capacity,  1000);
			snprintf(tmp_s, 100, "  double min_dist = fabs(result - centers[0]);\n");
			strcat(prog, tmp_s);

			strcat(prog, "  unsigned int closest_class_index = 0;\n");
			snprintf(tmp_s, 100, "  for (unsigned int c = 1; c < %u; c++)\n", num_classes);
			strcat(prog, tmp_s);

			snprintf(tmp_s, 100, "    if (min_dist > fabs(result - centers[c])) {\n");
			strcat(prog, tmp_s);

			snprintf(tmp_s, 100, "      min_dist = fabs(result - centers[c]);\n");
			strcat(prog, tmp_s);

			strcat(prog, "      closest_class_index = c;\n");
			strcat(prog, "    }\n");
			strcat(prog, "  outputs[0] = class_labels[closest_class_index];\n");
			break;
		}// end switch error_measure
		break;
	} // end switch (problem_type)
	
	increase_string_capacity2(prog, string_capacity, 1000);
	strcat(prog, "\n");
	strcat(prog, "}\n");
	
	print_example_of_utilization_C(data_double, data_long, prog);

	setlocale(LC_NUMERIC, saved_locale);
	free (saved_locale);

	return prog;
}
//---------------------------------------------------------------------------
