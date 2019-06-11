#include <iostream>
#include <cstdio>

#include "Simulator.h"
#include "Stats.h"


void print_stats(std::list<Stats> & stats)
{
	int run = 1;
	double permanence_average = 0.0;
	for(auto i = stats.begin(); i != stats.end(); ++i)
	{
		printf("Estadísticas para la corrida %d\n", run++);
		(*i).print_stats();
		permanence_average += (*i).get_permanence();
		printf("\n");
	}

	printf("Intervalo de confianza: wtf\n");
	printf("Prom. permanencia en el sistema: %lf\n", permanence_average/stats.size());
}

int main(int argc, char* argv[])
{
	int ret = 1;
	if(argc >= 5)
	{
		int times = atoi(argv[4]);
		std::list<Stats> stats;

		for (int index = 0; index < times; ++index)
		{
			Simulator simulator;
			ret = simulator.run(argv, stats);
		}

		print_stats(stats);
	}
	else
	{
		std::cout << "Usage <Total time> <timeout> <delay> <times>" << std::endl;
	}

  	return ret;
}