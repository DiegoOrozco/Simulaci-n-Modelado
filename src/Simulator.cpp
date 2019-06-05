
#include "Simulator.h"
#include <iostream>
#include <ctime>
#include <limits>

Simulator::Simulator()
	: max_time{0.0}
	, current_message{0}
	, current_frame{0}
	, A_free{true}
	, B_free{true}
	, clock{0.0}
	, total_message{0}
	, time_out{0.0}
	, current_ack{0}
    , acked_messages{0}

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

// Llega un mensaje a A (MA)
int Simulator::message_arrival()
{
	this->clock = this->timeline[0];
	Message new_message(false, total_message); 
  	++this->total_message;
  	this->message_list.push_back(new_message);
  	
  	if (this->A_free)
    {
		this->A_free = false;
      	double conversion_time = generate_conversion_time();
      	this->timeline[1] = this->clock + conversion_time + 1;
		this->A_free = true;
    }
  
	int arrival_time = generate_arrival_time();
  	
	// MA = T. Arribos
	this->timeline[0] = arrival_time;

  	if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;
}

// Se libera A
int Simulator::a_released()
{
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
        // FB = inf
        this->timeline[2] = std::numeric_limits<double>::infinity();
    }
    // Si el mensaje no se pierde
    else
    {
        // Si el mensaje viene con error
        if (random >= 0.9)
            (*iterator).set_error(true);

        // FB = reloj + 1
        this->timeline[2] = this->clock + 1;
      	frame_queue.push(this->current_message);
    }

	// Siempre asumo que el mensaje se envia
    (*iterator).set_send(true);
  
  	// Guardo el TO en el vector
    (*iterator).set_timeout(this->clock + this->time_out);
	
  	// Si TO es infinito
    if(this->timeline[5] == std::numeric_limits<double>::infinity() )
        this->timeline[5] = this->clock + this->time_out;
	
  	// Si hay más mensajes en la ventana
    if(this->current_message < this->total_message)
    {
        double conversion_time = generate_conversion_time();        
        this->timeline[1] = this->clock + conversion_time + 1;
    }
    else
    {
        this->timeline[1] = std::numeric_limits<double>::infinity();
    }
  	
  	++this->current_message;
  
  	// Si reloj > MAX
    if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;
}

// Llega un frame a B
int Simulator::frame_arrival()
{
	// Reloj = FB
	this->clock = this->timeline[2];
  	
	// Si proceso libre 
  	if (this->B_free)
    {
		this->B_free = false;
    	// Generar T. Revisión
		double check_time = generate_check_time();
    	
      	//LB = Reloj + T. Revisión + 0.25
     	this->timeline[1] = this->clock + check_time + 0.25;      
      	this->B_free = true;
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
  
	// Reloj = LB
	this->clock = this->timeline[3];
	
  	// obtengo el id del primer frame de la cola
    int index_frame = this->frame_queue.front();
	
  	// si no son iguales, el frame no ser perdió
	if( index_frame != this->current_frame)
    	// Desechar mensaje
		this->frame_queue.pop();
  	// si no se perdió
  	else
    {
      auto iterator = this->message_list.begin();

      // obtengo el los datos del mensaje enviado
      for(; iterator != this->message_list.end(); ++iterator)
          if((*iterator).get_id() == index_frame)
              break;
      
      // si el frame tuvo errores
      if ( (*iterator).get_error() )
    	// Desechar mensaje
      	this->frame_queue.pop();
      // Si frame bueno
      else
	    // ++Frame actual
        ++this->current_frame;	
	}

    std::srand(std::time(NULL));
    double random = ( std::rand() % 100 ) / 100.0;

	//Si no se pierde el ACK
    if(random > 0.15)
    {
    	// LACK = Reloj + 0.25 + 1
    	this->timeline[4] = this->clock + 0.25 + 1;
    }
    //Si se pierde
    else
    {
	    //LACK = ∞
        this->timeline[4] = std::numeric_limits<double>::infinity();
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
  	int received_ack = this->ack_queue.front();
  	
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
        }
      
    }
  	// Si ack correcto
  	else if( received_ack >= this->current_ack)
    {
      	// Desecho los mensajes ackeados de la lista
      	int acked_msgs = 1 + received_ack - this->current_ack;
 		
      	this->acked_messages += acked_msgs;
      
      	for(size_t message = 0; message < acked_msgs; ++message)
      		this->message_list.pop_front();
      
      	// Muevo la ventana
      	this->current_ack = received_ack;
      
      	// TO = TO del siguiente
      	if(!this->message_list.empty())
      		this->timeline[5] = this->message_list.front().get_timeout();
      	else
          	this->timeline[5] = std::numeric_limits<double>::infinity();
    }
  
  	if(this->A_free || !this->message_list.empty())
    {
        double conversion_time = generate_conversion_time();        
        this->timeline[1] = this->clock + conversion_time + 1;
    }
  
    if ( this->clock >= this->max_time )
      	return 1;
	else
		return 0;

}

//  timer que se vence el timer timeout time que se vence cuando da timeout con el timer
int Simulator::timeout()
{
	// Reloj = LACK
  	this->clock = this->timeline[5];

    // Msj actual = 0
    this->current_message = this->acked_messages;
    this->timeline[5] = std::numeric_limits<double>::infinity();

    for(auto iterator = this->message_list.begin(); iterator != message_list.end(); ++iterator)
    {
        (*iterator).set_timeout( std::numeric_limits<double>::infinity() );
        (*iterator).set_send(false);
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

