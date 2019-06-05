#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <list>

#include "Message.h"


class Simulator
{
    private:  
        // Llega un mensaje a A
        int message_arrival();
        // Se libera A
        int a_released();
        // Llega un frame a B
        int frame_arrival();
        // Se libera B
        int b_released();
        // Llega un ACK a A
        int ack_arrival();  
        //  timer que se vence el timer timeout time
        int timeout();
        //
        int minimum();
        // linea de tiempo miedo
        unsigned int timeline[6];
        // 
        std::list<Message> message_list;

        int max = 0;
    
    public:
        Simulator();
        ~Simulator();
        int run(char** argc);
      
};
#endif // SIMULATOR_H