#include "XarAction.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "core/include/Logger.hpp"

using namespace std;

XarAction::XarAction(AL::ALFrameManagerProxy *frame, std::string filepathname, std::string name, std::string identifier)
{
	//	try {
	//		//frame =KAlBroker::Instance().GetBroker()->getSpecialisedProxy(frame,"ALFrameManager");
	//		KAlBroker::Instance().GetBroker()->getProxy("ALFrameManager");
	//		frame = AL::dynamic_ptr_cast<AL::ALFrameManagerProxy>(KAlBroker::Instance().GetBroker()->getProxy("ALFrameManager"));
	//	} catch (AL::ALError& e) {
	//		Logger::Instance().WriteMsg("XARAction", "Error in getting motion proxy", Logger::FatalError);
	//	}
	this->frame = frame;
	this->name = name;
	this->filename = filepathname;
	this->identifier = identifier;
}

int XarAction::ExecutePost()
{
	return ExecuteActionXAR();
}

int XarAction::ExecuteActionXAR()
{
	cout << "playing behavior with id " << this->identifier << endl;
	//TODO find a way to see tha the behavior is running or not
	//if(!frame->isRunning( this->identifier))
	//this->identifier = frame->newBehaviorFromFile(filename, string(""));

	//frame->post.gotoAndPlay(this->identifier,0);
	//else
	try
	{
		frame->playBehavior(this->identifier);
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,"XARManager")
			<< "Error playing this->filename: " << this->filename << e.getDescription();
		//	continue;
	}

	int ret = frame->post.completeBehavior(this->identifier);
	//reload Motion
	this->identifier = frame->newBehaviorFromFile(filename, string(""));
	return ret;
}

//int XarAction::ExecuteActionBodyXAR() {

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

//return motion->post.angleInterpolation("Body", actionAngles, actionTimes, 1);
//}

// void XarAction::PrintActionsXAR() {
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
