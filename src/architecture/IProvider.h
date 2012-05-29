#ifndef _IPROVIDER_H_
#define _IPROVIDER_H_ 1

#include "hal/thread.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/endpoint.h"


#include "tools/genFactory.h"
#include "tools/genRegistrar.h"

#include <string>

class IProvider : public EndPoint , KSystem::Thread {

    public:
        IProvider( Narukom&);

        virtual void UserInit ()=0;

        virtual std::string GetName ()=0;

    protected:

        Narukom & _com;


};

typedef GenericFactory < IProvider, std::string,IProvider* (*)(Narukom&),Narukom&>  ProviderFactory;

template<class T>
struct ProviderRegistrar {
    typedef Registrar<ProviderFactory,IProvider,std::string,T,Narukom&> Type;
};

#endif /* _IPROVIDER_H_ */
