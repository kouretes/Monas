#ifndef _providerHandler_h_
#define _providerHandler_h_ 1

#include <string>
#include <vector>

#include "architecture/blackboard.h"
#include "architecture/provider.h"
#include "tools/logger.h"
#include "hal/syscall.h"

#include "tools/XMLConfig.h"


class ProviderHandler {

    public:
        ProviderHandler ( ) { //Blackboard * const blk ) {
            //XMLConfig providersXML ( asdf::configurationPath+"providers.xml" );
            //TODO  if xml file not loaded throw exception
            //std::vector<std::string> providers;
            //providersXML.QueryElement( "provider" , providers );
            //std::cout<<"Providers size: "<<providers.size()<<std::flush<<" provider 2: "<<providers[0]<<std::endl;  //TODO DEBUG & INFO directives

        }

        std::vector<Provider *> LoadProviders ( Blackboard * blk, std::vector<std::string> providers ) //const blackboar
        {
            std::vector<Provider *> prov;
            for ( unsigned int i=0;i<providers.size();++i) {
                //std::map<std::string,Provider::NewProviderPtr>::iterator it = Provider::ProvidersList.find( providers[i] );
                //if ( it == Provider::ProvidersList.end() ) {
                //    Logger::Instance()->WriteMsg("ProviderHandler","Can't find provider "+providers[i], Logger::FatalError );
                //    SysCall::_exit(1);
                //}
                //ProviderFactory::Instance()->CreateObject(providers[i],blk);
                prov.push_back( ProviderFactory::Instance()->CreateObject(providers[i],blk) );
                //prov.push_back( Provider::ProvidersList[ providers[i] ] (blk) ); //TODO fix
            }
            return prov;            
        }

        void UnloadProviders ( const Blackboard * const blk ) const
        {
            ;
        }
        
        
};

#endif // _providerHandler_h_
