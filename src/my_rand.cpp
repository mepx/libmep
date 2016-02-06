#include <inttypes.h>

static uint64_t my_next = 1;
//---------------------------------------------------------
uint32_t my_rand(void) // RAND_MAX assumed to be 32767
{
	my_next = my_next * 1103515245 + 12345;
	return (uint32_t)(my_next / 65536) % 32768;
}
//---------------------------------------------------------
void my_srand(uint32_t seed)
{
    my_next = seed;
}
//---------------------------------------------------------