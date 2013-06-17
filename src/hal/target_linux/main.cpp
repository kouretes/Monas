#include <iostream>

#include <string>
#include <signal.h>

#include "core/architecture/talws.h"
#include "core/architecture/configurator/Configurator.hpp"

Talws *tal;
volatile bool  running;
void
termination_handler (int signum)
{
	std::cout<<"Signaled, terminating"<<std::endl;
	running=false;

}

int main ()
{
	running=false;
	Configurator::Instance().initConfigurator("config","","");
	std::cout << "Main: Constructing Talws" << std::endl;
	tal= new Talws();
	std::cout << "Main: Starting" << std::endl;
	tal->Start();
	std::cout << "Main: Done!" << std::endl;

	running=true;
	signal (SIGINT, termination_handler);
	while(running)
		usleep(1000);

	std::cout << "Main: Stoping" << std::endl;
	tal->Stop();
	std::cout << "Main: Exiting" << std::endl;
	return 0;
}


