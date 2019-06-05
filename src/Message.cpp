#include "Message.h"

Message::Message()
{
}

Message::~Message()
{
}

void Message::set_arrived(bool value)
{
	this.message_arrived = value;
}

void Message::set_error(bool value)
{
	this.error = value;
}
  
void Message::set_timeout(unsigned int value)
{
  	this.timeout = value;
}
