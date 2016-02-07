#ifndef MEP_STATS_H
#define MEP_STATS_H

#include "mep_chromosome.h"


//-----------------------------------------------------------------
class t_mep_statistics{
public:
	double running_time;
	double *best_training_error;
	double best_validation_error;
	double test_error;
	double *average_training_error;
	t_mep_chromosome prg;
	int last_gen;

	void allocate(long num_gen);

	t_mep_statistics(void);
	~t_mep_statistics();

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent, int num_gens, int code_length);
	int to_csv(const char *file_name);

};
//-----------------------------------------------------------------


#endif