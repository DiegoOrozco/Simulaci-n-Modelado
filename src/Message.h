#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <list>

#include "Message.h"

class Simulator
{
	private:  
  		// Llega un mensaje a A
		void message_arrival();
  		// Se libera A
  		void a_released();
  		// Llega un frame a B
        void frame_arrival();
      	// Se libera B
        void b_released();
  		// Llega un ACK a A
        void ack_arrival();  
  		//  timer que se vence el timer timeout time
  		void timeout();
  		// linea de tiempo miedo
  		unsigned int timeline[6] = {0, -1, -1, -1, -1}
  		// 
  		std::list<Messasge> message_list;
    
  	public:
        Simulator();
        ~Simulator();
        int run(char* argc[]);
      
};
#endif // SIMULATOR_H