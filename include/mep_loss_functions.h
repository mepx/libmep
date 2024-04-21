// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef mep_loss_functions_H
#define mep_loss_functions_H
//-----------------------------------------------------------------
typedef double (*t_mep_error_function_double)(double, double);
//-----------------------------------------------------------------
double mep_absolute_error_double(double x, double y);
double mep_squared_error_double(double x, double y);
//-----------------------------------------------------------------
typedef double (*t_mep_error_function_long)(long long, long long);
//-----------------------------------------------------------------
double mep_absolute_error_long(long long x, long long y);
double mep_squared_error_long(long long x, long long y);
//-----------------------------------------------------------------
#endif
