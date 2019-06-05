
// Simulator.cpp

#include "Simulator.h"
#include <iostream>

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


