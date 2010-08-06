#include "MotionController.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include <vector>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
using namespace std;

void MotionController::loadActionsKME() {
	
	ifstream fin;
	string pose_str, ending = ".kme";
	string fileName;
	DIR * t =NULL;
	motSequence tempMotSeq;
	vector<float> row;
	unsigned int pos = 0,i = 0 ;
	int k;
	string tempStr;	
	
	t = opendir((ArchConfig::Instance().GetConfigPrefix()+"kme").c_str());
	
	struct dirent * files = readdir(t);

	while(files!=NULL)
	{	
		fileName = files->d_name;
		
		k = fileName.size()>4?fileName.compare( fileName.size() - 4 , 4, ending, 0 ,4):-1;
		if( k==0 ){
			spAct.push_back(tempMotSeq);
			fin.open((ArchConfig::Instance().GetConfigPrefix()+"kme/"+fileName).c_str(),ios::in);
			
			if ( fin.is_open() ){

				while(!(fin>>pose_str).eof()){
					
					spAct[i].seqName = fileName;
					pos = pose_str.find_first_of("%");
					pose_str.erase(0,pos+1);
					pos = pose_str.find_first_of("%");
					while( pos!= string::npos){
						tempStr = pose_str.substr(0,pos);
						row.push_back(atof(tempStr.c_str()));
						pose_str.erase(0,pos+1);
						pos = pose_str.find_first_of("%",1);
					}				
					tempStr = pose_str;
					row.push_back(atof(tempStr.c_str()));
					spAct[i].seqMotion.push_back(row);
					row.clear();
				}
		
				fin.close();
				actionMap[fileName] = i;
				Logger::Instance().WriteMsg("MotionController","Special Action Succesfully Loaded : "+fileName,Logger::ExtraInfo);
				i++;
			}
			else{
				Logger::Instance().WriteMsg("MotionController","Special Action Failed to Load : "+fileName,Logger::ExtraInfo);
			}
		}

		files = readdir(t);
	}		
}

void MotionController::printActionsKME() {

	for(unsigned int v =0;v<spAct.size();v++){
		cout<<"\n************\tSPECIAL ACTION : "<<spAct[v].seqName<<" Index: " << actionMap[spAct[v].seqName] <<"\t************"<<endl<<endl;
		for(unsigned int i = 0; i< spAct[v].seqMotion.size();i++){
			for(unsigned int j = 0;j<spAct[v].seqMotion[i].size();j++){
				cout<<spAct[v].seqMotion[i][j]<<" ";
			}
			cout<<endl;
		}
	cout<<endl<<endl;	
	}
}


int MotionController::executeActionKME(std::string action) {

	map<string,int>::iterator it = actionMap.find(action);
	if ( it == actionMap.end() ) {
		Logger::Instance().WriteMsg("MotionController","Special Action Not Found : "+action,Logger::ExtraInfo);
		return 0;
	}
	unsigned int i = it->second; 
	Logger::Instance().WriteMsg("MotionController","Special Action Found : "+action,Logger::ExtraInfo);
	
	AL::ALValue actionNames, actionAngles, actionTimes;
	unsigned int joints = 22;
	actionNames.arraySetSize(joints);
	actionAngles.arraySetSize(joints);
	actionTimes.arraySetSize(joints);
	
	unsigned int poses = spAct[i].seqMotion.size();
	for (unsigned int l = 0; l < joints; l++) {
		actionAngles[l].arraySetSize(poses);
		actionTimes[l].arraySetSize(poses);
	}
	
	for (unsigned int l = 0; l < joints; l++) {
		float time = 0.0;
		actionNames[l] = jointNames[l];
		for (unsigned int k = 0; k < poses; k++) {
			actionAngles[l][k] = spAct[i].seqMotion[k][l];
			time += spAct[i].seqMotion[k][22];
			actionTimes[l][k] = time;
		}
	}
	
	return motion->post.angleInterpolation(actionNames, actionAngles, actionTimes, 1);
}




int MotionController::executeActionBodyKME(std::string action) {

	map<string,int>::iterator it = actionMap.find(action);
	if ( it == actionMap.end() ) {
		Logger::Instance().WriteMsg("MotionController","Special Action Not Found : "+action,Logger::ExtraInfo);
		return 0;
	}
	unsigned int i = it->second; 
	Logger::Instance().WriteMsg("MotionController","Special Action Found : "+action,Logger::ExtraInfo);
	
	AL::ALValue actionAngles, actionTimes;
	
	unsigned int poses = spAct[i].seqMotion.size();
	actionAngles.arraySetSize(poses);
	actionTimes.arraySetSize(poses);
	
	unsigned int joints = 22;
	for (unsigned int k = 0; k < poses; k++) 
		actionAngles[k].arraySetSize(joints);
	
	float time = 0.0;
	for (unsigned int k = 0; k < poses; k++) {
		for (unsigned int l = 0; l < joints; l++) {
			actionAngles[k][l] = spAct[i].seqMotion[k][l];
		}
		time += spAct[i].seqMotion[k][22];
		actionTimes[k] = time;
	}
	
	return motion->post.angleInterpolation("Body", actionAngles, actionTimes, 1);
}



