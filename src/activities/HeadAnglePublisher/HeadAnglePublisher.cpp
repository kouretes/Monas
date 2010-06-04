#include "HeadAnglePublisher.h"

namespace {
    ActivityRegistrar<HeadAnglePublisher>::Type temp("HeadAnglePublisher");
}


int HeadAnglePublisher::Execute () { 
    std::cout<<"Updating HeadAnglePublisher"<<s.GetHeadAngle()<<std::endl; 
    msg.set_left( s.GetHeadAngle()>0?true:false );
    publish(&msg,"motion");
    usleep(200000);
}


