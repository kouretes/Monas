#ifndef _headAnglePublisher_h_
#define _headAnglePublisher_h_ 

#include "architecture/IActivity.h"


#include "hal/robot/generic_nao/headAngleSensor.h"

#include "architecture/narukom/narukom.h"
#include "messages/ledMsg.pb.h"

class HeadAnglePublisher : public Publisher, public IActivity {    
    public: 


        HeadAnglePublisher() : Publisher("Pub") {}

        void  UserInit () {

            std::cout<<"HeadAnglePublisher: Initializing"<<std::endl;

            _com->get_message_queue()->add_publisher(this);
        }


        
        
        int Execute () ;

    private:

        HeadAngleSensor s;

        LedMsg msg;


        

};


#endif // _headAnglePublisher_h_
