// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "mep_parameters.h"

//---------------------------------------------------------------------------
t_mep_parameters::t_mep_parameters(void)
{
	init();
}
//---------------------------------------------------------------------------
void t_mep_parameters::init(void)
{
	mutation_probability = 0.01;                      // mutation probability
	crossover_probability = 0.9;                   // crossover probability
	code_length = 50;               // the number of genes
	subpopulation_size = 100;                 // the number of individuals in population  (must be an odd number!!!)
	num_generations = 100;
	problem_type = MEP_PROBLEM_REGRESSION;               // 0- regression, 1-classification
	random_seed = 0;
	num_runs = 10;
	tournament_size = 2;
	num_subpopulations = 2;
	operators_probability = 0.5;
	variables_probability = 0.5;
	constants_probability = 0;
	use_validation_data = true;
	crossover_type = MEP_UNIFORM_CROSSOVER;
	simplified_programs = 0;
	num_threads = 2;
	random_subset_selection_size_percent = 100;
	error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
	num_generations_for_which_random_subset_is_kept_fixed = 1;
	time_series_mode = MEP_TIME_SERIES_TEST;
	num_predictions = 10;
	window_size = 2;
	
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
	sprintf(tmp_str, "%u", crossover_type);
	data.set_value(tmp_str);

	node = parent.append_child("chromosome_length");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", code_length);
	data.set_value(tmp_str);

	node = parent.append_child("subpopulation_size");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", subpopulation_size);
	data.set_value(tmp_str);

	node = parent.append_child("num_subpopulations");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", num_subpopulations);
	data.set_value(tmp_str);

	node = parent.append_child("tournament_size");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", tournament_size);
	data.set_value(tmp_str);

	node = parent.append_child("number_of_generations");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", num_generations);
	data.set_value(tmp_str);

	node = parent.append_child("problem_type");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", problem_type);
	data.set_value(tmp_str);

	node = parent.append_child("random_seed");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", random_seed);
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
	sprintf(tmp_str, "%u", num_runs);
	data.set_value(tmp_str);

	node = parent.append_child("use_validation_data");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", use_validation_data);
	data.set_value(tmp_str);

	node = parent.append_child("simplified");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", simplified_programs);
	data.set_value(tmp_str);

	node = parent.append_child("num_threads");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", num_threads);
	data.set_value(tmp_str);

	node = parent.append_child("random_subset_selection_size_percent");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", random_subset_selection_size_percent);
	data.set_value(tmp_str);

	node = parent.append_child("error_measure");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", error_measure);
	data.set_value(tmp_str);

	node = parent.append_child("num_generations_for_which_random_subset_is_kept_fixed");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", num_generations_for_which_random_subset_is_kept_fixed);
	data.set_value(tmp_str);

	node = parent.append_child("time_series_mode");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", time_series_mode);
	data.set_value(tmp_str);

	node = parent.append_child("num_predictions");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", num_predictions);
	data.set_value(tmp_str);

	node = parent.append_child("window_size");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%u", window_size);
	data.set_value(tmp_str);

	modified = false;

	return true;
}
//---------------------------------------------------------------------------
int t_mep_parameters::from_xml(pugi::xml_node parent)
{
	pugi::xml_node node = parent.child("mutation_probability");
	if (node) {
		const char *value_as_cstring = node.child_value();
		mutation_probability = atof(value_as_cstring);
	}

	node = parent.child("crossover_probability");
	if (node) {
		const char* value_as_cstring = node.child_value();
		crossover_probability = atof(value_as_cstring);
	}
	else
		crossover_probability = 0.9;

	node = parent.child("chromosome_length");
	if (node) {
		const char* value_as_cstring = node.child_value();
		code_length = (unsigned int)atoi(value_as_cstring);
	}
	else
		code_length = 30;

	node = parent.child("crossover_type");
	if (node) {
		const char* value_as_cstring = node.child_value();
		crossover_type = (unsigned int)atoi(value_as_cstring);
	}
	else
		crossover_type = MEP_UNIFORM_CROSSOVER;

	node = parent.child("subpopulation_size");
	if (node) {
		const char* value_as_cstring = node.child_value();
		subpopulation_size = (unsigned int)atoi(value_as_cstring);
	}
	else
		subpopulation_size = 50;

	node = parent.child("num_subpopulations");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_subpopulations = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_subpopulations = 1;

	node = parent.child("operators_probability");
	if (node) {
		const char *value_as_cstring = node.child_value();
		operators_probability = atof(value_as_cstring);
	}

	node = parent.child("variables_probability");
	if (node) {
		const char *value_as_cstring = node.child_value();
		variables_probability = atof(value_as_cstring);
	}

	node = parent.child("constants_probability");
	if (node) {
		const char *value_as_cstring = node.child_value();
		constants_probability = atof(value_as_cstring);
	}

	node = parent.child("number_of_generations");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_generations = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_generations = 100;

	node = parent.child("tournament_size");
	if (node) {
		const char* value_as_cstring = node.child_value();
		tournament_size = (unsigned int)atoi(value_as_cstring);
	}
	else
		tournament_size = 2;

	node = parent.child("problem_type");
	if (node) {
		const char* value_as_cstring = node.child_value();
		problem_type = (unsigned int)atoi(value_as_cstring);
	}
	else
		problem_type = MEP_PROBLEM_REGRESSION;

	node = parent.child("random_seed");
	if (node) {
		const char *value_as_cstring = node.child_value();
		random_seed = (unsigned int)atoi(value_as_cstring);
	}
	else
		random_seed = 0;

	node = parent.child("num_runs");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_runs = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_runs = 1;

	node = parent.child("use_validation_data");
	if (node) {
		const char *value_as_cstring = node.child_value();
		use_validation_data = (bool)atoi(value_as_cstring);
	}
	else
		use_validation_data = 0;

	node = parent.child("simplified");
	if (node) {
		const char *value_as_cstring = node.child_value();
		simplified_programs = (bool)atoi(value_as_cstring);
	}
	else
		simplified_programs = 0;

	node = parent.child("num_threads");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_threads = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_threads = 1;

	node = parent.child("random_subset_selection_size_percent");
	if (node) {
		const char *value_as_cstring = node.child_value();
		random_subset_selection_size_percent = (unsigned int)atoi(value_as_cstring);
		if (random_subset_selection_size_percent > 100)
			random_subset_selection_size_percent = 100;
	}
	else
		random_subset_selection_size_percent = 100;

	node = parent.child("num_generations_for_which_random_subset_is_kept_fixed");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_generations_for_which_random_subset_is_kept_fixed = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_generations_for_which_random_subset_is_kept_fixed = 1;

	node = parent.child("num_predictions");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_predictions = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_predictions = 10;

	node = parent.child("window_size");
	if (node) {
		const char* value_as_cstring = node.child_value();
		window_size = (unsigned int)atoi(value_as_cstring);
	}
	else
		window_size = 2;

	node = parent.child("time_series_mode");
	if (node) {
		const char* value_as_cstring = node.child_value();
		time_series_mode = (unsigned int)atoi(value_as_cstring);
	}
	else
		time_series_mode = MEP_TIME_SERIES_TEST;

	node = parent.child("error_measure");
	if (node) {
		const char *value_as_cstring = node.child_value();
		unsigned int value = (unsigned int)atoi(value_as_cstring);
		switch (problem_type) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_TIME_SERIE:
			if (value == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR || value == MEP_REGRESSION_MEAN_SQUARED_ERROR)
				error_measure = value;
			else
				error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
			break;
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			if (value == MEP_BINARY_CLASSIFICATION_BEST_THRESHOLD)
				error_measure = value;
			else
				error_measure = MEP_BINARY_CLASSIFICATION_BEST_THRESHOLD;
			break;
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			if (value == MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR || value == MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR || value == MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR)
				error_measure = value;
			else
				error_measure = MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR;
			break;

		}
	}
	else
		switch (problem_type) {
		case MEP_PROBLEM_REGRESSION:
		case MEP_PROBLEM_TIME_SERIE:
			error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
			break;
		case MEP_PROBLEM_BINARY_CLASSIFICATION:
			error_measure = MEP_BINARY_CLASSIFICATION_BEST_THRESHOLD;
			break;
		case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
			error_measure = MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR;
			break;
	}

	modified = false;

	return true;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_mutation_probability(void) const
{
	return mutation_probability;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_crossover_probability(void) const
{
	return crossover_probability;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_code_length(void) const
{
	return code_length;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_subpopulation_size(void) const
{
	return subpopulation_size;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_num_threads(void) const
{
	return num_threads;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_tournament_size(void) const
{
	return tournament_size;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_num_generations(void) const
{
	return num_generations;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_problem_type(void) const
{
	return problem_type;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_num_subpopulations(void) const
{
	return num_subpopulations;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_operators_probability(void) const
{
	return operators_probability;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_variables_probability(void) const
{
	return variables_probability;
}
//---------------------------------------------------------------------------
double t_mep_parameters::get_constants_probability(void) const
{
	return constants_probability;
}
//---------------------------------------------------------------------------
bool t_mep_parameters::get_use_validation_data(void) const
{
	return use_validation_data;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_crossover_type(void) const
{
	return crossover_type;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_random_seed(void) const
{
	return random_seed;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_num_runs(void) const
{
	return num_runs;
}
//---------------------------------------------------------------------------
bool t_mep_parameters::get_simplified_programs(void) const
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
void t_mep_parameters::set_code_length(unsigned int value)
{
	code_length = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_subpopulation_size(unsigned int value)
{
	subpopulation_size = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_threads(unsigned int value)
{
	num_threads = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_tournament_size(unsigned int value)
{
	tournament_size = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_generations(unsigned int value)
{
	num_generations = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_problem_type(unsigned int value)
{
	problem_type = value;
	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_TIME_SERIE:
		error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
		break;
	case  MEP_PROBLEM_BINARY_CLASSIFICATION:
		error_measure = MEP_BINARY_CLASSIFICATION_BEST_THRESHOLD;
		break;
	case  MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		error_measure = MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR;
		break;
	}
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_subpopulations(unsigned int value)
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
void t_mep_parameters::set_crossover_type(unsigned int value)
{
	crossover_type = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_random_seed(unsigned int value)
{
	random_seed = value;
	modified = true;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_runs(unsigned int value)
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
void t_mep_parameters::set_random_subset_selection_size_percent(unsigned int value)
{
	random_subset_selection_size_percent = value;
	modified = true;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_random_subset_selection_size_percent(void)  const
{
	return random_subset_selection_size_percent;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_error_measure(unsigned int value)
{
	switch (problem_type) {
	case MEP_PROBLEM_REGRESSION:
	case MEP_PROBLEM_TIME_SERIE:
		if (value == MEP_REGRESSION_MEAN_ABSOLUTE_ERROR || value == MEP_REGRESSION_MEAN_SQUARED_ERROR)
			error_measure = value;
		else
			error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
		break;
	case MEP_PROBLEM_BINARY_CLASSIFICATION:
		if (value == MEP_BINARY_CLASSIFICATION_BEST_THRESHOLD)
			error_measure = value;
		else
			error_measure = MEP_BINARY_CLASSIFICATION_BEST_THRESHOLD;
		break;
	case MEP_PROBLEM_MULTICLASS_CLASSIFICATION:
		if (value == MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR || value == MEP_MULTICLASS_CLASSIFICATION_SMOOTH_ERROR || value == MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_DYNAMIC_ERROR)
			error_measure = value;
		else
			error_measure = MEP_MULTICLASS_CLASSIFICATION_WINNER_TAKES_ALL_ERROR;
		break;

	}
	modified = true;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_error_measure(void) const
{
	return error_measure;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_num_generations_for_which_random_subset_is_kept_fixed(void)  const
{
	return num_generations_for_which_random_subset_is_kept_fixed;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_generations_for_which_random_subset_is_kept_fixed(unsigned int new_value)
{
	num_generations_for_which_random_subset_is_kept_fixed = new_value;
	modified = true;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_num_predictions(void) const
{
	return num_predictions;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_num_predictions(unsigned int new_value)
{
	num_predictions = new_value;
	modified = true;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_time_series_mode(void) const
{
	return time_series_mode;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_time_series_mode(unsigned int new_value)
{
	time_series_mode = new_value;
	modified = true;
}
//---------------------------------------------------------------------------
bool t_mep_parameters::operator ==(const t_mep_parameters& other)
{
	if (mutation_probability != other.mutation_probability)
		return false;

	if (crossover_probability != other.crossover_probability)
		return false;

	if (code_length != other.code_length)
		return false;

	if (subpopulation_size != other.subpopulation_size)
		return false;

	if (num_threads != other.num_threads)
		return false;

	if (tournament_size != other.tournament_size)
		return false;

	if (num_generations != other.num_generations)
		return false;

	if (problem_type != other.problem_type)
		return false;

	if (num_subpopulations != other.num_subpopulations)
		return false;

	if (operators_probability != other.operators_probability)
		return false;

	if (variables_probability != other.variables_probability)
		return false;

	if (constants_probability != other.constants_probability)
		return false;

	if (use_validation_data != other.use_validation_data)
		return false;

	if (crossover_type != other.crossover_type)
		return false;

	if (random_subset_selection_size_percent != other.random_subset_selection_size_percent)
		return false;

	if (num_generations_for_which_random_subset_is_kept_fixed != other.num_generations_for_which_random_subset_is_kept_fixed)
		return false;

	if (random_seed != other.random_seed)
		return false;

	if (error_measure != other.error_measure)
		return false;

	if (num_predictions != other.num_predictions)
		return false;

	if (time_series_mode != other.time_series_mode)
		return false;

	return true;
}
//---------------------------------------------------------------------------
unsigned int t_mep_parameters::get_window_size(void)const
{
	return window_size;
}
//---------------------------------------------------------------------------
void t_mep_parameters::set_window_size(unsigned int new_window_size)
{
	if (new_window_size) {
		window_size = new_window_size;
		modified = true;
	}
}
//---------------------------------------------------------------------------
