#ifndef BUTTON_LISTENER_H
#define BUTTON_LISTENER_H
#include "almemoryproxy.h"
#include "alproxy.h"
#include "alsentinelproxy.h"
#include <albroker.h>
#include <alproxy.h>
#include <alptr.h>
#include <alvalue.h>
#include <almodule.h>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "tools/logger.h"
#include "tools/toString.h"

class ButtonListener: public AL::ALModule {
	public:
		ButtonListener(AL::ALPtr<AL::ALBroker> pbroker, const std::string& name);
		virtual ~ButtonListener();
		void buttonPressed(const std::string& pDataName, const AL::ALValue& pValue, const std::string& pMessage); //event handler for the button pressed
		void RBumperPressed(const std::string& pDataName, const AL::ALValue& pValue, const std::string& pMessage); //
		void LBumperPressed(const std::string& pDataName, const AL::ALValue& pValue, const std::string& pMessage); //
	private:
		boost::posix_time::ptime ctime;

		AL::ALPtr<AL::ALProxy> sentinel; //proxy to alsentinel for overriding default button press
		AL::ALPtr<AL::ALMemoryProxy> memory; //proxy to almemory module to exchange data

		boost::posix_time::time_duration dur;
		boost::posix_time::ptime now;
};

#endif // BUTTON_LISTENER_H
