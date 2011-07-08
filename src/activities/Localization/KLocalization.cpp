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
#include "tools/XMLConfig.h"
#include <boost/math/distributions/normal.hpp>
#include "architecture/archConfig.h"
#include "tools/logger.h"
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
	if (loadOkay)
	{
		printf("\n%s:\n", filename.c_str());
		//dump_to_stdout(&doc2); // defined later in the tutorial
	} else
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
		printf("%s %f %f %f DP %d BP %d \n", ID.c_str(), x, y, w, CntDistErrorDevParams, CntBearErrorDevParams);

#ifdef DEBUGMXML

		for (int p = 0; p < CntDistErrorMeanParams; p++)
		{
			tmp = "DistM";
			tmp += boost::lexical_cast<string>(p);
			cout << tmp << ":" << DistErrorMeanParams[p] << "\t";
		}

		for (int p = 0; p < CntDistErrorMeanParams; p++)
		{
			tmp = "DistP";
			tmp += boost::lexical_cast<string>(p);
			cout << tmp << ":" << DistErrorDevParams[p] << "\t";
		}

		cout << endl;
		for (int p = 0; p < CntBearErrorDevParams; p++)
		{
			tmp = "BearD";
			tmp += boost::lexical_cast<string>(p);
			cout << tmp << ":" << BearignErrorDevParams[p] << "\t";
		}
		cout << endl;

#endif
		temp.set(x, y, ID, w, CntDistErrorDevParams, CntBearErrorDevParams, DistErrorDevParams, BearignErrorDevParams, CntDistErrorMeanParams, CntBearErrorMeanParams, DistErrorMeanParams, BearignErrorMeanParams);
		KFeaturesmap[temp.id] = temp;
		allfeatures.push_back(temp);
	}
	cout << "Size KFeaturesmap " << allfeatures.size() << endl;
	return 0;
}

int KLocalization::LoadMotionModelXML(string filename, vector<KMotionModel>& Motions, map<string, vector<KMotionModel> >& KMMmap)
{
	TiXmlDocument doc2(filename.c_str());

	bool loadOkay = doc2.LoadFile();
	if (loadOkay)
	{
		printf("\nLoadMotionModel loading %s:\n", filename.c_str());
		//dump_to_stdout(&doc2); // defined later in the tutorial
	} else
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
#ifdef DEBUGMXML
		printf("BasicMotion [%s] \n", BasicMotion->Value());
#endif
		tempM.type = string(BasicMotion->Value());
#ifdef DEBUGMXML
		printf("Motion Comment [%s]\n", BasicMotion->FirstChild()->Value());
#endif
		if (KMMmap.count(tempM.type) == 0)
		{
#ifdef DEBUGMXML
			cout << "\nEmpty map for type: \t" << tempM.type << " Adding to Map " << endl;
#endif

			KMMmap[tempM.type] = tempVMM;
		} else
		{
			;
#ifdef DEBUGMXML
			cout << "Exist mapping for type: " << tempM.type << endl;
#endif
		}
		MotionStep = BasicMotion->FirstChild()->NextSibling(); //Overide TextComment
		while (MotionStep != 0)
		{
#ifdef DEBUGMXML
			printf("Motion Steps [%s] ", MotionStep->Value());
#endif
			string Steps = "";
			Steps += MotionStep->Value();
			//Loading Steps
			tempM.Steps = atoi(Steps.c_str() + 11);
#ifdef DEBUGMXML
			cout << " STEPS " << tempM.Steps << endl;
#endif
			string name;
			for (MotionData = MotionStep->FirstChild(); MotionData != 0; MotionData = MotionData->NextSibling())
			{
#ifdef DEBUGMXML
				printf("Motion Data about [%s]\n", MotionData->Value());
#endif
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

			//	cout << "tempM. Distance.Emean:  " << tempM.Distance.Emean << " tempM.Distance.Edev " << tempM.Distance.Edev << endl;

			Motions.push_back(tempM);
			KMMmap[tempM.type].push_back(tempM);

			MotionStep = MotionStep->NextSibling();
		}
	}
	cout << "Motions Size Vector " << Motions.size() << endl;
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

	//double * Distances = new double(Particles.size);
	double DistSum = 0;
	double DistMean;
	//double WeightSum = 0;
	for (unsigned int i = 0; i < Particles.size; i++)
	{
		Distances[i] = DISTANCE(blf.x, Particles.x[i], blf.y, Particles.y[i]);
		DistSum += Distances[i];
		//WeightSum += Particles.Weight[i];
	}
	DistMean = DistSum / Particles.size;
	DistSum = 0;
	for (unsigned int i = 0; i < Particles.size; i++)
	{
		DistSum += pow((Distances[i] - DistMean), 2);
		//WeightSum += Particles.Weight[i];
	}
	//delete Distances;
	//Distance Deviation
	double DistConfidence = sqrt(DistSum / Particles.size);

	//WeightSum = WeightSum/ Particles.size;
	//Particles.WeightSum = WeightSum;

	//1.0 / (0.1 + DistSum / 500.0); // result to be between 0.01...10
	double WeigtConfidence = Particles.WeightSum; //
	blf.confidence = DistConfidence;
	blf.weightconfidence = WeigtConfidence;
	//cout << "\033[01;34m DistConfidence: " << setprecision(10) << DistConfidence << " WeigtConfidence: " << setprecision(10) << WeigtConfidence << "\033[0m " << endl;
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
		//cout << "\033[01;31m \n Setted particles Pose " <<Particles.x[i] << " "<< Particles.y[i] << " " << Particles.phi[i] << "\033[0m"<< endl;
	}
	//cout << "\033[01;31m \n Setted particles Pose " << x << " "<< y << " " << phi << "\033[0m"<< endl;
}

