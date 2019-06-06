#include <iostream>

#include "Simulator.h"

int main(int argc, char* argv[])
{
	int ret = 1;
	if(argc >= 4)
	{
		Simulator simulator;
		ret = simulator.run(argv);
	}
	else
	{
		std::cout << "Usage <Total time> <timeout> <delay>" << std::endl;
	}

  	return ret;
}