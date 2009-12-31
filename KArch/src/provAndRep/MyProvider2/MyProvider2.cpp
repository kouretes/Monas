#include "MyProvider2.h"

namespace {
    ProviderRegistrar<MyProvider2>::Type temp("MyProvider2");
}

void MyProvider2::Update () { 
    std::cout<<"Updating myProvider2"<<std::endl; 
    //SetTheX( 2 );
    //std::cout<<"The Asdf is :"<<theMyRep1->GetAsdf()<<std::endl;
}

