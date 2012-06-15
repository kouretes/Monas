/*
 * KLocalization.cpp
 *
 *  Created on: Jun 3, 2009
 *      Author: trs
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
#undef  DEBUG

using namespace boost;

KLocalization::KLocalization()
{
}

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
			}else{
				initPhi[i] = 0.0;
				initX[i] = 0.0;
				initY[i] = 0.0;
			}
		}
	}
	return true;
}

KLocalization::~KLocalization()
{
	// TODO Auto-generated destructor stub
}


void KLocalization::setBelief(double x, double y, double phi, double confidence)
{
	AgentPosition.x = x;
	AgentPosition.y = x;
	AgentPosition.theta = phi;
	AgentPosition.confidence = confidence;
}

void KLocalization::setParticlesPose(parts & Particles, double x, double y, double phi)
{
	for (unsigned int i = 0; i < Particles.size; i++)
	{
		Particles.x[i] = x;
		Particles.y[i] = y;
		Particles.phi[i] = phi;
		Particles.Weight[i] = 1.0 / Particles.size;
	}
}

void KLocalization::setParticlesPoseUniformly(parts & Particles)
{
	Uniform X, Y, P;
	float length = 2;//(FieldMaxX - FieldMinX)/4;
	//float width = (FieldMaxY - FieldMinY)/2;
	unsigned int particlesUp = partclsNum/2;
	unsigned int particlesDown = partclsNum - particlesUp;
	/*//all particles same point
	for (unsigned int i = 0; i < partclsNum; i++)
	{
		Particles.x[i] = -1000;
		Particles.y[i] = FieldMaxY;
		Particles.phi[i] = deg2rad(270);
		Particles.Weight[i] = 1.0 / partclsNum;
	}*/
	//Initialize top Particles
	for (unsigned int i = 0; i < particlesUp; i++)
	{
		Particles.x[i] = X.Next() * length + FieldMinX + 0.5;
		Particles.y[i] = FieldMaxY;
		Particles.phi[i] = deg2rad(270);
		Particles.Weight[i] = 1.0 / partclsNum;
	}
	//Initialize down Particles
	for (unsigned int i = particlesUp; i < partclsNum; i++)
	{
		Particles.x[i] = X.Next() * length + FieldMinX + 0.5;
		Particles.y[i] = -FieldMaxY;
		Particles.phi[i] = deg2rad(90);
		Particles.Weight[i] = 1.0 / partclsNum;
	}
}

