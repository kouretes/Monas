#include "gamecontroller.h"
#include <cstring>
#include <iostream>

using std::cout;
using std::endl;
GameController::GameController(RoboCupGameControlData* game_data,bool* received_data,Mutex* mx,int port,int team_number): game_data(game_data),received_data(received_data),mx(mx),port(port),team_number(team_number) {
    mx->lock();
//    initialize_data(game_data);
    cout << "Initialize gm" << endl;
    *received_data = false;
    mx->unlock();
    current_data = new RoboCupGameControlData;
    cout << "socket create" << endl;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0); //socket creation
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    //bind
    if (  (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr))) != 0 )
        std::cerr << "bind" << endl;
    FD_ZERO(&rset);
    FD_SET(socket_fd, &rset);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    cout << " end of gm init" << endl;
}



int GameController::Execute()
{
    //  cout << " gm run " << endl;
    static int i = 0;
    FD_SET(socket_fd,&rset);
    int tmpfd;
    timeout.tv_sec = 0  ;
    timeout.tv_usec = 30000000;
    //  int number_of_bites = 0;
    tmpfd = select(socket_fd+1,&rset,NULL,NULL,&timeout);
    int bytes;
    socklen_t addr_len=sizeof(addr);
// 		cout << "after select !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    if (tmpfd > 0) {
// 			cout << "tmpfd descriptor ============================================" << endl;
        if (FD_ISSET(socket_fd,&rset))
        {
//             cout << "Clear READSET " << endl;
            bytes =  recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);
            //bytes = read(sd,buffer,sizeof(buffer));
// 						cout << "reading bytes-----------------------------" << endl;
            if (check_data_and_copy(buffer,bytes))
            {
//                 cout << "True Checking Data " << endl;
                mx->lock();
                RoboCupGameControlData* tmp_ptr = game_data;
                game_data = current_data;
                current_data = tmp_ptr;
                *received_data = true;
//                 cout << "ptr " << (int)received_data << endl;
                mx->unlock();
            }
            else
            {
//                 cout << "check data failed " << endl;
//                 cout << "VR: " <<  (int)current_data->version << endl;
            }
            bzero(&buffer,sizeof(buffer));
// 						cout << "Closing if read set clear ()()()()()()" << endl;
        }
    }
    else {

        if (i++ % 20==0)
            cout << "Timeout occured! no messages received from gamecontroller" << endl;
    }
    //usleep(499000);
    return 0;
}
bool GameController::check_data_and_copy(char* bytes, int size)
{

    if ( (strncmp(bytes,GAMECONTROLLER_STRUCT_HEADER,4) == 0 ) && (size == sizeof(RoboCupGameControlData) ))
    {
        //cout << "Valid GameController packet" << endl;
        memcpy(current_data,bytes,size);
        if (current_data->teams[0].teamNumber == team_number || current_data->teams[1].teamNumber== team_number)
            return true;
//         cout << "Packet is not for our team " << team_number << "teams " <<  (int)current_data->teams[0].teamNumber << " " << (int)current_data->teams[1].teamNumber << endl;
    }
    else
    {
        cout << "Size: " << size << " sizeof " << sizeof(RoboCupGameControlData) << endl;
        cout << "Header " << GAMECONTROLLER_STRUCT_HEADER << bytes[0] << bytes[1] << bytes[2] << bytes[3] << endl;
        return false;
    }
//     return true;
}


GameController::~GameController() {

    delete current_data;
    mx->unlock();
    cout << " Shutting down gamecontroller " << endl;
    close(socket_fd);
}
