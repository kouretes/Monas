#include "Sensors.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "architecture/narukom/pub_sub/topicTree.h"

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
	Logger::Instance().WriteMsg("Sensors", "Sensors UserInit", Logger::Info);

	try
	{
		dcm = new AL::DCMProxy(KAlBroker::Instance().GetBroker());
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("Sensors", "Error in getting dcm proxy", Logger::FatalError);
	}

	try
	{
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("Sensors", "Error in getting memory proxy", Logger::FatalError);
	}

	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("Sensors", "Error in getting motion proxy", Logger::FatalError);
	}

	initialization();
	Logger::Instance().WriteMsg("Sensors", "Sensor Controller Initialized", Logger::Info);
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
		angle[0].reset(angX, 0.1);
		angle[1].reset(angY, 0.1);
		anglefilterreset = false;
	}

	try
	{
		angle[0].updateWithVel(angX, 0.01 * sqrt(fabs(Interpret::GRAVITY_PULL - accnorm) / accnorm) + 0.003, gyrX, 0.01 * gyrX * gyrX + 0.003, timediff / 1000000000.0);
		angle[1].updateWithVel(angY, 0.01 * sqrt(fabs(Interpret::GRAVITY_PULL - accnorm) / accnorm) + 0.003, gyrY, 0.01 * gyrY * gyrY + 0.003, timediff / 1000000000.0);
	}
	catch (...)
	{
		anglefilterreset = true;
		Logger::Instance().WriteMsg("SENSORS", "Singular Matrix Exception on Kalman update", Logger::Error);
	}

	for(int i = 0; i < ANGLE_SIZE; i++)
	{
		ASM.mutable_computeddata(ANGLE + i)->set_sensorvalue((angle[i].read())(0));
		ASM.mutable_computeddata(ANGLE + i)->set_sensorvaluediff((angle[i].read())(1)*timediff / 1000000000.0);
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
	float gyrRef = fabs((readVector(sensorValues, GYR + AXIS_Z) - Interpret::GYR_Z_RAW) / Interpret::GYR_Z_RAW);
	// 	Logger::Instance().WriteMsg("SENSORS","Current: "
	// 				      +_toString(readVector(sensorValues,GYR+AXIS_Z))
	// 				      +" Avg: "+_toString(gyravg[AXIS_Z].read_mean())
	// 				      + " Res: "+_toString(gyrRef),
	// 						Logger::Error);
	float accn = sqrt((readVector(sensorValues, ACC + AXIS_X)) * (readVector(sensorValues, ACC + AXIS_X)) +
	                  (readVector(sensorValues, ACC + AXIS_Y)) * (readVector(sensorValues, ACC + AXIS_Y)) +
	                  (readVector(sensorValues, ACC + AXIS_Z)) * (readVector(sensorValues, ACC + AXIS_Z))    );
	float accRef = fabs((accn - Interpret::ACC_NORM)) / Interpret::ACC_NORM;
	bool isvalid = gyrRef < 0.2 && accRef < 0.5;

	// 	if ( !isvalid )
	// 	  Logger::Instance().WriteMsg("SENSORS","Invalid Data",Logger::Error);

	if( isvalid )
	{
		sc.inc();
		accnorm.update(accn, sc);
		float accgain = Interpret::GRAVITY_PULL / accnorm.read_mean();
		oldval = ASM.sensordata(GYR + AXIS_Z).sensorvalue();
		gyravg[AXIS_Z].update(readVector(sensorValues, GYR + AXIS_Z), sc);
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

		fillComputedData(timediff);
	}

	for(unsigned i = L_FSR; i < sensorValues.size(); i++)
	{
		oldval = ASM.sensordata(i).sensorvalue();
		newval = smoothness * (readVector(sensorValues, i)) + (1 - smoothness) * oldval;
		ASM.mutable_sensordata(i)->set_sensorvalue(newval);
		ASM.mutable_sensordata(i)->set_sensorvaluediff(newval - oldval);
	}

	ASM.set_timediff(timediff);
}

#ifndef KROBOT_IS_REMOTE
void Sensors::synchronisedDCMcallback()
{
	fetchValues();
	//========== PUBLISH MSG ===/
	msgentry nmsg;
	google::protobuf::Message * newptr = ASM.New();
	newptr->CopyFrom(ASM);
	nmsg.msg.reset(newptr);
	nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
	//nmsg.timeoutstamp=now+boost::posix_time::millisec(50);
	nmsg.timestamp = now;
	nmsg.topic = sensorstopicid;
	//nmsg.publisher=Publisher::getName();
	nmsg.msgclass = msgentry::DATA;
	this->publish(nmsg);

	if(updateButtons())
	{
		msgentry nmsg;
		google::protobuf::Message * newptr = BM.New();
		newptr->CopyFrom(BM);
		nmsg.msg.reset(newptr);
		nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
		//nmsg.timeoutstamp=now+boost::posix_time::millisec(50);
		nmsg.timestamp = now;
		nmsg.topic = buttoneventstopicid;
		//nmsg.publisher=Publisher::getName();
		nmsg.msgclass = msgentry::SIGNAL;
		this->publish(nmsg);
	}
}
#endif
void Sensors::initialization()
{
	smoothness = 0.75;
	firstrun = true;
	anglefilterreset = true;
	std::vector<std::string> jointKeys, sensorKeys, buttonKeys;
	jointKeys = KDeviceLists::getJointKeys();
	sensorKeys = KDeviceLists::getSensorKeys();
	buttonKeys = KDeviceLists::getButtonKeys();
	jointValues.resize(jointKeys.size());
	sensorValues.resize(sensorKeys.size());
	buttonValues.resize(buttonKeys.size());
	buttonevnts.resize(buttonKeys.size());
	jointValues.assign(jointKeys.size(), 0);
	sensorValues.assign(sensorKeys.size(), 0);
	buttonValues.assign(buttonKeys.size(), 0);
	struct buttonstate t;
	t.last_pressed = boost::posix_time::microsec_clock::universal_time();
	t.last_val = KDeviceLists::Interpret::BUTTON_PRESSED;
	t.count = 0;
	buttonevnts.assign(buttonKeys.size(), t);
#ifndef KROBOT_IS_REMOTE

	for (unsigned i = 0; i < jointKeys.size(); i++)
	{
		jointValues[i] = (float *) memory->getDataPtr(jointKeys[i]);
	}

	for (unsigned i = 0; i < sensorKeys.size(); i++)
	{
		sensorValues[i] = (float *) memory->getDataPtr(sensorKeys[i]);
	}

	for (unsigned i = 0; i < buttonKeys.size(); i++)
	{
		buttonValues[i] = (float *) memory->getDataPtr(buttonKeys[i]);
		cout << buttonValues[i] << endl;
	}

	sensorstopicid = Topics::Instance().getId("sensors");
	buttoneventstopicid = Topics::Instance().getId("buttonevents");
#endif
	ASM.Clear();
	RPM.Clear();
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
	cout << " Number of joints distibuted " <<  jointKeys.size() << endl;
	cout << " Number of sensors distibuted " << sensorKeys.size() << endl;
	cout << " Number of buttons captured " << buttonKeys.size() << endl;
}

bool Sensors::updateButtons()
{
	bool dispachevent = false;
	boost::posix_time::ptime now, d, t;
	now = boost::posix_time::microsec_clock::universal_time();
	d = now - boost::posix_time::milliseconds(DEBOUNCE_MILLISEC);
	t = now - boost::posix_time::milliseconds(MCLICKDISTANCE_MILLISEC);

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
