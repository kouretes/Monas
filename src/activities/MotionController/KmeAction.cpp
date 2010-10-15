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

  this->name = name;
  this->actionNames = actionNames;
  this->actionAngles = actionAngles;
  this->actionTimes = actionTimes;

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
