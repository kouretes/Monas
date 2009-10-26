#ifndef _myProvider_h_
#define _myProvider_h_

#include "../../architecture/provider.h"

#include "myRep.h"


#include "../test1/myRep1.h"

//#include "../../architecture/abFactory.h"

//include for other representations? script?

//#STATIC_UPDATE { ... }
//#DYNAMIC_UPDATE("GetTestX")


class MyProvider : public Provider, public MyRep {
    
    public: 

        DEFAULT_PROV_CONSTRUCTOR(MyProvider,MyRep)

        REQUIREMENTS_START
        REQUIRE(MyRep1)
        REQUIREMENTS_END

        PROV_OBJECT(MyProvider)

        void UpdatePointers () {

            theMyRep1 = REP_INSTANCE(MyRep1);

            std::cout<<"TheMyRep1 addr is: "<<theMyRep1<<std::endl;
        }
        
        
        void Update () { 
            std::cout<<"Updating myProvider"<<std::endl; 
            usleep(200000);
            //SetTheX( 2 );
            //std::cout<<"The Asdf is :"<<theMyRep1->GetAsdf()<<std::endl;
        }

    private:

        MyRep1 * theMyRep1;
        

};

DECLARE_PROVIDER(MyProvider);

#endif // _myProvider_h_
