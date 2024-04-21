// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "mep_functions.h"
#include "mep_data.h"
//---------------------------------------------------------------------------
t_mep_functions::t_mep_functions(void)
{
	init();
}
//---------------------------------------------------------------------------
void t_mep_functions::init(void)
{
	use_addition = use_subtraction = use_multiplication = true;
	use_division = use_power = use_sqrt = use_exp = use_pow10 = use_ln = use_log10 = use_log2 = use_floor = use_ceil = use_abs = use_inv = use_x2 = use_min = use_max =  use_neg = false;

	use_not = use_and = use_or = use_xor = use_nand = use_nor = false;

	use_sin = use_cos = use_tan = use_asin = use_acos = use_atan = false;

	use_iflz = use_ifalbcd = use_if_a_or_b_cd = use_if_a_xor_b_cd = false;

	use_fmod = false;

	use_inputs_average = false;
	use_num_inputs = false;

	modified = false;
}
//---------------------------------------------------------------------------
void t_mep_functions::to_xml_node(pugi::xml_node parent)
{
	char tmp_str[15];
	pugi::xml_node node = parent.append_child("use_addition");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_addition);
	data.set_value(tmp_str);

	node = parent.append_child("use_subtraction");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_subtraction);
	data.set_value(tmp_str);

	node = parent.append_child("use_multiplication");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_multiplication);
	data.set_value(tmp_str);

	node = parent.append_child("use_division");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_division);
	data.set_value(tmp_str);

	node = parent.append_child("use_power");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_power);
	data.set_value(tmp_str);

	node = parent.append_child("use_sqrt");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_sqrt);
	data.set_value(tmp_str);

	node = parent.append_child("use_exp");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_exp);
	data.set_value(tmp_str);

	node = parent.append_child("use_pow10");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_pow10);
	data.set_value(tmp_str);

	node = parent.append_child("use_ln");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_ln);
	data.set_value(tmp_str);

	node = parent.append_child("use_log10");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_log10);
	data.set_value(tmp_str);

	node = parent.append_child("use_log2");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_log2);
	data.set_value(tmp_str);

	node = parent.append_child("use_floor");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_floor);
	data.set_value(tmp_str);

	node = parent.append_child("use_ceil");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_ceil);
	data.set_value(tmp_str);

	node = parent.append_child("use_abs");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_abs);
	data.set_value(tmp_str);

	node = parent.append_child("use_inv");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_inv);
	data.set_value(tmp_str);

	node = parent.append_child("use_neg");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_neg);
	data.set_value(tmp_str);

	node = parent.append_child("use_x2");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_x2);
	data.set_value(tmp_str);

	node = parent.append_child("use_min");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_min);
	data.set_value(tmp_str);

	node = parent.append_child("use_max");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_max);
	data.set_value(tmp_str);

	node = parent.append_child("use_not");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_not);
	data.set_value(tmp_str);

	node = parent.append_child("use_and");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_and);
	data.set_value(tmp_str);

	node = parent.append_child("use_or");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_or);
	data.set_value(tmp_str);

	node = parent.append_child("use_xor");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_xor);
	data.set_value(tmp_str);

	node = parent.append_child("use_nand");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_nand);
	data.set_value(tmp_str);

	node = parent.append_child("use_nor");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_nor);
	data.set_value(tmp_str);

	node = parent.append_child("use_sin");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_sin);
	data.set_value(tmp_str);

	node = parent.append_child("use_cos");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_cos);
	data.set_value(tmp_str);

	node = parent.append_child("use_tan");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_tan);
	data.set_value(tmp_str);

	node = parent.append_child("use_asin");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_asin);
	data.set_value(tmp_str);

	node = parent.append_child("use_acos");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_acos);
	data.set_value(tmp_str);

	node = parent.append_child("use_atan");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_atan);
	data.set_value(tmp_str);

	node = parent.append_child("use_iflz");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_iflz);
	data.set_value(tmp_str);

	node = parent.append_child("use_ifalbcd");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_ifalbcd);
	data.set_value(tmp_str);

	node = parent.append_child("use_if_a_or_b_cd");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_if_a_or_b_cd);
	data.set_value(tmp_str);

	node = parent.append_child("use_if_a_xor_b_cd");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_if_a_xor_b_cd);
	data.set_value(tmp_str);

	node = parent.append_child("use_fmod");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 15, "%d", use_fmod);
	data.set_value(tmp_str);

	/*
	node = parent.append_child("use_num_inputs");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, "%d", use_num_inputs);
	data.set_value(tmp_str);

	node = parent.append_child("use_inputs_average");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, "%d", use_inputs_average);
	data.set_value(tmp_str);
	*/

	modified = false;
}
//---------------------------------------------------------------------------
bool t_mep_functions::from_xml_node(pugi::xml_node parent)
{
	pugi::xml_node node = parent.child("use_addition");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_addition = atoi(value_as_cstring) > 0;
	}
	else
		use_addition = false;

	node = parent.child("use_multiplication");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_multiplication = atoi(value_as_cstring) > 0;
	}
	else
		use_multiplication = false;

	node = parent.child("use_division");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_division = atoi(value_as_cstring) > 0;
	}
	else
		use_division = false;

	node = parent.child("use_subtraction");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_subtraction = atoi(value_as_cstring) > 0;
	}
	else
		use_subtraction = false;

	node = parent.child("use_power");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_power = atoi(value_as_cstring) > 0;
	}
	else
		use_power = false;

	node = parent.child("use_sqrt");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_sqrt = atoi(value_as_cstring) > 0;
	}
	else
		use_sqrt = false;

	node = parent.child("use_exp");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_exp = atoi(value_as_cstring) > 0;
	}
	else
		use_exp = false;

	node = parent.child("use_pow10");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_pow10 = atoi(value_as_cstring) > 0;
	}
	else
		use_pow10 = false;

	node = parent.child("use_ln");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_ln = atoi(value_as_cstring) > 0;
	}
	else
		use_ln = false;

	node = parent.child("use_log10");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_log10 = atoi(value_as_cstring) > 0;
	}
	else
		use_log10 = false;

	node = parent.child("use_log2");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_log2 = atoi(value_as_cstring) > 0;
	}
	else
		use_log2 = false;

	node = parent.child("use_floor");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_floor = atoi(value_as_cstring) > 0;
	}
	else
		use_floor = false;

	node = parent.child("use_ceil");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_ceil = atoi(value_as_cstring) > 0;
	}
	else
		use_ceil = false;

	node = parent.child("use_abs");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_abs = atoi(value_as_cstring) > 0;
	}
	else
		use_abs = false;

	node = parent.child("use_inv");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_inv = atoi(value_as_cstring) > 0;
	}
	else
		use_inv = false;

	node = parent.child("use_neg");
	if (node) {
		const char* value_as_cstring = node.child_value();
		use_neg = atoi(value_as_cstring) > 0;
	}
	else
		use_neg = false;

	node = parent.child("use_x2");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_x2 = atoi(value_as_cstring) > 0;
	}
	else
		use_x2 = false;

	node = parent.child("use_min");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_min = atoi(value_as_cstring) > 0;
	}
	else
		use_min = false;

	node = parent.child("use_max");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_max = atoi(value_as_cstring) > 0;
	}
	else
		use_max = false;

	node = parent.child("use_not");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_not = atoi(value_as_cstring) > 0;
	}
	else
		use_not = false;

	node = parent.child("use_and");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_and = atoi(value_as_cstring) > 0;
	}
	else
		use_and = false;

	node = parent.child("use_or");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_or = atoi(value_as_cstring) > 0;
	}
	else
		use_or = false;

	node = parent.child("use_xor");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_xor = atoi(value_as_cstring) > 0;
	}
	else
		use_xor = false;

	node = parent.child("use_nand");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_nand = atoi(value_as_cstring) > 0;
	}
	else
		use_nand = false;

	node = parent.child("use_nor");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_nor = atoi(value_as_cstring) > 0;
	}
	else
		use_nor = false;

	node = parent.child("use_sin");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_sin = atoi(value_as_cstring) > 0;
	}
	else
		use_sin = false;

	node = parent.child("use_cos");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_cos = atoi(value_as_cstring) > 0;
	}
	else
		use_cos = false;

	node = parent.child("use_tan");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_tan = atoi(value_as_cstring) > 0;
	}
	else
		use_tan = false;

	node = parent.child("use_asin");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_asin = atoi(value_as_cstring) > 0;
	}
	else
		use_asin = false;

	node = parent.child("use_acos");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_acos = atoi(value_as_cstring) > 0;
	}
	else
		use_acos = false;

	node = parent.child("use_atan");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_atan = atoi(value_as_cstring) > 0;
	}
	else
		use_atan = false;

	node = parent.child("use_iflz");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_iflz = atoi(value_as_cstring) > 0;
	}
	else
		use_iflz = false;

	node = parent.child("use_ifalbcd");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_ifalbcd = atoi(value_as_cstring) > 0;
	}
	else
		use_ifalbcd = false;

	node = parent.child("use_if_a_or_b_cd");
	if (node) {
		const char* value_as_cstring = node.child_value();
		use_if_a_or_b_cd = atoi(value_as_cstring) > 0;
	}
	else
		use_if_a_or_b_cd = false;

	node = parent.child("use_if_a_xor_b_cd");
	if (node) {
		const char* value_as_cstring = node.child_value();
		use_if_a_xor_b_cd = atoi(value_as_cstring) > 0;
	}
	else
		use_if_a_xor_b_cd = false;

	node = parent.child("use_fmod");
	if (node) {
		const char* value_as_cstring = node.child_value();
		use_fmod = atoi(value_as_cstring) > 0;
	}
	else
		use_fmod = false;

	/*
	node = parent.child("use_num_inputs");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_num_inputs = atoi(value_as_cstring) > 0;
	}
	else
		use_num_inputs = false;

	node = parent.child("use_inputs_average");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_inputs_average = atoi(value_as_cstring) > 0;
	}
	else
		use_inputs_average = false;
		*/
	modified = false;

	return true;
}
//---------------------------------------------------------------------------
unsigned int t_mep_functions::get_list_of_operators(int *actual_operators)
{
	unsigned int num_operators = 0;

	if (use_addition) {
		actual_operators[num_operators] = O_ADDITION;
		num_operators++;
	}
	if (use_subtraction) {
		actual_operators[num_operators] = O_SUBTRACTION;
		num_operators++;
	}
	if (use_multiplication) {
		actual_operators[num_operators] = O_MULTIPLICATION;
		num_operators++;
	}
	if (use_division) {
		actual_operators[num_operators] = O_DIVISION;
		num_operators++;
	}
	if (use_power) {
		actual_operators[num_operators] = O_POWER;
		num_operators++;
	}
	if (use_sqrt) {
		actual_operators[num_operators] = O_SQRT;
		num_operators++;
	}
	if (use_exp) {
		actual_operators[num_operators] = O_EXP;
		num_operators++;
	}
	if (use_pow10) {
		actual_operators[num_operators] = O_POW10;
		num_operators++;
	}
	if (use_ln) {
		actual_operators[num_operators] = O_LN;
		num_operators++;
	}
	if (use_log10) {
		actual_operators[num_operators] = O_LOG10;
		num_operators++;
	}
	if (use_log2) {
		actual_operators[num_operators] = O_LOG2;
		num_operators++;
	}
	if (use_floor) {
		actual_operators[num_operators] = O_FLOOR;
		num_operators++;
	}
	if (use_ceil) {
		actual_operators[num_operators] = O_CEIL;
		num_operators++;
	}
	if (use_abs) {
		actual_operators[num_operators] = O_ABS;
		num_operators++;
	}
	if (use_inv) {
		actual_operators[num_operators] = O_INV;
		num_operators++;
	}
	if (use_neg) {
		actual_operators[num_operators] = O_NEG;
		num_operators++;
	}
	if (use_x2) {
		actual_operators[num_operators] = O_X2;
		num_operators++;
	}
	if (use_min) {
		actual_operators[num_operators] = O_MIN;
		num_operators++;
	}
	if (use_max) {
		actual_operators[num_operators] = O_MAX;
		num_operators++;
	}
	if (use_sin) {
		actual_operators[num_operators] = O_SIN;
		num_operators++;
	}
	if (use_cos) {
		actual_operators[num_operators] = O_COS;
		num_operators++;
	}
	if (use_tan) {
		actual_operators[num_operators] = O_TAN;
		num_operators++;
	}
	if (use_asin) {
		actual_operators[num_operators] = O_ASIN;
		num_operators++;
	}
	if (use_acos) {
		actual_operators[num_operators] = O_ACOS;
		num_operators++;
	}
	if (use_atan) {
		actual_operators[num_operators] = O_ATAN;
		num_operators++;
	}
	if (use_iflz) {
		actual_operators[num_operators] = O_IFLZ;
		num_operators++;
	}
	if (use_ifalbcd) {
		actual_operators[num_operators] = O_IFALBCD;
		num_operators++;
	}
	if (use_if_a_or_b_cd) {
		actual_operators[num_operators] = O_IF_A_OR_B_CD;
		num_operators++;
	}
	if (use_if_a_xor_b_cd) {
		actual_operators[num_operators] = O_IF_A_XOR_B_CD;
		num_operators++;
	}
	
	if (use_num_inputs) {
		actual_operators[num_operators] = O_NUM_INPUTS;
		num_operators++;
	}

	if (use_inputs_average) {
		actual_operators[num_operators] = O_INPUTS_AVERAGE;
		num_operators++;
	}
	
	if (use_fmod) {
		actual_operators[num_operators] = O_FMOD;
		num_operators++;
	}

	return num_operators;
}
//---------------------------------------------------------------------------
unsigned int t_mep_functions::count_operators(void) const
{
	unsigned int num_operators = 0;
	if (use_addition)
		num_operators++;
	
	if (use_subtraction)
		num_operators++;
	
	if (use_multiplication)
		num_operators++;

	if (use_division)
		num_operators++;

	if (use_power)
		num_operators++;
	
	if (use_sqrt) 
		num_operators++;
	
	if (use_exp) 
		num_operators++;
	
	if (use_pow10)
		num_operators++;
	
	if (use_ln)
		num_operators++;
	
	if (use_log10)
		num_operators++;
	
	if (use_log2)
		num_operators++;
	
	if (use_floor) 
		num_operators++;
	
	if (use_ceil) 
		num_operators++;
	
	if (use_abs)
		num_operators++;
	
	if (use_inv)
		num_operators++;
	
	if (use_neg)
		num_operators++;

	if (use_x2)
		num_operators++;
	
	if (use_min) 
		num_operators++;
	
	if (use_max)
		num_operators++;
	
	if (use_sin) 
		num_operators++;
	
	if (use_cos) 
		num_operators++;
	
	if (use_tan)
		num_operators++;
	
	if (use_asin)
		num_operators++;
	
	if (use_acos)
		num_operators++;
	
	if (use_atan)
		num_operators++;
	
	if (use_iflz)
		num_operators++;
	
	if (use_ifalbcd)
		num_operators++;

	if (use_if_a_or_b_cd)
		num_operators++;

	if (use_if_a_xor_b_cd)
		num_operators++;

	if (use_num_inputs)
		num_operators++;
	
	if (use_inputs_average)
		num_operators++;
	
	if (use_fmod)
		num_operators++;

	return num_operators;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_addition(void) const
{
	return use_addition;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_addition_enabled(int /*data_type*/) const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_subtraction(void)const
{
	return use_subtraction;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_subtraction_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_multiplication(void)const
{
	return use_multiplication;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_multiplication_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_division(void)const
{
	return use_division;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_division_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_power(void)const
{
	return use_power;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_power_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_sqrt(void)const
{
	return use_sqrt;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_sqrt_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_exp(void)const
{
	return use_exp;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_exp_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_pow10(void)const
{
	return use_pow10;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_pow10_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_ln(void)const
{
	return use_ln;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_ln_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_log10(void)const
{
	return use_log10;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_log10_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_log2(void)const
{
	return use_log2;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_log2_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_floor(void)const
{
	return use_floor;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_floor_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_ceil(void)const
{
	return use_ceil;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_ceil_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_abs(void)const
{
	return use_abs;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_abs_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_inv(void)const
{
	return use_inv;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_inv_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_neg(void)const
{
	return use_neg;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_neg_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_x2(void)const
{
	return use_x2;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_x2_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_min(void)const
{
	return use_min;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_min_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_max(void)const
{
	return use_max;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_max_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_sin(void)const
{
	return use_sin;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_sin_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_cos(void)const
{
	return use_cos;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_cos_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_tan(void)const
{
	return use_tan;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_tan_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_asin(void)const
{
	return use_asin;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_asin_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_acos(void)const
{
	return use_acos;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_acos_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_atan(void)const
{
	return use_atan;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_atan_enabled(int data_type)const
{
	return data_type == MEP_DATA_DOUBLE;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_iflz(void)const
{
	return use_iflz;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_iflz_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_ifalbcd(void)const
{
	return use_ifalbcd;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_ifalbcd_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_if_a_or_b_cd(void)const
{
	return use_if_a_or_b_cd;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_if_a_or_b_cd_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_if_a_xor_b_cd(void)const
{
	return use_if_a_xor_b_cd;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_if_a_xor_b_cd_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_mod(void)const
{
	return use_fmod;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_mod_enabled(int /*data_type*/)const
{
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void t_mep_functions::set_addition(bool value)
{
	use_addition = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_subtraction(bool value)
{
	use_subtraction = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_multiplication(bool value)
{
	use_multiplication = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_division(bool value)
{
	use_division = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_power(bool value)
{
	use_power = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_sqrt(bool value)
{
	use_sqrt = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_exp(bool value)
{
	use_exp = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_pow10(bool value)
{
	use_pow10 = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_ln(bool value)
{
	use_ln = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_log10(bool value)
{
	use_log10 = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_log2(bool value)
{
	use_log2 = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_floor(bool value)
{
	use_floor = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_ceil(bool value)
{
	use_ceil = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_abs(bool value)
{
	use_abs = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_inv(bool value)
{
	use_inv = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_neg(bool value)
{
	use_neg = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_x2(bool value)
{
	use_x2 = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_min(bool value)
{
	use_min = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_max(bool value)
{
	use_max = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_sin(bool value)
{
	use_sin = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_cos(bool value)
{
	use_cos = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_tan(bool value)
{
	use_tan = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_asin(bool value)
{
	use_asin = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_acos(bool value)
{
	use_acos = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_atan(bool value)
{
	use_atan = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_iflz(bool value)
{
	use_iflz = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_ifalbcd(bool value)
{
	use_ifalbcd = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_if_a_or_b_cd(bool value)
{
	use_if_a_or_b_cd = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_if_a_xor_b_cd(bool value)
{
	use_if_a_xor_b_cd = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_mod(bool value)
{
	use_fmod = value;
	modified = true;
}
//---------------------------------------------------------------------------
bool t_mep_functions::is_modified(void) const
{
	return modified;
}
//---------------------------------------------------------------------------
/*
bool t_mep_functions::get_inputs_average(void)
{
return use_inputs_average;
}
//---------------------------------------------------------------------------
bool t_mep_functions::get_num_inputs(void)
{
return use_num_inputs;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_inputs_average(bool value)
{
use_inputs_average = value;
modified = true;
}
//---------------------------------------------------------------------------
void t_mep_functions::set_num_inputs(bool value)
{
use_num_inputs = value;
modified = true;
}
//---------------------------------------------------------------------------
*/
bool t_mep_functions::operator==(const t_mep_functions &other) const
{
	if (use_addition != other.use_addition)
		return false;
	if (use_subtraction != other.use_subtraction)
		return false;
	if (use_multiplication != other.use_multiplication)
		return false;
	if (use_division != other.use_division)
		return false;
	if (use_power != other.use_power)
		return false;
	if (use_sqrt != other.use_sqrt)
		return false;
	if (use_exp != other.use_exp)
		return false;
	if (use_pow10 != other.use_pow10)
		return false;
	if (use_ln != other.use_ln)
		return false;
	if (use_log10 != other.use_log10)
		return false;
	if (use_log2 != other.use_log2)
		return false;
	if (use_floor != other.use_floor)
		return false;
	if (use_ceil != other.use_ceil)
		return false;
	if (use_abs != other.use_abs)
		return false;
	if (use_inv != other.use_inv)
		return false;
	if (use_neg != other.use_neg)
		return false;
	if (use_x2 != other.use_x2)
		return false;
	if (use_min != other.use_min)
		return false;
	if (use_max != other.use_max)
		return false;
	if (use_sin != other.use_sin)
		return false;
	if (use_cos != other.use_cos)
		return false;
	if (use_tan != other.use_tan)
		return false;
	if (use_asin != other.use_asin)
		return false;
	if (use_acos != other.use_acos)
		return false;
	if (use_atan != other.use_atan)
		return false;
	if (use_iflz != other.use_iflz)
		return false;
	if (use_ifalbcd != other.use_ifalbcd)
		return false;
	if (use_if_a_or_b_cd != other.use_if_a_or_b_cd)
		return false;
	if (use_if_a_xor_b_cd != other.use_if_a_xor_b_cd)
		return false;
	if (use_fmod != other.use_fmod)
		return false;

	return true;
}
//---------------------------------------------------------------------------
