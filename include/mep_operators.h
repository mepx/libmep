#ifndef OPERATORS_CLASS_H_INCLUDED
#define OPERATORS_CLASS_H_INCLUDED

#include "pugixml.hpp"


#define O_ADDITION -1
#define O_SUBTRACTION -2
#define O_MULTIPLICATION -3
#define O_DIVISION -4
#define O_POWER -5
#define O_SQRT -6
#define O_EXP -7
#define O_POW10 -8
#define O_LN -9
#define O_LOG10 -10
#define O_lOG2 -11
#define O_FLOOR -12
#define O_CEIL -13
#define O_ABS -14
#define O_INV -15
#define O_X2 -16
#define O_MIN -17
#define O_MAX -18
#define O_SIN -19
#define O_COS -20
#define O_TAN -21
#define O_ASIN -22
#define O_ACOS -23
#define O_ATAN -24
#define O_IFLZ -25
#define O_IFALBCD -26


/*
#define O_MOD -19
#define O_MOD10 -20
#define O_MOD2 -21

#define O_NOT -22
#define O_AND -23
#define O_OR -24
#define O_XOR -25
#define O_NAND -26
#define O_NOR -27
*/

#define MAX_OPERATORS 26

//-----------------------------------------------------------------
class t_mep_operators
{
    public:
    bool use_addition, use_subtraction, use_multiplication, use_division, use_power, use_sqrt, use_exp, use_pow10, use_ln, use_log10, use_log2, use_floor, use_ceil, use_abs, use_inv, use_x2, use_min, use_max;
//	bool use_mod, use_mod10, use_mod2;
	bool use_not, use_and, use_or, use_xor, use_nand, use_nor;
    bool use_sin, use_cos, use_tan, use_asin, use_acos, use_atan;
	// if lower than 0 then
	bool use_iflz;
		bool use_ifalbcd;

	t_mep_operators(void);

	void init(void);

    int to_xml(pugi::xml_node parent);
    int from_xml(pugi::xml_node parent);

    int get_list_of_operators(int *actual_operators);
};
//-----------------------------------------------------------------


#endif // OPERATORS_CLASS_H_INCLUDED
