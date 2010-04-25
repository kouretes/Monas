#ifndef _repProxy_h_
#define _repProxy_h_

#include <string>

#include "blackboard.h"

template < class T >
class RepProxy {

    private:

        T * theRep;
        
        RepProxy ( std::string pName, Blackboard * blk ) { }

        ~RepProxy () { } //FIXME should delete the "theProvider"?

    public:

        void ChangeProvider ( std::string newProv ) 
        
        void UpdatePointer ( Representation * rp ) { theRep = rp; }

        static void SetUrepproxy ( Blackboard * blk, Representation * rp ) {
            if ( ! blk->GetProxyInstanceByName( rp->GetRepName() ) ) 
                new Proxy(rp);
            else
                blk->GetProxyInstanceByName( rp->GetRepName() )->UpdatePointer(rp);
        }

        T * operator->() { return theRep; }
        T & operator*() { return *theRep; }

        friend class Representation;
};

#endif // _repProxy_h_
