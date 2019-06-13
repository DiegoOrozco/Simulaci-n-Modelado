
#include "Simulator.h"
#include "Stats.h"

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
	, max_window_size{0}
	, waiting{0}
	, send{0}
	, sent_messages{0}
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

int Simulator::run(char* argv[], std::list<Stats> & all_stats)
{
	(void)send;
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
			case 0: ret = this->message_arrival();    break;
			case 1: ret = this->a_released();       break;
			case 2: ret = this->frame_arrival();    break;
			case 3: ret = this->b_released();       break;
			case 4: ret = this->ack_arrival();      break;
			case 5: ret = this->timeout();        break;
			default: std::cout << "Wtf" << std::endl;   break;
		}
		
		if(ret == 1)
			stop = true;

		sleep(delay); 
	}

	printf("Stop\n");

	double queue_average = 0.0;
	for(auto itr:this->prom_message)
		queue_average += itr;

	// printf("\nTamaño promedio cola A: %f\n", queue_average/this->prom_message.size());

	double permanence_total = 0.0;
	if (this->current_frame > 0)
	{
		for(auto itr:this->prom_time_message)
		{
			// printf("%f - ", itr);
			permanence_total += itr;
		}

		// printf("tam %zu\n", this->prom_time_message.size());
		// printf("\nTamaño promedio de permanencia de un mensaje: %f\n", permanence_total/this->current_frame);

	}
	else
		printf("No se finalizó correctamente ningun mensaje\n");

	// Limpiar colas
	while(!this->message_list.empty())
			this->message_list.pop_front();

	while(!this->frame_queue.empty())
	    this->frame_queue.pop_front();

	while(!this->ack_queue.empty())
	   this->ack_queue.pop_front();
	
	// 3.b
	double permanence_average = (this->current_frame == 0) ? 0.0 : permanence_total/this->current_frame;
	// 3.c
	printf("Mensajes enviados = %d de %d\n", this->sent_messages, this->acked_messages);
	double transmission_average = (double)this->sent_messages*2/(double)this->acked_messages;
	// 3.d
	double service_average = permanence_average - transmission_average;
	// 3.e
	double efficiency = transmission_average / service_average;

	Stats stats( queue_average/this->prom_message.size(), permanence_average, transmission_average, service_average, efficiency);
	all_stats.push_back(stats);

	return 0;
}

