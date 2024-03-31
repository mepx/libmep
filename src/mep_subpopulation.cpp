// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <stdlib.h>

#include "mep_subpopulation.h"
//-----------------------------------------------------------------
t_sub_population::t_sub_population()
{
	individuals = NULL;
	worst_index = best_index = 0;
}
//-----------------------------------------------------------------
void t_sub_population::compute_index_of_the_worst(unsigned int pop_size, double precision)
{
	worst_index = 0;
	for (unsigned int i = 1; i < pop_size; i++)
		if (individuals[i].compare(individuals[worst_index], precision) > 0)
			worst_index = i;
}
//-----------------------------------------------------------------
void t_sub_population::compute_index_of_the_best(unsigned int pop_size, double precision)
{
	best_index = 0;
	for (unsigned int i = 1; i < pop_size; i++)
		if (individuals[i].compare(individuals[best_index], precision) < 0)
			best_index = i;
}
//-----------------------------------------------------------------
