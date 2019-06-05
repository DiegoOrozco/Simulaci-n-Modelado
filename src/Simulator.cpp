
#include "Simulator.h"
#include <iostream>
#include <ctime>

Simulator::Simulator()
{
	this->timeline[0] = 0;
	for(int index = 1; index < 6; ++index)
		this->timeline[index] = -1;
}

Simulator::~Simulator()
{
}

int Simulator::minimum()
{
	unsigned int min = this->timeline[0];
	int index = 0;
	for(int i = 1; i < 6; ++i)
	{
		if(this->timeline[i] < min)
		{
			min = this->timeline[i];
			index = i;
		}
	}
	
	return index;
}

int Simulator::run(char* argv[])
{
	// cambiar si es con interfaz
	this->max = (unsigned int) atoi(argv[1]);
	bool stop = false;
	int ret = 0;
	while(!stop)
	{
		int minimum = this->minimum();
		
		switch(minimum)
		{
			case 0: ret = this->message_arrival();	 	break;
			case 1: ret = this->a_released(); 			break;
			case 2: ret = this->frame_arrival(); 		break;
			case 3: ret = this->b_released(); 			break;
			case 4: ret = this->ack_arrival(); 			break;
			case 5: ret = this->timeout(); 				break;
			default: std::cout << "Wtf" << std::endl; 	break;
		}
		
		if(ret == 1)
			stop = true;
	}
	
	// print_statistics();
	
	return 0;
}

// Llega un mensaje a A
int Simulator::message_arrival()
{
	
}

// Se libera A
int Simulator::a_released()
{
	
}

// Llega un frame a B
int Simulator::frame_arrival()
{
	
}

// Se libera B
int Simulator::b_released()
{
	
}

// Llega un ACK a A
int Simulator::ack_arrival()
{
	
}

//  timer que se vence el timer timeout time
int Simulator::timeout()
{
	
}

// Generar tiempo de conversión
double Simulator::generate_conversion_time()
{
  	double result = 0.0;
  
  	while(result == 0.0)
    {
        std::srand(std::time(NULL));
        double r = ( std::rand() % 1000 ) / 1000;
        result = -2 * std::log(1-r);
    }
	
  	return result;
}

// Generar tiempo de revisión
double Simulator::generate_check_time()
{
  	std::srand(std::time(NULL));
  	double r = ( std::rand() % 1000 ) / 1000;
  	return sqrt(5*r+4);
}


// Generar tiempo entre arribos
double Simulator::generate_arrival_time()
{
  	double r[12];
  
	for (int index = 0; index < 12; ++index)
    {
        std::srand(std::time(NULL));
        r[index] = ( std::rand() % 1000 ) / 1000;    
    }
	
  	double sum = 0.0;
  	for (int index = 0; index < 12; ++index)
    	sum += r[index];
  
	sum -= 6;
  
  	return (1 * sum + 25);
}


