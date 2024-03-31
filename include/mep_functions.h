// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef mep_functions_H
#define mep_functions_H
//-----------------------------------------------------------------
#include "utils/pugixml.hpp"
//-----------------------------------------------------------------
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
#define O_LOG2 -11
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
#define O_NEG -29

#define O_IF_A_OR_B_CD -30
#define O_IF_A_XOR_B_CD -31
#define O_FMOD -32


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

#define MEP_MAX_OPERATORS 32
#define MEP_DIVISION_PROTECT 1E-10
//#define MEP_MIN_PROTECT 1E-10
//#define MEP_MAX_PROTECT 1E10

//-----------------------------------------------------------------
class t_mep_functions
{
    public:

    bool use_addition, use_subtraction, use_multiplication, use_division, 
		use_power, use_sqrt, use_exp, use_pow10,
	use_ln, use_log10, use_log2, 
		use_floor, use_ceil, use_abs, use_inv, use_x2, use_min, use_max, use_neg;
	bool use_inputs_average;
	bool use_num_inputs;
//	bool use_mod, use_mod10, use_mod2;
	bool use_not, use_and, use_or, use_xor, use_nand, use_nor;
    bool use_sin, use_cos, use_tan, use_asin, use_acos, use_atan;
	// if lower than 0 then
	bool use_iflz;
	bool use_ifalbcd;
	bool use_if_a_or_b_cd;
	bool use_if_a_xor_b_cd;
	bool use_fmod;

	bool modified;

	t_mep_functions(void);
	bool operator==(const t_mep_functions& other) const;
	
	void init(void);

    int to_xml(pugi::xml_node parent);
    int from_xml(pugi::xml_node parent);

	unsigned int get_list_of_operators(int *actual_operators);
	unsigned int count_operators(void) const;

	// returns true if the addition operator is used
	bool get_addition(void)const;
	bool get_addition_enabled(int data_type)const;

	// returns true if the subtraction operator is used
	bool get_subtraction(void)const;
	bool get_subtraction_enabled(int data_type)const;

	// returns true if the multiplication operator is used
	bool get_multiplication(void)const;
	bool get_multiplication_enabled(int data_type)const;

	// returns true if the division operator is used
	bool get_division(void)const;
	bool get_division_enabled(int data_type)const;

	// returns true if the power operator is used
	bool get_power(void)const;
	bool get_power_enabled(int data_type)const;

	// returns true if the sqrt operator is used
	bool get_sqrt(void)const;
	bool get_sqrt_enabled(int data_type)const;

	// returns true if the exp operator is used
	bool get_exp(void)const;
	bool get_exp_enabled(int data_type)const;

	// returns true if the pow10 operator is used
	bool get_pow10(void)const;
	bool get_pow10_enabled(int data_type)const;

	// returns true if the ln (natural logarithm) operator is used
	bool get_ln(void)const;
	bool get_ln_enabled(int data_type)const;

	// returns true if the log10 (log in base 10) operator is used
	bool get_log10(void)const;
	bool get_log10_enabled(int data_type)const;

	// returns true if the log2 (log in base 2) operator is used
	bool get_log2(void)const;
	bool get_log2_enabled(int data_type)const;

	// returns true if the floor operator is used
	bool get_floor(void)const;
	bool get_floor_enabled(int data_type)const;

	// returns true if the ceil operator is used
	bool get_ceil(void)const;
	bool get_ceil_enabled(int data_type)const;

	// returns true if the absolute operator is used
	bool get_abs(void)const;
	bool get_abs_enabled(int data_type)const;

	// returns true if the inv (1/x) operator is used
	bool get_inv(void)const;
	bool get_inv_enabled(int data_type)const;

	// returns true if the neg -x operator is used
	bool get_neg(void)const;
	bool get_neg_enabled(int data_type)const;

	// returns true if the x^2 operator is used
	bool get_x2(void)const;
	bool get_x2_enabled(int data_type)const;

	// returns true if the min (x, y) operator is used
	bool get_min(void)const;
	bool get_min_enabled(int data_type)const;

	// returns true if the max(x, y) operator is used
	bool get_max(void)const;
	bool get_max_enabled(int data_type)const;

