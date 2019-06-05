#include <iostream>

#include "Simulator.h"

int main(int argc, char* argv[])
{
	int ret = 1;
	if(argc >= 3)
	{
		Simulator simulator;
		ret = simulator.run(argv);
	}
	else
	{
		std::cout << "Usage <Total time> <timeout>" << std::endl;
	}

  	return ret;
}