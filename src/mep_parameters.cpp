#include "mep_parameters.h"

#include <stdio.h>
#include <stdlib.h>


t_mep_parameters::t_mep_parameters(void)
{
    init();
}
//---------------------------------------------------------------------------
void t_mep_parameters::init (void)
{
	mutation_probability = 0.01;                      // mutation probability
	crossover_probability = 0.9;                   // crossover probability
    code_length = 50;               // the number of genes
    subpopulation_size = 100;                 // the number of individuals in population  (must be an odd number!!!)
	num_generations = 100;
	problem_type = MEP_PROBLEM_REGRESSION;               // 0- regression, 1-classification
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
	random_subset_selection_size = 1000;
	error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;

	modified = false;
}
//---------------------------------------------------------------------------
int t_mep_parameters::to_xml(pugi::xml_node parent)
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

	node = parent.append_child("random_subset_selection_size");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", random_subset_selection_size);
	data.set_value(tmp_str);

	node = parent.append_child("error_measure");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", error_measure);
	data.set_value(tmp_str);

	modified = false;

	return true;
}
//---------------------------------------------------------------------------
int t_mep_parameters::from_xml(pugi::xml_node parent)
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

	node = parent.child("random_subset_selection_size");
	if (node) {
		const char *value_as_cstring = node.child_value();
		random_subset_selection_size = atoi(value_as_cstring);
	}
	else
		random_subset_selection_size = 0;

	node = parent.child("error_measure");
	if (node) {
		const char *value_as_cstring = node.child_value();
		int value = atoi(value_as_cstring);
		if (problem_type == MEP_PROBLEM_REGRESSION) {
			if (value == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR || value == MEP_REGRESSION_MEAN_SQUARED_ERROR)
				error_measure = value;
			else
				error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
		}
		else {// classification
			if (value == MEP_CLASSIFICATION_MEAN_ERROR)
				error_measure = value;
			else
				error_measure = MEP_CLASSIFICATION_MEAN_ERROR;
		}
	}
	else
		if (problem_type == MEP_PROBLEM_REGRESSION)
			error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
		else
			error_measure = MEP_CLASSIFICATION_MEAN_ERROR;

	modified = false;

	return true;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_mutation_probability(void)
{
	return mutation_probability;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_crossover_probability(void)
{
	return crossover_probability;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_code_length(void)
{
	return code_length;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_subpopulation_size(void)
{
	return subpopulation_size;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_num_threads(void)
{
	return num_threads;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_tournament_size(void)
{
	return tournament_size;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_num_generations(void)
{
	return num_generations;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_problem_type(void)
{
	return problem_type;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_num_subpopulations(void)
{
	return num_subpopulations;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_operators_probability(void)
{
	return operators_probability;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_variables_probability(void)
{
	return variables_probability;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_constants_probability(void)
{
	return constants_probability;
}
//---------------------------------------------------------------------------
bool t_mep_parameters::get_use_validation_data(void)
{
	return use_validation_data;
}
//---------------------------------------------------------------------------
int t_mep_parameters::get_crossover_type(void)
{
	return crossover_type;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_random_seed(void)
{
	return random_seed;
}
//---------------------------------------------------------------------------
long t_mep_parameters::get_num_runs(void)
{
	return num_runs;
}
//---------------------------------------------------------------------------
bool t_mep_parameters::get_simplified_programs(void)
{
	return simplified_programs;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_mutation_probability(double value)
{

		mutation_probability = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_crossover_probability(double value)
{

		crossover_probability = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_code_length(long value)
{

		code_length = value;
		modified = true;

}
//---------------------------------------------------------------------------

void t_mep_parameters::set_subpopulation_size(long value)
{

		subpopulation_size = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_threads(long value)
{

		num_threads = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_tournament_size(long value)
{

		tournament_size = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_generations(long value)
{
		num_generations = value;
		modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_problem_type(long value)
{
		problem_type = value;
		if (problem_type == MEP_PROBLEM_REGRESSION)
			error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
		else
			error_measure = MEP_CLASSIFICATION_MEAN_ERROR;
		modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_subpopulations(long value)
{

		num_subpopulations = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_operators_probability(double value)
{

		operators_probability = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_variables_probability(double value)
{

		variables_probability = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_constants_probability(double value)
{

		constants_probability = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_use_validation_data(bool value)
{

		use_validation_data = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_crossover_type(int value)
{

		crossover_type = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_random_seed(long value)
{

		random_seed = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_runs(long value)
{

		num_runs = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_simplified_programs(bool value)
{

		simplified_programs = value;
		modified = true;

}
//---------------------------------------------------------------------------
void t_mep_parameters::set_random_subset_selection_size(int value)
{
	random_subset_selection_size = value;
	modified = true;
}
//---------------------------------------------------------------------------
int t_mep_parameters::get_random_subset_selection_size(void)
{
	return random_subset_selection_size;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_error_measure(int value)
{
	if (problem_type == MEP_PROBLEM_REGRESSION) {
		if (value == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR || value == MEP_REGRESSION_MEAN_SQUARED_ERROR)
			error_measure = value;
	}
	else {// classification
		if (value == MEP_CLASSIFICATION_MEAN_ERROR)
			error_measure = value;
	}
	modified = true;
}
//---------------------------------------------------------------------------
int t_mep_parameters::get_error_measure(void)
{
	return error_measure;
}
//---------------------------------------------------------------------------
