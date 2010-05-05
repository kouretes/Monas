#include "Behavior.h"
#include "messages/VisionObservations.pb.h"
#include "messages/SensorsMessage.pb.h"

#include "hal/robot/generic_nao/kAlBroker.h"

#include "alxplatform.h"
#include <albroker.h>
#include <alproxy.h>
#include <almemoryfastaccess.h>
#include "almotionproxy.h"
#include "almemoryproxy.h"
#include "albrokermanager.h"
#include "alvalue.h"

#include "tools/logger.h"
#include "tools/toString.h"

namespace {
	ActivityRegistrar<Behavior>::Type temp("Behavior");
}

Behavior::Behavior() :
	Publisher("Behavior") {

}

void Behavior::UserInit() {

	_com->get_message_queue()->add_publisher(this);
	_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	try {
		memory = KAlBroker::Instance()->GetBroker()->getMemoryProxy();
	} catch (AL::ALError& e) {
        Logger::Instance()->WriteMsg( "Behavior", "Error in getting memory proxy", Logger::Error );
		//cout << "Error in getting memory proxy" << std::endl;
	}

	mot = new MotionMessage();
	mot->add_parameter(0.0f);
	mot->add_parameter(0.0f);
	mot->add_parameter(0.0f);
	mot->add_parameter(0.0f);
	mot->add_parameter(0.0f);

	ballfound = 0;
    Logger::Instance()->WriteMsg( "Behavior", "Controller Initialized", Logger::Info );
	//cout << "Behavior Controller Initialized" << endl;
}

int Behavior::Execute() {

	MessageBuffer* sub_buf = _blk->getBuffer();
	while (sub_buf->size() > 0) {
		process_messages();
	}
	//SleepMs(100);
}

void Behavior::process_messages() {
	_blk->process_messages();
	//MessageBuffer* sub_buf = Subscriber::getBuffer();
	//google::protobuf::Message* cur = sub_buf->remove_head();

	BallTrackMessage* bmsg = dynamic_cast<BallTrackMessage*> (_blk->in_nb("BallTrackMessage", "Vision"));
	if (bmsg != 0) {
        Logger::Instance()->WriteMsg( "Behavior", "ProcessMessages", Logger::ExtraInfo );
		//cout<<"ProcessMessages"<<endl;
		//if (cur != NULL) {
		//cout << "ProcessMessages found message" << endl;
		//if (cur->GetTypeName() == "BallTrackMessage") {

		//BallTrackMessage * bmsg = static_cast<BallTrackMessage*> (cur);
		if (bmsg->radius() > 0) {
			float overshootfix = bmsg->radius();
			overshootfix = 2 * (0.4f - overshootfix);
            Logger::Instance()->WriteMsg( "Behavior", "Overshoot Value: " + _toString( overshootfix ), Logger::ExtraInfo );
			//cout << "Overshoot Value: " << overshootfix << endl;
			float cx = bmsg->cx();
			float cy = bmsg->cy();
            Logger::Instance()->WriteMsg( "Behavior", "I want the freaking head to move towards (cx,cy):"+_toString(0.9f*(cx))+_toString(-0.9f*(cy)), Logger::ExtraInfo );
			//cout << "I want the freaking head to move towards (cx,cy):" << 0.9f * (cx) << " " << -0.9f * (cy) << endl;

			if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {
				//Sending command to motion
				mot->set_topic("motion");
				mot->set_command("changeHead");
				mot->set_parameter(0, 0.9f * overshootfix * (cx));
				mot->set_parameter(1, -0.9f * overshootfix * (cy));
				Publisher::publish( mot);
                Logger::Instance()->WriteMsg( "Behavior", "I realy want the freaking head to move towards (cx,cy):"+_toString(0.9f*(cx))+_toString(-0.9f*(cy)), Logger::Info );
				//cout << "I realy want the freaking head to move towards (cx,cy):" << 0.9f * (cx) << " " << -0.9f * (cy) << endl;
			}
            Logger::Instance()->WriteMsg( "Behavior", "Ball Found ole ", Logger::Info );
			//cout << "Ball Found ole " << endl;
			ballfound += 1;
		} else {
			if (ballfound > 0) {
				ballfound -= 1;
			} else {
				ballfound = 0;
			}
		}
		//cout << "At Cx " << (float) memory->getData("kouretes/Ball/cx") << " Cy: " << (float) memory->getData("kouretes/Ball/cy") << endl;

	}

	HeadJointSensorsMessage* hjsm = dynamic_cast<HeadJointSensorsMessage*> (_blk->in_nb("HeadJointSensorsMessage", "Sensors"));
	if (hjsm != 0 && ballfound == 1) { //We have seen a ball for sure and we should walk

		//if (cur->GetTypeName() == "HeadJointSensorsMessage") {
		//cout << "Received HeadJointSensorMessage" << endl;
		//HeadJointSensorsMessage * hjsm = static_cast<HeadJointSensorsMessage*> (cur);
		//cout << "My headjoint  :" << hjsm->sensordata(0).sensorname() << " has value  " << hjsm->sensordata(0).sensorvalue() << endl;
		SensorPair HeadYaw = hjsm->sensordata(0);
		SensorPair HeadPitch = hjsm->sensordata(1);

		float X, Y, theta, freq;
		//Calculate approach
		X = 0;
		Y = 0;
		theta = 0;
		bool readytokick = true;
		//Check max values !
		if (fabs(HeadYaw.sensorvalue()) > 0.06) {
			theta = HeadYaw.sensorvalue() * 0.5 * (0.6 - fabs(HeadPitch.sensorvalue()));
			readytokick = false;
		}
		if (fabs(HeadPitch.sensorvalue()) < 0.487) { //// Auto edw, to poso konta einai stin mpala gia na soutarei
			X = 0.6 * (0.5 - HeadPitch.sensorvalue());
			Y = HeadYaw.sensorvalue() * 0.6 * (1.4 - fabs(HeadYaw.sensorvalue()));
			readytokick = false;
		}

		mot->set_topic("motion");
		if (!readytokick) {
			mot->set_command("setWalkTargetVelocity");

			if (fabs(X) > 1.0)
				X = (X > 0) ? 1 : -1;
			if (fabs(Y) > 1.0)
				Y = (Y > 0) ? 1 : -1;
			if (fabs(theta) > 1.0)
				theta = (theta > 0) ? 1 : -1;
			freq = 1.3 - fabs(HeadPitch.sensorvalue());
			if (fabs(freq) > 1.0)
				freq = (freq > 0) ? 1 : -1;

			mot->set_parameter(0, X);
			mot->set_parameter(1, Y);
			mot->set_parameter(2, theta);
			mot->set_parameter(3, freq);
            Logger::Instance()->WriteMsg( "Behavior", "Sending Walk Command  setWalkTargetVelocity ", Logger::Info );
			//cout << "Sending Walk Command  setWalkTargetVelocity " << endl;

		} else {
            Logger::Instance()->WriteMsg( "Behavior", "Kicking", Logger::Info );
			//cout << "Kicking" << endl;
			if (HeadYaw.sensorvalue() > 0)
				mot->set_command("leftKick");
			else
				mot->set_command("rightKick");
		}
		Publisher::publish( mot);
	}
	//delete cur;
}
