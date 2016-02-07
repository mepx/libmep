#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "pugixml.hpp"

#define USER_DEFINED_CONSTANTS 0
#define AUTOMATIC_CONSTANTS 1

//-----------------------------------------------------------------
class t_mep_constants{
public:
	long num_automatic_constants;
	long num_user_defined_constants;
	double min_constants_interval_double, max_constants_interval_double;

	double *constants_double;

	int constants_type;
	bool constants_can_evolve;
	double constants_mutation_max_deviation;

	t_mep_constants(void);
	~t_mep_constants(void);
	void init(void);
	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent);
};
//-----------------------------------------------------------------
#endif