void KLocalization::setParticlesPoseUniformly(parts & Particles)
{
	Uniform X, Y, P;

	float length = (FieldMaxX - FieldMinX);
	float width = (FieldMaxY - FieldMinY);
	for (unsigned int i = 0; i < partclsNum; i++)
	{
		Particles.x[i] = X.Next() * length + FieldMinX;
		Particles.y[i] = Y.Next() * width + FieldMinY;
		Particles.phi[i] = P.Next() * deg2rad(360);
		Particles.Weight[i] = 1.0 / partclsNum;
#ifdef DEBUGf
		cout << "Particle[" << i << "] x: " << Particles.x[i] << " y: " << Particles.y[i] << " phi: " << Particles.phi[i] << " Weight " << Particles.Weight[i] << endl;
#endif
		//	sum += Particles.weight[i];
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
		found &= config->QueryElement("Beta", temp);
		Beta = temp;
		Beta2 = new float();
		*Beta2 = temp;
		//found &= config->QueryElement(" ",  );
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

		if (found)
		{
			//cout << "All Localization parameters loaded successfully" << endl;
			Logger::Instance().WriteMsg("Localization", "All Localization parameters loaded successfully" , Logger::Info);
		} else
		{
			Logger::Instance().WriteMsg("Localization", "Cant Find an attribute in the xml config file " , Logger::Error);
			//cerr << "Cant Find an attribute in the xml config file " << endl;
		}
		//		Beta = 0.7f;
		//		///Parameters
		//		max_observation_distance = 6500;
		//		max_observation_distance_deviation = 400 / 3.0;
		//		min_observation_distance_deviation = 80 / 3.0;
		//
		//		max_observation_bearing_deviation = 10 * TO_RAD; //0.5
		//		min_observation_bearing_deviation = 0.5 * TO_RAD;
		//
		//		robustmean = 10;
		//		P_observe_NotVisible = 0.3;
		//		P_Notobserve_NotVisible = 0.7;
		//		numofparticlesfromObservation = 4;
		//
		//		partclsNum = 200;
		//		SpreadParticlesDeviation = 150; // 0.2 meters deviation
		//		rotation_deviation = (10 * TO_RAD);
		//		PercentParticlesSpread = 20; // % of particles be spreaded
	} else
	{
		Logger::Instance().WriteMsg("Localization", "Cant Find xml config file " + filename , Logger::Error);
		cerr << "Cant Find xml config file " << filename << endl;
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
		cout << " CarpetMaxX " <<  CarpetMaxX << endl;

		found &= config->QueryElement("CarpetMinX", CarpetMinX);
		CarpetMinX*=1000;
		cout << " CarpetMinX " <<  CarpetMinX << endl;

		found &= config->QueryElement("CarpetMaxY", CarpetMaxY);
		CarpetMaxY*=1000;
		cout << " CarpetMaxY " <<  CarpetMaxY << endl;

		found &= config->QueryElement("CarpetMinY", CarpetMinY);
		CarpetMinY*=1000;
		cout << " CarpetMinY " <<  CarpetMinY << endl;


		found &= config->QueryElement("FieldMaxX", FieldMaxX);
		FieldMaxX*=1000;
		cout << " FieldMaxX " <<  FieldMaxX << endl;

		found &= config->QueryElement("FieldMinX", FieldMinX);
		FieldMinX*=1000;
		cout << " FieldMinX " <<  FieldMinX << endl;
		found &= config->QueryElement("FieldMaxY", FieldMaxY);
		FieldMaxY*=1000;
		cout << " FieldMaxY " <<  FieldMaxY << endl;
		found &= config->QueryElement("FieldMinY", FieldMinY);
		FieldMinY*=1000;
		cout << " FieldMinY " <<  FieldMinY << endl;

		if (found)
		{
			//cout << "All Localization parameters loaded successfully" << endl;
			Logger::Instance().WriteMsg("Localization", "All Field parameters loaded successfully", Logger::Info);
		} else
		{
			Logger::Instance().WriteMsg("Localization", "Cant Find an attribute in the Field xml config file ", Logger::Error);
			//cerr << "Cant Find an attribute in the xml config file " << endl;
		}

	}else
	{
		Logger::Instance().WriteMsg("Localization", "Cant Find Field xml config file " + filename , Logger::Error);
	}
	if(config)
	delete config;

	P_observe_Visible = 1 - P_observe_NotVisible;
	P_Notobserve_Visible = 1 - P_Notobserve_NotVisible;

	cout << "ParticlesNum: " << partclsNum << " SpreadParticlesDeviation: " << SpreadParticlesDeviation << " \n rotation_deviation: " << rotation_deviation
			<< " PercentParticlesSpread: " << PercentParticlesSpread << " Observation PArticles" << numofparticlesfromObservation << endl;

#ifdef WEBOTS
	halfrange = ((2 * /*30 90*/HFov) / 2.0) * TO_RAD; //TODO real angle adding hfov of the camera
#else
	halfrange = ((2 * 120 + HFov) / 2.0) * TO_RAD; //TODO real angle adding hfov of the camera
#endif

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
	cout << "Seed: " << seed << " Time " << time(NULL) % 100 << endl;
	Random::Set(seed);

	//cout << " After Set" << endl;

	//setParticlesPoseUniformly(SIRParticles);

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

	cout << "Particles Population Created Particles Num: " << endl;

	LoadMotionModelXML(ArchConfig::Instance().GetConfigPrefix() + "/MotionModel.xml", KouretesMotions, KMMmap);

	cout << "Motion Model Loaded Number of Motions:  " << KouretesMotions.size() << endl;

	// Loading features,
	LoadFeaturesXML(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml", KFeaturesmap);

	cout << " KFeaturesmap Loaded Number of Features:  " << KFeaturesmap.size() << endl;
	AgentPosition.x = 0;
	AgentPosition.y = 0;
	AgentPosition.theta = 0;
	AgentPosition.confidence = 100000;
#ifdef  DEBUG
	for (unsigned int i = 0; i < allfeatures.size(); i++)
	{
		cout << allfeatures[i].id << allfeatures[i].x << " " << allfeatures[i].y << " " << endl;
	}
#endif

	cout << "\033[22;32m All Features Loaded \033[0m " << endl;

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
		cout << "Motion Type " << MotionType << " Unavailable" << endl;
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
		cout << "Search for best Motion Model! " << endl;
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
			cout << "Examining " << tVector->at(i).type << " " << tVector->at(i).Steps << " ";
			cout << "Distance.Emean:  " << tVector->at(i).Distance.Emean << " Distance.Edev " << tVector->at(i).Distance.Edev << endl;

			if (abs((tVector->at(i).Steps - steps)) < abs(BestMotionModel->Steps - steps)){
			BestMotionModel = &tVector->at(i);
			cout << "BestMotionModel " << BestMotionModel->type << " BestMotionModel->Steps " << BestMotionModel->Steps << endl;
		}
		//	allfeatures.erase(0);
	}

		if ((BestMotionModel->Steps - steps) == 0)
		{
			cout << "Found Exact Motion Model" << endl;
			cout << BestMotionModel->type << " " << BestMotionModel->Steps << " " << endl;
			cout << "BestMotionModel->Distance.Emean:  " << BestMotionModel->Distance.Emean << " BestMotionModel->Distance.Edev " << BestMotionModel->Distance.Edev << endl;

			//sleep(2);
		} else
		{
			cerr << "Exact Motion Model" << MotionType << " Steps" << steps << " NOT Found Using: " << endl;

			cout << BestMotionModel->type << " " << BestMotionModel->Steps << " " << endl;
			//sleep(2);
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

			cout << "Creating Model from unit Model" << endl;
			if (steps == 0)
				*iterations = 1;
			else
				*iterations = abs(steps);

			//TODO fit Best Model

		}
		return BestMotionModel;
	}

}

