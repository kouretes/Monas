#ifndef _myRep_h_
#define _myRep_h_

#include "../../architecture/representation.h"

class MyRep : public Representation {

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
        DEFAULT_REP_CONSTRUCTOR(MyRep)
};

#endif // _myRep_h_
