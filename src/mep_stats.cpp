// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mep_stats.h"
#include "mep_parameters.h"


#ifdef _WIN32
#include <windows.h>
#endif
//---------------------------------------------------------------------------
t_mep_run_statistics::t_mep_run_statistics(void)
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

	num_generations = 0;
}
//---------------------------------------------------------------------------
void t_mep_run_statistics::allocate_memory(long _num_generations)
{
	num_generations = _num_generations;
	best_training_error = new double[num_generations];
	average_training_error = new double[num_generations];

	best_training_num_incorrect = new double[num_generations];
	average_training_num_incorrect = new double[num_generations];
}
//---------------------------------------------------------------------------
t_mep_run_statistics::~t_mep_run_statistics()
{
	delete_memory();
}
//---------------------------------------------------------------------------
void t_mep_run_statistics::delete_memory(void)
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

}
//---------------------------------------------------------------------------
int t_mep_run_statistics::to_xml(pugi::xml_node parent)
{
	char tmp_str2[100];
	pugi::xml_node node = parent.append_child("running_time");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str2, "%lg", running_time);
	data.set_value(tmp_str2);

	char *tmp_str = new char[num_generations * 21 + 1];

	if (best_training_error) {
		pugi::xml_node best_training_error_node = parent.append_child("best_training_error");

		data = best_training_error_node.append_child(pugi::node_pcdata);
		tmp_str[0] = 0;
		for (int g = 0; g <= last_generation; g++) {
			char tmp_s[30];
			sprintf(tmp_s, "%lg", best_training_error[g]);
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
			sprintf(tmp_s, "%lg", average_training_error[g]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		//now the target if there is one...
		data.set_value(tmp_str);
	}

	node = parent.append_child("best_validation_error");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str2, "%lg", best_validation_error);
	data.set_value(tmp_str2);

	node = parent.append_child("test_error");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str2, "%lg", test_error);
	data.set_value(tmp_str2);


	if (best_training_num_incorrect) {
		pugi::xml_node best_training_num_incorrect_node = parent.append_child("best_training_num_incorrect");

		data = best_training_num_incorrect_node.append_child(pugi::node_pcdata);
		tmp_str[0] = 0;
		for (int g = 0; g <= last_generation; g++) {
			char tmp_s[30];
			sprintf(tmp_s, "%lf", best_training_num_incorrect[g]);
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
			sprintf(tmp_s, "%lf", average_training_num_incorrect[g]);
			strcat(tmp_str, tmp_s);
			strcat(tmp_str, " ");
		}
		//now the target if there is one...
		data.set_value(tmp_str);
	}

	node = parent.append_child("best_validation_num_incorrect");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str2, "%lg", best_validation_num_incorrect);
	data.set_value(tmp_str2);

	node = parent.append_child("test_num_incorrect");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str2, "%lg", test_num_incorrect);
	data.set_value(tmp_str2);

	node = parent.append_child("last_generation");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str2, "%d", last_generation);
	data.set_value(tmp_str2);

	node = parent.append_child("num_generations");
	data = node.append_child(pugi::node_pcdata);
	sprintf(tmp_str2, "%d", num_generations);
	data.set_value(tmp_str2);

	node = parent.append_child("program");
	best_program.to_xml(node);

	delete[] tmp_str;

	return true;
}
//---------------------------------------------------------------------------
int t_mep_run_statistics::from_xml(pugi::xml_node parent, int problem_type)
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
		num_generations = atoi(value_as_cstring);
	}
	else
		num_generations = 0;

	if (num_generations == 0)
		num_generations = last_generation + 1;

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
	if (node)
	{
		best_program.from_xml(node);

		if (problem_type != MEP_PROBLEM_MULTICLASS_CLASSIFICATION)
			best_program.simplify();
	}


	return true;
}
//---------------------------------------------------------------------------
t_mep_run_statistics& t_mep_run_statistics::operator=(t_mep_run_statistics& source)
{
	if (&source != this) {
		delete_memory();
		allocate_memory(source.num_generations);

		running_time = source.running_time;
		for (int i = 0; i < num_generations; i++) {
			best_training_error[i] = source.best_training_error[i];
			average_training_error[i] = source.average_training_error[i];
			best_training_num_incorrect[i] = source.best_training_num_incorrect[i];
			average_training_num_incorrect[i] = source.average_training_num_incorrect[i];
		}
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
t_mep_statistics::t_mep_statistics()
{
	stats = NULL;
}
//---------------------------------------------------------------------------
void t_mep_statistics::compute_mean_stddev(bool compute_on_validation, bool compute_on_test, int problem_type)
{
	stddev_training_error = stddev_validation_error = stddev_test_error = stddev_runtime = 0;
	mean_training_error = mean_validation_error = mean_test_error = mean_runtime = 0;
	best_training_error = best_validation_error = best_test_error = best_runtime = -1;

	if (!num_runs)
		return;

	best_training_error = stats[0].best_training_error[stats[0].last_generation];
	if (compute_on_validation)
		best_validation_error = stats[0].best_validation_error;
	if (compute_on_test)
		best_test_error = stats[0].test_error;
	best_runtime = stats[0].running_time;
	mean_training_error = stats[0].best_training_error[stats[0].last_generation];
	if (compute_on_validation)
		mean_validation_error = stats[0].best_validation_error;
	if (compute_on_test)
		mean_test_error = stats[0].test_error;
	mean_runtime = stats[0].running_time;

	for (int r = 1; r < num_runs; r++) {
		mean_training_error += stats[r].best_training_error[stats[r].last_generation];
		if (best_training_error > stats[r].best_training_error[stats[r].last_generation])
			best_training_error = stats[r].best_training_error[stats[r].last_generation];
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
	mean_validation_error /= num_runs;
	mean_test_error /= num_runs;
	mean_runtime /= num_runs;

	// compute stddev

	for (int r = 0; r < num_runs; r++) {
		stddev_training_error += (mean_training_error - stats[r].best_training_error[stats[r].last_generation]) * (mean_training_error - stats[r].best_training_error[stats[r].last_generation]);
		if (compute_on_validation)
			stddev_validation_error += (mean_validation_error - stats[r].best_validation_error) * (mean_validation_error - stats[r].best_validation_error);
		if (compute_on_test)
			stddev_test_error += (mean_test_error - stats[r].test_error) * (mean_test_error - stats[r].test_error);
		stddev_runtime += (mean_runtime - stats[r].running_time) * (mean_runtime - stats[r].running_time);
	}
	stddev_training_error = sqrt(stddev_training_error / num_runs);
	stddev_validation_error = sqrt(stddev_validation_error / num_runs);
	stddev_test_error = sqrt(stddev_test_error / num_runs);
	stddev_runtime = sqrt(stddev_runtime / num_runs);

	if (problem_type == MEP_PROBLEM_BINARY_CLASSIFICATION || 
		problem_type == MEP_PROBLEM_MULTICLASS_CLASSIFICATION) {

		stddev_training_num_incorrect = stddev_validation_num_incorrect = stddev_test_num_incorrect = stddev_runtime = 0;
		mean_training_num_incorrect = mean_validation_num_incorrect = mean_test_num_incorrect = mean_runtime = 0;
		best_training_num_incorrect = best_validation_num_incorrect = best_test_num_incorrect = best_runtime = -1;

		if (stats[0].best_training_num_incorrect) {
			best_training_num_incorrect = stats[0].best_training_num_incorrect[stats[0].last_generation];
			if (compute_on_validation)
				best_validation_num_incorrect = stats[0].best_validation_num_incorrect;
			if (compute_on_test)
				best_test_num_incorrect = stats[0].test_num_incorrect;
			best_runtime = stats[0].running_time;
			mean_training_num_incorrect = stats[0].best_training_num_incorrect[stats[0].last_generation];
			if (compute_on_validation)
				mean_validation_num_incorrect = stats[0].best_validation_num_incorrect;
			if (compute_on_test)
				mean_test_num_incorrect = stats[0].test_num_incorrect;
			mean_runtime = stats[0].running_time;

			for (int r = 1; r < num_runs; r++) {
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

				if (best_runtime > stats[r].running_time)
					best_runtime = stats[r].running_time;

				mean_runtime += stats[r].running_time;
			}
			mean_training_num_incorrect /= num_runs;
			mean_validation_num_incorrect /= num_runs;
			mean_test_num_incorrect /= num_runs;
			mean_runtime /= num_runs;

			// compute stddev

			for (int r = 0; r < num_runs; r++) {
				stddev_training_num_incorrect += (mean_training_num_incorrect - stats[r].best_training_num_incorrect[stats[r].last_generation]) * (mean_training_num_incorrect - stats[r].best_training_num_incorrect[stats[r].last_generation]);
				if (compute_on_validation)
					stddev_validation_num_incorrect += (mean_validation_num_incorrect - stats[r].best_validation_num_incorrect) * (mean_validation_num_incorrect - stats[r].best_validation_num_incorrect);
				if (compute_on_test)
					stddev_test_num_incorrect += (mean_test_num_incorrect - stats[r].test_num_incorrect) * (mean_test_num_incorrect - stats[r].test_num_incorrect);
				stddev_runtime += (mean_runtime - stats[r].running_time) * (mean_runtime - stats[r].running_time);
			}
			stddev_training_num_incorrect = sqrt(stddev_training_num_incorrect / num_runs);
			stddev_validation_num_incorrect = sqrt(stddev_validation_num_incorrect / num_runs);
			stddev_test_num_incorrect = sqrt(stddev_test_num_incorrect / num_runs);
			stddev_runtime = sqrt(stddev_runtime / num_runs);
		}
	}
}
//---------------------------------------------------------------------------
void t_mep_statistics::get_mean_error(double &training, double &validation, double &test, double &running_time)
{
	training = mean_training_error;
	validation = mean_validation_error;
	test = mean_test_error;
	running_time = mean_runtime;
}
//---------------------------------------------------------------------------
void t_mep_statistics::get_sttdev_error(double &training, double &validation, double &test, double &running_time)
{
	training = stddev_training_error;
	validation = stddev_validation_error;
	test = stddev_test_error;
	running_time = stddev_runtime;
}
//---------------------------------------------------------------------------
void t_mep_statistics::get_best_error(double &training, double &validation, double &test, double &running_time)
{
	training = best_training_error;
	validation = best_validation_error;
	test = best_test_error;
	running_time = best_runtime;
}
//---------------------------------------------------------------------------
void t_mep_statistics::get_mean_num_incorrect(double &training, double &validation, double &test, double &running_time)
{
	training = mean_training_num_incorrect;
	validation = mean_validation_num_incorrect;
	test = mean_test_num_incorrect;
	running_time = mean_runtime;
}
//---------------------------------------------------------------------------
void t_mep_statistics::get_sttdev_num_incorrect(double &training, double &validation, double &test, double &running_time)
{
	training = stddev_training_num_incorrect;
	validation = stddev_validation_num_incorrect;
	test = stddev_test_num_incorrect;
	running_time = stddev_runtime;
}
//---------------------------------------------------------------------------
void t_mep_statistics::get_best_num_incorrect(double &training, double &validation, double &test, double &running_time)
{
	training = best_training_num_incorrect;
	validation = best_validation_num_incorrect;
	test = best_test_num_incorrect;
	running_time = best_runtime;
}
//---------------------------------------------------------------------------
// returns the best training num incorreclty classified
double t_mep_statistics::get_best_training_num_incorrectly_classified(int run, int generation)
{
	if (stats[run].best_training_num_incorrect)
		if (generation <= stats[run].last_generation)
			return stats[run].best_training_num_incorrect[generation];
		else
			return stats[run].best_training_num_incorrect[stats[run].last_generation];
	else
		return -1;
}
//---------------------------------------------------------------------------
// returns the best validation num incorreclty classified
double t_mep_statistics::get_best_validation_num_incorrectly_classified(int run)
{
	return stats[run].best_validation_num_incorrect;
}
//---------------------------------------------------------------------------
// returns the average (over the entire population) training num incorreclty classified
double t_mep_statistics::get_mean_training_num_incorrectly_classified(int run, int generation)
{
	return stats[run].average_training_num_incorrect[generation];
}
//---------------------------------------------------------------------------
double t_mep_statistics::get_stddev_training_num_incorrectly_classified(int /*run*/, int /*generation*/)
{
	return 0;// stats[run].stddev_training_num_incorrect[generation];
}
//---------------------------------------------------------------------------
double t_mep_statistics::get_test_num_incorrectly_classified(int run)
{
	return stats[run].test_num_incorrect;
}
//---------------------------------------------------------------------------
double t_mep_statistics::get_best_training_error(int run, int gen)
{
	if (stats[run].best_training_error)
		if (gen <= stats[run].last_generation)
			return stats[run].best_training_error[gen];
		else
			return stats[run].best_training_error[stats[run].last_generation];
	else
		return -1;
}
//---------------------------------------------------------------------------
double t_mep_statistics::get_best_validation_error(int run)
{
	return stats[run].best_validation_error;
}
//---------------------------------------------------------------------------
double t_mep_statistics::get_test_error(int run)
{
	return stats[run].test_error;
}
//---------------------------------------------------------------------------
double t_mep_statistics::get_average_training_error(int run, int gen)
{
	if (stats[run].average_training_error)
		if (gen <= stats[run].last_generation)
			return stats[run].average_training_error[gen];
		else
			return stats[run].average_training_error[stats[run].last_generation];
	else
		return -1;
}
//---------------------------------------------------------------------------
int t_mep_statistics::get_latest_generation(int run)
{
	return stats[run].last_generation;
}
//---------------------------------------------------------------------------
double t_mep_statistics::get_running_time(int run)
{
	return stats[run].running_time;
}
//---------------------------------------------------------------------------
int t_mep_statistics::to_csv(const char *filename, int problem_type)
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
	if (problem_type == MEP_PROBLEM_REGRESSION) {
		fprintf(f, "Run #;training error; validation error; test error; running time (s)\n");
		for (int r = 0; r < num_runs; r++) {
			fprintf(f, "%d;%lf;", r + 1, stats[r].best_training_error[stats[r].last_generation]);
			if (stats[r].best_validation_error > -1E-6)
				fprintf(f, "%lf;", stats[r].best_validation_error);
			else
				fprintf(f, ";");
			if (stats[r].test_error > -1E-6)
				fprintf(f, "%lf;", stats[r].test_error);
			else
				fprintf(f, ";");
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
		fprintf(f, "%lf\n", stddev_runtime);
	}
	else {// MEP CLASSIFICATION
		fprintf(f, "#;training num incorrect (%%); validation num incorrect (%%); test num incorrect (%%); running time (s)\n");
		for (int r = 0; r < num_runs; r++) {
			fprintf(f, "%d;%lf;", r + 1, stats[r].best_training_num_incorrect[stats[r].last_generation]);
			if (stats[r].best_validation_num_incorrect > -1E-6)
				fprintf(f, "%lf;", stats[r].best_validation_num_incorrect);
			else
				fprintf(f, ";");
			if (stats[r].test_num_incorrect > -1E-6)
				fprintf(f, "%lf;", stats[r].test_num_incorrect);
			else
				fprintf(f, ";");
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
		fprintf(f, "%lf\n", stddev_runtime);
	}

	fclose(f);
	return true;
}
//---------------------------------------------------------------------------
int t_mep_statistics::to_tex(const char *filename, int problem_type)
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

	fprintf(f, "\\begin{tabular}{c c c c c}\n");
	if (problem_type == MEP_PROBLEM_REGRESSION) {
		fprintf(f, "Run \\# & training error & validation error & test error & running time(s)\\\\ \n");
		for (int r = 0; r < num_runs; r++) {
			fprintf(f, "%d & %lf & ", r + 1, stats[r].best_training_error[stats[r].last_generation]);

			if (stats[r].best_validation_error > -1E-6)
				fprintf(f, "%lf & ", stats[r].best_validation_error);
			else
				fprintf(f, "& ");
			if (stats[r].test_error > -1E-6)
				fprintf(f, "%lf & ", stats[r].test_error);
			else
				fprintf(f, "& ");

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
		fprintf(f, "%lf\\\\ \n", stddev_runtime);
	}
	else {// MEP CLASSIFICATION
		fprintf(f, "Run \\# & training num incorrect (\\%%) & validation num incorrect (\\%%) & test num incorrect (\\%%) & running time(s)\\\\ \n");
		for (int r = 0; r < num_runs; r++) {
			fprintf(f, "%d & %lf &", r + 1, stats[r].best_training_num_incorrect[stats[r].last_generation]);
			if (stats[r].best_validation_num_incorrect > -1E-6)
				fprintf(f, "%lf & ", stats[r].best_validation_num_incorrect);
			else
				fprintf(f, "& ");

			if (stats[r].test_error > -1E-6)
				fprintf(f, "%lf &", stats[r].test_num_incorrect);
			else
				fprintf(f, "& ");
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
		fprintf(f, "%lf\\\\ \n", stddev_runtime);
	}

	fprintf(f, "\\end{tabular}\n");

	fclose(f);
	return true;
}
//---------------------------------------------------------------------------
int running_time_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_run_statistics*)a)->running_time < ((t_mep_run_statistics*)b)->running_time)
		return -1;
	else
		if (((t_mep_run_statistics*)a)->running_time > ((t_mep_run_statistics*)b)->running_time)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int running_time_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_run_statistics*)a)->running_time < ((t_mep_run_statistics*)b)->running_time)
		return 1;
	else
		if (((t_mep_run_statistics*)a)->running_time > ((t_mep_run_statistics*)b)->running_time)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep_statistics::sort_stats_by_running_time(bool ascending)
{
	if (ascending)
		qsort((void*)stats, num_runs, sizeof(t_mep_run_statistics), running_time_comparator_ascending);
	else
		qsort((void*)stats, num_runs, sizeof(t_mep_run_statistics), running_time_comparator_descending);
}
//---------------------------------------------------------------------------
int training_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_run_statistics*)a)->best_training_error[((t_mep_run_statistics*)a)->last_generation] < ((t_mep_run_statistics*)b)->best_training_error[((t_mep_run_statistics*)b)->last_generation])
		return -1;
	else
		if (((t_mep_run_statistics*)a)->best_training_error[((t_mep_run_statistics*)a)->last_generation] > ((t_mep_run_statistics*)b)->best_training_error[((t_mep_run_statistics*)b)->last_generation])
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int training_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_run_statistics*)a)->best_training_error[((t_mep_run_statistics*)a)->last_generation] < ((t_mep_run_statistics*)b)->best_training_error[((t_mep_run_statistics*)b)->last_generation])
		return 1;
	else
		if (((t_mep_run_statistics*)a)->best_training_error[((t_mep_run_statistics*)a)->last_generation] > ((t_mep_run_statistics*)b)->best_training_error[((t_mep_run_statistics*)b)->last_generation])
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep_statistics::sort_stats_by_training_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, num_runs, sizeof(t_mep_run_statistics), training_error_comparator_ascending);
	else
		qsort((void*)stats, num_runs, sizeof(t_mep_run_statistics), training_error_comparator_descending);

}
//---------------------------------------------------------------------------
int validation_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_run_statistics*)a)->best_validation_error < ((t_mep_run_statistics*)b)->best_validation_error)
		return -1;
	else
		if (((t_mep_run_statistics*)a)->best_validation_error > ((t_mep_run_statistics*)b)->best_validation_error)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int validation_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_run_statistics*)a)->best_validation_error < ((t_mep_run_statistics*)b)->best_validation_error)
		return 1;
	else
		if (((t_mep_run_statistics*)a)->best_validation_error > ((t_mep_run_statistics*)b)->best_validation_error)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep_statistics::sort_stats_by_validation_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, num_runs, sizeof(t_mep_run_statistics), validation_error_comparator_ascending);
	else
		qsort((void*)stats, num_runs, sizeof(t_mep_run_statistics), validation_error_comparator_descending);

}
//---------------------------------------------------------------------------
int test_error_comparator_ascending(const void * a, const void * b)
{
	if (((t_mep_run_statistics*)a)->test_error < ((t_mep_run_statistics*)b)->test_error)
		return -1;
	else
		if (((t_mep_run_statistics*)a)->test_error > ((t_mep_run_statistics*)b)->test_error)
			return 1;
		else
			return 0;
}
//---------------------------------------------------------------------------
int test_error_comparator_descending(const void * a, const void * b)
{
	if (((t_mep_run_statistics*)a)->test_error < ((t_mep_run_statistics*)b)->test_error)
		return 1;
	else
		if (((t_mep_run_statistics*)a)->test_error > ((t_mep_run_statistics*)b)->test_error)
			return -1;
		else
			return 0;
}
//---------------------------------------------------------------------------
void t_mep_statistics::sort_stats_by_test_error(bool ascending)
{
	if (ascending)
		qsort((void*)stats, num_runs, sizeof(t_mep_run_statistics), test_error_comparator_ascending);
	else
		qsort((void*)stats, num_runs, sizeof(t_mep_run_statistics), test_error_comparator_descending);
}
//---------------------------------------------------------------------------
t_mep_run_statistics *t_mep_statistics::get_stat_ptr(int index)
{
	return &stats[index];
}
//---------------------------------------------------------------------------
void t_mep_statistics::delete_memory(void)
{
	num_runs = 0;
	if (stats) {
		delete[] stats;
		stats = NULL;
	}
}
//---------------------------------------------------------------------------
void t_mep_statistics::create(int _num_runs)
{
	delete_memory();
	if (_num_runs > 0)
		stats = new t_mep_run_statistics[_num_runs];
}
//---------------------------------------------------------------------------
void t_mep_statistics::append(int num_generations)
{
	stats[num_runs].allocate_memory(num_generations);
	num_runs++;
}
//---------------------------------------------------------------------------
int t_mep_statistics::to_xml(pugi::xml_node parent)
{
	for (int r = 0; r < num_runs; r++) {
		pugi::xml_node run_node = parent.append_child("run");
		stats[r].to_xml(run_node);
	}
	return true;
}
//---------------------------------------------------------------------------
int t_mep_statistics::from_xml(pugi::xml_node parent, int problem_type)
{
	num_runs = 0;
	pugi::xml_node node_results = parent.child("results");
	if (node_results)
		for (pugi::xml_node row = node_results.child("run"); row; row = row.next_sibling("run"), num_runs++);

	if (num_runs) {
		stats = new t_mep_run_statistics[num_runs];
		num_runs = 0;
		for (pugi::xml_node row = node_results.child("run"); row; row = row.next_sibling("run"), num_runs++)
			stats[num_runs].from_xml(row, problem_type);
	}
	return true;
}
//---------------------------------------------------------------------------
t_mep_statistics& t_mep_statistics::operator=(t_mep_statistics& source)
{
	if (&source != this) {
		delete_memory();
		num_runs = source.num_runs;

		if (num_runs) {
			stats = new t_mep_run_statistics[num_runs];
			for (int i = 0; i < num_runs; i++)
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
