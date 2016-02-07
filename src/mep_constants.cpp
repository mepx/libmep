#include "mep_constants.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

t_constants::t_constants(void)
{
	init();
}
//---------------------------------------------------------------------------
t_constants::~t_constants(void)
{
	if (constants_double)
		delete[] constants_double;
	init();
}
//---------------------------------------------------------------------------
void t_constants::init(void)
{
	num_automatic_constants = 5;
	num_user_defined_constants = 0;
	min_constants_interval_double = -10;
	max_constants_interval_double = 10;

	constants_double = NULL;

	constants_type = 1; // automatically generated
	constants_can_evolve = true;
	constants_mutation_max_deviation = 1;
}
//---------------------------------------------------------------------------
int t_constants::to_xml(pugi::xml_node parent)
{
	char tmp_str[30];

	pugi::xml_node node = parent.append_child("type");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", constants_type);
	data.set_value(tmp_str);

	node = parent.append_child("num_automatic_constants");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", num_automatic_constants);
	data.set_value(tmp_str);

	node = parent.append_child("min_interval");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", min_constants_interval_double);
	data.set_value(tmp_str);

	node = parent.append_child("max_interval");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", max_constants_interval_double);
	data.set_value(tmp_str);

	node = parent.append_child("evolve");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", constants_can_evolve);
	data.set_value(tmp_str);

	node = parent.append_child("stddev");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", constants_mutation_max_deviation);
	data.set_value(tmp_str);

	node = parent.append_child("num_user_defined_constants");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", num_user_defined_constants);
	data.set_value(tmp_str);

	if (constants_double){
		pugi::xml_node node_constants = parent.append_child("constants");
		data = node_constants.append_child(pugi::node_pcdata);

		char *tmp_cst_str = NULL;

		if (constants_double){
			char tmp_s[30];
			tmp_cst_str = new char[num_user_defined_constants * 30]; // 30 digits for each constant !!!
			tmp_cst_str[0] = 0;
			for (int c = 0; c < num_user_defined_constants; c++){
				sprintf(tmp_s, "%lg", constants_double[c]);
				strcat(tmp_cst_str, tmp_s);
				strcat(tmp_cst_str, " ");
			}
		}

		data.set_value(tmp_cst_str);
		delete[] tmp_cst_str;

	}


	return true;
}
//---------------------------------------------------------------------------
int t_constants::from_xml(pugi::xml_node parent)
{
	pugi::xml_node node = parent.child("type");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		constants_type = atoi(value_as_cstring);
	}

	node = parent.child("num_automatic_constants");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		num_automatic_constants = atoi(value_as_cstring);
	}

	node = parent.child("min_interval");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		min_constants_interval_double = atof(value_as_cstring);
	}

	node = parent.child("max_interval");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		max_constants_interval_double = atof(value_as_cstring);
	}

	node = parent.child("stddev");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		constants_mutation_max_deviation = atof(value_as_cstring);
	}

	node = parent.child("evolve");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		constants_can_evolve = (bool)atoi(value_as_cstring);
	}

	node = parent.child("num_user_defined_constants");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		num_user_defined_constants = atoi(value_as_cstring);
	}

	if (constants_double)
		delete[] constants_double;

	pugi::xml_node node_constants = parent.child("constants");

	if (node_constants)
	{
		const char *value_as_cstring = node.child_value();
		int num_jumped_chars = 0;

		constants_double = new double[num_user_defined_constants];
		for (int c = 0; c < num_user_defined_constants; c++){
			sscanf(value_as_cstring + num_jumped_chars, "%lf", &constants_double[c]);
			long local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
