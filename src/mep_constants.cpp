// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//---------------------------------------------------------------------------
#include "mep_constants.h"
#include "mep_parameters.h"
#include "utils/validation_double.h"
//---------------------------------------------------------------------------
t_mep_constants::t_mep_constants(void)
{
	init();
}
//---------------------------------------------------------------------------
t_mep_constants::~t_mep_constants(void)
{
	if (constants_double) {
		delete[] constants_double;
		constants_double = NULL;
	}

	if (constants_long) {
		delete[] constants_long;
		constants_long = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep_constants::init(void)
{
	num_automatic_constants = 5;
	num_user_defined_constants = 0;
	min_constants_interval_double = 0;
	max_constants_interval_double = 1;
	min_constants_interval_long = 0;
	max_constants_interval_long = 1;
	data_type = MEP_DATA_DOUBLE;

	if (constants_double) {
		delete[] constants_double;
		constants_double = NULL;
	}
	constants_double = NULL;

	if (constants_long) {
		delete[] constants_long;
		constants_long = NULL;
	}
	constants_long = NULL;

	constants_type = MEP_CONSTANTS_AUTOMATIC; // automatically generated
	constants_can_evolve = true;
	constants_mutation_max_deviation_double = 1;
	constants_mutation_max_deviation_long = 1;
	constants_can_evolve_outside_initial_interval = false;

	modified = false;
}
//---------------------------------------------------------------------------
void t_mep_constants::to_xml_node(pugi::xml_node parent)
{
	char tmp_str[100];
	
	pugi::xml_node node = parent.append_child("data_type");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%d", data_type);
	data.set_value(tmp_str);

	node = parent.append_child("type");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", constants_type);
	data.set_value(tmp_str);

	node = parent.append_child("num_automatic_constants");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", num_automatic_constants);
	data.set_value(tmp_str);

	if (data_type == MEP_DATA_LONG_LONG) {
		node = parent.append_child("min_interval");
		data = node.append_child(pugi::node_pcdata);
		snprintf(tmp_str, 100, "%lld", min_constants_interval_long);
		data.set_value(tmp_str);

		node = parent.append_child("max_interval");
		data = node.append_child(pugi::node_pcdata);
		snprintf(tmp_str, 100, "%lld", max_constants_interval_long);
		data.set_value(tmp_str);

		node = parent.append_child("stddev");
		data = node.append_child(pugi::node_pcdata);
		snprintf(tmp_str, 100, "%lld", constants_mutation_max_deviation_long);
		data.set_value(tmp_str);

		if (constants_long) {
			pugi::xml_node node_constants = parent.append_child("user_constants");
			data = node_constants.append_child(pugi::node_pcdata);

			char* tmp_cst_str = NULL;

			char tmp_s[100];
			tmp_cst_str = new char[num_user_defined_constants * 30]; // 30 digits for each constant !!!
			tmp_cst_str[0] = 0;
			for (unsigned int c = 0; c < num_user_defined_constants; c++) {
				snprintf(tmp_s, 100, "%lld", constants_long[c]);
				strcat(tmp_cst_str, tmp_s);
				strcat(tmp_cst_str, " ");
			}

			data.set_value(tmp_cst_str);
			delete[] tmp_cst_str;
		}
	}
	else {// data type is DOUBLE
		node = parent.append_child("min_interval");
		data = node.append_child(pugi::node_pcdata);
		snprintf(tmp_str, 100, "%lg", min_constants_interval_double);
		data.set_value(tmp_str);

		node = parent.append_child("max_interval");
		data = node.append_child(pugi::node_pcdata);
		snprintf(tmp_str, 100, "%lg", max_constants_interval_double);
		data.set_value(tmp_str);

		node = parent.append_child("stddev");
		data = node.append_child(pugi::node_pcdata);
		snprintf(tmp_str, 100, "%lg", constants_mutation_max_deviation_double);
		data.set_value(tmp_str);

		if (constants_double) {
			pugi::xml_node node_constants = parent.append_child("user_constants");
			data = node_constants.append_child(pugi::node_pcdata);

			char* tmp_cst_str = NULL;

			char tmp_s[100];
			tmp_cst_str = new char[num_user_defined_constants * 30]; // 30 digits for each constant !!!
			tmp_cst_str[0] = 0;
			for (unsigned int c = 0; c < num_user_defined_constants; c++) {
				snprintf(tmp_s, 100, "%lg", constants_double[c]);
				strcat(tmp_cst_str, tmp_s);
				strcat(tmp_cst_str, " ");
			}

			data.set_value(tmp_cst_str);
			delete[] tmp_cst_str;
		}
	}

	node = parent.append_child("evolve");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%d", constants_can_evolve);
	data.set_value(tmp_str);

	node = parent.append_child("can_evolve_outside_initial_interval");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%d", constants_can_evolve_outside_initial_interval);
	data.set_value(tmp_str);

	node = parent.append_child("num_user_defined_constants");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", num_user_defined_constants);
	data.set_value(tmp_str);

	modified = false;
}
//---------------------------------------------------------------------------
bool t_mep_constants::from_xml_node(pugi::xml_node parent)
{
	pugi::xml_node node = parent.child("data_type");
	if (node) {
		const char* value_as_cstring = node.child_value();
		data_type = (char)atoi(value_as_cstring);
	}
	else{// node was not found, so we set it to Double
		data_type = MEP_DATA_DOUBLE;
	}

	node = parent.child("type");
	if (node) {
		const char* value_as_cstring = node.child_value();
		constants_type = (unsigned int)atoi(value_as_cstring);
	}
	else
		constants_type = MEP_CONSTANTS_AUTOMATIC;

	node = parent.child("num_automatic_constants");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_automatic_constants = (unsigned int) atoi(value_as_cstring);
	}
	else
		num_automatic_constants = 0;
	
	node = parent.child("num_user_defined_constants");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_user_defined_constants = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_user_defined_constants = 0;

	if (constants_type == MEP_CONSTANTS_AUTOMATIC){
		num_user_defined_constants = 0;
	}
	else{ // user defined constants
		num_automatic_constants = 0;
	}
	
	if (data_type == MEP_DATA_LONG_LONG) {
		node = parent.child("min_interval");
		if (node) {
			const char* value_as_cstring = node.child_value();
			min_constants_interval_long = atol(value_as_cstring);
		}

		node = parent.child("max_interval");
		if (node) {
			const char* value_as_cstring = node.child_value();
			max_constants_interval_long = atol(value_as_cstring);
		}

		node = parent.child("stddev");
		if (node) {
			const char* value_as_cstring = node.child_value();
			constants_mutation_max_deviation_long = atol(value_as_cstring);
		}

		if (constants_long) {
			delete[] constants_long;
			constants_long = NULL;
		}

		pugi::xml_node node_constants = parent.child("user_constants");

		if (node_constants) {
			const char* value_as_cstring = node_constants.child_value();
			size_t num_jumped_chars = 0;

			constants_long = new long long[num_user_defined_constants];
			for (unsigned int c = 0; c < num_user_defined_constants; c++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lld", &constants_long[c]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
		else
			num_user_defined_constants = 0;
	}
	else {// double
		node = parent.child("min_interval");
		if (node) {
			const char* value_as_cstring = node.child_value();
			min_constants_interval_double = atof(value_as_cstring);
		}

		node = parent.child("max_interval");
		if (node) {
			const char* value_as_cstring = node.child_value();
			max_constants_interval_double = atof(value_as_cstring);
		}

		node = parent.child("stddev");
		if (node) {
			const char* value_as_cstring = node.child_value();
			constants_mutation_max_deviation_double = atof(value_as_cstring);
		}

		if (constants_double) {
			delete[] constants_double;
			constants_double = NULL;
		}

		pugi::xml_node node_constants = parent.child("user_constants");

		if (node_constants) {
			const char* value_as_cstring = node_constants.child_value();
			size_t num_jumped_chars = 0;

			constants_double = new double[num_user_defined_constants];
			for (unsigned int c = 0; c < num_user_defined_constants; c++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &constants_double[c]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
		else
			num_user_defined_constants = 0;
	}

	node = parent.child("evolve");
	if (node) {
		const char *value_as_cstring = node.child_value();
		constants_can_evolve = (bool)atoi(value_as_cstring);
	}

	if (constants_can_evolve) {
		node = parent.child("can_evolve_outside_initial_interval");
		if (node) {
			const char* value_as_cstring = node.child_value();
			constants_can_evolve_outside_initial_interval = (bool)atoi(value_as_cstring);
		}
		else
			constants_can_evolve_outside_initial_interval = false;
	}
	else
		constants_can_evolve_outside_initial_interval = false;

	modified = false;
	return true;
}
//---------------------------------------------------------------------------
unsigned int t_mep_constants::get_num_automatic_constants(void)const
{
	return num_automatic_constants;
}
//---------------------------------------------------------------------------
unsigned int t_mep_constants::get_num_user_defined_constants(void)const
{
	return num_user_defined_constants;
}
//---------------------------------------------------------------------------
double t_mep_constants::get_min_constants_interval_double(void)const
{
	return min_constants_interval_double;
}
//---------------------------------------------------------------------------
double t_mep_constants::get_max_constants_interval_double(void)const
{
	return max_constants_interval_double;
}
//---------------------------------------------------------------------------
double t_mep_constants::get_constants_double(unsigned int index) const
{
	return constants_double[index];
}
//---------------------------------------------------------------------------
long long t_mep_constants::get_min_constants_interval_long(void)const
{
	return min_constants_interval_long;
}
//---------------------------------------------------------------------------
long long t_mep_constants::get_max_constants_interval_long(void)const
{
	return max_constants_interval_long;
}
//---------------------------------------------------------------------------
long long t_mep_constants::get_constants_long(unsigned int index) const
{
	return constants_long[index];
}
//---------------------------------------------------------------------------
unsigned int t_mep_constants::get_constants_type(void)const
{
	return constants_type;
}
//---------------------------------------------------------------------------
bool t_mep_constants::get_constants_can_evolve(void)const
{
	return constants_can_evolve;
}
//---------------------------------------------------------------------------
double t_mep_constants::get_constants_mutation_max_deviation_double(void)const
{
	return constants_mutation_max_deviation_double;
}
//---------------------------------------------------------------------------
long long t_mep_constants::get_constants_mutation_max_deviation_long(void)const
{
	return constants_mutation_max_deviation_long;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_num_automatic_constants(unsigned int value)
{
	num_automatic_constants = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_num_user_defined_constants(unsigned int value)
{
	if (constants_double){
		delete[] constants_double;
		constants_double = NULL;
	}
	if (constants_long){
		delete[] constants_long;
		constants_long = NULL;
	}

	if (value > 0)
		num_user_defined_constants = value;
	else
		num_user_defined_constants = 0;

	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_min_constants_interval_double(double value)
{
	min_constants_interval_double = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_max_constants_interval_double(double value)
{
	max_constants_interval_double = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_constants_double(unsigned int index, double value)
{
	if (!constants_double)
		if (num_user_defined_constants)
			constants_double = new double[num_user_defined_constants];
	
	if (constants_double){
		constants_double[index] = value;
		modified = true;
	}
}
//---------------------------------------------------------------------------
void t_mep_constants::set_min_constants_interval_long(long long value)
{
	min_constants_interval_long = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_max_constants_interval_long(long long value)
{
	max_constants_interval_long = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_constants_long(unsigned int index, long long value)
{
	if (!constants_long)
		if (num_user_defined_constants)
			constants_long = new long long[num_user_defined_constants];
	
	if (constants_long){
		constants_long[index] = value;
		modified = true;
	}
}
//---------------------------------------------------------------------------
void t_mep_constants::set_constants_type(unsigned int value)
{
	constants_type = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_constants_can_evolve(bool value)
{
	constants_can_evolve = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_constants_mutation_max_deviation_double(double value)
{
	constants_mutation_max_deviation_double = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_constants_mutation_max_deviation_long(long long value)
{
	constants_mutation_max_deviation_long = value;
	modified = true;
}
//---------------------------------------------------------------------------
bool t_mep_constants::operator==(const t_mep_constants &other) const
{
	if (num_automatic_constants != other.num_automatic_constants)
		return false;

    if (num_user_defined_constants != other.num_user_defined_constants)
        return false;

	if (min_constants_interval_double != other.min_constants_interval_double)
		return false;

    if (max_constants_interval_double != other.max_constants_interval_double)
        return false;
	
	if (min_constants_interval_long != other.min_constants_interval_long)
		return false;

	if (max_constants_interval_long != other.max_constants_interval_long)
		return false;

	if (constants_double){
		for (unsigned int i = 0; i < num_user_defined_constants; i++)
			if (constants_double[i] != other.constants_double[i])
				return false;
	}
	else
		if (constants_long){
			for (unsigned int i = 0; i < num_user_defined_constants; i++)
				if (constants_long[i] != other.constants_long[i])
					return false;
		}

	if (constants_type != other.constants_type)
		return false;

    if (constants_can_evolve != other.constants_can_evolve)
        return false;

	if (constants_can_evolve_outside_initial_interval != other.constants_can_evolve_outside_initial_interval)
		return false;
	
	if (constants_mutation_max_deviation_double != other.constants_mutation_max_deviation_double)
        return false;
	
	if (constants_mutation_max_deviation_long != other.constants_mutation_max_deviation_long)
		return false;

	return true;
}
//---------------------------------------------------------------------------
t_mep_constants& t_mep_constants::operator=(const t_mep_constants &source)
{
	// does nothing yet
	if (&source != this) {
		/*
		if (constants_double) {
			delete[] constants_double;
			constants_double = NULL;
		}
		 */
	}
	return *this;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_constants_can_evolve_outside_initial_interval(bool value)
{
	constants_can_evolve_outside_initial_interval = value;
	modified = true;
}
//---------------------------------------------------------------------------
bool t_mep_constants::get_constants_can_evolve_outside_initial_interval(void) const
{
	return constants_can_evolve_outside_initial_interval;
}
//---------------------------------------------------------------------------
char t_mep_constants::get_data_type(void) const
{
	return data_type;
}
//---------------------------------------------------------------------------
bool t_mep_constants::set_data_type(char new_data_type)
{
	if (data_type == new_data_type)
		return true;
	
	if (data_type == MEP_DATA_DOUBLE && new_data_type == MEP_DATA_LONG_LONG){
		// test first to see if they are actually integers
		if (!is_valid_long_long(min_constants_interval_double))
			return false;
		if (!is_valid_long_long(max_constants_interval_double))
			return false;
		
		if (!is_valid_long_long(constants_mutation_max_deviation_double))
			return false;
		
		for (unsigned int i = 0; i < num_user_defined_constants; i++)
			if (!is_valid_long_long(constants_double[i]))
				return false;
		// if the test succeded, then do the change
		
		min_constants_interval_long = (long long)min_constants_interval_double;
		max_constants_interval_long = (long long)max_constants_interval_double;
		constants_mutation_max_deviation_long = (long long)constants_mutation_max_deviation_double;
		if (constants_long) {
			delete[] constants_long;
			constants_long = NULL;
		}
		
		if (constants_double){
			constants_long = new long long[num_user_defined_constants];
			for (unsigned int i = 0; i < num_user_defined_constants; i++)
				constants_long[i] = (long long)constants_double[i];
			
			delete[] constants_double;
			constants_double = NULL;
		}
		
		data_type = new_data_type;
		return true;
	}
	else
		if (data_type == MEP_DATA_LONG_LONG && new_data_type == MEP_DATA_DOUBLE){
			min_constants_interval_double = (double)min_constants_interval_long;
			max_constants_interval_double = (double)max_constants_interval_long;
			constants_mutation_max_deviation_double = (double)constants_mutation_max_deviation_long;
			if (constants_double) {
				delete[] constants_double;
				constants_double = NULL;
			}
			
			if (constants_long){
				constants_double = new double[num_user_defined_constants];
				for (unsigned int i = 0; i < num_user_defined_constants; i++)
					constants_double[i] = (double)constants_long[i];
				
				delete[] constants_long;
				constants_long = NULL;
			}
			
			data_type = new_data_type;
			return true;
		}
	
	return false;
}
//---------------------------------------------------------------------------
void t_mep_constants::set_data_type_no_update(char new_data_type)
{
	data_type = new_data_type;
}
//---------------------------------------------------------------------------
