// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
//---------------------------------------------------------------------------
void t_mep_chromosome::to_xml_node(pugi::xml_node parent) const
{
	char tmp_str[100];

	pugi::xml_node node = parent.append_child("code_length");
	pugi::xml_node data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", code_length);
	data.set_value(tmp_str);

	node = parent.append_child("num_variables");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", num_total_variables);
	data.set_value(tmp_str);

	node = parent.append_child("num_outputs");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", num_program_outputs);
	data.set_value(tmp_str);
	
	node = parent.append_child("problem_type");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", problem_type);
	data.set_value(tmp_str);
	
	node = parent.append_child("data_type");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%d", data_type);
	data.set_value(tmp_str);
	
	node = parent.append_child("error_measure");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", error_measure);
	data.set_value(tmp_str);

	pugi::xml_node node_code = parent.append_child("code");

	for (unsigned int i = 0; i < code_length; i++) {
		node = node_code.append_child("i");
		data = node.append_child(pugi::node_pcdata);
		if (prg[i].op < 0) // operator
			snprintf(tmp_str, 100, "%d %d %d %d %d", prg[i].op, prg[i].addr[0], prg[i].addr[1], prg[i].addr[2], prg[i].addr[3]);
		else
			snprintf(tmp_str, 100, "%d", prg[i].op);
		data.set_value(tmp_str);
	}
	node = parent.append_child("best_genes");
	data = node.append_child(pugi::node_pcdata);

	if (num_program_outputs) {
		char tmp_s[100];
		char *tmp_genes_str = new char[num_program_outputs * 12 + 1]; // 12 digits for each output !!!
		tmp_genes_str[0] = 0;
		for (unsigned int c = 0; c < num_program_outputs; c++) {
			snprintf(tmp_s, 100, "%u", index_best_genes[c]);
			strcat(tmp_genes_str, tmp_s);
			strcat(tmp_genes_str, " ");
		}

		data.set_value(tmp_genes_str);
		delete[] tmp_genes_str;
	}

	node = parent.append_child("max_index_best_genes");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%d", max_index_best_genes);
	data.set_value(tmp_str);

	node = parent.append_child("error");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%lg", fitness);
	data.set_value(tmp_str);

	node = parent.append_child("num_incorrectly_classified");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%lf", num_incorrectly_classified);
	data.set_value(tmp_str);

	node = parent.append_child("binary_classification_threshold");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%lg", best_class_threshold);
	data.set_value(tmp_str);

	node = parent.append_child("num_constants");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", num_constants);
	data.set_value(tmp_str);

	if (num_constants) {
		node = parent.append_child("constants");
		data = node.append_child(pugi::node_pcdata);

		char *tmp_cst_str = NULL;

		if (real_constants) {
			char tmp_s[100];
			tmp_cst_str = new char[num_constants * 30]; // 30 digits for each constant !!!
			tmp_cst_str[0] = 0;
			for (unsigned int c = 0; c < num_constants; c++) {
				snprintf(tmp_s, 100, "%lg", real_constants[c]);
				strcat(tmp_cst_str, tmp_s);
				strcat(tmp_cst_str, " ");
			}
		}
		else{
			if (long_constants) {
				char tmp_s[100];
				tmp_cst_str = new char[num_constants * 30]; // 30 digits for each constant !!!
				tmp_cst_str[0] = 0;
				for (unsigned int c = 0; c < num_constants; c++) {
					snprintf(tmp_s, 100, "%lld", long_constants[c]);
					strcat(tmp_cst_str, tmp_s);
					strcat(tmp_cst_str, " ");
				}
			}
		}
		data.set_value(tmp_cst_str);
		if (tmp_cst_str)
			delete[] tmp_cst_str;
	}

	node = parent.append_child("num_classes");
	data = node.append_child(pugi::node_pcdata);
	snprintf(tmp_str, 100, "%u", num_classes);
	data.set_value(tmp_str);

	if (centers) {
		node = parent.append_child("class_centers");
		data = node.append_child(pugi::node_pcdata);

		char* tmp_cst_str = NULL;

		char tmp_s[100];
		tmp_cst_str = new char[num_classes * 30]; // 30 digits for each constant !!!
		tmp_cst_str[0] = 0;
		for (unsigned int c = 0; c < num_classes; c++) {
			snprintf(tmp_s, 100, "%lg", centers[c]);
			strcat(tmp_cst_str, tmp_s);
			strcat(tmp_cst_str, " ");
		}
		data.set_value(tmp_cst_str);
		if (tmp_cst_str)
			delete[] tmp_cst_str;
	}
	
	if (class_labels) {
		node = parent.append_child("class_labels");
		data = node.append_child(pugi::node_pcdata);

		char* tmp_cst_str = NULL;

		char tmp_s[100];
		tmp_cst_str = new char[num_classes * 30]; // 30 digits for each label !!!
		tmp_cst_str[0] = 0;
		for (unsigned int c = 0; c < num_classes; c++) {
			snprintf(tmp_s, 100, "%d", class_labels[c]);
			strcat(tmp_cst_str, tmp_s);
			strcat(tmp_cst_str, " ");
		}
		data.set_value(tmp_cst_str);
		if (tmp_cst_str)
			delete[] tmp_cst_str;
	}
}
//---------------------------------------------------------------------------
int t_mep_chromosome::from_xml_node(pugi::xml_node parent,
							   unsigned int _problem_type,
							   unsigned int _error_measure,
							   unsigned int _training_data_num_classes,
									unsigned int _code_length)
{
	clear();

	pugi::xml_node node = parent.child("code_length");
	if (node) {
		const char *value_as_cstring = node.child_value();
		code_length = (unsigned int)atoi(value_as_cstring);
	}
	else{
		code_length = 0;
	}
	
	node = parent.child("num_variables");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_total_variables = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_total_variables = 1;

	pugi::xml_node node_code = parent.child("code");
	if (node_code) { // there is some code inside
		if (!code_length)
			code_length = _code_length;
		if (code_length){
			prg = new t_code3[code_length];
				int i = 0;
				for (pugi::xml_node row = node_code.child("i"); row; row = row.next_sibling("i"), i++) {
					const char *value_as_cstring = row.child_value();
					sscanf(value_as_cstring, "%d", &prg[i].op);
					if (prg[i].op < 0) { // operator
						int num_read = sscanf(value_as_cstring, "%d%u%u%u%u", &prg[i].op, &prg[i].addr[0], &prg[i].addr[1], &prg[i].addr[2], &prg[i].addr[3]);
						if (num_read < 4) {
							prg[i].addr[2] = 0;
							prg[i].addr[3] = 0;
						}

					}

				}
				if (!i) {
					for (pugi::xml_node row = node_code.child("instruction"); row; row = row.next_sibling("instruction"), i++) {
						const char *value_as_cstring = row.child_value();
						sscanf(value_as_cstring, "%d", &prg[i].op);
						if (prg[i].op < 0) { // operator
							int num_read = sscanf(value_as_cstring, "%d%u%u%u%u", &prg[i].op, &prg[i].addr[0], &prg[i].addr[1], &prg[i].addr[2], &prg[i].addr[3]);
							if (num_read < 4) {
								prg[i].addr[2] = 0;
								prg[i].addr[3] = 0;
							}

						}

					}

			}
		}
	}

	node = parent.child("num_outputs");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_program_outputs = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_program_outputs = 1;
	
	if (num_program_outputs)
		index_best_genes = new unsigned int[num_program_outputs];

	node = parent.child("best_genes");
	if (node) {
		const char *value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		for (unsigned int c = 0; c < num_program_outputs; c++) {
			sscanf(value_as_cstring + num_jumped_chars, "%u", &index_best_genes[c]);
			size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}
	else {
// old versions
		node = parent.child("best");
		if (node) {
			const char *value_as_cstring = node.child_value();
			index_best_genes[0] = (unsigned int)atoi(value_as_cstring);
			max_index_best_genes = index_best_genes[0];
		}
	}

	node = parent.child("max_index_best_genes");
	if (node) {
		const char *value_as_cstring = node.child_value();
		max_index_best_genes = (unsigned int)atoi(value_as_cstring);
	}
	else
		max_index_best_genes = 0;

	node = parent.child("error");
	if (node) {
		const char *value_as_cstring = node.child_value();
		fitness = atof(value_as_cstring);
	}
	else
		fitness = 0;

	node = parent.child("num_incorrectly_classified");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_incorrectly_classified = atof(value_as_cstring);
	}
	else
		num_incorrectly_classified = 0;

	node = parent.child("binary_classification_threshold");
	if (node) {
		const char *value_as_cstring = node.child_value();
		best_class_threshold = atof(value_as_cstring);
	}
	else
		best_class_threshold = 0;

	node = parent.child("data_type");
	if (node) {
		const char* value_as_cstring = node.child_value();
		data_type = (char)atoi(value_as_cstring);
	}
	else{// node was not found, so we set it to Double
		data_type = MEP_DATA_DOUBLE;
	}
	
	node = parent.child("num_constants");
	if (node) {
		const char *value_as_cstring = node.child_value();
		num_constants = (unsigned int)atoi(value_as_cstring);
	}
	else
		num_constants = 0;
	
	node = parent.child("constants");
	if (node && num_constants) {
		const char *value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		if (data_type == MEP_DATA_DOUBLE){
			real_constants = new double[num_constants];
			long_constants = NULL;
			for (unsigned int c = 0; c < num_constants; c++) {
				sscanf(value_as_cstring + num_jumped_chars, "%lf", &real_constants[c]);
				size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
				num_jumped_chars += local_jump + 1;
			}
		}
		else
			if (data_type == MEP_DATA_LONG_LONG){
				long_constants = new long long[num_constants];
				real_constants = NULL;
				for (unsigned int c = 0; c < num_constants; c++) {
					sscanf(value_as_cstring + num_jumped_chars, "%lld", &long_constants[c]);
					size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
					num_jumped_chars += local_jump + 1;
				}
			}
	}
	else{
		real_constants = NULL;
		long_constants = NULL;
	}

	node = parent.child("num_classes");
	if (node) {
		const char* value_as_cstring = node.child_value();
		num_classes = (unsigned int)atoi(value_as_cstring);
	}
	else{
		if (_training_data_num_classes)
			num_classes = _training_data_num_classes;
		else
			num_classes = 0;
	}
	
	node = parent.child("problem_type");
	if (node) {
		const char* value_as_cstring = node.child_value();
		problem_type = (unsigned int)atoi(value_as_cstring);
	}
	else{// node was not found, so we take it from parameters
		if (_problem_type)
			problem_type = _problem_type;
		else
			problem_type = MEP_PROBLEM_REGRESSION;
	}
	
	node = parent.child("error_measure");
	if (node) {
		const char* value_as_cstring = node.child_value();
		error_measure = (unsigned int)atoi(value_as_cstring);
	}
	else{
		if (_error_measure)
			error_measure = _error_measure;
		else
			error_measure = MEP_REGRESSION_MEAN_ABSOLUTE_ERROR;
	}

	node = parent.child("class_centers");
	if (node) {
		const char* value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		centers = new double[num_classes];
		for (unsigned int c = 0; c < num_classes; c++) {
			sscanf(value_as_cstring + num_jumped_chars, "%lf", &centers[c]);
			size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}
	else{
		centers = NULL;
	}
	
	node = parent.child("class_labels");
	if (node) {
		const char* value_as_cstring = node.child_value();
		size_t num_jumped_chars = 0;
		class_labels = new int[num_classes];
		for (unsigned int c = 0; c < num_classes; c++) {
			sscanf(value_as_cstring + num_jumped_chars, "%d", &class_labels[c]);
			size_t local_jump = strcspn(value_as_cstring + num_jumped_chars, " ");
			num_jumped_chars += local_jump + 1;
		}
	}
	else{
		if (num_classes > 0){
			class_labels = new int[num_classes];
			for (unsigned int c = 0; c < num_classes; c++)
				class_labels[c] = c;
		}
	}
	
	return true;
}
//---------------------------------------------------------------------------
