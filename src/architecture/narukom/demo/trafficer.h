/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef TRAFFICER_H
#define TRAFFICER_H

#include "hal/Thread.h"
#include "src/publisher.h"


class Trafficer : public Thread, public Publisher
{
  public:
    
    Trafficer(bool running = false) : Thread(running),Publisher("Trafficer") {
      std::string shosts[] = {"localhost","robot_1", "robot_2"  };
    std::string stopic[] = {"global","motion", "score"  };
   hosts = new std::vector<std::string>(shosts, shosts + 3);
   topic = new std::vector<std::string>(stopic,stopic + 3);
    int tm[] = {100,200,300,400,500};
    timeouts = new std::vector<int>(tm,tm + 5);
    
    }
    virtual void run();
    
    
    virtual void publish(google::protobuf::Message* msg);
    
  private:
    
    std::vector<std::string>* hosts;
    std::vector<std::string>* topic ;
    
    std::vector<int>* timeouts;
    
};

#endif // TRAFFICER_H
