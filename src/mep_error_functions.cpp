#include <math.h>
//---------------------------------------------------------------------------
#include "mep_error_functions.h"
//---------------------------------------------------------------------------
double mep_absolute_error_double(double x, double y)
{
	return fabs(x - y);
}
//---------------------------------------------------------------------------
double mep_squared_error_double(double x, double y)
{
	return (x - y) * (x - y);
}
//---------------------------------------------------------------------------
double mep_absolute_error_long(long long x, long long y)
{
	return fabs(x - y);
}
//---------------------------------------------------------------------------
double mep_squared_error_long(long long x, long long y)
{
	return (double)((x - y) * (x - y));
}
//---------------------------------------------------------------------------
