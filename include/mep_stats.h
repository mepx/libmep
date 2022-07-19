// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#ifndef MEP_STATS_H
#define MEP_STATS_H
//-----------------------------------------------------------------
#include "mep_chromosome.h"
//-----------------------------------------------------------------
class t_mep_run_statistics{
public:
	double running_time;

	double *best_training_error;
	double *average_training_error;
	double *best_training_num_incorrect;
	double *average_training_num_incorrect;

	double best_validation_error;
	double best_validation_num_incorrect;

	double test_error;
	double test_num_incorrect;	

	t_mep_chromosome best_program;

	// could be less than the number of generations of the algorithm if the user stops it earlier
	int last_generation;
	unsigned int num_generations;

	t_mep_run_statistics(void);
	~t_mep_run_statistics();

	void allocate_memory(unsigned int num_generations);
	void delete_memory(void);

	t_mep_run_statistics& operator=(const t_mep_run_statistics& source);

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent,
				 unsigned int problem_type, unsigned int error_measure,
				 unsigned int num_classes);

};
//-----------------------------------------------------------------
class t_mep_statistics{
private:
	t_mep_run_statistics *stats;

	double stddev_training_error, stddev_validation_error, stddev_test_error, stddev_runtime;
	double mean_training_error, mean_validation_error, mean_test_error, mean_runtime;
	double best_training_error, best_validation_error, best_test_error, best_runtime;

	double stddev_training_num_incorrect, stddev_validation_num_incorrect, stddev_test_num_incorrect;
	double mean_training_num_incorrect, mean_validation_num_incorrect, mean_test_num_incorrect;
	double best_training_num_incorrect, best_validation_num_incorrect, best_test_num_incorrect;

public:

	unsigned int num_runs;

	t_mep_statistics();

	t_mep_statistics& operator=(const t_mep_statistics& source);

	t_mep_run_statistics *get_stat_ptr(unsigned int index)  const;

	void get_best_error(double &training, double &validation, double &test, double &running_time) const;
	void get_mean_error(double &training, double &validation, double &test, double &running_time) const;
	void get_sttdev_error(double &training, double &validation, double &test, double &running_time) const;

	void get_best_num_incorrect(double &training, double &validation, double &test, double &running_time) const;
	void get_mean_num_incorrect(double &training, double &validation, double &test, double &running_time) const;
	void get_sttdev_num_incorrect(double &training, double &validation, double &test, double &running_time) const;

	// returns the best training error
	double get_best_training_error(unsigned int run, unsigned int generation) const;

	// returns the best validation error
	double get_best_validation_error(unsigned int run) const;

	// returns the average (over the entire population) training error
	double get_average_training_error(unsigned int run, unsigned int generation) const;

	// returns the best training num incorreclty classified
	double get_best_training_num_incorrectly_classified(unsigned int run, unsigned int generation) const;

	// returns the best validation num incorreclty classified
	double get_best_validation_num_incorrectly_classified(unsigned int run) const;

	// returns the average (over the entire population) training num incorreclty classified
	double get_mean_training_num_incorrectly_classified(unsigned int run, unsigned int generation) const;

	// returns the average (over the entire population) training num incorreclty classified
	double get_stddev_training_num_incorrectly_classified(unsigned int run, unsigned int generation);

	// returns the running time
	double get_running_time(unsigned int run) const;

	// returns the error on the test data
	double get_test_error(unsigned int run) const;

	// returns the num incorreclty classified on the test data
	double get_test_num_incorrectly_classified(unsigned int run) const;

	// save statistics to csv file
	int to_csv(const char* file_name, unsigned int problem_type)const;
	int to_tex(const char* file_name, unsigned int problem_type)const;
	int to_html(const char* file_name, unsigned int problem_type)const;

	void sort_stats_by_running_time(bool ascending);
	void sort_stats_by_training_error(bool ascending);
	void sort_stats_by_validation_error(bool ascending);
	void sort_stats_by_test_error(bool ascending);

	// returns the last generation of a given run (useful when the run has been stopped earlier)
	int get_latest_generation(unsigned int run) const;

	void delete_memory(void);
	void append(unsigned int num_generations);
	void create(unsigned int _num_runs);

	void compute_mean_stddev(bool compute_on_validation, bool compute_on_test);

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent,
				 unsigned int problem_type, unsigned int error_measure,
				 unsigned int num_classes);
};
//-----------------------------------------------------------------
#endif