//, rangemaxleft, rangemaxright
belief KLocalization::LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel> & Observations, double rangemaxleft, double rangemaxright)
{

	int iterations = 1;
	//	KMotionModel *MotionModelptr = findBestMotionModel(steps, MotionType, KouretesMotions, &iterations);
	//
	//	if (MotionModelptr == NULL) {
	//		cout << "Warning No maching Motion Model!!!!!!!!!" << endl;
	//	} else {
	//		KMotionModel MotionModel = *MotionModelptr;
	cout << "LocalizationStep MotionModel.Distance.Emean:  " << MotionModel.Distance.Emean << " MotionModel.Distance.Edev " << MotionModel.Distance.Edev << endl;
	//	}

	int index[partclsNum];
	//Simple initialization

	//SpreadParticles
	if (Observations.empty())
	{
		cout << "No observations ... spreading" << endl;
		SpreadParticles(SIRParticles, SpreadParticlesDeviation, rotation_deviation, PercentParticlesSpread);
	}

	//	if (Observations.size() > 1)
	//		ObservationParticles(Observations, SIRParticles, 6000, 4000, 200, rangemaxleft, rangemaxright);

#ifdef DEBUG
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << SIRParticles.Weight[i] << "  ";
#endif
	//SIR Filter
	cout << "\nPredict Iterations " << iterations << endl;

	//sleep(1);
	//Predict - Move particles according the Prediction Model
	//if (MotionModelptr != NULL) {
	for (int i = 0; i < iterations; i++)
		Predict(SIRParticles, MotionModel);
	//} else {
	//	SpreadParticles(SIRParticles, iterations * 100, 30 * TO_RAD, 100);
	//}
	//Update - Using incomming observation

	Update(SIRParticles, Observations, MotionModel, partclsNum, rangemaxleft, rangemaxright);

#ifdef DEBUG
	cout << "\nUnnormalized SIR particles " << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << SIRParticles.Weight[i] << "  ";
#endif

	//Normalize Particles  Weight in order to Resample later
	if (normalize(SIRParticles.Weight, partclsNum) < 0.001)
	{
		cerr << "\033[01;31m \nOups SIRParticles Population Dissapeared Maybe the Robot have changed position\033[0m" << endl;
		depletions_counter++;
		if (depletions_counter > 1)
		{
			SpreadParticles(SIRParticles, 100.0 * depletions_counter, 30 * TO_RAD, 50);
		}
		cout << "Depletion Counter " << depletions_counter << endl;
	} else
	{
		depletions_counter = 0;

		//		//Resample
		//		Resample(SIRParticles, index, 0);
		//		//Propagate Original Particles Using the index from Auxiliary one
		//		Propagate(SIRParticles, index);
		//		if (!Observations.empty())
		//			SpreadParticles(SIRParticles, SpreadParticlesDeviation, rotation_deviation, PercentParticlesSpread);
	}
#ifdef DEBUG
	cout << "\nNormalized SIR particles  " << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << SIRParticles.Weight[i];
	//Maybe Usefull for others
	memcpy(AUXParticles.x, SIRParticles.x, partclsNum * sizeof(double));
	memcpy(AUXParticles.y, SIRParticles.y, partclsNum * sizeof(double));
	memcpy(AUXParticles.phi, SIRParticles.phi, partclsNum * sizeof(double));
	memcpy(AUXParticles.Weight, SIRParticles.Weight, partclsNum * sizeof(double));

	cout << "\nAUX particles Before, SIR before Resample after memcpy" << endl;
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << AUXParticles.Weight[i];

#endif

	//extract estimation
	partcl maxprtcl;

	//Find Max Weight
	maxprtcl.Weight = SIRParticles.Weight[0];
	maxprtcl.x = SIRParticles.x[0];
	maxprtcl.y = SIRParticles.y[0];
	maxprtcl.phi = SIRParticles.phi[0];
	SIRParticles.WeightSum = SIRParticles.Weight[0];
	for (unsigned int i = 0; i < SIRParticles.size; i++)
	{
		//Particles_cx += SIRParticles.x[i];
		//Particles_cy += SIRParticles.y[i];

		if (SIRParticles.Weight[i] > maxprtcl.Weight)
		{
			maxprtcl.x = SIRParticles.x[i];
			maxprtcl.y = SIRParticles.y[i];
			maxprtcl.phi = SIRParticles.phi[i];
			maxprtcl.Weight = SIRParticles.Weight[i];
		}
	}

	//Find Center of particles population
	//	double Particles_cx = 0;
	//	double Particles_cy = 0;
	//
	//	Particles_cx = Particles_cx / SIRParticles.size;
	//	Particles_cy = Particles_cy / SIRParticles.size;

	AgentPosition.x = maxprtcl.x;
	AgentPosition.y = maxprtcl.y;
	AgentPosition.theta = maxprtcl.phi;

	cout << "Probable agents position " << AgentPosition.x << ", " << AgentPosition.y << " maxprtcl W: " << maxprtcl.Weight << endl;

	//AgentPosition = RobustMean(SIRParticles, 2);
	//Complete the SIR
	Resample(SIRParticles, index, 0);
	Propagate(SIRParticles, index);

	//TODO only one value to determine confidance, Now its only distance confidence
	AgentPosition.confidence = CalculateConfidence(SIRParticles, AgentPosition);

	return AgentPosition;
	//AgentPosition
}

