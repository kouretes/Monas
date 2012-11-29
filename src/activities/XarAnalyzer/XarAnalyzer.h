#ifndef XARANALYZER_H
#define XARANALYZER_H

#include "architecture/executables/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"
#include <dirent.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "hal/robot/generic_nao/robot_consts.h"
#include "hal/robot/generic_nao/aldebaran-sensors.h"
#include "hal/robot/generic_nao/aldebaran-motion.h"

#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"


#define JOINTS 22
#define ACTION_TIME 8
#define XarAnalyzerFPS 10

ACTIVITY_START
class XarAnalyzer: public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(XarAnalyzer))
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "XarAnalyzer";
	}
	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void read_messages();

private:

	/* Incoming Messages */
	boost::shared_ptr<const AllSensorValuesMessage> allsm;
	boost::shared_ptr<const GameStateMessage>  gsm;

	/* Outgoing Messages */
	MotionWalkMessage* wmot;
	MotionActionMessage* amot;

	void velocityWalk(double x, double y, double th, double f);
	void createKmeFile();
	float angleStore [ACTION_TIME*XarAnalyzerFPS][JOINTS];
	boost::posix_time::ptime kickTime;
	int flag, temp;
	int gameState;
	bool play;
};

ACTIVITY_END


#endif
