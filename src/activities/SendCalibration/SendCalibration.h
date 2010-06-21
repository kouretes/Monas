
#ifndef _SendCalibration_h_
#define _SendCalibration_h_ 1

#include "architecture/IActivity.h"
#include "messages/VisionObservations.pb.h"

class SendCalibration : public IActivity, public Publisher {

public:

        SendCalibration();

	int Execute ();

	void UserInit ();

	std::string GetName ();

};

#endif // _SendCalibration_h_
