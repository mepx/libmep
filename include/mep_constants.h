#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "pugixml.hpp"

#define USER_DEFINED_CONSTANTS 0
#define AUTOMATIC_CONSTANTS 1

//-----------------------------------------------------------------
class t_mep_constants{
private:
	long num_automatic_constants;
	long num_user_defined_constants;
	double min_constants_interval_double, max_constants_interval_double;

	double *constants_double;

	int constants_type;
	bool constants_can_evolve;
	double constants_mutation_max_deviation;

	bool modified;

public:
	t_mep_constants(void);
	~t_mep_constants(void);
	void init(void);
	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent);


	// returns the number of automatic constants
	long get_num_automatic_constants(void);

	// returns the number of user defined constants
	long get_num_user_defined_constants(void);

	// returns the min interval for automatic constants
	double get_min_constants_interval_double(void);

	// returns the max interval for automatic constants
	double get_max_constants_interval_double(void);

	// returns a constant
	double get_constants_double(long index);

	// returns the constants type
	// 0 - USER_DEFINED_CONSTANTS
	// 1- AUTOMATIC_CONSTANTS
	long get_constants_type(void);

	// returns true if the constants can evolve
	bool get_constants_can_evolve(void);

	// returns the max deviation within which the constants can evolve
	double get_constants_mutation_max_deviation(void);

	// sets the number of automatically generated constants
	void set_num_automatic_constants(long value);

	// sets the number of user defined constants
	void set_num_user_defined_constants(long value);

	// sets the lower bound of the constants interval
	void set_min_constants_interval_double(double value);

	// sets the upper bound of the constants interval
	void set_max_constants_interval_double(double value);

	// sets a particular user-defined constant
	void set_constants_double(long index, double value);

	// sets the constants type
	// 0 - USER_DEFINED_CONSTANTS
	// 1- AUTOMATIC_CONSTANTS
	void set_constants_type(long value);

	// sets if the constants can evolve
	void  set_constants_can_evolve(bool value);

	// sets the max deviation when constants can evolve
	void set_constants_mutation_max_deviation(double value);
};
//-----------------------------------------------------------------
#endif
