#include "MyProvider.h"

namespace {
    ProviderRegistrar<MyProvider>::Type temp("MyProvider");
}


void MyProvider::Update () { 
    std::cout<<"Updating myProvider"<<std::endl; 
    usleep(200000);
    //SetTheX( 2 );
    //std::cout<<"The Asdf is :"<<theMyRep1->GetAsdf()<<std::endl;
}


