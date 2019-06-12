#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <list>
#include <cmath>
#include <queue> 
#include <deque>

#include "Message.h"
#include "queue.h"
#include "Stats.h"


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
        //  Se vence el timer
        int timeout();
        // Consigue el mínimo de los tiempos
        int minimum();
        // Estadísticas durante la corrida
        void update_data(const char* event);
        // linea de tiempo 
        double timeline[6];
        // Cola de mensajes por enviar
        std::list<Message> message_list;
        // Cola de frames recibidos
        std::deque<Message> frame_queue;
        // Cola con los id de los frames recibidos
        std::deque<int> frame_received;
        // Cola de ACKs recibidos
        std::deque<int> ack_queue;
        // 
        std::deque<int> prom_message;
        // 
        std::deque<double> prom_time_message;

        // Tiempo máximo
        double max_time;
        // Mensaje que A está atendiendo
        int current_message;
        // Frame que B está atendiendo
        int current_frame;
        // ACK que estoy esperando recibir (el primero de la ventana)
        int current_ack;
        // Cantidad de mensajes que han recibido el ACK
        int acked_messages;
        // Si A está libre
        bool A_free;
        // Si B está libre
        bool B_free;
        // Reloj del sistema
        double clock;
        // Cantidad de mensajes recibidos
        int total_message;
        // Timeout definido por el usuario
        double time_out;
        //
        int max_window_size;
        //
        int received_frames;
        //
        int waiting;
        //
        int send;

        int sent_messages;

    public:
        Simulator();
        ~Simulator();
        int run(char** argc, std::list<Stats> & all_stats);
        // Generar tiempo de conversión
        double generate_conversion_time();
        // Generar tiempo de revisión
        double generate_check_time();
        // Generar tiempo entre arribos
        double generate_arrival_time();
};
#endif // SIMULATOR_H

