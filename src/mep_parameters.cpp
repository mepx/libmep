#include "mep_parameters.h"

#include <stdio.h>
#include <stdlib.h>


t_parameters::t_parameters(void)
{
    init();
}
//---------------------------------------------------------------------------
void t_parameters::init (void)
{
	mutation_probability = 0.01;                      // mutation probability
	crossover_probability = 0.9;                   // crossover probability
    code_length = 50;               // the number of genes
    subpopulation_size = 100;                 // the number of individuals in population  (must be an odd number!!!)
	num_generations = 100;
	problem_type = PROBLEM_REGRESSION;               // 0- regression, 1-classification
	random_seed = 0;
	num_runs = 1;
	tournament_size = 2;
	num_subpopulations = 1;
	operators_probability = 0.5;
	variables_probability = 0.5;
	constants_probability = 0;
	use_validation_data = false;
	crossover_type = 0;
	simplified_programs = 0;
    num_threads = 1;
	constants.init();
}
//---------------------------------------------------------------------------
int t_parameters::to_xml(pugi::xml_node parent)
{
    char tmp_str[30];
	pugi::xml_node node = parent.append_child("mutation_probability");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", mutation_probability);
	data.set_value(tmp_str);

	node = parent.append_child("crossover_probability");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", crossover_probability);
	data.set_value(tmp_str);

	node = parent.append_child("crossover_type");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", crossover_type);
	data.set_value(tmp_str);

	node = parent.append_child("chromosome_length");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", code_length);
	data.set_value(tmp_str);

	node = parent.append_child("subpopulation_size");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", subpopulation_size);
	data.set_value(tmp_str);

	node = parent.append_child("num_subpopulations");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", num_subpopulations);
	data.set_value(tmp_str);

	node = parent.append_child("tournament_size");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", tournament_size);
	data.set_value(tmp_str);

	node = parent.append_child("number_of_generations");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", num_generations);
	data.set_value(tmp_str);

	node = parent.append_child("problem_type");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", problem_type);
	data.set_value(tmp_str);

	node = parent.append_child("random_seed");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", random_seed);
	data.set_value(tmp_str);

	node = parent.append_child("operators_probability");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", operators_probability);
	data.set_value(tmp_str);

	node = parent.append_child("variables_probability");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", variables_probability);
	data.set_value(tmp_str);

	node = parent.append_child("constants_probability");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", constants_probability);
	data.set_value(tmp_str);

	node = parent.append_child("num_runs");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", num_runs);
	data.set_value(tmp_str);

	node = parent.append_child("use_validation_data");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", use_validation_data);
	data.set_value(tmp_str);

	node = parent.append_child("simplified");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", simplified_programs);
	data.set_value(tmp_str);

    node = parent.append_child("num_threads");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%ld", num_threads);
	data.set_value(tmp_str);

	node = parent.append_child("constants");
	constants.to_xml(node);

	return true;
}
//---------------------------------------------------------------------------
int t_parameters::from_xml(pugi::xml_node parent)
{
	pugi::xml_node node = parent.child("mutation_probability");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		mutation_probability = atof(value_as_cstring);
	}

	node = parent.child("crossover_probability");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		crossover_probability = atof(value_as_cstring);
	}

	node = parent.child("chromosome_length");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		code_length = atoi(value_as_cstring);
	}

	node = parent.child("crossover_type");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		crossover_type = atoi(value_as_cstring);
	}

	node = parent.child("subpopulation_size");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		subpopulation_size = atoi(value_as_cstring);
	}

	node = parent.child("num_subpopulations");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		num_subpopulations = atoi(value_as_cstring);
	}

	node = parent.child("operators_probability");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		operators_probability = atof(value_as_cstring);
	}

	node = parent.child("variables_probability");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		variables_probability = atof(value_as_cstring);
	}

	node = parent.child("constants_probability");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		constants_probability = atof(value_as_cstring);
	}

	node = parent.child("number_of_generations");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		num_generations = atoi(value_as_cstring);
	}

	node = parent.child("tournament_size");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		tournament_size = atoi(value_as_cstring);
	}

	node = parent.child("problem_type");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		problem_type = atoi(value_as_cstring);
	}

	node = parent.child("random_seed");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		random_seed = atoi(value_as_cstring);
	}
	else
		random_seed = 0;
	
	node = parent.child("num_runs");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		num_runs = atoi(value_as_cstring);
	}
	else
		num_runs = 1;

	node = parent.child("use_validation_data");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		use_validation_data = atoi(value_as_cstring);
	}
	else
		use_validation_data = 0;

	node = parent.child("simplified");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		simplified_programs = atoi(value_as_cstring);
	}
	else
		simplified_programs = 0;
    
    node = parent.child("num_threads");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		num_threads = atoi(value_as_cstring);
	}
	else
		num_threads = 1;

	node = parent.child("constants");
	if (node)
	{
		constants.from_xml(node);
	}
	else
		constants.init();

		return true;
}
//---------------------------------------------------------------------------
