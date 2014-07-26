#include "Sensors.h"

#include "hal/robot/nao/generic_nao/kAlBroker.h"

#include "core/include/Logger.hpp"
#include "core/architecture/messaging/TopicTree.hpp"

using namespace AL;
using namespace std;
using namespace KDeviceLists;
//#define UNNEEDED


template<typename T>
T readVector(std::vector<T> const& v, int index )
{
	return v[index];
}

template<typename T>
T readVector(std::vector<T*> const& v, int index )
{
	return *v[index];
}



PROVIDER_REGISTER(Sensors);


void Sensors::UserInit()
{
	LogEntry(LogLevel::Info,GetName())<< "Sensors UserInit";

	try
	{
		dcm = new AL::DCMProxy(KAlBroker::Instance().GetBroker());
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,GetName()) << "Error in getting dcm proxy";
	}

	try
	{
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,GetName()) << "Error in getting memory proxy";
	}

	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,GetName()) << "Error in getting motion proxy";
	}

	initialization();
	LogEntry(LogLevel::Info,GetName()) <<  "Sensor Controller Initialized";
}


int Sensors::Execute()
{
	if(firstrun)
	{
		//Starting US Sensors
		ALValue commands;
		commands.arraySetSize(3);
		commands[0] = string("Device/SubDeviceList/US/Actuator/Value");
		commands[1] = string("Merge");
		commands[2].arraySetSize(1);
		commands[2][0].arraySetSize(2);
		commands[2][0][0] = 68.0;
		commands[2][0][1] = dcm->getTime(10);
		dcm->set(commands);
		rtm.start();
#ifndef KROBOT_IS_REMOTE
		KAlBroker::Instance().GetBroker()->getProxy("DCM")->getModule()->atPostProcess(KALBIND(&Sensors::synchronisedDCMcallback , this));
#endif
		firstrun = false;
	}

#ifdef KROBOT_IS_REMOTE
	//Fetch into vectors
	jointaccess.GetValues(jointValues);
	sensoraccess.GetValues(sensorValues);
	fetchValues();
	publishData(ASM, "sensors");

	if(updateButtons())
	{
		publishSignal(BM, "buttonevents");
	}

#endif

	return 0;
}

#ifndef KROBOT_IS_REMOTE
void Sensors::synchronisedDCMcallback()
{

	if(updateButtons())
	{
		Messaging::MessageEntry nmsg;
		google::protobuf::Message * newptr = BM.New();
		newptr->CopyFrom(BM);
		nmsg.msg.reset(newptr);
		nmsg.host = Messaging::MessageEntry::HOST_ID_LOCAL_HOST;
		KSystem::Time::TimeAbsolute now = KSystem::Time::SystemTime::now();
		//nmsg.timeoutstamp=now+KSystem::Time::millisec(50);
		nmsg.timestamp = now.wrapTo<KSystem::Time::TimeStamp>();
		nmsg.topic = buttoneventstopicid;
		//nmsg.publisher=Publisher::getName();
		nmsg.msgclass = Messaging::MessageEntry::SIGNAL;
		this->publish(nmsg);
	}
}
#endif
void Sensors::initialization()
{
	firstrun = true;
	std::vector<std::string>  buttonKeys;
	buttonKeys = KDeviceLists::getButtonKeys();
	buttonValues.resize(buttonKeys.size());
	buttonevnts.resize(buttonKeys.size());
	buttonValues.assign(buttonKeys.size(), 0);
	struct buttonstate t;
	t.last_pressed = KSystem::Time::SystemTime::now();
	t.last_val = KDeviceLists::Interpret::BUTTON_PRESSED;
	t.count = 0;
	buttonevnts.assign(buttonKeys.size(), t);
#ifndef KROBOT_IS_REMOTE

	for (unsigned i = 0; i < buttonKeys.size(); i++)
	{
		buttonValues[i] = (float *) memory->getDataPtr(buttonKeys[i]);
		cout << buttonValues[i] << endl;
	}

	buttoneventstopicid = Messaging::Topics::Instance().getId("buttonevents");
#endif
	BM.Clear();



	for(unsigned i = 0; i < buttonKeys.size(); i++)
	{
		BM.add_data(0);
	}

#ifdef KROBOT_IS_REMOTE
	jointaccess.ConnectToVariables(KAlBroker::Instance().GetBroker(), jointKeys, false);
	sensoraccess.ConnectToVariables(KAlBroker::Instance().GetBroker(), sensorKeys, false);
	buttonaccess.ConnectToVariables(KAlBroker::Instance().GetBroker(), buttonKeys, false);
#endif
	cout << " Number of buttons captured " << buttonKeys.size() << endl;
}

bool Sensors::updateButtons()
{
	bool dispachevent = false;
	KSystem::Time::TimeAbsolute now, d, t;
	now = KSystem::Time::SystemTime::now();
	d = now - KSystem::Time::TimeAbsolute::milliseconds(DEBOUNCE_MILLISEC);
	t = now - KSystem::Time::TimeAbsolute::milliseconds(MCLICKDISTANCE_MILLISEC);

	for(unsigned i = 0; i < buttonValues.size(); ++i)
	{
		if(buttonevnts[i].last_pressed < d) //Wait for switch debounce
		{
			float v = readVector(buttonValues, i);

			//cout<<"i:"<<i<< " :"<<v<<endl;
			if(buttonevnts[i].last_val != v) //If there is a change
			{
				buttonevnts[i].last_val = v;

				if(v == KDeviceLists::Interpret::BUTTON_PRESSED)
				{
					++buttonevnts[i].count;
					buttonevnts[i].last_pressed = now;
				}
			}
			else if(buttonevnts[i].count > 0 && buttonevnts[i].last_pressed < t)
				dispachevent = true;
		}
	}

	if(dispachevent)
	{
		//cout<<"DISPATCH:"<<endl;
		for(unsigned i = 0; i < buttonValues.size(); ++i)
		{
			//cout<<i<<" : "<< buttonevnts[i].count<<endl;
			BM.set_data(i, buttonevnts[i].count);
			buttonevnts[i].count = 0;
		}
	}

	return dispachevent;
}
