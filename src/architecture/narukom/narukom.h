#ifndef NARUKOM_H
#define NARUKOM_H
#include "message_queue.h"
#include "udp_multicast_channel.h"
class Narukom{
private:
    MessageQueue* mq;
    UdpMulticastChannel* udp_multi;
public:
    Narukom(){ mq = new MessageQueue;}
    MessageQueue* get_message_queue();
    ~Narukom(){delete mq;}
};
#endif
