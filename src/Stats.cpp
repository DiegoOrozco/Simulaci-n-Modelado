#include <cstdio>
#include "Stats.h"

Stats::Stats(double queue_size, double permanence_time, double transmission_time, double service_time, double efficiency)
	: queue_size{queue_size}
	, permanence_time{permanence_time}
	, transmission_time{transmission_time}
	, service_time{service_time}
	, efficiency{efficiency}
{
}

Stats::~Stats()
{
}

void Stats::print_stats()
{
	printf("Tamaño promedio de la cola:\t%lf\n", this->queue_size);
	printf("Tiempo promedio de permanencia:\t%lf\n", this->permanence_time);
	printf("Tiempo promedio de transmisión:\t%lf\n", this->transmission_time);
	printf("Tiempo promedio de servicio:\t%lf\n", this->service_time);
	printf("Eficencia:\t\t\t%lf\n", this->efficiency);
}