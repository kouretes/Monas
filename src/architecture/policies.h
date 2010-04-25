#ifndef _Policies_h_
#define _Policies_h_ 1

#include <string>

class AbErrPolicy {

    public:

        virtual void Error ( const std::string& ) =0;

};



class RuntimeErrPolicy : public AbErrPolicy {

    public:

        void Error( const std::string & er ) {
            throw std::runtime_error( "Error: "+er );
        }

};

#endif // _Policies_h_
