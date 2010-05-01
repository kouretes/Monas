#ifndef _ledController_h_
#define _ledController_h_ 

#include "architecture/IActivity.h"

#include "messages/ledMsg.pb.h"

#include "hal/robot/generic_nao/earLed.h"

class LedController : public IActivity {    
    public: 

        void  UserInit () {

            std::cout<<"LedController: Initializing"<<std::endl;
            _com->get_message_queue()->add_subscriber(_blk);
            _com->get_message_queue()->subscribe("global",_blk,2);
        }
        
        
        int Execute () ;

    private:

        LedMsg* msg;

        EarLed _eLed;

        

};


#endif // _ledController_h_
