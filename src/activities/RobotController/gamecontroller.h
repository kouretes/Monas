#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H
#include "messages/RoboCupGameControlData.h"
#include "hal/thread.h"
#include "architecture/narukom/system/Mutex.h"
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
class GameController : public Thread{
public:
    GameController(RoboCupGameControlData* game_data,bool* received_data,Mutex* mx,int port = GAMECONTROLLER_PORT,int team_number = 1);
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
    fd_set rset;
    bool check_data_and_copy(char* bytes, int size);


};

#endif
