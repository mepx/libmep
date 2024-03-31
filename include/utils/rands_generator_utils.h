// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#ifndef RANDS_GENERATOR_UTILS_H
#define RANDS_GENERATOR_UTILS_H
//---------------------------------------------------------------------------
#include <inttypes.h>
//---------------------------------------------------------------------------
class t_seed{
public:
	uint32_t z1, z2;
	t_seed(void);
	t_seed(uint32_t seed1);
	void init(uint32_t seed1);
};
//---------------------------------------------------------------------------
int mep_int_01_rand(t_seed& seed);
unsigned int mep_unsigned_int_rand(t_seed& seed, unsigned int _min, unsigned int _max);
long long mep_long_long_rand(t_seed &seed, long long _min, long long _max);
double mep_real_rand(t_seed &seed, double _min, double _max);
//---------------------------------------------------------------------------
#endif
