#ifndef _myProvider2_h_
#define _myProvider2_h_

#include "architecture/provider.h"

#include "myRep2.h"


//#include "../../architecture/abFactory.h"

//include for other representations? script?

//#STATIC_UPDATE { ... }
//#DYNAMIC_UPDATE("GetTestX")


class MyProvider2 : public Provider, public MyRep2 {
    
    public: 

        DEFAULT_PROV_CONSTRUCTOR(MyProvider2,MyRep2)

        REQUIREMENTS_START
        REQUIRE(MyRep1)
        REQUIREMENTS_END        
        
        void UpdatePointers () {

            theMyRep1 = REP_INSTANCE(MyRep2);

            std::cout<<"TheMyRep2 addr is: "<<theMyRep1<<std::endl;
        }
        
        
        void Update (); 
    private:

        MyRep2 * theMyRep1;
        

};


#endif // _myProvider2_h_
