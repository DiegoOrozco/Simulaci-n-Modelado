
#include "Simulator.h"

#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <limits>

Simulator::Simulator()
	: max_time{0.0}
	, current_message{0}
	, current_frame{0}
	, current_ack{1}
    , acked_messages{0}
	, A_free{true}
	, B_free{true}
	, clock{0.0}
	, total_message{0}
	, time_out{0.0}

{
	this->timeline[0] = 0;
	for(int index = 1; index < 6; ++index)
		this->timeline[index] = std::numeric_limits<double>::infinity();
}

Simulator::~Simulator()
{
}

int Simulator::minimum()
{
	double min = this->timeline[0];
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
	this->max_time = (unsigned int) atoi(argv[1]);
	this->time_out = (double) atof(argv[2]);
    int delay = atoi(argv[3]);

	bool stop = false;
	int ret = 0;

	while(!stop)
	{
		int minimum = this->minimum();
		
        printf("\n");

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

        printf("Event done. Clock: %lf\n", this->clock);
        for(int i = 0; i < 6; ++i)
            printf("| %lf ", this->timeline[i]);
        printf("|\n");
        sleep(delay); 
        // int a = 0;
        // std::cin >> a;

	}
	
    printf("Stop\n");

    // Clean messages
    while(!this->message_list.empty())
        this->message_list.pop_front();

    while(!this->frame_queue.empty())
        this->frame_queue.pop();

    while(!this->ack_queue.empty())
       this->ack_queue.pop();
	
	// print_statistics();
	return 0;
}

// Llega un mensaje a A (MA)
int Simulator::message_arrival()
{
    printf("Llega mensaje a A\n");

	this->clock = this->timeline[0];
	Message new_message(false, total_message); 
  	++this->total_message;
  	this->message_list.push_back(new_message);
  	
    //if (mensaje en la ventana)
  	if (this->A_free)
    {
      	double conversion_time = generate_conversion_time();
      	this->timeline[1] = this->clock + conversion_time + 1;
		this->A_free = false;
    }
  
	int arrival_time = generate_arrival_time();
  	
	// MA = T. Arribos
	this->timeline[0] = this->clock + arrival_time;

  	if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;
}

// Se libera A
int Simulator::a_released()
{
    printf("Se libera A\n");

    auto iterator = this->message_list.begin();
    for(; iterator != this->message_list.end(); ++iterator)
        if((*iterator).get_id() == this->current_message)
            break;

    this->clock = this->timeline[1];
    std::srand(std::time(NULL));

    double random = ( std::rand() % 100 ) / 100.0;

    // si el mensaje se pierde
    if(random <= 0.05)
    {
        printf("Se pierde el mensaje\n");
        this->timeline[2] = std::numeric_limits<double>::infinity();
    }
    // Si el mensaje no se pierde
    else
    {
        // Si el mensaje viene con error
        if (random >= 0.9)
            (*iterator).set_error(true);
        else
            (*iterator).set_error(false);

        // FB = reloj + 1
        this->timeline[2] = this->clock + 1;

        printf("Se pushea frame #%d %s\n", this->current_message, random >= 0.9 ? "con error" : "sin error");
      	frame_queue.push(this->current_message);
    }

  	// Si TO es infinito
    if(this->timeline[5] == std::numeric_limits<double>::infinity() )
    {
        this->timeline[5] = this->clock + this->time_out;
    }
  	// Guardo el TO en el vector
    (*iterator).set_timeout(this->clock + this->time_out);
	
	// Siempre asumo que el mensaje se envia
    (*iterator).set_send(true);

  	++this->current_message;

  	// Si hay más mensajes en la ventana
    if(this->current_message < this->total_message)
    {
        double conversion_time = generate_conversion_time();        
        this->timeline[1] = this->clock + conversion_time + 1;
    }
    else
    {
        this->timeline[1] = std::numeric_limits<double>::infinity();
	    this->A_free = true;
    }
  
  	// Si reloj > MAX
    if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;
}

// Llega un frame a B
int Simulator::frame_arrival()
{
    printf("Llega frame a B\n");
	// Reloj = FB
	this->clock = this->timeline[2];
  	
	// Si proceso libre 
  	if (this->B_free)
    {
		this->B_free = false;

    	// Generar T. Revisión
		double check_time = generate_check_time();
    	
      	//LB = Reloj + T. Revisión + 0.25
     	this->timeline[3] = this->clock + check_time + 0.25;      
    }
  
	// FB = inf	
	this->timeline[2] = std::numeric_limits<double>::infinity();
   
    if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;
}