	// returns true if the sin operator is used
	bool get_sin(void)const;
	bool get_sin_enabled(int data_type)const;

	// returns true if the cos operator is used
	bool get_cos(void)const;
	bool get_cos_enabled(int data_type)const;

	// returns true if the tan operator is used
	bool get_tan(void)const;
	bool get_tan_enabled(int data_type)const;

	// returns true if the asin operator is used
	bool get_asin(void)const;
	bool get_asin_enabled(int data_type)const;

	// returns true if the acos operator is used
	bool get_acos(void)const;
	bool get_acos_enabled(int data_type)const;

	// returns true if the atan operator is used
	bool get_atan(void)const;
	bool get_atan_enabled(int data_type)const;

	// returns true if the "if lower than zero" operator is used
	bool get_iflz(void)const;
	bool get_iflz_enabled(int data_type)const;

	// returns true if the "if a lower than b returns c else returns d" operator is used
	bool get_ifalbcd(void)const;
	bool get_ifalbcd_enabled(int data_type)const;

	// returns true if the "if a < 0 or b < 0 returns c else returns d" operator is used
	bool get_if_a_or_b_cd(void)const;
	bool get_if_a_or_b_cd_enabled(int data_type)const;

	// returns true if the "if a < 0 xor b < 0 returns c else returns d" operator is used
	bool get_if_a_xor_b_cd(void)const;
	bool get_if_a_xor_b_cd_enabled(int data_type)const;

	// returns true if the "fmod(x, y)" operator is used
	bool get_mod(void)const;
	bool get_mod_enabled(int data_type)const;

	//bool get_inputs_average(void);
	//bool get_num_inputs(void);

	// sets if the addition operator is used or not
	void set_addition(bool value);

	// sets if the subtraction operator is used or not
	void set_subtraction(bool value);

	// sets if the multiplication operator is used or not
	void set_multiplication(bool value);

	// sets if the division operator is used or not
	void set_division(bool value);

	// sets if the power operator is used or not
	void set_power(bool value);

	// sets if the sqrt operator is used or not
	void set_sqrt(bool value);

	// sets if the exp operator is used or not
	void set_exp(bool value);

	// sets if the pow10 operator is used or not
	void set_pow10(bool value);

	// sets if the ln operator is used or not
	void set_ln(bool value);

	// sets if the log10 operator is used or not
	void set_log10(bool value);

	// sets if the log2 operator is used or not
	void set_log2(bool value);

	// sets if the floor operator is used or not
	void set_floor(bool value);

	// sets if the ceil operator is used or not
	void set_ceil(bool value);

	// sets if the abs operator is used or not
	void set_abs(bool value);

	// sets if the inv (1/x) operator is used or not
	void set_inv(bool value);

	// sets if the neg -x operator is used or not
	void set_neg(bool value);

	// sets if the x^2 operator is used or not
	void set_x2(bool value);

	// sets if the min operator is used or not
	void set_min(bool value);

	// sets if the max operator is used or not
	void set_max(bool value);

	// sets if the sin operator is used or not
	void set_sin(bool value);

	// sets if the cos operator is used or not
	void set_cos(bool value);

	// sets if the tan operator is used or not
	void set_tan(bool value);

	// sets if the asin operator is used or not
	void set_asin(bool value);

	// sets if the acos operator is used or not
	void set_acos(bool value);

	// sets if the atan operator is used or not
	void set_atan(bool value);

	// sets if the "if less than zero" operator is used or not
	void set_iflz(bool value);

	// sets if the "if a lower than b returns c else returns d" operator is used or not
	void set_ifalbcd(bool value);

	// sets if the "if a < 0 or b < 0 returns c else returns d" operator is used or not
	void set_if_a_or_b_cd(bool value);

	// sets if the "if a < 0 xor b < 0 returns c else returns d" operator is used or not
	void set_if_a_xor_b_cd(bool value);

	// sets if the "fmod(x, y)" operator is used or not
	void set_mod(bool value);

	//void set_inputs_average(bool value);
	//void set_num_inputs(bool value);

	// returns true if the state of some operator is modified
	bool is_modified(void) const;
};
//-----------------------------------------------------------------
#endif // OPERATORS_CLASS_H_INCLUDED
