#ifndef _agent_h_
#define _agent_h_ 1

#include <vector>
#include <string>

#include "architecture/blackboard.h"
#include "architecture/provider.h"
#include "architecture/providerHandler.h"

#include "hal/threadable.h"

class Agent : public Threadable  {

    public:

        Agent ( std::vector<std::string> providerList ) {
            blk = new Blackboard;
            providers = hand.LoadProviders ( blk , providerList );
        }

        virtual ~Agent () {
            hand.UnloadProviders (blk); //,providers );
        }

        int ThreadMain ( ) {
            //Update frame
            for ( ProvList::iterator it=providers.begin(); it != providers.end(); ++it ) 
                (*it)->Update();
            return 0;
        }


    private:

        ProviderHandler hand;

        Blackboard * blk;

        typedef std::vector<Provider *> ProvList;

        ProvList providers; //in execution order 

};

#endif // _agent_h_
