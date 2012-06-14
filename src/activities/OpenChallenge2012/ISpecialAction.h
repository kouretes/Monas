#ifndef _ISPECIALACTION_H_
#define _ISPECIALACTION_H_ 1

#include "tools/genFactory.h"
#include "tools/genRegistrar.h"

#include <string>

class ISpecialAction  {

    public:        

        virtual std::string GetName ()=0;
	
	virtual int ExecutePost()=0;

};

typedef GenericFactory < ISpecialAction, std::string >  SpecialActionFactory;

template<class T>
struct SpecialActionRegistrar {
    typedef Registrar<SpecialActionFactory,ISpecialAction,std::string,T> Type;
};

#endif /* _ISPECIALACTION_H_ */
 