void MotionController::ALstandUp() {

#ifdef WEBOTS
	if (AccXvalue > 1.0) { // Webots
#else
	if (AccXvalue < 0.0) { // Robot
#endif
		//tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Up!"));
		ALstandUpBack2010();
		Logger::Instance().WriteMsg("MotionController", "Stand Up 2010: From Back", Logger::ExtraInfo);	
    }
#ifdef WEBOTS
	else if (AccXvalue < -1.0) { // Webots
#else
	else if (AccXvalue > 0.0) { // Robot
#endif
//		tts->pCall<AL::ALValue>(std::string("say"), std::string("Face Down!"));
		ALstandUpFront2009();
		Logger::Instance().WriteMsg("MotionController", "Stand Up 2009: From Front", Logger::ExtraInfo);	
    }
	return;
}

void MotionController::ALstandUpFront2009() {

	AL::ALValue jointCodes, angles, times;
	jointCodes.arraySetSize(21);
	angles.arraySetSize(21);
	times.arraySetSize(21);

	jointCodes[0] = "HeadPitch";
	angles[0].arraySetSize(6);
	times[0].arraySetSize(6);
	angles[0][0] = -0.57596f;
	times[0][0] = 1.40000f;
	angles[0][1] = 0.00000f;
	times[0][1] = 2.40000f;
	angles[0][2] = -0.34907f;
	times[0][2] = 3.70000f;
	angles[0][3] = -0.48869f;
	times[0][3] = 5.20000f;
	angles[0][4] = 0.00000f;
	times[0][4] = 6.20000f;
	angles[0][5] = 0.27925f;
	times[0][5] = 8.40000f;

	jointCodes[1] = "LAnklePitch";
	angles[1].arraySetSize(8);
	times[1].arraySetSize(8);
	angles[1][0] = -1.13446f;
	times[1][0] = 1.40000f;
	angles[1][1] = -1.13446f;
	times[1][1] = 2.40000f;
	angles[1][2] = -0.78365f;
	times[1][2] = 3.70000f;
	angles[1][3] = 0.08727f;
	times[1][3] = 4.40000f;
	angles[1][4] = -0.31241f;
	times[1][4] = 5.20000f;
	angles[1][5] = -0.71558f;
	times[1][5] = 6.20000f;
	angles[1][6] = -1.04720f;
	times[1][6] = 7.40000f;
	angles[1][7] = -0.17453f;
	times[1][7] = 8.40000f;

	jointCodes[2] = "LAnkleRoll";
	angles[2].arraySetSize(8);
	times[2].arraySetSize(8);
	angles[2][0] = 0.00000f;
	times[2][0] = 1.40000f;
	angles[2][1] = 0.00000f;
	times[2][1] = 2.40000f;
	angles[2][2] = -0.68068f;
	times[2][2] = 3.70000f;
	angles[2][3] = -0.55501f;
	times[2][3] = 4.40000f;
	angles[2][4] = -0.29671f;
	times[2][4] = 5.20000f;
	angles[2][5] = -0.10472f;
	times[2][5] = 6.20000f;
	angles[2][6] = 0.00000f;
	times[2][6] = 7.40000f;
	angles[2][7] = 0.00000f;
	times[2][7] = 8.40000f;

	jointCodes[3] = "LElbowRoll";
	angles[3].arraySetSize(8);
	times[3].arraySetSize(8);
	angles[3][0] = 0.00000f;
	times[3][0] = 1.40000f;
	angles[3][1] = -0.61087f;
	times[3][1] = 2.40000f;
	angles[3][2] = -1.65806f;
	times[3][2] = 3.70000f;
	angles[3][3] = -0.13963f;
	times[3][3] = 4.40000f;
	angles[3][4] = -0.71558f;
	times[3][4] = 5.20000f;
	angles[3][5] = -1.29154f;
	times[3][5] = 6.20000f;
	angles[3][6] = -1.39626f;
	times[3][6] = 7.40000f;
	angles[3][7] = -1.25664f;
	times[3][7] = 8.40000f;

	jointCodes[4] = "LElbowYaw";
	angles[4].arraySetSize(8);
	times[4].arraySetSize(8);
	angles[4][0] = -1.57080f;
	times[4][0] = 1.40000f;
	angles[4][1] = -1.57080f;
	times[4][1] = 2.40000f;
	angles[4][2] = -1.57080f;
	times[4][2] = 3.70000f;
	angles[4][3] = -1.57080f;
	times[4][3] = 4.40000f;
	angles[4][4] = -0.24435f;
	times[4][4] = 5.20000f;
	angles[4][5] = -0.92502f;
	times[4][5] = 6.20000f;
	angles[4][6] = -1.57080f;
	times[4][6] = 7.40000f;
	angles[4][7] = -1.23918f;
	times[4][7] = 8.40000f;

	jointCodes[5] = "LHipPitch";
	angles[5].arraySetSize(8);
	times[5].arraySetSize(8);
	angles[5][0] = 0.00000f;
	times[5][0] = 1.40000f;
	angles[5][1] = -1.57080f;
	times[5][1] = 2.40000f;
	angles[5][2] = -1.57080f;
	times[5][2] = 3.70000f;
	angles[5][3] = -1.57080f;
	times[5][3] = 4.40000f;
	angles[5][4] = -1.57080f;
	times[5][4] = 5.20000f;
	angles[5][5] = -1.06989f;
	times[5][5] = 6.20000f;
	angles[5][6] = -1.04720f;
	times[5][6] = 7.40000f;
	angles[5][7] = -0.17453f;
	times[5][7] = 8.40000f;

	jointCodes[6] = "LHipRoll";
	angles[6].arraySetSize(8);
	times[6].arraySetSize(8);
	angles[6][0] = 0.00000f;
	times[6][0] = 1.40000f;
	angles[6][1] = 0.00000f;
	times[6][1] = 2.40000f;
	angles[6][2] = 0.00000f;
	times[6][2] = 3.70000f;
	angles[6][3] = 0.00000f;
	times[6][3] = 4.40000f;
	angles[6][4] = 0.08727f;
	times[6][4] = 5.20000f;
	angles[6][5] = 0.10472f;
	times[6][5] = 6.20000f;
	angles[6][6] = -0.01047f;
	times[6][6] = 7.40000f;
	angles[6][7] = -0.01047f;
	times[6][7] = 8.40000f;

	jointCodes[7] = "LHipYawPitch";
	angles[7].arraySetSize(8);
	times[7].arraySetSize(8);
	angles[7][0] = 0.00000f;
	times[7][0] = 1.40000f;
	angles[7][1] = 0.00000f;
	times[7][1] = 2.40000f;
	angles[7][2] = -0.87266f;
	times[7][2] = 3.70000f;
	angles[7][3] = -0.87266f;
	times[7][3] = 4.40000f;
	angles[7][4] = -0.96517f;
	times[7][4] = 5.20000f;
	angles[7][5] = -0.78540f;
	times[7][5] = 6.20000f;
	angles[7][6] = 0.00000f;
	times[7][6] = 7.40000f;
	angles[7][7] = 0.00000f;
	times[7][7] = 8.40000f;

	jointCodes[8] = "LKneePitch";
	angles[8].arraySetSize(8);
	times[8].arraySetSize(8);
	angles[8][0] = 2.09440f;
	times[8][0] = 1.40000f;
	angles[8][1] = 2.09440f;
	times[8][1] = 2.40000f;
	angles[8][2] = 1.04720f;
	times[8][2] = 3.70000f;
	angles[8][3] = 1.01229f;
	times[8][3] = 4.40000f;
	angles[8][4] = 2.15548f;
	times[8][4] = 5.20000f;
	angles[8][5] = 2.16421f;
	times[8][5] = 6.20000f;
	angles[8][6] = 2.09440f;
	times[8][6] = 7.40000f;
	angles[8][7] = 0.34907f;
	times[8][7] = 8.40000f;

	jointCodes[9] = "LShoulderPitch";
	angles[9].arraySetSize(8);
	times[9].arraySetSize(8);
	angles[9][0] = -1.57080f;
	times[9][0] = 1.40000f;
	angles[9][1] = -0.87266f;
	times[9][1] = 2.40000f;
	angles[9][2] = -0.17453f;
	times[9][2] = 3.70000f;
	angles[9][3] = 0.00000f;
	times[9][3] = 4.40000f;
	angles[9][4] = 0.61087f;
	times[9][4] = 5.20000f;
	angles[9][5] = 1.11701f;
	times[9][5] = 6.20000f;
	angles[9][6] = 1.62316f;
	times[9][6] = 7.40000f;
	angles[9][7] = 1.83260f;
	times[9][7] = 8.40000f;

	jointCodes[10] = "LShoulderRoll";
	angles[10].arraySetSize(8);
	times[10].arraySetSize(8);
	angles[10][0] = 0.00000f;
	times[10][0] = 1.40000f;
	angles[10][1] = 0.00000f;
	times[10][1] = 2.40000f;
	angles[10][2] = 0.00000f;
	times[10][2] = 3.70000f;
	angles[10][3] = 0.00000f;
	times[10][3] = 4.40000f;
	angles[10][4] = 0.03491f;
	times[10][4] = 5.20000f;
	angles[10][5] = 0.13090f;
	times[10][5] = 6.20000f;
	angles[10][6] = 0.17453f;
	times[10][6] = 7.40000f;
	angles[10][7] = 0.19199f;
	times[10][7] = 8.40000f;

	jointCodes[11] = "RAnklePitch";
	angles[11].arraySetSize(8);
	times[11].arraySetSize(8);
	angles[11][0] = -1.13446f;
	times[11][0] = 1.40000f;
	angles[11][1] = -1.13446f;
	times[11][1] = 2.40000f;
	angles[11][2] = -0.78365f;
	times[11][2] = 3.70000f;
	angles[11][3] = 0.08727f;
	times[11][3] = 4.40000f;
	angles[11][4] = -0.31241f;
	times[11][4] = 5.20000f;
	angles[11][5] = -0.71558f;
	times[11][5] = 6.20000f;
	angles[11][6] = -1.04720f;
	times[11][6] = 7.40000f;
	angles[11][7] = -0.17453f;
	times[11][7] = 8.40000f;

	jointCodes[12] = "RAnkleRoll";
	angles[12].arraySetSize(8);
	times[12].arraySetSize(8);
	angles[12][0] = 0.00000f;
	times[12][0] = 1.40000f;
	angles[12][1] = 0.00000f;
	times[12][1] = 2.40000f;
	angles[12][2] = 0.68068f;
	times[12][2] = 3.70000f;
	angles[12][3] = 0.55501f;
	times[12][3] = 4.40000f;
	angles[12][4] = 0.29671f;
	times[12][4] = 5.20000f;
	angles[12][5] = 0.10472f;
	times[12][5] = 6.20000f;
	angles[12][6] = 0.00000f;
	times[12][6] = 7.40000f;
	angles[12][7] = 0.00000f;
	times[12][7] = 8.40000f;

	jointCodes[13] = "RElbowRoll";
	angles[13].arraySetSize(8);
	times[13].arraySetSize(8);
	angles[13][0] = 0.00000f;
	times[13][0] = 1.40000f;
	angles[13][1] = 0.61087f;
	times[13][1] = 2.40000f;
	angles[13][2] = 1.65806f;
	times[13][2] = 3.70000f;
	angles[13][3] = 0.13963f;
	times[13][3] = 4.40000f;
	angles[13][4] = 0.71558f;
	times[13][4] = 5.20000f;
	angles[13][5] = 1.29154f;
	times[13][5] = 6.20000f;
	angles[13][6] = 1.39626f;
	times[13][6] = 7.40000f;
	angles[13][7] = 1.25664f;
	times[13][7] = 8.40000f;

	jointCodes[14] = "RElbowYaw";
	angles[14].arraySetSize(8);
	times[14].arraySetSize(8);
	angles[14][0] = 1.57080f;
	times[14][0] = 1.40000f;
	angles[14][1] = 1.57080f;
	times[14][1] = 2.40000f;
	angles[14][2] = 1.57080f;
	times[14][2] = 3.70000f;
	angles[14][3] = 1.57080f;
	times[14][3] = 4.40000f;
	angles[14][4] = 0.24435f;
	times[14][4] = 5.20000f;
	angles[14][5] = 0.92502f;
	times[14][5] = 6.20000f;
	angles[14][6] = 1.57080f;
	times[14][6] = 7.40000f;
	angles[14][7] = 1.23918f;
	times[14][7] = 8.40000f;

	jointCodes[15] = "RHipPitch";
	angles[15].arraySetSize(8);
	times[15].arraySetSize(8);
	angles[15][0] = 0.00000f;
	times[15][0] = 1.40000f;
	angles[15][1] = -1.57080f;
	times[15][1] = 2.40000f;
	angles[15][2] = -1.57080f;
	times[15][2] = 3.70000f;
	angles[15][3] = -1.57080f;
	times[15][3] = 4.40000f;
	angles[15][4] = -1.57080f;
	times[15][4] = 5.20000f;
	angles[15][5] = -1.06989f;
	times[15][5] = 6.20000f;
	angles[15][6] = -1.04720f;
	times[15][6] = 7.40000f;
	angles[15][7] = -0.17453f;
	times[15][7] = 8.40000f;

	jointCodes[16] = "RHipRoll";
	angles[16].arraySetSize(8);
	times[16].arraySetSize(8);
	angles[16][0] = 0.00000f;
	times[16][0] = 1.40000f;
	angles[16][1] = 0.00000f;
	times[16][1] = 2.40000f;
	angles[16][2] = 0.00000f;
	times[16][2] = 3.70000f;
	angles[16][3] = 0.00000f;
	times[16][3] = 4.40000f;
	angles[16][4] = -0.08727f;
	times[16][4] = 5.20000f;
	angles[16][5] = -0.10472f;
	times[16][5] = 6.20000f;
	angles[16][6] = 0.01047f;
	times[16][6] = 7.40000f;
	angles[16][7] = 0.01047f;
	times[16][7] = 8.40000f;

	jointCodes[17] = "RHipYawPitch";
	angles[17].arraySetSize(8);
	times[17].arraySetSize(8);
	angles[17][0] = 0.00000f;
	times[17][0] = 1.40000f;
	angles[17][1] = 0.00000f;
	times[17][1] = 2.40000f;
	angles[17][2] = -0.87266f;
	times[17][2] = 3.70000f;
	angles[17][3] = -0.87266f;
	times[17][3] = 4.40000f;
	angles[17][4] = -0.96517f;
	times[17][4] = 5.20000f;
	angles[17][5] = -0.78540f;
	times[17][5] = 6.20000f;
	angles[17][6] = 0.00000f;
	times[17][6] = 7.40000f;
	angles[17][7] = 0.00000f;
	times[17][7] = 8.40000f;

	jointCodes[18] = "RKneePitch";
	angles[18].arraySetSize(8);
	times[18].arraySetSize(8);
	angles[18][0] = 2.09440f;
	times[18][0] = 1.40000f;
	angles[18][1] = 2.09440f;
	times[18][1] = 2.40000f;
	angles[18][2] = 1.04720f;
	times[18][2] = 3.70000f;
	angles[18][3] = 1.01229f;
	times[18][3] = 4.40000f;
	angles[18][4] = 2.15548f;
	times[18][4] = 5.20000f;
	angles[18][5] = 2.16421f;
	times[18][5] = 6.20000f;
	angles[18][6] = 2.09440f;
	times[18][6] = 7.40000f;
	angles[18][7] = 0.34907f;
	times[18][7] = 8.40000f;

	jointCodes[19] = "RShoulderPitch";
	angles[19].arraySetSize(8);
	times[19].arraySetSize(8);
	angles[19][0] = -1.57080f;
	times[19][0] = 1.40000f;
	angles[19][1] = -0.87266f;
	times[19][1] = 2.40000f;
	angles[19][2] = -0.17453f;
	times[19][2] = 3.70000f;
	angles[19][3] = 0.00000f;
	times[19][3] = 4.40000f;
	angles[19][4] = 0.61087f;
	times[19][4] = 5.20000f;
	angles[19][5] = 1.11701f;
	times[19][5] = 6.20000f;
	angles[19][6] = 1.62316f;
	times[19][6] = 7.40000f;
	angles[19][7] = 1.83260f;
	times[19][7] = 8.40000f;

	jointCodes[20] = "RShoulderRoll";
	angles[20].arraySetSize(8);
	times[20].arraySetSize(8);
	angles[20][0] = 0.00000f;
	times[20][0] = 1.40000f;
	angles[20][1] = 0.00000f;
	times[20][1] = 2.40000f;
	angles[20][2] = 0.00000f;
	times[20][2] = 3.70000f;
	angles[20][3] = 0.00000f;
	times[20][3] = 4.40000f;
	angles[20][4] = -0.03491f;
	times[20][4] = 5.20000f;
	angles[20][5] = -0.13090f;
	times[20][5] = 6.20000f;
	angles[20][6] = -0.17453f;
	times[20][6] = 7.40000f;
	angles[20][7] = -0.19199f;
	times[20][7] = 8.40000f;

	actionPID = motion->post.angleInterpolation(jointCodes, angles, times, 1);

}

void MotionController::ALstandUpBack2009() {

	AL::ALValue jointCodes, angles, times;
	jointCodes.arraySetSize(22);
	angles.arraySetSize(22);
	times.arraySetSize(22);

	jointCodes[0] = "HeadPitch";
	angles[0].arraySetSize(11);
	times[0].arraySetSize(11);
	angles[0][0] = 0.00000f;
	times[0][0] = 0.90000f;
	angles[0][1] = 0.00000f;
	times[0][1] = 1.90000f;
	angles[0][2] = -0.78540f;
	times[0][2] = 2.70000f;
	angles[0][3] = 0.00000f;
	times[0][3] = 3.40000f;
	angles[0][4] = 0.34907f;
	times[0][4] = 3.90000f;
	angles[0][5] = 0.32823f;
	times[0][5] = 4.90000f;
	angles[0][6] = 0.33129f;
	times[0][6] = 5.80000f;
	angles[0][7] = 0.37886f;
	times[0][7] = 6.80000f;
	angles[0][8] = 0.37886f;
	times[0][8] = 7.30000f;
	angles[0][9] = 0.37886f;
	times[0][9] = 8.40000f;
	angles[0][10] = 0.27925f;
	times[0][10] = 9.40000f;

	jointCodes[1] = "HeadYaw";
	angles[1].arraySetSize(10);
	times[1].arraySetSize(10);
	angles[1][0] = 0.00000f;
	times[1][0] = 0.90000f;
	angles[1][1] = -0.00000f;
	times[1][1] = 1.90000f;
	angles[1][2] = 0.00000f;
	times[1][2] = 2.70000f;
	angles[1][3] = 0.00000f;
	times[1][3] = 3.40000f;
	angles[1][4] = 0.00000f;
	times[1][4] = 3.90000f;
	angles[1][5] = -0.00744f;
	times[1][5] = 4.90000f;
	angles[1][6] = -0.01070f;
	times[1][6] = 5.80000f;
	angles[1][7] = -0.00940f;
	times[1][7] = 6.80000f;
	angles[1][8] = -0.00940f;
	times[1][8] = 7.30000f;
	angles[1][9] = -0.00940f;
	times[1][9] = 8.40000f;

	jointCodes[2] = "LAnklePitch";
	angles[2].arraySetSize(11);
	times[2].arraySetSize(11);
	angles[2][0] = 0.00000f;
	times[2][0] = 0.90000f;
	angles[2][1] = 0.24435f;
	times[2][1] = 1.90000f;
	angles[2][2] = 0.24435f;
	times[2][2] = 2.70000f;
	angles[2][3] = 0.24435f;
	times[2][3] = 3.40000f;
	angles[2][4] = 0.78540f;
	times[2][4] = 3.90000f;
	angles[2][5] = -0.57065f;
	times[2][5] = 4.90000f;
	angles[2][6] = -1.22173f;
	times[2][6] = 5.80000f;
	angles[2][7] = -1.22173f;
	times[2][7] = 6.80000f;
	angles[2][8] = -1.22173f;
	times[2][8] = 7.30000f;
	angles[2][9] = -1.22173f;
	times[2][9] = 8.40000f;
	angles[2][10] = -0.17453f;
	times[2][10] = 9.40000f;

	jointCodes[3] = "LAnkleRoll";
	angles[3].arraySetSize(11);
	times[3].arraySetSize(11);
	angles[3][0] = 0.00000f;
	times[3][0] = 0.90000f;
	angles[3][1] = 0.00000f;
	times[3][1] = 1.90000f;
	angles[3][2] = 0.00000f;
	times[3][2] = 2.70000f;
	angles[3][3] = 0.00000f;
	times[3][3] = 3.40000f;
	angles[3][4] = 0.00000f;
	times[3][4] = 3.90000f;
	angles[3][5] = -0.39573f;
	times[3][5] = 4.90000f;
	angles[3][6] = -0.10379f;
	times[3][6] = 5.80000f;
	angles[3][7] = 0.11810f;
	times[3][7] = 6.80000f;
	angles[3][8] = 0.08727f;
	times[3][8] = 7.30000f;
	angles[3][9] = 0.00000f;
	times[3][9] = 8.40000f;
	angles[3][10] = 0.00000f;
	times[3][10] = 9.40000f;

	jointCodes[4] = "LElbowRoll";
	angles[4].arraySetSize(11);
	times[4].arraySetSize(11);
	angles[4][0] = 0.00000f;
	times[4][0] = 0.90000f;
	angles[4][1] = 0.00000f;
	times[4][1] = 1.90000f;
	angles[4][2] = -1.65806f;
	times[4][2] = 2.70000f;
	angles[4][3] = -0.69813f;
	times[4][3] = 3.40000f;
	angles[4][4] = 0.00000f;
	times[4][4] = 3.90000f;
	angles[4][5] = -0.48869f;
	times[4][5] = 4.90000f;
	angles[4][6] = -0.82372f;
	times[4][6] = 5.80000f;
	angles[4][7] = -0.80535f;
	times[4][7] = 6.80000f;
	angles[4][8] = -0.80535f;
	times[4][8] = 7.30000f;
	angles[4][9] = -1.13446f;
	times[4][9] = 8.40000f;
	angles[4][10] = -1.25664f;
	times[4][10] = 9.40000f;

	jointCodes[5] = "LElbowYaw";
	angles[5].arraySetSize(11);
	times[5].arraySetSize(11);
	angles[5][0] = 0.00000f;
	times[5][0] = 0.90000f;
	angles[5][1] = 0.15708f;
	times[5][1] = 1.90000f;
	angles[5][2] = 0.08727f;
	times[5][2] = 2.70000f;
	angles[5][3] = 0.08727f;
	times[5][3] = 3.40000f;
	angles[5][4] = 0.08727f;
	times[5][4] = 3.90000f;
	angles[5][5] = 0.08295f;
	times[5][5] = 4.90000f;
	angles[5][6] = 0.09445f;
	times[5][6] = 5.80000f;
	angles[5][7] = 0.08308f;
	times[5][7] = 6.80000f;
	angles[5][8] = 0.08308f;
	times[5][8] = 7.30000f;
	angles[5][9] = -1.25664f;
	times[5][9] = 8.40000f;
	angles[5][10] = -1.23918f;
	times[5][10] = 9.40000f;

	jointCodes[6] = "LHipPitch";
	angles[6].arraySetSize(11);
	times[6].arraySetSize(11);
	angles[6][0] = 0.00000f;
	times[6][0] = 0.90000f;
	angles[6][1] = -0.17453f;
	times[6][1] = 1.90000f;
	angles[6][2] = -0.17453f;
	times[6][2] = 2.70000f;
	angles[6][3] = -1.57080f;
	times[6][3] = 3.40000f;
	angles[6][4] = -1.57080f;
	times[6][4] = 3.90000f;
	angles[6][5] = -0.85706f;
	times[6][5] = 4.90000f;
	angles[6][6] = 0.38551f;
	times[6][6] = 5.80000f;
	angles[6][7] = -0.85521f;
	times[6][7] = 6.80000f;
	angles[6][8] = -0.83599f;
	times[6][8] = 7.30000f;
	angles[6][9] = -0.87266f;
	times[6][9] = 8.40000f;
	angles[6][10] = -0.17453f;
	times[6][10] = 9.40000f;

	jointCodes[7] = "LHipRoll";
	angles[7].arraySetSize(11);
	times[7].arraySetSize(11);
	angles[7][0] = 0.00000f;
	times[7][0] = 0.90000f;
	angles[7][1] = 0.00000f;
	times[7][1] = 1.90000f;
	angles[7][2] = 0.00000f;
	times[7][2] = 2.70000f;
	angles[7][3] = 0.00000f;
	times[7][3] = 3.40000f;
	angles[7][4] = 0.54105f;
	times[7][4] = 3.90000f;
	angles[7][5] = 0.15498f;
	times[7][5] = 4.90000f;
	angles[7][6] = -0.29142f;
	times[7][6] = 5.80000f;
	angles[7][7] = 0.19199f;
	times[7][7] = 6.80000f;
	angles[7][8] = 0.36652f;
	times[7][8] = 7.30000f;
	angles[7][9] = 0.00000f;
	times[7][9] = 8.40000f;
	angles[7][10] = -0.01047f;
	times[7][10] = 9.40000f;

	jointCodes[8] = "LHipYawPitch";
	angles[8].arraySetSize(10);
	times[8].arraySetSize(10);
	angles[8][0] = 0.00000f;
	times[8][0] = 0.90000f;
	angles[8][1] = 0.00000f;
	times[8][1] = 1.90000f;
	angles[8][2] = -0.00000f;
	times[8][2] = 2.70000f;
	angles[8][3] = -0.66323f;
	times[8][3] = 3.40000f;
	angles[8][4] = -0.49909f;
	times[8][4] = 4.90000f;
	angles[8][5] = -0.85897f;
	times[8][5] = 5.80000f;
	angles[8][6] = -0.40225f;
	times[8][6] = 6.80000f;
	angles[8][7] = -0.40225f;
	times[8][7] = 7.30000f;
	angles[8][8] = 0.00000f;
	times[8][8] = 8.40000f;
	angles[8][9] = 0.00000f;
	times[8][9] = 9.40000f;

	jointCodes[9] = "LKneePitch";
	angles[9].arraySetSize(11);
	times[9].arraySetSize(11);
	angles[9][0] = 0.00000f;
	times[9][0] = 0.90000f;
	angles[9][1] = 1.67552f;
	times[9][1] = 1.90000f;
	angles[9][2] = 1.67552f;
	times[9][2] = 2.70000f;
	angles[9][3] = 1.67552f;
	times[9][3] = 3.40000f;
	angles[9][4] = 1.67552f;
	times[9][4] = 3.90000f;
	angles[9][5] = 2.20124f;
	times[9][5] = 4.90000f;
	angles[9][6] = 1.77479f;
	times[9][6] = 5.80000f;
	angles[9][7] = 2.20585f;
	times[9][7] = 6.80000f;
	angles[9][8] = 2.20585f;
	times[9][8] = 7.30000f;
	angles[9][9] = 2.09440f;
	times[9][9] = 8.40000f;
	angles[9][10] = 0.34907f;
	times[9][10] = 9.40000f;

	jointCodes[10] = "LShoulderPitch";
	angles[10].arraySetSize(11);
	times[10].arraySetSize(11);
	angles[10][0] = 0.00000f;
	times[10][0] = 0.90000f;
	angles[10][1] = 2.09440f;
	times[10][1] = 1.90000f;
	angles[10][2] = 2.09440f;
	times[10][2] = 2.70000f;
	angles[10][3] = 2.09440f;
	times[10][3] = 3.40000f;
	angles[10][4] = 2.09440f;
	times[10][4] = 3.90000f;
	angles[10][5] = 0.69813f;
	times[10][5] = 4.90000f;
	angles[10][6] = 0.74074f;
	times[10][6] = 5.80000f;
	angles[10][7] = 0.73321f;
	times[10][7] = 6.80000f;
	angles[10][8] = 0.73321f;
	times[10][8] = 7.30000f;
	angles[10][9] = 1.71042f;
	times[10][9] = 8.40000f;
	angles[10][10] = 1.83260f;
	times[10][10] = 9.40000f;

	jointCodes[11] = "LShoulderRoll";
	angles[11].arraySetSize(11);
	times[11].arraySetSize(11);
	angles[11][0] = 1.57080f;
	times[11][0] = 0.90000f;
	angles[11][1] = 0.80285f;
	times[11][1] = 1.90000f;
	angles[11][2] = 0.47124f;
	times[11][2] = 2.70000f;
	angles[11][3] = 0.36652f;
	times[11][3] = 3.40000f;
	angles[11][4] = 0.00000f;
	times[11][4] = 3.90000f;
	angles[11][5] = 1.04720f;
	times[11][5] = 4.90000f;
	angles[11][6] = 0.49851f;
	times[11][6] = 5.80000f;
	angles[11][7] = 0.49851f;
	times[11][7] = 6.80000f;
	angles[11][8] = 0.49851f;
	times[11][8] = 7.30000f;
	angles[11][9] = 0.03491f;
	times[11][9] = 8.40000f;
	angles[11][10] = 0.19199f;
	times[11][10] = 9.40000f;

	jointCodes[12] = "RAnklePitch";
	angles[12].arraySetSize(11);
	times[12].arraySetSize(11);
	angles[12][0] = 0.00000f;
	times[12][0] = 0.90000f;
	angles[12][1] = 0.24435f;
	times[12][1] = 1.90000f;
	angles[12][2] = 0.24435f;
	times[12][2] = 2.70000f;
	angles[12][3] = 0.24435f;
	times[12][3] = 3.40000f;
	angles[12][4] = 0.78540f;
	times[12][4] = 3.90000f;
	angles[12][5] = 0.78540f;
	times[12][5] = 4.90000f;
	angles[12][6] = 0.69115f;
	times[12][6] = 5.80000f;
	angles[12][7] = 0.40317f;
	times[12][7] = 6.80000f;
	angles[12][8] = -0.57945f;
	times[12][8] = 7.30000f;
	angles[12][9] = -1.22173f;
	times[12][9] = 8.40000f;
	angles[12][10] = -0.17453f;
	times[12][10] = 9.40000f;

	jointCodes[13] = "RAnkleRoll";
	angles[13].arraySetSize(11);
	times[13].arraySetSize(11);
	angles[13][0] = 0.00000f;
	times[13][0] = 0.90000f;
	angles[13][1] = 0.00000f;
	times[13][1] = 1.90000f;
	angles[13][2] = 0.00000f;
	times[13][2] = 2.70000f;
	angles[13][3] = 0.00000f;
	times[13][3] = 3.40000f;
	angles[13][4] = 0.00000f;
	times[13][4] = 3.90000f;
	angles[13][5] = 0.00929f;
	times[13][5] = 4.90000f;
	angles[13][6] = -0.12915f;
	times[13][6] = 5.80000f;
	angles[13][7] = 0.67960f;
	times[13][7] = 6.80000f;
	angles[13][8] = 0.27751f;
	times[13][8] = 7.30000f;
	angles[13][9] = 0.00000f;
	times[13][9] = 8.40000f;
	angles[13][10] = 0.00000f;
	times[13][10] = 9.40000f;

	jointCodes[14] = "RElbowRoll";
	angles[14].arraySetSize(11);
	times[14].arraySetSize(11);
	angles[14][0] = 0.00000f;
	times[14][0] = 0.90000f;
	angles[14][1] = 0.00000f;
	times[14][1] = 1.90000f;
	angles[14][2] = 1.65806f;
	times[14][2] = 2.70000f;
	angles[14][3] = 0.69813f;
	times[14][3] = 3.40000f;
	angles[14][4] = 0.00000f;
	times[14][4] = 3.90000f;
	angles[14][5] = 0.07205f;
	times[14][5] = 4.90000f;
	angles[14][6] = 0.05819f;
	times[14][6] = 5.80000f;
	angles[14][7] = 0.45379f;
	times[14][7] = 6.80000f;
	angles[14][8] = 0.55995f;
	times[14][8] = 7.30000f;
	angles[14][9] = 1.13446f;
	times[14][9] = 8.40000f;
	angles[14][10] = 1.25664f;
	times[14][10] = 9.40000f;

	jointCodes[15] = "RElbowYaw";
	angles[15].arraySetSize(11);
	times[15].arraySetSize(11);
	angles[15][0] = 0.00000f;
	times[15][0] = 0.90000f;
	angles[15][1] = -0.15708f;
	times[15][1] = 1.90000f;
	angles[15][2] = -0.08727f;
	times[15][2] = 2.70000f;
	angles[15][3] = -0.08727f;
	times[15][3] = 3.40000f;
	angles[15][4] = -0.08727f;
	times[15][4] = 3.90000f;
	angles[15][5] = -0.08080f;
	times[15][5] = 4.90000f;
	angles[15][6] = -0.08241f;
	times[15][6] = 5.80000f;
	angles[15][7] = 0.00062f;
	times[15][7] = 6.80000f;
	angles[15][8] = 0.00062f;
	times[15][8] = 7.30000f;
	angles[15][9] = 1.25664f;
	times[15][9] = 8.40000f;
	angles[15][10] = 1.23918f;
	times[15][10] = 9.40000f;

	jointCodes[16] = "RHipPitch";
	angles[16].arraySetSize(11);
	times[16].arraySetSize(11);
	angles[16][0] = 0.00000f;
	times[16][0] = 0.90000f;
	angles[16][1] = -0.17453f;
	times[16][1] = 1.90000f;
	angles[16][2] = -0.17453f;
	times[16][2] = 2.70000f;
	angles[16][3] = -1.57080f;
	times[16][3] = 3.40000f;
	angles[16][4] = -1.57080f;
	times[16][4] = 3.90000f;
	angles[16][5] = -1.52484f;
	times[16][5] = 4.90000f;
	angles[16][6] = -1.55965f;
	times[16][6] = 5.80000f;
	angles[16][7] = -0.90583f;
	times[16][7] = 6.80000f;
	angles[16][8] = -0.90583f;
	times[16][8] = 7.30000f;
	angles[16][9] = -0.87266f;
	times[16][9] = 8.40000f;
	angles[16][10] = -0.17453f;
	times[16][10] = 9.40000f;

	jointCodes[17] = "RHipRoll";
	angles[17].arraySetSize(11);
	times[17].arraySetSize(11);
	angles[17][0] = 0.00000f;
	times[17][0] = 0.90000f;
	angles[17][1] = 0.00000f;
	times[17][1] = 1.90000f;
	angles[17][2] = 0.00000f;
	times[17][2] = 2.70000f;
	angles[17][3] = 0.00000f;
	times[17][3] = 3.40000f;
	angles[17][4] = -0.54105f;
	times[17][4] = 3.90000f;
	angles[17][5] = -0.55842f;
	times[17][5] = 4.90000f;
	angles[17][6] = -0.56600f;
	times[17][6] = 5.80000f;
	angles[17][7] = -0.29671f;
	times[17][7] = 6.80000f;
	angles[17][8] = -0.01745f;
	times[17][8] = 7.30000f;
	angles[17][9] = 0.00000f;
	times[17][9] = 8.40000f;
	angles[17][10] = 0.01047f;
	times[17][10] = 9.40000f;

	jointCodes[18] = "RHipYawPitch";
	angles[18].arraySetSize(10);
	times[18].arraySetSize(10);
	angles[18][0] = 0.00000f;
	times[18][0] = 0.90000f;
	angles[18][1] = 0.00000f;
	times[18][1] = 1.90000f;
	angles[18][2] = -0.00000f;
	times[18][2] = 2.70000f;
	angles[18][3] = -0.66323f;
	times[18][3] = 3.40000f;
	angles[18][4] = -0.49909f;
	times[18][4] = 4.90000f;
	angles[18][5] = -0.85897f;
	times[18][5] = 5.80000f;
	angles[18][6] = -0.40225f;
	times[18][6] = 6.80000f;
	angles[18][7] = -0.40225f;
	times[18][7] = 7.30000f;
	angles[18][8] = 0.00000f;
	times[18][8] = 8.40000f;
	angles[18][9] = 0.00000f;
	times[18][9] = 9.40000f;

	jointCodes[19] = "RKneePitch";
	angles[19].arraySetSize(11);
	times[19].arraySetSize(11);
	angles[19][0] = 0.00000f;
	times[19][0] = 0.90000f;
	angles[19][1] = 1.67552f;
	times[19][1] = 1.90000f;
	angles[19][2] = 1.67552f;
	times[19][2] = 2.70000f;
	angles[19][3] = 1.67552f;
	times[19][3] = 3.40000f;
	angles[19][4] = 1.67552f;
	times[19][4] = 3.90000f;
	angles[19][5] = 1.22173f;
	times[19][5] = 4.90000f;
	angles[19][6] = 1.08036f;
	times[19][6] = 5.80000f;
	angles[19][7] = 0.87616f;
	times[19][7] = 6.80000f;
	angles[19][8] = 1.76278f;
	times[19][8] = 7.30000f;
	angles[19][9] = 2.09440f;
	times[19][9] = 8.40000f;
	angles[19][10] = 0.34907f;
	times[19][10] = 9.40000f;

	jointCodes[20] = "RShoulderPitch";
	angles[20].arraySetSize(11);
	times[20].arraySetSize(11);
	angles[20][0] = 0.00000f;
	times[20][0] = 0.90000f;
	angles[20][1] = 2.09440f;
	times[20][1] = 1.90000f;
	angles[20][2] = 2.09440f;
	times[20][2] = 2.70000f;
	angles[20][3] = 2.09440f;
	times[20][3] = 3.40000f;
	angles[20][4] = 2.09440f;
	times[20][4] = 3.90000f;
	angles[20][5] = 2.09440f;
	times[20][5] = 4.90000f;
	angles[20][6] = 1.77434f;
	times[20][6] = 5.80000f;
	angles[20][7] = 0.89131f;
	times[20][7] = 6.80000f;
	angles[20][8] = 0.89131f;
	times[20][8] = 7.30000f;
	angles[20][9] = 1.71042f;
	times[20][9] = 8.40000f;
	angles[20][10] = 1.83260f;
	times[20][10] = 9.40000f;

	jointCodes[21] = "RShoulderRoll";
	angles[21].arraySetSize(11);
	times[21].arraySetSize(11);
	angles[21][0] = -1.57080f;
	times[21][0] = 0.90000f;
	angles[21][1] = -0.80285f;
	times[21][1] = 1.90000f;
	angles[21][2] = -0.47124f;
	times[21][2] = 2.70000f;
	angles[21][3] = -0.36652f;
	times[21][3] = 3.40000f;
	angles[21][4] = 0.00000f;
	times[21][4] = 3.90000f;
	angles[21][5] = -0.57596f;
	times[21][5] = 4.90000f;
	angles[21][6] = -0.27770f;
	times[21][6] = 5.80000f;
	angles[21][7] = -0.87266f;
	times[21][7] = 6.80000f;
	angles[21][8] = -0.68068f;
	times[21][8] = 7.30000f;
	angles[21][9] = -0.03491f;
	times[21][9] = 8.40000f;
	angles[21][10] = -0.19199f;
	times[21][10] = 9.40000f;

	actionPID = motion->post.angleInterpolation(jointCodes, angles, times, 1);

}

void MotionController::ALstandUpCross() {

	AL::ALValue jointCodes, angles, times;
	jointCodes.arraySetSize(22);
	angles.arraySetSize(22);
	times.arraySetSize(22);

	jointCodes[0] = "HeadPitch";
	angles[0].arraySetSize(2);
	times[0].arraySetSize(2);
	angles[0][0] = 0.00000f;
	times[0][0] = 1.90000f;
	angles[0][1] = 0.00000f;
	times[0][1] = 2.90000f;

	jointCodes[1] = "HeadYaw";
	angles[1].arraySetSize(2);
	times[1].arraySetSize(2);
	angles[1][0] = 0.00000f;
	times[1][0] = 1.90000f;
	angles[1][1] = 0.00000f;
	times[1][1] = 2.90000f;

	jointCodes[2] = "LAnklePitch";
	angles[2].arraySetSize(2);
	times[2].arraySetSize(2);
	angles[2][0] = 0.52360f;
	times[2][0] = 1.90000f;
	angles[2][1] = 0.52360f;
	times[2][1] = 2.90000f;

	jointCodes[3] = "LAnkleRoll";
	angles[3].arraySetSize(2);
	times[3].arraySetSize(2);
	angles[3][0] = 0.00000f;
	times[3][0] = 1.90000f;
	angles[3][1] = 0.00000f;
	times[3][1] = 2.90000f;

	jointCodes[4] = "LElbowRoll";
	angles[4].arraySetSize(2);
	times[4].arraySetSize(2);
	angles[4][0] = 0.00000f;
	times[4][0] = 1.90000f;
	angles[4][1] = 0.00000f;
	times[4][1] = 2.90000f;

	jointCodes[5] = "LElbowYaw";
	angles[5].arraySetSize(2);
	times[5].arraySetSize(2);
	angles[5][0] = 0.00000f;
	times[5][0] = 1.90000f;
	angles[5][1] = 0.00000f;
	times[5][1] = 2.90000f;

	jointCodes[7] = "LHipPitch";
	angles[7].arraySetSize(2);
	times[7].arraySetSize(2);
	angles[7][0] = 0.00000f;
	times[7][0] = 1.90000f;
	angles[7][1] = 0.00000f;
	times[7][1] = 2.90000f;

	jointCodes[8] = "LHipRoll";
	angles[8].arraySetSize(2);
	times[8].arraySetSize(2);
	angles[8][0] = 0.00000f;
	times[8][0] = 1.90000f;
	angles[8][1] = 0.00000f;
	times[8][1] = 2.90000f;

	jointCodes[9] = "LHipYawPitch";
	angles[9].arraySetSize(2);
	times[9].arraySetSize(2);
	angles[9][0] = 0.00000f;
	times[9][0] = 1.90000f;
	angles[9][1] = 0.00000f;
	times[9][1] = 2.90000f;

	jointCodes[10] = "LKneePitch";
	angles[10].arraySetSize(2);
	times[10].arraySetSize(2);
	angles[10][0] = 0.00000f;
	times[10][0] = 1.90000f;
	angles[10][1] = 0.00000f;
	times[10][1] = 2.90000f;

	jointCodes[11] = "LShoulderPitch";
	angles[11].arraySetSize(2);
	times[11].arraySetSize(2);
	angles[11][0] = 0.00000f;
	times[11][0] = 1.90000f;
	angles[11][1] = 0.00000f;
	times[11][1] = 2.90000f;

	jointCodes[12] = "LShoulderRoll";
	angles[12].arraySetSize(2);
	times[12].arraySetSize(2);
	angles[12][0] = 1.57080f;
	times[12][0] = 1.90000f;
	angles[12][1] = 1.57080f;
	times[12][1] = 2.90000f;

	jointCodes[14] = "RAnklePitch";
	angles[14].arraySetSize(2);
	times[14].arraySetSize(2);
	angles[14][0] = 0.52360f;
	times[14][0] = 1.90000f;
	angles[14][1] = 0.52360f;
	times[14][1] = 2.90000f;

	jointCodes[15] = "RAnkleRoll";
	angles[15].arraySetSize(2);
	times[15].arraySetSize(2);
	angles[15][0] = 0.00000f;
	times[15][0] = 1.90000f;
	angles[15][1] = 0.00000f;
	times[15][1] = 2.90000f;

	jointCodes[16] = "RElbowRoll";
	angles[16].arraySetSize(2);
	times[16].arraySetSize(2);
	angles[16][0] = 0.00000f;
	times[16][0] = 1.90000f;
	angles[16][1] = 0.00000f;
	times[16][1] = 2.90000f;

	jointCodes[17] = "RElbowYaw";
	angles[17].arraySetSize(2);
	times[17].arraySetSize(2);
	angles[17][0] = 0.00000f;
	times[17][0] = 1.90000f;
	angles[17][1] = 0.00000f;
	times[17][1] = 2.90000f;

	jointCodes[19] = "RHipPitch";
	angles[19].arraySetSize(2);
	times[19].arraySetSize(2);
	angles[19][0] = 0.00000f;
	times[19][0] = 1.90000f;
	angles[19][1] = 0.00000f;
	times[19][1] = 2.90000f;

	jointCodes[20] = "RHipRoll";
	angles[20].arraySetSize(2);
	times[20].arraySetSize(2);
	angles[20][0] = 0.00000f;
	times[20][0] = 1.90000f;
	angles[20][1] = 0.00000f;
	times[20][1] = 2.90000f;

	jointCodes[6] = "RKneePitch";
	angles[6].arraySetSize(2);
	times[6].arraySetSize(2);
	angles[6][0] = 0.00000f;
	times[6][0] = 1.90000f;
	angles[6][1] = 0.00000f;
	times[6][1] = 2.90000f;

	jointCodes[13] = "RShoulderPitch";
	angles[13].arraySetSize(2);
	times[13].arraySetSize(2);
	angles[13][0] = 0.00000f;
	times[13][0] = 1.90000f;
	angles[13][1] = 0.00000f;
	times[13][1] = 2.90000f;

	jointCodes[18] = "RShoulderRoll";
	angles[18].arraySetSize(2);
	times[18].arraySetSize(2);
	angles[18][0] = -1.57080f;
	times[18][0] = 1.90000f;
	angles[18][1] = -1.57080f;
	times[18][1] = 2.90000f;

	jointCodes[21] = "RHipYawPitch";
	angles[21].arraySetSize(2);
	times[21].arraySetSize(2);
	angles[21][0] = 0.00000f;
	times[21][0] = 1.90000f;
	angles[21][1] = 0.00000f;
	times[21][1] = 2.90000f;

	motion->angleInterpolation(jointCodes, angles, times, 1);

}



void MotionController::ALstandUpFront2010() {

	AL::ALValue names, times, keys;
	names.arraySetSize(25);
	times.arraySetSize(25);
	keys.arraySetSize(25);

	names[0] = "HeadYaw";
	times[0].arraySetSize(4);
	keys[0].arraySetSize(4);

	times[0][0] = 1;
	keys[0][0] = AL::ALValue::array(-0.058334, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[0][1] = 1.7;
	keys[0][1] = AL::ALValue::array(-0.058334, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[0][2] = 2.4;
	keys[0][2] = AL::ALValue::array(-0.058334, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
	times[0][3] = 9.4;
	keys[0][3] = AL::ALValue::array(0.067454, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[1] = "HeadPitch";
	times[1].arraySetSize(8);
	keys[1].arraySetSize(8);

	times[1][0] = 1;
	keys[1][0] = AL::ALValue::array(-0.576826, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[1][1] = 1.7;
	keys[1][1] = AL::ALValue::array(-0.592166, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[1][2] = 2.4;
	keys[1][2] = AL::ALValue::array(-0.547679, AL::ALValue::array(2, -0.233333, -0.0444867), AL::ALValue::array(2, 0.333333, 0.0635524));
	times[1][3] = 3.4;
	keys[1][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[1][4] = 4.7;
	keys[1][4] = AL::ALValue::array(-0.349066, AL::ALValue::array(2, -0.433333, 0.0756309), AL::ALValue::array(2, 0.5, -0.0872665));
	times[1][5] = 6.2;
	keys[1][5] = AL::ALValue::array(-0.488692, AL::ALValue::array(2, -0.5, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[1][6] = 7.2;
	keys[1][6] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.733333, 0.0));
	times[1][7] = 9.4;
	keys[1][7] = AL::ALValue::array(-0.0414599, AL::ALValue::array(2, -0.733333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[2] = "LShoulderPitch";
	times[2].arraySetSize(10);
	keys[2].arraySetSize(10);

	times[2][0] = 1;
	keys[2][0] = AL::ALValue::array(0.084328, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[2][1] = 1.7;
	keys[2][1] = AL::ALValue::array(-0.37127, AL::ALValue::array(2, -0.233333, 0.216805), AL::ALValue::array(2, 0.233333, -0.216805));
	times[2][2] = 2.4;
	keys[2][2] = AL::ALValue::array(-1.2165, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[2][3] = 3.4;
	keys[2][3] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, -0.15101), AL::ALValue::array(2, 0.433333, 0.196313));
	times[2][4] = 4.7;
	keys[2][4] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.433333, -0.189077), AL::ALValue::array(2, 0.233333, 0.101811));
	times[2][5] = 5.4;
	keys[2][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.233333, -0.122173), AL::ALValue::array(2, 0.266667, 0.139626));
	times[2][6] = 6.2;
	keys[2][6] = AL::ALValue::array(0.610865, AL::ALValue::array(2, -0.266667, -0.165483), AL::ALValue::array(2, 0.333333, 0.206854));
	times[2][7] = 7.2;
	keys[2][7] = AL::ALValue::array(1.11701, AL::ALValue::array(2, -0.333333, -0.153377), AL::ALValue::array(2, 0.4, 0.184053));
	times[2][8] = 8.4;
	keys[2][8] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[2][9] = 9.4;
	keys[2][9] = AL::ALValue::array(1.59378, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[3] = "LShoulderRoll";
	times[3].arraySetSize(10);
	keys[3].arraySetSize(10);

	times[3][0] = 1;
	keys[3][0] = AL::ALValue::array(1.5539, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[3][1] = 1.7;
	keys[3][1] = AL::ALValue::array(1.03541, AL::ALValue::array(2, -0.233333, 0.256178), AL::ALValue::array(2, 0.233333, -0.256178));
	times[3][2] = 2.4;
	keys[3][2] = AL::ALValue::array(0.016832, AL::ALValue::array(2, -0.233333, 0.00569243), AL::ALValue::array(2, 0.333333, -0.00813205));
	times[3][3] = 3.4;
	keys[3][3] = AL::ALValue::array(0.00869999, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[3][4] = 4.7;
	keys[3][4] = AL::ALValue::array(0.00869999, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[3][5] = 5.4;
	keys[3][5] = AL::ALValue::array(0.00869999, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[3][6] = 6.2;
	keys[3][6] = AL::ALValue::array(0.0349066, AL::ALValue::array(2, -0.266667, -0.0181037), AL::ALValue::array(2, 0.333333, 0.0226296));
	times[3][7] = 7.2;
	keys[3][7] = AL::ALValue::array(0.1309, AL::ALValue::array(2, -0.333333, -0.0211555), AL::ALValue::array(2, 0.4, 0.0253866));
	times[3][8] = 8.4;
	keys[3][8] = AL::ALValue::array(0.174533, AL::ALValue::array(2, -0.4, -0.0157975), AL::ALValue::array(2, 0.333333, 0.0131645));
	times[3][9] = 9.4;
	keys[3][9] = AL::ALValue::array(0.217786, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[4] = "LElbowYaw";
	times[4].arraySetSize(10);
	keys[4].arraySetSize(10);

	times[4][0] = 1;
	keys[4][0] = AL::ALValue::array(-0.0353239, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[4][1] = 1.7;
	keys[4][1] = AL::ALValue::array(-1.70892, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[4][2] = 2.4;
	keys[4][2] = AL::ALValue::array(-1.58006, AL::ALValue::array(2, -0.233333, -0.00648616), AL::ALValue::array(2, 0.333333, 0.00926594));
	times[4][3] = 3.4;
	keys[4][3] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[4][4] = 4.7;
	keys[4][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[4][5] = 5.4;
	keys[4][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[4][6] = 6.2;
	keys[4][6] = AL::ALValue::array(-0.244346, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[4][7] = 7.2;
	keys[4][7] = AL::ALValue::array(-0.925025, AL::ALValue::array(2, -0.333333, 0.200977), AL::ALValue::array(2, 0.4, -0.241173));
	times[4][8] = 8.4;
	keys[4][8] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[4][9] = 9.4;
	keys[4][9] = AL::ALValue::array(-1.01862, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[5] = "LElbowRoll";
	times[5].arraySetSize(10);
	keys[5].arraySetSize(10);

	times[5][0] = 1;
	keys[5][0] = AL::ALValue::array(-0.0367741, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[5][1] = 1.7;
	keys[5][1] = AL::ALValue::array(-0.84826, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[5][2] = 2.4;
	keys[5][2] = AL::ALValue::array(-0.352778, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[5][3] = 3.4;
	keys[5][3] = AL::ALValue::array(-0.610865, AL::ALValue::array(2, -0.333333, 0.175264), AL::ALValue::array(2, 0.433333, -0.227843));
	times[5][4] = 4.7;
	keys[5][4] = AL::ALValue::array(-1.5621, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[5][5] = 5.4;
	keys[5][5] = AL::ALValue::array(-0.139626, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[5][6] = 6.2;
	keys[5][6] = AL::ALValue::array(-0.715585, AL::ALValue::array(2, -0.266667, 0.170654), AL::ALValue::array(2, 0.333333, -0.213318));
	times[5][7] = 7.2;
	keys[5][7] = AL::ALValue::array(-1.29154, AL::ALValue::array(2, -0.333333, 0.0872665), AL::ALValue::array(2, 0.4, -0.10472));
	times[5][8] = 8.4;
	keys[5][8] = AL::ALValue::array(-1.39626, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[5][9] = 9.4;
	keys[5][9] = AL::ALValue::array(-0.681054, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[6] = "LWristYaw";
	times[6].arraySetSize(4);
	keys[6].arraySetSize(4);

	times[6][0] = 1;
	keys[6][0] = AL::ALValue::array(-1.55092, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[6][1] = 1.7;
	keys[6][1] = AL::ALValue::array(-1.15054, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[6][2] = 2.4;
	keys[6][2] = AL::ALValue::array(-1.55092, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
	times[6][3] = 9.4;
	keys[6][3] = AL::ALValue::array(-0.710284, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[7] = "LHand";
	times[7].arraySetSize(4);
	keys[7].arraySetSize(4);

	times[7][0] = 1;
	keys[7][0] = AL::ALValue::array(0.0562249, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[7][1] = 1.7;
	keys[7][1] = AL::ALValue::array(0.0551777, AL::ALValue::array(2, -0.233333, 0.0010472), AL::ALValue::array(2, 0.233333, -0.0010472));
	times[7][2] = 2.4;
	keys[7][2] = AL::ALValue::array(0.0188749, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
	times[7][3] = 9.4;
	keys[7][3] = AL::ALValue::array(0.250305, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[8] = "RShoulderPitch";
	times[8].arraySetSize(10);
	keys[8].arraySetSize(10);

	times[8][0] = 1;
	keys[8][0] = AL::ALValue::array(-0.0275701, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[8][1] = 1.7;
	keys[8][1] = AL::ALValue::array(-0.622761, AL::ALValue::array(2, -0.233333, 0.188426), AL::ALValue::array(2, 0.233333, -0.188426));
	times[8][2] = 2.4;
	keys[8][2] = AL::ALValue::array(-1.15813, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[8][3] = 3.4;
	keys[8][3] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, -0.14255), AL::ALValue::array(2, 0.433333, 0.185315));
	times[8][4] = 4.7;
	keys[8][4] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.433333, -0.189077), AL::ALValue::array(2, 0.233333, 0.101811));
	times[8][5] = 5.4;
	keys[8][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.233333, -0.122173), AL::ALValue::array(2, 0.266667, 0.139626));
	times[8][6] = 6.2;
	keys[8][6] = AL::ALValue::array(0.610865, AL::ALValue::array(2, -0.266667, -0.165483), AL::ALValue::array(2, 0.333333, 0.206854));
	times[8][7] = 7.2;
	keys[8][7] = AL::ALValue::array(1.11701, AL::ALValue::array(2, -0.333333, -0.153377), AL::ALValue::array(2, 0.4, 0.184053));
	times[8][8] = 8.4;
	keys[8][8] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[8][9] = 9.4;
	keys[8][9] = AL::ALValue::array(1.47882, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[9] = "RShoulderRoll";
	times[9].arraySetSize(10);
	keys[9].arraySetSize(10);

	times[9][0] = 1;
	keys[9][0] = AL::ALValue::array(-1.53558, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[9][1] = 1.7;
	keys[9][1] = AL::ALValue::array(-1.07538, AL::ALValue::array(2, -0.233333, -0.251832), AL::ALValue::array(2, 0.233333, 0.251832));
	times[9][2] = 2.4;
	keys[9][2] = AL::ALValue::array(-0.0245859, AL::ALValue::array(2, -0.233333, -0.0111202), AL::ALValue::array(2, 0.333333, 0.0158859));
	times[9][3] = 3.4;
	keys[9][3] = AL::ALValue::array(-0.00869999, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[9][4] = 4.7;
	keys[9][4] = AL::ALValue::array(-0.00869999, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[9][5] = 5.4;
	keys[9][5] = AL::ALValue::array(-0.00869999, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[9][6] = 6.2;
	keys[9][6] = AL::ALValue::array(-0.0349066, AL::ALValue::array(2, -0.266667, 0.0181037), AL::ALValue::array(2, 0.333333, -0.0226296));
	times[9][7] = 7.2;
	keys[9][7] = AL::ALValue::array(-0.1309, AL::ALValue::array(2, -0.333333, 0.0211555), AL::ALValue::array(2, 0.4, -0.0253866));
	times[9][8] = 8.4;
	keys[9][8] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[9][9] = 9.4;
	keys[9][9] = AL::ALValue::array(-0.14884, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[10] = "RElbowYaw";
	times[10].arraySetSize(10);
	keys[10].arraySetSize(10);

	times[10][0] = 1;
	keys[10][0] = AL::ALValue::array(-0.01078, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[10][1] = 1.7;
	keys[10][1] = AL::ALValue::array(1.36215, AL::ALValue::array(2, -0.233333, -0.199421), AL::ALValue::array(2, 0.233333, 0.199421));
	times[10][2] = 2.4;
	keys[10][2] = AL::ALValue::array(1.56157, AL::ALValue::array(2, -0.233333, -0.0064581), AL::ALValue::array(2, 0.333333, 0.00922586));
	times[10][3] = 3.4;
	keys[10][3] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[10][4] = 4.7;
	keys[10][4] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[10][5] = 5.4;
	keys[10][5] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[10][6] = 6.2;
	keys[10][6] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[10][7] = 7.2;
	keys[10][7] = AL::ALValue::array(0.925025, AL::ALValue::array(2, -0.333333, -0.200977), AL::ALValue::array(2, 0.4, 0.241173));
	times[10][8] = 8.4;
	keys[10][8] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[10][9] = 9.4;
	keys[10][9] = AL::ALValue::array(0.90962, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[11] = "RElbowRoll";
	times[11].arraySetSize(10);
	keys[11].arraySetSize(10);

	times[11][0] = 1;
	keys[11][0] = AL::ALValue::array(0.0353239, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[11][1] = 1.7;
	keys[11][1] = AL::ALValue::array(0.805393, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[11][2] = 2.4;
	keys[11][2] = AL::ALValue::array(0.382007, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[11][3] = 3.4;
	keys[11][3] = AL::ALValue::array(0.610865, AL::ALValue::array(2, -0.333333, -0.171028), AL::ALValue::array(2, 0.433333, 0.222336));
	times[11][4] = 4.7;
	keys[11][4] = AL::ALValue::array(1.5621, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[11][5] = 5.4;
	keys[11][5] = AL::ALValue::array(0.139626, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[11][6] = 6.2;
	keys[11][6] = AL::ALValue::array(0.715585, AL::ALValue::array(2, -0.266667, -0.170654), AL::ALValue::array(2, 0.333333, 0.213318));
	times[11][7] = 7.2;
	keys[11][7] = AL::ALValue::array(1.29154, AL::ALValue::array(2, -0.333333, -0.0872665), AL::ALValue::array(2, 0.4, 0.10472));
	times[11][8] = 8.4;
	keys[11][8] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[11][9] = 9.4;
	keys[11][9] = AL::ALValue::array(0.392746, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[12] = "RWristYaw";
	times[12].arraySetSize(4);
	keys[12].arraySetSize(4);

	times[12][0] = 1;
	keys[12][0] = AL::ALValue::array(1.50941, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[12][1] = 1.7;
	keys[12][1] = AL::ALValue::array(1.50941, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[12][2] = 2.4;
	keys[12][2] = AL::ALValue::array(1.50941, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
	times[12][3] = 9.4;
	keys[12][3] = AL::ALValue::array(1.08603, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[13] = "RHand";
	times[13].arraySetSize(4);
	keys[13].arraySetSize(4);

	times[13][0] = 1;
	keys[13][0] = AL::ALValue::array(0.0178277, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[13][1] = 1.7;
	keys[13][1] = AL::ALValue::array(0.0174786, AL::ALValue::array(2, -0.233333, 0.000349068), AL::ALValue::array(2, 0.233333, -0.000349068));
	times[13][2] = 2.4;
	keys[13][2] = AL::ALValue::array(0.0122427, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 2.33333, 0.0));
	times[13][3] = 9.4;
	keys[13][3] = AL::ALValue::array(0.209115, AL::ALValue::array(2, -2.33333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[14] = "LHipYawPitch";
	times[14].arraySetSize(10);
	keys[14].arraySetSize(10);

	times[14][0] = 1;
	keys[14][0] = AL::ALValue::array(-0.033706, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[14][1] = 1.7;
	keys[14][1] = AL::ALValue::array(-0.033706, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[14][2] = 2.4;
	keys[14][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[14][3] = 3.4;
	keys[14][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, 4.19617e-05), AL::ALValue::array(2, 0.433333, -5.45502e-05));
	times[14][4] = 4.7;
	keys[14][4] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[14][5] = 5.4;
	keys[14][5] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[14][6] = 6.2;
	keys[14][6] = AL::ALValue::array(-0.965167, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[14][7] = 7.2;
	keys[14][7] = AL::ALValue::array(-0.785398, AL::ALValue::array(2, -0.333333, -0.146237), AL::ALValue::array(2, 0.4, 0.175485));
	times[14][8] = 8.4;
	keys[14][8] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[14][9] = 9.4;
	keys[14][9] = AL::ALValue::array(-0.236194, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[15] = "LHipRoll";
	times[15].arraySetSize(10);
	keys[15].arraySetSize(10);

	times[15][0] = 1;
	keys[15][0] = AL::ALValue::array(0.0629359, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[15][1] = 1.7;
	keys[15][1] = AL::ALValue::array(0.0614019, AL::ALValue::array(2, -0.233333, 0.00153397), AL::ALValue::array(2, 0.233333, -0.00153397));
	times[15][2] = 2.4;
	keys[15][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, 2.92634e-05), AL::ALValue::array(2, 0.333333, -4.18048e-05));
	times[15][3] = 3.4;
	keys[15][3] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[15][4] = 4.7;
	keys[15][4] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[15][5] = 5.4;
	keys[15][5] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[15][6] = 6.2;
	keys[15][6] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.266667, -0.0139626), AL::ALValue::array(2, 0.333333, 0.0174533));
	times[15][7] = 7.2;
	keys[15][7] = AL::ALValue::array(0.10472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	times[15][8] = 8.4;
	keys[15][8] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[15][9] = 9.4;
	keys[15][9] = AL::ALValue::array(0.14884, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[16] = "LHipPitch";
	times[16].arraySetSize(10);
	keys[16].arraySetSize(10);

	times[16][0] = 1;
	keys[16][0] = AL::ALValue::array(0.0614019, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[16][1] = 1.7;
	keys[16][1] = AL::ALValue::array(0.0598679, AL::ALValue::array(2, -0.233333, 0.00153397), AL::ALValue::array(2, 0.233333, -0.00153397));
	times[16][2] = 2.4;
	keys[16][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, 0.059826), AL::ALValue::array(2, 0.333333, -0.0854657));
	times[16][3] = 3.4;
	keys[16][3] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[16][4] = 4.7;
	keys[16][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[16][5] = 5.4;
	keys[16][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[16][6] = 6.2;
	keys[16][6] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[16][7] = 7.2;
	keys[16][7] = AL::ALValue::array(-1.06989, AL::ALValue::array(2, -0.333333, -0.0189077), AL::ALValue::array(2, 0.4, 0.0226893));
	times[16][8] = 8.4;
	keys[16][8] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.4, -0.0226893), AL::ALValue::array(2, 0.333333, 0.0189077));
	times[16][9] = 9.4;
	keys[16][9] = AL::ALValue::array(0.213269, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[17] = "LKneePitch";
	times[17].arraySetSize(10);
	keys[17].arraySetSize(10);

	times[17][0] = 1;
	keys[17][0] = AL::ALValue::array(-0.0706059, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[17][1] = 1.7;
	keys[17][1] = AL::ALValue::array(-0.067538, AL::ALValue::array(2, -0.233333, -0.00306794), AL::ALValue::array(2, 0.233333, 0.00306794));
	times[17][2] = 2.4;
	keys[17][2] = AL::ALValue::array(2.09234, AL::ALValue::array(2, -0.233333, -0.0014416), AL::ALValue::array(2, 0.333333, 0.00205942));
	times[17][3] = 3.4;
	keys[17][3] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[17][4] = 4.7;
	keys[17][4] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.433333, 0.0648265), AL::ALValue::array(2, 0.233333, -0.0349066));
	times[17][5] = 5.4;
	keys[17][5] = AL::ALValue::array(1.01229, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[17][6] = 6.2;
	keys[17][6] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[17][7] = 7.2;
	keys[17][7] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	times[17][8] = 8.4;
	keys[17][8] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.4, 0.018134), AL::ALValue::array(2, 0.333333, -0.0151117));
	times[17][9] = 9.4;
	keys[17][9] = AL::ALValue::array(-0.090548, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[18] = "LAnklePitch";
	times[18].arraySetSize(10);
	keys[18].arraySetSize(10);

	times[18][0] = 1;
	keys[18][0] = AL::ALValue::array(0.921892, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[18][1] = 1.7;
	keys[18][1] = AL::ALValue::array(0.922747, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[18][2] = 2.4;
	keys[18][2] = AL::ALValue::array(-1.1352, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[18][3] = 3.4;
	keys[18][3] = AL::ALValue::array(-1.13446, AL::ALValue::array(2, -0.333333, -0.000738228), AL::ALValue::array(2, 0.433333, 0.000959697));
	times[18][4] = 4.7;
	keys[18][4] = AL::ALValue::array(-0.783653, AL::ALValue::array(2, -0.433333, -0.264708), AL::ALValue::array(2, 0.233333, 0.142535));
	times[18][5] = 5.4;
	keys[18][5] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[18][6] = 6.2;
	keys[18][6] = AL::ALValue::array(-0.312414, AL::ALValue::array(2, -0.266667, 0.118941), AL::ALValue::array(2, 0.333333, -0.148676));
	times[18][7] = 7.2;
	keys[18][7] = AL::ALValue::array(-0.715585, AL::ALValue::array(2, -0.333333, 0.111331), AL::ALValue::array(2, 0.4, -0.133597));
	times[18][8] = 8.4;
	keys[18][8] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[18][9] = 9.4;
	keys[18][9] = AL::ALValue::array(0.076658, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[19] = "LAnkleRoll";
	times[19].arraySetSize(10);
	keys[19].arraySetSize(10);

	times[19][0] = 1;
	keys[19][0] = AL::ALValue::array(-0.00149204, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[19][1] = 1.7;
	keys[19][1] = AL::ALValue::array(-0.00149204, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[19][2] = 2.4;
	keys[19][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[19][3] = 3.4;
	keys[19][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, 4.19617e-05), AL::ALValue::array(2, 0.433333, -5.45502e-05));
	times[19][4] = 4.7;
	keys[19][4] = AL::ALValue::array(-0.680678, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[19][5] = 5.4;
	keys[19][5] = AL::ALValue::array(-0.555015, AL::ALValue::array(2, -0.233333, -0.059729), AL::ALValue::array(2, 0.266667, 0.0682618));
	times[19][6] = 6.2;
	keys[19][6] = AL::ALValue::array(-0.296706, AL::ALValue::array(2, -0.266667, -0.0667104), AL::ALValue::array(2, 0.333333, 0.083388));
	times[19][7] = 7.2;
	keys[19][7] = AL::ALValue::array(-0.10472, AL::ALValue::array(2, -0.333333, -0.0449554), AL::ALValue::array(2, 0.4, 0.0539465));
	times[19][8] = 8.4;
	keys[19][8] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[19][9] = 9.4;
	keys[19][9] = AL::ALValue::array(-0.13495, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[20] = "RHipRoll";
	times[20].arraySetSize(10);
	keys[20].arraySetSize(10);

	times[20][0] = 1;
	keys[20][0] = AL::ALValue::array(-0.084328, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[20][1] = 1.7;
	keys[20][1] = AL::ALValue::array(-0.085862, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[20][2] = 2.4;
	keys[20][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[20][3] = 3.4;
	keys[20][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[20][4] = 4.7;
	keys[20][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[20][5] = 5.4;
	keys[20][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[20][6] = 6.2;
	keys[20][6] = AL::ALValue::array(-0.0872665, AL::ALValue::array(2, -0.266667, 0.0139626), AL::ALValue::array(2, 0.333333, -0.0174533));
	times[20][7] = 7.2;
	keys[20][7] = AL::ALValue::array(-0.10472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	times[20][8] = 8.4;
	keys[20][8] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[20][9] = 9.4;
	keys[20][9] = AL::ALValue::array(-0.059784, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[21] = "RHipPitch";
	times[21].arraySetSize(10);
	keys[21].arraySetSize(10);

	times[21][0] = 1;
	keys[21][0] = AL::ALValue::array(0.076658, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[21][1] = 1.7;
	keys[21][1] = AL::ALValue::array(0.0812601, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[21][2] = 2.4;
	keys[21][2] = AL::ALValue::array(-4.19617e-05, AL::ALValue::array(2, -0.233333, 0.081302), AL::ALValue::array(2, 0.333333, -0.116146));
	times[21][3] = 3.4;
	keys[21][3] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[21][4] = 4.7;
	keys[21][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[21][5] = 5.4;
	keys[21][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[21][6] = 6.2;
	keys[21][6] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[21][7] = 7.2;
	keys[21][7] = AL::ALValue::array(-1.06989, AL::ALValue::array(2, -0.333333, -0.0189077), AL::ALValue::array(2, 0.4, 0.0226893));
	times[21][8] = 8.4;
	keys[21][8] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.4, -0.0226893), AL::ALValue::array(2, 0.333333, 0.0189077));
	times[21][9] = 9.4;
	keys[21][9] = AL::ALValue::array(0.210117, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[22] = "RKneePitch";
	times[22].arraySetSize(10);
	keys[22].arraySetSize(10);

	times[22][0] = 1;
	keys[22][0] = AL::ALValue::array(-0.078192, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[22][1] = 1.7;
	keys[22][1] = AL::ALValue::array(-0.076658, AL::ALValue::array(2, -0.233333, -0.00153397), AL::ALValue::array(2, 0.233333, 0.00153397));
	times[22][2] = 2.4;
	keys[22][2] = AL::ALValue::array(2.09548, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[22][3] = 3.4;
	keys[22][3] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.333333, 0.00108204), AL::ALValue::array(2, 0.433333, -0.00140666));
	times[22][4] = 4.7;
	keys[22][4] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.433333, 0.0648265), AL::ALValue::array(2, 0.233333, -0.0349066));
	times[22][5] = 5.4;
	keys[22][5] = AL::ALValue::array(1.01229, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[22][6] = 6.2;
	keys[22][6] = AL::ALValue::array(2.12019, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[22][7] = 7.2;
	keys[22][7] = AL::ALValue::array(2.12019, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	times[22][8] = 8.4;
	keys[22][8] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.4, 0.0257959), AL::ALValue::array(2, 0.333333, -0.0214966));
	times[22][9] = 9.4;
	keys[22][9] = AL::ALValue::array(-0.076658, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[23] = "RAnklePitch";
	times[23].arraySetSize(10);
	keys[23].arraySetSize(10);

	times[23][0] = 1;
	keys[23][0] = AL::ALValue::array(0.929646, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[23][1] = 1.7;
	keys[23][1] = AL::ALValue::array(0.93118, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[23][2] = 2.4;
	keys[23][2] = AL::ALValue::array(-1.13512, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[23][3] = 3.4;
	keys[23][3] = AL::ALValue::array(-1.13446, AL::ALValue::array(2, -0.333333, -0.000654472), AL::ALValue::array(2, 0.433333, 0.000850813));
	times[23][4] = 4.7;
	keys[23][4] = AL::ALValue::array(-0.783653, AL::ALValue::array(2, -0.433333, -0.264708), AL::ALValue::array(2, 0.233333, 0.142535));
	times[23][5] = 5.4;
	keys[23][5] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[23][6] = 6.2;
	keys[23][6] = AL::ALValue::array(-0.312414, AL::ALValue::array(2, -0.266667, 0.118941), AL::ALValue::array(2, 0.333333, -0.148676));
	times[23][7] = 7.2;
	keys[23][7] = AL::ALValue::array(-0.715585, AL::ALValue::array(2, -0.333333, 0.111331), AL::ALValue::array(2, 0.4, -0.133597));
	times[23][8] = 8.4;
	keys[23][8] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[23][9] = 9.4;
	keys[23][9] = AL::ALValue::array(0.0859459, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[24] = "RAnkleRoll";
	times[24].arraySetSize(10);
	keys[24].arraySetSize(10);

	times[24][0] = 1;
	keys[24][0] = AL::ALValue::array(0.00157596, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[24][1] = 1.7;
	keys[24][1] = AL::ALValue::array(0.00157596, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[24][2] = 2.4;
	keys[24][2] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, 2.93127e-05), AL::ALValue::array(2, 0.333333, -4.18753e-05));
	times[24][3] = 3.4;
	keys[24][3] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[24][4] = 4.7;
	keys[24][4] = AL::ALValue::array(0.680678, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[24][5] = 5.4;
	keys[24][5] = AL::ALValue::array(0.555015, AL::ALValue::array(2, -0.233333, 0.059729), AL::ALValue::array(2, 0.266667, -0.0682618));
	times[24][6] = 6.2;
	keys[24][6] = AL::ALValue::array(0.296706, AL::ALValue::array(2, -0.266667, 0.0667104), AL::ALValue::array(2, 0.333333, -0.083388));
	times[24][7] = 7.2;
	keys[24][7] = AL::ALValue::array(0.10472, AL::ALValue::array(2, -0.333333, 0.0449554), AL::ALValue::array(2, 0.4, -0.0539465));
	times[24][8] = 8.4;
	keys[24][8] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[24][9] = 9.4;
	keys[24][9] = AL::ALValue::array(0.0614019, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	actionPID = motion->post.angleInterpolationBezier(names, times, keys);

}

void MotionController::ALstandUpBack2010() {

	AL::ALValue names, times, keys;
	names.arraySetSize(25);
	times.arraySetSize(25);
	keys.arraySetSize(25);

	names[0] = "HeadYaw";
	times[0].arraySetSize(10);
	keys[0].arraySetSize(10);

	times[0][0] = 1;
	keys[0][0] = AL::ALValue::array(-0.0245859, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[0][1] = 2;
	keys[0][1] = AL::ALValue::array(-4.76838e-07, AL::ALValue::array(2, -0.333333, -1.43898e-06), AL::ALValue::array(2, 0.266667, 1.15119e-06));
	times[0][2] = 2.8;
	keys[0][2] = AL::ALValue::array(6.7435e-07, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[0][3] = 3.5;
	keys[0][3] = AL::ALValue::array(3.37177e-07, AL::ALValue::array(2, -0.233333, 2.44455e-12), AL::ALValue::array(2, 0.166667, -1.7461e-12));
	times[0][4] = 4;
	keys[0][4] = AL::ALValue::array(3.37175e-07, AL::ALValue::array(2, -0.166667, 1.7461e-12), AL::ALValue::array(2, 0.266667, -2.79377e-12));
	times[0][5] = 4.8;
	keys[0][5] = AL::ALValue::array(-0.513931, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[0][6] = 5.7;
	keys[0][6] = AL::ALValue::array(-0.30224, AL::ALValue::array(2, -0.3, -0.0796626), AL::ALValue::array(2, 0.333333, 0.0885141));
	times[0][7] = 6.7;
	keys[0][7] = AL::ALValue::array(-0.00940132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[0][8] = 7.3;
	keys[0][8] = AL::ALValue::array(-0.00940132, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[0][9] = 9.4;
	keys[0][9] = AL::ALValue::array(0.06592, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[1] = "HeadPitch";
	times[1].arraySetSize(10);
	keys[1].arraySetSize(10);

	times[1][0] = 1;
	keys[1][0] = AL::ALValue::array(0.0628521, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[1][1] = 2;
	keys[1][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, 0.0628521), AL::ALValue::array(2, 0.266667, -0.0502817));
	times[1][2] = 2.8;
	keys[1][2] = AL::ALValue::array(-0.785398, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[1][3] = 3.5;
	keys[1][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.233333, -0.22059), AL::ALValue::array(2, 0.166667, 0.157564));
	times[1][4] = 4;
	keys[1][4] = AL::ALValue::array(0.349066, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[1][5] = 4.8;
	keys[1][5] = AL::ALValue::array(0.171766, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[1][6] = 5.7;
	keys[1][6] = AL::ALValue::array(0.345107, AL::ALValue::array(2, -0.3, -0.0303761), AL::ALValue::array(2, 0.333333, 0.0337512));
	times[1][7] = 6.7;
	keys[1][7] = AL::ALValue::array(0.378859, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[1][8] = 7.3;
	keys[1][8] = AL::ALValue::array(0.378859, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[1][9] = 9.4;
	keys[1][9] = AL::ALValue::array(-0.039926, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[2] = "LShoulderPitch";
	times[2].arraySetSize(10);
	keys[2].arraySetSize(10);

	times[2][0] = 1;
	keys[2][0] = AL::ALValue::array(2.0856, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[2][1] = 2;
	keys[2][1] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[2][2] = 2.8;
	keys[2][2] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[2][3] = 3.5;
	keys[2][3] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[2][4] = 4;
	keys[2][4] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[2][5] = 4.8;
	keys[2][5] = AL::ALValue::array(2.03251, AL::ALValue::array(2, -0.266667, 0.0618893), AL::ALValue::array(2, 0.3, -0.0696255));
	times[2][6] = 5.7;
	keys[2][6] = AL::ALValue::array(1.28698, AL::ALValue::array(2, -0.3, 0.205152), AL::ALValue::array(2, 0.333333, -0.227947));
	times[2][7] = 6.7;
	keys[2][7] = AL::ALValue::array(0.733209, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[2][8] = 7.3;
	keys[2][8] = AL::ALValue::array(0.733209, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[2][9] = 9.4;
	keys[2][9] = AL::ALValue::array(1.59225, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[3] = "LShoulderRoll";
	times[3].arraySetSize(10);
	keys[3].arraySetSize(10);

	times[3][0] = 1;
	keys[3][0] = AL::ALValue::array(0.980184, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[3][1] = 2;
	keys[3][1] = AL::ALValue::array(0.366519, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[3][2] = 2.8;
	keys[3][2] = AL::ALValue::array(0.523599, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[3][3] = 3.5;
	keys[3][3] = AL::ALValue::array(0.366519, AL::ALValue::array(2, -0.233333, 0.101811), AL::ALValue::array(2, 0.166667, -0.0727221));
	times[3][4] = 4;
	keys[3][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[3][5] = 4.8;
	keys[3][5] = AL::ALValue::array(0.299088, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[3][6] = 5.7;
	keys[3][6] = AL::ALValue::array(0.0705221, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[3][7] = 6.7;
	keys[3][7] = AL::ALValue::array(0.498508, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[3][8] = 7.3;
	keys[3][8] = AL::ALValue::array(0.498508, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[3][9] = 9.4;
	keys[3][9] = AL::ALValue::array(0.21932, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[4] = "LElbowYaw";
	times[4].arraySetSize(10);
	keys[4].arraySetSize(10);

	times[4][0] = 1;
	keys[4][0] = AL::ALValue::array(-0.760906, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[4][1] = 2;
	keys[4][1] = AL::ALValue::array(0.15708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[4][2] = 2.8;
	keys[4][2] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[4][3] = 3.5;
	keys[4][3] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[4][4] = 4;
	keys[4][4] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[4][5] = 4.8;
	keys[4][5] = AL::ALValue::array(-1.96049, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[4][6] = 5.7;
	keys[4][6] = AL::ALValue::array(-0.85448, AL::ALValue::array(2, -0.3, -0.322669), AL::ALValue::array(2, 0.333333, 0.358521));
	times[4][7] = 6.7;
	keys[4][7] = AL::ALValue::array(0.0830765, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[4][8] = 7.3;
	keys[4][8] = AL::ALValue::array(0.0830765, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[4][9] = 9.4;
	keys[4][9] = AL::ALValue::array(-1.03089, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[5] = "LElbowRoll";
	times[5].arraySetSize(11);
	keys[5].arraySetSize(11);

	times[5][0] = 1;
	keys[5][0] = AL::ALValue::array(-0.624296, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[5][1] = 2;
	keys[5][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[5][2] = 2.8;
	keys[5][2] = AL::ALValue::array(-1.65806, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[5][3] = 3.5;
	keys[5][3] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.233333, -0.322401), AL::ALValue::array(2, 0.166667, 0.230287));
	times[5][4] = 4;
	keys[5][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.133333, 0.0));
	times[5][5] = 4.4;
	keys[5][5] = AL::ALValue::array(-0.191986, AL::ALValue::array(2, -0.133333, 0.174358), AL::ALValue::array(2, 0.133333, -0.174358));
	times[5][6] = 4.8;
	keys[5][6] = AL::ALValue::array(-1.04615, AL::ALValue::array(2, -0.133333, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[5][7] = 5.7;
	keys[5][7] = AL::ALValue::array(-0.872804, AL::ALValue::array(2, -0.3, -0.0380199), AL::ALValue::array(2, 0.333333, 0.0422443));
	times[5][8] = 6.7;
	keys[5][8] = AL::ALValue::array(-0.805354, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[5][9] = 7.3;
	keys[5][9] = AL::ALValue::array(-0.805354, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[5][10] = 9.4;
	keys[5][10] = AL::ALValue::array(-0.681054, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[6] = "LWristYaw";
	times[6].arraySetSize(4);
	keys[6].arraySetSize(4);

	times[6][0] = 1;
	keys[6][0] = AL::ALValue::array(-0.705682, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 1.26667, 0.0));
	times[6][1] = 4.8;
	keys[6][1] = AL::ALValue::array(-0.725624, AL::ALValue::array(2, -1.26667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[6][2] = 5.7;
	keys[6][2] = AL::ALValue::array(-0.719487, AL::ALValue::array(2, -0.3, -0.000900363), AL::ALValue::array(2, 1.23333, 0.00370149));
	times[6][3] = 9.4;
	keys[6][3] = AL::ALValue::array(-0.711819, AL::ALValue::array(2, -1.23333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[7] = "LHand";
	times[7].arraySetSize(4);
	keys[7].arraySetSize(4);

	times[7][0] = 1;
	keys[7][0] = AL::ALValue::array(0.0481964, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 1.26667, 0.0));
	times[7][1] = 4.8;
	keys[7][1] = AL::ALValue::array(0.120453, AL::ALValue::array(2, -1.26667, -0.0269052), AL::ALValue::array(2, 0.3, 0.00637229));
	times[7][2] = 5.7;
	keys[7][2] = AL::ALValue::array(0.148029, AL::ALValue::array(2, -0.3, -0.00830924), AL::ALValue::array(2, 1.23333, 0.0341602));
	times[7][3] = 9.4;
	keys[7][3] = AL::ALValue::array(0.247861, AL::ALValue::array(2, -1.23333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[8] = "RShoulderPitch";
	times[8].arraySetSize(10);
	keys[8].arraySetSize(10);

	times[8][0] = 1;
	keys[8][0] = AL::ALValue::array(2.0856, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[8][1] = 2;
	keys[8][1] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[8][2] = 2.8;
	keys[8][2] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[8][3] = 3.5;
	keys[8][3] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[8][4] = 4;
	keys[8][4] = AL::ALValue::array(2.0944, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[8][5] = 4.8;
	keys[8][5] = AL::ALValue::array(2.07708, AL::ALValue::array(2, -0.266667, 0.00897168), AL::ALValue::array(2, 0.3, -0.0100931));
	times[8][6] = 5.7;
	keys[8][6] = AL::ALValue::array(2.0372, AL::ALValue::array(2, -0.3, 0.0398808), AL::ALValue::array(2, 0.333333, -0.044312));
	times[8][7] = 6.7;
	keys[8][7] = AL::ALValue::array(0.891306, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[8][8] = 7.3;
	keys[8][8] = AL::ALValue::array(0.891306, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[8][9] = 9.4;
	keys[8][9] = AL::ALValue::array(1.46961, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[9] = "RShoulderRoll";
	times[9].arraySetSize(10);
	keys[9].arraySetSize(10);

	times[9][0] = 1;
	keys[9][0] = AL::ALValue::array(-1.07384, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[9][1] = 2;
	keys[9][1] = AL::ALValue::array(-0.366519, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[9][2] = 2.8;
	keys[9][2] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[9][3] = 3.5;
	keys[9][3] = AL::ALValue::array(-0.366519, AL::ALValue::array(2, -0.233333, -0.101811), AL::ALValue::array(2, 0.166667, 0.0727221));
	times[9][4] = 4;
	keys[9][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[9][5] = 4.8;
	keys[9][5] = AL::ALValue::array(-0.457173, AL::ALValue::array(2, -0.266667, 0.0885576), AL::ALValue::array(2, 0.3, -0.0996273));
	times[9][6] = 5.7;
	keys[9][6] = AL::ALValue::array(-0.564555, AL::ALValue::array(2, -0.3, 0.0656039), AL::ALValue::array(2, 0.333333, -0.0728932));
	times[9][7] = 6.7;
	keys[9][7] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[9][8] = 7.3;
	keys[9][8] = AL::ALValue::array(-0.680678, AL::ALValue::array(2, -0.2, -0.053503), AL::ALValue::array(2, 0.7, 0.187261));
	times[9][9] = 9.4;
	keys[9][9] = AL::ALValue::array(-0.150374, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[10] = "RElbowYaw";
	times[10].arraySetSize(10);
	keys[10].arraySetSize(10);

	times[10][0] = 1;
	keys[10][0] = AL::ALValue::array(0.765425, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[10][1] = 2;
	keys[10][1] = AL::ALValue::array(-0.15708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[10][2] = 2.8;
	keys[10][2] = AL::ALValue::array(-0.0872665, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[10][3] = 3.5;
	keys[10][3] = AL::ALValue::array(-0.0872665, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[10][4] = 4;
	keys[10][4] = AL::ALValue::array(-0.0872665, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[10][5] = 4.8;
	keys[10][5] = AL::ALValue::array(-0.081344, AL::ALValue::array(2, -0.266667, -0.00285404), AL::ALValue::array(2, 0.3, 0.0032108));
	times[10][6] = 5.7;
	keys[10][6] = AL::ALValue::array(-0.0690719, AL::ALValue::array(2, -0.3, -0.0122721), AL::ALValue::array(2, 0.333333, 0.0136357));
	times[10][7] = 6.7;
	keys[10][7] = AL::ALValue::array(0.000615569, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[10][8] = 7.3;
	keys[10][8] = AL::ALValue::array(0.000615569, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[10][9] = 9.4;
	keys[10][9] = AL::ALValue::array(0.912689, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[11] = "RElbowRoll";
	times[11].arraySetSize(10);
	keys[11].arraySetSize(10);

	times[11][0] = 1;
	keys[11][0] = AL::ALValue::array(0.710284, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[11][1] = 2;
	keys[11][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[11][2] = 2.8;
	keys[11][2] = AL::ALValue::array(1.65806, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[11][3] = 3.5;
	keys[11][3] = AL::ALValue::array(0.698132, AL::ALValue::array(2, -0.233333, 0.322401), AL::ALValue::array(2, 0.166667, -0.230287));
	times[11][4] = 4;
	keys[11][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[11][5] = 4.8;
	keys[11][5] = AL::ALValue::array(0.0966839, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[11][6] = 5.7;
	keys[11][6] = AL::ALValue::array(0.021518, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[11][7] = 6.7;
	keys[11][7] = AL::ALValue::array(0.453786, AL::ALValue::array(2, -0.333333, -0.112174), AL::ALValue::array(2, 0.2, 0.0673043));
	times[11][8] = 7.3;
	keys[11][8] = AL::ALValue::array(0.559952, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[11][9] = 9.4;
	keys[11][9] = AL::ALValue::array(0.380475, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[12] = "RWristYaw";
	times[12].arraySetSize(4);
	keys[12].arraySetSize(4);

	times[12][0] = 1;
	keys[12][0] = AL::ALValue::array(1.15199, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 1.26667, 0.0));
	times[12][1] = 4.8;
	keys[12][1] = AL::ALValue::array(1.07989, AL::ALValue::array(2, -1.26667, 0.00647765), AL::ALValue::array(2, 0.3, -0.00153418));
	times[12][2] = 5.7;
	keys[12][2] = AL::ALValue::array(1.07836, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 1.23333, 0.0));
	times[12][3] = 9.4;
	keys[12][3] = AL::ALValue::array(1.0845, AL::ALValue::array(2, -1.23333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[13] = "RHand";
	times[13].arraySetSize(6);
	keys[13].arraySetSize(6);

	times[13][0] = 1;
	keys[13][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[13][1] = 2;
	keys[13][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.5, 0.0));
	times[13][2] = 3.5;
	keys[13][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.5, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[13][3] = 4.8;
	keys[13][3] = AL::ALValue::array(0.157803, AL::ALValue::array(2, -0.433333, -0.0100841), AL::ALValue::array(2, 0.3, 0.00698131));
	times[13][4] = 5.7;
	keys[13][4] = AL::ALValue::array(0.164784, AL::ALValue::array(2, -0.3, -0.00318709), AL::ALValue::array(2, 1.23333, 0.0131025));
	times[13][5] = 9.4;
	keys[13][5] = AL::ALValue::array(0.206671, AL::ALValue::array(2, -1.23333, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[14] = "LHipYawPitch";
	times[14].arraySetSize(9);
	keys[14].arraySetSize(9);

	times[14][0] = 1;
	keys[14][0] = AL::ALValue::array(-0.470897, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[14][1] = 2;
	keys[14][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[14][2] = 2.8;
	keys[14][2] = AL::ALValue::array(-4.76838e-07, AL::ALValue::array(2, -0.266667, 4.76838e-07), AL::ALValue::array(2, 0.233333, -4.17233e-07));
	times[14][3] = 3.5;
	keys[14][3] = AL::ALValue::array(-0.654977, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.433333, 0.0));
	times[14][4] = 4.8;
	keys[14][4] = AL::ALValue::array(-0.498508, AL::ALValue::array(2, -0.433333, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[14][5] = 5.7;
	keys[14][5] = AL::ALValue::array(-0.858999, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[14][6] = 6.7;
	keys[14][6] = AL::ALValue::array(-0.402255, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[14][7] = 7.3;
	keys[14][7] = AL::ALValue::array(-0.402255, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[14][8] = 9.4;
	keys[14][8] = AL::ALValue::array(-0.230059, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[15] = "LHipRoll";
	times[15].arraySetSize(10);
	keys[15].arraySetSize(10);

	times[15][0] = 1;
	keys[15][0] = AL::ALValue::array(0.0445279, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[15][1] = 2;
	keys[15][1] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[15][2] = 2.8;
	keys[15][2] = AL::ALValue::array(1.56923e-07, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[15][3] = 3.5;
	keys[15][3] = AL::ALValue::array(-0.0647821, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[15][4] = 4;
	keys[15][4] = AL::ALValue::array(0.541052, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[15][5] = 4.8;
	keys[15][5] = AL::ALValue::array(0.154976, AL::ALValue::array(2, -0.266667, 0.112296), AL::ALValue::array(2, 0.3, -0.126333));
	times[15][6] = 5.7;
	keys[15][6] = AL::ALValue::array(-0.174835, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[15][7] = 6.7;
	keys[15][7] = AL::ALValue::array(0.191986, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[15][8] = 7.3;
	keys[15][8] = AL::ALValue::array(0.174533, AL::ALValue::array(2, -0.2, 0.00296877), AL::ALValue::array(2, 0.7, -0.0103907));
	times[15][9] = 9.4;
	keys[15][9] = AL::ALValue::array(0.151908, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[16] = "LHipPitch";
	times[16].arraySetSize(10);
	keys[16].arraySetSize(10);

	times[16][0] = 1;
	keys[16][0] = AL::ALValue::array(0.250085, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[16][1] = 2;
	keys[16][1] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[16][2] = 2.8;
	keys[16][2] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[16][3] = 3.5;
	keys[16][3] = AL::ALValue::array(-1.17808, AL::ALValue::array(2, -0.233333, 0.271496), AL::ALValue::array(2, 0.166667, -0.193925));
	times[16][4] = 4;
	keys[16][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[16][5] = 4.8;
	keys[16][5] = AL::ALValue::array(-0.857465, AL::ALValue::array(2, -0.266667, -0.237503), AL::ALValue::array(2, 0.3, 0.267191));
	times[16][6] = 5.7;
	keys[16][6] = AL::ALValue::array(-0.056716, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[16][7] = 6.7;
	keys[16][7] = AL::ALValue::array(-0.855211, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[16][8] = 7.3;
	keys[16][8] = AL::ALValue::array(-0.835988, AL::ALValue::array(2, -0.2, -0.019223), AL::ALValue::array(2, 0.7, 0.0672806));
	times[16][9] = 9.4;
	keys[16][9] = AL::ALValue::array(0.213269, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[17] = "LKneePitch";
	times[17].arraySetSize(10);
	keys[17].arraySetSize(10);

	times[17][0] = 1;
	keys[17][0] = AL::ALValue::array(0.091998, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[17][1] = 2;
	keys[17][1] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[17][2] = 2.8;
	keys[17][2] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[17][3] = 3.5;
	keys[17][3] = AL::ALValue::array(1.07777, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[17][4] = 4;
	keys[17][4] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.166667, -0.132434), AL::ALValue::array(2, 0.266667, 0.211894));
	times[17][5] = 4.8;
	keys[17][5] = AL::ALValue::array(2.11075, AL::ALValue::array(2, -0.266667, -0.00134981), AL::ALValue::array(2, 0.3, 0.00151853));
	times[17][6] = 5.7;
	keys[17][6] = AL::ALValue::array(2.11227, AL::ALValue::array(2, -0.3, -0.00151853), AL::ALValue::array(2, 0.333333, 0.00168726));
	times[17][7] = 6.7;
	keys[17][7] = AL::ALValue::array(2.20585, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[17][8] = 7.3;
	keys[17][8] = AL::ALValue::array(2.20585, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[17][9] = 9.4;
	keys[17][9] = AL::ALValue::array(-0.0890139, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[18] = "LAnklePitch";
	times[18].arraySetSize(10);
	keys[18].arraySetSize(10);

	times[18][0] = 1;
	keys[18][0] = AL::ALValue::array(0.825249, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[18][1] = 2;
	keys[18][1] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[18][2] = 2.8;
	keys[18][2] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[18][3] = 3.5;
	keys[18][3] = AL::ALValue::array(0.677476, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[18][4] = 4;
	keys[18][4] = AL::ALValue::array(0.663225, AL::ALValue::array(2, -0.166667, 0.0142506), AL::ALValue::array(2, 0.266667, -0.022801));
	times[18][5] = 4.8;
	keys[18][5] = AL::ALValue::array(-0.455639, AL::ALValue::array(2, -0.266667, 0.276813), AL::ALValue::array(2, 0.3, -0.311414));
	times[18][6] = 5.7;
	keys[18][6] = AL::ALValue::array(-1.10145, AL::ALValue::array(2, -0.3, 0.108248), AL::ALValue::array(2, 0.333333, -0.120276));
	times[18][7] = 6.7;
	keys[18][7] = AL::ALValue::array(-1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[18][8] = 7.3;
	keys[18][8] = AL::ALValue::array(-1.22173, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[18][9] = 9.4;
	keys[18][9] = AL::ALValue::array(0.0812601, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[19] = "LAnkleRoll";
	times[19].arraySetSize(10);
	keys[19].arraySetSize(10);

	times[19][0] = 1;
	keys[19][0] = AL::ALValue::array(-0.033706, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[19][1] = 2;
	keys[19][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[19][2] = 2.8;
	keys[19][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[19][3] = 3.5;
	keys[19][3] = AL::ALValue::array(0.171085, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[19][4] = 4;
	keys[19][4] = AL::ALValue::array(-0.10472, AL::ALValue::array(2, -0.166667, 0.0726687), AL::ALValue::array(2, 0.266667, -0.11627));
	times[19][5] = 4.8;
	keys[19][5] = AL::ALValue::array(-0.395731, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[19][6] = 5.7;
	keys[19][6] = AL::ALValue::array(-0.0996681, AL::ALValue::array(2, -0.3, -0.0811319), AL::ALValue::array(2, 0.333333, 0.0901466));
	times[19][7] = 6.7;
	keys[19][7] = AL::ALValue::array(0.118105, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[19][8] = 7.3;
	keys[19][8] = AL::ALValue::array(0.0872665, AL::ALValue::array(2, -0.2, 0.0188584), AL::ALValue::array(2, 0.7, -0.0660045));
	times[19][9] = 9.4;
	keys[19][9] = AL::ALValue::array(-0.136484, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[20] = "RHipRoll";
	times[20].arraySetSize(10);
	keys[20].arraySetSize(10);

	times[20][0] = 1;
	keys[20][0] = AL::ALValue::array(-0.148756, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[20][1] = 2;
	keys[20][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[20][2] = 2.8;
	keys[20][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[20][3] = 3.5;
	keys[20][3] = AL::ALValue::array(0.0618955, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[20][4] = 4;
	keys[20][4] = AL::ALValue::array(-0.541052, AL::ALValue::array(2, -0.166667, 0.0108014), AL::ALValue::array(2, 0.266667, -0.0172822));
	times[20][5] = 4.8;
	keys[20][5] = AL::ALValue::array(-0.558334, AL::ALValue::array(2, -0.266667, 0.0108922), AL::ALValue::array(2, 0.3, -0.0122538));
	times[20][6] = 5.7;
	keys[20][6] = AL::ALValue::array(-0.61049, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[20][7] = 6.7;
	keys[20][7] = AL::ALValue::array(-0.296706, AL::ALValue::array(2, -0.333333, -0.123549), AL::ALValue::array(2, 0.2, 0.0741296));
	times[20][8] = 7.3;
	keys[20][8] = AL::ALValue::array(-0.0174533, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[20][9] = 9.4;
	keys[20][9] = AL::ALValue::array(-0.0628521, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[21] = "RHipPitch";
	times[21].arraySetSize(10);
	keys[21].arraySetSize(10);

	times[21][0] = 1;
	keys[21][0] = AL::ALValue::array(0.185572, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[21][1] = 2;
	keys[21][1] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[21][2] = 2.8;
	keys[21][2] = AL::ALValue::array(-0.174533, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[21][3] = 3.5;
	keys[21][3] = AL::ALValue::array(-1.16456, AL::ALValue::array(2, -0.233333, 0.271496), AL::ALValue::array(2, 0.166667, -0.193925));
	times[21][4] = 4;
	keys[21][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[21][5] = 4.8;
	keys[21][5] = AL::ALValue::array(-1.52484, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[21][6] = 5.7;
	keys[21][6] = AL::ALValue::array(-1.56012, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[21][7] = 6.7;
	keys[21][7] = AL::ALValue::array(-0.905826, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[21][8] = 7.3;
	keys[21][8] = AL::ALValue::array(-0.905826, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[21][9] = 9.4;
	keys[21][9] = AL::ALValue::array(0.208583, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[22] = "RKneePitch";
	times[22].arraySetSize(10);
	keys[22].arraySetSize(10);

	times[22][0] = 1;
	keys[22][0] = AL::ALValue::array(0.191792, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[22][1] = 2;
	keys[22][1] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[22][2] = 2.8;
	keys[22][2] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[22][3] = 3.5;
	keys[22][3] = AL::ALValue::array(1.09386, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[22][4] = 4;
	keys[22][4] = AL::ALValue::array(1.67552, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[22][5] = 4.8;
	keys[22][5] = AL::ALValue::array(1.22111, AL::ALValue::array(2, -0.266667, 0.0902895), AL::ALValue::array(2, 0.3, -0.101576));
	times[22][6] = 5.7;
	keys[22][6] = AL::ALValue::array(1.09992, AL::ALValue::array(2, -0.3, 0.0544659), AL::ALValue::array(2, 0.333333, -0.0605176));
	times[22][7] = 6.7;
	keys[22][7] = AL::ALValue::array(0.876155, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[22][8] = 7.3;
	keys[22][8] = AL::ALValue::array(1.76278, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[22][9] = 9.4;
	keys[22][9] = AL::ALValue::array(-0.076658, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[23] = "RAnklePitch";
	times[23].arraySetSize(10);
	keys[23].arraySetSize(10);

	times[23][0] = 1;
	keys[23][0] = AL::ALValue::array(0.466378, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[23][1] = 2;
	keys[23][1] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[23][2] = 2.8;
	keys[23][2] = AL::ALValue::array(0.244346, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[23][3] = 3.5;
	keys[23][3] = AL::ALValue::array(0.717365, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[23][4] = 4;
	keys[23][4] = AL::ALValue::array(0.663225, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[23][5] = 4.8;
	keys[23][5] = AL::ALValue::array(0.785451, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.3, 0.0));
	times[23][6] = 5.7;
	keys[23][6] = AL::ALValue::array(0.783916, AL::ALValue::array(2, -0.3, 0.00153411), AL::ALValue::array(2, 0.333333, -0.00170457));
	times[23][7] = 6.7;
	keys[23][7] = AL::ALValue::array(0.403171, AL::ALValue::array(2, -0.333333, 0.284035), AL::ALValue::array(2, 0.2, -0.170421));
	times[23][8] = 7.3;
	keys[23][8] = AL::ALValue::array(-0.579449, AL::ALValue::array(2, -0.2, -0.0), AL::ALValue::array(2, 0.7, 0.0));
	times[23][9] = 9.4;
	keys[23][9] = AL::ALValue::array(0.081344, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	names[24] = "RAnkleRoll";
	times[24].arraySetSize(10);
	keys[24].arraySetSize(10);

	times[24][0] = 1;
	keys[24][0] = AL::ALValue::array(0.167248, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[24][1] = 2;
	keys[24][1] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[24][2] = 2.8;
	keys[24][2] = AL::ALValue::array(8.63852e-08, AL::ALValue::array(2, -0.266667, -0.0), AL::ALValue::array(2, 0.233333, 0.0));
	times[24][3] = 3.5;
	keys[24][3] = AL::ALValue::array(-0.160838, AL::ALValue::array(2, -0.233333, -0.0), AL::ALValue::array(2, 0.166667, 0.0));
	times[24][4] = 4;
	keys[24][4] = AL::ALValue::array(0.10472, AL::ALValue::array(2, -0.166667, -0.0), AL::ALValue::array(2, 0.266667, 0.0));
	times[24][5] = 4.8;
	keys[24][5] = AL::ALValue::array(0.00924597, AL::ALValue::array(2, -0.266667, 0.0318202), AL::ALValue::array(2, 0.3, -0.0357977));
	times[24][6] = 5.7;
	keys[24][6] = AL::ALValue::array(-0.0981341, AL::ALValue::array(2, -0.3, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	times[24][7] = 6.7;
	keys[24][7] = AL::ALValue::array(0.679603, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.2, 0.0));
	times[24][8] = 7.3;
	keys[24][8] = AL::ALValue::array(0.277507, AL::ALValue::array(2, -0.2, 0.0456791), AL::ALValue::array(2, 0.7, -0.159877));
	times[24][9] = 9.4;
	keys[24][9] = AL::ALValue::array(0.0629359, AL::ALValue::array(2, -0.7, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	actionPID = motion->post.angleInterpolationBezier(names, times, keys);

}

void MotionController::loadActions() {

	/************************* Lie Down *********************************/

	LieDown_names.arraySetSize(21);
	LieDown_times.arraySetSize(21);
	LieDown_keys.arraySetSize(21);

	LieDown_names[0] = "HeadYaw";
	LieDown_times[0].arraySetSize(5);
	LieDown_keys[0].arraySetSize(5);

	LieDown_times[0][0] = 2;
	LieDown_keys[0][0] = AL::ALValue::array(-0.680678, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[0][1] = 3;
	LieDown_keys[0][1] = AL::ALValue::array(0.750492, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[0][2] = 4;
	LieDown_keys[0][2] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[0][3] = 5;
	LieDown_keys[0][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[0][4] = 7;
	LieDown_keys[0][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[1] = "HeadPitch";
	LieDown_times[1].arraySetSize(6);
	LieDown_keys[1].arraySetSize(6);

	LieDown_times[1][0] = 1;
	LieDown_keys[1][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[1][1] = 2;
	LieDown_keys[1][1] = AL::ALValue::array(-0.0191986, AL::ALValue::array(2, -0.333333, 0.0191986), AL::ALValue::array(2, 0.333333, -0.0191986));
	LieDown_times[1][2] = 3;
	LieDown_keys[1][2] = AL::ALValue::array(-0.15708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[1][3] = 4;
	LieDown_keys[1][3] = AL::ALValue::array(-0.15708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[1][4] = 5;
	LieDown_keys[1][4] = AL::ALValue::array(-0.785398, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[1][5] = 7;
	LieDown_keys[1][5] = AL::ALValue::array(-0.785398, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[2] = "LShoulderPitch";
	LieDown_times[2].arraySetSize(6);
	LieDown_keys[2].arraySetSize(6);

	LieDown_times[2][0] = 1;
	LieDown_keys[2][0] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[2][1] = 2;
	LieDown_keys[2][1] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[2][2] = 3;
	LieDown_keys[2][2] = AL::ALValue::array(0.715585, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[2][3] = 4;
	LieDown_keys[2][3] = AL::ALValue::array(1.27409, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[2][4] = 5;
	LieDown_keys[2][4] = AL::ALValue::array(-0.453786, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[2][5] = 7;
	LieDown_keys[2][5] = AL::ALValue::array(-0.453786, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[3] = "LShoulderRoll";
	LieDown_times[3].arraySetSize(6);
	LieDown_keys[3].arraySetSize(6);

	LieDown_times[3][0] = 1;
	LieDown_keys[3][0] = AL::ALValue::array(0.314159, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[3][1] = 2;
	LieDown_keys[3][1] = AL::ALValue::array(0.314159, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[3][2] = 3;
	LieDown_keys[3][2] = AL::ALValue::array(0.645772, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[3][3] = 4;
	LieDown_keys[3][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[3][4] = 5;
	LieDown_keys[3][4] = AL::ALValue::array(0.907571, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[3][5] = 7;
	LieDown_keys[3][5] = AL::ALValue::array(0.907571, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[4] = "LElbowYaw";
	LieDown_times[4].arraySetSize(6);
	LieDown_keys[4].arraySetSize(6);

	LieDown_times[4][0] = 1;
	LieDown_keys[4][0] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[4][1] = 2;
	LieDown_keys[4][1] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[4][2] = 3;
	LieDown_keys[4][2] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[4][3] = 4;
	LieDown_keys[4][3] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[4][4] = 5;
	LieDown_keys[4][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[4][5] = 7;
	LieDown_keys[4][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[5] = "LElbowRoll";
	LieDown_times[5].arraySetSize(6);
	LieDown_keys[5].arraySetSize(6);

	LieDown_times[5][0] = 1;
	LieDown_keys[5][0] = AL::ALValue::array(-0.139626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[5][1] = 2;
	LieDown_keys[5][1] = AL::ALValue::array(-0.139626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[5][2] = 3;
	LieDown_keys[5][2] = AL::ALValue::array(-1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[5][3] = 4;
	LieDown_keys[5][3] = AL::ALValue::array(-1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[5][4] = 5;
	LieDown_keys[5][4] = AL::ALValue::array(-1.01229, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[5][5] = 7;
	LieDown_keys[5][5] = AL::ALValue::array(-1.01229, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[6] = "RShoulderPitch";
	LieDown_times[6].arraySetSize(6);
	LieDown_keys[6].arraySetSize(6);

	LieDown_times[6][0] = 1;
	LieDown_keys[6][0] = AL::ALValue::array(1.62316, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[6][1] = 2;
	LieDown_keys[6][1] = AL::ALValue::array(0.715585, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[6][2] = 3;
	LieDown_keys[6][2] = AL::ALValue::array(0.715585, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[6][3] = 4;
	LieDown_keys[6][3] = AL::ALValue::array(1.27409, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[6][4] = 5;
	LieDown_keys[6][4] = AL::ALValue::array(-0.453786, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[6][5] = 7;
	LieDown_keys[6][5] = AL::ALValue::array(-0.453786, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[7] = "RShoulderRoll";
	LieDown_times[7].arraySetSize(6);
	LieDown_keys[7].arraySetSize(6);

	LieDown_times[7][0] = 1;
	LieDown_keys[7][0] = AL::ALValue::array(-0.314159, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[7][1] = 2;
	LieDown_keys[7][1] = AL::ALValue::array(-0.645772, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[7][2] = 3;
	LieDown_keys[7][2] = AL::ALValue::array(-0.645772, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[7][3] = 4;
	LieDown_keys[7][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[7][4] = 5;
	LieDown_keys[7][4] = AL::ALValue::array(-0.907571, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[7][5] = 7;
	LieDown_keys[7][5] = AL::ALValue::array(-0.907571, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[8] = "RElbowYaw";
	LieDown_times[8].arraySetSize(6);
	LieDown_keys[8].arraySetSize(6);

	LieDown_times[8][0] = 1;
	LieDown_keys[8][0] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[8][1] = 2;
	LieDown_keys[8][1] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[8][2] = 3;
	LieDown_keys[8][2] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[8][3] = 4;
	LieDown_keys[8][3] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[8][4] = 5;
	LieDown_keys[8][4] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[8][5] = 7;
	LieDown_keys[8][5] = AL::ALValue::array(1.5708, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[9] = "RElbowRoll";
	LieDown_times[9].arraySetSize(6);
	LieDown_keys[9].arraySetSize(6);

	LieDown_times[9][0] = 1;
	LieDown_keys[9][0] = AL::ALValue::array(0.139626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[9][1] = 2;
	LieDown_keys[9][1] = AL::ALValue::array(1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[9][2] = 3;
	LieDown_keys[9][2] = AL::ALValue::array(1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[9][3] = 4;
	LieDown_keys[9][3] = AL::ALValue::array(1.22173, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[9][4] = 5;
	LieDown_keys[9][4] = AL::ALValue::array(1.01229, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[9][5] = 7;
	LieDown_keys[9][5] = AL::ALValue::array(1.01229, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[10] = "LHipYawPitch";
	LieDown_times[10].arraySetSize(6);
	LieDown_keys[10].arraySetSize(6);

	LieDown_times[10][0] = 1;
	LieDown_keys[10][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[10][1] = 2;
	LieDown_keys[10][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[10][2] = 3;
	LieDown_keys[10][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[10][3] = 4;
	LieDown_keys[10][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[10][4] = 5;
	LieDown_keys[10][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[10][5] = 7;
	LieDown_keys[10][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[11] = "LHipRoll";
	LieDown_times[11].arraySetSize(6);
	LieDown_keys[11].arraySetSize(6);

	LieDown_times[11][0] = 1;
	LieDown_keys[11][0] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[11][1] = 2;
	LieDown_keys[11][1] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[11][2] = 3;
	LieDown_keys[11][2] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[11][3] = 4;
	LieDown_keys[11][3] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[11][4] = 5;
	LieDown_keys[11][4] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[11][5] = 7;
	LieDown_keys[11][5] = AL::ALValue::array(-0.010472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[12] = "LHipPitch";
	LieDown_times[12].arraySetSize(6);
	LieDown_keys[12].arraySetSize(6);

	LieDown_times[12][0] = 1;
	LieDown_keys[12][0] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[12][1] = 2;
	LieDown_keys[12][1] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[12][2] = 3;
	LieDown_keys[12][2] = AL::ALValue::array(-0.610865, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[12][3] = 4;
	LieDown_keys[12][3] = AL::ALValue::array(-0.959931, AL::ALValue::array(2, -0.333333, 0.159989), AL::ALValue::array(2, 0.333333, -0.159989));
	LieDown_times[12][4] = 5;
	LieDown_keys[12][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[12][5] = 7;
	LieDown_keys[12][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[13] = "LKneePitch";
	LieDown_times[13].arraySetSize(6);
	LieDown_keys[13].arraySetSize(6);

	LieDown_times[13][0] = 1;
	LieDown_keys[13][0] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[13][1] = 2;
	LieDown_keys[13][1] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[13][2] = 3;
	LieDown_keys[13][2] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[13][3] = 4;
	LieDown_keys[13][3] = AL::ALValue::array(1.74533, AL::ALValue::array(2, -0.333333, -0.0756309), AL::ALValue::array(2, 0.333333, 0.0756309));
	LieDown_times[13][4] = 5;
	LieDown_keys[13][4] = AL::ALValue::array(1.85005, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[13][5] = 7;
	LieDown_keys[13][5] = AL::ALValue::array(1.85005, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[14] = "LAnklePitch";
	LieDown_times[14].arraySetSize(6);
	LieDown_keys[14].arraySetSize(6);

	LieDown_times[14][0] = 1;
	LieDown_keys[14][0] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[14][1] = 2;
	LieDown_keys[14][1] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[14][2] = 3;
	LieDown_keys[14][2] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[14][3] = 4;
	LieDown_keys[14][3] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, 0.0581776), AL::ALValue::array(2, 0.333333, -0.0581776));
	LieDown_times[14][4] = 5;
	LieDown_keys[14][4] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[14][5] = 7;
	LieDown_keys[14][5] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[15] = "LAnkleRoll";
	LieDown_times[15].arraySetSize(6);
	LieDown_keys[15].arraySetSize(6);

	LieDown_times[15][0] = 1;
	LieDown_keys[15][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[15][1] = 2;
	LieDown_keys[15][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[15][2] = 3;
	LieDown_keys[15][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[15][3] = 4;
	LieDown_keys[15][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[15][4] = 5;
	LieDown_keys[15][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[15][5] = 7;
	LieDown_keys[15][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[16] = "RHipRoll";
	LieDown_times[16].arraySetSize(6);
	LieDown_keys[16].arraySetSize(6);

	LieDown_times[16][0] = 1;
	LieDown_keys[16][0] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[16][1] = 2;
	LieDown_keys[16][1] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[16][2] = 3;
	LieDown_keys[16][2] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[16][3] = 4;
	LieDown_keys[16][3] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[16][4] = 5;
	LieDown_keys[16][4] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[16][5] = 7;
	LieDown_keys[16][5] = AL::ALValue::array(0.010472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[17] = "RHipPitch";
	LieDown_times[17].arraySetSize(6);
	LieDown_keys[17].arraySetSize(6);

	LieDown_times[17][0] = 1;
	LieDown_keys[17][0] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[17][1] = 2;
	LieDown_keys[17][1] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[17][2] = 3;
	LieDown_keys[17][2] = AL::ALValue::array(-0.610865, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[17][3] = 4;
	LieDown_keys[17][3] = AL::ALValue::array(-0.959931, AL::ALValue::array(2, -0.333333, 0.159989), AL::ALValue::array(2, 0.333333, -0.159989));
	LieDown_times[17][4] = 5;
	LieDown_keys[17][4] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[17][5] = 7;
	LieDown_keys[17][5] = AL::ALValue::array(-1.5708, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[18] = "RKneePitch";
	LieDown_times[18].arraySetSize(6);
	LieDown_keys[18].arraySetSize(6);

	LieDown_times[18][0] = 1;
	LieDown_keys[18][0] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[18][1] = 2;
	LieDown_keys[18][1] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[18][2] = 3;
	LieDown_keys[18][2] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[18][3] = 4;
	LieDown_keys[18][3] = AL::ALValue::array(1.74533, AL::ALValue::array(2, -0.333333, -0.0756309), AL::ALValue::array(2, 0.333333, 0.0756309));
	LieDown_times[18][4] = 5;
	LieDown_keys[18][4] = AL::ALValue::array(1.85005, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[18][5] = 7;
	LieDown_keys[18][5] = AL::ALValue::array(1.85005, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[19] = "RAnklePitch";
	LieDown_times[19].arraySetSize(6);
	LieDown_keys[19].arraySetSize(6);

	LieDown_times[19][0] = 1;
	LieDown_keys[19][0] = AL::ALValue::array(-0.523599, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[19][1] = 2;
	LieDown_keys[19][1] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[19][2] = 3;
	LieDown_keys[19][2] = AL::ALValue::array(-0.698132, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[19][3] = 4;
	LieDown_keys[19][3] = AL::ALValue::array(-0.872665, AL::ALValue::array(2, -0.333333, 0.0581776), AL::ALValue::array(2, 0.333333, -0.0581776));
	LieDown_times[19][4] = 5;
	LieDown_keys[19][4] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[19][5] = 7;
	LieDown_keys[19][5] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LieDown_names[20] = "RAnkleRoll";
	LieDown_times[20].arraySetSize(6);
	LieDown_keys[20].arraySetSize(6);

	LieDown_times[20][0] = 1;
	LieDown_keys[20][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[20][1] = 2;
	LieDown_keys[20][1] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[20][2] = 3;
	LieDown_keys[20][2] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[20][3] = 4;
	LieDown_keys[20][3] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.333333, 0.0));
	LieDown_times[20][4] = 5;
	LieDown_keys[20][4] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.333333, -0.0), AL::ALValue::array(2, 0.666667, 0.0));
	LieDown_times[20][5] = 7;
	LieDown_keys[20][5] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));


	/************************* Left Kick *********************************/

	LeftKick_names.arraySetSize(23);
	LeftKick_times.arraySetSize(23);
	LeftKick_keys.arraySetSize(23);

/*
	LeftKick_names[0] = "HeadYaw";
	LeftKick_times[0].arraySetSize(6);
	LeftKick_keys[0].arraySetSize(6);

	LeftKick_times[0][0] = 1.13333;
	LeftKick_keys[0][0] = AL::ALValue::array(0.021518, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[0][1] = 3.2;
	LeftKick_keys[0][1] = AL::ALValue::array(0.030638, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[0][2] = 4.06667;
	LeftKick_keys[0][2] = AL::ALValue::array(0.030722, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[0][3] = 4.4;
	LeftKick_keys[0][3] = AL::ALValue::array(0.0261199, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[0][4] = 5.53333;
	LeftKick_keys[0][4] = AL::ALValue::array(0.032172, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[0][5] = 6.73333;
	LeftKick_keys[0][5] = AL::ALValue::array(0.010696, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[1] = "HeadPitch";
	LeftKick_times[1].arraySetSize(6);
	LeftKick_keys[1].arraySetSize(6);

	LeftKick_times[1][0] = 1.13333;
	LeftKick_keys[1][0] = AL::ALValue::array(-0.016916, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[1][1] = 3.2;
	LeftKick_keys[1][1] = AL::ALValue::array(-0.0337899, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[1][2] = 4.06667;
	LeftKick_keys[1][2] = AL::ALValue::array(-0.012314, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[1][3] = 4.4;
	LeftKick_keys[1][3] = AL::ALValue::array(-0.023052, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[1][4] = 5.53333;
	LeftKick_keys[1][4] = AL::ALValue::array(-0.030722, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[1][5] = 6.73333;
	LeftKick_keys[1][5] = AL::ALValue::array(0.00762803, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));
*/

	LeftKick_names[2] = "LShoulderPitch";
	LeftKick_times[2].arraySetSize(6);
	LeftKick_keys[2].arraySetSize(6);

	LeftKick_times[2][0] = 1.13333;
	LeftKick_keys[2][0] = AL::ALValue::array(1.43126, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[2][1] = 3.2;
	LeftKick_keys[2][1] = AL::ALValue::array(1.52015, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[2][2] = 4.06667;
	LeftKick_keys[2][2] = AL::ALValue::array(1.76414, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[2][3] = 4.4;
	LeftKick_keys[2][3] = AL::ALValue::array(2.06635, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[2][4] = 5.53333;
	LeftKick_keys[2][4] = AL::ALValue::array(1.5539, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[2][5] = 6.73333;
	LeftKick_keys[2][5] = AL::ALValue::array(1.41584, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[3] = "LShoulderRoll";
	LeftKick_times[3].arraySetSize(6);
	LeftKick_keys[3].arraySetSize(6);

	LeftKick_times[3][0] = 1.13333;
	LeftKick_keys[3][0] = AL::ALValue::array(0.358999, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[3][1] = 3.2;
	LeftKick_keys[3][1] = AL::ALValue::array(0.762356, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[3][2] = 4.06667;
	LeftKick_keys[3][2] = AL::ALValue::array(1.10606, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[3][3] = 4.4;
	LeftKick_keys[3][3] = AL::ALValue::array(0.403483, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[3][4] = 5.53333;
	LeftKick_keys[3][4] = AL::ALValue::array(0.728609, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[3][5] = 6.73333;
	LeftKick_keys[3][5] = AL::ALValue::array(0.352778, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[4] = "LElbowYaw";
	LeftKick_times[4].arraySetSize(6);
	LeftKick_keys[4].arraySetSize(6);

	LeftKick_times[4][0] = 1.13333;
	LeftKick_keys[4][0] = AL::ALValue::array(-1.37749, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[4][1] = 3.2;
	LeftKick_keys[4][1] = AL::ALValue::array(-1.39138, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[4][2] = 4.06667;
	LeftKick_keys[4][2] = AL::ALValue::array(-1.88984, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[4][3] = 4.4;
	LeftKick_keys[4][3] = AL::ALValue::array(-1.76098, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[4][4] = 5.53333;
	LeftKick_keys[4][4] = AL::ALValue::array(-1.39752, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[4][5] = 6.73333;
	LeftKick_keys[4][5] = AL::ALValue::array(-1.39445, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[5] = "LElbowRoll";
	LeftKick_times[5].arraySetSize(6);
	LeftKick_keys[5].arraySetSize(6);

	LeftKick_times[5][0] = 1.13333;
	LeftKick_keys[5][0] = AL::ALValue::array(-1.0631, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[5][1] = 3.2;
	LeftKick_keys[5][1] = AL::ALValue::array(-1.01393, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[5][2] = 4.06667;
	LeftKick_keys[5][2] = AL::ALValue::array(-1.02782, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[5][3] = 4.4;
	LeftKick_keys[5][3] = AL::ALValue::array(-0.112024, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[5][4] = 5.53333;
	LeftKick_keys[5][4] = AL::ALValue::array(-0.998592, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[5][5] = 6.73333;
	LeftKick_keys[5][5] = AL::ALValue::array(-1.02927, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[6] = "LWristYaw";
	LeftKick_times[6].arraySetSize(6);
	LeftKick_keys[6].arraySetSize(6);

	LeftKick_times[6][0] = 1.13333;
	LeftKick_keys[6][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[6][1] = 3.2;
	LeftKick_keys[6][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[6][2] = 4.06667;
	LeftKick_keys[6][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[6][3] = 4.4;
	LeftKick_keys[6][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[6][4] = 5.53333;
	LeftKick_keys[6][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[6][5] = 6.73333;
	LeftKick_keys[6][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[7] = "LHand";
	LeftKick_times[7].arraySetSize(6);
	LeftKick_keys[7].arraySetSize(6);

	LeftKick_times[7][0] = 1.13333;
	LeftKick_keys[7][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[7][1] = 3.2;
	LeftKick_keys[7][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[7][2] = 4.06667;
	LeftKick_keys[7][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[7][3] = 4.4;
	LeftKick_keys[7][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[7][4] = 5.53333;
	LeftKick_keys[7][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[7][5] = 6.73333;
	LeftKick_keys[7][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[8] = "RShoulderPitch";
	LeftKick_times[8].arraySetSize(6);
	LeftKick_keys[8].arraySetSize(6);

	LeftKick_times[8][0] = 1.13333;
	LeftKick_keys[8][0] = AL::ALValue::array(1.43732, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[8][1] = 3.2;
	LeftKick_keys[8][1] = AL::ALValue::array(1.4374, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[8][2] = 4.06667;
	LeftKick_keys[8][2] = AL::ALValue::array(1.71957, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[8][3] = 4.4;
	LeftKick_keys[8][3] = AL::ALValue::array(1.69196, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[8][4] = 5.53333;
	LeftKick_keys[8][4] = AL::ALValue::array(1.47115, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[8][5] = 6.73333;
	LeftKick_keys[8][5] = AL::ALValue::array(1.42973, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[9] = "RShoulderRoll";
	LeftKick_times[9].arraySetSize(6);
	LeftKick_keys[9].arraySetSize(6);

	LeftKick_times[9][0] = 1.13333;
	LeftKick_keys[9][0] = AL::ALValue::array(-0.361981, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[9][1] = 3.2;
	LeftKick_keys[9][1] = AL::ALValue::array(-0.426494, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[9][2] = 4.06667;
	LeftKick_keys[9][2] = AL::ALValue::array(-0.371186, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[9][3] = 4.4;
	LeftKick_keys[9][3] = AL::ALValue::array(-0.386526, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[9][4] = 5.53333;
	LeftKick_keys[9][4] = AL::ALValue::array(-0.408086, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[9][5] = 6.73333;
	LeftKick_keys[9][5] = AL::ALValue::array(-0.342125, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[10] = "RElbowYaw";
	LeftKick_times[10].arraySetSize(6);
	LeftKick_keys[10].arraySetSize(6);

	LeftKick_times[10][0] = 1.13333;
	LeftKick_keys[10][0] = AL::ALValue::array(1.37757, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[10][1] = 3.2;
	LeftKick_keys[10][1] = AL::ALValue::array(1.05995, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[10][2] = 4.06667;
	LeftKick_keys[10][2] = AL::ALValue::array(1.24718, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[10][3] = 4.4;
	LeftKick_keys[10][3] = AL::ALValue::array(1.23491, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[10][4] = 5.53333;
	LeftKick_keys[10][4] = AL::ALValue::array(1.07069, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[10][5] = 6.73333;
	LeftKick_keys[10][5] = AL::ALValue::array(1.38363, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[11] = "RElbowRoll";
	LeftKick_times[11].arraySetSize(6);
	LeftKick_keys[11].arraySetSize(6);

	LeftKick_times[11][0] = 1.13333;
	LeftKick_keys[11][0] = AL::ALValue::array(1.02314, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[11][1] = 3.2;
	LeftKick_keys[11][1] = AL::ALValue::array(0.753235, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[11][2] = 4.06667;
	LeftKick_keys[11][2] = AL::ALValue::array(0.575208, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[11][3] = 4.4;
	LeftKick_keys[11][3] = AL::ALValue::array(0.58748, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[11][4] = 5.53333;
	LeftKick_keys[11][4] = AL::ALValue::array(0.722556, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[11][5] = 6.73333;
	LeftKick_keys[11][5] = AL::ALValue::array(1.02475, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[12] = "RWristYaw";
	LeftKick_times[12].arraySetSize(6);
	LeftKick_keys[12].arraySetSize(6);

	LeftKick_times[12][0] = 1.13333;
	LeftKick_keys[12][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[12][1] = 3.2;
	LeftKick_keys[12][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[12][2] = 4.06667;
	LeftKick_keys[12][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[12][3] = 4.4;
	LeftKick_keys[12][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[12][4] = 5.53333;
	LeftKick_keys[12][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[12][5] = 6.73333;
	LeftKick_keys[12][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[13] = "RHand";
	LeftKick_times[13].arraySetSize(6);
	LeftKick_keys[13].arraySetSize(6);

	LeftKick_times[13][0] = 1.13333;
	LeftKick_keys[13][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[13][1] = 3.2;
	LeftKick_keys[13][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[13][2] = 4.06667;
	LeftKick_keys[13][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[13][3] = 4.4;
	LeftKick_keys[13][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[13][4] = 5.53333;
	LeftKick_keys[13][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[13][5] = 6.73333;
	LeftKick_keys[13][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[14] = "LHipYawPitch";
	LeftKick_times[14].arraySetSize(6);
	LeftKick_keys[14].arraySetSize(6);

	LeftKick_times[14][0] = 1.13333;
	LeftKick_keys[14][0] = AL::ALValue::array(-0.00456004, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[14][1] = 3.2;
	LeftKick_keys[14][1] = AL::ALValue::array(-0.167164, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[14][2] = 4.06667;
	LeftKick_keys[14][2] = AL::ALValue::array(-0.101202, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[14][3] = 4.4;
	LeftKick_keys[14][3] = AL::ALValue::array(-0.0475121, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[14][4] = 5.53333;
	LeftKick_keys[14][4] = AL::ALValue::array(-0.174835, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[14][5] = 6.73333;
	LeftKick_keys[14][5] = AL::ALValue::array(-0.00609404, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[15] = "LHipRoll";
	LeftKick_times[15].arraySetSize(6);
	LeftKick_keys[15].arraySetSize(6);

	LeftKick_times[15][0] = 1.13333;
	LeftKick_keys[15][0] = AL::ALValue::array(0.00609404, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[15][1] = 3.2;
	LeftKick_keys[15][1] = AL::ALValue::array(-0.151824, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[15][2] = 4.06667;
	LeftKick_keys[15][2] = AL::ALValue::array(-0.12728, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[15][3] = 4.4;
	LeftKick_keys[15][3] = AL::ALValue::array(-0.128814, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[15][4] = 5.53333;
	LeftKick_keys[15][4] = AL::ALValue::array(-0.144154, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[15][5] = 6.73333;
	LeftKick_keys[15][5] = AL::ALValue::array(-0.00609404, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[16] = "LHipPitch";
	LeftKick_times[16].arraySetSize(6);
	LeftKick_keys[16].arraySetSize(6);

	LeftKick_times[16][0] = 1.13333;
	LeftKick_keys[16][0] = AL::ALValue::array(-0.44797, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[16][1] = 3.2;
	LeftKick_keys[16][1] = AL::ALValue::array(-0.420624, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[16][2] = 4.06667;
	LeftKick_keys[16][2] = AL::ALValue::array(0.357464, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[16][3] = 4.4;
	LeftKick_keys[16][3] = AL::ALValue::array(-1.15046, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[16][4] = 5.53333;
	LeftKick_keys[16][4] = AL::ALValue::array(-0.421808, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[16][5] = 6.73333;
	LeftKick_keys[16][5] = AL::ALValue::array(-0.443284, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[17] = "LKneePitch";
	LeftKick_times[17].arraySetSize(6);
	LeftKick_keys[17].arraySetSize(6);

	LeftKick_times[17][0] = 1.13333;
	LeftKick_keys[17][0] = AL::ALValue::array(0.699545, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[17][1] = 3.2;
	LeftKick_keys[17][1] = AL::ALValue::array(1.21335, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[17][2] = 4.06667;
	LeftKick_keys[17][2] = AL::ALValue::array(0.624296, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[17][3] = 4.4;
	LeftKick_keys[17][3] = AL::ALValue::array(1.22869, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[17][4] = 5.53333;
	LeftKick_keys[17][4] = AL::ALValue::array(1.20108, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[17][5] = 6.73333;
	LeftKick_keys[17][5] = AL::ALValue::array(0.708667, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[18] = "LAnklePitch";
	LeftKick_times[18].arraySetSize(6);
	LeftKick_keys[18].arraySetSize(6);

	LeftKick_times[18][0] = 1.13333;
	LeftKick_keys[18][0] = AL::ALValue::array(-0.360449, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[18][1] = 3.2;
	LeftKick_keys[18][1] = AL::ALValue::array(-0.618244, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[18][2] = 4.06667;
	LeftKick_keys[18][2] = AL::ALValue::array(-1.15054, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[18][3] = 4.4;
	LeftKick_keys[18][3] = AL::ALValue::array(-0.150374, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[18][4] = 5.53333;
	LeftKick_keys[18][4] = AL::ALValue::array(-0.619779, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[18][5] = 6.73333;
	LeftKick_keys[18][5] = AL::ALValue::array(-0.358999, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[19] = "LAnkleRoll";
	LeftKick_times[19].arraySetSize(6);
	LeftKick_keys[19].arraySetSize(6);

	LeftKick_times[19][0] = 1.13333;
	LeftKick_keys[19][0] = AL::ALValue::array(-0.00771196, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[19][1] = 3.2;
	LeftKick_keys[19][1] = AL::ALValue::array(-0.141086, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[19][2] = 4.06667;
	LeftKick_keys[19][2] = AL::ALValue::array(-0.0383081, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[19][3] = 4.4;
	LeftKick_keys[19][3] = AL::ALValue::array(-0.12728, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[19][4] = 5.53333;
	LeftKick_keys[19][4] = AL::ALValue::array(-0.138018, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[19][5] = 6.73333;
	LeftKick_keys[19][5] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[20] = "RHipRoll";
	LeftKick_times[20].arraySetSize(6);
	LeftKick_keys[20].arraySetSize(6);

	LeftKick_times[20][0] = 1.13333;
	LeftKick_keys[20][0] = AL::ALValue::array(-0.00617796, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[20][1] = 3.2;
	LeftKick_keys[20][1] = AL::ALValue::array(0.00310997, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[20][2] = 4.06667;
	LeftKick_keys[20][2] = AL::ALValue::array(0.00464395, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[20][3] = 4.4;
	LeftKick_keys[20][3] = AL::ALValue::array(0.0184499, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[20][4] = 5.53333;
	LeftKick_keys[20][4] = AL::ALValue::array(0.00924597, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[20][5] = 6.73333;
	LeftKick_keys[20][5] = AL::ALValue::array(-0.00609404, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[21] = "RHipPitch";
	LeftKick_times[21].arraySetSize(6);
	LeftKick_keys[21].arraySetSize(6);

	LeftKick_times[21][0] = 1.13333;
	LeftKick_keys[21][0] = AL::ALValue::array(-0.31765, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[21][1] = 3.2;
	LeftKick_keys[21][1] = AL::ALValue::array(-0.792379, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[21][2] = 4.06667;
	LeftKick_keys[21][2] = AL::ALValue::array(-0.911238, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[21][3] = 4.4;
	LeftKick_keys[21][3] = AL::ALValue::array(-0.18719, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[21][4] = 5.53333;
	LeftKick_keys[21][4] = AL::ALValue::array(-0.80079, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[21][5] = 6.73333;
	LeftKick_keys[21][5] = AL::ALValue::array(-0.44797, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[22] = "RKneePitch";
	LeftKick_times[22].arraySetSize(6);
	LeftKick_keys[22].arraySetSize(6);

	LeftKick_times[22][0] = 1.13333;
	LeftKick_keys[22][0] = AL::ALValue::array(0.690259, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[22][1] = 3.2;
	LeftKick_keys[22][1] = AL::ALValue::array(1.35917, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[22][2] = 4.06667;
	LeftKick_keys[22][2] = AL::ALValue::array(1.25025, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[22][3] = 4.4;
	LeftKick_keys[22][3] = AL::ALValue::array(1.09072, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[22][4] = 5.53333;
	LeftKick_keys[22][4] = AL::ALValue::array(1.37911, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[22][5] = 6.73333;
	LeftKick_keys[22][5] = AL::ALValue::array(0.70108, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[0] = "RAnklePitch";
	LeftKick_times[0].arraySetSize(6);
	LeftKick_keys[0].arraySetSize(6);

	LeftKick_times[0][0] = 1.13333;
	LeftKick_keys[0][0] = AL::ALValue::array(-0.352862, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[0][1] = 3.2;
	LeftKick_keys[0][1] = AL::ALValue::array(-0.566003, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[0][2] = 4.06667;
	LeftKick_keys[0][2] = AL::ALValue::array(-0.546063, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[0][3] = 4.4;
	LeftKick_keys[0][3] = AL::ALValue::array(-0.67952, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[0][4] = 5.53333;
	LeftKick_keys[0][4] = AL::ALValue::array(-0.553732, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[0][5] = 6.73333;
	LeftKick_keys[0][5] = AL::ALValue::array(-0.358915, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	LeftKick_names[1] = "RAnkleRoll";
	LeftKick_times[1].arraySetSize(6);
	LeftKick_keys[1].arraySetSize(6);

	LeftKick_times[1][0] = 1.13333;
	LeftKick_keys[1][0] = AL::ALValue::array(0.00762803, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.688889, 0.0));
	LeftKick_times[1][1] = 3.2;
	LeftKick_keys[1][1] = AL::ALValue::array(-0.314428, AL::ALValue::array(2, -0.688889, -0.0), AL::ALValue::array(2, 0.288889, 0.0));
	LeftKick_times[1][2] = 4.06667;
	LeftKick_keys[1][2] = AL::ALValue::array(-0.286815, AL::ALValue::array(2, -0.288889, -0.0), AL::ALValue::array(2, 0.111111, 0.0));
	LeftKick_times[1][3] = 4.4;
	LeftKick_keys[1][3] = AL::ALValue::array(-0.263807, AL::ALValue::array(2, -0.111111, -0.0), AL::ALValue::array(2, 0.377778, 0.0));
	LeftKick_times[1][4] = 5.53333;
	LeftKick_keys[1][4] = AL::ALValue::array(-0.315962, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.4, 0.0));
	LeftKick_times[1][5] = 6.73333;
	LeftKick_keys[1][5] = AL::ALValue::array(0.00771196, AL::ALValue::array(2, -0.4, -0.0), AL::ALValue::array(2, 0.0, 0.0));


	/************************* Right Kick *********************************/

	RightKick_names.arraySetSize(23);
	RightKick_times.arraySetSize(23);
	RightKick_keys.arraySetSize(23);

/*
	RightKick_names[0] = "HeadYaw";
	RightKick_times[0].arraySetSize(6);
	RightKick_keys[0].arraySetSize(6);

	RightKick_times[0][0] = 1.13333;
	RightKick_keys[0][0] = AL::ALValue::array(-0.021518, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[0][1] = 3.4;
	RightKick_keys[0][1] = AL::ALValue::array(-0.030638, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[0][2] = 4.06667;
	RightKick_keys[0][2] = AL::ALValue::array(-0.030722, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[0][3] = 4.33333;
	RightKick_keys[0][3] = AL::ALValue::array(-0.0261199, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[0][4] = 5.4;
	RightKick_keys[0][4] = AL::ALValue::array(-0.032172, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[0][5] = 6.86667;
	RightKick_keys[0][5] = AL::ALValue::array(-0.010696, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[1] = "HeadPitch";
	RightKick_times[1].arraySetSize(6);
	RightKick_keys[1].arraySetSize(6);

	RightKick_times[1][0] = 1.13333;
	RightKick_keys[1][0] = AL::ALValue::array(-0.016916, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[1][1] = 3.4;
	RightKick_keys[1][1] = AL::ALValue::array(-0.0337899, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[1][2] = 4.06667;
	RightKick_keys[1][2] = AL::ALValue::array(-0.012314, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[1][3] = 4.33333;
	RightKick_keys[1][3] = AL::ALValue::array(-0.023052, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[1][4] = 5.4;
	RightKick_keys[1][4] = AL::ALValue::array(-0.030722, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[1][5] = 6.86667;
	RightKick_keys[1][5] = AL::ALValue::array(0.00762803, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));
*/


	RightKick_names[2] = "LShoulderPitch";
	RightKick_times[2].arraySetSize(6);
	RightKick_keys[2].arraySetSize(6);

	RightKick_times[2][0] = 1.13333;
	RightKick_keys[2][0] = AL::ALValue::array(1.43732, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[2][1] = 3.4;
	RightKick_keys[2][1] = AL::ALValue::array(1.4374, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[2][2] = 4.06667;
	RightKick_keys[2][2] = AL::ALValue::array(1.71957, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[2][3] = 4.33333;
	RightKick_keys[2][3] = AL::ALValue::array(1.69196, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[2][4] = 5.4;
	RightKick_keys[2][4] = AL::ALValue::array(1.47115, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[2][5] = 6.86667;
	RightKick_keys[2][5] = AL::ALValue::array(1.42973, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[3] = "LShoulderRoll";
	RightKick_times[3].arraySetSize(6);
	RightKick_keys[3].arraySetSize(6);

	RightKick_times[3][0] = 1.13333;
	RightKick_keys[3][0] = AL::ALValue::array(0.361981, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[3][1] = 3.4;
	RightKick_keys[3][1] = AL::ALValue::array(0.426494, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[3][2] = 4.06667;
	RightKick_keys[3][2] = AL::ALValue::array(0.371186, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[3][3] = 4.33333;
	RightKick_keys[3][3] = AL::ALValue::array(0.386526, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[3][4] = 5.4;
	RightKick_keys[3][4] = AL::ALValue::array(0.408086, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[3][5] = 6.86667;
	RightKick_keys[3][5] = AL::ALValue::array(0.342125, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[4] = "LElbowYaw";
	RightKick_times[4].arraySetSize(6);
	RightKick_keys[4].arraySetSize(6);

	RightKick_times[4][0] = 1.13333;
	RightKick_keys[4][0] = AL::ALValue::array(-1.37757, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[4][1] = 3.4;
	RightKick_keys[4][1] = AL::ALValue::array(-1.05995, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[4][2] = 4.06667;
	RightKick_keys[4][2] = AL::ALValue::array(-1.24718, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[4][3] = 4.33333;
	RightKick_keys[4][3] = AL::ALValue::array(-1.23491, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[4][4] = 5.4;
	RightKick_keys[4][4] = AL::ALValue::array(-1.07069, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[4][5] = 6.86667;
	RightKick_keys[4][5] = AL::ALValue::array(-1.38363, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[5] = "LElbowRoll";
	RightKick_times[5].arraySetSize(6);
	RightKick_keys[5].arraySetSize(6);

	RightKick_times[5][0] = 1.13333;
	RightKick_keys[5][0] = AL::ALValue::array(-1.02314, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[5][1] = 3.4;
	RightKick_keys[5][1] = AL::ALValue::array(-0.753235, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[5][2] = 4.06667;
	RightKick_keys[5][2] = AL::ALValue::array(-0.575208, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[5][3] = 4.33333;
	RightKick_keys[5][3] = AL::ALValue::array(-0.58748, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[5][4] = 5.4;
	RightKick_keys[5][4] = AL::ALValue::array(-0.722556, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[5][5] = 6.86667;
	RightKick_keys[5][5] = AL::ALValue::array(-1.02475, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[6] = "LWristYaw";
	RightKick_times[6].arraySetSize(6);
	RightKick_keys[6].arraySetSize(6);

	RightKick_times[6][0] = 1.13333;
	RightKick_keys[6][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[6][1] = 3.4;
	RightKick_keys[6][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[6][2] = 4.06667;
	RightKick_keys[6][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[6][3] = 4.33333;
	RightKick_keys[6][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[6][4] = 5.4;
	RightKick_keys[6][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[6][5] = 6.86667;
	RightKick_keys[6][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[7] = "LHand";
	RightKick_times[7].arraySetSize(6);
	RightKick_keys[7].arraySetSize(6);

	RightKick_times[7][0] = 1.13333;
	RightKick_keys[7][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[7][1] = 3.4;
	RightKick_keys[7][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[7][2] = 4.06667;
	RightKick_keys[7][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[7][3] = 4.33333;
	RightKick_keys[7][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[7][4] = 5.4;
	RightKick_keys[7][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[7][5] = 6.86667;
	RightKick_keys[7][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[8] = "RShoulderPitch";
	RightKick_times[8].arraySetSize(6);
	RightKick_keys[8].arraySetSize(6);

	RightKick_times[8][0] = 1.13333;
	RightKick_keys[8][0] = AL::ALValue::array(1.43126, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[8][1] = 3.4;
	RightKick_keys[8][1] = AL::ALValue::array(1.52015, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[8][2] = 4.06667;
	RightKick_keys[8][2] = AL::ALValue::array(1.76414, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[8][3] = 4.33333;
	RightKick_keys[8][3] = AL::ALValue::array(2.06635, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[8][4] = 5.4;
	RightKick_keys[8][4] = AL::ALValue::array(1.5539, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[8][5] = 6.86667;
	RightKick_keys[8][5] = AL::ALValue::array(1.41584, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[9] = "RShoulderRoll";
	RightKick_times[9].arraySetSize(6);
	RightKick_keys[9].arraySetSize(6);

	RightKick_times[9][0] = 1.13333;
	RightKick_keys[9][0] = AL::ALValue::array(-0.358999, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[9][1] = 3.4;
	RightKick_keys[9][1] = AL::ALValue::array(-0.762356, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[9][2] = 4.06667;
	RightKick_keys[9][2] = AL::ALValue::array(-1.10606, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[9][3] = 4.33333;
	RightKick_keys[9][3] = AL::ALValue::array(-0.403483, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[9][4] = 5.4;
	RightKick_keys[9][4] = AL::ALValue::array(-0.728609, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[9][5] = 6.86667;
	RightKick_keys[9][5] = AL::ALValue::array(-0.352778, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[10] = "RElbowYaw";
	RightKick_times[10].arraySetSize(6);
	RightKick_keys[10].arraySetSize(6);

	RightKick_times[10][0] = 1.13333;
	RightKick_keys[10][0] = AL::ALValue::array(1.37749, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[10][1] = 3.4;
	RightKick_keys[10][1] = AL::ALValue::array(1.39138, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[10][2] = 4.06667;
	RightKick_keys[10][2] = AL::ALValue::array(1.88984, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[10][3] = 4.33333;
	RightKick_keys[10][3] = AL::ALValue::array(1.76098, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[10][4] = 5.4;
	RightKick_keys[10][4] = AL::ALValue::array(1.39752, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[10][5] = 6.86667;
	RightKick_keys[10][5] = AL::ALValue::array(1.39445, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[11] = "RElbowRoll";
	RightKick_times[11].arraySetSize(6);
	RightKick_keys[11].arraySetSize(6);

	RightKick_times[11][0] = 1.13333;
	RightKick_keys[11][0] = AL::ALValue::array(1.0631, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[11][1] = 3.4;
	RightKick_keys[11][1] = AL::ALValue::array(1.01393, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[11][2] = 4.06667;
	RightKick_keys[11][2] = AL::ALValue::array(1.02782, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[11][3] = 4.33333;
	RightKick_keys[11][3] = AL::ALValue::array(0.112024, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[11][4] = 5.4;
	RightKick_keys[11][4] = AL::ALValue::array(0.998592, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[11][5] = 6.86667;
	RightKick_keys[11][5] = AL::ALValue::array(1.02927, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[12] = "RWristYaw";
	RightKick_times[12].arraySetSize(6);
	RightKick_keys[12].arraySetSize(6);

	RightKick_times[12][0] = 1.13333;
	RightKick_keys[12][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[12][1] = 3.4;
	RightKick_keys[12][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[12][2] = 4.06667;
	RightKick_keys[12][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[12][3] = 4.33333;
	RightKick_keys[12][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[12][4] = 5.4;
	RightKick_keys[12][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[12][5] = 6.86667;
	RightKick_keys[12][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[13] = "RHand";
	RightKick_times[13].arraySetSize(6);
	RightKick_keys[13].arraySetSize(6);

	RightKick_times[13][0] = 1.13333;
	RightKick_keys[13][0] = AL::ALValue::array(0, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[13][1] = 3.4;
	RightKick_keys[13][1] = AL::ALValue::array(0, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[13][2] = 4.06667;
	RightKick_keys[13][2] = AL::ALValue::array(0, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[13][3] = 4.33333;
	RightKick_keys[13][3] = AL::ALValue::array(0, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[13][4] = 5.4;
	RightKick_keys[13][4] = AL::ALValue::array(0, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[13][5] = 6.86667;
	RightKick_keys[13][5] = AL::ALValue::array(0, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[14] = "LHipYawPitch";
	RightKick_times[14].arraySetSize(6);
	RightKick_keys[14].arraySetSize(6);

	RightKick_times[14][0] = 1.13333;
	RightKick_keys[14][0] = AL::ALValue::array(-0.00456004, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[14][1] = 3.4;
	RightKick_keys[14][1] = AL::ALValue::array(-0.167164, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[14][2] = 4.06667;
	RightKick_keys[14][2] = AL::ALValue::array(-0.101202, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[14][3] = 4.33333;
	RightKick_keys[14][3] = AL::ALValue::array(-0.0475121, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[14][4] = 5.4;
	RightKick_keys[14][4] = AL::ALValue::array(-0.174835, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[14][5] = 6.86667;
	RightKick_keys[14][5] = AL::ALValue::array(-0.00609404, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[15] = "LHipRoll";
	RightKick_times[15].arraySetSize(6);
	RightKick_keys[15].arraySetSize(6);

	RightKick_times[15][0] = 1.13333;
	RightKick_keys[15][0] = AL::ALValue::array(0.00617796, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[15][1] = 3.4;
	RightKick_keys[15][1] = AL::ALValue::array(-0.00310997, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[15][2] = 4.06667;
	RightKick_keys[15][2] = AL::ALValue::array(-0.00464395, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[15][3] = 4.33333;
	RightKick_keys[15][3] = AL::ALValue::array(-0.0184499, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[15][4] = 5.4;
	RightKick_keys[15][4] = AL::ALValue::array(-0.00924597, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[15][5] = 6.86667;
	RightKick_keys[15][5] = AL::ALValue::array(0.00609404, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[16] = "LHipPitch";
	RightKick_times[16].arraySetSize(6);
	RightKick_keys[16].arraySetSize(6);

	RightKick_times[16][0] = 1.13333;
	RightKick_keys[16][0] = AL::ALValue::array(-0.31765, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[16][1] = 3.4;
	RightKick_keys[16][1] = AL::ALValue::array(-0.792379, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[16][2] = 4.06667;
	RightKick_keys[16][2] = AL::ALValue::array(-0.911238, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[16][3] = 4.33333;
	RightKick_keys[16][3] = AL::ALValue::array(-0.18719, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[16][4] = 5.4;
	RightKick_keys[16][4] = AL::ALValue::array(-0.80079, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[16][5] = 6.86667;
	RightKick_keys[16][5] = AL::ALValue::array(-0.44797, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[17] = "LKneePitch";
	RightKick_times[17].arraySetSize(6);
	RightKick_keys[17].arraySetSize(6);

	RightKick_times[17][0] = 1.13333;
	RightKick_keys[17][0] = AL::ALValue::array(0.690259, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[17][1] = 3.4;
	RightKick_keys[17][1] = AL::ALValue::array(1.35917, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[17][2] = 4.06667;
	RightKick_keys[17][2] = AL::ALValue::array(1.25025, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[17][3] = 4.33333;
	RightKick_keys[17][3] = AL::ALValue::array(1.09072, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[17][4] = 5.4;
	RightKick_keys[17][4] = AL::ALValue::array(1.37911, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[17][5] = 6.86667;
	RightKick_keys[17][5] = AL::ALValue::array(0.70108, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[18] = "LAnklePitch";
	RightKick_times[18].arraySetSize(6);
	RightKick_keys[18].arraySetSize(6);

	RightKick_times[18][0] = 1.13333;
	RightKick_keys[18][0] = AL::ALValue::array(-0.352862, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[18][1] = 3.4;
	RightKick_keys[18][1] = AL::ALValue::array(-0.566003, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[18][2] = 4.06667;
	RightKick_keys[18][2] = AL::ALValue::array(-0.546063, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[18][3] = 4.33333;
	RightKick_keys[18][3] = AL::ALValue::array(-0.67952, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[18][4] = 5.4;
	RightKick_keys[18][4] = AL::ALValue::array(-0.553732, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[18][5] = 6.86667;
	RightKick_keys[18][5] = AL::ALValue::array(-0.358915, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[19] = "LAnkleRoll";
	RightKick_times[19].arraySetSize(6);
	RightKick_keys[19].arraySetSize(6);

	RightKick_times[19][0] = 1.13333;
	RightKick_keys[19][0] = AL::ALValue::array(-0.00762803, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[19][1] = 3.4;
	RightKick_keys[19][1] = AL::ALValue::array(0.314428, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[19][2] = 4.06667;
	RightKick_keys[19][2] = AL::ALValue::array(0.286815, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[19][3] = 4.33333;
	RightKick_keys[19][3] = AL::ALValue::array(0.263807, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[19][4] = 5.4;
	RightKick_keys[19][4] = AL::ALValue::array(0.315962, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[19][5] = 6.86667;
	RightKick_keys[19][5] = AL::ALValue::array(-0.00771196, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[20] = "RHipRoll";
	RightKick_times[20].arraySetSize(6);
	RightKick_keys[20].arraySetSize(6);

	RightKick_times[20][0] = 1.13333;
	RightKick_keys[20][0] = AL::ALValue::array(-0.00609404, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[20][1] = 3.4;
	RightKick_keys[20][1] = AL::ALValue::array(0.151824, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[20][2] = 4.06667;
	RightKick_keys[20][2] = AL::ALValue::array(0.12728, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[20][3] = 4.33333;
	RightKick_keys[20][3] = AL::ALValue::array(0.128814, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[20][4] = 5.4;
	RightKick_keys[20][4] = AL::ALValue::array(0.144154, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[20][5] = 6.86667;
	RightKick_keys[20][5] = AL::ALValue::array(0.00609404, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[21] = "RHipPitch";
	RightKick_times[21].arraySetSize(6);
	RightKick_keys[21].arraySetSize(6);

	RightKick_times[21][0] = 1.13333;
	RightKick_keys[21][0] = AL::ALValue::array(-0.44797, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[21][1] = 3.4;
	RightKick_keys[21][1] = AL::ALValue::array(-0.420624, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[21][2] = 4.06667;
	RightKick_keys[21][2] = AL::ALValue::array(0.357464, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[21][3] = 4.33333;
	RightKick_keys[21][3] = AL::ALValue::array(-1.15046, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[21][4] = 5.4;
	RightKick_keys[21][4] = AL::ALValue::array(-0.421808, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[21][5] = 6.86667;
	RightKick_keys[21][5] = AL::ALValue::array(-0.443284, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[22] = "RKneePitch";
	RightKick_times[22].arraySetSize(6);
	RightKick_keys[22].arraySetSize(6);

	RightKick_times[22][0] = 1.13333;
	RightKick_keys[22][0] = AL::ALValue::array(0.699545, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[22][1] = 3.4;
	RightKick_keys[22][1] = AL::ALValue::array(1.21335, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[22][2] = 4.06667;
	RightKick_keys[22][2] = AL::ALValue::array(0.624296, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[22][3] = 4.33333;
	RightKick_keys[22][3] = AL::ALValue::array(1.22869, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[22][4] = 5.4;
	RightKick_keys[22][4] = AL::ALValue::array(1.20108, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[22][5] = 6.86667;
	RightKick_keys[22][5] = AL::ALValue::array(0.708667, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[0] = "RAnklePitch";
	RightKick_times[0].arraySetSize(6);
	RightKick_keys[0].arraySetSize(6);

	RightKick_times[0][0] = 1.13333;
	RightKick_keys[0][0] = AL::ALValue::array(-0.360449, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[0][1] = 3.4;
	RightKick_keys[0][1] = AL::ALValue::array(-0.618244, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[0][2] = 4.06667;
	RightKick_keys[0][2] = AL::ALValue::array(-1.15054, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[0][3] = 4.33333;
	RightKick_keys[0][3] = AL::ALValue::array(-0.150374, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[0][4] = 5.4;
	RightKick_keys[0][4] = AL::ALValue::array(-0.619779, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[0][5] = 6.86667;
	RightKick_keys[0][5] = AL::ALValue::array(-0.358999, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));

	RightKick_names[1] = "RAnkleRoll";
	RightKick_times[1].arraySetSize(6);
	RightKick_keys[1].arraySetSize(6);

	RightKick_times[1][0] = 1.13333;
	RightKick_keys[1][0] = AL::ALValue::array(0.00771196, AL::ALValue::array(2, -0.377778, -0.0), AL::ALValue::array(2, 0.755556, 0.0));
	RightKick_times[1][1] = 3.4;
	RightKick_keys[1][1] = AL::ALValue::array(0.141086, AL::ALValue::array(2, -0.755556, -0.0), AL::ALValue::array(2, 0.222222, 0.0));
	RightKick_times[1][2] = 4.06667;
	RightKick_keys[1][2] = AL::ALValue::array(0.0383081, AL::ALValue::array(2, -0.222222, -0.0), AL::ALValue::array(2, 0.0888889, 0.0));
	RightKick_times[1][3] = 4.33333;
	RightKick_keys[1][3] = AL::ALValue::array(0.12728, AL::ALValue::array(2, -0.0888889, -0.0), AL::ALValue::array(2, 0.355556, 0.0));
	RightKick_times[1][4] = 5.4;
	RightKick_keys[1][4] = AL::ALValue::array(0.138018, AL::ALValue::array(2, -0.355556, -0.0), AL::ALValue::array(2, 0.488889, 0.0));
	RightKick_times[1][5] = 6.86667;
	RightKick_keys[1][5] = AL::ALValue::array(-4.19617e-05, AL::ALValue::array(2, -0.488889, -0.0), AL::ALValue::array(2, 0.0, 0.0));


	/******************************** Right Dive **********************************/
	
	RightDive_names.reserve(25);
	RightDive_times.arraySetSize(25);
	RightDive_keys.arraySetSize(25);

	RightDive_names.push_back("HeadYaw");
	RightDive_times[0].arraySetSize(3);
	RightDive_keys[0].arraySetSize(3);

	RightDive_times[0][0] = 0.100000;
	RightDive_keys[0][0] = AL::ALValue::array(0.286815, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[0][1] = 0.500000;
	RightDive_keys[0][1] = AL::ALValue::array(0.0843280, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[0][2] = 0.900000;
	RightDive_keys[0][2] = AL::ALValue::array(0.0873960, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("HeadPitch");
	RightDive_times[1].arraySetSize(3);
	RightDive_keys[1].arraySetSize(3);

	RightDive_times[1][0] = 0.100000;
	RightDive_keys[1][0] = AL::ALValue::array(0.165630, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[1][1] = 0.500000;
	RightDive_keys[1][1] = AL::ALValue::array(-0.164180, AL::ALValue::array(2, -0.133333, 0.00153396), AL::ALValue::array(2, 0.133333, -0.00153396));
	RightDive_times[1][2] = 0.900000;
	RightDive_keys[1][2] = AL::ALValue::array(-0.165714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LShoulderPitch");
	RightDive_times[2].arraySetSize(3);
	RightDive_keys[2].arraySetSize(3);

	RightDive_times[2][0] = 0.100000;
	RightDive_keys[2][0] = AL::ALValue::array(1.73798, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[2][1] = 0.500000;
	RightDive_keys[2][1] = AL::ALValue::array(1.30846, AL::ALValue::array(2, -0.133333, 0.429520), AL::ALValue::array(2, 0.133333, -0.429520));
	RightDive_times[2][2] = 0.900000;
	RightDive_keys[2][2] = AL::ALValue::array(-1.68284, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LShoulderRoll");
	RightDive_times[3].arraySetSize(3);
	RightDive_keys[3].arraySetSize(3);

	RightDive_times[3][0] = 0.100000;
	RightDive_keys[3][0] = AL::ALValue::array(0.265341, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[3][1] = 0.500000;
	RightDive_keys[3][1] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[3][2] = 0.900000;
	RightDive_keys[3][2] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LElbowYaw");
	RightDive_times[4].arraySetSize(3);
	RightDive_keys[4].arraySetSize(3);

	RightDive_times[4][0] = 0.100000;
	RightDive_keys[4][0] = AL::ALValue::array(-1.47882, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[4][1] = 0.500000;
	RightDive_keys[4][1] = AL::ALValue::array(-0.559952, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[4][2] = 0.900000;
	RightDive_keys[4][2] = AL::ALValue::array(-0.806927, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LElbowRoll");
	RightDive_times[5].arraySetSize(3);
	RightDive_keys[5].arraySetSize(3);

	RightDive_times[5][0] = 0.100000;
	RightDive_keys[5][0] = AL::ALValue::array(-0.515382, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[5][1] = 0.500000;
	RightDive_keys[5][1] = AL::ALValue::array(-0.977116, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[5][2] = 0.900000;
	RightDive_keys[5][2] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LWristYaw");
	RightDive_times[6].arraySetSize(3);
	RightDive_keys[6].arraySetSize(3);

	RightDive_times[6][0] = 0.100000;
	RightDive_keys[6][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[6][1] = 0.500000;
	RightDive_keys[6][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[6][2] = 0.900000;
	RightDive_keys[6][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LHand");
	RightDive_times[7].arraySetSize(3);
	RightDive_keys[7].arraySetSize(3);

	RightDive_times[7][0] = 0.100000;
	RightDive_keys[7][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[7][1] = 0.500000;
	RightDive_keys[7][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[7][2] = 0.900000;
	RightDive_keys[7][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RShoulderPitch");
	RightDive_times[8].arraySetSize(3);
	RightDive_keys[8].arraySetSize(3);

	RightDive_times[8][0] = 0.100000;
	RightDive_keys[8][0] = AL::ALValue::array(1.74573, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[8][1] = 0.500000;
	RightDive_keys[8][1] = AL::ALValue::array(1.19810, AL::ALValue::array(2, -0.133333, 0.547636), AL::ALValue::array(2, 0.133333, -0.547636));
	RightDive_times[8][2] = 0.900000;
	RightDive_keys[8][2] = AL::ALValue::array(-1.65975, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RShoulderRoll");
	RightDive_times[9].arraySetSize(3);
	RightDive_keys[9].arraySetSize(3);

	RightDive_times[9][0] = 0.100000;
	RightDive_keys[9][0] = AL::ALValue::array(-0.243948, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[9][1] = 0.500000;
	RightDive_keys[9][1] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[9][2] = 0.900000;
	RightDive_keys[9][2] = AL::ALValue::array(-0.0169160, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RElbowYaw");
	RightDive_times[10].arraySetSize(3);
	RightDive_keys[10].arraySetSize(3);

	RightDive_times[10][0] = 0.100000;
	RightDive_keys[10][0] = AL::ALValue::array(1.48180, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[10][1] = 0.500000;
	RightDive_keys[10][1] = AL::ALValue::array(0.806841, AL::ALValue::array(2, -0.133333, 0.184336), AL::ALValue::array(2, 0.133333, -0.184336));
	RightDive_times[10][2] = 0.900000;
	RightDive_keys[10][2] = AL::ALValue::array(0.375789, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RElbowRoll");
	RightDive_times[11].arraySetSize(3);
	RightDive_keys[11].arraySetSize(3);

	RightDive_times[11][0] = 0.100000;
	RightDive_keys[11][0] = AL::ALValue::array(0.524670, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[11][1] = 0.500000;
	RightDive_keys[11][1] = AL::ALValue::array(0.955723, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[11][2] = 0.900000;
	RightDive_keys[11][2] = AL::ALValue::array(0.0890139, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RWristYaw");
	RightDive_times[12].arraySetSize(3);
	RightDive_keys[12].arraySetSize(3);

	RightDive_times[12][0] = 0.100000;
	RightDive_keys[12][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[12][1] = 0.500000;
	RightDive_keys[12][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[12][2] = 0.900000;
	RightDive_keys[12][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RHand");
	RightDive_times[13].arraySetSize(3);
	RightDive_keys[13].arraySetSize(3);

	RightDive_times[13][0] = 0.100000;
	RightDive_keys[13][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[13][1] = 0.500000;
	RightDive_keys[13][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[13][2] = 0.900000;
	RightDive_keys[13][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LHipYawPitch");
	RightDive_times[14].arraySetSize(4);
	RightDive_keys[14].arraySetSize(4);

	RightDive_times[14][0] = 0.100000;
	RightDive_keys[14][0] = AL::ALValue::array(0.00464395, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[14][1] = 0.500000;
	RightDive_keys[14][1] = AL::ALValue::array(-0.0659200, AL::ALValue::array(2, -0.133333, 0.0232657), AL::ALValue::array(2, 0.133333, -0.0232657));
	RightDive_times[14][2] = 0.900000;
	RightDive_keys[14][2] = AL::ALValue::array(-0.134950, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[14][3] = 1.40000;
	RightDive_keys[14][3] = AL::ALValue::array(-0.0337060, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LHipRoll");
	RightDive_times[15].arraySetSize(4);
	RightDive_keys[15].arraySetSize(4);

	RightDive_times[15][0] = 0.100000;
	RightDive_keys[15][0] = AL::ALValue::array(-0.0152980, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[15][1] = 0.500000;
	RightDive_keys[15][1] = AL::ALValue::array(-0.138018, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[15][2] = 0.900000;
	RightDive_keys[15][2] = AL::ALValue::array(0.112024, AL::ALValue::array(2, -0.133333, -0.0474971), AL::ALValue::array(2, 0.166667, 0.0593714));
	RightDive_times[15][3] = 1.40000;
	RightDive_keys[15][3] = AL::ALValue::array(0.182588, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LHipPitch");
	RightDive_times[16].arraySetSize(4);
	RightDive_keys[16].arraySetSize(4);

	RightDive_times[16][0] = 0.100000;
	RightDive_keys[16][0] = AL::ALValue::array(-0.460158, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[16][1] = 0.500000;
	RightDive_keys[16][1] = AL::ALValue::array(-0.866668, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[16][2] = 0.900000;
	RightDive_keys[16][2] = AL::ALValue::array(0.0598679, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[16][3] = 1.40000;
	RightDive_keys[16][3] = AL::ALValue::array(0.0568000, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LKneePitch");
	RightDive_times[17].arraySetSize(4);
	RightDive_keys[17].arraySetSize(4);

	RightDive_times[17][0] = 0.100000;
	RightDive_keys[17][0] = AL::ALValue::array(0.811444, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[17][1] = 0.500000;
	RightDive_keys[17][1] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[17][2] = 0.900000;
	RightDive_keys[17][2] = AL::ALValue::array(0.315962, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[17][3] = 1.40000;
	RightDive_keys[17][3] = AL::ALValue::array(0.331302, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LAnklePitch");
	RightDive_times[18].arraySetSize(4);
	RightDive_keys[18].arraySetSize(4);

	RightDive_times[18][0] = 0.100000;
	RightDive_keys[18][0] = AL::ALValue::array(-0.403483, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[18][1] = 0.500000;
	RightDive_keys[18][1] = AL::ALValue::array(-1.18952, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[18][2] = 0.900000;
	RightDive_keys[18][2] = AL::ALValue::array(-0.242414, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[18][3] = 1.40000;
	RightDive_keys[18][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("LAnkleRoll");
	RightDive_times[19].arraySetSize(4);
	RightDive_keys[19].arraySetSize(4);

	RightDive_times[19][0] = 0.100000;
	RightDive_keys[19][0] = AL::ALValue::array(0.0184499, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[19][1] = 0.500000;
	RightDive_keys[19][1] = AL::ALValue::array(0.101286, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[19][2] = 0.900000;
	RightDive_keys[19][2] = AL::ALValue::array(-0.665714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[19][3] = 1.40000;
	RightDive_keys[19][3] = AL::ALValue::array(0.184122, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RHipRoll");
	RightDive_times[20].arraySetSize(4);
	RightDive_keys[20].arraySetSize(4);

	RightDive_times[20][0] = 0.100000;
	RightDive_keys[20][0] = AL::ALValue::array(0.0583340, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[20][1] = 0.500000;
	RightDive_keys[20][1] = AL::ALValue::array(0.0383920, AL::ALValue::array(2, -0.133333, 0.0199420), AL::ALValue::array(2, 0.133333, -0.0199420));
	RightDive_times[20][2] = 0.900000;
	RightDive_keys[20][2] = AL::ALValue::array(-0.269941, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[20][3] = 1.40000;
	RightDive_keys[20][3] = AL::ALValue::array(0.322183, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RHipPitch");
	RightDive_times[21].arraySetSize(4);
	RightDive_keys[21].arraySetSize(4);

	RightDive_times[21][0] = 0.100000;
	RightDive_keys[21][0] = AL::ALValue::array(-0.460242, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[21][1] = 0.500000;
	RightDive_keys[21][1] = AL::ALValue::array(-0.885159, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[21][2] = 0.900000;
	RightDive_keys[21][2] = AL::ALValue::array(-0.803859, AL::ALValue::array(2, -0.133333, -0.0813009), AL::ALValue::array(2, 0.166667, 0.101626));
	RightDive_times[21][3] = 1.40000;
	RightDive_keys[21][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RKneePitch");
	RightDive_times[22].arraySetSize(4);
	RightDive_keys[22].arraySetSize(4);

	RightDive_times[22][0] = 0.100000;
	RightDive_keys[22][0] = AL::ALValue::array(0.803859, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[22][1] = 0.500000;
	RightDive_keys[22][1] = AL::ALValue::array(2.12020, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[22][2] = 0.900000;
	RightDive_keys[22][2] = AL::ALValue::array(2.12020, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[22][3] = 1.40000;
	RightDive_keys[22][3] = AL::ALValue::array(0.642787, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RAnklePitch");
	RightDive_times[23].arraySetSize(4);
	RightDive_keys[23].arraySetSize(4);

	RightDive_times[23][0] = 0.100000;
	RightDive_keys[23][0] = AL::ALValue::array(-0.409536, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[23][1] = 0.500000;
	RightDive_keys[23][1] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[23][2] = 0.900000;
	RightDive_keys[23][2] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[23][3] = 1.40000;
	RightDive_keys[23][3] = AL::ALValue::array(0.177985, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	RightDive_names.push_back("RAnkleRoll");
	RightDive_times[24].arraySetSize(4);
	RightDive_keys[24].arraySetSize(4);

	RightDive_times[24][0] = 0.100000;
	RightDive_keys[24][0] = AL::ALValue::array(-0.0521140, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[24][1] = 0.500000;
	RightDive_keys[24][1] = AL::ALValue::array(-0.0275701, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[24][2] = 0.900000;
	RightDive_keys[24][2] = AL::ALValue::array(-0.108872, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	RightDive_times[24][3] = 1.40000;
	RightDive_keys[24][3] = AL::ALValue::array(-0.0858620, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	
	
	
	
	/*
	RightDive_names.reserve(25);
	RightDive_times.arraySetSize(25);
	RightDive_keys.arraySetSize(25);
	
	RightDive_names.push_back("HeadYaw");
	RightDive_times[0].arraySetSize(3);
	RightDive_keys[0].arraySetSize(3);
	
	RightDive_times[0][0] = 1.00000;
	RightDive_keys[0][0] = AL::ALValue::array(0.286815, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[0][1] = 2.10000;
	RightDive_keys[0][1] = AL::ALValue::array(0.0843280, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[0][2] = 2.50000;
	RightDive_keys[0][2] = AL::ALValue::array(0.0873960, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("HeadPitch");
	RightDive_times[1].arraySetSize(3);
	RightDive_keys[1].arraySetSize(3);
	
	RightDive_times[1][0] = 1.00000;
	RightDive_keys[1][0] = AL::ALValue::array(0.165630, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[1][1] = 2.10000;
	RightDive_keys[1][1] = AL::ALValue::array(-0.164180, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[1][2] = 2.50000;
	RightDive_keys[1][2] = AL::ALValue::array(-0.165714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LShoulderPitch");
	RightDive_times[2].arraySetSize(3);
	RightDive_keys[2].arraySetSize(3);
	
	RightDive_times[2][0] = 1.00000;
	RightDive_keys[2][0] = AL::ALValue::array(1.73798, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[2][1] = 2.10000;
	RightDive_keys[2][1] = AL::ALValue::array(1.30846, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[2][2] = 2.50000;
	RightDive_keys[2][2] = AL::ALValue::array(-1.68284, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LShoulderRoll");
	RightDive_times[3].arraySetSize(3);
	RightDive_keys[3].arraySetSize(3);
	
	RightDive_times[3][0] = 1.00000;
	RightDive_keys[3][0] = AL::ALValue::array(0.265341, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[3][1] = 2.10000;
	RightDive_keys[3][1] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[3][2] = 2.50000;
	RightDive_keys[3][2] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LElbowYaw");
	RightDive_times[4].arraySetSize(3);
	RightDive_keys[4].arraySetSize(3);
	
	RightDive_times[4][0] = 1.00000;
	RightDive_keys[4][0] = AL::ALValue::array(-1.47882, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[4][1] = 2.10000;
	RightDive_keys[4][1] = AL::ALValue::array(-0.559952, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[4][2] = 2.50000;
	RightDive_keys[4][2] = AL::ALValue::array(-0.806927, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LElbowRoll");
	RightDive_times[5].arraySetSize(3);
	RightDive_keys[5].arraySetSize(3);
	
	RightDive_times[5][0] = 1.00000;
	RightDive_keys[5][0] = AL::ALValue::array(-0.515382, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[5][1] = 2.10000;
	RightDive_keys[5][1] = AL::ALValue::array(-0.977116, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[5][2] = 2.50000;
	RightDive_keys[5][2] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LWristYaw");
	RightDive_times[6].arraySetSize(3);
	RightDive_keys[6].arraySetSize(3);
	
	RightDive_times[6][0] = 1.00000;
	RightDive_keys[6][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[6][1] = 2.10000;
	RightDive_keys[6][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[6][2] = 2.50000;
	RightDive_keys[6][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LHand");
	RightDive_times[7].arraySetSize(3);
	RightDive_keys[7].arraySetSize(3);
	
	RightDive_times[7][0] = 1.00000;
	RightDive_keys[7][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[7][1] = 2.10000;
	RightDive_keys[7][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[7][2] = 2.50000;
	RightDive_keys[7][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RShoulderPitch");
	RightDive_times[8].arraySetSize(3);
	RightDive_keys[8].arraySetSize(3);
	
	RightDive_times[8][0] = 1.00000;
	RightDive_keys[8][0] = AL::ALValue::array(1.74573, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[8][1] = 2.10000;
	RightDive_keys[8][1] = AL::ALValue::array(1.19810, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[8][2] = 2.50000;
	RightDive_keys[8][2] = AL::ALValue::array(-1.65975, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RShoulderRoll");
	RightDive_times[9].arraySetSize(3);
	RightDive_keys[9].arraySetSize(3);
	
	RightDive_times[9][0] = 1.00000;
	RightDive_keys[9][0] = AL::ALValue::array(-0.243948, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[9][1] = 2.10000;
	RightDive_keys[9][1] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[9][2] = 2.50000;
	RightDive_keys[9][2] = AL::ALValue::array(-0.0169160, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RElbowYaw");
	RightDive_times[10].arraySetSize(3);
	RightDive_keys[10].arraySetSize(3);
	
	RightDive_times[10][0] = 1.00000;
	RightDive_keys[10][0] = AL::ALValue::array(1.48180, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[10][1] = 2.10000;
	RightDive_keys[10][1] = AL::ALValue::array(0.806841, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[10][2] = 2.50000;
	RightDive_keys[10][2] = AL::ALValue::array(0.375789, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RElbowRoll");
	RightDive_times[11].arraySetSize(3);
	RightDive_keys[11].arraySetSize(3);
	
	RightDive_times[11][0] = 1.00000;
	RightDive_keys[11][0] = AL::ALValue::array(0.524670, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[11][1] = 2.10000;
	RightDive_keys[11][1] = AL::ALValue::array(0.955723, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[11][2] = 2.50000;
	RightDive_keys[11][2] = AL::ALValue::array(0.0890139, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RWristYaw");
	RightDive_times[12].arraySetSize(3);
	RightDive_keys[12].arraySetSize(3);
	
	RightDive_times[12][0] = 1.00000;
	RightDive_keys[12][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[12][1] = 2.10000;
	RightDive_keys[12][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[12][2] = 2.50000;
	RightDive_keys[12][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RHand");
	RightDive_times[13].arraySetSize(3);
	RightDive_keys[13].arraySetSize(3);
	
	RightDive_times[13][0] = 1.00000;
	RightDive_keys[13][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[13][1] = 2.10000;
	RightDive_keys[13][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[13][2] = 2.50000;
	RightDive_keys[13][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LHipYawPitch");
	RightDive_times[14].arraySetSize(4);
	RightDive_keys[14].arraySetSize(4);
	
	RightDive_times[14][0] = 1.00000;
	RightDive_keys[14][0] = AL::ALValue::array(0.00464395, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[14][1] = 2.10000;
	RightDive_keys[14][1] = AL::ALValue::array(-0.0659200, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[14][2] = 2.50000;
	RightDive_keys[14][2] = AL::ALValue::array(-0.134950, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[14][3] = 3.60000;
	RightDive_keys[14][3] = AL::ALValue::array(-0.0337060, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LHipRoll");
	RightDive_times[15].arraySetSize(4);
	RightDive_keys[15].arraySetSize(4);
	
	RightDive_times[15][0] = 1.00000;
	RightDive_keys[15][0] = AL::ALValue::array(-0.0152980, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[15][1] = 2.10000;
	RightDive_keys[15][1] = AL::ALValue::array(-0.138018, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[15][2] = 2.50000;
	RightDive_keys[15][2] = AL::ALValue::array(0.112024, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[15][3] = 3.60000;
	RightDive_keys[15][3] = AL::ALValue::array(0.182588, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LHipPitch");
	RightDive_times[16].arraySetSize(4);
	RightDive_keys[16].arraySetSize(4);
	
	RightDive_times[16][0] = 1.00000;
	RightDive_keys[16][0] = AL::ALValue::array(-0.460158, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[16][1] = 2.10000;
	RightDive_keys[16][1] = AL::ALValue::array(-0.866668, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[16][2] = 2.50000;
	RightDive_keys[16][2] = AL::ALValue::array(0.0598679, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[16][3] = 3.60000;
	RightDive_keys[16][3] = AL::ALValue::array(0.0568000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LKneePitch");
	RightDive_times[17].arraySetSize(4);
	RightDive_keys[17].arraySetSize(4);
	
	RightDive_times[17][0] = 1.00000;
	RightDive_keys[17][0] = AL::ALValue::array(0.811444, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[17][1] = 2.10000;
	RightDive_keys[17][1] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[17][2] = 2.50000;
	RightDive_keys[17][2] = AL::ALValue::array(0.315962, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[17][3] = 3.60000;
	RightDive_keys[17][3] = AL::ALValue::array(0.331302, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LAnklePitch");
	RightDive_times[18].arraySetSize(4);
	RightDive_keys[18].arraySetSize(4);
	
	RightDive_times[18][0] = 1.00000;
	RightDive_keys[18][0] = AL::ALValue::array(-0.403483, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[18][1] = 2.10000;
	RightDive_keys[18][1] = AL::ALValue::array(-1.18952, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[18][2] = 2.50000;
	RightDive_keys[18][2] = AL::ALValue::array(-0.242414, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[18][3] = 3.60000;
	RightDive_keys[18][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("LAnkleRoll");
	RightDive_times[19].arraySetSize(4);
	RightDive_keys[19].arraySetSize(4);
	
	RightDive_times[19][0] = 1.00000;
	RightDive_keys[19][0] = AL::ALValue::array(0.0184499, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[19][1] = 2.10000;
	RightDive_keys[19][1] = AL::ALValue::array(0.101286, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[19][2] = 2.50000;
	RightDive_keys[19][2] = AL::ALValue::array(-0.665714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[19][3] = 3.60000;
	RightDive_keys[19][3] = AL::ALValue::array(0.184122, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RHipRoll");
	RightDive_times[20].arraySetSize(4);
	RightDive_keys[20].arraySetSize(4);
	
	RightDive_times[20][0] = 1.00000;
	RightDive_keys[20][0] = AL::ALValue::array(0.0583340, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[20][1] = 2.10000;
	RightDive_keys[20][1] = AL::ALValue::array(0.0383920, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[20][2] = 2.50000;
	RightDive_keys[20][2] = AL::ALValue::array(-0.269941, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[20][3] = 3.60000;
	RightDive_keys[20][3] = AL::ALValue::array(0.322183, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RHipPitch");
	RightDive_times[21].arraySetSize(4);
	RightDive_keys[21].arraySetSize(4);
	
	RightDive_times[21][0] = 1.00000;
	RightDive_keys[21][0] = AL::ALValue::array(-0.460242, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[21][1] = 2.10000;
	RightDive_keys[21][1] = AL::ALValue::array(-0.885159, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[21][2] = 2.50000;
	RightDive_keys[21][2] = AL::ALValue::array(-0.803859, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[21][3] = 3.60000;
	RightDive_keys[21][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RKneePitch");
	RightDive_times[22].arraySetSize(4);
	RightDive_keys[22].arraySetSize(4);
	
	RightDive_times[22][0] = 1.00000;
	RightDive_keys[22][0] = AL::ALValue::array(0.803859, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[22][1] = 2.10000;
	RightDive_keys[22][1] = AL::ALValue::array(2.12020, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[22][2] = 2.50000;
	RightDive_keys[22][2] = AL::ALValue::array(2.12020, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[22][3] = 3.60000;
	RightDive_keys[22][3] = AL::ALValue::array(0.642787, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RAnklePitch");
	RightDive_times[23].arraySetSize(4);
	RightDive_keys[23].arraySetSize(4);
	
	RightDive_times[23][0] = 1.00000;
	RightDive_keys[23][0] = AL::ALValue::array(-0.409536, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[23][1] = 2.10000;
	RightDive_keys[23][1] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[23][2] = 2.50000;
	RightDive_keys[23][2] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[23][3] = 3.60000;
	RightDive_keys[23][3] = AL::ALValue::array(0.177985, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	RightDive_names.push_back("RAnkleRoll");
	RightDive_times[24].arraySetSize(4);
	RightDive_keys[24].arraySetSize(4);
	
	RightDive_times[24][0] = 1.00000;
	RightDive_keys[24][0] = AL::ALValue::array(-0.0521140, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[24][1] = 2.10000;
	RightDive_keys[24][1] = AL::ALValue::array(-0.0275701, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	RightDive_times[24][2] = 2.50000;
	RightDive_keys[24][2] = AL::ALValue::array(-0.108872, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	RightDive_times[24][3] = 3.60000;
	RightDive_keys[24][3] = AL::ALValue::array(-0.0858620, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	*/


	/******************************** Left Dive ****************************/
	
	
	LeftDive_names.reserve(25);
	LeftDive_times.arraySetSize(25);
	LeftDive_keys.arraySetSize(25);

	LeftDive_names.push_back("HeadYaw");
	LeftDive_times[0].arraySetSize(3);
	LeftDive_keys[0].arraySetSize(3);

	LeftDive_times[0][0] = 0.100000;
	LeftDive_keys[0][0] = AL::ALValue::array(-0.286815, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[0][1] = 0.500000;
	LeftDive_keys[0][1] = AL::ALValue::array(-0.0843280, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[0][2] = 0.900000;
	LeftDive_keys[0][2] = AL::ALValue::array(-0.0873960, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("HeadPitch");
	LeftDive_times[1].arraySetSize(3);
	LeftDive_keys[1].arraySetSize(3);

	LeftDive_times[1][0] = 0.100000;
	LeftDive_keys[1][0] = AL::ALValue::array(0.165630, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[1][1] = 0.500000;
	LeftDive_keys[1][1] = AL::ALValue::array(-0.164180, AL::ALValue::array(2, -0.133333, 0.00153396), AL::ALValue::array(2, 0.133333, -0.00153396));
	LeftDive_times[1][2] = 0.900000;
	LeftDive_keys[1][2] = AL::ALValue::array(-0.165714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LShoulderPitch");
	LeftDive_times[2].arraySetSize(3);
	LeftDive_keys[2].arraySetSize(3);

	LeftDive_times[2][0] = 0.100000;
	LeftDive_keys[2][0] = AL::ALValue::array(1.74573, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[2][1] = 0.500000;
	LeftDive_keys[2][1] = AL::ALValue::array(1.19810, AL::ALValue::array(2, -0.133333, 0.547636), AL::ALValue::array(2, 0.133333, -0.547636));
	LeftDive_times[2][2] = 0.900000;
	LeftDive_keys[2][2] = AL::ALValue::array(-1.65975, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LShoulderRoll");
	LeftDive_times[3].arraySetSize(3);
	LeftDive_keys[3].arraySetSize(3);

	LeftDive_times[3][0] = 0.100000;
	LeftDive_keys[3][0] = AL::ALValue::array(0.243948, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[3][1] = 0.500000;
	LeftDive_keys[3][1] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[3][2] = 0.900000;
	LeftDive_keys[3][2] = AL::ALValue::array(0.0169160, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LElbowYaw");
	LeftDive_times[4].arraySetSize(3);
	LeftDive_keys[4].arraySetSize(3);

	LeftDive_times[4][0] = 0.100000;
	LeftDive_keys[4][0] = AL::ALValue::array(-1.48180, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[4][1] = 0.500000;
	LeftDive_keys[4][1] = AL::ALValue::array(-0.806841, AL::ALValue::array(2, -0.133333, -0.184336), AL::ALValue::array(2, 0.133333, 0.184336));
	LeftDive_times[4][2] = 0.900000;
	LeftDive_keys[4][2] = AL::ALValue::array(-0.375789, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LElbowRoll");
	LeftDive_times[5].arraySetSize(3);
	LeftDive_keys[5].arraySetSize(3);

	LeftDive_times[5][0] = 0.100000;
	LeftDive_keys[5][0] = AL::ALValue::array(-0.524670, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[5][1] = 0.500000;
	LeftDive_keys[5][1] = AL::ALValue::array(-0.955723, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[5][2] = 0.900000;
	LeftDive_keys[5][2] = AL::ALValue::array(-0.0890139, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LWristYaw");
	LeftDive_times[6].arraySetSize(3);
	LeftDive_keys[6].arraySetSize(3);

	LeftDive_times[6][0] = 0.100000;
	LeftDive_keys[6][0] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[6][1] = 0.500000;
	LeftDive_keys[6][1] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[6][2] = 0.900000;
	LeftDive_keys[6][2] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LHand");
	LeftDive_times[7].arraySetSize(3);
	LeftDive_keys[7].arraySetSize(3);

	LeftDive_times[7][0] = 0.100000;
	LeftDive_keys[7][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[7][1] = 0.500000;
	LeftDive_keys[7][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[7][2] = 0.900000;
	LeftDive_keys[7][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RShoulderPitch");
	LeftDive_times[8].arraySetSize(3);
	LeftDive_keys[8].arraySetSize(3);

	LeftDive_times[8][0] = 0.100000;
	LeftDive_keys[8][0] = AL::ALValue::array(1.73798, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[8][1] = 0.500000;
	LeftDive_keys[8][1] = AL::ALValue::array(1.30846, AL::ALValue::array(2, -0.133333, 0.429520), AL::ALValue::array(2, 0.133333, -0.429520));
	LeftDive_times[8][2] = 0.900000;
	LeftDive_keys[8][2] = AL::ALValue::array(-1.68284, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RShoulderRoll");
	LeftDive_times[9].arraySetSize(3);
	LeftDive_keys[9].arraySetSize(3);

	LeftDive_times[9][0] = 0.100000;
	LeftDive_keys[9][0] = AL::ALValue::array(-0.265341, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[9][1] = 0.500000;
	LeftDive_keys[9][1] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[9][2] = 0.900000;
	LeftDive_keys[9][2] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RElbowYaw");
	LeftDive_times[10].arraySetSize(3);
	LeftDive_keys[10].arraySetSize(3);

	LeftDive_times[10][0] = 0.100000;
	LeftDive_keys[10][0] = AL::ALValue::array(1.47882, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[10][1] = 0.500000;
	LeftDive_keys[10][1] = AL::ALValue::array(0.559952, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[10][2] = 0.900000;
	LeftDive_keys[10][2] = AL::ALValue::array(0.806927, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RElbowRoll");
	LeftDive_times[11].arraySetSize(3);
	LeftDive_keys[11].arraySetSize(3);

	LeftDive_times[11][0] = 0.100000;
	LeftDive_keys[11][0] = AL::ALValue::array(0.515382, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[11][1] = 0.500000;
	LeftDive_keys[11][1] = AL::ALValue::array(0.977116, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[11][2] = 0.900000;
	LeftDive_keys[11][2] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RWristYaw");
	LeftDive_times[12].arraySetSize(3);
	LeftDive_keys[12].arraySetSize(3);

	LeftDive_times[12][0] = 0.100000;
	LeftDive_keys[12][0] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[12][1] = 0.500000;
	LeftDive_keys[12][1] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[12][2] = 0.900000;
	LeftDive_keys[12][2] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RHand");
	LeftDive_times[13].arraySetSize(3);
	LeftDive_keys[13].arraySetSize(3);

	LeftDive_times[13][0] = 0.100000;
	LeftDive_keys[13][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[13][1] = 0.500000;
	LeftDive_keys[13][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[13][2] = 0.900000;
	LeftDive_keys[13][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LHipYawPitch");
	LeftDive_times[14].arraySetSize(4);
	LeftDive_keys[14].arraySetSize(4);

	LeftDive_times[14][0] = 0.100000;
	LeftDive_keys[14][0] = AL::ALValue::array(0.00464395, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[14][1] = 0.500000;
	LeftDive_keys[14][1] = AL::ALValue::array(-0.0659200, AL::ALValue::array(2, -0.133333, 0.0232657), AL::ALValue::array(2, 0.133333, -0.0232657));
	LeftDive_times[14][2] = 0.900000;
	LeftDive_keys[14][2] = AL::ALValue::array(-0.134950, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[14][3] = 1.40000;
	LeftDive_keys[14][3] = AL::ALValue::array(-0.0337060, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LHipRoll");
	LeftDive_times[15].arraySetSize(4);
	LeftDive_keys[15].arraySetSize(4);

	LeftDive_times[15][0] = 0.100000;
	LeftDive_keys[15][0] = AL::ALValue::array(-0.0583340, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[15][1] = 0.500000;
	LeftDive_keys[15][1] = AL::ALValue::array(-0.0383920, AL::ALValue::array(2, -0.133333, -0.0199420), AL::ALValue::array(2, 0.133333, 0.0199420));
	LeftDive_times[15][2] = 0.900000;
	LeftDive_keys[15][2] = AL::ALValue::array(0.269941, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[15][3] = 1.40000;
	LeftDive_keys[15][3] = AL::ALValue::array(-0.322183, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LHipPitch");
	LeftDive_times[16].arraySetSize(4);
	LeftDive_keys[16].arraySetSize(4);

	LeftDive_times[16][0] = 0.100000;
	LeftDive_keys[16][0] = AL::ALValue::array(-0.460242, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[16][1] = 0.500000;
	LeftDive_keys[16][1] = AL::ALValue::array(-0.885159, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[16][2] = 0.900000;
	LeftDive_keys[16][2] = AL::ALValue::array(-0.803859, AL::ALValue::array(2, -0.133333, -0.0813009), AL::ALValue::array(2, 0.166667, 0.101626));
	LeftDive_times[16][3] = 1.40000;
	LeftDive_keys[16][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LKneePitch");
	LeftDive_times[17].arraySetSize(4);
	LeftDive_keys[17].arraySetSize(4);

	LeftDive_times[17][0] = 0.100000;
	LeftDive_keys[17][0] = AL::ALValue::array(0.803859, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[17][1] = 0.500000;
	LeftDive_keys[17][1] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[17][2] = 0.900000;
	LeftDive_keys[17][2] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[17][3] = 1.40000;
	LeftDive_keys[17][3] = AL::ALValue::array(0.642787, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LAnklePitch");
	LeftDive_times[18].arraySetSize(4);
	LeftDive_keys[18].arraySetSize(4);

	LeftDive_times[18][0] = 0.100000;
	LeftDive_keys[18][0] = AL::ALValue::array(-0.409536, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[18][1] = 0.500000;
	LeftDive_keys[18][1] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[18][2] = 0.900000;
	LeftDive_keys[18][2] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[18][3] = 1.40000;
	LeftDive_keys[18][3] = AL::ALValue::array(0.177985, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("LAnkleRoll");
	LeftDive_times[19].arraySetSize(4);
	LeftDive_keys[19].arraySetSize(4);

	LeftDive_times[19][0] = 0.100000;
	LeftDive_keys[19][0] = AL::ALValue::array(0.0521140, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[19][1] = 0.500000;
	LeftDive_keys[19][1] = AL::ALValue::array(0.0275701, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[19][2] = 0.900000;
	LeftDive_keys[19][2] = AL::ALValue::array(0.108872, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[19][3] = 1.40000;
	LeftDive_keys[19][3] = AL::ALValue::array(0.0858620, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RHipRoll");
	LeftDive_times[20].arraySetSize(4);
	LeftDive_keys[20].arraySetSize(4);

	LeftDive_times[20][0] = 0.100000;
	LeftDive_keys[20][0] = AL::ALValue::array(0.0152980, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[20][1] = 0.500000;
	LeftDive_keys[20][1] = AL::ALValue::array(0.138018, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[20][2] = 0.900000;
	LeftDive_keys[20][2] = AL::ALValue::array(-0.112024, AL::ALValue::array(2, -0.133333, 0.0474971), AL::ALValue::array(2, 0.166667, -0.0593714));
	LeftDive_times[20][3] = 1.40000;
	LeftDive_keys[20][3] = AL::ALValue::array(-0.182588, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RHipPitch");
	LeftDive_times[21].arraySetSize(4);
	LeftDive_keys[21].arraySetSize(4);

	LeftDive_times[21][0] = 0.100000;
	LeftDive_keys[21][0] = AL::ALValue::array(-0.460158, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[21][1] = 0.500000;
	LeftDive_keys[21][1] = AL::ALValue::array(-0.866668, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[21][2] = 0.900000;
	LeftDive_keys[21][2] = AL::ALValue::array(0.0598679, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[21][3] = 1.40000;
	LeftDive_keys[21][3] = AL::ALValue::array(0.0568000, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RKneePitch");
	LeftDive_times[22].arraySetSize(4);
	LeftDive_keys[22].arraySetSize(4);

	LeftDive_times[22][0] = 0.100000;
	LeftDive_keys[22][0] = AL::ALValue::array(0.811444, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[22][1] = 0.500000;
	LeftDive_keys[22][1] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[22][2] = 0.900000;
	LeftDive_keys[22][2] = AL::ALValue::array(0.315962, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[22][3] = 1.40000;
	LeftDive_keys[22][3] = AL::ALValue::array(0.331302, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RAnklePitch");
	LeftDive_times[23].arraySetSize(4);
	LeftDive_keys[23].arraySetSize(4);

	LeftDive_times[23][0] = 0.100000;
	LeftDive_keys[23][0] = AL::ALValue::array(-0.403483, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[23][1] = 0.500000;
	LeftDive_keys[23][1] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[23][2] = 0.900000;
	LeftDive_keys[23][2] = AL::ALValue::array(-0.242414, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[23][3] = 1.40000;
	LeftDive_keys[23][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	LeftDive_names.push_back("RAnkleRoll");
	LeftDive_times[24].arraySetSize(4);
	LeftDive_keys[24].arraySetSize(4);

	LeftDive_times[24][0] = 0.100000;
	LeftDive_keys[24][0] = AL::ALValue::array(-0.0184499, AL::ALValue::array(2, -0.0333333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[24][1] = 0.500000;
	LeftDive_keys[24][1] = AL::ALValue::array(-0.101286, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[24][2] = 0.900000;
	LeftDive_keys[24][2] = AL::ALValue::array(0.665714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.166667, 0.00000));
	LeftDive_times[24][3] = 1.40000;
	LeftDive_keys[24][3] = AL::ALValue::array(-0.184122, AL::ALValue::array(2, -0.166667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));



	
	/*
	LeftDive_names.reserve(25);
	LeftDive_times.arraySetSize(25);
	LeftDive_keys.arraySetSize(25);
	
	LeftDive_names.push_back("HeadYaw");
	LeftDive_times[0].arraySetSize(3);
	LeftDive_keys[0].arraySetSize(3);
	
	LeftDive_times[0][0] = 1.00000;
	LeftDive_keys[0][0] = AL::ALValue::array(-0.286815, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[0][1] = 2.10000;
	LeftDive_keys[0][1] = AL::ALValue::array(-0.0843280, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[0][2] = 2.50000;
	LeftDive_keys[0][2] = AL::ALValue::array(-0.0873960, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("HeadPitch");
	LeftDive_times[1].arraySetSize(3);
	LeftDive_keys[1].arraySetSize(3);
	
	LeftDive_times[1][0] = 1.00000;
	LeftDive_keys[1][0] = AL::ALValue::array(0.165630, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[1][1] = 2.10000;
	LeftDive_keys[1][1] = AL::ALValue::array(-0.164180, AL::ALValue::array(2, -0.366667, 0.00421840), AL::ALValue::array(2, 0.133333, -0.00153396));
	LeftDive_times[1][2] = 2.50000;
	LeftDive_keys[1][2] = AL::ALValue::array(-0.165714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LShoulderPitch");
	LeftDive_times[2].arraySetSize(3);
	LeftDive_keys[2].arraySetSize(3);
	
	LeftDive_times[2][0] = 1.00000;
	LeftDive_keys[2][0] = AL::ALValue::array(1.74573, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[2][1] = 2.10000;
	LeftDive_keys[2][1] = AL::ALValue::array(1.19810, AL::ALValue::array(2, -0.366667, 0.547635), AL::ALValue::array(2, 0.133333, -0.199140));
	LeftDive_times[2][2] = 2.50000;
	LeftDive_keys[2][2] = AL::ALValue::array(-1.65975, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LShoulderRoll");
	LeftDive_times[3].arraySetSize(3);
	LeftDive_keys[3].arraySetSize(3);
	
	LeftDive_times[3][0] = 1.00000;
	LeftDive_keys[3][0] = AL::ALValue::array(0.243948, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[3][1] = 2.10000;
	LeftDive_keys[3][1] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[3][2] = 2.50000;
	LeftDive_keys[3][2] = AL::ALValue::array(0.0169160, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LElbowYaw");
	LeftDive_times[4].arraySetSize(3);
	LeftDive_keys[4].arraySetSize(3);
	
	LeftDive_times[4][0] = 1.00000;
	LeftDive_keys[4][0] = AL::ALValue::array(-1.48180, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[4][1] = 2.10000;
	LeftDive_keys[4][1] = AL::ALValue::array(-0.806841, AL::ALValue::array(2, -0.366667, -0.270359), AL::ALValue::array(2, 0.133333, 0.0983123));
	LeftDive_times[4][2] = 2.50000;
	LeftDive_keys[4][2] = AL::ALValue::array(-0.375789, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LElbowRoll");
	LeftDive_times[5].arraySetSize(3);
	LeftDive_keys[5].arraySetSize(3);
	
	LeftDive_times[5][0] = 1.00000;
	LeftDive_keys[5][0] = AL::ALValue::array(-0.524670, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[5][1] = 2.10000;
	LeftDive_keys[5][1] = AL::ALValue::array(-0.955723, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[5][2] = 2.50000;
	LeftDive_keys[5][2] = AL::ALValue::array(-0.0890139, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LWristYaw");
	LeftDive_times[6].arraySetSize(3);
	LeftDive_keys[6].arraySetSize(3);
	
	LeftDive_times[6][0] = 1.00000;
	LeftDive_keys[6][0] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[6][1] = 2.10000;
	LeftDive_keys[6][1] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[6][2] = 2.50000;
	LeftDive_keys[6][2] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LHand");
	LeftDive_times[7].arraySetSize(3);
	LeftDive_keys[7].arraySetSize(3);
	
	LeftDive_times[7][0] = 1.00000;
	LeftDive_keys[7][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[7][1] = 2.10000;
	LeftDive_keys[7][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[7][2] = 2.50000;
	LeftDive_keys[7][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RShoulderPitch");
	LeftDive_times[8].arraySetSize(3);
	LeftDive_keys[8].arraySetSize(3);
	
	LeftDive_times[8][0] = 1.00000;
	LeftDive_keys[8][0] = AL::ALValue::array(1.73798, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[8][1] = 2.10000;
	LeftDive_keys[8][1] = AL::ALValue::array(1.30846, AL::ALValue::array(2, -0.366667, 0.429520), AL::ALValue::array(2, 0.133333, -0.156189));
	LeftDive_times[8][2] = 2.50000;
	LeftDive_keys[8][2] = AL::ALValue::array(-1.68284, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RShoulderRoll");
	LeftDive_times[9].arraySetSize(3);
	LeftDive_keys[9].arraySetSize(3);
	
	LeftDive_times[9][0] = 1.00000;
	LeftDive_keys[9][0] = AL::ALValue::array(-0.265341, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[9][1] = 2.10000;
	LeftDive_keys[9][1] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[9][2] = 2.50000;
	LeftDive_keys[9][2] = AL::ALValue::array(-0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RElbowYaw");
	LeftDive_times[10].arraySetSize(3);
	LeftDive_keys[10].arraySetSize(3);
	
	LeftDive_times[10][0] = 1.00000;
	LeftDive_keys[10][0] = AL::ALValue::array(1.47882, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[10][1] = 2.10000;
	LeftDive_keys[10][1] = AL::ALValue::array(0.559952, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[10][2] = 2.50000;
	LeftDive_keys[10][2] = AL::ALValue::array(0.806927, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RElbowRoll");
	LeftDive_times[11].arraySetSize(3);
	LeftDive_keys[11].arraySetSize(3);
	
	LeftDive_times[11][0] = 1.00000;
	LeftDive_keys[11][0] = AL::ALValue::array(0.515382, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[11][1] = 2.10000;
	LeftDive_keys[11][1] = AL::ALValue::array(0.977116, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[11][2] = 2.50000;
	LeftDive_keys[11][2] = AL::ALValue::array(0.00870000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RWristYaw");
	LeftDive_times[12].arraySetSize(3);
	LeftDive_keys[12].arraySetSize(3);
	
	LeftDive_times[12][0] = 1.00000;
	LeftDive_keys[12][0] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[12][1] = 2.10000;
	LeftDive_keys[12][1] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[12][2] = 2.50000;
	LeftDive_keys[12][2] = AL::ALValue::array(-0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RHand");
	LeftDive_times[13].arraySetSize(3);
	LeftDive_keys[13].arraySetSize(3);
	
	LeftDive_times[13][0] = 1.00000;
	LeftDive_keys[13][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[13][1] = 2.10000;
	LeftDive_keys[13][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[13][2] = 2.50000;
	LeftDive_keys[13][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LHipYawPitch");
	LeftDive_times[14].arraySetSize(4);
	LeftDive_keys[14].arraySetSize(4);
	
	LeftDive_times[14][0] = 1.00000;
	LeftDive_keys[14][0] = AL::ALValue::array(0.00464395, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[14][1] = 2.10000;
	LeftDive_keys[14][1] = AL::ALValue::array(-0.0659200, AL::ALValue::array(2, -0.366667, 0.0341230), AL::ALValue::array(2, 0.133333, -0.0124084));
	LeftDive_times[14][2] = 2.50000;
	LeftDive_keys[14][2] = AL::ALValue::array(-0.134950, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[14][3] = 3.60000;
	LeftDive_keys[14][3] = AL::ALValue::array(-0.0337060, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LHipRoll");
	LeftDive_times[15].arraySetSize(4);
	LeftDive_keys[15].arraySetSize(4);
	
	LeftDive_times[15][0] = 1.00000;
	LeftDive_keys[15][0] = AL::ALValue::array(-0.0583340, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[15][1] = 2.10000;
	LeftDive_keys[15][1] = AL::ALValue::array(-0.0383920, AL::ALValue::array(2, -0.366667, -0.0199420), AL::ALValue::array(2, 0.133333, 0.00725162));
	LeftDive_times[15][2] = 2.50000;
	LeftDive_keys[15][2] = AL::ALValue::array(0.269941, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[15][3] = 3.60000;
	LeftDive_keys[15][3] = AL::ALValue::array(-0.322183, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LHipPitch");
	LeftDive_times[16].arraySetSize(4);
	LeftDive_keys[16].arraySetSize(4);
	
	LeftDive_times[16][0] = 1.00000;
	LeftDive_keys[16][0] = AL::ALValue::array(-0.460242, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[16][1] = 2.10000;
	LeftDive_keys[16][1] = AL::ALValue::array(-0.885159, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[16][2] = 2.50000;
	LeftDive_keys[16][2] = AL::ALValue::array(-0.803859, AL::ALValue::array(2, -0.133333, -0.0537240), AL::ALValue::array(2, 0.366667, 0.147741));
	LeftDive_times[16][3] = 3.60000;
	LeftDive_keys[16][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LKneePitch");
	LeftDive_times[17].arraySetSize(4);
	LeftDive_keys[17].arraySetSize(4);
	
	LeftDive_times[17][0] = 1.00000;
	LeftDive_keys[17][0] = AL::ALValue::array(0.803859, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[17][1] = 2.10000;
	LeftDive_keys[17][1] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[17][2] = 2.50000;
	LeftDive_keys[17][2] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[17][3] = 3.60000;
	LeftDive_keys[17][3] = AL::ALValue::array(0.642787, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LAnklePitch");
	LeftDive_times[18].arraySetSize(4);
	LeftDive_keys[18].arraySetSize(4);
	
	LeftDive_times[18][0] = 1.00000;
	LeftDive_keys[18][0] = AL::ALValue::array(-0.409536, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[18][1] = 2.10000;
	LeftDive_keys[18][1] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[18][2] = 2.50000;
	LeftDive_keys[18][2] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[18][3] = 3.60000;
	LeftDive_keys[18][3] = AL::ALValue::array(0.177985, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("LAnkleRoll");
	LeftDive_times[19].arraySetSize(4);
	LeftDive_keys[19].arraySetSize(4);
	
	LeftDive_times[19][0] = 1.00000;
	LeftDive_keys[19][0] = AL::ALValue::array(0.0521140, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[19][1] = 2.10000;
	LeftDive_keys[19][1] = AL::ALValue::array(0.0275701, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[19][2] = 2.50000;
	LeftDive_keys[19][2] = AL::ALValue::array(0.108872, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[19][3] = 3.60000;
	LeftDive_keys[19][3] = AL::ALValue::array(0.0858620, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RHipRoll");
	LeftDive_times[20].arraySetSize(4);
	LeftDive_keys[20].arraySetSize(4);
	
	LeftDive_times[20][0] = 1.00000;
	LeftDive_keys[20][0] = AL::ALValue::array(0.0152980, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[20][1] = 2.10000;
	LeftDive_keys[20][1] = AL::ALValue::array(0.138018, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[20][2] = 2.50000;
	LeftDive_keys[20][2] = AL::ALValue::array(-0.112024, AL::ALValue::array(2, -0.133333, 0.0256595), AL::ALValue::array(2, 0.366667, -0.0705637));
	LeftDive_times[20][3] = 3.60000;
	LeftDive_keys[20][3] = AL::ALValue::array(-0.182588, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RHipPitch");
	LeftDive_times[21].arraySetSize(4);
	LeftDive_keys[21].arraySetSize(4);
	
	LeftDive_times[21][0] = 1.00000;
	LeftDive_keys[21][0] = AL::ALValue::array(-0.460158, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[21][1] = 2.10000;
	LeftDive_keys[21][1] = AL::ALValue::array(-0.866668, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[21][2] = 2.50000;
	LeftDive_keys[21][2] = AL::ALValue::array(0.0598679, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[21][3] = 3.60000;
	LeftDive_keys[21][3] = AL::ALValue::array(0.0568000, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RKneePitch");
	LeftDive_times[22].arraySetSize(4);
	LeftDive_keys[22].arraySetSize(4);
	
	LeftDive_times[22][0] = 1.00000;
	LeftDive_keys[22][0] = AL::ALValue::array(0.811444, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[22][1] = 2.10000;
	LeftDive_keys[22][1] = AL::ALValue::array(2.11253, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[22][2] = 2.50000;
	LeftDive_keys[22][2] = AL::ALValue::array(0.315962, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[22][3] = 3.60000;
	LeftDive_keys[22][3] = AL::ALValue::array(0.331302, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RAnklePitch");
	LeftDive_times[23].arraySetSize(4);
	LeftDive_keys[23].arraySetSize(4);
	
	LeftDive_times[23][0] = 1.00000;
	LeftDive_keys[23][0] = AL::ALValue::array(-0.403483, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[23][1] = 2.10000;
	LeftDive_keys[23][1] = AL::ALValue::array(-1.18645, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[23][2] = 2.50000;
	LeftDive_keys[23][2] = AL::ALValue::array(-0.242414, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[23][3] = 3.60000;
	LeftDive_keys[23][3] = AL::ALValue::array(-0.280764, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	
	LeftDive_names.push_back("RAnkleRoll");
	LeftDive_times[24].arraySetSize(4);
	LeftDive_keys[24].arraySetSize(4);
	
	LeftDive_times[24][0] = 1.00000;
	LeftDive_keys[24][0] = AL::ALValue::array(-0.0184499, AL::ALValue::array(2, -0.333333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[24][1] = 2.10000;
	LeftDive_keys[24][1] = AL::ALValue::array(-0.101286, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	LeftDive_times[24][2] = 2.50000;
	LeftDive_keys[24][2] = AL::ALValue::array(0.665714, AL::ALValue::array(2, -0.133333, -0.00000), AL::ALValue::array(2, 0.366667, 0.00000));
	LeftDive_times[24][3] = 3.60000;
	LeftDive_keys[24][3] = AL::ALValue::array(-0.184122, AL::ALValue::array(2, -0.366667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));
	*/
	
	
	/******************************** Punt Kick ****************************/
	

	PuntKick_names.reserve(25);
	PuntKick_times.arraySetSize(25);
	PuntKick_keys.arraySetSize(25);

	PuntKick_names.push_back("HeadYaw");
	PuntKick_times[0].arraySetSize(7);
	PuntKick_keys[0].arraySetSize(7);

	PuntKick_times[0][0] = 0.700000;
	PuntKick_keys[0][0] = AL::ALValue::array(0.0889301, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.500000, 0.00000));
	PuntKick_times[0][1] = 2.20000;
	PuntKick_keys[0][1] = AL::ALValue::array(0.0873960, AL::ALValue::array(2, -0.500000, 0.00153415), AL::ALValue::array(2, 2.96667, -0.00910261));
	PuntKick_times[0][2] = 11.1000;
	PuntKick_keys[0][2] = AL::ALValue::array(0.0122300, AL::ALValue::array(2, -2.96667, 0.0751659), AL::ALValue::array(2, 0.233333, -0.00591193));
	PuntKick_times[0][3] = 11.8000;
	PuntKick_keys[0][3] = AL::ALValue::array(-1.03703, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[0][4] = 12.5000;
	PuntKick_keys[0][4] = AL::ALValue::array(0.944902, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[0][5] = 13.4000;
	PuntKick_keys[0][5] = AL::ALValue::array(-0.0583340, AL::ALValue::array(2, -0.300000, 0.142826), AL::ALValue::array(2, 0.633333, -0.301522));
	PuntKick_times[0][6] = 15.3000;
	PuntKick_keys[0][6] = AL::ALValue::array(-0.388144, AL::ALValue::array(2, -0.633333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("HeadPitch");
	PuntKick_times[1].arraySetSize(7);
	PuntKick_keys[1].arraySetSize(7);

	PuntKick_times[1][0] = 0.700000;
	PuntKick_keys[1][0] = AL::ALValue::array(0.514900, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.500000, 0.00000));
	PuntKick_times[1][1] = 2.20000;
	PuntKick_keys[1][1] = AL::ALValue::array(-0.332920, AL::ALValue::array(2, -0.500000, -0.00000), AL::ALValue::array(2, 2.96667, 0.00000));
	PuntKick_times[1][2] = 11.1000;
	PuntKick_keys[1][2] = AL::ALValue::array(0.246933, AL::ALValue::array(2, -2.96667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[1][3] = 11.8000;
	PuntKick_keys[1][3] = AL::ALValue::array(-0.162646, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[1][4] = 12.5000;
	PuntKick_keys[1][4] = AL::ALValue::array(0.0413760, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[1][5] = 13.4000;
	PuntKick_keys[1][5] = AL::ALValue::array(-0.0445279, AL::ALValue::array(2, -0.300000, 0.0664003), AL::ALValue::array(2, 0.633333, -0.140178));
	PuntKick_times[1][6] = 15.3000;
	PuntKick_keys[1][6] = AL::ALValue::array(-0.578360, AL::ALValue::array(2, -0.633333, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LShoulderPitch");
	PuntKick_times[2].arraySetSize(16);
	PuntKick_keys[2].arraySetSize(16);

	PuntKick_times[2][0] = 0.700000;
	PuntKick_keys[2][0] = AL::ALValue::array(1.81621, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[2][1] = 1.50000;
	PuntKick_keys[2][1] = AL::ALValue::array(1.37596, AL::ALValue::array(2, -0.266667, 0.145899), AL::ALValue::array(2, 0.233333, -0.127662));
	PuntKick_times[2][2] = 2.20000;
	PuntKick_keys[2][2] = AL::ALValue::array(0.995524, AL::ALValue::array(2, -0.233333, 0.0878131), AL::ALValue::array(2, 0.266667, -0.100358));
	PuntKick_times[2][3] = 3.00000;
	PuntKick_keys[2][3] = AL::ALValue::array(0.811444, AL::ALValue::array(2, -0.266667, 0.0824951), AL::ALValue::array(2, 0.533333, -0.164990));
	PuntKick_times[2][4] = 4.60000;
	PuntKick_keys[2][4] = AL::ALValue::array(0.253067, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[2][5] = 6.00000;
	PuntKick_keys[2][5] = AL::ALValue::array(0.276078, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[2][6] = 6.40000;
	PuntKick_keys[2][6] = AL::ALValue::array(0.352778, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[2][7] = 7.40000;
	PuntKick_keys[2][7] = AL::ALValue::array(0.245399, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[2][8] = 9.10000;
	PuntKick_keys[2][8] = AL::ALValue::array(0.263807, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[2][9] = 10.5000;
	PuntKick_keys[2][9] = AL::ALValue::array(0.139552, AL::ALValue::array(0, -0.466668, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[2][10] = 11.1000;
	PuntKick_keys[2][10] = AL::ALValue::array(0.157960, AL::ALValue::array(2, -0.200000, -0.00306801), AL::ALValue::array(2, 0.466667, 0.00715869));
	PuntKick_times[2][11] = 12.5000;
	PuntKick_keys[2][11] = AL::ALValue::array(0.170232, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[2][12] = 13.4000;
	PuntKick_keys[2][12] = AL::ALValue::array(0.165630, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[2][13] = 13.6000;
	PuntKick_keys[2][13] = AL::ALValue::array(0.312894, AL::ALValue::array(2, -0.0666667, -0.0214757), AL::ALValue::array(2, 0.100000, 0.0322135));
	PuntKick_times[2][14] = 13.9000;
	PuntKick_keys[2][14] = AL::ALValue::array(0.345107, AL::ALValue::array(2, -0.100000, -0.0322135), AL::ALValue::array(2, 0.466667, 0.150330));
	PuntKick_times[2][15] = 15.3000;
	PuntKick_keys[2][15] = AL::ALValue::array(1.82081, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LShoulderRoll");
	PuntKick_times[3].arraySetSize(16);
	PuntKick_keys[3].arraySetSize(16);

	PuntKick_times[3][0] = 0.700000;
	PuntKick_keys[3][0] = AL::ALValue::array(0.187106, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[3][1] = 1.50000;
	PuntKick_keys[3][1] = AL::ALValue::array(0.346642, AL::ALValue::array(2, -0.266667, -0.0227910), AL::ALValue::array(2, 0.233333, 0.0199421));
	PuntKick_times[3][2] = 2.20000;
	PuntKick_keys[3][2] = AL::ALValue::array(0.366584, AL::ALValue::array(2, -0.233333, -0.0199421), AL::ALValue::array(2, 0.266667, 0.0227910));
	PuntKick_times[3][3] = 3.00000;
	PuntKick_keys[3][3] = AL::ALValue::array(0.708667, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[3][4] = 4.60000;
	PuntKick_keys[3][4] = AL::ALValue::array(0.236194, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[3][5] = 6.00000;
	PuntKick_keys[3][5] = AL::ALValue::array(0.0904641, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[3][6] = 6.40000;
	PuntKick_keys[3][6] = AL::ALValue::array(0.296020, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[3][7] = 7.40000;
	PuntKick_keys[3][7] = AL::ALValue::array(0.303691, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[3][8] = 9.10000;
	PuntKick_keys[3][8] = AL::ALValue::array(0.294486, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[3][9] = 10.5000;
	PuntKick_keys[3][9] = AL::ALValue::array(0.191709, AL::ALValue::array(0, -0.466668, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[3][10] = 11.1000;
	PuntKick_keys[3][10] = AL::ALValue::array(0.182504, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[3][11] = 12.5000;
	PuntKick_keys[3][11] = AL::ALValue::array(0.185572, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[3][12] = 13.4000;
	PuntKick_keys[3][12] = AL::ALValue::array(0.176367, AL::ALValue::array(2, -0.300000, 0.00920487), AL::ALValue::array(2, 0.0666667, -0.00204553));
	PuntKick_times[3][13] = 13.6000;
	PuntKick_keys[3][13] = AL::ALValue::array(0.00869999, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[3][14] = 13.9000;
	PuntKick_keys[3][14] = AL::ALValue::array(0.0260361, AL::ALValue::array(2, -0.100000, -0.0104945), AL::ALValue::array(2, 0.466667, 0.0489743));
	PuntKick_times[3][15] = 15.3000;
	PuntKick_keys[3][15] = AL::ALValue::array(0.187106, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LElbowYaw");
	PuntKick_times[4].arraySetSize(16);
	PuntKick_keys[4].arraySetSize(16);

	PuntKick_times[4][0] = 0.700000;
	PuntKick_keys[4][0] = AL::ALValue::array(-1.47575, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[4][1] = 1.50000;
	PuntKick_keys[4][1] = AL::ALValue::array(-0.971065, AL::ALValue::array(2, -0.266667, -0.0280509), AL::ALValue::array(2, 0.233333, 0.0245446));
	PuntKick_times[4][2] = 2.20000;
	PuntKick_keys[4][2] = AL::ALValue::array(-0.946520, AL::ALValue::array(2, -0.233333, -0.0245446), AL::ALValue::array(2, 0.266667, 0.0280509));
	PuntKick_times[4][3] = 3.00000;
	PuntKick_keys[4][3] = AL::ALValue::array(-0.779314, AL::ALValue::array(2, -0.266667, -0.116754), AL::ALValue::array(2, 0.533333, 0.233509));
	PuntKick_times[4][4] = 4.60000;
	PuntKick_keys[4][4] = AL::ALValue::array(0.104270, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[4][5] = 6.00000;
	PuntKick_keys[4][5] = AL::ALValue::array(0.118076, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[4][6] = 6.40000;
	PuntKick_keys[4][6] = AL::ALValue::array(0.00302603, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[4][7] = 7.40000;
	PuntKick_keys[4][7] = AL::ALValue::array(0.0751240, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[4][8] = 9.10000;
	PuntKick_keys[4][8] = AL::ALValue::array(0.0858620, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[4][9] = 10.5000;
	PuntKick_keys[4][9] = AL::ALValue::array(0.0966000, AL::ALValue::array(0, -0.466668, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[4][10] = 11.1000;
	PuntKick_keys[4][10] = AL::ALValue::array(0.108872, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[4][11] = 12.5000;
	PuntKick_keys[4][11] = AL::ALValue::array(0.108872, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[4][12] = 13.4000;
	PuntKick_keys[4][12] = AL::ALValue::array(0.110406, AL::ALValue::array(2, -0.300000, -0.000836712), AL::ALValue::array(2, 0.0666667, 0.000185936));
	PuntKick_times[4][13] = 13.6000;
	PuntKick_keys[4][13] = AL::ALValue::array(0.111940, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[4][14] = 13.9000;
	PuntKick_keys[4][14] = AL::ALValue::array(0.108872, AL::ALValue::array(2, -0.100000, 0.00306794), AL::ALValue::array(2, 0.466667, -0.0143171));
	PuntKick_times[4][15] = 15.3000;
	PuntKick_keys[4][15] = AL::ALValue::array(-1.47575, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LElbowRoll");
	PuntKick_times[5].arraySetSize(16);
	PuntKick_keys[5].arraySetSize(16);

	PuntKick_times[5][0] = 0.700000;
	PuntKick_keys[5][0] = AL::ALValue::array(-0.541460, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[5][1] = 1.50000;
	PuntKick_keys[5][1] = AL::ALValue::array(-0.509245, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[5][2] = 2.20000;
	PuntKick_keys[5][2] = AL::ALValue::array(-1.09984, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[5][3] = 3.00000;
	PuntKick_keys[5][3] = AL::ALValue::array(-1.08603, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[5][4] = 4.60000;
	PuntKick_keys[5][4] = AL::ALValue::array(-1.11671, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[5][5] = 6.00000;
	PuntKick_keys[5][5] = AL::ALValue::array(-1.10290, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[5][6] = 6.40000;
	PuntKick_keys[5][6] = AL::ALValue::array(-1.37596, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[5][7] = 7.40000;
	PuntKick_keys[5][7] = AL::ALValue::array(-1.48640, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[5][8] = 9.10000;
	PuntKick_keys[5][8] = AL::ALValue::array(-1.50021, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[5][9] = 10.5000;
	PuntKick_keys[5][9] = AL::ALValue::array(-1.33147, AL::ALValue::array(0, -0.466668, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[5][10] = 11.1000;
	PuntKick_keys[5][10] = AL::ALValue::array(-1.34374, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[5][11] = 12.5000;
	PuntKick_keys[5][11] = AL::ALValue::array(-1.32687, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[5][12] = 13.4000;
	PuntKick_keys[5][12] = AL::ALValue::array(-1.34528, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[5][13] = 13.6000;
	PuntKick_keys[5][13] = AL::ALValue::array(-1.01700, AL::ALValue::array(2, -0.0666667, -0.0122720), AL::ALValue::array(2, 0.100000, 0.0184080));
	PuntKick_times[5][14] = 13.9000;
	PuntKick_keys[5][14] = AL::ALValue::array(-0.998592, AL::ALValue::array(2, -0.100000, -0.0184080), AL::ALValue::array(2, 0.466667, 0.0859039));
	PuntKick_times[5][15] = 15.3000;
	PuntKick_keys[5][15] = AL::ALValue::array(-0.538392, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LWristYaw");
	PuntKick_times[6].arraySetSize(19);
	PuntKick_keys[6].arraySetSize(19);

	PuntKick_times[6][0] = 0.700000;
	PuntKick_keys[6][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[6][1] = 1.50000;
	PuntKick_keys[6][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[6][2] = 2.20000;
	PuntKick_keys[6][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[6][3] = 3.00000;
	PuntKick_keys[6][3] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[6][4] = 4.60000;
	PuntKick_keys[6][4] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[6][5] = 6.00000;
	PuntKick_keys[6][5] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[6][6] = 6.40000;
	PuntKick_keys[6][6] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[6][7] = 7.40000;
	PuntKick_keys[6][7] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[6][8] = 8.20000;
	PuntKick_keys[6][8] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[6][9] = 9.10000;
	PuntKick_keys[6][9] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[6][10] = 9.50000;
	PuntKick_keys[6][10] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[6][11] = 10.5000;
	PuntKick_keys[6][11] = AL::ALValue::array(0.00000, AL::ALValue::array(0, -0.333333, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[6][12] = 11.1000;
	PuntKick_keys[6][12] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[6][13] = 11.8000;
	PuntKick_keys[6][13] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[6][14] = 12.5000;
	PuntKick_keys[6][14] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[6][15] = 13.4000;
	PuntKick_keys[6][15] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[6][16] = 13.6000;
	PuntKick_keys[6][16] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[6][17] = 13.9000;
	PuntKick_keys[6][17] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[6][18] = 15.3000;
	PuntKick_keys[6][18] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LHand");
	PuntKick_times[7].arraySetSize(19);
	PuntKick_keys[7].arraySetSize(19);

	PuntKick_times[7][0] = 0.700000;
	PuntKick_keys[7][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[7][1] = 1.50000;
	PuntKick_keys[7][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[7][2] = 2.20000;
	PuntKick_keys[7][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[7][3] = 3.00000;
	PuntKick_keys[7][3] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[7][4] = 4.60000;
	PuntKick_keys[7][4] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[7][5] = 6.00000;
	PuntKick_keys[7][5] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[7][6] = 6.40000;
	PuntKick_keys[7][6] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[7][7] = 7.40000;
	PuntKick_keys[7][7] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[7][8] = 8.20000;
	PuntKick_keys[7][8] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[7][9] = 9.10000;
	PuntKick_keys[7][9] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[7][10] = 9.50000;
	PuntKick_keys[7][10] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[7][11] = 10.5000;
	PuntKick_keys[7][11] = AL::ALValue::array(0.00000, AL::ALValue::array(0, -0.333333, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[7][12] = 11.1000;
	PuntKick_keys[7][12] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[7][13] = 11.8000;
	PuntKick_keys[7][13] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[7][14] = 12.5000;
	PuntKick_keys[7][14] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[7][15] = 13.4000;
	PuntKick_keys[7][15] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[7][16] = 13.6000;
	PuntKick_keys[7][16] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[7][17] = 13.9000;
	PuntKick_keys[7][17] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[7][18] = 15.3000;
	PuntKick_keys[7][18] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RShoulderPitch");
	PuntKick_times[8].arraySetSize(16);
	PuntKick_keys[8].arraySetSize(16);

	PuntKick_times[8][0] = 0.700000;
	PuntKick_keys[8][0] = AL::ALValue::array(1.82856, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[8][1] = 1.50000;
	PuntKick_keys[8][1] = AL::ALValue::array(1.00941, AL::ALValue::array(2, -0.266667, 0.176170), AL::ALValue::array(2, 0.233333, -0.154149));
	PuntKick_times[8][2] = 2.20000;
	PuntKick_keys[8][2] = AL::ALValue::array(0.837606, AL::ALValue::array(2, -0.233333, 0.0472473), AL::ALValue::array(2, 0.266667, -0.0539969));
	PuntKick_times[8][3] = 3.00000;
	PuntKick_keys[8][3] = AL::ALValue::array(0.705682, AL::ALValue::array(2, -0.266667, 0.0615304), AL::ALValue::array(2, 0.533333, -0.123061));
	PuntKick_times[8][4] = 4.60000;
	PuntKick_keys[8][4] = AL::ALValue::array(0.283832, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[8][5] = 6.00000;
	PuntKick_keys[8][5] = AL::ALValue::array(0.283832, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[8][6] = 6.40000;
	PuntKick_keys[8][6] = AL::ALValue::array(0.247016, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[8][7] = 7.40000;
	PuntKick_keys[8][7] = AL::ALValue::array(0.230143, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[8][8] = 9.10000;
	PuntKick_keys[8][8] = AL::ALValue::array(0.251617, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[8][9] = 10.5000;
	PuntKick_keys[8][9] = AL::ALValue::array(0.217869, AL::ALValue::array(0, -0.466668, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[8][10] = 11.1000;
	PuntKick_keys[8][10] = AL::ALValue::array(0.231677, AL::ALValue::array(2, -0.200000, -0.00230104), AL::ALValue::array(2, 0.466667, 0.00536910));
	PuntKick_times[8][11] = 12.5000;
	PuntKick_keys[8][11] = AL::ALValue::array(0.240880, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[8][12] = 13.4000;
	PuntKick_keys[8][12] = AL::ALValue::array(0.236277, AL::ALValue::array(2, -0.300000, 0.00292835), AL::ALValue::array(2, 0.0666667, -0.000650743));
	PuntKick_times[8][13] = 13.6000;
	PuntKick_keys[8][13] = AL::ALValue::array(0.230143, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[8][14] = 13.9000;
	PuntKick_keys[8][14] = AL::ALValue::array(0.263890, AL::ALValue::array(2, -0.100000, -0.0337477), AL::ALValue::array(2, 0.466667, 0.157489));
	PuntKick_times[8][15] = 15.3000;
	PuntKick_keys[8][15] = AL::ALValue::array(1.82856, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RShoulderRoll");
	PuntKick_times[9].arraySetSize(17);
	PuntKick_keys[9].arraySetSize(17);

	PuntKick_times[9][0] = 0.700000;
	PuntKick_keys[9][0] = AL::ALValue::array(-0.184122, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[9][1] = 1.50000;
	PuntKick_keys[9][1] = AL::ALValue::array(-0.184122, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[9][2] = 2.20000;
	PuntKick_keys[9][2] = AL::ALValue::array(-0.0215180, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[9][3] = 3.00000;
	PuntKick_keys[9][3] = AL::ALValue::array(-0.0368580, AL::ALValue::array(2, -0.266667, 0.00613600), AL::ALValue::array(2, 0.533333, -0.0122720));
	PuntKick_times[9][4] = 4.60000;
	PuntKick_keys[9][4] = AL::ALValue::array(-0.0767419, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[9][5] = 6.00000;
	PuntKick_keys[9][5] = AL::ALValue::array(-0.0951499, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[9][6] = 6.40000;
	PuntKick_keys[9][6] = AL::ALValue::array(-0.371270, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[9][7] = 7.40000;
	PuntKick_keys[9][7] = AL::ALValue::array(-0.550747, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[9][8] = 9.10000;
	PuntKick_keys[9][8] = AL::ALValue::array(-0.483252, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[9][9] = 9.50000;
	PuntKick_keys[9][9] = AL::ALValue::array(-0.532325, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[9][10] = 10.5000;
	PuntKick_keys[9][10] = AL::ALValue::array(-0.424960, AL::ALValue::array(0, -0.333334, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[9][11] = 11.1000;
	PuntKick_keys[9][11] = AL::ALValue::array(-0.533873, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[9][12] = 12.5000;
	PuntKick_keys[9][12] = AL::ALValue::array(-0.428028, AL::ALValue::array(2, -0.466667, -0.0292571), AL::ALValue::array(2, 0.300000, 0.0188081));
	PuntKick_times[9][13] = 13.4000;
	PuntKick_keys[9][13] = AL::ALValue::array(-0.389678, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[9][14] = 13.6000;
	PuntKick_keys[9][14] = AL::ALValue::array(-0.628982, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[9][15] = 13.9000;
	PuntKick_keys[9][15] = AL::ALValue::array(-0.605971, AL::ALValue::array(2, -0.100000, -0.0230104), AL::ALValue::array(2, 0.466667, 0.107382));
	PuntKick_times[9][16] = 15.3000;
	PuntKick_keys[9][16] = AL::ALValue::array(-0.190258, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RElbowYaw");
	PuntKick_times[10].arraySetSize(16);
	PuntKick_keys[10].arraySetSize(16);

	PuntKick_times[10][0] = 0.700000;
	PuntKick_keys[10][0] = AL::ALValue::array(1.47260, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[10][1] = 1.50000;
	PuntKick_keys[10][1] = AL::ALValue::array(0.802241, AL::ALValue::array(2, -0.266667, 0.0157798), AL::ALValue::array(2, 0.233333, -0.0138073));
	PuntKick_times[10][2] = 2.20000;
	PuntKick_keys[10][2] = AL::ALValue::array(0.788433, AL::ALValue::array(2, -0.233333, 0.0138073), AL::ALValue::array(2, 0.266667, -0.0157798));
	PuntKick_times[10][3] = 3.00000;
	PuntKick_keys[10][3] = AL::ALValue::array(0.667248, AL::ALValue::array(2, -0.266667, 0.104994), AL::ALValue::array(2, 0.533333, -0.209987));
	PuntKick_times[10][4] = 4.60000;
	PuntKick_keys[10][4] = AL::ALValue::array(-0.156510, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[10][5] = 6.00000;
	PuntKick_keys[10][5] = AL::ALValue::array(-0.139636, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[10][6] = 6.40000;
	PuntKick_keys[10][6] = AL::ALValue::array(-0.108956, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[10][7] = 7.40000;
	PuntKick_keys[10][7] = AL::ALValue::array(-0.0966839, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[10][8] = 9.10000;
	PuntKick_keys[10][8] = AL::ALValue::array(-0.0874799, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[10][9] = 10.5000;
	PuntKick_keys[10][9] = AL::ALValue::array(-0.107422, AL::ALValue::array(0, -0.466668, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[10][10] = 11.1000;
	PuntKick_keys[10][10] = AL::ALValue::array(-0.121228, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[10][11] = 12.5000;
	PuntKick_keys[10][11] = AL::ALValue::array(-0.116626, AL::ALValue::array(2, -0.466667, -0.00217872), AL::ALValue::array(2, 0.300000, 0.00140060));
	PuntKick_times[10][12] = 13.4000;
	PuntKick_keys[10][12] = AL::ALValue::array(-0.110490, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[10][13] = 13.6000;
	PuntKick_keys[10][13] = AL::ALValue::array(-0.144238, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[10][14] = 13.9000;
	PuntKick_keys[10][14] = AL::ALValue::array(-0.139636, AL::ALValue::array(2, -0.100000, -0.00460208), AL::ALValue::array(2, 0.466667, 0.0214764));
	PuntKick_times[10][15] = 15.3000;
	PuntKick_keys[10][15] = AL::ALValue::array(1.47106, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RElbowRoll");
	PuntKick_times[11].arraySetSize(17);
	PuntKick_keys[11].arraySetSize(17);

	PuntKick_times[11][0] = 0.700000;
	PuntKick_keys[11][0] = AL::ALValue::array(0.515466, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[11][1] = 1.50000;
	PuntKick_keys[11][1] = AL::ALValue::array(0.546147, AL::ALValue::array(2, -0.266667, -0.0306811), AL::ALValue::array(2, 0.233333, 0.0268460));
	PuntKick_times[11][2] = 2.20000;
	PuntKick_keys[11][2] = AL::ALValue::array(0.886694, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[11][3] = 3.00000;
	PuntKick_keys[11][3] = AL::ALValue::array(0.819198, AL::ALValue::array(2, -0.266667, 0.0526672), AL::ALValue::array(2, 0.533333, -0.105334));
	PuntKick_times[11][4] = 4.60000;
	PuntKick_keys[11][4] = AL::ALValue::array(0.412688, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[11][5] = 6.00000;
	PuntKick_keys[11][5] = AL::ALValue::array(0.392746, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[11][6] = 6.40000;
	PuntKick_keys[11][6] = AL::ALValue::array(1.14441, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[11][7] = 7.40000;
	PuntKick_keys[11][7] = AL::ALValue::array(1.36990, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[11][8] = 9.10000;
	PuntKick_keys[11][8] = AL::ALValue::array(1.29320, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[11][9] = 9.50000;
	PuntKick_keys[11][9] = AL::ALValue::array(1.27409, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[11][10] = 10.5000;
	PuntKick_keys[11][10] = AL::ALValue::array(1.16435, AL::ALValue::array(0, -0.333334, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[11][11] = 11.1000;
	PuntKick_keys[11][11] = AL::ALValue::array(1.14901, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[11][12] = 12.5000;
	PuntKick_keys[11][12] = AL::ALValue::array(1.14901, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[11][13] = 13.4000;
	PuntKick_keys[11][13] = AL::ALValue::array(1.18889, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[11][14] = 13.6000;
	PuntKick_keys[11][14] = AL::ALValue::array(1.09072, AL::ALValue::array(2, -0.0666667, 0.00920371), AL::ALValue::array(2, 0.100000, -0.0138056));
	PuntKick_times[11][15] = 13.9000;
	PuntKick_keys[11][15] = AL::ALValue::array(1.07691, AL::ALValue::array(2, -0.100000, 0.0138056), AL::ALValue::array(2, 0.466667, -0.0644260));
	PuntKick_times[11][16] = 15.3000;
	PuntKick_keys[11][16] = AL::ALValue::array(0.543078, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RWristYaw");
	PuntKick_times[12].arraySetSize(19);
	PuntKick_keys[12].arraySetSize(19);

	PuntKick_times[12][0] = 0.700000;
	PuntKick_keys[12][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[12][1] = 1.50000;
	PuntKick_keys[12][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[12][2] = 2.20000;
	PuntKick_keys[12][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[12][3] = 3.00000;
	PuntKick_keys[12][3] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[12][4] = 4.60000;
	PuntKick_keys[12][4] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[12][5] = 6.00000;
	PuntKick_keys[12][5] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[12][6] = 6.40000;
	PuntKick_keys[12][6] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[12][7] = 7.40000;
	PuntKick_keys[12][7] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[12][8] = 8.20000;
	PuntKick_keys[12][8] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[12][9] = 9.10000;
	PuntKick_keys[12][9] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[12][10] = 9.50000;
	PuntKick_keys[12][10] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[12][11] = 10.5000;
	PuntKick_keys[12][11] = AL::ALValue::array(0.00000, AL::ALValue::array(0, -0.333333, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[12][12] = 11.1000;
	PuntKick_keys[12][12] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[12][13] = 11.8000;
	PuntKick_keys[12][13] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[12][14] = 12.5000;
	PuntKick_keys[12][14] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[12][15] = 13.4000;
	PuntKick_keys[12][15] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[12][16] = 13.6000;
	PuntKick_keys[12][16] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[12][17] = 13.9000;
	PuntKick_keys[12][17] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[12][18] = 15.3000;
	PuntKick_keys[12][18] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RHand");
	PuntKick_times[13].arraySetSize(19);
	PuntKick_keys[13].arraySetSize(19);

	PuntKick_times[13][0] = 0.700000;
	PuntKick_keys[13][0] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[13][1] = 1.50000;
	PuntKick_keys[13][1] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[13][2] = 2.20000;
	PuntKick_keys[13][2] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[13][3] = 3.00000;
	PuntKick_keys[13][3] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[13][4] = 4.60000;
	PuntKick_keys[13][4] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[13][5] = 6.00000;
	PuntKick_keys[13][5] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[13][6] = 6.40000;
	PuntKick_keys[13][6] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[13][7] = 7.40000;
	PuntKick_keys[13][7] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[13][8] = 8.20000;
	PuntKick_keys[13][8] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[13][9] = 9.10000;
	PuntKick_keys[13][9] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[13][10] = 9.50000;
	PuntKick_keys[13][10] = AL::ALValue::array(0.00000, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[13][11] = 10.5000;
	PuntKick_keys[13][11] = AL::ALValue::array(0.00000, AL::ALValue::array(0, -0.333333, 0.00000), AL::ALValue::array(0, 0.200000, 0.00000));
	PuntKick_times[13][12] = 11.1000;
	PuntKick_keys[13][12] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[13][13] = 11.8000;
	PuntKick_keys[13][13] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[13][14] = 12.5000;
	PuntKick_keys[13][14] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[13][15] = 13.4000;
	PuntKick_keys[13][15] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[13][16] = 13.6000;
	PuntKick_keys[13][16] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[13][17] = 13.9000;
	PuntKick_keys[13][17] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[13][18] = 15.3000;
	PuntKick_keys[13][18] = AL::ALValue::array(0.00000, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LHipYawPitch");
	PuntKick_times[14].arraySetSize(19);
	PuntKick_keys[14].arraySetSize(19);

	PuntKick_times[14][0] = 0.700000;
	PuntKick_keys[14][0] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[14][1] = 1.50000;
	PuntKick_keys[14][1] = AL::ALValue::array(-0.0843280, AL::ALValue::array(2, -0.266667, 0.0376343), AL::ALValue::array(2, 0.233333, -0.0329300));
	PuntKick_times[14][2] = 2.20000;
	PuntKick_keys[14][2] = AL::ALValue::array(-0.211651, AL::ALValue::array(2, -0.233333, 0.0424748), AL::ALValue::array(2, 0.266667, -0.0485427));
	PuntKick_times[14][3] = 3.00000;
	PuntKick_keys[14][3] = AL::ALValue::array(-0.357381, AL::ALValue::array(2, -0.266667, 0.0606781), AL::ALValue::array(2, 0.533333, -0.121356));
	PuntKick_times[14][4] = 4.60000;
	PuntKick_keys[14][4] = AL::ALValue::array(-0.757754, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[14][5] = 6.00000;
	PuntKick_keys[14][5] = AL::ALValue::array(-0.759288, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[14][6] = 6.40000;
	PuntKick_keys[14][6] = AL::ALValue::array(-0.759288, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[14][7] = 7.40000;
	PuntKick_keys[14][7] = AL::ALValue::array(-0.754686, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[14][8] = 8.20000;
	PuntKick_keys[14][8] = AL::ALValue::array(-0.722472, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[14][9] = 9.10000;
	PuntKick_keys[14][9] = AL::ALValue::array(-0.604353, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[14][10] = 9.50000;
	PuntKick_keys[14][10] = AL::ALValue::array(-0.604353, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[14][11] = 10.5000;
	PuntKick_keys[14][11] = AL::ALValue::array(-0.481634, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[14][12] = 11.1000;
	PuntKick_keys[14][12] = AL::ALValue::array(-0.145688, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[14][13] = 11.8000;
	PuntKick_keys[14][13] = AL::ALValue::array(-0.145688, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[14][14] = 12.5000;
	PuntKick_keys[14][14] = AL::ALValue::array(-0.145688, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[14][15] = 13.4000;
	PuntKick_keys[14][15] = AL::ALValue::array(-0.145688, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[14][16] = 13.6000;
	PuntKick_keys[14][16] = AL::ALValue::array(-0.154892, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[14][17] = 13.9000;
	PuntKick_keys[14][17] = AL::ALValue::array(-0.154892, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[14][18] = 15.3000;
	PuntKick_keys[14][18] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LHipRoll");
	PuntKick_times[15].arraySetSize(19);
	PuntKick_keys[15].arraySetSize(19);

	PuntKick_times[15][0] = 0.700000;
	PuntKick_keys[15][0] = AL::ALValue::array(0.00157596, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[15][1] = 1.50000;
	PuntKick_keys[15][1] = AL::ALValue::array(-0.0122300, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[15][2] = 2.20000;
	PuntKick_keys[15][2] = AL::ALValue::array(0.0828780, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[15][3] = 3.00000;
	PuntKick_keys[15][3] = AL::ALValue::array(0.00310997, AL::ALValue::array(2, -0.266667, 0.0513039), AL::ALValue::array(2, 0.533333, -0.102608));
	PuntKick_times[15][4] = 4.60000;
	PuntKick_keys[15][4] = AL::ALValue::array(-0.378857, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[15][5] = 6.00000;
	PuntKick_keys[15][5] = AL::ALValue::array(-0.378857, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[15][6] = 6.40000;
	PuntKick_keys[15][6] = AL::ALValue::array(-0.378857, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[15][7] = 7.40000;
	PuntKick_keys[15][7] = AL::ALValue::array(-0.354312, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[15][8] = 8.20000;
	PuntKick_keys[15][8] = AL::ALValue::array(-0.205514, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[15][9] = 9.10000;
	PuntKick_keys[15][9] = AL::ALValue::array(0.125830, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[15][10] = 9.50000;
	PuntKick_keys[15][10] = AL::ALValue::array(0.125830, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[15][11] = 10.5000;
	PuntKick_keys[15][11] = AL::ALValue::array(0.131966, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[15][12] = 11.1000;
	PuntKick_keys[15][12] = AL::ALValue::array(0.0660040, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[15][13] = 11.8000;
	PuntKick_keys[15][13] = AL::ALValue::array(0.0660040, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[15][14] = 12.5000;
	PuntKick_keys[15][14] = AL::ALValue::array(0.0660040, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[15][15] = 13.4000;
	PuntKick_keys[15][15] = AL::ALValue::array(-0.119610, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[15][16] = 13.6000;
	PuntKick_keys[15][16] = AL::ALValue::array(-0.108872, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[15][17] = 13.9000;
	PuntKick_keys[15][17] = AL::ALValue::array(-0.115008, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[15][18] = 15.3000;
	PuntKick_keys[15][18] = AL::ALValue::array(4.19617e-05, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LHipPitch");
	PuntKick_times[16].arraySetSize(19);
	PuntKick_keys[16].arraySetSize(19);

	PuntKick_times[16][0] = 0.700000;
	PuntKick_keys[16][0] = AL::ALValue::array(-0.384992, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[16][1] = 1.50000;
	PuntKick_keys[16][1] = AL::ALValue::array(-0.325165, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[16][2] = 2.20000;
	PuntKick_keys[16][2] = AL::ALValue::array(-0.831386, AL::ALValue::array(2, -0.233333, 0.174672), AL::ALValue::array(2, 0.266667, -0.199625));
	PuntKick_times[16][3] = 3.00000;
	PuntKick_keys[16][3] = AL::ALValue::array(-1.44805, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[16][4] = 4.60000;
	PuntKick_keys[16][4] = AL::ALValue::array(-1.38976, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[16][5] = 6.00000;
	PuntKick_keys[16][5] = AL::ALValue::array(-1.38976, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[16][6] = 6.40000;
	PuntKick_keys[16][6] = AL::ALValue::array(-1.38823, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[16][7] = 7.40000;
	PuntKick_keys[16][7] = AL::ALValue::array(-0.990923, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[16][8] = 8.20000;
	PuntKick_keys[16][8] = AL::ALValue::array(-0.742414, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[16][9] = 9.10000;
	PuntKick_keys[16][9] = AL::ALValue::array(-0.418739, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[16][10] = 9.50000;
	PuntKick_keys[16][10] = AL::ALValue::array(-0.418739, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[16][11] = 10.5000;
	PuntKick_keys[16][11] = AL::ALValue::array(-0.713267, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[16][12] = 11.1000;
	PuntKick_keys[16][12] = AL::ALValue::array(-0.503110, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[16][13] = 11.8000;
	PuntKick_keys[16][13] = AL::ALValue::array(-0.503110, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[16][14] = 12.5000;
	PuntKick_keys[16][14] = AL::ALValue::array(-0.503110, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[16][15] = 13.4000;
	PuntKick_keys[16][15] = AL::ALValue::array(-0.526121, AL::ALValue::array(2, -0.300000, 0.0230104), AL::ALValue::array(2, 0.0666667, -0.00511343));
	PuntKick_times[16][16] = 13.6000;
	PuntKick_keys[16][16] = AL::ALValue::array(-0.618161, AL::ALValue::array(2, -0.0666667, 0.00204438), AL::ALValue::array(2, 0.100000, -0.00306656));
	PuntKick_times[16][17] = 13.9000;
	PuntKick_keys[16][17] = AL::ALValue::array(-0.621227, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[16][18] = 15.3000;
	PuntKick_keys[16][18] = AL::ALValue::array(-0.384992, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LKneePitch");
	PuntKick_times[17].arraySetSize(19);
	PuntKick_keys[17].arraySetSize(19);

	PuntKick_times[17][0] = 0.700000;
	PuntKick_keys[17][0] = AL::ALValue::array(0.941834, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[17][1] = 1.50000;
	PuntKick_keys[17][1] = AL::ALValue::array(1.34988, AL::ALValue::array(2, -0.266667, -0.199898), AL::ALValue::array(2, 0.233333, 0.174911));
	PuntKick_times[17][2] = 2.20000;
	PuntKick_keys[17][2] = AL::ALValue::array(2.06626, AL::ALValue::array(2, -0.233333, -0.0402560), AL::ALValue::array(2, 0.266667, 0.0460069));
	PuntKick_times[17][3] = 3.00000;
	PuntKick_keys[17][3] = AL::ALValue::array(2.11227, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[17][4] = 4.60000;
	PuntKick_keys[17][4] = AL::ALValue::array(2.11227, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[17][5] = 6.00000;
	PuntKick_keys[17][5] = AL::ALValue::array(2.11227, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[17][6] = 6.40000;
	PuntKick_keys[17][6] = AL::ALValue::array(2.11253, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[17][7] = 7.40000;
	PuntKick_keys[17][7] = AL::ALValue::array(2.11075, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[17][8] = 8.20000;
	PuntKick_keys[17][8] = AL::ALValue::array(2.07546, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[17][9] = 9.10000;
	PuntKick_keys[17][9] = AL::ALValue::array(1.89291, AL::ALValue::array(2, -0.300000, 0.0495606), AL::ALValue::array(2, 0.133333, -0.0220270));
	PuntKick_times[17][10] = 9.50000;
	PuntKick_keys[17][10] = AL::ALValue::array(1.86070, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[17][11] = 10.5000;
	PuntKick_keys[17][11] = AL::ALValue::array(1.43271, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[17][12] = 11.1000;
	PuntKick_keys[17][12] = AL::ALValue::array(0.472429, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[17][13] = 11.8000;
	PuntKick_keys[17][13] = AL::ALValue::array(0.472429, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[17][14] = 12.5000;
	PuntKick_keys[17][14] = AL::ALValue::array(0.470897, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[17][15] = 13.4000;
	PuntKick_keys[17][15] = AL::ALValue::array(0.618161, AL::ALValue::array(2, -0.300000, -0.0707034), AL::ALValue::array(2, 0.0666667, 0.0157119));
	PuntKick_times[17][16] = 13.6000;
	PuntKick_keys[17][16] = AL::ALValue::array(0.730143, AL::ALValue::array(2, -0.0666667, -0.00102163), AL::ALValue::array(2, 0.100000, 0.00153245));
	PuntKick_times[17][17] = 13.9000;
	PuntKick_keys[17][17] = AL::ALValue::array(0.731675, AL::ALValue::array(2, -0.100000, -0.00153245), AL::ALValue::array(2, 0.466667, 0.00715143));
	PuntKick_times[17][18] = 15.3000;
	PuntKick_keys[17][18] = AL::ALValue::array(0.941834, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LAnklePitch");
	PuntKick_times[18].arraySetSize(19);
	PuntKick_keys[18].arraySetSize(19);

	PuntKick_times[18][0] = 0.700000;
	PuntKick_keys[18][0] = AL::ALValue::array(-0.556884, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[18][1] = 1.50000;
	PuntKick_keys[18][1] = AL::ALValue::array(-0.902033, AL::ALValue::array(2, -0.266667, 0.112468), AL::ALValue::array(2, 0.233333, -0.0984093));
	PuntKick_times[18][2] = 2.20000;
	PuntKick_keys[18][2] = AL::ALValue::array(-1.18952, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[18][3] = 3.00000;
	PuntKick_keys[18][3] = AL::ALValue::array(-1.18952, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[18][4] = 4.60000;
	PuntKick_keys[18][4] = AL::ALValue::array(-1.18952, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[18][5] = 6.00000;
	PuntKick_keys[18][5] = AL::ALValue::array(-1.18952, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[18][6] = 6.40000;
	PuntKick_keys[18][6] = AL::ALValue::array(-1.18889, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[18][7] = 7.40000;
	PuntKick_keys[18][7] = AL::ALValue::array(-1.18952, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[18][8] = 8.20000;
	PuntKick_keys[18][8] = AL::ALValue::array(-1.18582, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[18][9] = 9.10000;
	PuntKick_keys[18][9] = AL::ALValue::array(-1.13367, AL::ALValue::array(2, -0.300000, -0.0162839), AL::ALValue::array(2, 0.133333, 0.00723730));
	PuntKick_times[18][10] = 9.50000;
	PuntKick_keys[18][10] = AL::ALValue::array(-1.11526, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[18][11] = 10.5000;
	PuntKick_keys[18][11] = AL::ALValue::array(-0.719487, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[18][12] = 11.1000;
	PuntKick_keys[18][12] = AL::ALValue::array(-0.128898, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[18][13] = 11.8000;
	PuntKick_keys[18][13] = AL::ALValue::array(-0.128898, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[18][14] = 12.5000;
	PuntKick_keys[18][14] = AL::ALValue::array(-0.128898, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[18][15] = 13.4000;
	PuntKick_keys[18][15] = AL::ALValue::array(-0.188724, AL::ALValue::array(2, -0.300000, 0.0288669), AL::ALValue::array(2, 0.0666667, -0.00641487));
	PuntKick_times[18][16] = 13.6000;
	PuntKick_keys[18][16] = AL::ALValue::array(-0.234743, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[18][17] = 13.9000;
	PuntKick_keys[18][17] = AL::ALValue::array(-0.234743, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[18][18] = 15.3000;
	PuntKick_keys[18][18] = AL::ALValue::array(-0.556884, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("LAnkleRoll");
	PuntKick_times[19].arraySetSize(19);
	PuntKick_keys[19].arraySetSize(19);

	PuntKick_times[19][0] = 0.700000;
	PuntKick_keys[19][0] = AL::ALValue::array(-0.00149204, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[19][1] = 1.50000;
	PuntKick_keys[19][1] = AL::ALValue::array(0.153442, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[19][2] = 2.20000;
	PuntKick_keys[19][2] = AL::ALValue::array(0.0491300, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[19][3] = 3.00000;
	PuntKick_keys[19][3] = AL::ALValue::array(0.0491300, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[19][4] = 4.60000;
	PuntKick_keys[19][4] = AL::ALValue::array(0.0429939, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[19][5] = 6.00000;
	PuntKick_keys[19][5] = AL::ALValue::array(0.0429939, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[19][6] = 6.40000;
	PuntKick_keys[19][6] = AL::ALValue::array(0.0429939, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[19][7] = 7.40000;
	PuntKick_keys[19][7] = AL::ALValue::array(0.0491300, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[19][8] = 8.20000;
	PuntKick_keys[19][8] = AL::ALValue::array(0.0506639, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[19][9] = 9.10000;
	PuntKick_keys[19][9] = AL::ALValue::array(0.0767419, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[19][10] = 9.50000;
	PuntKick_keys[19][10] = AL::ALValue::array(0.0767419, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[19][11] = 10.5000;
	PuntKick_keys[19][11] = AL::ALValue::array(0.00157596, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[19][12] = 11.1000;
	PuntKick_keys[19][12] = AL::ALValue::array(-0.102736, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[19][13] = 11.8000;
	PuntKick_keys[19][13] = AL::ALValue::array(-0.101202, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[19][14] = 12.5000;
	PuntKick_keys[19][14] = AL::ALValue::array(-0.102736, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[19][15] = 13.4000;
	PuntKick_keys[19][15] = AL::ALValue::array(0.148840, AL::ALValue::array(2, -0.300000, -0.0970602), AL::ALValue::array(2, 0.0666667, 0.0215689));
	PuntKick_times[19][16] = 13.6000;
	PuntKick_keys[19][16] = AL::ALValue::array(0.253151, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[19][17] = 13.9000;
	PuntKick_keys[19][17] = AL::ALValue::array(0.251617, AL::ALValue::array(2, -0.100000, 0.00153415), AL::ALValue::array(2, 0.466667, -0.00715935));
	PuntKick_times[19][18] = 15.3000;
	PuntKick_keys[19][18] = AL::ALValue::array(-0.00149204, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RHipRoll");
	PuntKick_times[20].arraySetSize(19);
	PuntKick_keys[20].arraySetSize(19);

	PuntKick_times[20][0] = 0.700000;
	PuntKick_keys[20][0] = AL::ALValue::array(0.0291880, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[20][1] = 1.50000;
	PuntKick_keys[20][1] = AL::ALValue::array(-0.0291041, AL::ALValue::array(2, -0.266667, 0.0182717), AL::ALValue::array(2, 0.233333, -0.0159877));
	PuntKick_times[20][2] = 2.20000;
	PuntKick_keys[20][2] = AL::ALValue::array(-0.0735901, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[20][3] = 3.00000;
	PuntKick_keys[20][3] = AL::ALValue::array(-0.0659200, AL::ALValue::array(2, -0.266667, -0.00767002), AL::ALValue::array(2, 0.533333, 0.0153400));
	PuntKick_times[20][4] = 4.60000;
	PuntKick_keys[20][4] = AL::ALValue::array(0.319114, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[20][5] = 6.00000;
	PuntKick_keys[20][5] = AL::ALValue::array(0.319114, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[20][6] = 6.40000;
	PuntKick_keys[20][6] = AL::ALValue::array(0.319114, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[20][7] = 7.40000;
	PuntKick_keys[20][7] = AL::ALValue::array(0.0767419, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[20][8] = 8.20000;
	PuntKick_keys[20][8] = AL::ALValue::array(0.0552660, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[20][9] = 9.10000;
	PuntKick_keys[20][9] = AL::ALValue::array(0.0491300, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[20][10] = 9.50000;
	PuntKick_keys[20][10] = AL::ALValue::array(0.0491300, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[20][11] = 10.5000;
	PuntKick_keys[20][11] = AL::ALValue::array(0.0951499, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[20][12] = 11.1000;
	PuntKick_keys[20][12] = AL::ALValue::array(0.0460620, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[20][13] = 11.8000;
	PuntKick_keys[20][13] = AL::ALValue::array(0.0460620, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[20][14] = 12.5000;
	PuntKick_keys[20][14] = AL::ALValue::array(0.0460620, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[20][15] = 13.4000;
	PuntKick_keys[20][15] = AL::ALValue::array(-0.00302603, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[20][16] = 13.6000;
	PuntKick_keys[20][16] = AL::ALValue::array(-0.00149204, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[20][17] = 13.9000;
	PuntKick_keys[20][17] = AL::ALValue::array(-0.00149204, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[20][18] = 15.3000;
	PuntKick_keys[20][18] = AL::ALValue::array(0.0291880, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RHipPitch");
	PuntKick_times[21].arraySetSize(19);
	PuntKick_keys[21].arraySetSize(19);

	PuntKick_times[21][0] = 0.700000;
	PuntKick_keys[21][0] = AL::ALValue::array(-0.385075, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[21][1] = 1.50000;
	PuntKick_keys[21][1] = AL::ALValue::array(-0.609040, AL::ALValue::array(2, -0.266667, 0.154900), AL::ALValue::array(2, 0.233333, -0.135537));
	PuntKick_times[21][2] = 2.20000;
	PuntKick_keys[21][2] = AL::ALValue::array(-1.25639, AL::ALValue::array(2, -0.233333, 0.126947), AL::ALValue::array(2, 0.266667, -0.145082));
	PuntKick_times[21][3] = 3.00000;
	PuntKick_keys[21][3] = AL::ALValue::array(-1.42513, AL::ALValue::array(2, -0.266667, 0.0424408), AL::ALValue::array(2, 0.533333, -0.0848815));
	PuntKick_times[21][4] = 4.60000;
	PuntKick_keys[21][4] = AL::ALValue::array(-1.63835, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[21][5] = 6.00000;
	PuntKick_keys[21][5] = AL::ALValue::array(-1.63989, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[21][6] = 6.40000;
	PuntKick_keys[21][6] = AL::ALValue::array(-1.64142, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[21][7] = 7.40000;
	PuntKick_keys[21][7] = AL::ALValue::array(-1.58160, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[21][8] = 8.20000;
	PuntKick_keys[21][8] = AL::ALValue::array(-1.23491, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[21][9] = 9.10000;
	PuntKick_keys[21][9] = AL::ALValue::array(-0.483252, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[21][10] = 9.50000;
	PuntKick_keys[21][10] = AL::ALValue::array(-0.483252, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[21][11] = 10.5000;
	PuntKick_keys[21][11] = AL::ALValue::array(-0.704148, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[21][12] = 11.1000;
	PuntKick_keys[21][12] = AL::ALValue::array(-0.451038, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[21][13] = 11.8000;
	PuntKick_keys[21][13] = AL::ALValue::array(-0.451038, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[21][14] = 12.5000;
	PuntKick_keys[21][14] = AL::ALValue::array(-0.451038, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[21][15] = 13.4000;
	PuntKick_keys[21][15] = AL::ALValue::array(-0.447970, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[21][16] = 13.6000;
	PuntKick_keys[21][16] = AL::ALValue::array(-0.667332, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[21][17] = 13.9000;
	PuntKick_keys[21][17] = AL::ALValue::array(-0.664264, AL::ALValue::array(2, -0.100000, -0.00306823), AL::ALValue::array(2, 0.466667, 0.0143184));
	PuntKick_times[21][18] = 15.3000;
	PuntKick_keys[21][18] = AL::ALValue::array(-0.386610, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RKneePitch");
	PuntKick_times[22].arraySetSize(19);
	PuntKick_keys[22].arraySetSize(19);

	PuntKick_times[22][0] = 0.700000;
	PuntKick_keys[22][0] = AL::ALValue::array(0.941918, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[22][1] = 1.50000;
	PuntKick_keys[22][1] = AL::ALValue::array(1.17815, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[22][2] = 2.20000;
	PuntKick_keys[22][2] = AL::ALValue::array(1.07384, AL::ALValue::array(2, -0.233333, 0.104313), AL::ALValue::array(2, 0.266667, -0.119215));
	PuntKick_times[22][3] = 3.00000;
	PuntKick_keys[22][3] = AL::ALValue::array(0.0951499, AL::ALValue::array(2, -0.266667, -0.00000), AL::ALValue::array(2, 0.533333, 0.00000));
	PuntKick_times[22][4] = 4.60000;
	PuntKick_keys[22][4] = AL::ALValue::array(0.339056, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[22][5] = 6.00000;
	PuntKick_keys[22][5] = AL::ALValue::array(0.337522, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[22][6] = 6.40000;
	PuntKick_keys[22][6] = AL::ALValue::array(0.337522, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[22][7] = 7.40000;
	PuntKick_keys[22][7] = AL::ALValue::array(0.369736, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[22][8] = 8.20000;
	PuntKick_keys[22][8] = AL::ALValue::array(0.312978, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[22][9] = 9.10000;
	PuntKick_keys[22][9] = AL::ALValue::array(0.165714, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[22][10] = 9.50000;
	PuntKick_keys[22][10] = AL::ALValue::array(0.165714, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[22][11] = 10.5000;
	PuntKick_keys[22][11] = AL::ALValue::array(0.731761, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[22][12] = 11.1000;
	PuntKick_keys[22][12] = AL::ALValue::array(0.587563, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[22][13] = 11.8000;
	PuntKick_keys[22][13] = AL::ALValue::array(0.587563, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[22][14] = 12.5000;
	PuntKick_keys[22][14] = AL::ALValue::array(0.589097, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[22][15] = 13.4000;
	PuntKick_keys[22][15] = AL::ALValue::array(0.579894, AL::ALValue::array(2, -0.300000, 0.00920315), AL::ALValue::array(2, 0.0666667, -0.00204514));
	PuntKick_times[22][16] = 13.6000;
	PuntKick_keys[22][16] = AL::ALValue::array(0.254685, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[22][17] = 13.9000;
	PuntKick_keys[22][17] = AL::ALValue::array(0.254685, AL::ALValue::array(2, -0.100000, -0.00000), AL::ALValue::array(2, 0.466667, 0.00000));
	PuntKick_times[22][18] = 15.3000;
	PuntKick_keys[22][18] = AL::ALValue::array(0.941918, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RAnklePitch");
	PuntKick_times[23].arraySetSize(19);
	PuntKick_keys[23].arraySetSize(19);

	PuntKick_times[23][0] = 0.700000;
	PuntKick_keys[23][0] = AL::ALValue::array(-0.556800, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[23][1] = 1.50000;
	PuntKick_keys[23][1] = AL::ALValue::array(-0.429478, AL::ALValue::array(2, -0.266667, -0.127322), AL::ALValue::array(2, 0.233333, 0.111407));
	PuntKick_times[23][2] = 2.20000;
	PuntKick_keys[23][2] = AL::ALValue::array(0.250085, AL::ALValue::array(2, -0.233333, -0.199966), AL::ALValue::array(2, 0.266667, 0.228532));
	PuntKick_times[23][3] = 3.00000;
	PuntKick_keys[23][3] = AL::ALValue::array(0.856014, AL::ALValue::array(2, -0.266667, -0.00997104), AL::ALValue::array(2, 0.533333, 0.0199421));
	PuntKick_times[23][4] = 4.60000;
	PuntKick_keys[23][4] = AL::ALValue::array(0.875956, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[23][5] = 6.00000;
	PuntKick_keys[23][5] = AL::ALValue::array(0.874422, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[23][6] = 6.40000;
	PuntKick_keys[23][6] = AL::ALValue::array(0.875956, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[23][7] = 7.40000;
	PuntKick_keys[23][7] = AL::ALValue::array(0.912772, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[23][8] = 8.20000;
	PuntKick_keys[23][8] = AL::ALValue::array(0.880559, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[23][9] = 9.10000;
	PuntKick_keys[23][9] = AL::ALValue::array(0.763974, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[23][10] = 9.50000;
	PuntKick_keys[23][10] = AL::ALValue::array(0.763974, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[23][11] = 10.5000;
	PuntKick_keys[23][11] = AL::ALValue::array(0.0951499, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[23][12] = 11.1000;
	PuntKick_keys[23][12] = AL::ALValue::array(-0.286815, AL::ALValue::array(2, -0.200000, 0.00131498), AL::ALValue::array(2, 0.233333, -0.00153415));
	PuntKick_times[23][13] = 11.8000;
	PuntKick_keys[23][13] = AL::ALValue::array(-0.288349, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[23][14] = 12.5000;
	PuntKick_keys[23][14] = AL::ALValue::array(-0.285283, AL::ALValue::array(2, -0.233333, -0.00306653), AL::ALValue::array(2, 0.300000, 0.00394268));
	PuntKick_times[23][15] = 13.4000;
	PuntKick_keys[23][15] = AL::ALValue::array(-0.214717, AL::ALValue::array(2, -0.300000, -0.0705654), AL::ALValue::array(2, 0.0666667, 0.0156812));
	PuntKick_times[23][16] = 13.6000;
	PuntKick_keys[23][16] = AL::ALValue::array(0.190258, AL::ALValue::array(2, -0.0666667, -0.00000), AL::ALValue::array(2, 0.100000, 0.00000));
	PuntKick_times[23][17] = 13.9000;
	PuntKick_keys[23][17] = AL::ALValue::array(0.188724, AL::ALValue::array(2, -0.100000, 0.00153415), AL::ALValue::array(2, 0.466667, -0.00715935));
	PuntKick_times[23][18] = 15.3000;
	PuntKick_keys[23][18] = AL::ALValue::array(-0.556800, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

	PuntKick_names.push_back("RAnkleRoll");
	PuntKick_times[24].arraySetSize(19);
	PuntKick_keys[24].arraySetSize(19);

	PuntKick_times[24][0] = 0.700000;
	PuntKick_keys[24][0] = AL::ALValue::array(-0.0291041, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[24][1] = 1.50000;
	PuntKick_keys[24][1] = AL::ALValue::array(0.131966, AL::ALValue::array(2, -0.266667, -0.0499060), AL::ALValue::array(2, 0.233333, 0.0436677));
	PuntKick_times[24][2] = 2.20000;
	PuntKick_keys[24][2] = AL::ALValue::array(0.251617, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.266667, 0.00000));
	PuntKick_times[24][3] = 3.00000;
	PuntKick_keys[24][3] = AL::ALValue::array(0.115092, AL::ALValue::array(2, -0.266667, 0.0122719), AL::ALValue::array(2, 0.533333, -0.0245439));
	PuntKick_times[24][4] = 4.60000;
	PuntKick_keys[24][4] = AL::ALValue::array(0.0905480, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[24][5] = 6.00000;
	PuntKick_keys[24][5] = AL::ALValue::array(0.0905480, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[24][6] = 6.40000;
	PuntKick_keys[24][6] = AL::ALValue::array(0.0905480, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[24][7] = 7.40000;
	PuntKick_keys[24][7] = AL::ALValue::array(0.0506639, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[24][8] = 8.20000;
	PuntKick_keys[24][8] = AL::ALValue::array(0.0429939, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[24][9] = 9.10000;
	PuntKick_keys[24][9] = AL::ALValue::array(0.0399260, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.133333, 0.00000));
	PuntKick_times[24][10] = 9.50000;
	PuntKick_keys[24][10] = AL::ALValue::array(0.0399260, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[24][11] = 10.5000;
	PuntKick_keys[24][11] = AL::ALValue::array(0.121228, AL::ALValue::array(1, 0.00000, 0.00000), AL::ALValue::array(1, 0.00000, 0.00000));
	PuntKick_times[24][12] = 11.1000;
	PuntKick_keys[24][12] = AL::ALValue::array(-0.0444441, AL::ALValue::array(2, -0.200000, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[24][13] = 11.8000;
	PuntKick_keys[24][13] = AL::ALValue::array(-0.0444441, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.233333, 0.00000));
	PuntKick_times[24][14] = 12.5000;
	PuntKick_keys[24][14] = AL::ALValue::array(-0.0444441, AL::ALValue::array(2, -0.233333, -0.00000), AL::ALValue::array(2, 0.300000, 0.00000));
	PuntKick_times[24][15] = 13.4000;
	PuntKick_keys[24][15] = AL::ALValue::array(0.0368580, AL::ALValue::array(2, -0.300000, -0.00000), AL::ALValue::array(2, 0.0666667, 0.00000));
	PuntKick_times[24][16] = 13.6000;
	PuntKick_keys[24][16] = AL::ALValue::array(0.0291880, AL::ALValue::array(2, -0.0666667, 0.00102265), AL::ALValue::array(2, 0.100000, -0.00153397));
	PuntKick_times[24][17] = 13.9000;
	PuntKick_keys[24][17] = AL::ALValue::array(0.0276540, AL::ALValue::array(2, -0.100000, 0.00153397), AL::ALValue::array(2, 0.466667, -0.00715853));
	PuntKick_times[24][18] = 15.3000;
	PuntKick_keys[24][18] = AL::ALValue::array(-0.0291041, AL::ALValue::array(2, -0.466667, -0.00000), AL::ALValue::array(2, 0.00000, 0.00000));

}
