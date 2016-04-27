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
#define O_INPUTS_AVERAGE -27
#define O_NUM_INPUTS -28


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

#define MAX_OPERATORS 28

//-----------------------------------------------------------------
class t_mep_operators
{
    public:
    bool use_addition, use_subtraction, use_multiplication, use_division, use_power, use_sqrt, use_exp, use_pow10, use_ln, use_log10, use_log2, use_floor, use_ceil, use_abs, use_inv, use_x2, use_min, use_max;
	bool use_inputs_average;
	bool use_num_inputs;
//	bool use_mod, use_mod10, use_mod2;
	bool use_not, use_and, use_or, use_xor, use_nand, use_nor;
    bool use_sin, use_cos, use_tan, use_asin, use_acos, use_atan;
	// if lower than 0 then
	bool use_iflz;
	bool use_ifalbcd;

	bool modified;

	t_mep_operators(void);
	
	void init(void);

    int to_xml(pugi::xml_node parent);
    int from_xml(pugi::xml_node parent);

    int get_list_of_operators(int *actual_operators);

	// returns true if the addition operator is enabled
	bool get_addition(void);

	// returns true if the subtraction operator is enabled
	bool get_subtraction(void);

	// returns true if the multiplication operator is enabled
	bool get_multiplication(void);

	// returns true if the division operator is enabled
	bool get_division(void);

	// returns true if the power operator is enabled
	bool get_power(void);

	// returns true if the sqrt operator is enabled
	bool get_sqrt(void);

	// returns true if the exp operator is enabled
	bool get_exp(void);

	// returns true if the pow10 operator is enabled
	bool get_pow10(void);

	// returns true if the ln (natural logarithm) operator is enabled
	bool get_ln(void);

	// returns true if the log10 (log in base 10) operator is enabled
	bool get_log10(void);

	// returns true if the log2 (log in base 2) operator is enabled
	bool get_log2(void);

	// returns true if the floor operator is enabled
	bool get_floor(void);

	// returns true if the ceil operator is enabled
	bool get_ceil(void);

	// returns true if the absolute operator is enabled
	bool get_abs(void);

	// returns true if the inv (1/x) operator is enabled
	bool get_inv(void);

	// returns true if the x^2 operator is enabled
	bool get_x2(void);

	// returns true if the min (x, y) operator is enabled
	bool get_min(void);

	// returns true if the max(x, y) operator is enabled
	bool get_max(void);

	// returns true if the sin operator is enabled
	bool get_sin(void);

	// returns true if the cos operator is enabled
	bool get_cos(void);

	// returns true if the tan operator is enabled
	bool get_tan(void);

	// returns true if the asin operator is enabled
	bool get_asin(void);

	// returns true if the acos operator is enabled
	bool get_acos(void);

	// returns true if the atan operator is enabled
	bool get_atan(void);

	// returns true if the "if lower than zero" operator is enabled
	bool get_iflz(void);

	// returns true if the "if a lower than b returns c else returns d" operator is enabled
	bool get_ifalbcd(void);

	//bool get_inputs_average(void);
	//bool get_num_inputs(void);



	// sets if the addition operator is enabled or not
	void set_addition(bool value);

	// sets if the subtraction operator is enabled or not
	void set_subtraction(bool value);

	// sets if the multiplication operator is enabled or not
	void set_multiplication(bool value);

	// sets if the division operator is enabled or not
	void set_division(bool value);

	// sets if the power operator is enabled or not
	void set_power(bool value);

	// sets if the sqrt operator is enabled or not
	void set_sqrt(bool value);

	// sets if the exp operator is enabled or not
	void set_exp(bool value);

	// sets if the pow10 operator is enabled or not
	void set_pow10(bool value);

	// sets if the ln operator is enabled or not
	void set_ln(bool value);

	// sets if the log10 operator is enabled or not
	void set_log10(bool value);

	// sets if the log2 operator is enabled or not
	void set_log2(bool value);

	// sets if the floor operator is enabled or not
	void set_floor(bool value);

	// sets if the ceil operator is enabled or not
	void set_ceil(bool value);

	// sets if the abs operator is enabled or not
	void set_abs(bool value);

	// sets if the inv (1/x) operator is enabled or not
	void set_inv(bool value);

	// sets if the x^2 operator is enabled or not
	void set_x2(bool value);

	// sets if the min operator is enabled or not
	void set_min(bool value);

	// sets if the max operator is enabled or not
	void set_max(bool value);

	// sets if the sin operator is enabled or not
	void set_sin(bool value);

	// sets if the cos operator is enabled or not
	void set_cos(bool value);

	// sets if the tan operator is enabled or not
	void set_tan(bool value);

	// sets if the asin operator is enabled or not
	void set_asin(bool value);

	// sets if the acos operator is enabled or not
	void set_acos(bool value);

	// sets if the atan operator is enabled or not
	void set_atan(bool value);

	// sets if the "if less than zero" operator is enabled or not
	void set_iflz(bool value);

	// sets if the "if a lower than b returns c else returns d" operator is enabled or not
	void set_ifalbcd(bool value);

	//void set_inputs_average(bool value);
	//void set_num_inputs(bool value);

	// returns true if the state of some operator is modified
	bool is_modified(void);

};
//-----------------------------------------------------------------


#endif // OPERATORS_CLASS_H_INCLUDED
