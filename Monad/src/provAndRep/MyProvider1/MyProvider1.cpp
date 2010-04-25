#include "MyProvider1.h"


namespace {
    ProviderRegistrar<MyProvider1>::Type temp("MyProvider1");
}

void MyProvider1::Update () { 
    std::cout<<"Updating myProvider1"<<std::endl;
    usleep(5000);
    //SetAsdf ( 0.19 );
}
