// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef mep_constants_H
#define mep_constants_H

#include "pugixml.hpp"
//-----------------------------------------------------------------

#define MEP_USER_DEFINED_CONSTANTS 0
#define MEP_AUTOMATIC_CONSTANTS 1

//-----------------------------------------------------------------
class t_mep_constants{
private:
	unsigned int num_automatic_constants;
	unsigned int num_user_defined_constants;
	double min_constants_interval_double, max_constants_interval_double;

	double *constants_double;

	unsigned int constants_type;
	bool constants_can_evolve;
	bool constants_can_evolve_outside_initial_interval;
	double constants_mutation_max_deviation;

	bool modified;

public:
	t_mep_constants(void);
	~t_mep_constants(void);

	bool operator==(const t_mep_constants& other);
	void init(void);
	int to_xml(pugi::xml_node parent);
	int from_xml(pugi::xml_node parent);

	t_mep_constants& operator=(const t_mep_constants &source);

	// returns the number of automatic constants
	unsigned int get_num_automatic_constants(void)const;

	// returns the number of user defined constants
	unsigned int get_num_user_defined_constants(void)const;

	// returns the min interval for automatic constants
	double get_min_constants_interval_double(void)const;

	// returns the max interval for automatic constants
	double get_max_constants_interval_double(void)const;

	// returns a constant
	double get_constants_double(unsigned int index)const;

	// returns the constants type
	// 0 - USER_DEFINED_CONSTANTS
	// 1- AUTOMATIC_CONSTANTS
	unsigned int get_constants_type(void)const;

	// returns true if the constants can evolve
	bool get_constants_can_evolve(void)const;

	// returns true if the constants can evolve outside interval
	bool get_constants_can_evolve_outside_initial_interval(void)const;

	// returns the max deviation within which the constants can evolve
	double get_constants_mutation_max_deviation(void)const;

	// sets the number of automatically generated constants
	void set_num_automatic_constants(unsigned int value);

	// sets the number of user defined constants
	void set_num_user_defined_constants(unsigned int value);

	// sets the lower bound of the constants interval
	void set_min_constants_interval_double(double value);

	// sets the upper bound of the constants interval
	void set_max_constants_interval_double(double value);

	// sets a particular user-defined constant
	void set_constants_double(unsigned int index, double value);

	// sets the constants type
	// 0 - USER_DEFINED_CONSTANTS
	// 1- AUTOMATIC_CONSTANTS
	void set_constants_type(unsigned int value);

	// sets if the constants can evolve
	void  set_constants_can_evolve(bool value);

	// sets if the constants can evolve outside initial interval
	void  set_constants_can_evolve_outside_initial_interval(bool value);

	// sets the max deviation when constants can evolve
	void set_constants_mutation_max_deviation(double value);
};
//-----------------------------------------------------------------
#endif
