#include <math.h>
//---------------------------------------------------------------------------
#include "mep_error_functions.h"
//---------------------------------------------------------------------------
double mep_absolute_error(double x, double y)
{
	return fabs(x - y);
}
//---------------------------------------------------------------------------
double mep_squared_error(double x, double y)
{
	return (x - y) * (x - y);
}
//---------------------------------------------------------------------------