// Llega un mensaje a A (MA)
int Simulator::message_arrival()
{
	printf("Llega mensaje a A\n");

	this->clock = this->timeline[0];
	Message new_message(false, total_message);
	printf("guardo tiempo {%f}\n", this->clock);
	new_message.set_time(this->clock); 

	++this->total_message;
	++this->max_window_size;
	
	this->message_list.push_back(new_message);
	this->prom_message.push_back(this->message_list.size());

	
	//if (mensaje en la ventana)
	if(this->waiting <= 8)
	{
		if (this->A_free)
		{
			this->A_free = false;
			printf("proceso el msj que me acaba de llegar\n");
			double conversion_time = generate_conversion_time();
			this->timeline[1] = this->clock + conversion_time + 1;
		}
	}

	
	int arrival_time = generate_arrival_time();
		
	update_data("Llega mensaje a A");
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

	if(iterator == this->message_list.end())
	{
		printf("No se encontró el mensaje con id #%d\n", this->current_message);

	}

	this->clock = this->timeline[1];
	std::srand(std::time(NULL));

	double random = ( std::rand() % 100 ) / 100.0;

	if (this->waiting + 1 <= this->total_message)
		++this->waiting;
	printf("Mensajes pendientes %d de %d\n", this->waiting, total_message);

	// si el mensaje se pierde
	random = ( std::rand() % 100 ) / 100.0;
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

		Message frame(true, this->current_message, (*iterator).get_error());
		frame.set_time((*iterator).get_time());
		this->frame_queue.push_back(frame);
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
	
	(*iterator).inc_times();

	++this->current_message;

	// Si hay más mensajes en la ventana
	if(this->waiting <= 8 && this->current_message < this->total_message)
	{
		double conversion_time = generate_conversion_time();        
		this->timeline[1] = this->clock + conversion_time + 1;
	}
	else
	{
		this->timeline[1] = std::numeric_limits<double>::infinity();
		this->A_free = true;
	}
	

	update_data("Se libera A");

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
	update_data("Llega frame a B");
	 
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

	std::srand(std::time(NULL));
	double random = ( std::rand() % 100 ) / 100.0;

	// obtengo el id del primer frame de la cola
	int index_frame = (this->frame_queue.front()).get_id();
	printf("Leo frame con id #%d, espero frame #%d\n", index_frame, this->current_frame);

	// Si viene la secuencia esperada
	if(index_frame == this->current_frame)
	{
		// Obtengo los datos del mensaje enviado
		 
		// Si frame bueno
		if ( !(this->frame_queue.front()).get_error() )
		{
			++this->received_frames;
			printf("Frame bueno\n");
			this->frame_received.push_back(this->current_frame);
			if(random > 0.15)
			{		
				printf("start %f - end %f - total %f\n", (this->frame_queue.front()).get_time(), this->clock + 0.25 + 1, (this->clock + 0.25 + 1) - (this->frame_queue.front()).get_time());
				this->prom_time_message.push_back((this->clock + 0.25 + 1) - (this->frame_queue.front()).get_time());
			}
			++this->current_frame;  


		}
		else
			printf("Frame malo, desechando\n");
	}

	// Saco el mensaje de la cola


	//Si no se pierde el ACK
	if(random > 0.15)
	{
		this->timeline[4] = this->clock + 0.25 + 1;
		this->ack_queue.push_back(this->current_frame);
		printf("ACK Push de %d\n", this->current_frame);
	}
	//Si se pierde
	else
	{
		printf("Se pierde el ACK\n");
		this->timeline[4] = std::numeric_limits<double>::infinity();
	}
			 
	printf("A. id es %d \n",(this->frame_queue.front()).get_id() );
	if(!this->frame_queue.empty())
		this->frame_queue.pop_front();
	printf("D. id es %d \n",(this->frame_queue.front()).get_id() );
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

	update_data("Se libera B");
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

		printf("pendientes %d de %d\n", this->waiting, total_message);
	}
	// Si ack correcto
	else // ( received_ack >= this->current_ack)
	{

		// Desecho los mensajes ackeados de la lista
		unsigned int acked_msgs = 1 + received_ack - this->current_ack;
		printf("Se reportaron %d mensajes correctamente enviados\n", acked_msgs);

		this->acked_messages += acked_msgs;
			
		for(size_t message = 0; message < acked_msgs && !message_list.empty(); ++message)
		{
			printf("Veces enviadas %d para el mensaje %d\n", this->message_list.front().get_times(), this->message_list.front().get_id());
			this->sent_messages += this->message_list.front().get_times();
			this->message_list.pop_front();
		}
	
		// Muevo la ventana
		// Falta mover la ventana
		// this->max_window_size -= acked_msgs;
		
		--this->max_window_size;
		
		if (received_ack == this->current_ack)
			--this->waiting;
		else 
			this->waiting -= (received_ack - this->current_ack) + 1;

	
		printf("pendientes %d de %d\n", this->waiting, total_message);

		this->current_ack = this->acked_messages+1;
		this->current_message = this->acked_messages;


	
		// TO = TO del siguiente si se ha enviado
		if(!this->message_list.empty() && this->message_list.front().get_send())
		{
			printf("Siguiente TO: %f [%d]\n", this->message_list.front().get_timeout(), this->message_list.front().get_id());
			this->timeline[5] = this->message_list.front().get_timeout();
		}
		else
			this->timeline[5] = std::numeric_limits<double>::infinity();
	} 
		
	this->max_window_size = this->current_message;
	this->ack_queue.pop_front();

	if(this->A_free && !this->message_list.empty())
	{
		double conversion_time = generate_conversion_time();        
		this->timeline[1] = this->clock + conversion_time + 1;
		this->A_free = false;
	}

	this->timeline[4] = std::numeric_limits<double>::infinity();
	update_data("Llega ACK a A");

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
//  this->max_window_size = 0;
	this->max_window_size = this->current_message = this->acked_messages;

	this->waiting = this->message_list.size();
	printf("pendientes %d de %d\n", this->waiting, total_message);
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

	update_data("Ocurre un timeout");

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

void Simulator::update_data(const char* event)
{
	printf("----------------------------------------------------\n");
	std::cout << "Evento \t\t\t\t\t\t| " << event << std::endl;
	std::cout << "Reloj \t\t\t\t\t\t| " << this->clock << std::endl;
	std::cout << "Tamaño de la cola de A\t\t\t\t| " << this->message_list.size() << std::endl;
	std::cout << "Cola de mensajes de A\t\t\t\t| ";

	int total_printed = 0;
		 
	for(auto iterator = this->message_list.begin(); iterator != this->message_list.end() && total_printed <= 20; ++iterator)
	{
		if(total_printed == 0)
			printf("[");
		else if(total_printed == 8)
			printf("]");

		printf("%d - ", (*iterator).get_id()  );
			++total_printed;
	}

	if(total_printed > 0 &&  total_printed <= 7)
		printf("]");

	std::cout << std::endl;
	std::cout << "Último ACK recibido en A\t\t\t| " << this->current_ack-1 << std::endl;
	std::cout << "Último ACK enviado por B\t\t\t| " << this->current_frame << std::endl;
	std::cout << "Cantidad de frames enviados correctamente\t| " << this->current_frame << std::endl;
	std::cout << "Frames recibidos correctamente\t\t\t| ";

	int index = 0;
	if (this->frame_received.size() > 20 && this->current_frame != 0)
	{
		for (size_t index = 0; index < this->frame_received.size() - 20; ++index)
			this->frame_received.pop_front();    
	}

	for(auto itr:this->frame_received)
	{
		if(index < 20)
			printf("%d - ", itr);
		++index;

	}
	std::cout << std::endl;

	for(int i = 0; i < 6; ++i)
		printf("| %lf ", this->timeline[i]);
	printf("|\n");
	printf("----------------------------------------------------\n");
}
