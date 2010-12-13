#include "gamecontroller.h"
#include <cstring>
#include <iostream>
#include <errno.h>
#include "tools/logger.h"
#include "tools/toString.h"

using std::cout;
using std::endl;
GameController::GameController(RoboCupGameControlData* game_data, bool* received_data, Mutex* mx, int port, int team_number) :
	game_data(game_data), received_data(received_data), mx(mx), port(port), team_number(team_number)
{
	mx->lock();
	//Initialize_data
	Logger::Instance().WriteMsg("GameController", "Initialize GameController", Logger::Info);
	*received_data = false;
	mx->unlock();

	current_data = new RoboCupGameControlData;

	cout << "socket create" << endl;
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0); //socket creation

	if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		Logger::Instance().WriteMsg("GameController", "Cannot create Socket ", Logger::FatalError);

	}
	{
		int reuse = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
		{
			Logger::Instance().WriteMsg("GameController", "Setting SO_REUSEADDR error ", Logger::FatalError);
			close(socket_fd);
		}
	}

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
	{
		Logger::Instance().WriteMsg("GameController", "Setting SO_RCVTIMEO error ", Logger::FatalError);
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	//binding
	if ((bind(socket_fd, (struct sockaddr*) &addr, sizeof(addr))) != 0)
	{
		Logger::Instance().WriteMsg("GameController", "Cannot Bind  ", Logger::FatalError);
	}
	Logger::Instance().WriteMsg("GameController", " Game controller Initialized", Logger::Info);
}

int GameController::Execute()
{
	static int i = 0;
	int bytes;
	if ((bytes = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0)
	{
		if (check_data_and_copy(buffer, bytes))
		{
			mx->lock();
			RoboCupGameControlData* tmp_ptr = game_data;
			game_data = current_data;
			current_data = tmp_ptr;
			*received_data = true;
			mx->unlock();
		} else
		{
			//                 cout << "check data failed " << endl;
			//                 cout << "VR: " <<  (int)current_data->version << endl;
		}
	} else
	{
		if (i++ % 20 == 0)
			Logger::Instance().WriteMsg("GameController", "Is Game Controller Running? Cant Listen", Logger::Error);
	}

	return 0;
}
bool GameController::check_data_and_copy(char* bytes, int size)
{

	if ((strncmp(bytes, GAMECONTROLLER_STRUCT_HEADER, 4) == 0) && (size == sizeof(RoboCupGameControlData)))
	{
		// Valid GameController packet
		memcpy(current_data, bytes, size);
		if (current_data->teams[0].teamNumber == team_number || current_data->teams[1].teamNumber == team_number)
		{
			//Packet is  for our team
			return true;
		} else
		{
			//Not  for our team "
		}
	} else
	{
		//Header mismatch
		return false;
	}
	return false;
}

GameController::~GameController()
{
	delete current_data;
	mx->unlock();
	Logger::Instance().WriteMsg("GameController", "Shutting down gamecontroller", Logger::Info);
	close(socket_fd);
}
