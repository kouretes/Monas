#ifndef _kalbroker_h_
#define _kalbroker_h_ 1
#include "hal/robot/generic_nao/aldebaran-proxy.h"
#include "tools/singleton.h"

namespace AL
{
	class ALBroker;
	//    template <class T> class  ALPtr;
}

class KAlBrokerClass
{

public:
	KAlBrokerClass();

	void SetBroker(boost::shared_ptr<AL::ALBroker>  broker);

	boost::shared_ptr<AL::ALBroker> GetBroker () const;

	boost::shared_ptr<AL::ALBroker>  _broker;

};

typedef Singleton<KAlBrokerClass> KAlBroker;

#endif // _kalbroker_h_
