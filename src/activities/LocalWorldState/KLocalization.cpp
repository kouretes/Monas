/*
 * KLocalization.cpp
 *
 *      Author: krambo
 *		Patched: eldr4d
 */
#include <time.h>
#include <boost/random.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <math.h>
#include "newran/newran.h"
#include "KLocalization.h"
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "architecture/archConfig.h"
#include <boost/math/distributions/normal.hpp>
#include "tools/logger.h"
#include "tools/toString.h"
using namespace boost;

KLocalization::KLocalization()
{
}

KLocalization::~KLocalization()
{

}

int KLocalization::Initialize()
{
	XMLConfig * config = NULL;
	string filename = ArchConfig::Instance().GetConfigPrefix() + "/Localizationconf.xml" ;
	config= new XMLConfig(filename);
	if (config->IsLoadedSuccessfully())
	{
		bool found = true;
		float temp;
		///Parameters
		found &= config->QueryElement("robustmean", robustmean);
		found &= config->QueryElement("partclsNum", partclsNum);
		found &= config->QueryElement("SpreadParticlesDeviation", SpreadParticlesDeviation);
		found &= config->QueryElement("rotation_deviation", rotation_deviation);
		found &= config->QueryElement("PercentParticlesSpread", PercentParticlesSpread);
		found &= config->QueryElement("RotationDeviationAfterFallInDeg", RotationDeviationAfterFallInDeg);
		found &= config->QueryElement("NumberOfParticlesSpreadAfterFall", NumberOfParticlesSpreadAfterFall);
		if (found)
		{
			Logger::Instance().WriteMsg("Localization", "All Localization parameters loaded successfully" , Logger::Info);
		} else
		{
			Logger::Instance().WriteMsg("Localization", "Cant Find an attribute in the xml config file " , Logger::Error);
		}
	} else
	{
		Logger::Instance().WriteMsg("Localization", "Cant Find xml config file " + filename , Logger::Error);
	}
	if(config)
		delete config;
	config = NULL;

	filename = ArchConfig::Instance().GetConfigPrefix() + "/Field.xml" ;
	config= new XMLConfig(filename);

	if (config->IsLoadedSuccessfully())
	{
		bool found = true;
		found &= config->QueryElement("CarpetMaxX", CarpetMaxX);
		found &= config->QueryElement("CarpetMinX", CarpetMinX);
		found &= config->QueryElement("CarpetMaxY", CarpetMaxY);
		found &= config->QueryElement("CarpetMinY", CarpetMinY);
		found &= config->QueryElement("FieldMaxX", FieldMaxX);
		found &= config->QueryElement("FieldMinX", FieldMinX);
		found &= config->QueryElement("FieldMaxY", FieldMaxY);
		found &= config->QueryElement("FieldMinY", FieldMinY);
		if (found)
		{
			Logger::Instance().WriteMsg("Localization", "All Field parameters loaded successfully", Logger::Info);
		} else
		{
			Logger::Instance().WriteMsg("Localization", "Cant Find an attribute in the Field xml config file ", Logger::Error);
		}

	}else
	{
		Logger::Instance().WriteMsg("Localization", "Cant Find Field xml config file " + filename , Logger::Error);
	}
	if(config)
		delete config;

	//Initialize particles
	SIRParticles.size = partclsNum;
	SIRParticles.x = new double[partclsNum];
	SIRParticles.y = new double[partclsNum];
	SIRParticles.phi = new double[partclsNum];
	SIRParticles.Weight = new double[partclsNum];
	max_weight_particle_index=0;
	double seed = (double) (time(NULL) % 100 / 100.0);
	Random::Set(seed);
    srand(time(0));

	// Loading features,
	LoadFeaturesXML(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml", KFeaturesmap);

	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	readRobotConf(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");
	initParticles();
	cout << "\033[22;32m All Features Loaded \033[0m " << endl;
	return 1;
}

void KLocalization::initParticles()
{
	Uniform X, Y, P;
	float length = FieldMaxX*2/3;
	unsigned int particlesUp = partclsNum/2;
	unsigned int particlesDown = partclsNum - particlesUp;
	//Initialize top Particles
	for (unsigned int i = 0; i < particlesUp; i++){
		SIRParticles.x[i] = X.Next() * length + FieldMinX + 0.5;
		SIRParticles.y[i] = FieldMaxY;
		SIRParticles.phi[i] = deg2rad(270);
		SIRParticles.Weight[i] = 1.0 / partclsNum;
	}
	//Initialize down Particles
	for (unsigned int i = particlesUp; i < partclsNum; i++){
		SIRParticles.x[i] = X.Next() * length + FieldMinX + 0.5;
		SIRParticles.y[i] = -FieldMaxY;
		SIRParticles.phi[i] = deg2rad(90);
		SIRParticles.Weight[i] = 1.0 / partclsNum;
	}

}

void KLocalization::setParticlesPoseUniformly()
{
	Uniform X, Y, P;
	float length = FieldMaxX*2/3;
	unsigned int resetParticles = 20;
	unsigned int particlesUp = partclsNum/2-resetParticles/2;
	unsigned int particlesDown = partclsNum - particlesUp;
	//Initialize top Particles
	for (unsigned int i = 0; i < resetParticles; i++){
		float x,y;
		if(playerNumber == 1){
			y = 0;
			x = -2.7;
		}else if(playerNumber == 2){
			y = 1.2;
			x = -2.4;
		}else if(playerNumber == 3){
			y = -1.2;
			x = -2.4;
		}else if(playerNumber == 4){
			y = -0.4;
			x = -2.4;
		}
		SIRParticles.x[i] = x;
		SIRParticles.y[i] = y;
		SIRParticles.phi[i] = deg2rad(0);
		SIRParticles.Weight[i] = 1.0 / partclsNum;
	}
	for (unsigned int i = resetParticles; i < particlesUp; i++){
		SIRParticles.x[i] = X.Next() * length + FieldMinX + 0.5;
		SIRParticles.y[i] = FieldMaxY;
		SIRParticles.phi[i] = deg2rad(270);
		SIRParticles.Weight[i] = 1.0 / partclsNum;
	}
	//Initialize down Particles
	for (unsigned int i = particlesUp; i < partclsNum; i++){
		SIRParticles.x[i] = X.Next() * length + FieldMinX + 0.5;
		SIRParticles.y[i] = -FieldMaxY;
		SIRParticles.phi[i] = deg2rad(90);
		SIRParticles.Weight[i] = 1.0 / partclsNum;
	}
}

void KLocalization::initializeParticles(int playerState,bool kickOff){
	if(playerState == PLAYER_PENALISED){
		setParticlesPoseUniformly();
	}else if(playerState == PLAYER_READY){
		float phi,x,y;
		if(playerNumber == 1){
			y = -FieldMaxY;
			x = -2.4;
			phi = deg2rad(90);
		}else if(playerNumber == 2){
			y = FieldMaxY;
			x = -2.4;
			phi = deg2rad(270);
		}else if(playerNumber == 3){
			y = -FieldMaxY;
			x = -1.2;
			phi = deg2rad(90);
		}else if(playerNumber == 4){
			y = FieldMaxY;
			x = -1.2;
			phi = deg2rad(270);
		}
		//Leave some particles to the current position in case of ready state after goal
		int percentageOfParticle = partclsNum*0.2;
		for (unsigned int i = percentageOfParticle; i < partclsNum; i++)
		{
			if(i == max_weight_particle_index)
				continue;
			SIRParticles.x[i] = x;
			SIRParticles.y[i] = y;
			SIRParticles.phi[i] = phi;
			SIRParticles.Weight[i] = 1.0 / partclsNum;
		}
	}else if(playerState == PLAYER_SET){
		for (unsigned int i = 0; i < partclsNum; i++)
		{
			SIRParticles.x[i] = initX[(kickOff)?0:1] + ((double)rand()/(double)RAND_MAX)*0.2 - 0.1;
			SIRParticles.y[i] = initY[(kickOff)?0:1] + ((double)rand()/(double)RAND_MAX)*0.2 - 0.1;
			SIRParticles.phi[i] = initPhi[(kickOff)?0:1];
			SIRParticles.Weight[i] = 1.0 / partclsNum;
		}
	}
}

//Sequential Importance Resampling
belief KLocalization::LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel>& Observations, vector<KObservationModel>& AmbiguousObservations)
{
	//SIR Filter
	//int index[partclsNum];
	bool weightsChanged = false;
	belief AgentPosition;

	//Predict - Move particles according the Prediction Model
	if(MotionModel.freshData)
		Predict(MotionModel);

	//Update - Using incoming observation
	if(Observations.size()>=1){
		Update(Observations, partclsNum);
		weightsChanged = true;
	}
	else if(AmbiguousObservations.size()==1){
		Update_Ambiguous(AmbiguousObservations, partclsNum);
		weightsChanged = true;
	}

	//Normalize Particles  Weight in order to Resample later
	//Find the index of the max weight in the process
	if(weightsChanged)
		normalize(SIRParticles.Weight,&max_weight_particle_index);


	//extract estimation
	AgentPosition.x =  SIRParticles.x[max_weight_particle_index];// maxprtcl.x;
	AgentPosition.y = SIRParticles.y[max_weight_particle_index];//maxprtcl.y;
	AgentPosition.theta = SIRParticles.phi[max_weight_particle_index];//maxprtcl.phi;

	//AgentPosition = RobustMean(SIRParticles, 10);

	//TODO only one value to determine confidance, Now its only distance confidence
	AgentPosition.confidence = 0.0;

	//Resample and propagate
	if(weightsChanged){
		rouletteResample();
		if(Observations.size()>=1){
			;//ForceBearing(SIRParticles,Observations);
		}
	}
	return AgentPosition;

}


void KLocalization::Predict(KMotionModel & MotionModel)
{
	double tmpDist, tmpDir, tmpRot;
	Normal X, Y, P;
	//Move the particles with noise
	for (unsigned int i = 0; i < partclsNum; i++)
	{
		tmpDist = MotionModel.Distance.val * (MotionModel.Distance.ratiomean + X.Next() * MotionModel.Distance.ratiodev);
		tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean + Y.Next() * MotionModel.Direction.Edev;
		tmpRot = MotionModel.Rotation.val * (MotionModel.Rotation.ratiomean + P.Next() * MotionModel.Rotation.ratiodev);

		SIRParticles.x[i] = SIRParticles.x[i] + cos(tmpDir + SIRParticles.phi[i]) * tmpDist;
		SIRParticles.y[i] = SIRParticles.y[i] + sin(tmpDir + SIRParticles.phi[i]) * tmpDist;
		SIRParticles.phi[i] = SIRParticles.phi[i] + tmpRot;
	}
}

double KLocalization::normpdf(double diff, double dev)
{
	boost::math::normal dist = boost::math::normal_distribution<double>(0, dev);
	return boost::math::pdf(dist, diff);
}

void KLocalization::spreadParticlesAfterFall(){
	//Normal X, Y, P;
	Random X, Y, P;

	int step = round((float) SIRParticles.size / NumberOfParticlesSpreadAfterFall);
	if (NumberOfParticlesSpreadAfterFall == 0)
		return;

	for (unsigned int i = 0; i < SIRParticles.size; i += step)
	{
		SIRParticles.x[i] += (X.Next() - 0.5) * SpreadParticlesDeviationAfterFall;
		SIRParticles.y[i] += (Y.Next() - 0.5) * SpreadParticlesDeviationAfterFall;
		SIRParticles.phi[i] += (P.Next() - 0.5) * deg2rad(RotationDeviationAfterFallInDeg);
	}
}

void KLocalization::Update(vector<KObservationModel> &Observation, int NumofParticles)
{

	//Function to update the weights of each particle regarding the ObservationDistance from an object and the direction

	double OverallWeightOwnField, OverallWeightEnemyField, OverallWeightTotal;
	double ParticlePointBearingAngle, ParticleBearing;
	double DistanceFromPastBelief, DirectionFromPastBelief;
	double R;
	
	for (int p = 0; p < NumofParticles; p++)
	{
		OverallWeightOwnField = 1.0;
		OverallWeightEnemyField = 1.0;
		OverallWeightTotal = 0;
		for (unsigned int i = 0; i < Observation.size(); i++)
		{
			R = DISTANCE_2(SIRParticles.x[p]-Observation[i].Feature.x,SIRParticles.y[p]-Observation[i].Feature.y);
			OverallWeightEnemyField *= normpdf((Observation[i].Distance.val - Observation[i].Distance.Emean) - R, Observation[i].Distance.Edev);
			
			ParticlePointBearingAngle = atan2(Observation[i].Feature.y - SIRParticles.y[p], Observation[i].Feature.x - SIRParticles.x[p]);
			ParticleBearing = anglediff2(Observation[i].Bearing.val, SIRParticles.phi[p]);
			OverallWeightEnemyField *= normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Observation[i].Bearing.Edev);

			//we take the symetric yellow now, so we put a - to the x and y of the observation
			R = DISTANCE_2(SIRParticles.x[p] - (-Observation[i].Feature.x),SIRParticles.y[p]-(-Observation[i].Feature.y));
			OverallWeightOwnField *= normpdf((Observation[i].Distance.val - Observation[i].Distance.Emean) - R, Observation[i].Distance.Edev);
			
			ParticlePointBearingAngle = atan2((-Observation[i].Feature.y) - SIRParticles.y[p], (-Observation[i].Feature.x) - SIRParticles.x[p]);
			ParticleBearing = anglediff2(Observation[i].Bearing.val, SIRParticles.phi[p]);
			OverallWeightOwnField *= normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Observation[i].Bearing.Edev);
		}
		OverallWeightTotal = (OverallWeightOwnField > OverallWeightEnemyField) ? OverallWeightOwnField : OverallWeightEnemyField;
		//totalWeight = OverallWeightYellowYellow + OverallWeight;

		OverallWeightTotal = (OverallWeightTotal<0.0001)?0.0001:OverallWeightTotal;
		SIRParticles.Weight[p] = OverallWeightTotal;
	}
}
void KLocalization::Update_Ambiguous(vector<KObservationModel> &Observation, int NumofParticles){
	//Function to update the weights of each particle regarding the ObservationDistance from an object and the direction
	double OverallWeight, ParticlePointBearingAngle, ParticleBearing, Deviation;
	double DistanceFromPastBelief, DirectionFromPastBelief;
	double AdditiveWeightTotal = 0,AdditiveOwnField=0,AdditiveEnemyField=0;
	double R;
	float xPosOfFeature 	= Observation[0].Feature.x;
	float yPosOfFeature 	= Observation[0].Feature.y;
	float obsDistEmean	= Observation[0].Distance.Emean;
	float obsDistEdev 	= Observation[0].Distance.Edev;
	float obsDistValue	= Observation[0].Distance.val;
	float obsBearingEdev 	= Observation[0].Bearing.Edev;
	float obsBearingValue 	= Observation[0].Bearing.val;
	//Find the best candiate for the landmark
	for (int p = 0; p < NumofParticles; p++)
	{
		AdditiveWeightTotal = 0;
		float oldWeight = SIRParticles.Weight[p];
		AdditiveEnemyField = 1;
		AdditiveOwnField = 1;

		//Enemy Left
		R = DISTANCE_2(SIRParticles.x[p]-xPosOfFeature,SIRParticles.y[p]-yPosOfFeature);
		AdditiveEnemyField *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(yPosOfFeature - SIRParticles.y[p], xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		AdditiveEnemyField *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveEnemyField;
		AdditiveEnemyField = 1;

		//Enemy Right
		R = DISTANCE_2(SIRParticles.x[p]-xPosOfFeature,SIRParticles.y[p]-(-yPosOfFeature));
		AdditiveEnemyField *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2((-yPosOfFeature) - SIRParticles.y[p], xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		AdditiveEnemyField *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveEnemyField;

		//Own Left
		R = DISTANCE_2(SIRParticles.x[p]- (-xPosOfFeature),SIRParticles.y[p]- (-yPosOfFeature));
		AdditiveOwnField *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-yPosOfFeature - SIRParticles.y[p], -xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		AdditiveOwnField *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveOwnField;
		AdditiveOwnField = 1;

		//Own Right
		R = DISTANCE_2(SIRParticles.x[p]- (-xPosOfFeature),SIRParticles.y[p]- (-(-yPosOfFeature)));
		AdditiveOwnField *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-(-yPosOfFeature) - SIRParticles.y[p], -xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		AdditiveOwnField *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveOwnField;

		AdditiveWeightTotal = (AdditiveWeightTotal<0.0001)?0.0001:AdditiveWeightTotal;
		SIRParticles.Weight[p] = AdditiveWeightTotal;
	}
}


void KLocalization::rouletteResample(){
	double r = rand()/((double)RAND_MAX);
	r = r/SIRParticles.size;
  	double cumSum = SIRParticles.Weight[0];
	double step = 1/((double)SIRParticles.size);
  	unsigned int i = 0;
	double tempX[SIRParticles.size];
	double tempY[SIRParticles.size];
	double tempPhi[SIRParticles.size];
   	for (unsigned int m=0; m < SIRParticles.size; m++)
   	{
       	//double u = r + 1.0*m/nsamples;
       	while (r > cumSum)
       	{
        	i++;
    		cumSum += SIRParticles.Weight[i];
    	}
		r += step;
		tempX[m] = SIRParticles.x[i];
		tempY[m] = SIRParticles.y[i];
		tempPhi[m] = SIRParticles.phi[i];
	}
	// lets propagate them
	for (i = 0; i < partclsNum; i++)
	{
		SIRParticles.phi[i] = tempPhi[i];
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
		SIRParticles.x[i] = tempX[i];
		SIRParticles.y[i] = tempY[i];
	}
}



///Normalizes and Returns ESS effective Sample size;
void KLocalization::normalize(double * Weights,unsigned int *max_weight_index)
{	
	double sum = 0;
	for (unsigned int i = 0; i < partclsNum; i++)
		sum += Weights[i];

	//normalize particles
	double max = -1;
	for (unsigned int i = 0; i < partclsNum; i++)
	{		
		Weights[i] = Weights[i] / sum;			
		if(Weights[i] > max){
			max = Weights[i];
			*max_weight_index = i;
		}
	}

}


float KLocalization::circular_mean_angle(float *angles, unsigned int size)
{
	float x = 0;
	float y = 0;
	for (unsigned int i = 0; i < size; i++)
	{
		x += cos(angles[i]);
		y += sin(angles[i]);
	}
	x = x / (float) size;
	y = y / (float) size;
	return atan2(y, x);
}

//Bad idea for 2 yellow goals
void KLocalization::ForceBearing(vector<KObservationModel> &Observation)
{
	//Calculate the bearing from each particle from each Observation
	//Force Bearing under some criteria
	float ParticlePointBearingAngle;
		unsigned int index;
	if (Observation.size() > 1)
	{
		float * angles = new float[Observation.size()];
		//for the real one
		do{
			index = rand() % SIRParticles.size;
		}while(index == max_weight_particle_index);
		for (unsigned int o = 0; o < Observation.size(); o++)
		{
			ParticlePointBearingAngle = atan2(Observation[o].Feature.y - SIRParticles.y[index], Observation[o].Feature.x - SIRParticles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Observation[o].Bearing.val);
		}
		SIRParticles.phi[index] = circular_mean_angle(angles, Observation.size());
		//for the symetric one
		do{
			index = rand() % SIRParticles.size;
		}while(index == max_weight_particle_index);
		for (unsigned int o = 0; o < Observation.size(); o++)
		{
			ParticlePointBearingAngle = atan2(-Observation[o].Feature.y - SIRParticles.y[index], -Observation[o].Feature.x - SIRParticles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Observation[o].Bearing.val);
		}
		SIRParticles.phi[index] = circular_mean_angle(angles, Observation.size());
		delete angles;
	} else if (Observation.size() == 1)
	{
		//for the real one
		do{
			index = rand() % SIRParticles.size;
		}while(index == max_weight_particle_index);
		ParticlePointBearingAngle = atan2(Observation[0].Feature.y - SIRParticles.y[index], Observation[0].Feature.x - SIRParticles.x[index]);
		SIRParticles.phi[index] = anglediff2(ParticlePointBearingAngle, Observation[0].Bearing.val);
		//for the symetric one
		do{
			index = rand() % SIRParticles.size;
		}while(index == max_weight_particle_index);
		ParticlePointBearingAngle = atan2(-Observation[0].Feature.y - SIRParticles.y[index], -Observation[0].Feature.x - SIRParticles.x[index]);
		SIRParticles.phi[index] = anglediff2(ParticlePointBearingAngle, Observation[0].Bearing.val);
	}
}

//Load xml files
int KLocalization::LoadFeaturesXML(string filename, map<string, feature>& KFeaturesmap)
{
	TiXmlDocument doc2(filename.c_str());
	bool loadOkay = doc2.LoadFile();
	if (!loadOkay)
	{
		Logger::Instance().WriteMsg("KLocalization", "Feature loading failed!!!!", Logger::Error);
		return -1;
	}
	TiXmlNode * Ftr;
	TiXmlElement * Attr;
	string ID;
	double x, y, w;
	feature temp;
	for (Ftr = doc2.FirstChild()->NextSibling(); Ftr != 0; Ftr = Ftr->NextSibling())
	{
		if(Ftr->ToComment() == NULL){
			Attr = Ftr->ToElement();
			Attr->Attribute("x", &x);
			Attr->Attribute("y", &y);
			Attr->Attribute("weight", &w);
			ID = Attr->Attribute("ID");
			temp.set(x, y, ID, w);
			KFeaturesmap[temp.id] = temp;
		}
	}
	return 0;
}

int KLocalization::readConfiguration(const std::string& file_name) {
	XMLConfig config(file_name);
	if (!config.QueryElement("player", playerNumber)){
		Logger::Instance().WriteMsg("KLocalization", "Configuration file has no player, setting to default value: " + _toString(playerNumber), Logger::Error);
		playerNumber = 1;
	}
	return 1;
}

bool KLocalization::readRobotConf(const std::string& file_name) {
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	Logger::Instance().WriteMsg("Localization",  " readRobotConfiguration "  , Logger::Info);
	for (int i = 0; i < 2; i++) {
		string kickoff = (i==0)?"KickOff":"noKickOff";	//KICKOFF==0, NOKICKOFF == 1
		bool found = false;
		teamPositions = config.QueryElement<std::string, float, std::string>(kickoff);

		if (teamPositions.size() != 0)
			robotPosition = config.QueryElement<std::string, float, std::string>("robot", &(teamPositions[0]));
		for (NodeCont::iterator it = robotPosition.begin(); it != robotPosition.end(); it++) {
			if (it->attrb["number"] == playerNumber) {
				initPhi[i] = 0.0;
				initX[i] = (it->attrb["posx"]);
				initY[i] = (it->attrb["posy"]);
				found = true;
				break;
			}else{
				initPhi[i] = 0.0;
				initX[i] = 0.0;
				initY[i] = 0.0;
			}
		}
	}
	return true;
}


