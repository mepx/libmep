#ifndef MEP_STATS_H
#define MEP_STATS_H

#include "chromosome_class.h"


//-----------------------------------------------------------------
class t_mep_stat{
public:
	double running_time;
	double *best_training_error;
	double best_validation_error;
	double test_error;
	double *average_training_error;
	chromosome prg;
	int last_gen;

	void allocate(long num_gen);

	t_mep_stat(void);
	~t_mep_stat();

	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent, int num_gens, int code_length);
	int to_csv(const char *file_name);

};
//-----------------------------------------------------------------


#endif