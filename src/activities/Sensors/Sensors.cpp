#include "Sensors.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include "hal/robot/generic_nao/kAlBroker.h"

#include "tools/logger.h"
#include "tools/toString.h"

using namespace AL;
using namespace std;
//#define UNNEEDED

namespace {
	ActivityRegistrar<Sensors>::Type temp("Sensors");
}

Sensors::Sensors():Publisher("RtSensors")
{
	;
}

void Sensors::UserInit() {
	Logger::Instance().WriteMsg("Sensors", "Sensors UserInit", Logger::Info);

	try {
		dcm = KAlBroker::Instance().GetBroker()->getDcmProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("Sensors", "Error in getting dcm proxy", Logger::FatalError);
	}

	try {
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("Sensors", "Error in getting ALmemory proxy", Logger::FatalError);
	}

	try {
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("Sensors", "Error in getting motion proxy", Logger::FatalError);
	}

	initialisation();
	this->attachPublisherToMessageQueue(*_com->get_message_queue());
	Logger::Instance().WriteMsg("Sensors", "Sensor Controller Initialized", Logger::Info);
}

int Sensors::Execute() {

	static bool firstrun = true;
	float oldvalue,newvalue;
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
#ifdef KROBOT_IS_REMOTE_OFF
		rtmfast.start();
		cout<<"BIND TO DCM postProcess!"<<endl;
		KAlBroker::Instance().GetBroker()->getProxy("DCM")->getModule()->atPostProcess(KALBIND(&Sensors::synchronisedDCMcallback , this));
		//dcm->getModule()->atPostProcess(KALBIND(&Sensors::synchronisedDCMcallback , this));
#endif
		firstrun = false;
	}

	rtm.stop();
	timediff = rtm.diffNs();
	rtm.start();

#ifndef KROBOT_IS_REMOTE_OFF
	//Values["Head"] = memory->getListData(devicesInChains["Head"]);


	jointValues = memory->getListData(jointKeys);
	for(unsigned i=0;i<jointValues.size();i++)
	{
		oldvalue = ASM.jointdata(i).sensorvalue();
		ASM.mutable_jointdata(i)->set_sensorvalue(jointValues[i]);
		ASM.mutable_jointdata(i)->set_sensorvaluediff(jointValues[i] - oldvalue);

	}
	sensorValues = memory->getListData(sensorKeys);
	for(unsigned i=0;i<jointValues.size();i++)
	{
		oldvalue = ASM.sensordata(i).sensorvalue();
		newvalue=smoothness*jointValues[i]+(1-smoothness)*oldvalue;
		ASM.mutable_sensordata(i)->set_sensorvalue(newvalue);
		ASM.mutable_sensordata(i)->set_sensorvaluediff(newvalue - oldvalue);


	}

	ASM.set_timediff(timediff);
	_blk->publishData(ASM, "sensors");
#endif

	vector<float> RobotValues=motion->getRobotPosition(true);
	//A vector containing the World Absolute Robot Position. (Absolute Position X, Absolute Position Y, Absolute Angle Z)
	for (unsigned i = 0; i < RobotValues.size(); i++) {
		oldvalue = RPM.sensordata(i).sensorvalue();
		RPM.mutable_sensordata(i)->set_sensorvalue(RobotValues[i]);
		RPM.mutable_sensordata(i)->set_sensorvaluediff(RobotValues[i] - oldvalue);


	}
	RPM.set_timediff(timediff);
	_blk->publishData(RPM, "sensors");

	return 0;
}
#ifdef KROBOT_IS_REMOTE_OFF
void Sensors::synchronisedDCMcallback() {
	//Fead data add to messages
	rtmfast.stop();
	timedifffast = rtmfast.diffNs();
	rtmfast.start();
	float oldval,newval;
	//All Joints
	for(unsigned i=0;i<jointPtr.size();i++)
	{
		oldval=ASM.jointdata(i).sensorvalue();
		newval=*jointPtr[i];
		ASM.mutable_jointdata(i)->set_sensorvalue(newval);
		ASM.mutable_jointdata(i)->set_sensorvaluediff(newval-oldval);
	}
	//All Sensors
	for(unsigned i=0;i<sensorPtr.size();i++)
	{
		oldval=ASM.sensordata(i).sensorvalue();
		newval=smoothness*(*sensorPtr[i])+(1-smoothness)*oldval;
		ASM.mutable_sensordata(i)->set_sensorvalue(newval);
		ASM.mutable_sensordata(i)->set_sensorvaluediff(newval-oldval);
	}
	ASM.set_timediff(timedifffast);

	//========== PUBLISH MSG ===/

    msgentry nmsg;

    google::protobuf::Message * newptr=ASM.New();
    newptr->CopyFrom(ASM);
    nmsg.msg.reset(newptr);

    //cout<<"In:"<<&msg;
    //cout<<"Copy:"<<nmsg.msg<<endl;
    nmsg.host="localhost";
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    //nmsg.timeoutstamp=now+boost::posix_time::millisec(50);
    nmsg.timestamp=now;
    nmsg.topic="sensors";
    //nmsg.publisher=Publisher::getName();
    nmsg.msgclass=msgentry::DATA;

    this->publish(nmsg);
}

void Sensors::initFastAccess() {

	jointPtr.resize(KDeviceLists::NUMOFJOINTS);
	for (int i = 0; i < KDeviceLists::NUMOFJOINTS; i++) {

		jointPtr[i] = (float *) memory->getDataPtr(jointKeys[i]);
		//cout<<jointKeys[i]<<i<<" "<<jointPtr[i] <<endl;
		ASM.add_jointdata();
	}
	sensorPtr.resize(KDeviceLists::NUMOFSENSORS);
	for (int i = 0; i < KDeviceLists::NUMOFSENSORS; i++) {
		sensorPtr[i] = (float *) memory->getDataPtr(sensorKeys[i]);
		//cout<<sensorKeys[i]<<i<<" "<<sensorPtr[i] <<endl;
		ASM.add_sensordata();
	}
}
#endif
void Sensors::initialisation() {
	smoothness = 0.75;

	//HeadJointSensorsMessage

	//InertialSensorsMessage

	//UltraSoundSensorsMessage

	//RobotPositionMessage

	ASM.Clear();
	jointKeys=KDeviceLists::getJointKeys();
	for(unsigned i=0;i<jointKeys.size();i++)
		ASM.add_jointdata();

	sensorKeys=KDeviceLists::getSensorKeys();
	for(unsigned i=0;i<sensorKeys.size();i++)
		ASM.add_sensordata();

	RPM.add_sensordata(); //X
	RPM.add_sensordata(); //Y
	RPM.add_sensordata(); //Angle

#ifndef KROBOT_IS_REMOTE_OFF
	jointValues.resize(jointKeys.size());//Robot Position
	jointValues.resize(sensorKeys.size());//Robot Position
	jointValues.assign(jointKeys.size(), 0);
	sensorValues.assign(sensorKeys.size(),0);
#else
	initFastAccess();
#endif
	cout << " Number of joints distibuted " <<  jointKeys.size() << endl;
	cout << " Number of sensors distibuted " << sensorKeys.size() << endl;
}

