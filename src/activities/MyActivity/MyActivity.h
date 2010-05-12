#ifndef _myActivity_h_
#define _myActivity_h_

#include "architecture/IActivity.h"


class MyActivity : public IActivity {//, public MyRep {
    
    public: 

        void  UserInit () {

            std::cout<<"MyActivity: Initializing"<<std::endl;
        }

        std::string GetName () {
            return "MyActivity";
        }

      
        
        
        int Execute () ;

    private:

        

};


#endif // _myActivity_h_
