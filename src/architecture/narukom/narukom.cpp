#include "narukom.h"
//TOBE REMOVED
//#include "pub_sub/filters/content_filter.h"
#include "pub_sub/tuple.h"
///////
Narukom::Narukom()
{
  mq = new MessageQueue("/home/vagvaz/topic_tree.xml");
  //   udp_multi = new UdpMulticastChannel();
  // mq->add_subscriber(udp_multi);
  //   mq->subscribe("global",udp_multi,2);
  mq->StartThread();

}
MessageQueue* Narukom::get_message_queue()
{
  if(mq == 0)
    mq = new MessageQueue();
  return mq;
  
  
}