void KLocalization::initializeParticles(parts & Particles,int playerNumber,bool kickOff){
	if(playerNumber == -1)
		setParticlesPoseUniformly(Particles);
	else{
		for (unsigned int i = 0; i < partclsNum; i++)
		{
			Particles.x[i] = initX[(kickOff)?0:1];
			Particles.y[i] = initY[(kickOff)?0:1];
			Particles.phi[i] = initPhi[(kickOff)?0:1];
			Particles.Weight[i] = 1.0 / partclsNum;
		}
	}
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
		found &= config->QueryElement("Beta", Beta);
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

	AgentPosition.x = 0;
	AgentPosition.y = 0;
	AgentPosition.theta = 0;
	AgentPosition.confidence = 100000;
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	readRobotConf(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");
	cout << "\033[22;32m All Features Loaded \033[0m " << endl;
	return 1;
}

//Function to find the best Belief using the average of the % of the "heavier" particles
belief KLocalization::RobustMean(parts & Particles, int PercenteOfParticles)
{
	belief RmeanAgentPosition;
	///ROBUST MEAN ... .
	unsigned int robustmean = round((double) Particles.size * ((double) PercenteOfParticles / 100.0));
	robustmean = (robustmean < 1) ? 1 : robustmean;
	priority_queue<partcl> particlesQueue;
	partcl temp;
	for (unsigned int i = 0; i < Particles.size; i++)
	{
		temp.x = Particles.x[i];
		temp.y = Particles.y[i];
		temp.phi = Particles.phi[i];
		temp.Weight = Particles.Weight[i];
		particlesQueue.push(temp);
	}
	float x = 0;
	float y = 0;
	double sumX = 0;
	double sumY = 0;
	for (unsigned int i = 0; i < robustmean; i++)
	{
		temp = particlesQueue.top();
		sumX += temp.x;
		sumY += temp.y;
		x += cos(temp.phi);
		y += sin(temp.phi);
		particlesQueue.pop();
	}
	RmeanAgentPosition.x = sumX / (double) robustmean;
	RmeanAgentPosition.y = sumY / (double) robustmean;
	if(x!=0)
		RmeanAgentPosition.theta = wrapTo0_2Pi(atan2(y/(double)robustmean,x/(double)robustmean));
	else
		RmeanAgentPosition.theta=0;

	return RmeanAgentPosition;
}

void KLocalization::Predict(parts & Particles, KMotionModel & MotionModel)
{
	double tmpDist, tmpDir, tmpRot;
	Normal X, Y, P;
	//Move the particles with noise
	for (unsigned int i = 0; i < partclsNum; i++)
	{
		tmpDist = MotionModel.Distance.val * (MotionModel.Distance.ratiomean + X.Next() * MotionModel.Distance.ratiodev);
		tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean + Y.Next() * MotionModel.Direction.Edev;
		tmpRot = MotionModel.Rotation.val * (MotionModel.Rotation.ratiomean + P.Next() * MotionModel.Rotation.ratiodev);

		Particles.x[i] = Particles.x[i] + cos(tmpDir + Particles.phi[i]) * tmpDist;
		Particles.y[i] = Particles.y[i] + sin(tmpDir + Particles.phi[i]) * tmpDist;
		Particles.phi[i] = Particles.phi[i] + tmpRot;
	}
}

double KLocalization::normpdf(double diff, double dev)
{
	boost::math::normal dist = boost::math::normal_distribution<double>(0, dev);
	return boost::math::pdf(dist, diff);
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



void KLocalization::spreadParticlesAfterFall(parts &Particles, double Deviation, double RotDeviation, int numOfParticles){
	//Normal X, Y, P;
	Random X, Y, P;

	int step = round((float) Particles.size / numOfParticles);
	if (numOfParticles == 0)
		return;

	for (unsigned int i = 0; i < Particles.size; i += step)
	{
		Particles.x[i] += (X.Next() - 0.5) * Deviation;
		Particles.y[i] += (Y.Next() - 0.5) * Deviation;
		Particles.phi[i] += (P.Next() - 0.5) * deg2rad(RotDeviation);
	}
}



void KLocalization::Update(parts & Particles, vector<KObservationModel> &Observation, KMotionModel & MotionModel, int NumofParticles)
{

	//	 Function to update the weights of each particle regarding the ObservationDistance
	//	 from an object and the direction

	double OverallWeight, totalWeight,ParticlePointBearingAngle, ParticleBearing, Deviation, OverallWeightYellowYellow,ParticlePointBearingAngleS, ParticleBearingS;
	double DistanceFromPastBelief, DirectionFromPastBelief;
	double R = 0,RS = 0;
	double Meanerror = 0;

	//float ratio = 1.0;
	for (int p = 0; p < NumofParticles; p++)
	{
		OverallWeight = 1.0;
		OverallWeightYellowYellow = 1.0;
		totalWeight = 0;
		for (unsigned int i = 0; i < Observation.size(); i++)
		{
			// Distance
			// R Distance the particle has from the LandMark
			R = DISTANCE_2(Particles.x[p]-Observation[i].Feature.x, Particles.y[p]-Observation[i].Feature.y);

			Meanerror = Observation[i].Distance.Emean;
			Deviation = Observation[i].Distance.Edev;
			OverallWeight = OverallWeight * normpdf((Observation[i].Distance.val - Meanerror) - R, Deviation);

			//Bearing
			ParticlePointBearingAngle = atan2(Observation[i].Feature.y - Particles.y[p], Observation[i].Feature.x - Particles.x[p]);
			ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
			Deviation = Observation[i].Bearing.Edev;
			OverallWeight = OverallWeight * normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Deviation);

			// Distance
			// R Distance the particle has from the LandMark
			// s is for symetry
			//we take the symetric yellow now, so we put a - to the x and y of the observation
			RS = DISTANCE_2(Particles.x[p] - (-Observation[i].Feature.x), Particles.y[p] - (-Observation[i].Feature.y));

			OverallWeightYellowYellow = OverallWeightYellowYellow * normpdf((Observation[i].Distance.val - Meanerror) - RS, Deviation);

			//Bearing
			ParticlePointBearingAngleS = atan2(-Observation[i].Feature.y - Particles.y[p], -Observation[i].Feature.x - Particles.x[p]);
			ParticleBearingS = anglediff2(ParticlePointBearingAngleS, Particles.phi[p]);
			OverallWeightYellowYellow = OverallWeightYellowYellow * normpdf(anglediff(Observation[i].Bearing.val, ParticleBearingS), Deviation);
		}
		totalWeight = (OverallWeightYellowYellow > OverallWeight) ? OverallWeightYellowYellow : OverallWeight;
		//totalWeight = OverallWeightYellowYellow + OverallWeight;

		totalWeight = (totalWeight<0.0001)?0.0001:totalWeight;
		Particles.Weight[p] = totalWeight;
	}
}
void KLocalization::Update_Ambiguous(parts & Particles, vector<KObservationModel> &Observation, int NumofParticles){
	//	 Function to update the weights of each particle regarding the ObservationDistance
	//	 from an object and the direction
	double OverallWeight, ParticlePointBearingAngle, ParticleBearing, Deviation;
	double DistanceFromPastBelief, DirectionFromPastBelief;
	double AdditiveWeightTotal = 0,AdditiveOwnField=0,AdditiveEnemyField=0;
	double R;
	float xPosOfFeature 	= Observation[0].Feature.x;
	float yPosOfFeature 	= Observation[0].Feature.x;
	float obsDistEmean		= Observation[0].Distance.Emean;
	float obsDistEdev 		= Observation[0].Distance.Edev;
	float obsDistValue		= Observation[0].Distance.val;
	float obsBearingEdev 	= Observation[0].Bearing.Edev;
	float obsBearingValue 	= Observation[0].Bearing.val;
	//Find the best candiate for the landmark
	for (int p = 0; p < NumofParticles; p++)
	{
		AdditiveWeightTotal = 0;//Particles.Weight[p]; Ama paraminei auto tote den allazei pote
		float oldWeight = Particles.Weight[p];
		AdditiveEnemyField = 1;
		AdditiveOwnField = 1;

		//Enemy Left
		R = DISTANCE_2(Particles.x[p]-xPosOfFeature,Particles.y[p]-yPosOfFeature);
		AdditiveEnemyField *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(yPosOfFeature - Particles.y[p], xPosOfFeature - Particles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
		AdditiveEnemyField *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveEnemyField;
		AdditiveEnemyField = 1;

		//Enemy Right
		R = DISTANCE_2(Particles.x[p]-xPosOfFeature,Particles.y[p]-(-yPosOfFeature));
		AdditiveEnemyField *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2((-yPosOfFeature) - Particles.y[p], xPosOfFeature - Particles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
		AdditiveEnemyField *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveEnemyField;

		//Own Left
		R = DISTANCE_2(Particles.x[p]- (-xPosOfFeature),Particles.y[p]- (-yPosOfFeature));
		AdditiveOwnField *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-yPosOfFeature - Particles.y[p], -xPosOfFeature - Particles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
		AdditiveOwnField *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveOwnField;
		AdditiveOwnField = 1;

		//Own Right
		R = DISTANCE_2(Particles.x[p]- (-xPosOfFeature),Particles.y[p]- (-(-yPosOfFeature)));
		AdditiveOwnField *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-(-yPosOfFeature) - Particles.y[p], -xPosOfFeature - Particles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
		AdditiveOwnField *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveOwnField;

		AdditiveWeightTotal = (AdditiveWeightTotal<0.0001)?0.0001:AdditiveWeightTotal;
		Particles.Weight[p] = AdditiveWeightTotal;//OverallWeight;
	}
}




void KLocalization::rouletteResample(parts & Particles){
	double r = rand()/((double)RAND_MAX);
	r = r/Particles.size;
  	double cumSum = Particles.Weight[0];
	double step = 1/((double)Particles.size);
  	unsigned int i = 0;
	double tempX[Particles.size];
	double tempY[Particles.size];
	double tempPhi[Particles.size];
   	for (unsigned int m=0; m < Particles.size; m++)
   	{
       	//double u = r + 1.0*m/nsamples;
       	while (r > cumSum)
       	{
        	i++;
    		cumSum += Particles.Weight[i];
    	}
		r += step;
		tempX[m] = Particles.x[i];
		tempY[m] = Particles.y[i];
		tempPhi[m] = Particles.phi[i];
	}
	// lets propagate them
	for (i = 0; i < partclsNum; i++)
	{
		Particles.phi[i] = tempPhi[i];
		Particles.Weight[i] = 1.0 / Particles.size;
		Particles.x[i] = tempX[i];
		Particles.y[i] = tempY[i];
	}
}



///Normalizes and Returns ESS effective Sample size;
double KLocalization::normalize(double * Weights,unsigned int *max_weight_index)
{	
	//Removed ESS calculation
	double sum = 0;
	//double cv = 0;

	for (unsigned int i = 0; i < partclsNum; i++)
		sum += Weights[i];

	if (sum == 0){
		Logger::Instance().WriteMsg("KLocalization", "Error All weights zero no normalize", Logger::Error);
		return -1;
	}
	//normalize particles and calculate ESS
	double max = -1;
	for (unsigned int i = 0; i < partclsNum; i++)
	{		
		Weights[i] = Weights[i] / sum;			
		if(Weights[i] > max){
			max = Weights[i];
			*max_weight_index = i;
		}
		//cv += pow((Weights[i] * partclsNum - 1), 2);
	}

	//cv = cv / (double) partclsNum;
	//return (float) 1.0f / (1.0f + cv);
	return 0.5;

}



//Bad idea for 2 yellow goals
void KLocalization::ForceBearing(parts & Particles, vector<KObservationModel> &Observation)
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
			index = rand() % Particles.size;
		}while(index == max_weight_particle_index);
		for (unsigned int o = 0; o < Observation.size(); o++)
		{
			ParticlePointBearingAngle = atan2(Observation[o].Feature.y - Particles.y[index], Observation[o].Feature.x - Particles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Observation[o].Bearing.val);
		}
		Particles.phi[index] = circular_mean_angle(angles, Observation.size());
		//for the symetric one
		do{
			index = rand() % Particles.size;
		}while(index == max_weight_particle_index);
		for (unsigned int o = 0; o < Observation.size(); o++)
		{
			ParticlePointBearingAngle = atan2(-Observation[o].Feature.y - Particles.y[index], -Observation[o].Feature.x - Particles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Observation[o].Bearing.val);
		}
		Particles.phi[index] = circular_mean_angle(angles, Observation.size());
		delete angles;
	} else if (Observation.size() == 1)
	{
		//for the real one
		do{
			index = rand() % Particles.size;
		}while(index == max_weight_particle_index);
		ParticlePointBearingAngle = atan2(Observation[0].Feature.y - Particles.y[index], Observation[0].Feature.x - Particles.x[index]);
		Particles.phi[index] = anglediff2(ParticlePointBearingAngle, Observation[0].Bearing.val);
		//for the symetric one
		do{
			index = rand() % Particles.size;
		}while(index == max_weight_particle_index);
		ParticlePointBearingAngle = atan2(-Observation[0].Feature.y - Particles.y[index], -Observation[0].Feature.x - Particles.x[index]);
		Particles.phi[index] = anglediff2(ParticlePointBearingAngle, Observation[0].Bearing.val);
	}
}

