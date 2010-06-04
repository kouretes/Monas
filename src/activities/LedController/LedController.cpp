#include "LedController.h"

namespace {
    ActivityRegistrar<LedController>::Type temp("LedController");
}


int LedController::Execute () { 
    _blk->process_messages();
    msg = &(_blk->read<LedMsg>("LedMsg","Pub"));

    std::cout<<"Updating LedController "<<std::endl; 
    if ( msg->left() )
        _eLed.AllOff();
    else
        _eLed.AllOn();
}


