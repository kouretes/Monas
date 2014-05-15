/**
 * @author Cbm
 *
 * Version : $Id$
 * This file was generated by Aldebaran Robotics ModuleGenerator
 */

#include "mainmodule.h"

#include "hal/robot/nao/generic_nao/kAlBroker.h"

#include "core/include/Configurator.hpp"

#include <alproxies/almemoryproxy.h>
#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>

//______________________________________________
// constructor
//______________________________________________
mainModule::mainModule(boost::shared_ptr<AL::ALBroker> broker, const std::string& name) :
		AL::ALModule(broker, name)
{
	std::string bodyId, headId;
	setModuleDescription("This is the Kouretes Team root module ");
	functionName("Start", "mainModule", "Method to start Talws");
	BIND_METHOD(mainModule::Start);
	functionName("Stop", "mainModule", "Method to stop Talws");
	BIND_METHOD(mainModule::Stop);
	boost::shared_ptr<AL::ALMemoryProxy> memory;
	KAlBroker::Instance().SetBroker(broker);
	try
	{
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	} catch (AL::ALError& e)
	{
		std::cerr << "Error in getting memory proxy" << std::endl;
		std::cout << e.what() << std::endl;
	}

	try
	{
		bodyId = std::string(memory->getData("Device/DeviceList/ChestBoard/BodyId"));
		headId = std::string(memory->getData("RobotConfig/Head/HeadId"));
		if (bodyId.size() > 15)
		{
			bodyId = bodyId.substr(16, 19);/*bodyId.size()-5, bodyId.size()-2*/ //manually because aldebarab forgot to put a \0...
		} else
		{
			try
			{
				bodyId = bodyId.substr(11, 14);
			} catch (const std::out_of_range& oor)
			{
				std::cerr << "Out of Range error: " << oor.what() <<" " <<  bodyId << '\n';
			}


		}
	} catch (AL::ALError& e)
	{
		std::cerr << "Error in getting body and/or head id`s" << std::endl;
		bodyId = "";
		headId = "";
	}

	std::cout << "KRobot - Found Head ID: '" << headId << "'" << std::endl;
	//std::cout << "KRobot - Found Body ID: '" << _toString(KRobotConfig::Instance().getConfig(KDeviceLists::Interpret::BODY_ID).size()) << "'" << std::endl;
	std::cout << "KRobot - Found Body ID: '" << bodyId << "'" << std::endl;
#ifndef KROBOT_IS_REMOTE
	Configurator::Instance().initConfigurator("/home/nao/naoqi/config/", headId, bodyId);
#else
	Configurator::Instance().initConfigurator("./config/", "", "");
#endif
	tal = new Talws();
}

//______________________________________________
// destructor
//______________________________________________
mainModule::~mainModule()
{
	delete tal;
}

void mainModule::Start()
{
	tal->Start();
}

void mainModule::Stop()
{
	tal->Stop();
}