//, rangemaxleft, rangemaxright
belief KLocalization::LocalizationStep(int steps, string MotionType, vector<KObservationModel> & Observations, double rangemaxleft, double rangemaxright)
{

	int iterations = 1;
	KMotionModel *MotionModelptr = findBestMotionModel(steps, MotionType, KouretesMotions, &iterations);

	KMotionModel MotionModel = *MotionModelptr;
	cout << "LocalizationStep MotionModel.Distance.Emean:  " << MotionModel.Distance.Emean << " MotionModel.Distance.Edev " << MotionModel.Distance.Edev << endl;

	int index[partclsNum];
	//Simple initialization
	cout << "Predict Iterations " << iterations << endl;

	//Initialize Aux particles from current particles information
	memcpy(AUXParticles.x, SIRParticles.x, partclsNum * sizeof(double));
	memcpy(AUXParticles.y, SIRParticles.y, partclsNum * sizeof(double));
	memcpy(AUXParticles.phi, SIRParticles.phi, partclsNum * sizeof(double));
	memcpy(AUXParticles.Weight, SIRParticles.Weight, partclsNum * sizeof(double));

	//SpreadParticles
	if (!Observations.empty())
		SpreadParticles(AUXParticles, SpreadParticlesDeviation, rotation_deviation, PercentParticlesSpread);

	//	if (Observations.size() > 1)
	//		ObservationParticles(Observations, SIRParticles, 6000, 4000, 200, rangemaxleft, rangemaxright);

#ifdef DEBUG
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << AUXParticles.Weight[i];
#endif
	//Auxiliary Filter

	//Predict - Move particles according the Prediction Model
	for (int i = 0; i < iterations; i++)
		Predict(AUXParticles, MotionModel);
	//Update - Using incomming observation
	Update(AUXParticles, Observations, *MotionModelptr, partclsNum, rangemaxleft, rangemaxright);

#ifdef DEBUG
	for (int i = 0; i < partclsNum / 10.0; i++)
	cout << AUXParticles.Weight[i];
#endif

	//Normalize Particles  Weight in order to Resample later
	if (normalize(AUXParticles.Weight, partclsNum) < 0.01)
	{
		cerr << " Oups AUXParticles Population Dissapeared Maybe the Robot have changed position" << endl;
		cout << " Oups AUXParticles Population Dissapeared Maybe the Robot have changed position" << endl;
		//setParticlesPoseUniformly(AUXParticles);
		depletions_counter++;
		if (depletions_counter > 1)
		{
			SpreadParticles(SIRParticles, 100.0 * depletions_counter, 30 * TO_RAD, 100);
		}
		cout << "Depletion Counter " << depletions_counter << endl;

		//		if (Observations.size() > 1)
		//			ObservationParticles(Observations, SIRParticles, 6000, 3000, 50, rangemaxleft, rangemaxright);

	} else
	{
		depletions_counter = 0;
#ifdef DEBUG
		for (int i = 0; i < partclsNum / 10.0; i++)
		cout << AUXParticles.Weight[i];
#endif

		//Resample
		Resample(AUXParticles, index, 0);
		//Propagate Original Particles Using the index from Auxiliary one
		Propagate(SIRParticles, index);
		if (!Observations.empty())
			SpreadParticles(SIRParticles, SpreadParticlesDeviation, rotation_deviation, PercentParticlesSpread);
	}
	//SIR
	//Predict - Move particles according the Prediction Model
	for (int i = 0; i < iterations; i++)
		Predict(SIRParticles, MotionModel);

	//Update - Using incoming observation
	Update(SIRParticles, Observations, *MotionModelptr, partclsNum, rangemaxleft, rangemaxright);

	//Normalize Particles  Weight in order to Resample later
	if ((SIRParticles.WeightSum = normalize(SIRParticles.Weight, partclsNum)) < 0.01)
	{
		cerr << " Oops SIRParticles Population Disappeared Maybe the Robot have changed position ...\nRespreading them over the field " << endl;
		cout << " Oops SIRParticles Population Disappeared Maybe the Robot have changed position ...\nRespreading them over the field" << endl;
		//setParticlesPoseUniformly(SIRParticles);
		depletions_counter++;
		if (depletions_counter > 1)
			SpreadParticles(SIRParticles, 100.0 * depletions_counter, 30 * TO_RAD, 100);

		cout << "Depletion Counter " << depletions_counter << endl;

		//		if (Observations.size() > 1)
		//			ObservationParticles(Observations, SIRParticles, 6000, 3000, 50, rangemaxleft, rangemaxright);

	} else
	{
		depletions_counter = 0;
	}

	//Initialize Aux particles from current particles information
	//Maybe Usefull for others
#ifdef DEBUG
	memcpy(SIRParticles.x, AUXParticles.x, partclsNum * sizeof(double));
	memcpy(SIRParticles.y, AUXParticles.y, partclsNum * sizeof(double));
	memcpy(SIRParticles.phi, AUXParticles.phi, partclsNum * sizeof(double));
	memcpy(SIRParticles.Weight, AUXParticles.Weight, partclsNum * sizeof(double));
#endif

	//extract estimation
	partcl temp;

	//Find Max Weight
	temp.Weight = SIRParticles.Weight[0];
	temp.x = SIRParticles.x[0];
	temp.y = SIRParticles.y[0];
	temp.phi = SIRParticles.phi[0];
	SIRParticles.WeightSum = SIRParticles.Weight[0];
	for (unsigned int i = 1; i < SIRParticles.size; i++)
	{
		//Particles_cx += SIRParticles.x[i];
		//Particles_cy += SIRParticles.y[i];

		if (SIRParticles.Weight[i] > temp.Weight)
		{
			temp.x = SIRParticles.x[i];
			temp.y = SIRParticles.y[i];
			temp.phi = SIRParticles.phi[i];
			temp.Weight = SIRParticles.Weight[i];
		}
	}

	//Find
	double Particles_cx = 0;
	double Particles_cy = 0;

	Particles_cx = Particles_cx / SIRParticles.size;
	Particles_cy = Particles_cy / SIRParticles.size;

	AgentPosition.x = temp.x;
	AgentPosition.y = temp.y;
	AgentPosition.theta = temp.phi;

	//AgentPosition = RobustMean(SIRParticles, 2);
	//Complete the SIR
	Resample(SIRParticles, index, 0);
	Propagate(SIRParticles, index);

	//TODO only one value to determine confidance, Now its only distance confidence
	AgentPosition.confidence = CalculateConfidence(SIRParticles, AgentPosition);

	return AgentPosition;
	//AgentPosition
}

//Function to find the best Belief using the average of the % of the "hevier" particles
belief KLocalization::RobustMean(parts & Particles, int PercenteOfParticles)
{
	belief RmeanAgentPosition;
	///ROBUST MEAN ... .
	unsigned int robustmean = round((double) Particles.size * ((double) PercenteOfParticles / 100.0));
	robustmean = (robustmean < 1) ? 1 : robustmean;
	//#ifdef DEBUG
	cout << "RobustMean Particles " << robustmean << endl;
	//#endif
	priority_queue<partcl> particlesQueue;
	partcl temp;
	for (unsigned int i = 0; i < Particles.size; i++)
	{
		temp.x = Particles.x[i];
		temp.y = Particles.y[i];
		temp.phi = Particles.phi[i];
		temp.Weight = Particles.Weight[i];
		if (particlesQueue.size() >= robustmean)
			particlesQueue.pop();
		particlesQueue.push(temp);
	}

	double sumX = 0;
	double sumY = 0;
	double sumTheta0toPI = 0; /// TODO !!!! hm check this angle
	int countTheta0toPI = 0;
	double sumThetaPIto2PI = 0;
	int countThetaPIto2PI = 0;

	double tempTheta;
	for (unsigned int i = 0; i < robustmean; i++)
	{
		temp = particlesQueue.top();
		sumX += temp.x;
		sumY += temp.y;
		if ((tempTheta = wrapTo0_2Pi(temp.phi)) > M_PI)
		{
			sumThetaPIto2PI += tempTheta;
			countThetaPIto2PI++;

		} else
		{
			sumTheta0toPI += tempTheta;
			countTheta0toPI++;
		}

		particlesQueue.pop();

	}
	cout << " SumX" << sumX << " SumY" << sumY << "sumTheta0toPI " << sumTheta0toPI << endl;
	RmeanAgentPosition.x = sumX / (double) robustmean;
	RmeanAgentPosition.y = sumY / (double) robustmean;

	int bothsemicycles = 0;
	if (countTheta0toPI > 0)
	{
		bothsemicycles++;

		sumTheta0toPI = wrapToPi((double) sumTheta0toPI / (double) countTheta0toPI);
	}
	if (countThetaPIto2PI > 0)
	{
		sumThetaPIto2PI = wrapToPi(sumThetaPIto2PI / (double) countThetaPIto2PI);
		bothsemicycles++;
	}

	RmeanAgentPosition.theta = wrapTo0_2Pi(sumTheta0toPI + sumThetaPIto2PI) / (double) bothsemicycles;

	return RmeanAgentPosition;
}

void KLocalization::Predict(parts & Particles, KMotionModel & MotionModel)
{
	double tmpDist, tmpDir, tmpRot;
	Normal X, Y, P;
	unsigned int i;
	//	cout << "MotionModel.Distance.val:  " << MotionModel.Distance.val << "Distance.Emean:  " << MotionModel.Distance.Emean << " Distance.Edev " << MotionModel.Distance.Edev
	//			<< endl;
	//	cout << "MotionModel.Direction.val:  " << MotionModel.Direction.val << "Direction.Emean:  " << MotionModel.Direction.Emean << " Direction.Edev " << MotionModel.Direction.Edev
	//			<< endl;

	for (i = 0; i < partclsNum; i++)
	{
#ifdef  DEBUGPredict
		cout << "BEFORE Particles .x[i]" << Particles .x[i] << "Particles .y[i]" << Particles .y[i] << "Particles .phi[i]" << Particles .phi[i] << endl;
#endif
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

#ifdef  DEBUGPredict
		cout << "tmpDist:  " << tmpDist << " tmpDir " << tmpDir << endl;

		cout << "tmpDist:  " << tmpDist << " tmpDir " << tmpDir << " tmpRot "<<tmpRot<< endl;
		cout << "AFTER  Particles .x[i]" << Particles .x[i] << "Particles .y[i]" << Particles .y[i] << "Particles .phi[i]" << Particles .phi[i] << endl;
		//Particles.Weight[i] = 1.0;
#endif
	}
#ifdef  DEBUG
	cout << "Prediction Done " << endl;
#endif
}

