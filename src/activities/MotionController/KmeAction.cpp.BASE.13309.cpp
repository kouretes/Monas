#include "KmeAction.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "hal/robot/generic_nao/robot_consts.h"

using namespace std;

KmeAction::KmeAction(std::string name, AL::ALValue actionNames, AL::ALValue actionAngles, AL::ALValue actionTimes)
{
	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("KMEAction", "Error in getting motion proxy", Logger::FatalError);
	}

	try
	{
		dcm = KAlBroker::Instance().GetBroker()->getDcmProxy();
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("KMEAction", "Error in getting dcm proxy", Logger::FatalError);
	}

	this->name = name;
	this->actionNames = actionNames;
	this->actionAngles = actionAngles;
	this->actionTimes = actionTimes;
	DcmInit();
}

void KmeAction::DcmInit()
{
	AL::ALValue jointAliasses;
	vector<std::string> PosActuatorStrings = KDeviceLists::getPositionActuatorKeys();
	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("BodyWithoutHead"); // Alias for Body joint actuators without Head joins
	jointAliasses[1].arraySetSize(20);

	// Joints actuator list
	for (int i = KDeviceLists::L_ARM; i < KDeviceLists::NUMOFJOINTS; i++)
	{
		jointAliasses[1][i - KDeviceLists::L_ARM] = PosActuatorStrings[i];
	}

	// Create alias
	try
	{
		dcm->createAlias(jointAliasses);
	}
	catch (const AL::ALError &e)
	{
		throw ALERROR("KmeAction", "createKmeActuatorAlias()", "Error when creating Alias : " + e.toString());
	}

	// Create Commands
	commands.arraySetSize(6);
	commands[0] = string("BodyWithoutHead");
	commands[1] = string("ClearAll"); // Erase all previous commands
	commands[2] = string("time-separate");
	commands[3] = 0;
	commands[5].arraySetSize(20); // For all joints except head

	for (unsigned int i = 0; i < commands[5].getSize(); i++)
	{
		commands[5][i].arraySetSize(actionTimes[0].getSize()); 		//num of poses

		for (unsigned int j = 0; j < commands[5][i].getSize(); j++)
		{
			commands[5][i][j] = actionAngles[i + 2][j];					// actionAngles[joints][poses], commands[5][joints][poses]
			// Logger::Instance().WriteMsg("KmeACTION", "commands " + _toString(commands[5][i][j]) , Logger::ExtraInfo);
		}
	}
}

AL::ALValue KmeAction::ReturnALValues()
{
	return actionAngles;
}

boost::posix_time::ptime KmeAction::ExecuteDCM()
{
	try
	{
		int DcmAt = dcm->getTime(0);
		commands[4].clear();
		commands[4].arraySetSize(actionTimes[0].getSize());

		for (unsigned int j = 0; j < commands[4].getSize(); j++)
		{
			//commands[4][j] = dcm->getTime((int) rint((double) (actionTimes[0][j]) * 1000));
			commands[4][j] = DcmAt + ((int) rint((double) (actionTimes[0][j]) * 1000));
		}

		dcm->setAlias(commands);
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("KMEAction", "Error when creating Alias", Logger::FatalError);
	}

	float max_time = actionTimes[0][(actionTimes[0].getSize()) - 1];
	return boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(max_time);
}

boost::posix_time::ptime KmeAction::ExecuteFrameDCM(unsigned int frameStart, unsigned int frameEnd)
{
	if(frameEnd < frameStart)
		return boost::posix_time::microsec_clock::universal_time();

	if(frameEnd > actionTimes[0].getSize() - 1)
		frameEnd = actionTimes[0].getSize() - 1;

	for (unsigned int i = 0; i < commands[5].getSize(); i++)
	{
		commands[5][i].clear();
		commands[5][i].arraySetSize(frameEnd - frameStart + 1);

		for (unsigned int j = 0; j < commands[5][i].getSize(); j++)
		{
			commands[5][i][j] = actionAngles[i + 2][j + frameStart];					// actionAngles[joints][poses], commands[5][joints][poses]
		}
	}

	int offset;
	float max_time;
	int delay;

	if (frameStart == 0)
	{
		offset = 0;
		delay = 0;
		max_time = (float)actionTimes[0][frameEnd];
	}
	else
	{
		offset = (int) rint((double) (actionTimes[0][frameStart - 1]) * 1000);
		delay = 200;
		max_time = (float)actionTimes[0][frameEnd] - (float)actionTimes[0][frameStart - 1] + delay / 1000.0;
	}

	commands[4].clear();
	commands[4].arraySetSize(frameEnd - frameStart + 1);
	int DcmAt = dcm->getTime(0) + delay;

	for (unsigned int j = 0; j < commands[4].getSize(); j++)
	{
		commands[4][j] = DcmAt + (((int) rint((double) (actionTimes[0][j + frameStart]) * 1000)) - offset);
	}

	try
	{
		dcm->setAlias(commands);
	}
	catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg("KMEAction", "Error when creating Alias", Logger::FatalError);
	}

	return boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(max_time);
}


int KmeAction::ExecutePost()
{
	return ExecuteActionKME();
}

int KmeAction::ExecuteActionKME()
{
	Logger::Instance().WriteMsg("KmeAction", "PRIN THN ANGLE INTERPOLATIONNNN", Logger::ExtraInfo);
	return motion->post.angleInterpolation(actionNames, actionAngles, actionTimes, true);
}

int KmeAction::ExecuteActionBodyKME()
{
	//   AL::ALValue actionAngles, actionTimes;
	//
	//   unsigned int poses = spAct[i].seqMotion.size();
	//   actionAngles.arraySetSize(poses);
	//   actionTimes.arraySetSize(poses);
	//
	//   unsigned int joints = 22;
	//   for (unsigned int k = 0; k < poses; k++)
	//     actionAngles[k].arraySetSize(joints);
	//
	//   float time = 0.0;
	//   for (unsigned int k = 0; k < poses; k++) {
	//     for (unsigned int l = 0; l < joints; l++) {
	//       actionAngles[k][l] = spAct[i].seqMotion[k][l];
	//     }
	//     time += spAct[i].seqMotion[k][22];
	//     actionTimes[k] = time;
	//   }
	return motion->post.angleInterpolation("Body", actionAngles, actionTimes, 1);
}

// void KmeAction::PrintActionsKME() {
//
//   for(unsigned int v =0;v<spAct.size();v++){
//     cout<<"\n************\tSPECIAL ACTION : "<<spAct[v].seqName<<" Index: " << actionMap[spAct[v].seqName] <<"\t************"<<endl<<endl;
//     for(unsigned int i = 0; i< spAct[v].seqMotion.size();i++){
//       for(unsigned int j = 0;j<spAct[v].seqMotion[i].size();j++){
//         cout<<spAct[v].seqMotion[i][j]<<" ";
//       }
//       cout<<endl;
//     }
//     cout<<endl<<endl;
//   }
// }
