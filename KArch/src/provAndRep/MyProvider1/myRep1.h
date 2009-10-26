#ifndef _myRep1_h_
#define _myRep1_h_

#include "architecture/representation.h"
#include <iostream>

class MyRep1 : public Representation {

    private:

        float asdf;
        long ppp;

    protected:

        void SetAsdf ( float x ) { 
            std::cout<<"Setting Asdf"<<std::endl; 
            asdf = x ; 
        }

    public:

        float GetAsdf () { return asdf; }

        MyRep1 ( Blackboard * blk ) : Representation( "MyRep1" , blk ) { 
            std::cout<<"MyRep1 "<<this<<std::endl; 
        }

};

#endif // _myRep1_h_        
