#include <iostream>

#include "Simulator.h"


int main(int argc, char* argv[])
{
	int ret = 1;
	if(argc >= 5)
	{
		int times = atoi(argv[4]);
		for (int index = 0; index < times; ++index)
		{
			Simulator simulator;
			ret = simulator.run(argv);
			std::cout << "-------------------\n";

		}
	}
	else
	{
		std::cout << "Usage <Total time> <timeout> <delay> <times>" << std::endl;
	}

  	return ret;
}