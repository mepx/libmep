// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//---------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif
//---------------------------------------------------------------------------
#include "mep_stats.h"
#include "mep_parameters.h"
//---------------------------------------------------------------------------
t_mep_single_run_statistics::t_mep_single_run_statistics(void)
{
	running_time = 0;
	best_training_error = NULL;
	average_training_error = NULL;
	last_generation = -1;
	best_validation_error = -1;
	test_error = -1;

	best_training_num_incorrect = NULL;
	best_validation_num_incorrect = -1;
	test_num_incorrect = -1;
	average_training_num_incorrect = NULL;

	validation_error = NULL;
	validation_num_incorrect = NULL;
	
	num_generations = 0;
}
//---------------------------------------------------------------------------
void t_mep_single_run_statistics::allocate_memory(unsigned int _num_generations)
{
	num_generations = _num_generations;
	
	best_training_error = new double[num_generations];
	average_training_error = new double[num_generations];

	best_training_num_incorrect = new double[num_generations];
	average_training_num_incorrect = new double[num_generations];
	
	validation_error = new double[num_generations];
	validation_num_incorrect = new double[num_generations];
	
	for (unsigned int i = 0; i < num_generations; i++)
		validation_error[i] = validation_num_incorrect[i] = -1;
}
//---------------------------------------------------------------------------
t_mep_single_run_statistics::~t_mep_single_run_statistics()
{
	delete_memory();
}
//---------------------------------------------------------------------------
void t_mep_single_run_statistics::delete_memory(void)
{
	if (best_training_error) {
		delete[] best_training_error;
		best_training_error = NULL;
	}
	if (average_training_error) {
		delete[] average_training_error;
		average_training_error = NULL;
	}
	if (best_training_num_incorrect) {
		delete[] best_training_num_incorrect;
		best_training_num_incorrect = NULL;
	}
	if (average_training_num_incorrect) {
		delete[] average_training_num_incorrect;
		average_training_num_incorrect = NULL;
	}
	
	if (validation_error) {
		delete[] validation_error;
		validation_error = NULL;
	}

	if (validation_num_incorrect) {
		delete[] validation_num_incorrect;
		validation_num_incorrect = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep_single_run_statistics::to_xml_node(pugi::xml_node parent) const
{
	char tmp_str2[100];
	pugi::xml_node node = parent.append_child("running_time");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str2, 100, "%lg", running_time);
	data.set_value(tmp_str2);

	char *tmp_str = new char[num_generations * 21 + 1];

	if (best_training_error) {
		pugi::xml_node best_training_error_node = parent.append_child("best_training_error");

		data = best_training_error_node.append_child(pugi::node_pcdata);
		tmp_str[0] = 0;
		for (int g = 0; g <= last_generation; g++) {
			char tmp_s[30];
			snprintf(tmp_s, 30, "%lg", best_training_error[g]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		//now the target if there is one...
		data.set_value(tmp_str);
	}

	if (average_training_error) {
		pugi::xml_node average_training_error_node = parent.append_child("average_training_error");

		data = average_training_error_node.append_child(pugi::node_pcdata);
		tmp_str[0] = 0;
		for (int g = 0; g <= last_generation; g++) {
			char tmp_s[30];
			snprintf(tmp_s, 30, "%lg", average_training_error[g]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		//now the target if there is one...
		data.set_value(tmp_str);
	}
	
	if (validation_error && validation_error[0] >= 0) {
		pugi::xml_node validation_error_node = parent.append_child("validation_error");

		data = validation_error_node.append_child(pugi::node_pcdata);
		tmp_str[0] = 0;
		for (int g = 0; g <= last_generation; g++) {
			char tmp_s[30];
			snprintf(tmp_s, 30, "%lg", validation_error[g]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		//now the target if there is one...
		data.set_value(tmp_str);
	}

	if (validation_num_incorrect && validation_num_incorrect[0] >= 0) {
		pugi::xml_node validation_num_incorrect_node = parent.append_child("validation_num_incorrect");

		data = validation_num_incorrect_node.append_child(pugi::node_pcdata);
		tmp_str[0] = 0;
		for (int g = 0; g <= last_generation; g++) {
			char tmp_s[30];
			snprintf(tmp_s, 30, "%lg", validation_num_incorrect[g]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		//now the target if there is one...
		data.set_value(tmp_str);
	}

	node = parent.append_child("best_validation_error");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str2, 100, "%lg", best_validation_error);
	data.set_value(tmp_str2);

	node = parent.append_child("test_error");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str2, 100, "%lg", test_error);
	data.set_value(tmp_str2);

	if (best_training_num_incorrect) {
		pugi::xml_node best_training_num_incorrect_node = parent.append_child("best_training_num_incorrect");

		data = best_training_num_incorrect_node.append_child(pugi::node_pcdata);
		tmp_str[0] = 0;
		for (int g = 0; g <= last_generation; g++) {
			char tmp_s[30];
			snprintf(tmp_s, 30, "%lf", best_training_num_incorrect[g]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		//now the target if there is one...
		data.set_value(tmp_str);
	}

	if (average_training_num_incorrect) {
		pugi::xml_node average_training_num_incorrect_node = parent.append_child("average_training_num_incorrect");

		data = average_training_num_incorrect_node.append_child(pugi::node_pcdata);
		tmp_str[0] = 0;
		for (int g = 0; g <= last_generation; g++) {
			char tmp_s[30];
			snprintf(tmp_s, 30, "%lf", average_training_num_incorrect[g]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		//now the target if there is one...
		data.set_value(tmp_str);
	}

	node = parent.append_child("best_validation_num_incorrect");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str2, 100, "%lg", best_validation_num_incorrect);
	data.set_value(tmp_str2);

	node = parent.append_child("test_num_incorrect");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str2, 100, "%lg", test_num_incorrect);
	data.set_value(tmp_str2);

	node = parent.append_child("last_generation");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str2, 100, "%d", last_generation);
	data.set_value(tmp_str2);

	node = parent.append_child("num_generations");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str2, 100, "%u", num_generations);
	data.set_value(tmp_str2);

	node = parent.append_child("program");
	best_program.to_xml_node(node);

	delete[] tmp_str;
}
//---------------------------------------------------------------------------
bool t_mep_single_run_statistics::from_xml_node(pugi::xml_node parent,
								   unsigned int problem_type,
										  unsigned int error_measure,
								   unsigned int num_classes,
										  unsigned int code_length)
{
	delete_memory();

	pugi::xml_node node = parent.child("running_time");
	if (node) {
		const char *value_as_cstring = node.child_value();
		running_time = atof(value_as_cstring);
	}


	node = parent.child("last_generation");
	if (node) {
		const char *value_as_cstring = node.child_value();
		last_generation = atoi(value_as_cstring);
	}
	else
		last_generation = -1;

	node = parent.child("num_generations");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_generations = (unsigned int) atoi(value_as_cstring);
	}
	else
		num_generations = 0;

	if (num_generations == 0)
		num_generations = (unsigned int)(last_generation + 1);

	// evolution
	pugi::xml_node best_training_error_node = parent.child("best_training_error");
	if (best_training_error_node) {
		best_training_error = new double[num_generations];
		int gen = 0;
		for (pugi::xml_node row = best_training_error_node.child("e"); row; row = row.next_sibling("e"), gen++) {
			const char *value_as_cstring = row.child_value();
			best_training_error[gen] = atof(value_as_cstring);
		}
		if (!gen)
			for (pugi::xml_node row = best_training_error_node.child("error"); row; row = row.next_sibling("error"), gen++) {
				const char *value_as_cstring = row.child_value();
				best_training_error[gen] = atof(value_as_cstring);
			}
		if (!gen) {
			size_t num_jumped_chars = 0;
			const char *value_as_cstring = best_training_error_node.child_value();
			for (gen = 0; gen <= last_generation; gen++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &best_training_error[gen]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
	}

	pugi::xml_node average_training_error_node = parent.child("average_training_error");
	if (average_training_error_node) {
		average_training_error = new double[num_generations];
		int gen = 0;
		for (pugi::xml_node row = average_training_error_node.child("e"); row; row = row.next_sibling("e"), gen++) {
			const char *value_as_cstring = row.child_value();
			average_training_error[gen] = atof(value_as_cstring);
		}
		if (!gen)
			for (pugi::xml_node row = average_training_error_node.child("error"); row; row = row.next_sibling("error"), gen++) {
				const char *value_as_cstring = row.child_value();
				average_training_error[gen] = atof(value_as_cstring);
			}
		if (!gen) {
			size_t num_jumped_chars = 0;
			const char *value_as_cstring = average_training_error_node.child_value();
			for (gen = 0; gen <= last_generation; gen++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &average_training_error[gen]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
	}
// validation
	pugi::xml_node validation_error_node = parent.child("validation_error");
	if (validation_error_node) {
		validation_error = new double[num_generations];
		int gen = 0;
		for (pugi::xml_node row = validation_error_node.child("e"); row; row = row.next_sibling("e"), gen++) {
			const char *value_as_cstring = row.child_value();
			validation_error[gen] = atof(value_as_cstring);
		}
		if (!gen)
			for (pugi::xml_node row = validation_error_node.child("error"); row; row = row.next_sibling("error"), gen++) {
				const char *value_as_cstring = row.child_value();
				validation_error[gen] = atof(value_as_cstring);
			}
		if (!gen) {
			size_t num_jumped_chars = 0;
			const char *value_as_cstring = validation_error_node.child_value();
			for (gen = 0; gen <= last_generation; gen++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &validation_error[gen]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
	}
	
	pugi::xml_node validation_num_incorrect_node = parent.child("validation_num_incorrect");
	if (validation_num_incorrect_node) {
		validation_num_incorrect = new double[num_generations];
		int gen = 0;
		for (pugi::xml_node row = validation_num_incorrect_node.child("e"); row; row = row.next_sibling("e"), gen++) {
			const char *value_as_cstring = row.child_value();
			validation_num_incorrect[gen] = atof(value_as_cstring);
		}
		if (!gen)
			for (pugi::xml_node row = validation_num_incorrect_node.child("error"); row; row = row.next_sibling("error"), gen++) {
				const char *value_as_cstring = row.child_value();
				validation_num_incorrect[gen] = atof(value_as_cstring);
			}
		if (!gen) {
			size_t num_jumped_chars = 0;
			const char *value_as_cstring = validation_num_incorrect_node.child_value();
			for (gen = 0; gen <= last_generation; gen++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &validation_num_incorrect[gen]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
	}

	node = parent.child("best_validation_error");
	if (node) {
		const char *value_as_cstring = node.child_value();
		best_validation_error = atof(value_as_cstring);
	}

	node = parent.child("test_error");
	if (node) {
		const char *value_as_cstring = node.child_value();
		test_error = atof(value_as_cstring);
	}

	// check if num incorrect is available
	pugi::xml_node best_training_num_incorrect_node = parent.child("best_training_num_incorrect");


	if (best_training_num_incorrect_node) {
		best_training_num_incorrect = new double[num_generations];
		int gen = 0;
		for (pugi::xml_node row = best_training_num_incorrect_node.child("e"); row; row = row.next_sibling("e"), gen++) {
			const char *value_as_cstring = row.child_value();
			best_training_num_incorrect[gen] = atof(value_as_cstring);
		}
		if (!gen)
			for (pugi::xml_node row = best_training_num_incorrect_node.child("error"); row; row = row.next_sibling("error"), gen++) {
				const char *value_as_cstring = row.child_value();
				best_training_num_incorrect[gen] = atof(value_as_cstring);
			}
		if (!gen) {
			size_t num_jumped_chars = 0;
			const char *value_as_cstring = best_training_num_incorrect_node.child_value();
			for (gen = 0; gen <= last_generation; gen++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &best_training_num_incorrect[gen]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}

	}
	/*
			else
				for (int g = 0; g < num_generations; g++)
					best_training_num_incorrect[g] = -1;
					*/

	pugi::xml_node average_training_num_incorrect_node = parent.child("average_training_num_incorrect");
	if (average_training_num_incorrect_node) {
		average_training_num_incorrect = new double[num_generations];

		int gen = 0;
		for (pugi::xml_node row = average_training_error_node.child("e"); row; row = row.next_sibling("e"), gen++) {
			const char *value_as_cstring = row.child_value();
			average_training_num_incorrect[gen] = atof(value_as_cstring);
		}
		if (!gen)
			for (pugi::xml_node row = average_training_error_node.child("error"); row; row = row.next_sibling("error"), gen++) {
				const char *value_as_cstring = row.child_value();
				average_training_num_incorrect[gen] = atof(value_as_cstring);
			}

		if (!gen) {
			size_t num_jumped_chars = 0;
			const char *value_as_cstring = average_training_num_incorrect_node.child_value();
			for (gen = 0; gen <= last_generation; gen++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &average_training_num_incorrect[gen]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
	}
	/*
	else
		for (int g = 0; g < num_generations; g++)
			average_training_num_incorrect[g] = -1;
			*/
	node = parent.child("best_validation_num_incorrect");
	if (node) {
		const char *value_as_cstring = node.child_value();
		best_validation_num_incorrect = atof(value_as_cstring);
	}

	node = parent.child("test_num_incorrect");
	if (node) {
		const char *value_as_cstring = node.child_value();
		test_num_incorrect = atof(value_as_cstring);
	}

	node = parent.child("program");
	if (node){
		best_program.from_xml_node(node, problem_type, error_measure, num_classes, code_length);

		if (problem_type != MEP_PROBLEM_MULTICLASS_CLASSIFICATION ||
			(problem_type == MEP_PROBLEM_MULTICLASS_CLASSIFICATION &&
			error_measure == MEP_MULTICLASS_CLASSIFICATION_CLOSEST_CENTER_ERROR))
			best_program.simplify();
	}

	return true;
}
//---------------------------------------------------------------------------
t_mep_single_run_statistics& t_mep_single_run_statistics::operator=(const t_mep_single_run_statistics& source)
{
	if (&source != this) {
		delete_memory();
		allocate_memory(source.num_generations);

		running_time = source.running_time;
		for (unsigned int i = 0; i < num_generations; i++) {
			best_training_error[i] = source.best_training_error[i];
			average_training_error[i] = source.average_training_error[i];
			best_training_num_incorrect[i] = source.best_training_num_incorrect[i];
			average_training_num_incorrect[i] = source.average_training_num_incorrect[i];
		}
		if (source.validation_error)
			for (unsigned int i = 0; i < num_generations; i++)
				validation_error[i] = source.validation_error[i];
		if (source.validation_num_incorrect)
			for (unsigned int i = 0; i < num_generations; i++)
				validation_num_incorrect[i] = source.validation_num_incorrect[i];
		
		last_generation = source.last_generation;
		best_validation_error = source.best_validation_error;
		test_error = source.test_error;

		best_validation_num_incorrect = source.best_validation_num_incorrect;
		test_num_incorrect = source.test_num_incorrect;
		num_generations = source.num_generations;
	}

	return *this;
}
//---------------------------------------------------------------------------
t_mep_all_runs_statistics::t_mep_all_runs_statistics()
{
	stats = NULL;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::compute_mean_stddev(
										bool compute_on_validation,
										bool compute_on_test)
{
	stddev_training_error = stddev_validation_error = stddev_test_error = -1;
	mean_training_error = mean_validation_error = mean_test_error = -1;
	best_training_error = best_validation_error = best_test_error = -1;

	stddev_runtime = 0;
	mean_runtime = 0;
	best_runtime = 0;
	
	best_code_length = -1;
	mean_code_length = 0;
	stddev_code_length = 0;
	
	if (!num_runs)
		return;
	
	unsigned int problem_type = stats[0].best_program.get_problem_type();

	best_training_error = stats[0].best_training_error[stats[0].last_generation];
	mean_training_error = 0;
	stddev_training_error = 0;
	
	
	
	if (compute_on_validation){
		best_validation_error = stats[0].best_validation_error;
		stddev_validation_error = 0;
		mean_validation_error = 0;
	}
	if (compute_on_test){
		best_test_error = stats[0].test_error;
		mean_test_error = 0;
		stddev_test_error = 0;
	}
	// mean and best
	best_runtime = stats[0].running_time;
	mean_runtime = stats[0].running_time;

	best_code_length = stats[0].best_program.get_num_utilized_genes();
	mean_code_length = best_code_length;
	
	mean_training_error = stats[0].best_training_error[stats[0].last_generation];
	if (compute_on_validation)
		mean_validation_error = stats[0].best_validation_error;
	if (compute_on_test)
		mean_test_error = stats[0].test_error;
	
	for (unsigned int r = 1; r < num_runs; r++) {
		mean_training_error += stats[r].best_training_error[stats[r].last_generation];
		if (best_training_error > stats[r].best_training_error[stats[r].last_generation])
			best_training_error = stats[r].best_training_error[stats[r].last_generation];
		
		mean_code_length += stats[r].best_program.get_num_utilized_genes();
		if (best_code_length > stats[r].best_program.get_num_utilized_genes())
			best_code_length = stats[r].best_program.get_num_utilized_genes();
			
		if (compute_on_validation) {
			mean_validation_error += stats[r].best_validation_error;
			if (best_validation_error > stats[r].best_validation_error)
				best_validation_error = stats[r].best_validation_error;
		}
		if (compute_on_test) {
			mean_test_error += stats[r].test_error;
			if (best_test_error > stats[r].test_error)
				best_test_error = stats[r].test_error;
		}

		if (best_runtime > stats[r].running_time)
			best_runtime = stats[r].running_time;

		mean_runtime += stats[r].running_time;
	}
	mean_training_error /= num_runs;
	if (compute_on_validation)
		mean_validation_error /= num_runs;
	if (compute_on_test)
		mean_test_error /= num_runs;
	mean_runtime /= num_runs;
	mean_code_length /= num_runs;

	// compute stddev

	for (unsigned int r = 0; r < num_runs; r++) {
		stddev_training_error += (mean_training_error - stats[r].best_training_error[stats[r].last_generation]) * (mean_training_error - stats[r].best_training_error[stats[r].last_generation]);
		if (compute_on_validation)
			stddev_validation_error += (mean_validation_error - stats[r].best_validation_error) * (mean_validation_error - stats[r].best_validation_error);
		if (compute_on_test)
			stddev_test_error += (mean_test_error - stats[r].test_error) * (mean_test_error - stats[r].test_error);
		stddev_runtime += (mean_runtime - stats[r].running_time) * (mean_runtime - stats[r].running_time);
		stddev_code_length += (mean_code_length - stats[r].best_program.get_num_utilized_genes()) *
							(mean_code_length - stats[r].best_program.get_num_utilized_genes());
	}
	stddev_training_error = sqrt(stddev_training_error / num_runs);
	if (compute_on_validation)
		stddev_validation_error = sqrt(stddev_validation_error / num_runs);
	if (compute_on_test)
		stddev_test_error = sqrt(stddev_test_error / num_runs);
	stddev_runtime = sqrt(stddev_runtime / num_runs);
	stddev_code_length = sqrt(stddev_code_length / num_runs);

	if (problem_type == MEP_PROBLEM_BINARY_CLASSIFICATION || 
		problem_type == MEP_PROBLEM_MULTICLASS_CLASSIFICATION) {

		stddev_training_num_incorrect = stddev_validation_num_incorrect = stddev_test_num_incorrect = -1;
		mean_training_num_incorrect = mean_validation_num_incorrect = mean_test_num_incorrect = -1;
		best_training_num_incorrect = best_validation_num_incorrect = best_test_num_incorrect = -1;

		if (stats[0].best_training_num_incorrect) {
			best_training_num_incorrect = stats[0].best_training_num_incorrect[stats[0].last_generation];
			stddev_training_num_incorrect = 0;
			
			if (compute_on_validation){
				best_validation_num_incorrect = stats[0].best_validation_num_incorrect;
				mean_validation_num_incorrect = 0;
				stddev_validation_num_incorrect = 0;
			}
			if (compute_on_test){
				best_test_num_incorrect = stats[0].test_num_incorrect;
				stddev_test_num_incorrect = 0;
				mean_test_num_incorrect = 0;
			}
			// mean
			mean_training_num_incorrect = stats[0].best_training_num_incorrect[stats[0].last_generation];
			if (compute_on_validation)
				mean_validation_num_incorrect = stats[0].best_validation_num_incorrect;
			if (compute_on_test)
				mean_test_num_incorrect = stats[0].test_num_incorrect;

			for (unsigned int r = 1; r < num_runs; r++) {
				mean_training_num_incorrect += stats[r].best_training_num_incorrect[stats[r].last_generation];
				if (best_training_num_incorrect > stats[r].best_training_num_incorrect[stats[r].last_generation])
					best_training_num_incorrect = stats[r].best_training_num_incorrect[stats[r].last_generation];
				if (compute_on_validation) {
					mean_validation_num_incorrect += stats[r].best_validation_num_incorrect;
					if (best_validation_num_incorrect > stats[r].best_validation_num_incorrect)
						best_validation_num_incorrect = stats[r].best_validation_num_incorrect;
				}
				if (compute_on_test) {
					mean_test_num_incorrect += stats[r].test_num_incorrect;
					if (best_test_num_incorrect > stats[r].test_num_incorrect)
						best_test_num_incorrect = stats[r].test_num_incorrect;
				}

			}
			mean_training_num_incorrect /= num_runs;
			if (compute_on_validation)
				mean_validation_num_incorrect /= num_runs;
			if (compute_on_test)
				mean_test_num_incorrect /= num_runs;

			// compute stddev

			for (unsigned int r = 0; r < num_runs; r++) {
				stddev_training_num_incorrect += (mean_training_num_incorrect - stats[r].best_training_num_incorrect[stats[r].last_generation]) * (mean_training_num_incorrect - stats[r].best_training_num_incorrect[stats[r].last_generation]);
				if (compute_on_validation)
					stddev_validation_num_incorrect += (mean_validation_num_incorrect - stats[r].best_validation_num_incorrect) * (mean_validation_num_incorrect - stats[r].best_validation_num_incorrect);
				if (compute_on_test)
					stddev_test_num_incorrect += (mean_test_num_incorrect - stats[r].test_num_incorrect) * (mean_test_num_incorrect - stats[r].test_num_incorrect);
			}
			stddev_training_num_incorrect = sqrt(stddev_training_num_incorrect / num_runs);
			if (compute_on_validation)
				stddev_validation_num_incorrect = sqrt(stddev_validation_num_incorrect / num_runs);
			if (compute_on_test)
				stddev_test_num_incorrect = sqrt(stddev_test_num_incorrect / num_runs);
		}
	}
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::get_mean_error(double &training,
											   double &validation,
											   double &test,
											   double &running_time,
											   double &code_length) const
{
	training = mean_training_error;
	validation = mean_validation_error;
	test = mean_test_error;
	running_time = mean_runtime;
	code_length = mean_code_length;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::get_sttdev_error(double &training,
												 double &validation,
												 double &test,
												 double &running_time,
												 double &code_length) const
{
	training = stddev_training_error;
	validation = stddev_validation_error;
	test = stddev_test_error;
	running_time = stddev_runtime;
	code_length = stddev_code_length;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::get_best_error(double &training,
											   double &validation,
											   double &test,
											   double &running_time,
											   int& code_length) const
{
	training = best_training_error;
	validation = best_validation_error;
	test = best_test_error;
	running_time = best_runtime;
	code_length = best_code_length;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::get_mean_num_incorrect(double &training,
											  double &validation,
											  double &test,
											  double &running_time,
											double &code_length) const
{
	training = mean_training_num_incorrect;
	validation = mean_validation_num_incorrect;
	test = mean_test_num_incorrect;
	running_time = mean_runtime;
	code_length = mean_code_length;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::get_sttdev_num_incorrect(double &training,
														 double &validation,
														 double &test,
														 double &running_time,
														 double &code_length) const
{
	training = stddev_training_num_incorrect;
	validation = stddev_validation_num_incorrect;
	test = stddev_test_num_incorrect;
	running_time = stddev_runtime;
	code_length = stddev_code_length;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::get_best_num_incorrect(double &training,
													   double &validation,
													   double &test,
													   double &running_time,
													   int& code_length) const
{
	training = best_training_num_incorrect;
	validation = best_validation_num_incorrect;
	test = best_test_num_incorrect;
	running_time = best_runtime;
	code_length = best_code_length;
}
//---------------------------------------------------------------------------
// returns the best training num incorreclty classified
double t_mep_all_runs_statistics::get_best_training_num_incorrectly_classified(unsigned int run, unsigned int generation) const
{
	if (!stats[run].best_training_num_incorrect)
		return -1;
	if ((int)generation <= stats[run].last_generation)
		return stats[run].best_training_num_incorrect[generation];
	else
		return stats[run].best_training_num_incorrect[stats[run].last_generation];
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_validation_num_incorrectly_classified(unsigned int run, unsigned int generation) const
{
	if (!stats[run].validation_num_incorrect)
		return -1;
	if ((int)generation <= stats[run].last_generation)
		return stats[run].validation_num_incorrect[generation];
	else
		return stats[run].validation_num_incorrect[stats[run].last_generation];
}
//---------------------------------------------------------------------------
// returns the best validation num incorreclty classified
double t_mep_all_runs_statistics::get_best_validation_num_incorrectly_classified(unsigned int run) const
{
	return stats[run].best_validation_num_incorrect;
}
//---------------------------------------------------------------------------
// returns the average (over the entire population) training num incorreclty classified
double t_mep_all_runs_statistics::get_mean_training_num_incorrectly_classified(unsigned int run, unsigned int generation) const
{
	return stats[run].average_training_num_incorrect[generation];
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_stddev_training_num_incorrectly_classified(unsigned int /*run*/, unsigned int /*generation*/)
{
	return 0;// stats[run].stddev_training_num_incorrect[generation];
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_test_num_incorrectly_classified(unsigned int run) const
{
	return stats[run].test_num_incorrect;
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_best_training_error(unsigned int run, unsigned int gen) const
{
	if (!stats[run].best_training_error)
		return -1;
	
	if ((int)gen <= stats[run].last_generation)
		return stats[run].best_training_error[gen];
	else
		return stats[run].best_training_error[stats[run].last_generation];
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_validation_error(unsigned int run, unsigned int gen) const
{
	if (!stats[run].validation_error)
		return -1;
	
	if ((int)gen <= stats[run].last_generation)
		return stats[run].validation_error[gen];
	else
		return stats[run].validation_error[stats[run].last_generation];
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_best_validation_error(unsigned int run) const
{
	//mep_alg.get_validation_data_ptr()->get_num_rows() &&
		//mep_alg.get_parameters_ptr()->get_use_validation_data()
	
	return stats[run].best_validation_error;
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_test_error(unsigned int run) const
{
	return stats[run].test_error;
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_average_training_error(unsigned int run, unsigned int gen) const
{
	if (stats[run].average_training_error)
		if ((int)gen <= stats[run].last_generation)
			return stats[run].average_training_error[gen];
		else
			return stats[run].average_training_error[stats[run].last_generation];
	else
		return -1;
}
//---------------------------------------------------------------------------
int t_mep_all_runs_statistics::get_latest_generation(unsigned int run) const
{
	return stats[run].last_generation;
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_running_time(unsigned int run) const
{
	return stats[run].running_time;
}
//---------------------------------------------------------------------------
double t_mep_all_runs_statistics::get_num_utilized_instructions(unsigned int run) const
{
	return stats[run].best_program.get_num_utilized_genes();
}
//---------------------------------------------------------------------------
bool t_mep_all_runs_statistics::to_csv_file(const char *filename, unsigned int problem_type)const
{
	FILE *f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"w");
	delete[] w_filename;

#else
	f = fopen(filename, "w");
#endif

	if (!f)
		return false;
	if (problem_type == MEP_PROBLEM_REGRESSION || problem_type == MEP_PROBLEM_TIME_SERIE) {
		fprintf(f, "Run #;Training error; Validation error; Test error; Num. Instructions; Running time (s)\n");
		for (unsigned int r = 0; r < num_runs; r++) {
			fprintf(f, "%u;%lf;", r + 1, stats[r].best_training_error[stats[r].last_generation]);
			if (stats[r].best_validation_error > -1E-6)
				fprintf(f, "%lf;", stats[r].best_validation_error);
			else
				fprintf(f, ";");
			if (stats[r].test_error > -1E-6)
				fprintf(f, "%lf;", stats[r].test_error);
			else
				fprintf(f, ";");
			fprintf(f, "%d;", stats[r].best_program.get_num_utilized_genes());
			fprintf(f, "%lf\n", stats[r].running_time);
		}
		// best
		fprintf(f, "Best;%lf;", best_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "%lf;", best_validation_error);
		else
			fprintf(f, ";");
		if (best_test_error > -1E-6)
			fprintf(f, "%lf;", best_test_error);
		else
			fprintf(f, ";");
		fprintf(f, "%d;", best_code_length);
		fprintf(f, "%lf\n", best_runtime);
		// average
		fprintf(f, "Average;%lf;", mean_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "%lf;", mean_validation_error);
		else
			fprintf(f, ";");
		if (best_test_error > -1E-6)
			fprintf(f, "%lf;", mean_test_error);
		else
			fprintf(f, ";");
		fprintf(f, "%lf;", mean_code_length);
		fprintf(f, "%lf\n", mean_runtime);

		//std dev
		fprintf(f, "StdDev;%lf;", stddev_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "%lf;", stddev_validation_error);
		else
			fprintf(f, ";");
		if (best_test_error > -1E-6)
			fprintf(f, "%lf;", stddev_test_error);
		else
			fprintf(f, ";");
		fprintf(f, "%lf;", stddev_code_length);
		fprintf(f, "%lf\n", stddev_runtime);
	}
	else {// MEP CLASSIFICATION
		fprintf(f, "#;training num incorrect (%%); validation num incorrect (%%); test num incorrect (%%); running time (s)\n");
		for (unsigned int r = 0; r < num_runs; r++) {
			fprintf(f, "%u;%lf;", r + 1, stats[r].best_training_num_incorrect[stats[r].last_generation]);
			if (stats[r].best_validation_num_incorrect > -1E-6)
				fprintf(f, "%lf;", stats[r].best_validation_num_incorrect);
			else
				fprintf(f, ";");
			if (stats[r].test_num_incorrect > -1E-6)
				fprintf(f, "%lf;", stats[r].test_num_incorrect);
			else
				fprintf(f, ";");
			fprintf(f, "%d;", stats[r].best_program.get_num_utilized_genes());
			fprintf(f, "%lf\n", stats[r].running_time);
		}

		// best
		fprintf(f, "Best;%lf;", best_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "%lf;", best_validation_num_incorrect);
		else
			fprintf(f, ";");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "%lf;", best_test_num_incorrect);
		else
			fprintf(f, ";");
		fprintf(f, "%d;", best_code_length);
		fprintf(f, "%lf\n", best_runtime);

		// average
		fprintf(f, "Average;%lf;", mean_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "%lf;", mean_validation_num_incorrect);
		else
			fprintf(f, ";");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "%lf;", mean_test_num_incorrect);
		else
			fprintf(f, ";");
		fprintf(f, "%lf;", mean_code_length);
		fprintf(f, "%lf\n", mean_runtime);

		//std dev
		fprintf(f, "StdDev;%lf;", stddev_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "%lf;", stddev_validation_num_incorrect);
		else
			fprintf(f, ";");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "%lf;", stddev_test_num_incorrect);
		else
			fprintf(f, ";");
		fprintf(f, "%lf;", stddev_code_length);
		fprintf(f, "%lf\n", stddev_runtime);
	}

	fclose(f);
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_all_runs_statistics::to_tex_file(const char *filename, unsigned int problem_type) const
{
	FILE *f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t *w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"w");
	delete[] w_filename;

#else
	f = fopen(filename, "w");
#endif

	if (!f)
		return false;

	fprintf(f, "\\begin{tabular}{c c c c c c}\n");
	if (problem_type == MEP_PROBLEM_REGRESSION || problem_type == MEP_PROBLEM_TIME_SERIE) {
		fprintf(f, "Run \\# & Training error & Validation error & Test error & Num. instructions & Running time(s)\\\\ \n");
		for (unsigned int r = 0; r < num_runs; r++) {
			fprintf(f, "%u & %lf & ", r + 1, stats[r].best_training_error[stats[r].last_generation]);

			if (stats[r].best_validation_error > -1E-6)
				fprintf(f, "%lf & ", stats[r].best_validation_error);
			else
				fprintf(f, "& ");
			if (stats[r].test_error > -1E-6)
				fprintf(f, "%lf & ", stats[r].test_error);
			else
				fprintf(f, "& ");

			fprintf(f, "%d & ", stats[r].best_program.get_num_utilized_genes());
			fprintf(f, "%lf\\\\ \n", stats[r].running_time);
		}

		// best
		fprintf(f, "Best & %lf & ", best_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "%lf & ", best_validation_error);
		else
			fprintf(f, "& ");
		if (best_test_error > -1E-6)
			fprintf(f, "%lf & ", best_test_error);
		else
			fprintf(f, "& ");
		fprintf(f, "%d & ", best_code_length);
		fprintf(f, "%lf\\\\ \n", best_runtime);

		// average
		fprintf(f, "Average & %lf & ", mean_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "%lf & ", mean_validation_error);
		else
			fprintf(f, "& ");
		if (best_test_error > -1E-6)
			fprintf(f, "%lf & ", mean_test_error);
		else
			fprintf(f, "& ");
		fprintf(f, "%lf & ", mean_code_length);
		fprintf(f, "%lf\\\\ \n", mean_runtime);

		//std dev
		fprintf(f, "StdDev & %lf & ", stddev_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "%lf & ", stddev_validation_error);
		else
			fprintf(f, "& ");
		if (best_test_error > -1E-6)
			fprintf(f, "%lf & ", stddev_test_error);
		else
			fprintf(f, "& ");
		fprintf(f, "%lf & ", stddev_code_length);
		fprintf(f, "%lf\\\\ \n", stddev_runtime);
	}
	else {// MEP CLASSIFICATION
		fprintf(f, "Run \\# & training num incorrect (\\%%) & validation num incorrect (\\%%) & test num incorrect (\\%%) & running time\\\\ \n");
		for (unsigned int r = 0; r < num_runs; r++) {
			fprintf(f, "%u & %lf &", r + 1, stats[r].best_training_num_incorrect[stats[r].last_generation]);
			if (stats[r].best_validation_num_incorrect > -1E-6)
				fprintf(f, "%lf & ", stats[r].best_validation_num_incorrect);
			else
				fprintf(f, "& ");

			if (stats[r].test_error > -1E-6)
				fprintf(f, "%lf &", stats[r].test_num_incorrect);
			else
				fprintf(f, "& ");
			fprintf(f, "%d & ", stats[r].best_program.get_num_utilized_genes());
			fprintf(f, "%lf\\\\ \n", stats[r].running_time);
		}
		
		// best
		fprintf(f, "Best & %lf & ", best_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "%lf & ", best_validation_num_incorrect);
		else
			fprintf(f, "& ");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "%lf & ", best_test_num_incorrect);
		else
			fprintf(f, "& ");
		fprintf(f, "%d & ", best_code_length);
		fprintf(f, "%lf\\\\ \n", best_runtime);

		// average
		fprintf(f, "Average & %lf & ", mean_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "%lf & ", mean_validation_num_incorrect);
		else
			fprintf(f, "& ");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "%lf & ", mean_test_num_incorrect);
		else
			fprintf(f, "& ");
		fprintf(f, "%lf & ", mean_code_length);
		fprintf(f, "%lf\\\\ \n", mean_runtime);

		//std dev
		fprintf(f, "StdDev & %lf & ", stddev_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "%lf & ", stddev_validation_num_incorrect);
		else
			fprintf(f, "& ");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "%lf & ", stddev_test_num_incorrect);
		else
			fprintf(f, "& ");
		fprintf(f, "%lf & ", stddev_code_length);
		fprintf(f, "%lf\\\\ \n", stddev_runtime);
	}

	fprintf(f, "\\end{tabular}\n");

	fclose(f);
	return true;
}
//---------------------------------------------------------------------------
bool t_mep_all_runs_statistics::to_html_file(const char* filename, unsigned int problem_type) const
{
	FILE* f = NULL;
#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	f = _wfopen(w_filename, L"w");
	delete[] w_filename;

#else
	f = fopen(filename, "w");
#endif

	if (!f)
		return false;

	fprintf(f, "<!DOCTYPE html>\n");

	fprintf(f, "<head>\n");
	fprintf(f, "<style>\n");
	fprintf(f, "th{\n");
	fprintf(f, "border:1px solid black;\n");
		fprintf(f, "}\n");
	fprintf(f, "tr:nth-child(even) {\n");
	fprintf(f, "background-color: #D6EEEE;\n");
	fprintf(f, "}\n");
	fprintf(f, "</style>\n");
	fprintf(f, "</head>\n");

	fprintf(f, "<html>\n");
	fprintf(f, "<body>\n");

	fprintf(f, "<table>\n");

	if (problem_type == MEP_PROBLEM_REGRESSION || problem_type == MEP_PROBLEM_TIME_SERIE) {
		fprintf(f, "<tr>\n");
		fprintf(f, "<th>Run #</th><th>Training error</th><th>Validation error</th><th>Test error</th><th>Num. instructions</th><th>Running time</th>\n");
		fprintf(f, "</tr>\n");
		for (unsigned int r = 0; r < num_runs; r++) {
			fprintf(f, "<tr>\n");
			fprintf(f, "<td>%u</td><td>%lf</td>", r + 1, stats[r].best_training_error[stats[r].last_generation]);

			if (stats[r].best_validation_error > -1E-6)
				fprintf(f, "<td>%lf</td>", stats[r].best_validation_error);
			else
				fprintf(f, "<td></td>");
			if (stats[r].test_error > -1E-6)
				fprintf(f, "<td>%lf</td>", stats[r].test_error);
			else
				fprintf(f, "<td></td>");

			fprintf(f, "<td>%d</td>", stats[r].best_program.get_num_utilized_genes());
			fprintf(f, "<td>%lf</td>\n", stats[r].running_time);
			fprintf(f, "</tr>\n");
		}

		// best
		fprintf(f, "<tr>\n");
		fprintf(f, "<td>Best</td><td>%lf</td>", best_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "<td>%lf</td>", best_validation_error);
		else
			fprintf(f, "<td></td>");
		if (best_test_error > -1E-6)
			fprintf(f, "<td>%lf</td>", best_test_error);
		else
			fprintf(f, "<td></td>");
		fprintf(f, "<td>%d</td>", best_code_length);
		fprintf(f, "<td>%lf</td>\n", best_runtime);
		fprintf(f, "</tr>\n");

		// average
		fprintf(f, "<tr>\n");
		fprintf(f, "<td>Average</td><td>%lf</td>", mean_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "<td>%lf</td>", mean_validation_error);
		else
			fprintf(f, "<td></td>");
		if (best_test_error > -1E-6)
			fprintf(f, "<td>%lf</td>", mean_test_error);
		else
			fprintf(f, "<td></td>");
		fprintf(f, "<td>%lf</td>", mean_code_length);
		fprintf(f, "<td>%lf</td>\n", mean_runtime);
		fprintf(f, "</tr>\n");

		//std dev
		fprintf(f, "<tr>\n");
		fprintf(f, "<td>StdDev</td><td>%lf</td>", stddev_training_error);
		if (best_validation_error > -1E-6)
			fprintf(f, "<td>%lf</td>", stddev_validation_error);
		else
			fprintf(f, "<td></td>");
		if (best_test_error > -1E-6)
			fprintf(f, "<td>%lf</td>", stddev_test_error);
		else
			fprintf(f, "<td></td>");
		fprintf(f, "<td>%lf</td>", stddev_code_length);
		fprintf(f, "<td>%lf</td>\n", stddev_runtime);
		fprintf(f, "</tr>\n");
	}
	else {// MEP CLASSIFICATION
		fprintf(f, "<tr>\n");
		fprintf(f, "<th>Run #</th><th>Training num incorrect (%%)</th><th>Validation num incorrect (%%)</th><th>Test num incorrect (%%)</th><th>Num. instructions</th><th>Running time</th>\n");
		fprintf(f, "</tr>\n");

		for (unsigned int r = 0; r < num_runs; r++) {
			fprintf(f, "<tr>\n");
			fprintf(f, "<td>%u</td><td>%lf</td>", r + 1, stats[r].best_training_num_incorrect[stats[r].last_generation]);
			if (stats[r].best_validation_num_incorrect > -1E-6)
				fprintf(f, "<td>%lf</td>", stats[r].best_validation_num_incorrect);
			else
				fprintf(f, "<td></td>");

			if (stats[r].test_error > -1E-6)
				fprintf(f, "<td>%lf</td>", stats[r].test_num_incorrect);
			else
				fprintf(f, "<td></td>");
			fprintf(f, "<td>%d</td>", stats[r].best_program.get_num_utilized_genes());
			fprintf(f, "<td>%lf</td>\n", stats[r].running_time);
			fprintf(f, "</tr>\n");
		}

		// best
		fprintf(f, "<tr>\n");
		fprintf(f, "<td>Best</td><td>%lf</td>", best_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "<td>%lf</td>", best_validation_num_incorrect);
		else
			fprintf(f, "<td></td>");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "<td>%lf</td>", best_test_num_incorrect);
		else
			fprintf(f, "<td></td>");
		fprintf(f, "<td>%d</td>", best_code_length);
		fprintf(f, "<td>%lf</td>\n", best_runtime);
		fprintf(f, "</tr>\n");

		// average
		fprintf(f, "<tr>\n");
		fprintf(f, "<td>Average</td><td>%lf</td>", mean_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "<td>%lf</td>", mean_validation_num_incorrect);
		else
			fprintf(f, "<td></td>");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "<td>%lf</td>", mean_test_num_incorrect);
		else
			fprintf(f, "<td></td>");
		fprintf(f, "<td>%lf</td>", mean_code_length);
		fprintf(f, "<td>%lf</td>\n", mean_runtime);
		fprintf(f, "</tr>\n");

		//std dev
		fprintf(f, "<tr>\n");
		fprintf(f, "<td>StdDev</td><td>%lf</td>", stddev_training_num_incorrect);
		if (best_validation_num_incorrect > -1E-6)
			fprintf(f, "<td>%lf</td>", stddev_validation_num_incorrect);
		else
			fprintf(f, "<td></td>");
		if (best_test_num_incorrect > -1E-6)
			fprintf(f, "<td>%lf</td>", stddev_test_num_incorrect);
		else
			fprintf(f, "<td></td>");
		fprintf(f, "<td>%lf</td>", stddev_code_length);
		fprintf(f, "<td>%lf</td>\n", stddev_runtime);
		fprintf(f, "</tr>\n");
	}

	fprintf(f, "</table>\n");
	fprintf(f, "</body>\n");
	fprintf(f, "</html>\n");

	fclose(f);
	return true;
}
//---------------------------------------------------------------------------
int running_time_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_single_run_statistics*)a)->running_time < ((t_mep_single_run_statistics*)b)->running_time)
		return -1;
	else
		if (((t_mep_single_run_statistics*)a)->running_time > ((t_mep_single_run_statistics*)b)->running_time)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int running_time_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_single_run_statistics*)a)->running_time < ((t_mep_single_run_statistics*)b)->running_time)
		return 1;
	else
		if (((t_mep_single_run_statistics*)a)->running_time > ((t_mep_single_run_statistics*)b)->running_time)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::sort_stats_by_running_time(bool ascending)
{
	if (ascending)
		qsort((void*)stats, num_runs, sizeof(t_mep_single_run_statistics), running_time_comparator_ascending);
	else
		qsort((void*)stats, num_runs, sizeof(t_mep_single_run_statistics), running_time_comparator_descending);
}
//---------------------------------------------------------------------------
int training_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_single_run_statistics*)a)->best_training_error[((t_mep_single_run_statistics*)a)->last_generation] < ((t_mep_single_run_statistics*)b)->best_training_error[((t_mep_single_run_statistics*)b)->last_generation])
		return -1;
	else
		if (((t_mep_single_run_statistics*)a)->best_training_error[((t_mep_single_run_statistics*)a)->last_generation] > ((t_mep_single_run_statistics*)b)->best_training_error[((t_mep_single_run_statistics*)b)->last_generation])
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int training_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_single_run_statistics*)a)->best_training_error[((t_mep_single_run_statistics*)a)->last_generation] < ((t_mep_single_run_statistics*)b)->best_training_error[((t_mep_single_run_statistics*)b)->last_generation])
		return 1;
	else
		if (((t_mep_single_run_statistics*)a)->best_training_error[((t_mep_single_run_statistics*)a)->last_generation] > ((t_mep_single_run_statistics*)b)->best_training_error[((t_mep_single_run_statistics*)b)->last_generation])
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::sort_stats_by_training_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, num_runs, sizeof(t_mep_single_run_statistics), training_error_comparator_ascending);
	else
		qsort((void*)stats, num_runs, sizeof(t_mep_single_run_statistics), training_error_comparator_descending);

}
//---------------------------------------------------------------------------
int validation_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_single_run_statistics*)a)->best_validation_error < ((t_mep_single_run_statistics*)b)->best_validation_error)
		return -1;
	else
		if (((t_mep_single_run_statistics*)a)->best_validation_error > ((t_mep_single_run_statistics*)b)->best_validation_error)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int validation_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_single_run_statistics*)a)->best_validation_error < ((t_mep_single_run_statistics*)b)->best_validation_error)
		return 1;
	else
		if (((t_mep_single_run_statistics*)a)->best_validation_error > ((t_mep_single_run_statistics*)b)->best_validation_error)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::sort_stats_by_validation_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, num_runs, sizeof(t_mep_single_run_statistics), validation_error_comparator_ascending);
	else
		qsort((void*)stats, num_runs, sizeof(t_mep_single_run_statistics), validation_error_comparator_descending);

}
//---------------------------------------------------------------------------
int test_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_single_run_statistics*)a)->test_error < ((t_mep_single_run_statistics*)b)->test_error)
		return -1;
	else
		if (((t_mep_single_run_statistics*)a)->test_error > ((t_mep_single_run_statistics*)b)->test_error)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int test_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_single_run_statistics*)a)->test_error < ((t_mep_single_run_statistics*)b)->test_error)
		return 1;
	else
		if (((t_mep_single_run_statistics*)a)->test_error > ((t_mep_single_run_statistics*)b)->test_error)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::sort_stats_by_test_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, num_runs, sizeof(t_mep_single_run_statistics), test_error_comparator_ascending);
	else
		qsort((void*)stats, num_runs, sizeof(t_mep_single_run_statistics), test_error_comparator_descending);
}
//---------------------------------------------------------------------------
t_mep_single_run_statistics *t_mep_all_runs_statistics::get_stat_ptr(unsigned int index) const
{
	return &stats[index];
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::delete_memory(void)
{
	num_runs = 0;
	if (stats) {
		delete[] stats;
		stats = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::create(unsigned int _num_runs)
{
	delete_memory();
	if (_num_runs > 0)
		stats = new t_mep_single_run_statistics[_num_runs];
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::append(unsigned int num_generations)
{
	stats[num_runs].allocate_memory(num_generations);
	num_runs++;
}
//---------------------------------------------------------------------------
void t_mep_all_runs_statistics::to_xml_node(pugi::xml_node parent) const
{
	for (unsigned int r = 0; r < num_runs; r++) {
		pugi::xml_node run_node = parent.append_child("run");
		stats[r].to_xml_node(run_node);
	}
}
//---------------------------------------------------------------------------
bool t_mep_all_runs_statistics::from_xml_node(pugi::xml_node parent,
							   unsigned int problem_type,
							   unsigned int error_measure,
							   unsigned int num_classes,
							   unsigned int code_length)
{
	num_runs = 0;
	pugi::xml_node node_results = parent.child("results");
	if (node_results)
		for (pugi::xml_node row = node_results.child("run"); row; row = row.next_sibling("run"), num_runs++);

	if (num_runs) {
		stats = new t_mep_single_run_statistics[num_runs];
		num_runs = 0;
		for (pugi::xml_node row = node_results.child("run"); row; row = row.next_sibling("run"), num_runs++)
			stats[num_runs].from_xml_node(row, problem_type, error_measure, num_classes, code_length);
	}
	return true;
}
//---------------------------------------------------------------------------
t_mep_all_runs_statistics& t_mep_all_runs_statistics::operator=(const t_mep_all_runs_statistics& source)
{
	if (&source != this) {
		delete_memory();
		num_runs = source.num_runs;

		if (num_runs) {
			stats = new t_mep_single_run_statistics[num_runs];
			for (unsigned int i = 0; i < num_runs; i++)
				stats[i] = source.stats[i];

			stddev_training_error = source.stddev_training_error;
			stddev_validation_error = source.stddev_validation_error;
			stddev_test_error = source.stddev_test_error;
			stddev_runtime = source.stddev_runtime;
			mean_training_error = source.mean_training_error;
			mean_validation_error = source.mean_validation_error;
			mean_test_error = source.mean_test_error;
			mean_runtime = source.mean_runtime;
			best_training_error = source.best_training_error;
			best_validation_error = source.best_validation_error;
			best_test_error = source.best_test_error;
			best_runtime = source.best_runtime;

			stddev_training_num_incorrect = source.stddev_training_num_incorrect;
			stddev_validation_num_incorrect = source.stddev_validation_num_incorrect;
			stddev_test_num_incorrect = source.stddev_test_num_incorrect;
			mean_training_num_incorrect = source.mean_training_num_incorrect;
			mean_validation_num_incorrect = source.mean_validation_num_incorrect;
			mean_test_num_incorrect = source.mean_test_num_incorrect;
			best_training_num_incorrect = source.best_training_num_incorrect;
			best_validation_num_incorrect = source.best_validation_num_incorrect;
			best_test_num_incorrect = source.best_test_num_incorrect;
		}
	}

	return *this;
}
//---------------------------------------------------------------------------
