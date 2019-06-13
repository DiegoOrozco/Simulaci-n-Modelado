#include <iostream>
#include <cstdio>
#include <cmath>

#include "Simulator.h"
#include "Stats.h"


void print_stats(std::list<Stats> & stats)
{
	int run = 1;

	double queue_sum = 0.0;
	double permanence_sum = 0.0;
	double transmission_sum = 0.0;
	double service_sum = 0.0;
	double efficiency_sum = 0.0;

	for(auto i = stats.begin(); i != stats.end(); ++i)
	{
		printf("Estadísticas para la corrida %d\n", run++);
		(*i).print_stats();

		queue_sum += (*i).get_queue();
		permanence_sum += (*i).get_permanence();
		transmission_sum += (*i).get_transmission();
		service_sum += (*i).get_service();
		efficiency_sum += (*i).get_efficiency();

		printf("\n");
	}

	double permanence_average = permanence_sum/stats.size();

	double variance = 0.0;

	for(auto i = stats.begin(); i != stats.end(); ++i)
	{
		double subtraction = (*i).get_permanence() - permanence_average;
		variance += subtraction * subtraction;
	}

	if((stats.size() - 1) == 0)
		variance = 0;
	else
		variance /= ((double)stats.size() - 1);

	double lower_limit = permanence_average - 2.26 * sqrt(variance / 10);
	double upper_limit = permanence_average + 2.26 * sqrt(variance / 10);

	printf("Intervalo de confianza del 95%%: Límite inferior = %lf, Límite superior = %lf\n", lower_limit, upper_limit);

	printf("Promedio del tamaño de la cola: %lf\n", queue_sum/stats.size());
	printf("Promedio de permanencia de un mensaje: %lf\n", permanence_average);
	printf("Promedio de transmisión: %lf\n", transmission_sum/stats.size());
	printf("Promedio del tiempo de servicio: %lf\n", service_sum/stats.size());
	printf("Promedio de eficencia: %lf\n", efficiency_sum/stats.size());

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