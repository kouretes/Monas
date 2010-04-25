#ifndef _provider_h_
#define _provider_h_ 1

#include <vector>
#include <map>

#include <iostream>

#include "architecture/blackboard.h"
#include "architecture/genFactory.h"
#include "architecture/genRegistrar.h"

class Provider {

    public:

        Provider( const std::string pName, Blackboard * const BlackboardPtr ) 
          : providerName ( pName ),
            BlackboardPtr ( BlackboardPtr ) { 
                BlackboardPtr->Providers[providerName] = this; //FIXME
        }

        virtual ~Provider () { 
            BlackboardPtr->Providers.erase( providerName ); 
        }

        virtual void Update () =0;

        std::string GetName () const { return providerName; } 

        const Blackboard * GetBlackboardPtr () const { return BlackboardPtr; } 

        virtual const std::vector<std::string> GetRequirements () { return Requirements; } 

        
    protected:

        void SetRequirement ( std::string ReqName ) { Requirements.push_back ( ReqName ); }

        virtual void SetRequirements () =0;

    private:

        const std::string providerName;

        Blackboard * const BlackboardPtr;

        std::vector<std::string> Requirements;


};


typedef GenericFactory<Provider,std::string,Provider * (*) (Blackboard *),Blackboard*> ProviderFactory;

template<class T>
struct ProviderRegistrar {
    typedef Registrar<ProviderFactory,Provider,std::string,T,Blackboard*> Type;
};

//FIXME setreq can be done by scheduler
#define DEFAULT_PROV_CONSTRUCTOR(PROV,REP)                            \
                PROV ( Blackboard * blk ) : Provider ( #PROV , blk ), \
                REP ( blk )                                           \
                { SetRequirements(); }


#define REQUIREMENTS_START void SetRequirements() {
#define REQUIRE(TYPE) SetRequirement(#TYPE);
#define REQUIREMENTS_END }

#define REP_INSTANCE(TYPE) \
    static_cast<TYPE*> ( GetBlackboardPtr()->GetRepInstanceByName(#TYPE) )

#endif // _provider_h_
