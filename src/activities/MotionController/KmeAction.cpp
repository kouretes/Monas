#include "KmeAction.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "hal/robot/generic_nao/robot_consts.h"

using namespace std;

KmeAction::KmeAction(std::string name, AL::ALValue actionNames, AL::ALValue actionAngles, AL::ALValue actionTimes) {
	try {
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("KMEAction", "Error in getting motion proxy", Logger::FatalError);
	}

	try {
		dcm = KAlBroker::Instance().GetBroker()->getDcmProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("KMEAction", "Error in getting dcm proxy", Logger::FatalError);
	}

	this->name = name;
	this->actionNames = actionNames;
	this->actionAngles = actionAngles;
	this->actionTimes = actionTimes;
	DcmInit();
}

void KmeAction::DcmInit() {

	AL::ALValue jointAliasses;
	vector<std::string> PosActuatorStrings = KDeviceLists::getPositionActuatorKeys();

	jointAliasses.arraySetSize(2);
	jointAliasses[0] = std::string("BodyWithoutHead"); // Alias for Body joint actuators without Head joins

	jointAliasses[1].arraySetSize(20);

	// Joints actuator list
	for (int i = KDeviceLists::L_LEG; i < KDeviceLists::NUMOFJOINTS; i++) {
		jointAliasses[1][i - KDeviceLists::L_LEG] = PosActuatorStrings[i];
	}

	// Create alias
	try {
		dcm->createAlias(jointAliasses);
	} catch (const AL::ALError &e) {
		throw ALERROR("KmeAction", "createKmeActuatorAlias()", "Error when creating Alias : " + e.toString());
	}

	// Create Commands
	commands.arraySetSize(6);
	commands[0] = string("BodyWithoutHead");
	commands[1] = string("ClearAll"); // Erase all previous commands
	commands[2] = string("time-separate");
	commands[3] = 0;
	commands[5].arraySetSize(20); // For all joints except head

	for (unsigned int i = 0; i < commands[5].getSize(); i++) {
		commands[5][i].arraySetSize(actionTimes.getSize()); 		//num of poses
		for (unsigned int j = 0; j < commands[5][i].getSize(); j++) {
			commands[5][i][j] = actionAngles[i+2][j];					// actionAngles[joints][poses], commands[joints][poses]

			// Logger::Instance().WriteMsg("KmeACTION", "commands " + _toString(commands[5][i][j]) , Logger::ExtraInfo);
		}
	}
}

AL::ALValue KmeAction::ReturnALValues(){

	return actionAngles;
}

boost::posix_time::ptime KmeAction::ExecuteDCM() {

	try {
		commands[4].arraySetSize(actionTimes.getSize());
		for (unsigned int j = 0; j < commands[4].getSize(); j++) {
			commands[4][j] = dcm->getTime(rint((float)(actionTimes[j]) * 1000));
			//Logger::Instance().WriteMsg("KMEAction", "TIME" + _toString(actionTimes[j]), Logger::FatalError);
			//Logger::Instance().WriteMsg("KMEAction", "TIME" + _toString(commands[4][j]), Logger::FatalError);
		}

		dcm->setAlias(commands);
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("KMEAction", "Error when creating Alias", Logger::FatalError);
	}

	float max_time = actionTimes[(actionTimes.getSize()) - 1];



//	Logger::Instance().WriteMsg("KMEAction", "MAX  TIME : " + _toString(boost::posix_time::seconds(max_time)), Logger::Info);
//	Logger::Instance().WriteMsg("KMEAction", "REAL TIME : " + _toString(boost::posix_time::microsec_clock::universal_time()), Logger::Info);
//	Logger::Instance().WriteMsg("KMEAction", "TIME Command" + _toString(commands[4][commands[4].getSize()-1]), Logger::Info);
//	Logger::Instance().WriteMsg("KMEAction", "TIME Command" + _toString(boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(max_time)), Logger::Info);
	return boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(max_time);
}

boost::posix_time::ptime KmeAction::ExecuteFrameDCM(unsigned int frameStart, unsigned int frameEnd){

	try {
		frameEnd = actionTimes.getSize();
		for (unsigned int i = 0; i < commands[5].getSize(); i++) {
			commands[5][i].clear();
			if(frameEnd != actionTimes.getSize())
				commands[5][i].arraySetSize(frameEnd - frameStart + 1);
			else
				commands[5][i].arraySetSize(actionTimes.getSize() - frameStart + 1);

			for (unsigned int j = 0; j < commands[5][i].getSize(); j++) {
				commands[5][i][j] = actionAngles[i+2][j + frameStart - 1];					// actionAngles[joints][poses], commands[joints][poses]

				// Logger::Instance().WriteMsg("KmeACTION", "commands " + _toString(commands[5][i][j]) , Logger::ExtraInfo);
			}
		}

		if(frameEnd != actionTimes.getSize())
			commands[4].arraySetSize(frameEnd - frameStart + 1);
		else
			commands[4].arraySetSize(actionTimes.getSize() - frameStart + 1);
		for (unsigned int j = 0; j < commands[4].getSize(); j++) {
			commands[4][j] = dcm->getTime(rint((float) (actionTimes[j + frameStart - 1]) * 1000));
		}

		dcm->setAlias(commands);

	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("KMEAction", "Error when creating Alias", Logger::FatalError);
	}

	float max_time = actionTimes[(actionTimes.getSize()) - 1];

	return boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(max_time);

}


int KmeAction::ExecutePost() {
	return ExecuteActionKME();
}

int KmeAction::ExecuteActionKME() {
	Logger::Instance().WriteMsg("KmeAction", "PRIN THN ANGLE INTERPOLATIONNNN", Logger::ExtraInfo);
	return motion->post.angleInterpolation(actionNames, actionAngles, actionTimes, true);
}

int KmeAction::ExecuteActionBodyKME() {

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
