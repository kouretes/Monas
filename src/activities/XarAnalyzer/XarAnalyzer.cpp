#include "XarAnalyzer.h"


using namespace boost::posix_time;
using namespace KDeviceLists;

namespace {
	ActivityRegistrar<XarAnalyzer>::Type temp("XarAnalyzer");
}
using namespace std;

XarAnalyzer::XarAnalyzer() {

}

void XarAnalyzer::UserInit() {
	
	_blk->updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);

	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);

	amot = new MotionActionMessage();
	play = false;
	gameState = PLAYER_INITIAL;

	kickTime = microsec_clock::universal_time();
	flag = 0;
	temp = 0;
}


int XarAnalyzer::Execute() {
	
	read_messages();

	if (gsm != 0) {
		gameState = gsm->player_state();

		if (gameState == PLAYER_PLAYING) {
			play = true;
		}
		else if (gameState == PLAYER_PENALISED) {
			play = false;
			velocityWalk(0.0,0.0,0.0,1);
		}
	}


	if (play) {
		if(flag == 0){
			amot->set_command("KickSideRightFast.xar");
			_blk->publishSignal(*amot, "motion");
			kickTime = microsec_clock::universal_time();
			flag = 1;
		}

		if(microsec_clock::universal_time() <= kickTime+seconds(ACTION_TIME) && microsec_clock::universal_time() >= kickTime && flag == 1){
			if(allsm != 0){
				angleStore[temp][0] = allsm->jointdata(HEAD+YAW).sensorvalue();
				angleStore[temp][1] = allsm->jointdata(HEAD+PITCH).sensorvalue();

				angleStore[temp][2] = allsm->jointdata(L_LEG+HIP_YAW_PITCH).sensorvalue();
				angleStore[temp][3] = allsm->jointdata(L_LEG+HIP_ROLL).sensorvalue();
				angleStore[temp][4] = allsm->jointdata(L_LEG+HIP_PITCH).sensorvalue();
				angleStore[temp][5] = allsm->jointdata(L_LEG+KNEE_PITCH).sensorvalue();
				angleStore[temp][6] = allsm->jointdata(L_LEG+ANKLE_PITCH).sensorvalue();
				angleStore[temp][7] = allsm->jointdata(L_LEG+ANKLE_ROLL).sensorvalue();

				angleStore[temp][8] = allsm->jointdata(R_LEG+HIP_YAW_PITCH).sensorvalue();
				angleStore[temp][9] = allsm->jointdata(R_LEG+HIP_ROLL).sensorvalue();
				angleStore[temp][10] = allsm->jointdata(R_LEG+HIP_PITCH).sensorvalue();
				angleStore[temp][11] = allsm->jointdata(R_LEG+KNEE_PITCH).sensorvalue();
				angleStore[temp][12] = allsm->jointdata(R_LEG+ANKLE_PITCH).sensorvalue();
				angleStore[temp][13] = allsm->jointdata(R_LEG+ANKLE_ROLL).sensorvalue();

				angleStore[temp][14] = allsm->jointdata(L_ARM+SHOULDER_PITCH).sensorvalue();
				angleStore[temp][15] = allsm->jointdata(L_ARM+SHOULDER_ROLL).sensorvalue();
				angleStore[temp][16] = allsm->jointdata(L_ARM+ELBOW_ROLL).sensorvalue();
				angleStore[temp][17] = allsm->jointdata(L_ARM+ELBOW_YAW).sensorvalue();

				angleStore[temp][18] = allsm->jointdata(R_ARM+SHOULDER_PITCH).sensorvalue();
				angleStore[temp][19] = allsm->jointdata(R_ARM+SHOULDER_ROLL).sensorvalue();
				angleStore[temp][20] = allsm->jointdata(R_ARM+ELBOW_ROLL).sensorvalue();
				angleStore[temp][21] = allsm->jointdata(R_ARM+ELBOW_YAW).sensorvalue();

				temp++;
			}
			if(temp == ACTION_TIME*XarAnalyzerFPS)
				createKmeFile();
		}
	
	} else if (!play) {   // Non-Play state
		velocityWalk(0.0,0.0,0.0,1);
	}

	return 0;
}


void XarAnalyzer::read_messages() {
	gsm  = _blk->readState<GameStateMessage> ("behavior");
	allsm = _blk->readData<AllSensorValuesMessage> ("sensors");
}


void XarAnalyzer::createKmeFile() {
	Logger::Instance().WriteMsg("XarAnalyzer", "CREATE FILE", Logger::ExtraInfo);
	ofstream fin, finM;
	string fileName, fileNameM;
	float num, frameTime;
	unsigned int pos = 0;
	frameTime = 0.0;
	num = 0.0;
	fileName = "KickSideRightFast.kme";
	fileNameM = "KickSideRightFast.txt";	//for Matlab

	fin.open((ArchConfig::Instance().GetConfigPrefix()+"kme/"+fileName).c_str(),ios::out);
	finM.open((ArchConfig::Instance().GetConfigPrefix()+"kme/"+fileNameM).c_str(),ios::out);

	if ( fin.is_open() && finM.is_open() ){
		for(int i = 0; i < ACTION_TIME*XarAnalyzerFPS; i++){
			fin.write("play%", 5);
			for(int j = 0; j < JOINTS; j++){
				num = angleStore[i][j];
				fin << num;
				finM << num;
				fin.write("%", 1);
				finM.write(" ", 1);
			}
			frameTime = 1.0/XarAnalyzerFPS;
			fin << frameTime;
			fin << "\n";
			finM << "\n";
		}
		fin.close();
		finM.close();
		Logger::Instance().WriteMsg("XarAnalyzer", "CREATE FILE SIGOURA?", Logger::ExtraInfo);
	}
}


void XarAnalyzer::velocityWalk(double x, double y, double th, double f)
{
	static ptime lastcommand=microsec_clock::universal_time();
	if(microsec_clock::universal_time()-lastcommand<milliseconds(200))
		return ;
	wmot->set_command("setWalkTargetVelocity");
	lastcommand=microsec_clock::universal_time();

	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publishSignal(*wmot, "motion");
}
