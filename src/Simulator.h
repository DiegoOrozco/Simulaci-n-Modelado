#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <list>

#include "Message.h"
#include <cmath>


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
        // 
        int max = 0;
    
    public:
        Simulator();
        ~Simulator();
        int run(char** argc);
        // Generar tiempo de conversión
        double generate_conversion_time();
        // Generar tiempo de revisión
        double generate_check_time();
        // Generar tiempo entre arribos
        double generate_arrival_time();
};
#endif // SIMULATOR_H


