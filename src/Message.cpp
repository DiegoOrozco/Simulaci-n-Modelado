#include "Message.h"

#include <cstdio>

Message::Message()
	: message_send{false}
	, error {false}
	, timeout {0}
	, id{-1}
	, times_sent{0}

{
}

Message::Message(bool send_message, int new_id)
	: message_send{send_message}
	, error{false}
	, timeout {0}
	, id{new_id}
	, time_arrival{0}
	, times_sent{0}
{
}

Message::Message(bool send_message, int new_id, bool error)
	: message_send{send_message}
	, error{error}
	, timeout {0}
	, id{new_id}
	, time_arrival{0}
	, times_sent{0}
{
}

Message::~Message()
{
}

void Message::set_send(bool value)
{
	this->message_send = value;
}

void Message::set_error(bool value)
{
	this->error = value;
}
  
void Message::set_timeout(double value)
{
  	this->timeout = value;
}

void Message::set_id(int value)
{
	this->id = value;
}

void Message::set_time(int value)
{
	this->time_arrival = value;
}

void Message::inc_times()
{
	printf("Veces enviadas para el mensaje %d: %d\n", this->id, this->times_sent);
	this->times_sent += 1;
}
