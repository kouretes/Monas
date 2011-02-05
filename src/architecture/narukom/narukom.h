#ifndef NARUKOM_H
#define NARUKOM_H

#include <string>
/**
Narukom Class is used to combine all the different pieces of Narukom's
framework such as message queue, catalog module

*/
class MessageQueue;
class Narukom{
  public:
    Narukom();
    MessageQueue* get_message_queue();
  private:
   MessageQueue* mq;
    //Catalog* cat;

};



#endif
