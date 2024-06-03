//taken from: https://prng.di.unimi.it/xoroshiro64starstar.c
//---------------------------------------------------------------------------
#include "utils/rands_generator_utils.h"
//---------------------------------------------------------------------------
t_seed::t_seed(void)
{
	z1 = z2 = 1;
}
//---------------------------------------------------------------------------
t_seed::t_seed(uint32_t seed)
{
	init(seed);
}
//---------------------------------------------------------------------------
void t_seed::init(uint32_t seed)
{
	z1 = 1 + seed;
	z2 = 1 + seed;
}
//---------------------------------------------------------------------------
uint32_t rotl(const uint32_t x, int k)
{
	return (x << k) | (x >> (32 - k));
}
//---------------------------------------------------------------------------
uint32_t RNG(t_seed &state)
{
	const uint32_t s0 = state.z1;
	uint32_t s1 = state.z2;
	const uint32_t result = rotl(s0 * 0x9E3779BB, 5) * 5;

	s1 ^= s0;
	state.z1 = rotl(s0, 26) ^ s1 ^ (s1 << 9); // a, b
	state.z2 = rotl(s1, 13); // c

	return result;
}
//---------------------------------------------------------------------------
unsigned int mep_unsigned_int_rand(t_seed &seed, unsigned int _min, unsigned int _max)
{
	return (unsigned int)(RNG(seed) % (_max - _min) + _min);
}
//---------------------------------------------------------------------------
long long mep_long_long_rand(t_seed &seed, long long _min, long long _max)
{
	return (long long)(RNG(seed) % (_max - _min) + _min);
}
//---------------------------------------------------------------------------
double mep_real_rand(t_seed &seed, double _min, double _max)
{
	return RNG(seed) / (double)0xFFFFFFFF * (_max - _min) + _min;
}
//---------------------------------------------------------------------------
unsigned int mep_int_01_rand(t_seed& seed)
{
	return RNG(seed) % 2;
}
//---------------------------------------------------------------------------
char mep_char_01_rand(t_seed& seed)
{
	return char(RNG(seed) % 2);
}
//---------------------------------------------------------------------------
double mep_real_01_rand(t_seed& seed)
{
	return RNG(seed);
}
//---------------------------------------------------------------------------
