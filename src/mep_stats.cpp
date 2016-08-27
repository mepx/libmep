#include "mep_stats.h"
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------------
t_mep_run_statistics::t_mep_run_statistics(void)
{
	running_time = 0;
	best_training_error = NULL;
	average_training_error = NULL;
	last_generation = -1;
	best_validation_error = -1;
	test_error = -1;
}
//---------------------------------------------------------------------------
void t_mep_run_statistics::allocate(long num_generations)
{
	best_training_error = new double[num_generations];
	average_training_error = new double[num_generations];
}
//---------------------------------------------------------------------------
t_mep_run_statistics::~t_mep_run_statistics()
{
	if (best_training_error){
		delete[] best_training_error;
		best_training_error = NULL;
	}
	if (average_training_error){
		delete[] average_training_error;
		average_training_error = NULL;
	}
}
//---------------------------------------------------------------------------
int t_mep_run_statistics::to_xml(pugi::xml_node parent)
{
	char tmp_str[100];
	pugi::xml_node node = parent.append_child("running_time");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", running_time);
	data.set_value(tmp_str);

	pugi::xml_node best_training_error_node = parent.append_child("best_training_error");
	for (int g = 0; g <= last_generation; g++) {
		pugi::xml_node node = best_training_error_node.append_child("e");
		pugi::xml_node data = node.append_child(pugi::node_pcdata);
		sprintf(tmp_str, "%lg", best_training_error[g]);
		data.set_value(tmp_str);
	}

	pugi::xml_node average_training_error_node = parent.append_child("average_training_error");
	for (int g = 0; g <= last_generation; g++) {
		pugi::xml_node node = average_training_error_node.append_child("e");
		pugi::xml_node data = node.append_child(pugi::node_pcdata);
		sprintf(tmp_str, "%lg", average_training_error[g]);
		data.set_value(tmp_str);
	}

	node = parent.append_child("best_validation_error");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", best_validation_error);
	data.set_value(tmp_str);

	node = parent.append_child("test_error");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%lg", test_error);
	data.set_value(tmp_str);

	node = parent.append_child("last_generation");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str, "%d", last_generation);
	data.set_value(tmp_str);

	node = parent.append_child("program");
	best_program.to_xml(node);

	return true;
}
//---------------------------------------------------------------------------
int t_mep_run_statistics::from_xml(pugi::xml_node parent, int num_generations, int code_length, int problem_type)
{
	pugi::xml_node node = parent.child("running_time");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		running_time = atof(value_as_cstring);
	}

	best_training_error = new double[num_generations];
	average_training_error = new double[num_generations];

	// evolution
	pugi::xml_node best_training_error_node = parent.child("best_training_error");
	if (best_training_error_node)
	{
		int gen = 0;
		for (pugi::xml_node row = best_training_error_node.child("e"); row; row = row.next_sibling("e"), gen++){
			const char *value_as_cstring = row.child_value();
			best_training_error[gen] = atof(value_as_cstring);
		}
		if (!gen)
			for (pugi::xml_node row = best_training_error_node.child("error"); row; row = row.next_sibling("error"), gen++) {
				const char *value_as_cstring = row.child_value();
				best_training_error[gen] = atof(value_as_cstring);
			}
	}
	else
		for (int g = 0; g < num_generations; g++)
			best_training_error[g] = -1;


	pugi::xml_node average_training_error_node = parent.child("average_training_error");
	if (average_training_error_node)
	{
		int gen = 0;
		for (pugi::xml_node row = average_training_error_node.child("e"); row; row = row.next_sibling("e"), gen++){
			const char *value_as_cstring = row.child_value();
			average_training_error[gen] = atof(value_as_cstring);
		}
		if (!gen)
			for (pugi::xml_node row = average_training_error_node.child("error"); row; row = row.next_sibling("error"), gen++) {
				const char *value_as_cstring = row.child_value();
				average_training_error[gen] = atof(value_as_cstring);
			}
	}
	else
		for (int g = 0; g < num_generations; g++)
			average_training_error[g] = -1;

	node = parent.child("best_validation_error");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		best_validation_error = atof(value_as_cstring);
	}

	node = parent.child("test_error");
	if (node) {
		const char *value_as_cstring = node.child_value();
		test_error = atof(value_as_cstring);
	}

	node = parent.child("last_generation");
	if (node)
	{
		const char *value_as_cstring = node.child_value();
		last_generation = atoi(value_as_cstring);
	}
	else
		last_generation = -1;

	node = parent.child("program");
	if (node)
	{
		best_program.from_xml(node);
		if (problem_type != MEP_PROBLEM_MULTICLASS_CLASSIFICATION)
			best_program.simplify();
	}


	return true;
}
//---------------------------------------------------------------------------
