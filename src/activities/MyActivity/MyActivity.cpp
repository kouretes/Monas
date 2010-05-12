#include "MyActivity.h"

namespace {
    ActivityRegistrar<MyActivity>::Type temp("MyActivity");
}


int MyActivity::Execute () { 
    //std::cout<<"Updating myActivity"<<std::endl; 
    usleep(2000);
}


