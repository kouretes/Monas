#ifndef _ISPECIALACTION_H_
#define _ISPECIALACTION_H_ 1

#include "core/elements/factory/Factory.hpp"
#include "core/elements/factory/Registrar.hpp"

#include <string>

class ISpecialAction
{

public:

	virtual std::string GetName () = 0;

	virtual int ExecutePost() = 0;

};

typedef Factory < ISpecialAction, std::string >  SpecialActionFactory;

template<class T>
struct SpecialActionRegistrar
{
	typedef Registrar<SpecialActionFactory, ISpecialAction, std::string, T> Type;
};

#endif /* _ISPECIALACTION_H_ */

