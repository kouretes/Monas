#include "Sensors.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "tools/logger.h"
#include "tools/toString.h"

using namespace AL;
using namespace std;
using namespace KDeviceLists;
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
void Sensors::fillComputedData(unsigned int timediff)
{
	static bool firstrun=true;

	//COMPUTED SENSORS AKA ANGLE
	float AccZvalue = ASM.sensordata(ACC + AXIS_Z).sensorvalue();
	float AccXvalue = ASM.sensordata(ACC + AXIS_X).sensorvalue();
	float AccYvalue = ASM.sensordata(ACC + AXIS_Y).sensorvalue();
	float gyrX = ASM.sensordata(GYR + AXIS_X).sensorvalue();
	float gyrY = ASM.sensordata(GYR + AXIS_Y).sensorvalue();
	float accnorm = sqrt(AccZvalue * AccZvalue + AccYvalue * AccYvalue + AccXvalue * AccXvalue);
	float angX = atan2(-AccYvalue , -AccZvalue) ;//+ gyrX *GYRTOANG;
	float angY = atan2(AccXvalue , -AccZvalue) ;//+ gyrY *GYRTOANG;
	//cout<<gyrX<<" "<<gyrY<<" "<<angX<<" "<<angY<<endl;
	if(firstrun)
	{
		angle[0].reset(angX,0.1);
		angle[1].reset(angY,0.1);
		firstrun=false;
	}
	angle[0].updateWithVel(angX,0.01*sqrt(fabs(Interpret::GRAVITY_PULL-accnorm)/accnorm)+0.0003,gyrX,0.01*gyrX*gyrX+0.0003,timediff/1000000000.0);
	angle[1].updateWithVel(angY,0.01*sqrt(fabs(Interpret::GRAVITY_PULL-accnorm)/accnorm)+0.0003,gyrY,0.01*gyrY*gyrY+0.0003,timediff/1000000000.0);
	for(int i=0;i<ANGLE_SIZE;i++)
	{
		ASM.mutable_computeddata(ANGLE+i)->set_sensorvalue((angle[i].read())(0));
		ASM.mutable_computeddata(ANGLE+i)->set_sensorvaluediff((angle[i].read())(1)*timediff/1000000000.0);
	}



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
		//Init inertial statistics
		sc.inc();
		//gyravg[AXIS_Z].update(Interpret::GYR_OFFSET,sc);

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
	//Detect inertialBoard Glitch
	if(fabs(sensorValues[GYR+AXIS_Z]-gyravg[AXIS_Z].read_mean())/gyravg[AXIS_Z].read_mean()<0.1||sc.i<50 )
	{
		sc.inc();
		float accn=sqrt(sensorValues[ACC+AXIS_X] +sensorValues[ACC+AXIS_X] +sensorValues[ACC+AXIS_Y] *sensorValues[ACC+AXIS_Y] +sensorValues[ACC+AXIS_Z]*sensorValues[ACC+AXIS_Z]    );


		oldvalue = ASM.sensordata(GYR+AXIS_Z).sensorvalue();
		gyravg[AXIS_Z].update(sensorValues[AXIS_Z],sc);
		newvalue=gyravg[AXIS_Z].read_mean();
		ASM.mutable_sensordata(GYR+AXIS_Z)->set_sensorvalue(newvalue);
		ASM.mutable_sensordata(GYR+AXIS_Z)->set_sensorvaluediff(newvalue - oldvalue);

		accnorm.update(accn,sc);
		float accgain=Interpret::GRAVITY_PULL/accnorm.read_mean();
		float gyrgain=(Interpret:GYR_Z_REF/sensorValues[ACC+AXIS_Z])*Interpret::GYR_GAIN;
		for(unsigned i=0;i<GYR_SIZE-1;i++)//EXCLUDE GYR_REF/GYR_Z
		{
			oldvalue = ASM.sensordata(GYR+i).sensorvalue();
			gyravg[i].update(sensorValues[GYR+i],sc);
			newvalue=(sensorValues[GYR+i]-gyravg[i].read_mean())*gyrgain;
			//newvalue=(newvalue)+(1-smoothness)*oldvalue;
			ASM.mutable_sensordata(GYR+i)->set_sensorvalue(newvalue);
			ASM.mutable_sensordata(GYR+i)->set_sensorvaluediff(newvalue - oldvalue);

		}

		for(unsigned i=0;i<ACC_SIZE;i++)//EXCLUDE GYR_REF/GYR_Z
		{
			oldvalue = ASM.sensordata(ACC+i).sensorvalue();
			newvalue=sensorValues[ACC+i]*accgain;
			//newvalue=smoothness*(newvalue)+(1-smoothness)*oldvalue;
			ASM.mutable_sensordata(ACC+i)->set_sensorvalue(newvalue);
			ASM.mutable_sensordata(ACC+i)->set_sensorvaluediff(newvalue - oldvalue);
		}
		fillComputedData(timediff);

	}
	for(unsigned i=L_FSR;i<sensorValues.size();i++)
	{
		oldval=ASM.sensordata(i).sensorvalue();
		newval=smoothness*(sensorValues[i])+(1-smoothness)*oldval;
		ASM.mutable_sensordata(i)->set_sensorvalue(newval);
		ASM.mutable_sensordata(i)->set_sensorvaluediff(newval-oldval);
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
	//cout<<fabs((*sensorPtr[GYR+AXIS_Z])-gyravg[AXIS_Z].read_mean())<<" "<<3*sqrt(gyravg[AXIS_Z].read_var())<<endl;
	if(fabs((*sensorPtr[GYR+AXIS_Z])-gyravg[AXIS_Z].read_mean())/gyravg[AXIS_Z].read_mean()<0.1||sc.i<50 )
	{
		sc.inc();
		float accn=sqrt((*sensorPtr[ACC+AXIS_X])*(*sensorPtr[ACC+AXIS_X]) +
						(*sensorPtr[ACC+AXIS_Y])*(*sensorPtr[ACC+AXIS_Y]) +
						(*sensorPtr[ACC+AXIS_Z])*(*sensorPtr[ACC+AXIS_Z])    );
		accnorm.update(accn,sc);
		float accgain=Interpret::GRAVITY_PULL/accnorm.read_mean();
		oldval = ASM.sensordata(GYR+AXIS_Z).sensorvalue();
		gyravg[AXIS_Z].update(*sensorPtr[GYR+AXIS_Z],sc);
		newval=gyravg[AXIS_Z].read_mean();
		ASM.mutable_sensordata(GYR+AXIS_Z)->set_sensorvalue(newval);
		ASM.mutable_sensordata(GYR+AXIS_Z)->set_sensorvaluediff(newval - oldval);

		float gyrgain=(Interpret::GYR_Z_REF/gyravg[AXIS_Z].read_mean())*Interpret::GYR_GAIN;
		//cout<<gyravg[AXIS_Z].read_mean()<<" "<<gyrgain<<endl;

		for(unsigned i=0;i<GYR_SIZE-1;i++)//EXCLUDE GYR_REF/GYR_Z
		{
			oldval = ASM.sensordata(GYR+i).sensorvalue();
			gyravg[i].update(*sensorPtr[GYR+i],sc);
			newval=(*sensorPtr[GYR+i]-gyravg[i].read_mean())*gyrgain;
			//newval=(newval)+(1-smoothness)*oldval;

			ASM.mutable_sensordata(GYR+i)->set_sensorvalue(newval);
			ASM.mutable_sensordata(GYR+i)->set_sensorvaluediff(newval - oldval);

		}

		for(unsigned i=0;i<ACC_SIZE;i++)//EXCLUDE GYR_REF/GYR_Z
		{
			oldval = ASM.sensordata(ACC+i).sensorvalue();
			newval=(*sensorPtr[ACC+i])*accgain;
			//newval=(newval)+(1-smoothness)*oldval;
			ASM.mutable_sensordata(ACC+i)->set_sensorvalue(newval);
			ASM.mutable_sensordata(ACC+i)->set_sensorvaluediff(newval - oldval);
		}

		fillComputedData(timedifffast);
	}
	for(unsigned i=L_FSR;i<sensorPtr.size();i++)
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

    nmsg.host="localhost";
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    //nmsg.timeoutstamp=now+boost::posix_time::millisec(50);
    nmsg.timestamp=now;
    nmsg.topic="sensors";
    //nmsg.publisher=Publisher::getName();
    nmsg.msgclass=msgentry::DATA;

    this->publish(nmsg);




    commands[5][0][0] =-(angle[0].read())(0) - 0.0010*(angle[0].read())(1);//hm->parameter(p);
	commands[5][1][0] = -(angle[1].read())(0)- 0.0010*(angle[1].read())(1);//hm->parameter(p);
	//cout<<(angle[0].read())(0)<<" "<<(angle[1].read())(0)<<endl;
	int DCMtime;
	try
	{ // Get time in 0 ms
		DCMtime = dcm->getTime(0);
	} catch (const AL::ALError &e)
	{
		throw ALERROR("mainModule", "execute_action()", "Error on DCM getTime : " + e.toString());
	}
	commands[4][0] = DCMtime;
	//Send command
	try
	{
		//dcm->setAlias(commands);
	} catch (const AL::ALError &e)
	{
		throw ALERROR("mainModule", "execute_action", "Error when sending command to DCM : " + e.toString());
	}
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

	for (int i = 0; i < KDeviceLists::NUMOFCOMPUTEDSENSORS; i++) {
		ASM.add_computeddata();
	}

	AL::ALValue jointAliasses;
	vector<std::string> PosActuatorStrings = KDeviceLists::getPositionActuatorKeys();

	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("HeadjointActuator");

	jointAliasses[1].arraySetSize(2);

	// Joints actuator list
	jointAliasses[1][YAW] = PosActuatorStrings[HEAD + YAW];
	jointAliasses[1][PITCH] = PosActuatorStrings[HEAD + PITCH];

	// Create alias
	try
	{
		dcm->createAlias(jointAliasses);
	} catch (const AL::ALError &e)
	{
		throw ALERROR("mainModule", "createPositionActuatorAlias()", "Error when creating Alias : " + e.toString());
	}

	// Create Commands
	commands.arraySetSize(6);
	commands[0] = std::string("HeadjointActuator");
	commands[1] = std::string("ClearAll"); // Erase all previous commands
	commands[2] = std::string("time-separate");
	commands[3] = 0;

	commands[4].arraySetSize(1);
	//commands[4][0]  Will be the new time

	commands[5].arraySetSize(HEAD_SIZE); // For all joints

	for (int i = 0; i < (HEAD_SIZE); i++)
	{
		commands[5][i].arraySetSize(1);
		//commands[5][i][0] will be the new angle
	}
	angle[0].init(60000);
	angle[1].init(60000);
	//angle[0].reset(0,10);
	//angle[1].reset(0,10);

}
#endif
void Sensors::initialisation() {
	smoothness = 0.75;

	ASM.Clear();
	jointKeys=KDeviceLists::getJointKeys();
	for(unsigned i=0;i<jointKeys.size();i++)
	{
		ASM.add_jointdata();
		ASM.mutable_jointdata(i)->set_sensorvalue(0);
	}

	sensorKeys=KDeviceLists::getSensorKeys();
	for(unsigned i=0;i<sensorKeys.size();i++)
	{
		ASM.add_sensordata();
		ASM.mutable_sensordata(i)->set_sensorvalue(0);

	}

	RPM.add_sensordata(); //X
	RPM.add_sensordata(); //Y
	RPM.add_sensordata(); //Angle

#ifndef KROBOT_IS_REMOTE_OFF
	jointValues.resize(jointKeys.size());
	jointValues.resize(sensorKeys.size());
	jointValues.assign(jointKeys.size(), 0);
	sensorValues.assign(sensorKeys.size(),0);
#else
	initFastAccess();
#endif
	cout << " Number of joints distibuted " <<  jointKeys.size() << endl;
	cout << " Number of sensors distibuted " << sensorKeys.size() << endl;
}

