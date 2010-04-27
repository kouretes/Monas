#include <iostream>
#include "message_queue.h"
#include "publisher.h"
#include "subscriber.h"
#include <string>
#include <vector>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "pinger.h"
#include "ponger.h"
#include "echo.h"
#include "scorekeeper.h"
#include "pingpong.pb.h"
/*#include "src/blackboard.h"
#include "Basic.pb.h"
#include "trafficer.h"
#include "src/networkpubsub.h"
#include "image.pb.h"
#include "src/network_message.h"
#include "src/network_message_buffer.h"
#include "src/udp_multicast_channel.h"*/
#include "../narukom.h"
using namespace std;
int main(int argc, char **argv) {
   
//	    message queue code
     Narukom n;
    MessageQueue* mq = n.get_message_queue();
    std::cout << "Pinger Application" << std::endl;
//     std::stringstream s;
    
    mq->start();
//     Pinger* ping = new Pinger(false);
   // mq->StartThread(mq);
//   ping pong code
  Pinger* ping = new Pinger(false);
  Ponger* pong = new Ponger(false);
  ScoreKeeper* sc = new ScoreKeeper(false);
  Echo* echo = new Echo;
// ping pong code
cout << "Adding publishers to message queue " << endl;
  mq->add_publisher(ping);
  mq->add_publisher(pong);

  cout << "Adding subscribers to message queue" << endl;
  mq->add_subscriber(sc);
  mq->add_subscriber ( echo );
  mq->add_subscriber ( pong );
  mq->add_subscriber ( ping );
  
	cout  << "Subscibing to topics" << endl;
	mq->subscribe ( "score",sc,1 );
  mq->subscribe ( "global",echo,2 );
  mq->subscribe ( "motion",ping,0 );
  mq->subscribe ( "motion",pong,0 );
    
	cout << "Start Threads for all publishers and Subscribers" << endl;
	ping->start();
pong->start();
	echo->start();
	sc->start();	
	ping->start_service();
//   */
/*
ping->start();
 echo->start();
sc->start();
pong->start();*/
//sleep(1);
//  ping->start_service();
// udp->start();
//sleep(3);
ping->join();
pong->join();
/* ping pong Code    
    cout << "Before join" << endl;
  
		ping->join();
		cout << "after ping join() " << endl;
		pong->join();	
		cout << "after pong join() " << endl;
*/
		
    
  
    
    
//     boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time());
    cout << "After join " << endl;
    return 0;
}
