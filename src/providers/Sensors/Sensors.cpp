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
void Sensors::fillComputedData(unsigned int timediff)
{
	//COMPUTED SENSORS AKA ANGLE
	float AccZvalue = ASM.sensordata(ACC + AXIS_Z).sensorvalue();
	float AccXvalue = ASM.sensordata(ACC + AXIS_X).sensorvalue();
	float AccYvalue = ASM.sensordata(ACC + AXIS_Y).sensorvalue();
	float gyrX = ASM.sensordata(GYR + AXIS_X).sensorvalue();
	float gyrY = ASM.sensordata(GYR + AXIS_Y).sensorvalue();
	float accnorm = sqrt(AccZvalue * AccZvalue + AccYvalue * AccYvalue + AccXvalue * AccXvalue);
	float angX = atan2(-AccYvalue , -AccZvalue) ;//+ gyrX *GYRTOANG;
	float angY = atan2(AccXvalue , -AccZvalue) ;//+ gyrY *GYRTOANG;

	// 	cout<<gyrX<<" "<<gyrY<<" "<<angX<<" "<<angY<<endl;
	if(anglefilterreset)
	{
		angle[0].reset(angX, 0.12);
		angle[1].reset(angY, 0.12);
		anglefilterreset = false;
	}
	try{
		angle[0].predict(timediff/1000000000.0);
		angle[1].predict(timediff/1000000000.0);
		angle[0].updateWithVel(angX,sqrt(fabs(Interpret::GRAVITY_PULL-accnorm)/accnorm)+0.001,gyrX,0.0001*gyrX*gyrX+0.01);
		angle[1].updateWithVel(angY,sqrt(fabs(Interpret::GRAVITY_PULL-accnorm)/accnorm)+0.001,gyrY,0.0001*gyrY*gyrY+0.01);
	} catch (...) {
	   anglefilterreset=true;
	   LogEntry(LogLevel::Error,GetName()) << "Singular Matrix Exception on Kalman update";
	}

	for(int i = 0; i < ANGLE_SIZE; i++)
	{
		//std::cout<<"true:"<<(angle[i].read())(0)<<std::endl;

		ASM.mutable_computeddata(ANGLE + i)->set_sensorvalue((angle[i].read())(0));
		ASM.mutable_computeddata(ANGLE + i)->set_sensorvaluediff((angle[i].read())(1)*timediff / 1000000000.0);
	}

	//Compute support leg
	float L_FSR_FL = ASM.sensordata(L_FSR + FSR_FL).sensorvalue();
	float L_FSR_FR = ASM.sensordata(L_FSR + FSR_FR).sensorvalue();
	float L_FSR_RL = ASM.sensordata(L_FSR + FSR_RL).sensorvalue();
	float L_FSR_RR = ASM.sensordata(L_FSR + FSR_RR).sensorvalue();
	float R_FSR_FL = ASM.sensordata(R_FSR + FSR_FL).sensorvalue();
	float R_FSR_FR = ASM.sensordata(R_FSR + FSR_FR).sensorvalue();
	float R_FSR_RL = ASM.sensordata(R_FSR + FSR_RL).sensorvalue();
	float R_FSR_RR = ASM.sensordata(R_FSR + FSR_RR).sensorvalue();
	float threshold = Interpret::ROBOT_WEIGHT/4;

	float lWeight = L_FSR_FL + L_FSR_FR + L_FSR_RL + L_FSR_RR;
	float rWeight = R_FSR_FL + R_FSR_FR + R_FSR_RL + R_FSR_RR;
	if(lWeight > threshold && rWeight > threshold){
		ASM.mutable_computeddata(SUPPORT_LEG)->set_sensorvalue(SUPPORT_LEG_BOTH);
	}else if(lWeight > threshold){
		ASM.mutable_computeddata(SUPPORT_LEG)->set_sensorvalue(SUPPORT_LEG_LEFT);
	}else if(rWeight > threshold){
		ASM.mutable_computeddata(SUPPORT_LEG)->set_sensorvalue(SUPPORT_LEG_RIGHT);
	}else{
		ASM.mutable_computeddata(SUPPORT_LEG)->set_sensorvalue(SUPPORT_LEG_NONE);
	}
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
	float oldvalue;
	vector<float> RobotValues = motion->getRobotPosition(true);

	//A vector containing the World Absolute Robot Position. (Absolute Position X, Absolute Position Y, Absolute Angle Z)
	for (unsigned i = 0; i < RobotValues.size(); i++)
	{
		oldvalue = RPM.sensordata(i).sensorvalue();
		RPM.mutable_sensordata(i)->set_sensorvalue(RobotValues[i]);
		RPM.mutable_sensordata(i)->set_sensorvaluediff(RobotValues[i] - oldvalue);
	}

	RPM.set_timediff(timediff);
	publishData(RPM, "sensors");
	return 0;
}

