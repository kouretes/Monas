#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H
#include "messages/RoboCupGameControlData.h"
#include "hal/thread.h"
#include "hal/mutex.h"
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include "tools/logger.h"
#include "tools/toString.h"

class GameController: public Thread {
	public:
		GameController(RoboCupGameControlData* game_data, bool* received_data, Mutex* mx, int port = GAMECONTROLLER_PORT, int team_number = 1);
		~GameController();
		int Execute();

	private:
		RoboCupGameControlData* game_data;
		RoboCupGameControlData* current_data;
		bool* received_data;


		Mutex* mx;

		int port;
		int team_number;
		struct sockaddr_in addr;
		char buffer[10024]; // maximum bytes per message
		int socket_fd;
		struct timeval timeout;


		bool check_data_and_copy(char* bytes, int size);

};

#endif
