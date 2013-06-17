#include "gamecontroller.h"
#include <cstring>
#include <iostream>
#include <errno.h>
#include "core/include/Logger.hpp"
#include "tools/toString.h"

using std::cout;
using std::endl;



//, bool* received_data, Mutex* mx, int port, int team_number) :
//	game_data(game_data), received_data(received_data), mx(mx), port(port), team_number(team_number)

GameController::GameController(RoboCupGameControlData & storage ) : game_data(storage)
{
	runcnt = 0;
	recvflag = 0;
	memcpy(rd.header, GAMECONTROLLER_RETURN_STRUCT_HEADER, strlen(GAMECONTROLLER_RETURN_STRUCT_HEADER));
	rd.message = (GAMECONTROLLER_RETURN_MSG_ALIVE);
	rd.version = (GAMECONTROLLER_RETURN_STRUCT_VERSION);
}
void GameController::connectTo(int port, int tn )
{
	team_number = tn;
	//Initialize_data
	LogEntry(LogLevel::Info,"GameController")<< "Initialize GameController" << std::endl;
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0); //socket creation

	if (socket_fd == -1)
	{
		LogEntry(LogLevel::FatalError,"GameController")<< "Cannot create Socket " << std::endl;
	}

	{
		int reuse = 1;

		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
		{
			LogEntry(LogLevel::FatalError,"GameController")<< "Setting SO_REUSEADDR error " << std::endl;
			close(socket_fd);
		}
	}

	{
		int reuse = 1;

		if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, (char *) &reuse, sizeof(reuse)) < 0)
		{
			LogEntry(LogLevel::FatalError,"GameController") << "Setting SO_BROADCAST error " << std::endl;
			close(socket_fd);
		}
	}

	struct timeval timeout;

	timeout.tv_sec = 1;

	timeout.tv_usec = 0;

	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
	{
		LogEntry(LogLevel::FatalError,"GameController") << "Setting SO_RCVTIMEO error " << std::endl;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	//binding
	if ((bind(socket_fd, (struct sockaddr*) &addr, sizeof(addr))) != 0)
	{
		LogEntry(LogLevel::FatalError,"GameController") << "Cannot Bind  " << std::endl;
	}

	LogEntry(LogLevel::Info,"GameController") << " Game controller Initialized" << std::endl;
	addr.sin_addr.s_addr = INADDR_BROADCAST;
}


void GameController::setNonBlock(bool nb)
{
	if(nb == true)
		recvflag = MSG_DONTWAIT;
	else
		recvflag = 0;
}
bool GameController::poll()
{
	int bytes;
	bool update = false;

	if ((bytes = recv(socket_fd, buffer, sizeof(buffer), recvflag)) > 0)
	{
		update = check_data_and_copy(buffer, bytes);
	}
	else
	{
		if (runcnt++ % 5000 == 0)
		{
			LogEntry(LogLevel::Error,"GameController") << "Is Game Controller Running? Cant Listen"  << std::endl;
			runcnt = 1;
		}
	}

	return update;
}
#include <errno.h>

void GameController::SendAlive(int pnum)
{
	rd.team = (team_number);
	rd.player = (pnum);
	sendto(socket_fd, &rd, sizeof(rd), recvflag, (struct sockaddr*)  &addr, sizeof(addr));
}
bool GameController::check_data_and_copy(char* bytes, int size)
{
	if ((strncmp(bytes, GAMECONTROLLER_STRUCT_HEADER, 4) == 0) && (size == sizeof(RoboCupGameControlData)))
	{
		// Valid GameController packet
		RoboCupGameControlData *c = (RoboCupGameControlData *)bytes;

		if (c->teams[0].teamNumber == team_number || c->teams[1].teamNumber == team_number)
		{
			//Packet is  for our team
			memcpy(&game_data, bytes, size);
			return true;
		}
		else
		{
		}
	}
	else
	{
		//Header mismatch
		return false;
	}

	return false;
}

GameController::~GameController()
{
	LogEntry(LogLevel::Info,"GameController") << "Shutting down gamecontroller" << std::endl;
	close(socket_fd);
}
