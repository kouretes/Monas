#include <iostream>
#include "src/message_queue.h"
#include "src/publisher.h"
#include "src/subscriber.h"
#include <string>
#include <vector>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "pinger.h"
#include "ponger.h"
#include "echo.h"
#include "scorekeeper.h"
#include "pingpong.pb.h"
#include "src/blackboard.h"
#include "Basic.pb.h"
#include "trafficer.h"
#include "src/networkpubsub.h"
#include "image.pb.h"
#include "src/network_message.h"
#include "src/network_message_buffer.h"
#include "src/udp_multicast_channel.h"
#include "ratio.h"
#include "slow.h"
#include "fast.h"

using namespace std;
using namespace google::protobuf;
int main(int argc, char **argv) {
   MessageQueue mq;
	 Slow s;
	 Fast f;
	 Ratio r;
	 cout << "Add Publishers " << endl;
	 mq.add_publisher(&s);
	 mq.add_publisher(&f);
	 cout << "Add Subscribers" << endl;
	 mq.add_subscriber((r.get_blackboard()));
	 cout <<  "Subscibing.." << endl;
	 mq.subscribe("global",r.get_blackboard(),2);
	 cout << "start threads" << endl;
	 mq.start();
	 s.start();
	 f.start();
	 r.start();
	 cout << "Wait for joing" << endl;
	 s.join();
	 f.join();
	 r.join();
	 
	 mq.join();
	 cout << "Exiting.. " << endl;
	 return 0;
return 0;
}
