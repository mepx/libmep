#ifndef MEP_STATS_H
#define MEP_STATS_H

#include "mep_chromosome.h"


//-----------------------------------------------------------------
class t_mep_run_statistics{
public:
	double running_time;
	double *best_training_error;
	double best_validation_error;
	double test_error;
	double *average_training_error;

	t_mep_chromosome best_program;

	// could be less than the number of generations of the algorithm if the user stops it earlier
	int last_generation;

	void allocate(long num_generations);

	t_mep_run_statistics(void);
	~t_mep_run_statistics();

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent, int num_gens, int code_length, int problem_type);
	//int to_csv(const char *file_name);

	void compute_mean_stddev(int num_runs);

};
//-----------------------------------------------------------------


#endif