bool KLocalization::isVisible(feature & Feature, parts &Particles, int pos, double rangemaxleft, double rangemaxright)
{

	//	double anglesOfView[2] ;
	//	anglesOfView[0] =  wrapTo2Pi(prtcl.phi+ halfrange );
	//	anglesOfView[1] =  wrapTo2Pi(prtcl.phi- halfrange );
	//= (prtcl.phi - halfrange )]); //Callibrate the angles 0..2pi

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

	//if()
	/*%if the feature is inside the field of view then should be observable
	 %     if( anglediff(angleToFeature,anglesOfView(1))<=halfrange &&  anglediff(angleToFeature,anglesOfView(2))<=halfrange)
	 %         visible = 1;
	 %     end*/
	//if(anglediff2(Particles.phi[pos]-rangemaxleft <= angleToFeature <=
	//> Particles.phi[pos]+rangemaxright
	//if (anglediff(angleToFeature, Particles.phi[pos]) <= halfrange)
	//visible = 1;
	return visible;
}

bool KLocalization::isVisible(feature & Feature, partcl prtcl, double rangemaxleft, double rangemaxright)
{

	//	double anglesOfView[2] ;
	//	anglesOfView[0] =  wrapTo2Pi(prtcl.phi+ halfrange );
	//	anglesOfView[1] =  wrapTo2Pi(prtcl.phi- halfrange );
	//= (prtcl.phi - halfrange )]); //Callibrate the angles 0..2pi

	bool visible = 0;
	//%calculate the direction on the feature regarding the robot position
	double angleToFeature = wrapTo0_2Pi(atan2(Feature.y - prtcl.y, Feature.x - prtcl.x));

	/*%if the feature is inside the field of view then should be observable
	 %     if( anglediff(angleToFeature,anglesOfView(1))<=halfrange &&  anglediff(angleToFeature,anglesOfView(2))<=halfrange)
	 %         visible = 1;
	 %     end*/

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

	//if (anglediff(angleToFeature, prtcl.phi) <= halfrange)
	//visible = 1;

	return visible;

}

double KLocalization::normpdf(double diff, double dev)
{

	boost::math::normal dist = boost::math::normal_distribution<double>(0, dev);
	//cout << " diff " << diff << " dev " << dev << endl;
	return boost::math::pdf(dist, diff);
	//return (exp(-pow(diff, 2) / (2 * pow(dev, 2))) / (riza2pi * dev));
}
int KLocalization::CircleIntersectionPossibleParticles(vector<KObservationModel> &Observation, parts &Particles, int numofparticlesfromObservation)
{
	//Just in case
	if (Observation.size() < 2 || Particles.size < 1 || numofparticlesfromObservation < 1 || (unsigned int) numofparticlesfromObservation > Particles.size)
		return -2;
	//-(int) findIntersectionOfCircle: (Circle *)c1 circle:(Circle *)c2 sol1:(CGPoint *)sol1 sol2:(CGPoint *)sol2
	//Color, Circle, Value
	//0 Yellow // 1 Blue //0 circle1 , 1 circle 2;
	float Circles[2][2][4]; //0 x, 1 y, 2 radius, 3 bearing
	//	float Circle2[2][3];

	int cy, cb; //counter for yellow and blue goalpost
	cy = 0;
	cb = 0;
	for (unsigned int i = 0; i < Observation.size(); i++)
	{
		//cout << Observation[i].Feature.id[0] << endl;
		if (Observation[i].Feature.id[0] == 'Y' && cy < 2)
		{
			Circles[0][cy][0] = Observation[i].Feature.x;
			Circles[0][cy][1] = Observation[i].Feature.y;
			Circles[0][cy][2] = Observation[i].Distance.val;
			Circles[0][cy][3] = Observation[i].Bearing.val;
			//cout <<Circles[1][cb][2] << endl;
			cy++;
		} else if (Observation[i].Feature.id[0] == 'S' && cb < 2)
		{
			Circles[1][cb][0] = Observation[i].Feature.x;
			Circles[1][cb][1] = Observation[i].Feature.y;
			Circles[1][cb][2] = Observation[i].Distance.val;
			Circles[1][cb][3] = Observation[i].Bearing.val;
			//cout <<Circles[1][cb][2] << endl;
			cb++;

		}
	}

	int ci;
	//Calculate distance between centres of circle
	if (cb > cy)
		ci = 1; //blue
	else
		ci = 0; //Yellow

	float d = DISTANCE(Circles[ci][0][0],Circles[ci][1][0],Circles[ci][0][1],Circles[ci][1][1]); //[MathsFunctions calcDistance:c1.centre end:c2.centre];
	float c1r = Circles[ci][0][2];
	float c2r = Circles[ci][1][2];
	float m = c1r + c2r;
	float n = c1r - c2r;

	if (n < 0)
		n = (n * -1);
	//cout << " d " << d << " n "<< n << " m " << m << endl;
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
	Particles.x[index] = p.x;
	Particles.y[index] = p.y;

	for (int i = 0; i < numofparticlesfromObservation - 1; i++)
	{
		index = (rand() + i) % Particles.size;

		Particles.x[index] = p.x;
		Particles.y[index] = p.y;
		//		Particles.phi[index] = temp.phi;
		float angles[2], ParticlePointBearingAngle;
		for (unsigned int o = 0; o < 2; o++)
		{
			//cout << " ci "<< ci << " o " << o << " Circles[ci][o][4] " << Circles[ci][o][3] << endl;
			ParticlePointBearingAngle = atan2(Circles[ci][o][1] - Particles.y[index], Circles[ci][o][0] - Particles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Circles[ci][o][3]);
			//cout << "ParticlePointBearingAngle  " << ParticlePointBearingAngle << " Circles[ci][o][4] " << Circles[ci][o][3] << endl;
		}

		Particles.phi[index] = circular_mean_angle(angles, Observation.size());
		//cout << "index: " << index << " x: " << p.x << " y: " << p.y << " phi: " << Particles.phi[index] << endl;

	}

	return nsol;
}

