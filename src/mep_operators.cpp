
#include <stdio.h>
#include <stdlib.h>

#include "mep_operators.h"
//#include "data_class.h"

//---------------------------------------------------------------------------
t_mep_operators::t_mep_operators(void)
{
	init();
}
//---------------------------------------------------------------------------
void t_mep_operators::init(void)
{
	use_addition = use_subtraction = use_multiplication = true;
	use_division = use_power = use_sqrt = use_exp = use_pow10 = use_ln = use_log10 = use_log2 = use_floor = use_ceil = use_abs = use_inv = use_x2 = use_min = use_max = false;
	
	use_not = use_and = use_or = use_xor = use_nand = use_nor = false;
    
    use_sin = use_cos = use_tan = use_asin = use_acos = use_atan = false;

	use_iflz = use_ifalbcd = false;

	modified = false;
}
//---------------------------------------------------------------------------
int t_mep_operators::to_xml(pugi::xml_node parent)
{
    char tmp_str[10];
    pugi::xml_node node = parent.append_child("use_addition");
    pugi::xml_node data = node.append_child(pugi::node_pcdata);
    sprintf(tmp_str, "%d", use_addition);
    data.set_value(tmp_str);
    
    node = parent.append_child("use_subtraction");
    data = node.append_child(pugi::node_pcdata);
    sprintf(tmp_str, "%d", use_subtraction);
    data.set_value(tmp_str);
    
    node = parent.append_child("use_multiplication");
    data = node.append_child(pugi::node_pcdata);
    sprintf(tmp_str, "%d", use_multiplication);
    data.set_value(tmp_str);
    
    node = parent.append_child("use_division");
    data = node.append_child(pugi::node_pcdata);
    sprintf(tmp_str, "%d", use_division);
    data.set_value(tmp_str);
	
	node = parent.append_child("use_power");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_power);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_sqrt");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_sqrt);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_exp");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_exp);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_pow10");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_pow10);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_ln");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_ln);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_log10");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_log10);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_log2");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_log2);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_floor");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_floor);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_ceil");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_ceil);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_abs");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_abs);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_inv");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_inv);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_x2");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_x2);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_min");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_min);
	data.set_value(tmp_str);
	
	node = parent.append_child("use_max");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_max);
	data.set_value(tmp_str);
    
	node = parent.append_child("use_not");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_not);
	data.set_value(tmp_str);
    
	node = parent.append_child("use_and");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_and);
	data.set_value(tmp_str);
    
	node = parent.append_child("use_or");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_or);
	data.set_value(tmp_str);
    
	node = parent.append_child("use_xor");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_xor);
	data.set_value(tmp_str);
    
	node = parent.append_child("use_nand");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_nand);
	data.set_value(tmp_str);
    
	node = parent.append_child("use_nor");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_nor);
	data.set_value(tmp_str);
    
	node = parent.append_child("use_sin");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_sin);
	data.set_value(tmp_str);
    
   	node = parent.append_child("use_cos");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_cos);
	data.set_value(tmp_str);
    
    node = parent.append_child("use_tan");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_tan);
	data.set_value(tmp_str);
    
    node = parent.append_child("use_asin");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_asin);
	data.set_value(tmp_str);
    
    node = parent.append_child("use_acos");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_acos);
	data.set_value(tmp_str);
    
    node = parent.append_child("use_atan");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_atan);
	data.set_value(tmp_str);
    
	node = parent.append_child("use_iflz");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_iflz);
	data.set_value(tmp_str);

	node = parent.append_child("use_ifalbcd");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_ifalbcd);
	data.set_value(tmp_str);

	modified = false;

	return true;
}
//---------------------------------------------------------------------------
int t_mep_operators::from_xml(pugi::xml_node parent)
{
    pugi::xml_node node = parent.child("use_addition");
    if (node)
    {
        const char *value_as_cstring = node.child_value();
        use_addition = atoi(value_as_cstring) > 0;
    }
	else
		use_addition = false;

    node = parent.child("use_multiplication");
    if (node)
    {
        const char *value_as_cstring = node.child_value();
		use_multiplication = atoi(value_as_cstring) > 0;
    }
	else
		use_multiplication = false;

    node = parent.child("use_division");
    if (node)
    {
        const char *value_as_cstring = node.child_value();
		use_division = atoi(value_as_cstring) > 0;
    }
	else
		use_division = false;

    node = parent.child("use_subtraction");
    if (node)
    {
        const char *value_as_cstring = node.child_value();
		use_subtraction = atoi(value_as_cstring) > 0;
    }
	else
		use_subtraction = false;

	node = parent.child("use_power");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_power = atoi(value_as_cstring) > 0;
	}
	else
		use_power = false;

	node = parent.child("use_sqrt");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_sqrt = atoi(value_as_cstring) > 0;
	}
	else
		use_sqrt = false;

	node = parent.child("use_exp");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_exp = atoi(value_as_cstring) > 0;
	}
	else
		use_exp = false;

	node = parent.child("use_pow10");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_pow10 = atoi(value_as_cstring) > 0;
	}
	else
		use_pow10 = false;

	node = parent.child("use_ln");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_ln = atoi(value_as_cstring) > 0;
	}
	else
		use_ln = false;

	node = parent.child("use_log10");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_log10 = atoi(value_as_cstring) > 0;
	}
	else
		use_log10 = false;

	node = parent.child("use_log2");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_log2 = atoi(value_as_cstring) > 0;
	}
	else
		use_log2 = false;

	node = parent.child("use_floor");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_floor = atoi(value_as_cstring) > 0;
	}
	else
		use_floor = false;

	node = parent.child("use_ceil");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_ceil = atoi(value_as_cstring) > 0;
	}
	else
		use_ceil = false;

	node = parent.child("use_abs");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_abs = atoi(value_as_cstring) > 0;
	}
	else
		use_abs = false;

	node = parent.child("use_inv");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_inv = atoi(value_as_cstring) > 0;
	}
	else
		use_inv = false;

	node = parent.child("use_x2");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_x2 = atoi(value_as_cstring) > 0;
	}
	else
		use_x2 = false;

	node = parent.child("use_min");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_min = atoi(value_as_cstring) > 0;
	}
	else
		use_min = false;

	node = parent.child("use_max");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_max = atoi(value_as_cstring) > 0;
	}
	else
		use_max = false;

	node = parent.child("use_not");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_not = atoi(value_as_cstring) > 0;
	}
	else
		use_not = false;

	node = parent.child("use_and");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_and = atoi(value_as_cstring) > 0;
	}
	else
		use_and = false;

	node = parent.child("use_or");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_or = atoi(value_as_cstring) > 0;
	}
	else
		use_or = false;

	node = parent.child("use_xor");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_xor = atoi(value_as_cstring) > 0;
	}
	else
		use_xor = false;

	node = parent.child("use_nand");
	if (node)
	{
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
	if (node)
	{
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

	modified = false;

	return true;
}
//---------------------------------------------------------------------------
int t_mep_operators::get_list_of_operators(int *actual_operators)
{
    int num_operators = 0;
    if (use_addition){
        actual_operators[num_operators] = O_ADDITION;
        num_operators++;
    }
    if (use_subtraction){
        actual_operators[num_operators] = O_SUBTRACTION;
        num_operators++;
    }
    if (use_multiplication){
        actual_operators[num_operators] = O_MULTIPLICATION;
        num_operators++;
    }
    if (use_division){
        actual_operators[num_operators] = O_DIVISION;
        num_operators++;
    }
    if (use_power){
        actual_operators[num_operators] = O_POWER;
        num_operators++;
    }
    if (use_sqrt){
        actual_operators[num_operators] = O_SQRT;
        num_operators++;
    }
    if (use_exp){
        actual_operators[num_operators] = O_EXP;
        num_operators++;
    }
    if (use_pow10){
        actual_operators[num_operators] = O_POW10;
        num_operators++;
    }
    if (use_ln){
        actual_operators[num_operators] = O_LN;
        num_operators++;
    }
    if (use_log10){
        actual_operators[num_operators] = O_LOG10;
        num_operators++;
    }
    if (use_log2){
        actual_operators[num_operators] = O_lOG2;
        num_operators++;
    }
    if (use_floor){
        actual_operators[num_operators] = O_FLOOR;
        num_operators++;
    }
    if (use_ceil){
        actual_operators[num_operators] = O_CEIL;
        num_operators++;
    }
    if (use_abs){
        actual_operators[num_operators] = O_ABS;
        num_operators++;
    }
    if (use_inv){
        actual_operators[num_operators] = O_INV;
        num_operators++;
    }
    if (use_x2){
        actual_operators[num_operators] = O_X2;
        num_operators++;
    }
    if (use_min){
        actual_operators[num_operators] = O_MIN;
        num_operators++;
    }
    if (use_max){
        actual_operators[num_operators] = O_MAX;
        num_operators++;
    }
    if (use_sin){
        actual_operators[num_operators] = O_SIN;
        num_operators++;
    }
    if (use_cos){
        actual_operators[num_operators] = O_COS;
        num_operators++;
    }
    if (use_tan){
        actual_operators[num_operators] = O_TAN;
        num_operators++;
    }
    if (use_asin){
        actual_operators[num_operators] = O_ASIN;
        num_operators++;
    }
    if (use_acos){
        actual_operators[num_operators] = O_ACOS;
        num_operators++;
    }
    if (use_atan){
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
	return num_operators;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_addition(void)
{
	return use_addition;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_subtraction(void)
{
	return use_subtraction;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_multiplication(void)
{
	return use_multiplication;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_division(void)
{
	return use_division;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_power(void)
{
	return use_power;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_sqrt(void)
{
	return use_sqrt;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_exp(void)
{
	return use_exp;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_pow10(void)
{
	return use_pow10;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_ln(void)
{
	return use_ln;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_log10(void)
{
	return use_log10;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_log2(void)
{
	return use_log2;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_floor(void)
{
	return use_floor;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_ceil(void)
{
	return use_ceil;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_abs(void)
{
	return use_abs;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_inv(void)
{
	return use_inv;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_x2(void)
{
	return use_x2;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_min(void)
{
	return use_min;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_max(void)
{
	return use_max;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_sin(void)
{
	return use_sin;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_cos(void)
{
	return use_cos;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_tan(void)
{
	return use_tan;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_asin(void)
{
	return use_asin;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_acos(void)
{
	return use_acos;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_atan(void)
{
	return use_atan;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_iflz(void)
{
	return use_iflz;
}
//---------------------------------------------------------------------------
bool t_mep_operators::get_ifalbcd(void)
{
	return use_ifalbcd;
}
//---------------------------------------------------------------------------
void t_mep_operators::set_addition(bool value)
{
	
		use_addition = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_subtraction(bool value)
{
	
		use_subtraction = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_multiplication(bool value)
{
	
		use_multiplication = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_division(bool value)
{
	
		use_division = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_power(bool value)
{
	
		use_power = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_sqrt(bool value)
{
	
		use_sqrt = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_exp(bool value)
{
	
		use_exp = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_pow10(bool value)
{
	
		use_pow10 = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_ln(bool value)
{
	
		use_ln = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_log10(bool value)
{
	
		use_log10 = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_log2(bool value)
{
	
		use_log2 = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_floor(bool value)
{
	
		use_floor = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_ceil(bool value)
{
	
		use_ceil = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_abs(bool value)
{
	
		use_abs = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_inv(bool value)
{
	
		use_inv = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_x2(bool value)
{
	
		use_x2 = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_min(bool value)
{
	
		use_min = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_max(bool value)
{
	
		use_max = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_sin(bool value)
{
	
		use_sin = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_cos(bool value)
{
	
		use_cos = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_tan(bool value)
{
	
		use_tan = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_asin(bool value)
{
	
		use_asin = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_acos(bool value)
{
	
		use_acos = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_atan(bool value)
{
	
		use_atan = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_iflz(bool value)
{
	
		use_iflz = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
void t_mep_operators::set_ifalbcd(bool value)
{
	
		use_ifalbcd = value;
		modified = true;
	
}
//---------------------------------------------------------------------------
bool t_mep_operators::is_modified(void)
{
	return modified;
}
//---------------------------------------------------------------------------

