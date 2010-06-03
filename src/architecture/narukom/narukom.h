#ifndef NARUKOM_H
#define NARUKOM_H

#include "pub_sub/message_queue.h"
#include <string>
/**
Narukom Class is used to combine all the different pieces of Narukom's 
framework such as message queue, catalog module

*/
class Narukom{
  public:
    Narukom();
    Narukom(std::string configurationFile);
    MessageQueue* get_message_queue();
  private:
   MessageQueue* mq;
    //Catalog* cat;

};



#endif