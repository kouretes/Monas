#include "KmeAction.h"

#include "hal/robot/generic_nao/kAlBroker.h"

using namespace std;

KmeAction::KmeAction(std::string name, AL::ALValue actionNames,
                     AL::ALValue actionAngles, AL::ALValue actionTimes ) {
  try {
    motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
  } catch (AL::ALError& e) {
    Logger::Instance().WriteMsg("KMEAction","Error in getting motion proxy",Logger::FatalError);
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
  
	try{
		//DCMProxy* dcm = new DCMProxy(pBroker);
		//DCMProxy(*dcm pBroker);


		commands.arraySetSize(4);
		commands[0] = string("BodyWithoutHead");
		commands[1] = string("ClearAll");
		commands[2] = string("time-separate");
		commands[3] = 0;
		commands[5].arraySetSize(22);		//22 h actionAngles.getSize();;;;;;;;;;;;;;;;;


		for(int i = 0; i<commands[5].getSize(); i++){
			commands[5][i].arraySetSize(actionTimes[i].getSize());	//getSize ???pairnoume to sunolo apo pozes h tpt se sec ???
			for(int j = 0; j<commands[5][i].getSize(); j++){
				commands[5][i][j] = actionAngles[i][j];		//prob afou de 8eloume to head mhpos prepei na ksekiname apo 3-4 kai meta???to loop....
			}
		}

	} catch (AL::ALError& e) {
			Logger::Instance().WriteMsg("KMEAction","Error in getting motion proxy",Logger::FatalError);
	}

}

boost::posix_time::ptime KmeAction::ExecuteDCM() {

 	try{
		for(int i = 0; i<commands[5].getSize(); i++){
			commands[4].arraySetSize(actionTimes[i].getSize());
			for(int j = 0; j<commands[4].getSize(); j++){
				commands[4][j] = dcm->getTime(round((float)actionTimes[i][j])*1000);
			}
		}

		dcm->setAlias(commands);

	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("KMEAction","Error in getting motion proxy",Logger::FatalError);
	}
	
	float max_time = actionTimes[0][(actionTimes[0].getSize())-1];
	
	return boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(max_time);
}



int KmeAction::ExecutePost() {
  return ExecuteActionKME();
}



int KmeAction::ExecuteActionKME() {
  return motion->post.angleInterpolation(actionNames, actionAngles, actionTimes, 1);
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
