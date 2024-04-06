// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef mep_constants_H
#define mep_constants_H
//-----------------------------------------------------------------
#include "utils/pugixml.hpp"
#include "mep_parameters.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
class t_mep_constants{
private:
	unsigned int num_automatic_constants;
	unsigned int num_user_defined_constants;
	double min_constants_interval_double, max_constants_interval_double;
	long long min_constants_interval_long, max_constants_interval_long;

	double *constants_double;
	
	long long *constants_long;

	unsigned int constants_type;
	bool constants_can_evolve;
	bool constants_can_evolve_outside_initial_interval;
	
	double constants_mutation_max_deviation_double;
	
	long long constants_mutation_max_deviation_long;

	bool modified;
	
	char data_type; // 0-double, 1-string, 2-long long

public:
	t_mep_constants(void);
	~t_mep_constants(void);

	bool operator==(const t_mep_constants& other) const;
	void init(void);
	void to_xml(pugi::xml_node parent);
	bool from_xml(pugi::xml_node parent);

	t_mep_constants& operator=(const t_mep_constants &source);

	// returns the number of automatic constants
	unsigned int get_num_automatic_constants(void) const;

	// returns the number of user defined constants
	unsigned int get_num_user_defined_constants(void) const;

	// returns the min interval for automatic constants
	double get_min_constants_interval_double(void) const;
	long long get_min_constants_interval_long(void) const;

	// returns the max interval for automatic constants
	double get_max_constants_interval_double(void) const;
	long long get_max_constants_interval_long(void) const;

	// returns a constant
	double get_constants_double(unsigned int index) const;
	long long get_constants_long(unsigned int index) const;

	// returns the constants type
	// 0 - USER_DEFINED_CONSTANTS
	// 1- AUTOMATIC_CONSTANTS
	unsigned int get_constants_type(void) const;

	// returns true if the constants can evolve
	bool get_constants_can_evolve(void) const;

	// returns true if the constants can evolve outside interval
	bool get_constants_can_evolve_outside_initial_interval(void) const;

	// returns the max deviation within which the constants can evolve
	double get_constants_mutation_max_deviation_double(void) const;
	long long get_constants_mutation_max_deviation_long(void) const;

	// sets the number of automatically generated constantss
	void set_num_automatic_constants(unsigned int value);

	// sets the number of user defined constants
	void set_num_user_defined_constants(unsigned int value);

	// sets the lower bound of the constants interval
	void set_min_constants_interval_double(double value);
	void set_min_constants_interval_long(long long value);

	// sets the upper bound of the constants interval
	void set_max_constants_interval_double(double value);
	void set_max_constants_interval_long(long long value);

	// sets a particular user-defined constant
	void set_constants_double(unsigned int index, double value);
	void set_constants_long(unsigned int index, long long value);

	// sets the constants type
	// 0 - USER_DEFINED_CONSTANTS
	// 1- AUTOMATIC_CONSTANTS
	void set_constants_type(unsigned int value);

	// sets if the constants can evolve
	void set_constants_can_evolve(bool value);

	// sets if the constants can evolve outside initial interval
	void set_constants_can_evolve_outside_initial_interval(bool value);

	// sets the max deviation when constants can evolve
	void set_constants_mutation_max_deviation_double(double value);
	void set_constants_mutation_max_deviation_long(long long value);
	
	char get_data_type(void) const;
	
	bool set_data_type(char data_type);
	void set_data_type_no_update(char data_type);
};
//-----------------------------------------------------------------
#endif
