// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//------------------------------------------------------
#include "utils/validation_double.h"
//------------------------------------------------------
bool is_valid_long_long(double value)
{
	return (long long)value == value;
}
//------------------------------------------------------
