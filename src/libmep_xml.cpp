// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <locale.h>
//-----------------------------------------------------------------
#ifdef _WIN32
	#include <windows.h>
#endif
//-----------------------------------------------------------------
#include "libmep.h"
//-----------------------------------------------------------------
void t_mep::to_xml_node(pugi::xml_node parent)
{
	// utilized variables

	pugi::xml_node training_node = parent.append_child("training");
	training_data.to_xml_node(training_node);
	pugi::xml_node validation_node = parent.append_child("validation");
	validation_data.to_xml_node(validation_node);
	pugi::xml_node testing_node = parent.append_child("test");
	test_data.to_xml_node(testing_node);

	if (mep_parameters.get_problem_type() != MEP_PROBLEM_TIME_SERIE) {
		if (variables_enabled && training_data.get_num_cols()) {

			char* tmp_str = new char[training_data.get_num_cols() * 2 + 10];

			pugi::xml_node utilized_variables_node = parent.append_child("variables_utilization");
			unsigned int num_outputs = mep_parameters.get_num_outputs();
			
			tmp_str[0] = 0;
			for (unsigned int v = 0; v < training_data.get_num_cols() - num_outputs; v++) {
				char tmp_s[30];
				snprintf(tmp_s, 30, "%d", variables_enabled[v]);
				strcat(tmp_str, tmp_s);
				strcat(tmp_str, " ");
			}
			pugi::xml_node utilized_variables_data_node = utilized_variables_node.append_child(pugi::node_pcdata);
			utilized_variables_data_node.set_value(tmp_str);

			delete[] tmp_str;
		}
	}

	pugi::xml_node parameters_node = parent.append_child("parameters");
	mep_parameters.to_xml_node(parameters_node);

	pugi::xml_node constants_node = parent.append_child("constants");
	mep_constants.to_xml_node(constants_node);

	pugi::xml_node operators_node = parent.append_child("operators");
	mep_operators.to_xml_node(operators_node);

	pugi::xml_node results_node = parent.append_child("results");

	statistics.to_xml_node(results_node);

	modified_project = false;
}
//---------------------------------------------------------------------------
void t_mep::to_xml_node_current_generation(pugi::xml_node parent)
{
	unsigned int num_subpops = mep_parameters.get_num_subpopulations();
	
	for (unsigned int d = 0; d < num_subpops; d++) {
		char tmp_s[25];
		snprintf(tmp_s, 25, "sub_pop %u", d);
		pugi::xml_node subpop_node = parent.append_child(tmp_s);
		
		for (unsigned int w = 0; w < mep_parameters.get_subpopulation_size(); w++){ // din d in d+1
			snprintf(tmp_s, 25, "individual %u", w);
			pugi::xml_node individual_node = subpop_node.append_child(tmp_s);
			population[d].individuals[w].to_xml_node(individual_node);
		}
	}
}
//---------------------------------------------------------------------------
bool t_mep::from_xml_node(pugi::xml_node parent)
{
	//get the utilized variables first

	if (variables_enabled) {
		delete[] variables_enabled;
		variables_enabled = NULL;
	}

	pugi::xml_node node = parent.child("parameters");
	if (node) {
		mep_parameters.from_xml_node(node);
		if (mep_parameters.get_random_subset_selection_size_percent() == 0) {
			mep_parameters.set_random_subset_selection_size_percent(100);
		}
	}
	else
		mep_parameters.init();

	training_data.clear_data();
	node = parent.child("training");
	if (node) {
		training_data.from_xml_node(node);
		if (training_data.get_num_cols() > 0)
			num_total_variables = training_data.get_num_cols() - mep_parameters.get_num_outputs();
		else
			num_total_variables = 0;
	}
	else
		num_total_variables = 0;

	// for other problems except time series,
	// if the number of cols for test is smaller than number of cols for training
	// it means that the test does not have output
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_TIME_SERIE) {
		//training_data_ts.to_time_serie_from_single_col(training_data, mep_parameters.get_window_size());
		//num_total_variables = mep_parameters.get_window_size();
		num_total_variables = 0;
	}


	if (mep_parameters.get_problem_type() != MEP_PROBLEM_TIME_SERIE) {
		if (training_data.get_num_rows()) {
			//actual_enabled_variables = new int[num_total_variables];
			variables_enabled = new bool[num_total_variables];

			node = parent.child("variables_utilization");
			if (node) {
				const char* value_as_cstring = node.child_value();
				size_t num_jumped_chars = 0;
				num_actual_variables = 0;
				int i = 0;

				while (*(value_as_cstring + num_jumped_chars)) {
					int int_read;
					if (sscanf(value_as_cstring + num_jumped_chars, "%d", &int_read)) {
						variables_enabled[i] = (bool)int_read;

						if (variables_enabled[i]) {
							//	actual_enabled_variables[num_actual_variables] = i;
							num_actual_variables++;
						}
					}
					size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
					if (local_jump == strlen(value_as_cstring + num_jumped_chars))
						local_jump = 0;
					num_jumped_chars += local_jump + 1;
					i++;
				}
			}
			else {// not found, use everything
				num_actual_variables = num_total_variables;
				for (unsigned int i = 0; i < num_total_variables; i++) {
					variables_enabled[i] = 1;
					//actual_enabled_variables[i] = i;
				}
			}
		}
	}
	/*
	else {// time series
		variables_enabled = new bool[num_total_variables];
		num_actual_variables = num_total_variables;
		for (unsigned int i = 0; i < num_total_variables; i++) {
			variables_enabled[i] = 1;
			//actual_enabled_variables[i] = i;
		}
	}
	*/

	validation_data.clear_data();
	node = parent.child("validation");
	if (node)
		validation_data.from_xml_node(node);

	test_data.clear_data();
	node = parent.child("test");
	if (node) {
		test_data.from_xml_node(node);
	}

	node = parent.child("operators");
	if (node)
		mep_operators.from_xml_node(node);
	else
		mep_operators.init();

	node = parent.child("constants");
	if (node)
		mep_constants.from_xml_node(node);
	else
		mep_constants.init();
	
	if (mep_parameters.get_problem_type() == MEP_PROBLEM_MULTICLASS_CLASSIFICATION ||
		mep_parameters.get_problem_type() == MEP_PROBLEM_BINARY_CLASSIFICATION){
		training_data.compute_class_labels(mep_parameters.get_num_outputs());
		validation_data.assign_class_index_from_training_class_labels(training_data.get_class_labels_ptr(), training_data.get_num_classes());
		test_data.assign_class_index_from_training_class_labels(training_data.get_class_labels_ptr(), training_data.get_num_classes());
	}

	statistics.from_xml_node(parent,
						mep_parameters.get_problem_type(),
						mep_parameters.get_error_measure(),
						training_data.get_num_classes(),
						mep_parameters.get_code_length());

	last_run_index = (int)statistics.num_runs - 1;

	statistics.compute_mean_stddev(
			mep_parameters.get_use_validation_data() && validation_data.get_num_rows() > 0,
			test_data.get_num_rows() > 0);

	modified_project = false;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep::to_xml_file(const char* filename)
{
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	char* saved_locale = strdup (old_locale);
	setlocale(LC_NUMERIC, "C");

	pugi::xml_document doc;
	// add node with some name
	pugi::xml_node body = doc.append_child("project");

	pugi::xml_node version_node = body.append_child("version");
	pugi::xml_node data = version_node.append_child(pugi::node_pcdata);
	data.set_value(version);

	pugi::xml_node problem_description_node = body.append_child("problem_description");
	data = problem_description_node.append_child(pugi::node_pcdata);
	data.set_value(problem_description);

	pugi::xml_node alg_node = body.append_child("algorithm");
	to_xml_node(alg_node);

	modified_project = false;

	setlocale(LC_NUMERIC, saved_locale);
	free (saved_locale);

#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	int result = doc.save_file(w_filename);
	delete[] w_filename;
	return result;
#else
	return doc.save_file(filename);
#endif

}
//-----------------------------------------------------------------
bool t_mep::to_xml_file_current_generation(const char* filename)
{
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	char* saved_locale = strdup (old_locale);
	setlocale(LC_NUMERIC, "C");
	
	pugi::xml_document doc;
	// add node with some name
	pugi::xml_node body = doc.append_child("population");
	
	to_xml_node_current_generation(body);
	
	setlocale(LC_NUMERIC, saved_locale);
	free (saved_locale);

#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	int result = doc.save_file(w_filename);
	delete[] w_filename;
	return result;
#else
	return doc.save_file(filename);
#endif
}
//-----------------------------------------------------------------
bool t_mep::from_xml_file(const char* filename)
{
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	char* saved_locale = strdup (old_locale);
	setlocale(LC_NUMERIC, "C");

	pugi::xml_document doc;

	pugi::xml_parse_result result;

#ifdef _WIN32
	int count_chars = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
	wchar_t* w_filename = new wchar_t[count_chars];
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, w_filename, count_chars);

	result = doc.load_file(w_filename);
	delete[] w_filename;
#else
	result = doc.load_file(filename);
#endif

	if (result.status != pugi::status_ok) {
		setlocale(LC_NUMERIC, saved_locale);
		free (saved_locale);
		return false;
	}

	pugi::xml_node body_node = doc.child("project");

	if (!body_node) {
		setlocale(LC_NUMERIC, saved_locale);
		free (saved_locale);

		return false;
	}

	if (problem_description) {
		delete[] problem_description;
		problem_description = NULL;
	}
	pugi::xml_node node = body_node.child("problem_description");
	if (node) {
		const char* value_as_cstring = node.child_value();

		if (strlen(value_as_cstring)) {
			problem_description = new char[strlen(value_as_cstring) + 1];
			strcpy(problem_description, value_as_cstring);
		}
	}
	else {
		problem_description = new char[100];
		strcpy(problem_description, "Problem description here ...");
	}

	pugi::xml_node alg_node = body_node.child("algorithm");

	if (!alg_node) {
		setlocale(LC_NUMERIC, saved_locale);
		free (saved_locale);

		return false;
	}

	from_xml_node(alg_node);

	setlocale(LC_NUMERIC, saved_locale);
	free (saved_locale);

	return true;
}
//-----------------------------------------------------------------