// Se libera B
int Simulator::b_released()
{
    printf("Se libera B\n");

	// Reloj = LB
	this->clock = this->timeline[3];
	
  	// obtengo el id del primer frame de la cola
    int index_frame = this->frame_queue.front();
    printf("Leo frame con id #%d, espero frame #%d\n", index_frame, this->current_frame);
	
  	// Si viene la secuencia esperada
  	if(index_frame == this->current_frame)
    {
        // Obtengo los datos del mensaje enviado
        auto iterator = this->message_list.begin();
        for(; iterator != this->message_list.end(); ++iterator)
            if((*iterator).get_id() == index_frame)
                break;
         
        // Si frame bueno
        if ( !(*iterator).get_error() )
        {
            printf("Frame bueno\n");
            ++this->current_frame;	
        }
        else
            printf("Frame malo, desechando\n");
	}

    // Saco el mensaje de la cola
	this->frame_queue.pop();

    std::srand(std::time(NULL));
    double random = ( std::rand() % 100 ) / 100.0;

	//Si no se pierde el ACK
    if(random > 0.15)
    {
    	this->timeline[4] = this->clock + 0.25 + 1;
        this->ack_queue.push(this->current_frame);
        printf("ACK Push de %d\n", this->current_frame);
    }
    //Si se pierde
    else
    {
        printf("Se pierde el ACK\n");
        this->timeline[4] = std::numeric_limits<double>::infinity();
    }
		     
    // Si hay más frames en la cola
    if(!this->frame_queue.empty())
    {
        double check_time = generate_check_time();        
        this->timeline[3] = this->clock + check_time + 0.25;
    }
    else
    {
        this->timeline[3] = std::numeric_limits<double>::infinity();
  	    this->B_free = true;
    }

    if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;
}

// Llega un ACK a A
int Simulator::ack_arrival()
{
	// Reloj = LACK
  	this->clock = this->timeline[4];

    printf("Llega ACK a A\n");
  	int received_ack = this->ack_queue.front();
    printf("Leo ACK #%d, espero ACK #%d\n", received_ack, this->current_ack);
  	
  	// Si ack menor que el esperado
    if(received_ack < this->current_ack)  
    {
       	// Msj actual = 0
      	this->current_message = this->acked_messages;
      	this->timeline[5] = std::numeric_limits<double>::infinity();

      	for(auto iterator = this->message_list.begin(); iterator != message_list.end(); ++iterator)
        {
          	(*iterator).set_timeout( std::numeric_limits<double>::infinity() );
          	(*iterator).set_send(false);
            (*iterator).set_error(false);
        }
    }
  	// Si ack correcto
  	else // ( received_ack >= this->current_ack)
    {
      	// Desecho los mensajes ackeados de la lista
      	unsigned int acked_msgs = 1 + received_ack - this->current_ack;
 		printf("Se reportaron %d mensajes correctamente enviados\n", acked_msgs);

      	this->acked_messages += acked_msgs;
      
      	for(size_t message = 0; message < acked_msgs; ++message)
        {
            this->message_list.pop_front();
        }
      
      	// Muevo la ventana
        // Falta mover la ventana

      	this->current_ack = this->acked_messages+1;
      
      	// TO = TO del siguiente si se ha enviado
      	if(!this->message_list.empty() && this->message_list.front().get_send())
        {
            printf("Siguiente TO: %f [%d]\n", this->message_list.front().get_timeout(), this->message_list.front().get_id());
      		this->timeline[5] = this->message_list.front().get_timeout();
        }
      	else
          	this->timeline[5] = std::numeric_limits<double>::infinity();
    }
    
    this->ack_queue.pop();

  	if(this->A_free && !this->message_list.empty())
    {
        double conversion_time = generate_conversion_time();        
        this->timeline[1] = this->clock + conversion_time + 1;
        this->A_free = false;
    }

    this->timeline[4] = std::numeric_limits<double>::infinity();
  
    if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;

}

//  timer que se vence el timer timeout time que se vence cuando da timeout con el timer
int Simulator::timeout()
{
    printf("Ocurre un timeout\n");
	// Reloj = LACK
  	this->clock = this->timeline[5];

    // Msj actual = 0
    this->current_message = this->acked_messages;
    this->timeline[5] = std::numeric_limits<double>::infinity();
    this->timeline[4] = std::numeric_limits<double>::infinity();

    for(auto iterator = this->message_list.begin(); iterator != message_list.end(); ++iterator)
    {
        (*iterator).set_timeout( std::numeric_limits<double>::infinity() );
        (*iterator).set_send(false);
        (*iterator).set_error(false);
    }
    
    if(this->A_free && !this->message_list.empty())
    {
        double conversion_time = generate_conversion_time();        
        this->timeline[1] = this->clock + conversion_time + 1;
        this->A_free = false;
    }

    if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;    
}


// Generar tiempo de conversión
double Simulator::generate_conversion_time()
{
  	double result = 0.0;
  
  	while(result == 0.0)
    {
        std::srand(std::time(NULL));
        double r = ( std::rand() % 1000 ) / 1000.0;
        result = -2 * std::log(1-r);
    }
	
  	return result;
}

// Generar tiempo de revisión
double Simulator::generate_check_time()
{
  	std::srand(std::time(NULL));
  	double r = ( std::rand() % 1000 ) / 1000.0;
  	return sqrt(5*r+4);
}


// Generar tiempo entre arribos
double Simulator::generate_arrival_time()
{
  	double r[12];
  
	for (int index = 0; index < 12; ++index)
    {
        std::srand(std::time(NULL));
        r[index] = ( std::rand() % 1000 ) / 1000.0;    
    }
	
  	double sum = 0.0;
  	for (int index = 0; index < 12; ++index)
    	sum += r[index];
  
	sum -= 6;
  
  	return (1 * sum + 25);
}

