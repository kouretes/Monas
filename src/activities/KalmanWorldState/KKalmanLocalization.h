/*
 * KKalmanLocalization.h
 *
 *      Author: vosk
 */

#ifndef KKalmanLocalization_H_
#define KKalmanLocalization_H_

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <cstring>
#include <algorithm>
#include <queue>
#include <map>
#include "tools/mathcommon.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "messages/RoboCupPlayerData.h"

#include "KalmanParticle.h"


using namespace std;


//Special structure to keep the data of a particle
typedef struct pvar
{
	double x;
	double y;
	double phi;
	double Weight;
	pvar()
	{
		x = 0;
		y = 0;
		phi = 0;
		Weight = 0;
	}
	bool operator<(const struct pvar &other) const
	{
		return Weight < other.Weight;
	}

} partcl;

//A structure used to store statistical data about a recognized feature
typedef struct ftr
{
	double x;
	double y;
	string id;
	double weight;
	void set(double x_, double y_, string id_, double weight_)
	{
		id = id_;
		x = x_;
		y = y_;
		weight = weight_;
	}

} feature;


//Random Variable
typedef struct rvar
{
	double val;
	double Emean;
	double Edev;
	double ratiomean;
	double ratiodev;
} randvar;

//MotionModel
typedef struct MM
{
	bool freshData;
	string type;
	randvar Distance;
	randvar Direction;
	randvar Rotation;
} KMotionModel;

//ObservationModel
typedef struct OM
{
	feature Feature;
	randvar Distance;
	randvar Bearing;
} KObservationModel;

//Structure to store the belief of the robot about its position
typedef struct blf
{
	double x;
	double y;
	double theta;
	double confidence;
	double weightconfidence;
} belief;

class KKalmanLocalization
{
public:



	//Random number generetors

	float NumberOfParticlesSpreadAfterFall;
	unsigned int robustmean;


	double rotation_deviation;
	double SpreadParticlesDeviationAfterFall;
	double SpreadParticlesDeviation;
	double RotationDeviationAfterFallInDeg;
	int PercentParticlesSpread;


	//Field
	float CarpetMaxX;
	float CarpetMinX;
	float CarpetMaxY;
	float CarpetMinY;
	float FieldMaxX;
	float FieldMinX;
	float FieldMaxY;
	float FieldMinY;
	//Team
	float initX[2], initY[2], initPhi[2];
	int playerNumber;

	int maxparticles;
	unsigned current_particles;

	//map with all the features we read from an xml
	map<string, feature> KFeaturesmap;

	KKalmanLocalization();
	virtual ~KKalmanLocalization();

	//initialize localization
	int Initialize();

	//Functions to read from xml files
	int LoadFeaturesXML(string filename, map<string, feature>& KFeaturesmap);
	int readConfiguration(const std::string& file_name);
	bool readRobotConf(const std::string& file_name);

	//The step of the localization SIR filter
	belief LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel>& Observations, vector<KObservationModel>& AmbiguousObservations,float dt);

	//Update particles with the data from the odometry
	void Predict(KMotionModel & MotionModel,float dt);

	//Recalculate weights of the particles using the current observation
	void Update(vector<KObservationModel> &Observation);

	//Recalculate weights of the particles using the current ambiguous observation
	void Update_Ambiguous(vector<KObservationModel> &Observation);





	//Returns the current position of the agent
	belief getCurrentPosition() const;

	//Initialize the particles of the filter
	void initParticles();
	void setParticlesPoseUniformly();
	void initializeParticles(int playerState, bool kickOff);

	vector<KalmanParticle> kalmanpoints;

	void pruneParticles();

	//Calculate belief
	KalmanParticle::Pvector MMSE_belief(unsigned start=0)const;
	KalmanParticle::Pmatrix MMSE_variance(KalmanParticle::Pvector mean,unsigned start=0)const;

	//Spread the particles after the fall of the robot (change the orientation)
	void spreadParticlesAfterFall();
};

#endif /* KKalmanLocalization_H_ */
