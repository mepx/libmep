#include <inttypes.h>
#include <stdlib.h>
#include <random>
#include <thread>

void my_srand(uint32_t seed)
{

	srand(seed);
    //my_next = seed;
}
//---------------------------------------------------------
// taken from http://stackoverflow.com/questions/18298280/how-to-declare-a-variable-as-thread-local-portably

#ifndef thread_local
# if __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#  define thread_local _Thread_local
# elif defined _WIN32 && ( \
       defined _MSC_VER || \
       defined __ICL || \
       defined __DMC__ || \
       defined __BORLANDC__ )
#  define thread_local __declspec(thread) 
/* note that ICC (linux) and Clang are covered by __GNUC__ */
# elif defined __GNUC__ || \
       defined __SUNPRO_C || \
       defined __xlC__
#  define thread_local __thread
# endif
#endif
//---------------------------------------------------------
#ifdef thread_local
int my_int_rand(int _min, int _max)
{
	static thread_local std::mt19937* generator = nullptr;
	if (!generator) generator = new std::mt19937(clock() + std::this_thread::get_id().hash());
	std::uniform_int_distribution<int> distribution(_min, _max);
	int r = distribution(*generator);
	//	printf("%d ", r);
	return r;
}
//---------------------------------------------------------
double my_real_rand(double _min, double _max)
{
	static thread_local std::mt19937* generator = nullptr;
	if (!generator) generator = new std::mt19937(clock() + std::this_thread::get_id().hash());
	std::uniform_real_distribution<double> distribution(_min, _max);
	double r = distribution(*generator);
	//	printf("%d ", r);
	return r;
}
# else
//---------------------------------------------------------
int my_int_rand(int _min, int _max)
{
	return rand() % (_max - _min + 1) + _min;
}
//---------------------------------------------------------
double my_real_rand(int _min, int _max)
{
	return rand() / (double)RAND_MAX * (_max - _min) + _min;
}
#endif
//---------------------------------------------------------
