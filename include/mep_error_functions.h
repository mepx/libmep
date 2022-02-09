#ifndef mep_error_functions_H
#define mep_error_functions_H

typedef double (*t_mep_error_function)(double, double);

double mep_absolute_error(double x, double y);
double mep_squared_error(double x, double y);

#endif