int KLocalization::ObservationParticles(vector<KObservationModel> &Observation, parts &Particles, int Xdim, int Ydim, int resolution, double rangemaxleft, double rangemaxright)
{

	float Xstep = round((float) Xdim / (float) resolution);
	//float Yresolution = round((float) Ydim / (float) resolution);

	vector<partcl> QueueContainer;
	priority_queue<partcl> particlesQueue;

	partcl temp;
	float ratio = 0;
	double Meanerror = 0;
	//double MeanAngle = 0;
	// Scan a grid on the field
	double OverallWeight, MinOverallWeight, R, ParticlePointBearingAngle, ParticleBearing, Deviation;

	//cout << "Observations size " << Observation.size() << endl;

	double OverallWeight2;
	double max_weight = 0;
	MinOverallWeight = 0;
	for (float x = -Xdim / 2.0; x <= Xdim / 2; x = x + Xstep)
		for (float y = -Ydim / 2.0; y <= Ydim / 2; y = y + Xstep)
		{
			temp.x = x;
			temp.y = y;
			OverallWeight = 1.0;
			//cout << "Examining position x " << temp.x << " y: " << temp.y << endl;
			for (unsigned int i = 0; i < Observation.size(); i++)
			{

				if (isVisible(Observation[i].Feature, temp, rangemaxleft, rangemaxright))
					ratio = P_observe_Visible;
				else
					ratio = P_observe_NotVisible;
				// Distance
				// R Distance the particle has from the LandMark
				R = DISTANCE(x, Observation[i].Feature.x, y, Observation[i].Feature.y);
				//Deviation = max_observation_distance_deviation / max_observation_distance * Observation[i].Distance.val + min_observation_distance_deviation;
				Meanerror = CalcDistMean(Observation[i].Feature, R);
				Deviation = CalcDistDev(Observation[i].Feature, R);
				OverallWeight = OverallWeight * normpdf((Observation[i].Distance.val - Meanerror) - R, Deviation);

				OverallWeight2 = OverallWeight;
				//Try to find out the best bearing angle

				Deviation = CalcBearDev(Observation[i].Feature, R);
				for (double test_phi = 0; test_phi < 2.0 * M_PI; test_phi = test_phi + M_PI_4 / 2.0)
				{
					ParticlePointBearingAngle = atan2(Observation[i].Feature.y - temp.y, Observation[i].Feature.x - temp.x);
					ParticleBearing = anglediff2(ParticlePointBearingAngle, test_phi);

					//Deviation = max_observation_bearing_deviation / max_observation_distance * Observation[i].Distance .val + min_observation_bearing_deviation;
					OverallWeight2 = OverallWeight * normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Deviation);

					if (OverallWeight2 > max_weight)
					{
						max_weight = OverallWeight2;
						temp.phi = test_phi;
					}
				}
				OverallWeight = OverallWeight * ratio;

				//Add particles to the priority
				//So the top particle to have the lower weight

			}
			temp.Weight = 1 - OverallWeight;
			particlesQueue.push(temp);
			//cout << " Particle inserted " << endl;
			if (particlesQueue.size() > numofparticlesfromObservation)
				particlesQueue.pop();
		}

	//TODO more efficient-correct addition of particles!
	//Now put them in the particles
	//easy way
	//just replace random numofparticlesfromObservation particles
	int index;

	//cout << "Observation Generated Particles " << particlesQueue.size() << endl;

	for (int i = 0; i < numofparticlesfromObservation; i++)
	{
		index = (rand() + i) % Particles.size;
		temp = particlesQueue.top();
		//	Particles.x[index] = temp.x;
		//		Particles.y[index] = temp.y;
		//		Particles.phi[index] = temp.phi;
		//cout << "index: " << index << " x: " << temp.x << " y: " << temp.y << " phi: " << temp.phi << endl;
		particlesQueue.pop();
	}
	return 1;
}

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
}

float KLocalization::circular_mean_angle(float *angles, unsigned int size)
{
	//
	float x = 0;
	float y = 0;

	for (unsigned int i = 0; i < size; i++)
	{
		x += cos(angles[i]);
		y += sin(angles[i]);
		//cout << "Angle " << i << " value: " << angles[i] * TO_DEG << endl;
	}
	//	for angle in angles:
	//		x += math.cos(angle)
	//		y += math.sin(angle)
	//
	x = x / (float) size;
	y = y / (float) size;
	//
	//	meanx = x/len(angles)
	//	meany = y/len(angles)
	//
	//cout << "Mean Angle " << atan2(y, x) * TO_DEG << endl;
	return atan2(y, x);
	//	mean = math.atan2(meany, meanx)
	//	R = math.sqrt(meanx**2 + meany**2)
	//	print R,meanx,meany,len(angles)
	//	std = math.sqrt(2*(1-R))
	//	return mean, std
}

