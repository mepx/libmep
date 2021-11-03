//taken from: https://www.iro.umontreal.ca/~simardr/rng/lfsr113.c

#include "mep_rands.h"

//---------------------------------------------------------------------------
t_seed::t_seed(void)
{
	z1 = z2 = z3 = z4 = 987654321;
}
//---------------------------------------------------------------------------
void t_seed::init(uint32_t seed)
{
	z1 = z2 = z3 = z4 = 987654321 + seed;
}
//---------------------------------------------------------------------------
double RNG(t_seed &seed)
{
	uint32_t b;
	b = ((seed.z1 << 6) ^ seed.z1) >> 13;
	seed.z1 = ((seed.z1 & 4294967294U) << 18) ^ b;
	b = ((seed.z2 << 2) ^ seed.z2) >> 27;
	seed.z2 = ((seed.z2 & 4294967288U) << 2) ^ b;
	b = ((seed.z3 << 13) ^ seed.z3) >> 21;
	seed.z3 = ((seed.z3 & 4294967280U) << 7) ^ b;
	b = ((seed.z4 << 3) ^ seed.z4) >> 12;
	seed.z4 = ((seed.z4 & 4294967168U) << 13) ^ b;
	return (seed.z1 ^ seed.z2 ^ seed.z3 ^ seed.z4) * 2.3283064365386963e-10;
}
//---------------------------------------------------------------------------
int mep_int_rand(t_seed &seed, int _min, int _max)
{
	return int(RNG(seed) * (_max - _min + 1) + _min);
}
//---------------------------------------------------------------------------
double mep_real_rand(t_seed &seed, double _min, double _max)
{
	return RNG(seed) * (_max - _min) + _min;
}
//---------------------------------------------------------------------------
int rand_int_01(t_seed& seed)
{
	return RNG(seed) >= 0.5;
}
//---------------------------------------------------------------------------
char rand_char_01(t_seed& seed)
{
	return char(RNG(seed) >= 0.5);
}
//---------------------------------------------------------------------------
double rand_real_01(t_seed& seed)
{
	return RNG(seed);
}
//---------------------------------------------------------------------------