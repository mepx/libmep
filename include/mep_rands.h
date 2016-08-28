#ifndef MEP_RANDS_H
#define MEP_RANDS_H

#include <inttypes.h>


struct t_seed{
	uint32_t z1, z2, z3, z4;
	t_seed(void);
	void init(uint32_t seed);
};


int mep_int_rand(t_seed &seed, int _min, int _max);
double mep_real_rand(t_seed &seed, double _min, double _max);

#endif