void KLocalization::Update(parts & Particles, vector<KObservationModel> &Observation, KMotionModel & MotionModel, int NumofParticles, double rangemaxleft, double rangemaxright)
{

	//
	//	 Function to update the weights of each particle regarding the ObservationDistance
	//	 from an object and the direction

	double OverallWeight, /*ObservationAngle, ObservationBearing,*/ParticlePointBearingAngle, ParticleBearing, Deviation;
	double DistanceFromPastBelief, DirectionFromPastBelief;
	double R;
	double Meanerror = 0;
	//double MeanAngle = 0;
#ifdef  DEBUG
	cout << "Num Of Observation: " << Observation.size() << endl;
#endif

	//float ratio = 1.0;
	for (int p = 0; p < NumofParticles; p++)
	{
		OverallWeight = Particles.Weight[p];
#ifdef DEBUGupdate
		cout << " ParticleWeightBefore: " << OverallWeight << endl;
#endif
		if (!Observation.empty())
		{ // an landMark has been observed
			OverallWeight = 1.0;
			//	cout << "Weighting prtl X" << Particles.x[p] << " Y: " << Particles.y[p] << " phi: " << Particles.phi[p] << endl;
			for (unsigned int i = 0; i < Observation.size(); i++)
			{
#ifdef DISTANCE_WEIGHTING
				// Distance
				// R Distance the particle has from the LandMark
				R = DISTANCE(Particles.x[p],Observation[i].Feature.x,Particles.y[p],Observation[i].Feature.y);
				//				Deviation = (max_observation_distance_deviation / max_observation_distance) * Observation[i].Distance.val + min_observation_distance_deviation;
				//				Deviation = (Deviation < min_observation_distance_deviation) ? min_observation_distance_deviation : Deviation;

				Meanerror = Observation[i].Distance.Emean; // CalcDistMean(Observation[i].Feature, R);
				Deviation = Observation[i].Distance.Edev; //CalcDistDev(Observation[i].Feature, R);

#ifdef DEBUGupdate
				cout << " Distance Deviation " << Deviation << " Meanerror " << Meanerror << endl;
#endif
				OverallWeight = OverallWeight * normpdf((Observation[i].Distance.val - Meanerror) - R, Deviation);
#endif
				//cout << " ParticleAfterDist: " << OverallWeight << endl;
				//					if(~isempty(ParticlesIn.phi))
				//	TODO 			 FIX ME
				//					ObservationAngle = atan2(LandMark(i).y - TrackPoint.y, LandMark(i).x - TrackPoint.x); // calculated 2nd time
				//					ObservationBearing = anglediff2(ObservationAngle, TrackPoint.a);
				//
				//				Deviation = (max_observation_bearing_deviation / max_observation_distance) * Observation[i].Distance .val + min_observation_bearing_deviation;
				//				Deviation = (Deviation < min_observation_bearing_deviation) ? min_observation_bearing_deviation : Deviation;
				//Meanerror = CalcBearMean(Observation[i].Feature, R);
				//MeanAngle = -anglediff(Observation[i].Bearing.val, Meanerror);

				//Bearing
#ifdef	BEARING_WEIGHTING
				ParticlePointBearingAngle = atan2(Observation[i].Feature.y - Particles.y[p], Observation[i].Feature.x - Particles.x[p]);
				ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
				Deviation = Observation[i].Bearing.Edev; //CalcBearDev(Observation[i].Feature, R);
#ifdef DEBUGupdate
						cout << " Bearing Deviation " << Deviation << " Meanerror " << Meanerror << endl;
#endif
				//cout << "Bearing Error " << anglediff2(Observation[i].Bearing.val, ParticleBearing) * 100 << " Deviation " << Deviation << endl;
				//Particles.phi[p] = ParticlePointBearingAngle  + Observation[0].Bearing.val;
				//Particles.phi[p] -= anglediff2(Observation[i].Bearing.val, ParticleBearing);
				//Particles.phi[p] = anglediff2(ParticlePointBearingAngle, Observation[i].Bearing.val);
				//cout << "Particle weight " << normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Deviation) << "Observation " << i << endl;
				OverallWeight = OverallWeight * normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Deviation);
				//cout << "Bearing  weight " <<
#endif
				//
#ifdef VISIBILITY_WEIGHTING
				if (isVisible(Observation[i].Feature, Particles, p, rangemaxleft, rangemaxright))
				ratio = P_observe_Visible;
				else
				ratio = P_observe_NotVisible;
				OverallWeight = OverallWeight * ratio;
#endif

				//cout << " R: " << R << " Ow: " << OverallWeight;
			}
			//
		}
#ifdef ALLVISIBILITY
		else
		{
			// Check if any particle sees LandMark
			//cout << "Observation Empty checking if the particles sees Landmark" << endl;
			OverallWeight = OverallWeight * P_Notobserve_NotVisible;
			for (unsigned int f = 0; f < allfeatures.size(); f++)
			{
				if (isVisible(allfeatures[f], Particles, p, rangemaxleft, rangemaxright))
				{
					OverallWeight = OverallWeight * P_Notobserve_Visible;
					//cout << "Feature is visible from particle " << p << endl;
					//break;
				} else
				{
					//cout << "no Feature visible from particle " << p << endl;
				}
			}
		}
#endif
		//USE past belief to weight particle
		//Here we need also the motion model average
#ifdef  PASTBELIEF
		//Make an Observation from the past estimated position
		DistanceFromPastBelief = DISTANCE(AgentPosition.x,Particles.x[p],AgentPosition.y,Particles.y[p]);
		ParticlePointBearingAngle = atan2(Particles .y[p] - AgentPosition.y, Particles.x[p] - AgentPosition.x);
		DirectionFromPastBelief = anglediff2(ParticlePointBearingAngle, AgentPosition.theta);
		//MotionModel.Direction
		OverallWeight *= normpdf(DistanceFromPastBelief - MotionModel.Distance.val * MotionModel.Distance.ratiomean, abs(2500 / (0.5 + AgentPosition.confidence))+0.01);
		OverallWeight *= normpdf(DirectionFromPastBelief - (MotionModel.Direction.val + MotionModel.Direction.Emean), abs(MotionModel.Direction.Edev + AgentPosition.confidence) * 3+0.000001);
		//OverallWeight *= normpdf(anglediff2(Particles.phi[p], AgentPosition.theta) - MotionModel.Rotation.val, abs(MotionModel.Rotation.Edev + AgentPosition.confidence)+0.00000001);
#ifdef  DEBUGupdate
		cout << " pastBelief Ow: " << OverallWeight << endl;
#endif
#endif
		//cout << endl;
		//set the weight
#ifdef  DEBUGupdate
		cout << " Ow: " << OverallWeight << endl;
		if (OverallWeight == 0)
		{
			cout << "\033[01;31m  Particle' s " << p << " Weight is Zero, a dead particle  \033[0m" << endl;
		}
#endif
		Particles.Weight[p] = OverallWeight;
	}
#ifdef  DEBUG

	cout << "Update Done " << endl;

#endif
}

void KLocalization::Update_Ambigius(parts & Particles, vector<KObservationModel> &Observation, int NumofParticles)
{

	//
	//	 Function to update the weights of each particle regarding the ObservationDistance
	//	 from an object and the direction

	double OverallWeight, /*ObservationAngle, ObservationBearing,*/ParticlePointBearingAngle, ParticleBearing, Deviation;
	double AdditiveWeight = 0;
	double DistanceFromPastBelief, DirectionFromPastBelief;
	double R;
	double Meanerror = 0;

#ifdef  DEBUG
	cout << "Num Of Ambugius Observations: " << Observation.size() << endl;
#endif

	for (int p = 0; p < NumofParticles; p++)
	{
		OverallWeight = Particles.Weight[p];
#ifdef DEBUGupdate
		cout << " ParticleWeightBefore: " << OverallWeight << endl;
#endif
		if (!Observation.empty())
		{ // an Ambigius landMark has been observed
			for (unsigned int i = 0; i < Observation.size(); i++)
			{
				for (int j = -1; j <= 1; j = j + 2)
				{
#ifdef DISTANCE_WEIGHTING
					// Distance
					// R Distance the particle has from the LandMark
					R = DISTANCE(Particles.x[p],Observation[i].Feature.x,Particles.y[p],Observation[i].Feature.y*j);

					Meanerror = Observation[i].Distance.Emean;
					Deviation = Observation[i].Distance.Edev;

#ifdef DEBUGupdate
					cout << " Distance Deviation " << Deviation << " Meanerror " << Meanerror << endl;
#endif
					AdditiveWeight += normpdf((Observation[i].Distance.val - Meanerror) - R, Deviation);
#endif

					//Bearing
#ifdef	BEARING_WEIGHTING
					ParticlePointBearingAngle = atan2(Observation[i].Feature.y - Particles.y[p] * j, Observation[i].Feature.x - Particles.x[p]);
					ParticleBearing = anglediff2(ParticlePointBearingAngle, Particles.phi[p]);
					Deviation = Observation[i].Bearing.Edev; //CalcBearDev(Observation[i].Feature, R);
#ifdef DEBUGupdate
							cout << " Bearing Deviation " << Deviation << " Meanerror " << Meanerror << endl;
#endif

					AdditiveWeight += normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Deviation);
#endif
				}
			}
		}
		OverallWeight *= AdditiveWeight;
		AdditiveWeight=0;
		//set the weight
#ifdef  DEBUGupdate
		cout << " Ow: " << OverallWeight << endl;
		if (OverallWeight == 0)
		{
			cout << "\033[01;31m  Particle' s " << p << " Weight is Zero, a dead particle  \033[0m" << endl;
		}
#endif
		Particles.Weight[p] = OverallWeight;
	}
#ifdef  DEBUG

	cout << "Update Done " << endl;

#endif
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
#ifdef  DEBUG
	cout << "Propagation Done " << endl;

#endif
}

double * KLocalization::CumSum(double * Table, int size)
{
	if (Table != NULL)
	{
#ifdef  DEBUG
		cout << " CumSum Table not Null  " << endl;
#endif
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

#ifdef  DEBUG
		cout << "\033[22;31m  CumSum Table IS Null \033[01;32m" << endl;
#endif
		return NULL;

	}
}

