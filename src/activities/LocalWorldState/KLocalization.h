/*
 * KLocalization.h
 *
 *      Author: krambo
 *		Patched: eldr4d
 */

#ifndef KLOCALIZATION_H_
#define KLOCALIZATION_H_

#include <boost/random.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <cstring>
#include <algorithm>
#include <map>
#include "tools/mathcommon.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"

#include "messages/RoboCupPlayerData.h"
#include "messages/WorldInfo.pb.h"

using namespace std;

//A structure used to store statistical data about a recognized feature
typedef struct ftr
{
	float x;
	float y;
	string id;
	float weight;
	void set(float x_, float y_, string id_, float weight_)
	{
		id = id_;
		x = x_;
		y = y_;
		weight = weight_;
	}

} feature;

//Special structure to keep the data of a particle
typedef struct pvar
{
	float x;
	float y;
	float phi;
	float Weight;
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

//usualy particles are processed in arrays
//So in this structure a number of arrays store individually a property
typedef struct var
{
	double *x;
	double *y;
	double *phi;
	double *Weight;
	double WeightSum;
    int size;
	~var()
	{
		if (x != NULL)
			delete[] x;

		if (y != NULL)
			delete[] y;

		if (phi != NULL)
			delete[] phi;

		if (Weight != NULL)
			delete[] Weight;
	}
} parts;

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
	float x;
	float y;
	float phi;
	float confidence;
	float weightconfidence;
} belief;

//random genetator
typedef boost::mt19937 r_gen;

class KLocalization
{
public:

    //random generator
    r_gen generator;

	float NumberOfParticlesSpreadAfterFall;

	float rotation_deviation;
	float SpreadParticlesDeviationAfterFall;
	float SpreadParticlesDeviation;
	float RotationDeviationAfterFallInDeg;
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
	//Particle with the max weight

    int maxWeightParticleIndex;

	//The particles we are using
	parts SIRParticles;

    int partclsNum;

	//map with all the features we read from an xml
	map<string, feature> KFeaturesmap;

	KLocalization();
	virtual ~KLocalization();

    //initialize localization
	int Initialize();

	//Functions to read from xml files
	bool readRobotConf(const std::string& file_name);

	//The step of the localization SIR filter
	belief LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel>& Observations, vector<KObservationModel>& AmbiguousObservations);

	//Update particles with the data from the odometry
	void Predict(KMotionModel & MotionModel);

	//Recalculate weights of the particles using the current observation
	void Update(vector<KObservationModel> &Observation, int NumofParticles);

	//Recalculate weights of the particles using the current ambiguous observation
	void Update_Ambiguous(vector<KObservationModel> &Observation, int NumofParticles);

	//Select parts at random and change the angle of these particles to the angle of the obervation. Bad idea for symetric field
	void ForceBearing(vector<KObservationModel> &Observation);

	//Normilize the weights of the particles
	void normalize(float *Weights, unsigned int *max_weight_index);

	//Find the mean angle from a set of angles
	float circular_mean_angle(float *angles, unsigned int size);

	//Return the propability of value from a normal pdf with deviation dev
	float normpdf(float diff, float dev);

	//This function resamples the particles with the new weigths and reposition the particles given the new weights
	void rouletteResampleAndNormalize();

	//Initialize the particles of the filter
	void initParticles();
	void setParticlesPoseUniformly();
	void initializeParticles(int resetType, bool kickOff, float inX, float inY, float inPhi);

	//Spread the particles after the fall of the robot (change the orientation)
	void spreadParticlesAfterFall();

    belief calculateAvg();
};

#endif /* KLOCALIZATION_H_ */
