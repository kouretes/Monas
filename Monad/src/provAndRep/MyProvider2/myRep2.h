#ifndef _myRep2_h_
#define _myRep2_h_

#include "architecture/representation.h"

#include <iostream>

class MyRep2 : public Representation {

    private:
        int myTestx;
        bool myTestBoolean;

    protected:
        virtual void SetTestX ( int x ) { myTestx = x; }
        virtual void SetTestBool ( bool bl ) { myTestBoolean = bl; }
        virtual void SetTheX ( int x ) { myTestx = x; }

    public:
        virtual int GetTestX () { return myTestx; }
        bool GetTestBool () { return myTestBoolean; }

        //MyRep ( Blackboard * blk ) : Representation( "MyRep", blk ) { ; }
        DEFAULT_REP_CONSTRUCTOR(MyRep2)
};

#endif // _myRep2_h_