double * KLocalization::FlipCumProd(double * Table, int size)
{
	if (Table != NULL)
	{
#ifdef  DEBUG
		cout << "CumProd Table not Null" << endl;
#endif
		double * Cumprod = new double[size];
		double prod = 1.0;

		for (int i = 0; i < size; i++)
		{
			prod *= Table[i];
			Cumprod[size - 1 - i] = prod; ///saving in reverse order
		}

#ifdef  DEBUG
		cout << "CumProd OK" << endl;
#endif
		return Cumprod;

	} else
		return NULL;
}

int * KLocalization::ResampleSWR(parts & Particles, int *Index)
{
#ifdef  DEBUG
	cout << "ResampleSWR Started " << endl;
#endif
	//	for (int i = 0; i < partclsNum; i++)
	//		cout << "W[" << i << "]" << "=" << Particles.Weight[i];

	double * Q = CumSum(Particles.Weight, partclsNum);
	int N = partclsNum;
	double* t = new double[N + 1];
	//	for (int i = 0; i < N + 1; i++)
	//		t[i] = randfloat(1);
Uniform 	U;
	for (int i = 0; i < N; i++)
	{
		t[i] = U.Next(); //randfloat(1);
		Index[i] = i;
	}
	t[N] = U.Next();
	// Sort
	sort(t, t + N + 1);

#ifdef  DEBUG
	cout << "t Sorted " << endl;

#endif

	t[N] = 1;
	int i = 0;
	int j = 0;

	/*for (int k = 0; k < N - 1; k++) {
	 cout << "tk[" << k << "] " << t[k] << endl;
	 cout << "Qk[" << k << "] " << Q[k] << endl;
	 }
	 */
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

#ifdef  DEBUG
	cout << "ResampleSWR Done " << endl;

#endif
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
#ifdef  DEBUG
	cout << "Resample Multinomial Ended " << endl;
#endif

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
		cout << "\033[22;31m Error All weights zero no normalize \033[01;32m";
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
	//return sum / size; // old method
}

void KLocalization::SpreadParticlesCirc(parts & Particles, double Deviation, double rotation_deviation, int Percent)
{

	//Normal X, Y, P;
	Uniform D, P;
	Normal R;
	Percent = (Percent > 100) ? 100 : Percent;
	Percent = (Percent <= 0) ? 0 : Percent;

	float numofrandom = ((float) Particles.size * ((float) Percent / 100.0));

	int step = round((float) Particles.size / numofrandom);
	if (numofrandom == 0)
		return;

	//cout << "SpreadParticles Deviation " << Deviation << " Step (numofrandom): " << numofrandom << " " << ((double) Percent / 100.0) * (double) Particles.size << " % " << Percent
	//		<< " Step: " << step << endl;
	float tmpDist, tmpDir;
	int count = 0;

	for (unsigned int i = rand() % step; i < Particles.size; i += step)
	{
		tmpDist = D.Next() * Deviation;
		tmpDir = R.Next() * deg2rad(360);

		Particles.x[i] = Particles.x[i] + cos(tmpDir + Particles.phi[i]) * tmpDist;
		Particles.y[i] = Particles.y[i] + sin(tmpDir + Particles.phi[i]) * tmpDist;
		Particles.phi[i] = Particles.phi[i] + P.Next() * rotation_deviation;
		Particles.Weight[i] = 1.0;
		count++;
	}
	//cout << "Num of Particles Spreaded:  " << count << endl;

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

//	cout << "SpreadParticles Deviation " << Deviation << " Step (numofrandom): " << numofrandom << " " << ((double) Percent / 100.0) * (double) Particles.size << " % " << Percent
//			<< " Step: " << step << endl;

	int count = 0;
	for (unsigned int i = 0; i < Particles.size; i += step)
	{
		count++;
		Particles.x[i] += (X.Next() - 0.5) * Deviation;
		Particles.y[i] += (Y.Next() - 0.5) * Deviation;
		Particles.phi[i] = P.Next() * deg2rad(360);
		//wrapTo0_2Pi(Particles.phi[i] + (P.Next() - 0.5) * rotation_deviation);
		cout << " Particles.phi[" << i << "]   " << Particles.phi[i] << endl;
		/*Particles.x[i] += X.Next() * Deviation;
		 Particles.y[i] += Y.Next() * Deviation;
		 Particles.y[i] += P.Next() * rotation_deviation;*/
	}
	//			//	particlesId = randperm(N);
	//			//	particlesId = particlesId(1:numofrandom) ;
	//			//	particles = particlesIn;
	//			//
	//			//	particles.x(particlesId) = particlesIn.x(particlesId) +Deviation*(rand(numofrandom,1)-0.5);
	//			//	particles.y(particlesId) = particlesIn.y(particlesId) +Deviation*(rand(numofrandom,1)-0.5);
	//			//	particles.phi(particlesId) = particlesIn.phi(particlesId) +0.9*(rand(numofrandom,1)-0.5);
	//			//
	//			//	particles.Weight(particlesId) = 0.5;
	//		}
	//cout << "Num of Particles Spreaded:  " << count << endl;

}

double KLocalization::CalcDistMean(feature afeature, double Distance)
{
	double ret = 0.0;
	int p;

	for (p = afeature.CntDistErrorMeanParams - 1; p >= 0; p--)
	{
		ret = ret * Distance + afeature.DistErrorMeanParams[p];
	}
	//
	//	if ((p = afeature.CntDistErrorMeanParams) > 0)
	//		ret = afeature.DistErrorMeanParams[0];
	//	for (p = 1; p < afeature.CntDistErrorMeanParams; p++) {
	//		if (p == 1) {
	//			ret += afeature.DistErrorMeanParams[p] * Distance;
	//		} else {
	//			ret += afeature.DistErrorMeanParams[p] * pow(Distance, p);
	//		}
	//	}
	//cout << "CalcDistMean " << ret << endl;
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
	//	if ((p = afeature.CntDistErrorDevParams) > 0)
	//		ret = afeature.DistErrorDevParams[0];
	//	for (p = 1; p < afeature.CntDistErrorDevParams; p++) {
	//		if (p == 1) {
	//			ret += afeature.DistErrorDevParams[p] * Distance;
	//		} else {
	//			ret += afeature.DistErrorDevParams[p] * pow(Distance, p);
	//		}
	//	}

	//ecout << "CalcDistDev " << ret << endl;
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

	//	if ((p = afeature.CntBearErrorMeanParams) > -1)
	//		ret = afeature.BearignErrorMeanParams[0];
	//	for (p = 1; p < afeature.CntBearErrorMeanParams; p++) {
	//		if (p == 1) {
	//			ret += afeature.BearignErrorMeanParams[p] * Distance;
	//		} else {
	//			ret += afeature.BearignErrorMeanParams[p] * pow(Distance, p);
	//		}
	//	}
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

	//	if ((p = afeature.CntBearErrorDevParams) > -1)
	//		ret = afeature.BearignErrorDevParams[0];
	//	for (p = 1; p < afeature.CntBearErrorDevParams; p++) {
	//		if (p == 1) {
	//			ret += afeature.BearignErrorDevParams[p] * Distance;
	//		} else {
	//			ret += afeature.BearignErrorDevParams[p] * pow(Distance, p);
	//		}
	//	}
	return ret;
}
