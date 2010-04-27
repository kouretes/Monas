#include "narukom.h"


MessageQueue* Narukom::get_message_queue()
{
    if(mq == 0)
        mq = new MessageQueue();
    return mq;

}
