// Multi Expression Programming library
// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <stdlib.h>
// ---------------------------------------------------------------------------
bool is_valid_int(const char* str, long *value)
{
	if (!str)
		return 0;
	char * pEnd;
	*value = strtol(str, &pEnd, 10);
	if ((pEnd == NULL) || (pEnd[0] == 0))
		return 1;
	return 0;
}
// ---------------------------------------------------------------------------
bool is_valid_double(const char* str, double *value)
{
	if (!str)
		return 0;
	char * pEnd;
	*value = strtod(str, &pEnd);
	if ((pEnd == NULL) || (pEnd[0] == 0))
		return 1;
	return 0;
}
// ---------------------------------------------------------------------------
bool is_valid_long_long(const char* str, long long *value)
{
	if (!str)
		return 0;
	char * pEnd;
	*value = strtoll(str, &pEnd, 10);
	if ((pEnd == NULL) || (pEnd[0] == 0))
		return 1;
	return 0;
}
// ---------------------------------------------------------------------------
