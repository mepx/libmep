// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include "mep_subpopulation.h"

//-----------------------------------------------------------------
t_sub_population::t_sub_population()
{
	individuals = NULL;
	worst_index = best_index = -1;
}
//-----------------------------------------------------------------
void t_sub_population::compute_worst_index(int pop_size) 
{
	worst_index = 0;
	double worst_fitness = individuals[0].get_fitness();
	for (int i = 1; i < pop_size; i++)
		if (individuals[i].get_fitness() > worst_fitness) {
			worst_fitness = individuals[i].get_fitness();
			worst_index = i;
		}
}
//-----------------------------------------------------------------
void t_sub_population::compute_best_index(int pop_size) 
{
	best_index = 0;
	double best_fitness = individuals[0].get_fitness();
	for (int i = 1; i < pop_size; i++)
		if (individuals[i].get_fitness() < best_fitness) {
			best_fitness = individuals[i].get_fitness();
			best_index = i;
		}
}
//-----------------------------------------------------------------