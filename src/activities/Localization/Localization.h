#ifndef Localization_H
#define Localization_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"

class Localization: public IActivity, public Publisher {

	public:
		Localization();
		int Execute();
		void UserInit();
		void read_messages();

		std::string GetName() {
			return "Localization";
		}

	private:

		SensorPair HeadYaw;
		SensorPair HeadPitch;
		BallTrackMessage lastballseen;
		HeadJointSensorsMessage* hjsm;
		BallTrackMessage* bmsg;
		GameStateMessage* gsm;

};

#endif
