#ifndef _myProvider1_h_
#define _myProvider1_h_

#include "architecture/provider.h"

#include "myRep1.h"


#include "../MyProvider/myRep.h"


class MyProvider1 :  public Provider, public MyRep1  {
    
    public: 

        DEFAULT_PROV_CONSTRUCTOR(MyProvider1,MyRep1)

        REQUIREMENTS_START
        REQUIREMENTS_END

        virtual ~MyProvider1() { ; }


        void UpdatePointers () { ; }
        
        
        void Update ();        

};

//DECLARE_PROVIDER(MyProvider1);

#endif // _myProvider1_h_
