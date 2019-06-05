#include "Message.h"

Message::Message()
	: message_arrived{false}
	, error {false}
	, timeout {0}

{
}

Message::~Message()
{
}

void Message::set_arrived(bool value)
{
	this->message_arrived = value;
}

void Message::set_error(bool value)
{
	this->error = value;
}
  
void Message::set_timeout(unsigned int value)
{
  	this->timeout = value;
}
