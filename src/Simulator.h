#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <list>
#include <cmath>
#include <queue> 


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
        //
        void update_data(const char* event);
        // linea de tiempo miedo
        double timeline[6];
        // 
        std::list<Message> message_list;
        //
        std::queue<long> frame_queue;
  
        std::queue<int> ack_queue;
        // 
        double max_time;
        // Mensaje que A está atendiendo
        int current_message;
        // Frame que B está atendiendo
        int current_frame;
        // ACK que estoy esperando recibir (el primero de la ventana)
        int current_ack;
        // Cantidad de mensajes que han recibido el ACK
        int acked_messages;
        //
        bool A_free;
        //
        bool B_free;
        //
        double clock;
        //
        int total_message;
        //
        double time_out;
        //
        int max_window_size;
        //
        int frames_received;

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

