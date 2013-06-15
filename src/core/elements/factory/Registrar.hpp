#ifndef _genRegistrar_h_
#define _genRegistrar_h_ 1

//#include "tools/logger.h"
#include <iostream>
template
<
class FactoryClass,
      class AbstractProductClass,
      class FactoryID,
      class ProductClass,
      class Par1 = bool,
      class Par2 = bool
      >
class Registrar
{

public:

	Registrar ( FactoryID id )
	{
		if ( FactoryClass::Instance()->Register( id, NewProviderFunc ) )
			std::cout << "Registrar: " << id << std::endl;//Logger::Instance().WriteMsg("Registrar", id, Logger::ExtraInfo);
	}

	static AbstractProductClass * NewProviderFunc ()
	{
		return new ProductClass();
	}

	static AbstractProductClass * NewProviderFunc (Par1 p1)
	{
		return new ProductClass(p1);
	}

	static AbstractProductClass * NewProviderFunc (Par1 p1, Par2 p2)
	{
		return new ProductClass(p1, p2);
	}

};


#endif // _genRegistrar_h_
