#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H
#include "messages/RoboCupGameControlData.h"
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


class GameController
{
	public:
		GameController(RoboCupGameControlData & storage );
		~GameController();
		void connectTo(int port = GAMECONTROLLER_PORT, int tn = 1);
		bool poll();
		void setNonBlock(bool nb);
		void SendAlive(int pnum);
	private:
		RoboCupGameControlData & game_data;
		RoboCupGameControlReturnData rd;
		int recvflag;

		int port;
		int team_number;
		int runcnt;
		struct sockaddr_in addr;
		char buffer[10024]; // maximum bytes per message
		int socket_fd;
		bool check_data_and_copy(char* bytes, int size);


};

#endif
