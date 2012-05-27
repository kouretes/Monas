#ifndef _IPROVIDER_H_
#define _IPROVIDER_H_ 1

#include "hal/thread.h"

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "tools/genFactory.h"
#include "tools/genRegistrar.h"

#include <string>

class IProvider : public KSystem::Thread {

    public:

        void Initialize ( Narukom*, Blackboard* );

        virtual void UserInit ()=0;

        virtual std::string GetName ()=0;

    protected:

        Narukom* _com;

        Blackboard* _blk;

};

typedef GenericFactory < IProvider, std::string >  ProviderFactory;

template<class T>
struct ProviderRegistrar {
    typedef Registrar<ProviderFactory,IProvider,std::string,T> Type;
};

#endif /* _IPROVIDER_H_ */
