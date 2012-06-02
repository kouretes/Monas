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
#include "Kxmlutils.h"
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "Kutils.h"
//#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"
#include <boost/math/distributions/normal.hpp>
#include "tools/logger.h"
#include "tools/toString.h"
#define riza2pi (sqrt(2.0 * M_PI))
//#undef  WEBOTS
#undef  DEBUG

// MAX(a, b) ((a) > (b) ? (a) : (b))
#ifdef WEBOTS
float VFov = 33.02;
float HFov = 44.14;
#else
float VFov = 34.8;
float HFov = 46.4;
#endif
//#define DEBUG
//#define DEBUGPredict
//#define DEBUGupdate
//#define COUT_ON
using namespace boost;

KLocalization::KLocalization()
{
	// TODO Auto-generated constructor stub
	//Initialize();
}

int KLocalization::LoadFeaturesXML(string filename, map<string, feature>& KFeaturesmap)
{
	TiXmlDocument doc2(filename.c_str());
	bool loadOkay = doc2.LoadFile();
	if (!loadOkay)
	{
		printf("Failed to load file \"%s\"\n", filename.c_str());
		return -1;
	}
	TiXmlNode * Ftr;
	TiXmlElement * Attr;
	string tmp = "";
	double x, y, w;
	int CntDistErrorDevParams, CntBearErrorDevParams;
	int CntDistErrorMeanParams, CntBearErrorMeanParams;
	double *DistErrorDevParams = NULL;
	double *BearignErrorDevParams = NULL;
	double *DistErrorMeanParams = NULL;
	double *BearignErrorMeanParams = NULL;
	string ID;
	feature temp;
	for (Ftr = doc2.FirstChild()->NextSibling(); Ftr != 0; Ftr = Ftr->NextSibling())
	{
		Attr = Ftr->ToElement();
		Attr->Attribute("x", &x);
		Attr->Attribute("y", &y);
		Attr->Attribute("weight", &w);
		Attr->Attribute("DistMeanParams", &CntDistErrorMeanParams);
		if (CntDistErrorMeanParams > 0)
		{
			DistErrorMeanParams = new double[CntDistErrorMeanParams];
			for (int p = 0; p < CntDistErrorMeanParams; p++)
			{
				tmp = "DistM";
				tmp += boost::lexical_cast<string>(p);
				Attr->Attribute(tmp.c_str(), DistErrorMeanParams + p);
			}
		}
		Attr->Attribute("DistDevParams", &CntDistErrorDevParams);
		if (CntDistErrorDevParams > 0)
		{
			DistErrorDevParams = new double[CntDistErrorDevParams];
			for (int p = 0; p < CntDistErrorDevParams; p++)
			{
				tmp = "DistD";
				tmp += boost::lexical_cast<string>(p);
				Attr->Attribute(tmp.c_str(), DistErrorDevParams + p);
			}
		}
		Attr->Attribute("BearMeanParams", &CntBearErrorMeanParams);
		if (CntBearErrorMeanParams > 0)
		{
			BearignErrorMeanParams = new double[CntBearErrorMeanParams];
			for (int p = 0; p < CntBearErrorMeanParams; p++)
			{
				tmp = "BearM";
				tmp += boost::lexical_cast<string>(p);
				Attr->Attribute(tmp.c_str(), BearignErrorMeanParams + p);
			}
		}
		Attr->Attribute("BearDevParams", &CntBearErrorDevParams);
		if (CntBearErrorDevParams > 0)
		{
			BearignErrorDevParams = new double[CntBearErrorDevParams];
			for (int p = 0; p < CntBearErrorDevParams; p++)
			{
				tmp = "BearD";
				tmp += boost::lexical_cast<string>(p);
				Attr->Attribute(tmp.c_str(), BearignErrorDevParams + p);
			}
		}
		ID = Attr->Attribute("ID");
		temp.set(x, y, ID, w, CntDistErrorDevParams, CntBearErrorDevParams, DistErrorDevParams, BearignErrorDevParams, CntDistErrorMeanParams, CntBearErrorMeanParams, DistErrorMeanParams, BearignErrorMeanParams);
		KFeaturesmap[temp.id] = temp;
		allfeatures.push_back(temp);
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
	Logger::Instance().WriteMsg("Behavior",  " readRobotConfiguration "  , Logger::Info);
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

int KLocalization::LoadMotionModelXML(string filename, vector<KMotionModel>& Motions, map<string, vector<KMotionModel> >& KMMmap)
{
	TiXmlDocument doc2(filename.c_str());

	bool loadOkay = doc2.LoadFile();
	if (!loadOkay)
	{
		printf("Failed to load file \"%s\"\n", filename.c_str());
		return -1;
	}
	TiXmlNode * BasicMotion;
	TiXmlNode * MotionStep;
	TiXmlNode * MotionData;
	BasicMotion = doc2.FirstChild();
	BasicMotion = BasicMotion->NextSibling();
	vector<KMotionModel> tempVMM;
	KMotionModel tempM;
	for (BasicMotion->NextSibling(); BasicMotion != 0; BasicMotion = BasicMotion->NextSibling())
	{
		tempM.type = string(BasicMotion->Value());
		if (KMMmap.count(tempM.type) == 0)
		{

			KMMmap[tempM.type] = tempVMM;
		} else
		{
			;
		}
		MotionStep = BasicMotion->FirstChild()->NextSibling(); //Overide TextComment
		while (MotionStep != 0)
		{
			string Steps = "";
			Steps += MotionStep->Value();
			//Loading Steps
			tempM.Steps = atoi(Steps.c_str() + 11);
			string name;
			for (MotionData = MotionStep->FirstChild(); MotionData != 0; MotionData = MotionData->NextSibling())
			{
				name = "";
				name += (MotionData->Value());
				if (name.compare("Distance") == 0)
					dump_attribs_to_randVar(MotionData->ToElement(), 1 + 2 + 1, tempM.Distance);
				else if (name.compare("Direction") == 0)
					dump_attribs_to_randVar(MotionData->ToElement(), 1 + 2 + 1, tempM.Direction);
				else if (name.compare("Rotation") == 0)
					dump_attribs_to_randVar(MotionData->ToElement(), 1 + 2 + 1, tempM.Rotation);
				else
					printf("\033[22;32mError Uknown Data value please check xml file\033[0m\n");
			}
			Motions.push_back(tempM);
			KMMmap[tempM.type].push_back(tempM);
			MotionStep = MotionStep->NextSibling();
		}
	}
	return 0;
}

KLocalization::~KLocalization()
{
	// TODO Auto-generated destructor stub
	//	delete[] Particles.x;
	//	delete[] Particles.y;
	//	delete[] Particles.phi;
	//	delete[] Particles.Weight;
	//
	//	delete[] AUXParticles.x;
	//	delete[] AUXParticles.y;
	//	delete[] AUXParticles.phi;
	//	delete[] AUXParticles.Weight;

}

double KLocalization::CalculateConfidence(parts & Particles, belief & blf)
{

	/// DistSume must Be near zero and Weight Sum before normalization must be
	/// Near 1 which means that all the particles are
	double DistSum = 0;
	double DistMean;
	for (unsigned int i = 0; i < Particles.size; i++)
	{
		Distances[i] = DISTANCE(blf.x, Particles.x[i], blf.y, Particles.y[i]);
		DistSum += Distances[i];
	}
	DistMean = DistSum / Particles.size;
	DistSum = 0;
	for (unsigned int i = 0; i < Particles.size; i++)
	{
		DistSum += pow((Distances[i] - DistMean), 2);
	}
	//Distance Deviation
	double DistConfidence = sqrt(DistSum / Particles.size);
	//1.0 / (0.1 + DistSum / 500.0); // result to be between 0.01...10
	double WeigtConfidence = Particles.WeightSum; 
	blf.confidence = DistConfidence;
	blf.weightconfidence = WeigtConfidence;
	return DistConfidence; //* (WeigtConfidence + 1.0) + WeigtConfidence; // Max degree == 20 :p
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
	float length = 2000;//(FieldMaxX - FieldMinX)/4;
	//float width = (FieldMaxY - FieldMinY)/2;
	unsigned int particlesUp = partclsNum/2;
	unsigned int particlesDown = partclsNum - particlesUp;
	//Initialize top Particles
	for (unsigned int i = 0; i < particlesUp; i++)
	{
		Particles.x[i] = X.Next() * length + FieldMinX + 500;
		Particles.y[i] = FieldMaxY;
		Particles.phi[i] = deg2rad(270);
		Particles.Weight[i] = 1.0 / partclsNum;
	}
	//Initialize down Particles
	for (unsigned int i = particlesUp; i < partclsNum; i++)
	{
		Particles.x[i] = X.Next() * length + FieldMinX + 500;
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
	beliefForGoalPosts[0] = 0;
	beliefForGoalPosts[1] = 0;
	beliefForGoalPosts[2] = 0;
	beliefForGoalPosts[3] = 0;
	timesOfContAmbig = 0;
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
		found &= config->QueryElement("Beta", temp);
		Beta = temp;
		Beta2 = new float();
		*Beta2 = temp;
		///Parameters
		found &= config->QueryElement("max_observation_distance", max_observation_distance);
		found &= config->QueryElement("max_observation_distance_deviation", max_observation_distance_deviation);
		found &= config->QueryElement("min_observation_distance_deviation", min_observation_distance_deviation);
		found &= config->QueryElement("max_observation_bearing_deviation", max_observation_bearing_deviation);
		found &= config->QueryElement("min_observation_bearing_deviation", min_observation_bearing_deviation);
		found &= config->QueryElement("robustmean", robustmean);
		found &= config->QueryElement("P_observe_NotVisible", P_observe_NotVisible);
		found &= config->QueryElement("P_Notobserve_NotVisible", P_Notobserve_NotVisible);
		found &= config->QueryElement("numofparticlesfromObservation", numofparticlesfromObservation);
		found &= config->QueryElement("partclsNum", partclsNum); // 0.2 meters deviation
		found &= config->QueryElement("SpreadParticlesDeviation", SpreadParticlesDeviation);
		found &= config->QueryElement("rotation_deviation", rotation_deviation); // % of particles be spreaded
		found &= config->QueryElement("PercentParticlesSpread", PercentParticlesSpread);	
		found &= config->QueryElement("SpreadParticlesDeviationAfterFall", SpreadParticlesDeviationAfterFall);
		found &= config->QueryElement("RotationDeviationAfterFallInDeg", RotationDeviationAfterFallInDeg);
		found &= config->QueryElement("NumberOfParticlesSpreadAfterFall", NumberOfParticlesSpreadAfterFall);
		found &= config->QueryElement("ForceBearingParticles", ForceBearingParticles);
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
		CarpetMaxX*=1000; //convert to mm
		found &= config->QueryElement("CarpetMinX", CarpetMinX);
		CarpetMinX*=1000;
		found &= config->QueryElement("CarpetMaxY", CarpetMaxY);
		CarpetMaxY*=1000;
		found &= config->QueryElement("CarpetMinY", CarpetMinY);
		CarpetMinY*=1000;
		found &= config->QueryElement("FieldMaxX", FieldMaxX);
		FieldMaxX*=1000;
		found &= config->QueryElement("FieldMinX", FieldMinX);
		FieldMinX*=1000;
		found &= config->QueryElement("FieldMaxY", FieldMaxY);
		FieldMaxY*=1000;
		found &= config->QueryElement("FieldMinY", FieldMinY);
		FieldMinY*=1000;


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

	P_observe_Visible = 1 - P_observe_NotVisible;
	P_Notobserve_Visible = 1 - P_Notobserve_NotVisible;

	halfrange = ((2 * 120 + HFov) / 2.0) * TO_RAD; //TODO real angle adding hfov of the camera

	depletions_counter = 0;
	///TODO somehow load motion files!

	//Initialize particles
	SIRParticles.size = partclsNum;
	SIRParticles.x = new double[partclsNum];
	SIRParticles.y = new double[partclsNum];
	SIRParticles.phi = new double[partclsNum];
	SIRParticles.Weight = new double[partclsNum];
	Distances = new double[partclsNum];
	//
	//	Random::Set(0.78/*(double) time(NULL)*/);
	double seed = (double) (time(NULL) % 100 / 100.0);
	Random::Set(seed);

	//Also Allocate Auxiliary particles

	AUXParticles.size = partclsNum;
	AUXParticles.x = new double[partclsNum];
	memcpy(AUXParticles.x, SIRParticles.x, partclsNum * sizeof(double));

	AUXParticles.y = new double[partclsNum]; // (double *) memcpy(Particles.y, new double[partclsNum], partclsNum * sizeof(double));
	AUXParticles.phi = new double[partclsNum]; // (double *) memcpy(Particles.phi, new double[partclsNum], partclsNum * sizeof(double));
	AUXParticles.Weight = new double[partclsNum]; //(double *) memcpy(Particles.Weight, new double[partclsNum], partclsNum * sizeof(double));
	memcpy(AUXParticles.y, SIRParticles.y, partclsNum * sizeof(double));
	memcpy(AUXParticles.phi, SIRParticles.phi, partclsNum * sizeof(double));
	memcpy(AUXParticles.Weight, SIRParticles.Weight, partclsNum * sizeof(double));

	LoadMotionModelXML(ArchConfig::Instance().GetConfigPrefix() + "/MotionModel.xml", KouretesMotions, KMMmap);
	// Loading features,
	LoadFeaturesXML(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml", KFeaturesmap);

	AgentPosition.x = 0;
	AgentPosition.y = 0;
	AgentPosition.theta = 0;
	AgentPosition.confidence = 100000;

	cout << "\033[22;32m All Features Loaded \033[0m " << endl;
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	readRobotConf(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");
	return 1;
}

KMotionModel * KLocalization::findBestMotionModel(int steps, string MotionType, vector<KMotionModel> & Motions, int *iterations)
{
	KMotionModel *BestMotionModel;
	KMotionModel *PositiveUnitMotionModel = NULL;
	KMotionModel *NegativeUnitMotionModel = NULL;
	KMotionModel tModel;
	vector<KMotionModel> *tVector;
	if (KMMmap.count(MotionType) == 0)
	{
		//cout << "Motion Type " << MotionType << " Unavailable" << endl;
		return NULL;
	} else
	{
		tVector = &KMMmap[MotionType];
		if (tVector->empty())
		{
			cerr << "NO MOTION MODEL OF TYPE" << MotionType << endl;
			return NULL;
		}
		BestMotionModel = &tVector->at(0);
		for (unsigned int i = 0; i < tVector->size(); i++)
		{
			if (tVector->at(i).Steps == 1)
			{
				PositiveUnitMotionModel = &tVector->at(1);
			}
			if (tVector->at(i).Steps == -1)
			{
				NegativeUnitMotionModel = &tVector->at(1);
			}
			if (abs((tVector->at(i).Steps - steps)) < abs(BestMotionModel->Steps - steps)){
				BestMotionModel = &tVector->at(i);
			}
		}	
		if ((BestMotionModel->Steps - steps) == 0)
		{
			;
		} else
		{
			cerr << "Exact Motion Model" << MotionType << " Steps" << steps << " NOT Found Using: " << endl;
			if (steps > 0)
			{
				BestMotionModel = PositiveUnitMotionModel;
			} else if (steps < 0)
			{
				BestMotionModel = NegativeUnitMotionModel;
			} else
			{
				return NULL;
			}
			if (steps == 0)
				*iterations = 1;
			else
				*iterations = abs(steps);
			//TODO fit Best Model
		}
		return BestMotionModel;
	}

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
		RmeanAgentPosition.theta = wrapTo0_2Pi(atan2(y/(double) robustmean,x/(double) (double) robustmean));
	else
		RmeanAgentPosition.theta=0;

	return RmeanAgentPosition;
}

void KLocalization::Predict(parts & Particles, KMotionModel & MotionModel)
{
	double tmpDist, tmpDir, tmpRot;
	Normal X, Y, P;
	unsigned int i;
	//Just for the 1st particle use only the mean motionmodel without noise
	if (MotionModel.type == "ratio")
	{
		tmpDist = MotionModel.Distance.val * (MotionModel.Distance.ratiomean);
		tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean;
		tmpRot = MotionModel.Rotation.val + MotionModel.Rotation.Emean;

	} else
	{
		tmpDist = MotionModel.Distance.val + MotionModel.Distance.Emean;
		tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean;
		tmpRot = MotionModel.Rotation.val + MotionModel.Rotation.Emean;
	}
	if (abs(tmpDir) > 400 || isnan(tmpDir))
		tmpDir = 0;
	Particles.x[0] = Particles.x[0] + cos(tmpDir + Particles.phi[0]) * tmpDist;
	Particles.y[0] = Particles.y[0] + sin(tmpDir + Particles.phi[0]) * tmpDist;
	Particles.phi[0] = Particles.phi[0] + tmpRot;
	//Move the others with noise
	for (i = 1; i < partclsNum; i++)
	{
		if (MotionModel.type == "ratio")
		{
			tmpDist = MotionModel.Distance.val * (MotionModel.Distance.ratiomean + X.Next() * MotionModel.Distance.ratiodev);
			tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean + Y.Next() * MotionModel.Direction.Edev;
			tmpRot = MotionModel.Rotation.val + MotionModel.Rotation.Emean + P.Next() * MotionModel.Rotation.Edev;

		} else
		{
			tmpDist = MotionModel.Distance.val + MotionModel.Distance.Emean + X.Next() * MotionModel.Distance.Edev;
			tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean + Y.Next() * MotionModel.Direction.Edev;
			tmpRot = MotionModel.Rotation.val + MotionModel.Rotation.Emean + P.Next() * MotionModel.Rotation.Edev;
		}
		if (abs(tmpDir) > 400 || isnan(tmpDir))
			tmpDir = 0;
		Particles.x[i] = Particles.x[i] + cos(tmpDir + Particles.phi[i]) * tmpDist;
		Particles.y[i] = Particles.y[i] + sin(tmpDir + Particles.phi[i]) * tmpDist;
		Particles.phi[i] = Particles.phi[i] + tmpRot;
	}
}

bool KLocalization::isVisible(feature & Feature, parts &Particles, int pos, double rangemaxleft, double rangemaxright)
{
	bool visible = 0;
	//%calculate the direction on the feature regarding the robot position
	double angleToFeature = wrapTo0_2Pi(atan2(Feature.y - Particles.y[pos], Feature.x - Particles.x[pos]));
	//Range values are both positive and negative left is positive right negative
	double phileft = wrapTo0_2Pi(Particles.phi[pos] + rangemaxleft);
	double phiright = wrapTo0_2Pi(Particles.phi[pos] + rangemaxright);
	double fullrange = rangemaxleft - rangemaxright;
	if (fullrange < M_PI)
	{
		if (anglediff2(phileft, angleToFeature) > 0 && anglediff2(angleToFeature, phiright) > 0)
			visible = 1;
	} else
	{
		double middle = phileft - fullrange / 2.0;

		if (anglediff2(phileft, angleToFeature) > 0 && anglediff2(angleToFeature, middle) > 0)
			visible = 1;
		if (anglediff2(middle, angleToFeature) > 0 && anglediff2(angleToFeature, phiright) > 0)
			visible = 1;
	}
	return visible;
}

bool KLocalization::isVisible(feature & Feature, partcl prtcl, double rangemaxleft, double rangemaxright)
{
	bool visible = 0;
	//%calculate the direction on the feature regarding the robot position
	double angleToFeature = wrapTo0_2Pi(atan2(Feature.y - prtcl.y, Feature.x - prtcl.x));

	double phileft = wrapTo0_2Pi(prtcl.phi + rangemaxleft);
	double phiright = wrapTo0_2Pi(prtcl.phi + rangemaxright);
	double fullrange = rangemaxleft - rangemaxright;

	if (fullrange < M_PI)
	{
		if (anglediff2(phileft, angleToFeature) > 0 && anglediff2(angleToFeature, phiright) > 0)
			visible = 1;
	} else
	{
		double middle = phileft - fullrange / 2.0;
		if (anglediff2(phileft, angleToFeature) > 0 && anglediff2(angleToFeature, middle) > 0)
			visible = 1;
		if (anglediff2(middle, angleToFeature) > 0 && anglediff2(angleToFeature, phiright) > 0)
			visible = 1;
	}
	return visible;

}

double KLocalization::normpdf(double diff, double dev)
{
	boost::math::normal dist = boost::math::normal_distribution<double>(0, dev);
	return boost::math::pdf(dist, diff);
}

//Bad idea with symetric field
int KLocalization::CircleIntersectionPossibleParticles(vector<KObservationModel> &Observation, parts &Particles, int numofparticlesfromObservation)
{
	//Just in case
	if (Observation.size() < 2 || Particles.size < 1 || numofparticlesfromObservation < 1 || (unsigned int) numofparticlesfromObservation > Particles.size)
		return -2;
	//Color, Circle, Value
	//0 Yellow // 1 Yellow :P //0 circle1 , 1 circle 2;
	float Circles[2][2][4]; //0 x, 1 y, 2 radius, 3 bearing
	cerr << "Ta pigenw omiomorfa\n";
	int halfParticlesFromObs = numofparticlesfromObservation/2;
	int cy, cb; //counter for yellow and blue goalpost
	cy = 0;
	cb = 0;
	for (unsigned int i = 0; i < Observation.size(); i++)
	{
		if (Observation[i].Feature.id[0] == 'Y' && cy < 2)
		{
			Circles[0][cy][0] = Observation[i].Feature.x;
			Circles[0][cy][1] = Observation[i].Feature.y;
			Circles[0][cy][2] = Observation[i].Distance.val;
			Circles[0][cy][3] = Observation[i].Bearing.val;

			Circles[1][cy][0] = -Observation[i].Feature.x;
			Circles[1][cy][1] = -Observation[i].Feature.y;
			Circles[1][cy][2] = Observation[i].Distance.val;
			Circles[1][cy][3] = Observation[i].Bearing.val;
			cy++;
		}
	}

	int ci;
	//Calculate distance between centres of circle
	ci = 0; //Only yellow
	float d = DISTANCE(Circles[ci][0][0],Circles[ci][1][0],Circles[ci][0][1],Circles[ci][1][1]); //[MathsFunctions calcDistance:c1.centre end:c2.centre];
	float c1r = Circles[ci][0][2];
	float c2r = Circles[ci][1][2];
	float m = c1r + c2r;
	float n = c1r - c2r;

	if (n < 0)
		n = (n * -1);
	//No solns
	if (d > m)
		return 0;

	//Circle are contained within each other
	if (d < n)
		return 0;

	//Circles are the same
	if (d == 0 && c1r == c2r)
		return -1;

	//Solve for a
	float a = (c1r * c1r - c2r * c2r + d * d) / (2 * d);

	//Solve for h
	float h = sqrt(c1r * c1r - a * a);

	//Calculate point p, where the line through the circle intersection points crosses the line between the circle centers.
	partcl p;

	p.x = Circles[ci][0][0] + (a / d) * (Circles[ci][1][0] - Circles[ci][0][0]);
	p.y = Circles[ci][0][1] + (a / d) * (Circles[ci][1][1] - Circles[ci][0][1]);

	int nsol = 2;
	//1 soln , circles are touching
	if (d == c1r + c2r)
	{
		nsol = 1;
	} else
	{
		//2solns
		partcl p1;
		//partcl p2;

		p1.x = p.x + (h / d) * (Circles[ci][1][1] - Circles[ci][0][1]);
		p1.y = p.y - (h / d) * (Circles[ci][1][0] - Circles[ci][0][0]);
		if (fabs(p1.x) > 4000 || fabs(p1.y) > 2800) // TODO add this values to conf file
		{
			//Is the other point
			p.x = p.x - (h / d) * (Circles[ci][1][1] - Circles[ci][0][1]);
			p.y = p.y + (h / d) * (Circles[ci][1][0] - Circles[ci][0][0]);
		} else
			p = p1;
	}

	int index = (rand()) % Particles.size;

	for (int i = 0; i < halfParticlesFromObs; i++)
	{
		index = (rand() + i) % Particles.size;

		Particles.x[index] = p.x;
		Particles.y[index] = p.y;
		//		Particles.phi[index] = temp.phi;
		float angles[2], ParticlePointBearingAngle;
		for (unsigned int o = 0; o < 2; o++)
		{
			ParticlePointBearingAngle = atan2(Circles[ci][o][1] - Particles.y[index], Circles[ci][o][0] - Particles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Circles[ci][o][3]);
		}

		Particles.phi[index] = circular_mean_angle(angles, Observation.size());

	}
	ci = 1; // change to second yellow 
	for (int i = halfParticlesFromObs; i < numofparticlesfromObservation; i++)
	{
		index = (rand() + i) % Particles.size;

		Particles.x[index] = -p.x;
		Particles.y[index] = -p.y;
		//		Particles.phi[index] = temp.phi;
		float angles[2], ParticlePointBearingAngle;
		for (unsigned int o = 0; o < 2; o++)
		{
			ParticlePointBearingAngle = atan2(Circles[ci][o][1] - Particles.y[index], Circles[ci][o][0] - Particles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Circles[ci][o][3]);
		}

		Particles.phi[index] = circular_mean_angle(angles, Observation.size());

	}
	
	return nsol;
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

void KLocalization::Update(parts & Particles, vector<KObservationModel> &Observation, KMotionModel & MotionModel, int NumofParticles, double rangemaxleft, double rangemaxright)
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
		if (!Observation.empty())// an landMark has been observed
		{ 
			for (unsigned int i = 0; i < Observation.size(); i++)
			{
#ifdef DISTANCE_WEIGHTING
				// Distance
				// R Distance the particle has from the LandMark
				R = DISTANCE(Particles.x[p],Observation[i].Feature.x,Particles.y[p],Observation[i].Feature.y);

				Meanerror = Observation[i].Distance.Emean;
				Deviation = Observation[i].Distance.Edev;
				OverallWeight = OverallWeight * normpdf((Observation[i].Distance.val - Meanerror) - R, Deviation);
#endif

#ifdef	BEARING_WEIGHTING
				//Bearing
				ParticlePointBearingAngle = atan2(Observation[i].Feature.y - Particles.y[p], Observation[i].Feature.x - Particles.x[p]);
				ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
				Deviation = Observation[i].Bearing.Edev; 
				OverallWeight = OverallWeight * normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Deviation);
#endif

#ifdef DISTANCE_WEIGHTING
				// Distance
				// R Distance the particle has from the LandMark
				// s is for symetry
				//we take the symetric yellow now, so we put a - to the x and y of the observation
				RS = DISTANCE(Particles.x[p],-Observation[i].Feature.x,Particles.y[p],-Observation[i].Feature.y);
				
				OverallWeightYellowYellow = OverallWeightYellowYellow * normpdf((Observation[i].Distance.val - Meanerror) - RS, Deviation);
#endif
#ifdef	BEARING_WEIGHTING
				//Bearing
				ParticlePointBearingAngleS = atan2(-Observation[i].Feature.y - Particles.y[p], -Observation[i].Feature.x - Particles.x[p]);
				ParticleBearingS = anglediff2(ParticlePointBearingAngleS, Particles.phi[p]);
				OverallWeightYellowYellow = OverallWeightYellowYellow * normpdf(anglediff(Observation[i].Bearing.val, ParticleBearingS), Deviation);
#endif
			}
			totalWeight = (OverallWeightYellowYellow > totalWeight) ? OverallWeightYellowYellow : totalWeight;
			totalWeight = (OverallWeight > totalWeight) ? OverallWeight : totalWeight;
		}
		else
		{
			totalWeight = Particles.Weight[p];
		}

		//USE past belief to weight particle
		//Here we need also the motion model average
		//#ifdef  PASTBELIEF
		//TODO FIX
		//Make an Observation from the past estimated position
		//DistanceFromPastBelief = DISTANCE(AgentPosition.x,Particles.x[p],AgentPosition.y,Particles.y[p]);
		//ParticlePointBearingAngle = atan2(Particles .y[p] - AgentPosition.y, Particles.x[p] - AgentPosition.x);
		//DirectionFromPastBelief = anglediff2(ParticlePointBearingAngle, AgentPosition.theta);
		//OverallWeight *= normpdf(DistanceFromPastBelief - MotionModel.Distance.val * MotionModel.Distance.ratiomean, abs(2500 / (0.5 + AgentPosition.confidence))+0.01);
		//OverallWeight *= normpdf(DirectionFromPastBelief - (MotionModel.Direction.val + MotionModel.Direction.Emean), abs(MotionModel.Direction.Edev + AgentPosition.confidence) * 3+0.000001);
		//OverallWeight *= normpdf(anglediff2(Particles.phi[p], AgentPosition.theta) - MotionModel.Rotation.val, abs(MotionModel.Rotation.Edev + AgentPosition.confidence)+0.00000001);
		//#endif
		/*if(p == 20){
			int num = 0;
			if(Observation.size()>0)
				num = Observation[0].Distance.val;
			else
				num = 0;
			cout << "Update OverallWeight = " << OverallWeight << " OverallYellow = " << OverallWeightYellowYellow << endl;
			cout << "X = " << Particles.x[p] << " Y = " << Particles.y[p] << " R = " << R << " RS = " << RS << " Real Dist = " << num << endl;
		}*/
		//set the weight
		Particles.Weight[p] = totalWeight;
	}
}
void KLocalization::Update_Ambigius(parts & Particles, vector<KObservationModel> &Observation, int NumofParticles){
	//	 Function to update the weights of each particle regarding the ObservationDistance
	//	 from an object and the direction
	double OverallWeight, ParticlePointBearingAngle, ParticleBearing, Deviation,ParticlePointBearingAngleS, ParticleBearingS;
	double DistanceFromPastBelief, DirectionFromPastBelief;
	double AdditiveWeightTotal = 0,AdditiveBlueField,AdditiveYellowField;
	double R,RS;
	double Meanerror = 0;
	float xPosOfFeature 	= Observation[0].Feature.x;
	float yPosOfFeature 	= Observation[0].Feature.x;
	float obsDistEmean		= Observation[0].Distance.Emean;
	float obsDistEdev 		= Observation[0].Distance.Edev;
	float obsDistValue		= Observation[0].Distance.val;
	float obsBearingEdev 	= Observation[0].Bearing.Edev;
	float obsBearingValue 	= Observation[0].Bearing.val;


	Meanerror = obsDistEmean;
	Deviation = obsDistEdev;
	Deviation = obsBearingEdev;
	//Find the best candiate for the landmark
	for (int p = 0; p < NumofParticles; p++)
	{
		AdditiveWeightTotal = Particles.Weight[p];
		int oldChoise = 0;
		float oldWeight = 0;
		for (int j = -1; j <= 1; j = j + 2)
		{
			AdditiveYellowField = 1;
			AdditiveBlueField = 1; 
			// Distance
			// R Distance the particle has from the LandMark
			R = DISTANCE(Particles.x[p],xPosOfFeature,Particles.y[p],yPosOfFeature*j);
			AdditiveYellowField *= normpdf((obsDistValue - Meanerror) - R, Deviation);
			//Bearing
			ParticlePointBearingAngle = atan2(yPosOfFeature*j - Particles.y[p], xPosOfFeature - Particles.x[p]);
			ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);

			AdditiveYellowField *= normpdf(anglediff(obsBearingValue, ParticleBearing), Deviation);

			// Distance
			// R Distance the particle has from the LandMark
			RS = DISTANCE(Particles.x[p],-xPosOfFeature,Particles.y[p],-yPosOfFeature*j);

			AdditiveBlueField *= normpdf((obsDistValue - Meanerror) - RS, Deviation);

			//Bearing
			ParticlePointBearingAngleS = atan2(-yPosOfFeature*j - Particles.y[p], -xPosOfFeature - Particles.x[p]);
			ParticleBearingS = anglediff2(ParticlePointBearingAngleS, Particles.phi[p]);
			AdditiveBlueField *= normpdf(anglediff(obsBearingValue, ParticleBearingS), Deviation);
			//Check The Best GoalPost	
			if(j==-1){
				if(AdditiveBlueField > AdditiveYellowField){
					oldChoise = 0;
					oldWeight = AdditiveBlueField;
				}else{
					oldChoise = 1;
					oldWeight = AdditiveYellowField;
				}
			}else{
				if(AdditiveBlueField > AdditiveYellowField){
					if(oldWeight < AdditiveBlueField){
						oldChoise = 2;
					}
				}else{
					if(oldWeight < AdditiveYellowField){
						oldChoise = 3;
					}else{
					}
				}
			}
		}
		beliefForGoalPosts[oldChoise]+=0.4;
	}
	int finalChoise = findMaxIndex(beliefForGoalPosts[0],beliefForGoalPosts[1],beliefForGoalPosts[2],beliefForGoalPosts[3]);
	int rightOrLeftGoalpost = (finalChoise-2 < 0)?-1:1;
	int rightOrLeftGoal = (finalChoise%2 ==0)? -1:1;
	float yPosition = rightOrLeftGoal*yPosOfFeature*rightOrLeftGoalpost;
	float xPosition = rightOrLeftGoal*xPosOfFeature;
	if(true ||timesOfContAmbig*NumofParticles/3<beliefForGoalPosts[finalChoise]){
		for (int p = 0; p < NumofParticles; p++)
		{
			//AdditiveWeightTotal = 0;
			AdditiveWeightTotal = Particles.Weight[p];
			// Distance
			// R Distance the particle has from the LandMark
			R = DISTANCE(Particles.x[p],xPosition,Particles.y[p],yPosition);

			AdditiveWeightTotal += normpdf((obsDistValue - Meanerror) - R, Deviation);
			ParticlePointBearingAngle = atan2(yPosOfFeature - Particles.y[p], xPosOfFeature - Particles.x[p]);
			ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
			AdditiveWeightTotal *= normpdf(anglediff(obsBearingValue, ParticleBearing), Deviation);

			OverallWeight = Particles.Weight[p]*0.6 + AdditiveWeightTotal*0.4;
			Particles.Weight[p] = AdditiveWeightTotal;//OverallWeight;
		}
	}
}


void KLocalization::Propagate(parts & Particles, int *Index)
{
	if (Index <= 0 || !Particles.size > 0)
	{
		if (Index <= 0)
			cerr << "Cant propagate nothing check the index " << endl;
		if (!Particles.size > 0)
			cerr << "Cant propagate nothing and particles existance" << endl;
		return;
	}
	double tempX[partclsNum];
	double tempY[partclsNum];
	double tempPhi[partclsNum];

	// Make a copy of the particles
	for (unsigned int i = 0; i < partclsNum; i++)
	{
		tempX[i] = Particles.x[i];
		tempY[i] = Particles.y[i];
		tempPhi[i] = Particles.phi[i];

	}

	// lets propagate them
	for (unsigned int i = 0; i < partclsNum; i++)
	{
		Particles.phi[i] = tempPhi[Index[i]];
		Particles.Weight[i] = 1.0 / partclsNum;
		Particles.x[i] = tempX[Index[i]];
		Particles.y[i] = tempY[Index[i]];
	}
}

double * KLocalization::CumSum(double * Table, int size)
{
	if (Table != NULL)
	{
		double * cumsum = new double[size];
		cumsum[0] = 0;
		for (int i = 0; i < size; i++)
		{
			if (i == 0)
				cumsum[0] = Table[0];
			else
				cumsum[i] = cumsum[i - 1] + Table[i];
		}

		return cumsum;
	} else
	{
		return NULL;

	}
}

double * KLocalization::FlipCumProd(double * Table, int size)
{
	if (Table != NULL)
	{
		double * Cumprod = new double[size];
		double prod = 1.0;

		for (int i = 0; i < size; i++)
		{
			prod *= Table[i];
			Cumprod[size - 1 - i] = prod; ///saving in reverse order
		}
		return Cumprod;

	} else
		return NULL;
}

int * KLocalization::ResampleSWR(parts & Particles, int *Index)
{
	double * Q = CumSum(Particles.Weight, partclsNum);
	int N = partclsNum;
	double* t = new double[N + 1];
Uniform 	U;
	for (int i = 0; i < N; i++)
	{
		t[i] = U.Next();
		Index[i] = i;
	}
	t[N] = U.Next();
	// Sort
	sort(t, t + N + 1);


	t[N] = 1;
	int i = 0;
	int j = 0;

	while (i < N && j < N)
	{
		if (t[i] < Q[j])
		{
			Index[i] = j;
			i++;
		} else
		{
			j++;
		}
	}
	// Index represents the particles which are going to be propagated

	delete[] Q;
	delete[] t;

	return Index;
}

//AUTHORS  : Arnaud Doucet and Nando de Freitas - for the acknowledgement.
int * KLocalization::multinomialR(parts & Particles, int *Index)
{ // (inIndex,q);
	double * cumDist = CumSum(Particles.Weight, Particles.size);
	int N = partclsNum; //Number of particles
	double t[N];
	double N_babies[N];

	// MULTINOMIAL SAMPLING
	//generate N ordered random variables uniformly distributed in [0,1]
	Uniform U;
	for (int i = 0; i < N; i++)
	{
		t[i] = pow(U.Next(), 1.0 / double(N - i));
		Index[i] = i;
		N_babies[i] = 0;
	}
	double * u = FlipCumProd(t, N);

	int j = 0;

	for (int i = 0; i < N; i++)
	{
		while (u[i] > cumDist[j])
			j++;
		N_babies[j] += 1;
	}

	//Copy Resampled Trajectories
	int idx = 0;
	for (int i = 0; i < N; i++)
	{
		if (N_babies[i] > 0)
			for (j = idx; j < (idx + N_babies[i]); j++)
			{ ///TODO check that 2
				Index[j] = i;
			}
		idx = idx + N_babies[i];
	}

	delete[] cumDist;
	delete[] u;

	return Index;
}
//Only on Normalized particles!
float KLocalization::ESS(parts &Particles)
{
	unsigned int size = Particles.size;
	double * Weights = Particles.Weight;
	double cv;
	cv = 0;
	for (unsigned int i = 0; i < size; i++)
	{
		cv += pow((Weights[i] * size - 1), 2);
	}
	cv = cv / (double) (size);
	//ESS
	return (float) 1.0f / (1.0f + cv);

}

void KLocalization::Resample(parts & Particles, int *Index, int param)
{
	//ResampleSWR(Particles, Index);

	multinomialR(Particles, Index);
}

///Normalizes and Returns ESS effective Sample size;
double KLocalization::normalize(double * Weights, int size)
{

	if (size < 1 || Weights == NULL)
	{
		cout << "\033[22;31m Size < 1  || Weights  == NULL  There are no particles ... to normalize \033[01;32m" << endl;
		return -2;
	}
	double sum = 0;
	double cv = 0;

	for (int i = 0; i < size; i++)
	{
		sum += Weights[i];
	}
	if (sum == 0)
	{
		cout << "\033[22;31m Error All weights zero no normalize \033[01;32m" << endl;
		return -1;
	}
	//normalize particles and calculate ESS
	for (int i = 0; i < size; i++)
	{
		Weights[i] = Weights[i] / sum;
		cv += pow((Weights[i] * size - 1), 2);
	}

	cv = cv / (double) size;
	return (float) 1.0f / (1.0f + cv);

}

void KLocalization::SpreadParticlesCirc(parts & Particles, double Deviation, double rotation_deviation, int Percent)
{
	//Spread random choosen particles except the 1st
	//Normal X, Y, P;
	Uniform D, P;
	Normal R;
	Percent = (Percent > 100) ? 100 : Percent;
	Percent = (Percent <= 0) ? 0 : Percent;
	float numofrandom = ((float) (Particles.size-1) * ((float) Percent / 100.0));
	int step = round((float) (Particles.size-1) / numofrandom);
	if (numofrandom == 0)
		return;
	float tmpDist, tmpDir;
	int count = 0;
	for (unsigned int i = (rand() % step)+1; i < Particles.size; i += step)
	{
		tmpDist = D.Next() * Deviation;
		tmpDir = R.Next() * deg2rad(360);
		Particles.x[i] = Particles.x[i] + cos(tmpDir + Particles.phi[i]) * tmpDist;
		Particles.y[i] = Particles.y[i] + sin(tmpDir + Particles.phi[i]) * tmpDist;
		Particles.phi[i] = Particles.phi[i] + P.Next() * rotation_deviation;
		count++;
	}


}

void KLocalization::SpreadParticles(parts & Particles, double Deviation, double rotation_deviation, int Percent)
{
	//Normal X, Y, P;
	Random X, Y, P;
	Percent = (Percent > 100) ? 100 : Percent;
	Percent = (Percent <= 0) ? 0 : Percent;
	float numofrandom = ((float) Particles.size * ((float) Percent / 100.0));
	int step = round((float) Particles.size / numofrandom);
	if (numofrandom == 0)
		return;
	int count = 0;
	for (unsigned int i = 0; i < Particles.size; i += step)
	{
		count++;
		Particles.x[i] += (X.Next() - 0.5) * Deviation;
		Particles.y[i] += (Y.Next() - 0.5) * Deviation;
		Particles.phi[i] += (P.Next() - 0.5) * deg2rad(10);
	}
}

double KLocalization::CalcDistMean(feature afeature, double Distance)
{
	double ret = 0.0;
	int p;
	for (p = afeature.CntDistErrorMeanParams - 1; p >= 0; p--)
	{
		ret = ret * Distance + afeature.DistErrorMeanParams[p];
	}
	return ret;
}

double KLocalization::CalcDistDev(feature afeature, double Distance)
{
	double ret = 0.0;
	int p;
	for (p = afeature.CntDistErrorDevParams - 1; p >= 0; p--)
	{
		ret = ret * Distance + afeature.DistErrorDevParams[p];
	}
	return ret;
}

double KLocalization::CalcBearMean(feature afeature, double Distance)
{
	double ret = 0.0;
	int p;
	for (p = afeature.CntBearErrorMeanParams - 1; p >= 0; p--)
	{
		ret = ret * Distance + afeature.BearignErrorMeanParams[p];
	}

	return ret;
}

double KLocalization::CalcBearDev(feature afeature, double Distance)
{
	double ret = 0.0;
	int p;
	for (p = afeature.CntBearErrorDevParams - 1; p >= 0; p--)
	{
		ret = ret * Distance + afeature.BearignErrorDevParams[p];
	}
	return ret;
}

/*
//Bad idea for 2 yellow goals
void KLocalization::ForceBearing(parts & Particles, vector<KObservationModel> &Observation)
{
	//Calculate the bearing from each particle from each Observation
	//Force Bearing under some criteria

	float ParticlePointBearingAngle;
	if (Observation.size() > 1)
	{
		float * angles = new float[Observation.size()];for (unsigned int p = 0; p < Particles.size; p++)
		{
			for (unsigned int o = 0; o < Observation.size(); o++)
			{
				ParticlePointBearingAngle = atan2(Observation[o].Feature.y - Particles .y[p], Observation[o].Feature.x - Particles.x[p]);
				angles[o] = anglediff2(ParticlePointBearingAngle, Observation[o].Bearing.val);
			}
			Particles.phi[p] = circular_mean_angle(angles, Observation.size());
		}
		delete angles;
	} else if (Observation.size() == 1)
	{
		for (unsigned int p = 0; p < Particles.size; p++)
		{
			ParticlePointBearingAngle = atan2(Observation[0].Feature.y - Particles .y[p], Observation[0].Feature.x - Particles.x[p]);
			Particles.phi[p] = anglediff2(ParticlePointBearingAngle, Observation[0].Bearing.val);
		}
	}
}*/

