/*
 * KLocalization.h
 *
 *  Created on: Jun 3, 2009
 *      Author: trs
 */

#ifndef KLOCALIZATION_H_
#define KLOCALIZATION_H_

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <cstring>
#include <algorithm>
#include <queue>
#include <map>
#include "tools/MathFunctions.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/normal_distribution.hpp>
//#include <boost/random/variate_generator.hpp>

#define DISTANCE_WEIGHTING
#define BEARING_WEIGHTING

//#define PASTBELIEF

using namespace std;

//A structure used to store statistical data about a recognized feature
typedef struct ftr {
	double x; //position of the feature
	double y;
	string id; //Name of the feature
	double weight;
	//Parameterized Constructor
	void set(double x_, double y_, string id_, double weight_) {
		id = id_;
		x = x_;
		y = y_;
		weight = weight_;
	}

} feature;

//Special structure to keep the data of a particle
typedef struct pvar {
	double x;//position
	double y;
	double phi;// orientation
	double Weight;//Weight of a particle
	pvar() {x=0;y=0;phi=0;Weight = 0;}
	//Operator to compare 2 particles by their weight
	bool operator<(const struct pvar &other) const {
		//	cout << Weight << "Other " << other.Weight << endl;
		return Weight < other.Weight;
	}

} partcl;

//usualy particles are processed in arrays
//So in this structure a number of arrays store individually a property
typedef struct var {
	double *x; //positions of all particles
	double *y;
	double *phi;
	double *Weight; //all the weights
	double WeightSum; //the sum of the weights
	unsigned int size; //The number of the particles
	~var() {
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
typedef struct rvar {
	double val;
	double Emean;
	double Edev;
	double ratiomean;
	double ratiodev;
} randvar;

//MotionModel
typedef struct MM {
	bool freshData;
	string type; //Mono gia to distance
	randvar Distance;
	randvar Direction;
	randvar Rotation;
} KMotionModel;

//ObservationModel
typedef struct OM {
	feature Feature;
	randvar Distance;
	randvar Bearing;
} KObservationModel;

//Structure to store the belief of the robot about its position
typedef struct blf {
	double x;
	double y;
	double theta;
	double confidence;
	double weightconfidence;
} belief;

class KLocalization {
public:
	//private:

	//Random number generetors
	//boost::mt19937                     ENG;    // Mersenne Twister
    //boost::normal_distribution<double> DIST;   // Normal Distribution
    //boost::variate_generator<ENG,DIST> NORMAL_ENGINE;    // Variate generator

	float NumberOfParticlesSpreadAfterFall;
	unsigned int robustmean;
	belief AgentPosition;
	//public:

	float Beta;


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
	//Particle with the max weight
	unsigned int max_weight_particle_index;

	parts SIRParticles;
	unsigned int partclsNum;

	map<string, feature> KFeaturesmap;
	KLocalization();
	virtual ~KLocalization();

	int Initialize();

	int LoadFeaturesXML(string filename, map<string, feature>& KFeaturesmap);
	int readConfiguration(const std::string& file_name);
	bool readRobotConf(const std::string& file_name);
	///Basic SIR functions
	void Predict(parts &Particles, KMotionModel & MotionModel);
	void Update(parts &Particles, vector<KObservationModel> &Observation, KMotionModel & MotionModel, int NumofParticles);
	void Update_Ambiguous(parts & Particles, vector<KObservationModel> &Observation, int NumofParticles);
	//Not used with 2 yellow goals
	void ForceBearing(parts & Particles, vector<KObservationModel> &Observation);

	KMotionModel * findBestMotionModel(int steps, string MotionType, vector<KMotionModel> & Motions, int *iterations);

	double normalize(double *Weights, unsigned int *max_weight_index);

	float circular_mean_angle(float *angles, unsigned int size);
	//! Get Particles from observation
	belief getCurrentPosition();


	double normpdf(double diff, double dev);
	belief RobustMean(parts & Particles, int PercenteOfParticles);
	void rouletteResample(parts & Particles);

	void setBelief(double x, double y, double phi, double confidence);
	void setParticlesPose(parts & Particles, double x, double y, double phi);
	void setParticlesPoseUniformly(parts & Particles);
	void initializeParticles(parts & Particles,int playerNumber,bool kickOff);

	void spreadParticlesAfterFall(parts &,double,double, int);
	/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*::  This function converts decimal degrees to radians             :*/
	/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	double deg2rad(double deg) {
		return (deg * M_PI / 180.0);
	}

	/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*::  This function converts radians to decimal degrees             :*/
	/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	double rad2deg(double rad) {
		return (rad * 180.0 / M_PI);
	}
};

#endif /* KLOCALIZATION_H_ */
