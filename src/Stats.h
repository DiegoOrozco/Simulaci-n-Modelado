#ifndef STATS_H
#define STATS_H

class Stats 
{
private:
	double queue_size;
	double permanence_time;
	double transmission_time;
	double service_time;
	double efficiency;

public:
	Stats(double queue_size, double permanence_time, double transmission_time, double service_time, double efficiency);
	~Stats();

	void print_stats();

	inline double get_queue() { return this->queue_size; }
	inline double get_permanence() { return this->permanence_time; }
	inline double get_transmission() { return this->transmission_time; }
	inline double get_service() { return this->service_time; }
	inline double get_efficiency() { return this->efficiency; }

};

#endif // STATS_H