void Sensors::fetchValues()
{
	//Fead data add to messages
	rtm.stop();
	timediff = rtm.diffNs();
	rtm.start();
	float oldval, newval;
	//All JointsjointValues

	for(unsigned i = 0; i < jointValues.size(); i++)
	{
		oldval = ASM.jointdata(i).sensorvalue();
		newval = readVector(jointValues, i);
		ASM.mutable_jointdata(i)->set_sensorvalue(newval);
		ASM.mutable_jointdata(i)->set_sensorvaluediff(newval - oldval);
	}

	//All Sensors
	//float gyrRef = fabs((readVector(sensorValues, GYR + AXIS_Z) - Interpret::GYR_Z_RAW) / Interpret::GYR_Z_RAW);
	// 	Logger::Instance().WriteMsg("SENSORS","Current: "
	// 				      +_toString(readVector(sensorValues,GYR+AXIS_Z))
	// 				      +" Avg: "+_toString(gyravg[AXIS_Z].read_mean())
	// 				      + " Res: "+_toString(gyrRef),
	// 						Logger::Error);
	float accn = sqrt((readVector(sensorValues, ACC + AXIS_X)) * (readVector(sensorValues, ACC + AXIS_X)) +
	                  (readVector(sensorValues, ACC + AXIS_Y)) * (readVector(sensorValues, ACC + AXIS_Y)) +
	                  (readVector(sensorValues, ACC + AXIS_Z)) * (readVector(sensorValues, ACC + AXIS_Z))    );
	float accRef = fabs((accn - Interpret::ACC_NORM)) / Interpret::ACC_NORM;
	bool isvalid = accRef < 0.5;

	//if ( !isvalid )
	//	Logger::Instance().WriteMsg("SENSORS","Invalid Data gyrref = " + _toString(gyrRef) + " accRef = " + _toString(accRef),Logger::Error);

	if( isvalid )
	{
		sc.inc();
		accnorm.update(accn, sc);
		float accgain = Interpret::GRAVITY_PULL / accnorm.read_mean();
		oldval = ASM.sensordata(GYR + AXIS_Z).sensorvalue();
		gyravg[AXIS_Z].update(-1520/*readVector(sensorValues, GYR + AXIS_Z)*/, sc);
		newval = gyravg[AXIS_Z].read_mean();
		ASM.mutable_sensordata(GYR + AXIS_Z)->set_sensorvalue(newval);
		ASM.mutable_sensordata(GYR + AXIS_Z)->set_sensorvaluediff(newval - oldval);
		float gyrgain = (Interpret::GYR_Z_REF / gyravg[AXIS_Z].read_mean()) * Interpret::GYR_GAIN;
		//cout<<gyravg[AXIS_Z].read_mean()<<" "<<gyrgain<<endl;

		for(unsigned i = 0; i < GYR_SIZE - 1; i++) //EXCLUDE GYR_REF/GYR_Z
		{
			oldval = ASM.sensordata(GYR + i).sensorvalue();
			gyravg[i].update(readVector(sensorValues, GYR + i), sc);
			newval = (readVector(sensorValues, GYR + i) - gyravg[i].read_mean()) * gyrgain;
			//newval=(newval)+(1-smoothness)*oldval;
			ASM.mutable_sensordata(GYR + i)->set_sensorvalue(newval);
			ASM.mutable_sensordata(GYR + i)->set_sensorvaluediff(newval - oldval);
		}

		for(unsigned i = 0; i < ACC_SIZE; i++)
		{
			oldval = ASM.sensordata(ACC + i).sensorvalue();
			newval = (readVector(sensorValues, ACC + i)) * accgain;
			//newval=(newval)+(1-smoothness)*oldval;
			ASM.mutable_sensordata(ACC + i)->set_sensorvalue(newval);
			ASM.mutable_sensordata(ACC + i)->set_sensorvaluediff(newval - oldval);
		}

	}

	for(unsigned i = L_FSR; i < sensorValues.size(); i++)
	{
		oldval = ASM.sensordata(i).sensorvalue();
		newval = smoothness * (readVector(sensorValues, i)) + (1 - smoothness) * oldval;
		ASM.mutable_sensordata(i)->set_sensorvalue(newval);
		ASM.mutable_sensordata(i)->set_sensorvaluediff(newval - oldval);
	}
	if( isvalid){
		fillComputedData(timediff);
	}
	ASM.set_timediff(timediff);
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

	sensorstopicid = Messaging::Topics::Instance().getId("sensors");
	buttoneventstopicid = Messaging::Topics::Instance().getId("buttonevents");
#endif
	BM.Clear();

	for(unsigned i = 0; i < jointKeys.size(); i++)
	{
		ASM.add_jointdata();
	}

	for(unsigned i = 0; i < sensorKeys.size(); i++)
	{
		ASM.add_sensordata();
	}

	for (int i = 0; i < KDeviceLists::NUMOFCOMPUTEDSENSORS; i++)
	{
		ASM.add_computeddata();
	}

	for(unsigned i = 0; i < buttonKeys.size(); i++)
	{
		BM.add_data(0);
	}

	RPM.add_sensordata(); //X
	RPM.add_sensordata(); //Y
	RPM.add_sensordata(); //Angle
	angle[0].init(60000);
	angle[1].init(60000);
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
