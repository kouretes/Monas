#ifndef _GenFactory_h_
#define _GenFactory_h_ 1

#include <string>
#include <map>
#include <sstream>
#include <vector>

#include "tools/errorPolicies.h"


template
<
  class Product,
  class IdType,
  class ProductCreator = Product* (*)(),
  class T1 = bool,
  class T2 = bool,
  class ErrorPolicy =LogErrAndExitPolicy
>
class GenericFactory
    : public ErrorPolicy
{

    public:

        bool Register(const IdType& id, ProductCreator cr){

            return assoc.insert(typename Id2TypeMap::value_type(id,cr)).second;

        }

        bool Unregister(const IdType& id ) {

            return assoc.eraser(id) == 1;

        }

        std::vector<IdType> GetRegisteredProducts() {
	    std::vector<IdType> result;
	    typename Id2TypeMap::const_iterator it;
	    for (  it = assoc.begin(); it != assoc.end(); it++ )
		result.push_back(it->first);
	    return result;
	}

        Product* CreateObject(const IdType& id ) {

            typename  Id2TypeMap::const_iterator it = assoc.find(id);
            if ( it != assoc.end() )
                return (it->second)();
            return OnUnknownType(id);
        }

        Product* CreateObject(const IdType& id, T1 p1 ) {

            typename  Id2TypeMap::const_iterator it = assoc.find(id);
            if ( it != assoc.end() )
                return (it->second)(p1);
            return OnUnknownType(id);
        }

        Product* CreateObject(const IdType& id, T1 p1, T2 p2) {

            typename  Id2TypeMap::const_iterator it = assoc.find(id);
            if ( it != assoc.end() )
                return (it->second)(p1,p2);
            return OnUnknownType(id);
        }


        static GenericFactory* Instance() {
            static GenericFactory G;
            return &G;
        }

    private:

        Product* OnUnknownType (const IdType& id) {
            std::ostringstream ostrs;
            ostrs<<"Can't find type for "<<id;
            this->Error(ostrs.str() );
            return NULL;
        }

        GenericFactory() {;}

        typedef std::map<IdType, ProductCreator> Id2TypeMap;
        Id2TypeMap assoc;

};

#endif // _GenFactory_h_
