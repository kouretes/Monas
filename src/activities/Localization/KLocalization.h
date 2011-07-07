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




//#define WEBOTS
//#define VISIBILITY_WEIGHTING
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

		//Distance and Bearing error
		//For polunomial mean error Functions
		//Pointer to Arrays of parameters
		double *DistErrorMeanParams;
		double *BearignErrorMeanParams;
		//Number of polunomial parameters
		short CntDistErrorMeanParams;
		short CntBearErrorMeanParams;
		//For polunomial error's deviation Functions
		//Pointer to Arrays of parameters
		double *DistErrorDevParams;
		double *BearignErrorDevParams;
		//Number of polunomial parameters
		short CntDistErrorDevParams;
		short CntBearErrorDevParams;

		//Zero Constructor
		ftr() {
			id = "";
			x = 0;
			y = 0;
			weight = 0;
			CntDistErrorDevParams = 0;
			CntBearErrorDevParams = 0;

			CntDistErrorMeanParams = 0;
			CntBearErrorMeanParams = 0;

			DistErrorMeanParams = NULL;
			BearignErrorMeanParams = NULL;

			DistErrorDevParams = NULL;
			BearignErrorDevParams = NULL;
		}
		//Parameterized Constructor
		ftr(double x_, double y_, string id_, double weight_, short CntDistErrorDevParams_, short CntBearErrorDevParams_, double *DistErrorDevParams_, double*BearignErrorDevParams_, short CntDistErrorMeanParams_, short CntBearErrorMeanParams_, double *DistErrorMeanParams_, double*BearignErrorMeanParams_) {

			set(x_, y_, id_, weight_, CntDistErrorDevParams_, CntBearErrorDevParams_, DistErrorDevParams_, BearignErrorDevParams_, CntDistErrorMeanParams_, CntBearErrorMeanParams_, DistErrorMeanParams_, BearignErrorMeanParams_);

		}

		void set(double x_, double y_, string id_, double weight_, short CntDistErrorDevParams_, short CntBearErrorDevParams_, double *DistErrorDevParams_, double*BearignErrorDevParams_, short CntDistErrorMeanParams_, short CntBearErrorMeanParams_, double *DistErrorMeanParams_, double*BearignErrorMeanParams_) {

			id = id_;
			x = x_;
			y = y_;
			weight = weight_;
			CntDistErrorDevParams = CntDistErrorDevParams_;
			CntBearErrorDevParams = CntBearErrorDevParams_;
			DistErrorDevParams = DistErrorDevParams_;
			BearignErrorDevParams = BearignErrorDevParams_;

			CntDistErrorMeanParams = CntDistErrorMeanParams_;
			CntBearErrorMeanParams = CntBearErrorMeanParams_;
			DistErrorMeanParams = DistErrorMeanParams_;
			BearignErrorMeanParams = BearignErrorMeanParams_;
		}

} feature;

//Special structure to keep the data of a particle
typedef struct pvar {
		double x;//position
		double y;
		double phi;// orientation
		double Weight;//Weight of a particle

		pvar() {
			x = 0;
			y = 0;
			phi = 0;
			Weight = 0;
		}

		pvar(int x_, int y_, int phi_, int Weight_) {

			x = x_;
			y = y_;
			phi = phi_;
			Weight = Weight_;
		}

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
		var() { //zero Constructor
			size = 0;
			x = NULL;
			y = NULL;
			phi = NULL;
			Weight = NULL;
			WeightSum = -1;
		}
		var(int size_) {
			if (size > 0) {
				size = size_;
				x = new double[size];
				y = new double[size];
				phi = new double[size];
				Weight = new double[size];
				WeightSum = -1;
			} else
				var();
		}
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
		partcl * get_particle(int pos) { //Take a particle
			if (pos > 0)
				if ((unsigned int) pos < size)
					return new partcl(x[pos], y[pos], phi[pos], Weight[pos]);

			return NULL;
		}

} parts;

//Random Variable
//Here is store the odometry data
typedef struct rvar {
		double val;
		double Emean;
		double Edev;
		double ratiomean;
		double ratiodev;
		rvar() {
			val = 0;
			Emean = 0;
			Edev = 0;
			ratiomean = 0;
			ratiodev =0 ;
		}

		rvar(double val_, double mean_, double dev_) {
			set(val_, mean_, dev_);
		}
		void set(double val_, double mean_, double dev_) {
			val = val_;
			Emean = mean_;
			Edev = dev_;
		}
		//operator to export data
		// friend ostream &operator<<(ostream &stream, struct rvar obj);
		friend ostream &operator<<(ostream &stream, struct rvar obj) {
			stream << obj.val << " ";
			stream << obj.Emean << " ";
			stream << obj.Edev;
			return stream; // return the stream
		}
		//operator to import data
		friend istream &operator>>(istream &stream, struct rvar &obj) {
			cout << "Entered X,Y,Z values: ";
			stream >> obj.val >> obj.Emean >> obj.Edev;
			return stream;
		}

} randvar;

//MotionModel
typedef struct MM {
		string type;
		int Steps;
		randvar Distance;
		randvar Direction;
		randvar Rotation;
} KMotionModel;

//ObservationModel
typedef struct OM {
		feature Feature;
		randvar Distance;
		//randvar Angle;
		randvar Bearing;
} KObservationModel;

//Structure to store the belief of the robot about its position
typedef struct blf {
		double x;
		double y;
		double theta;
		double confidence;
		double weightconfidence;
		blf() {
			x = 0.0;
			y = 0.0;
			theta = 0.0;
			confidence = 0.0;
			weightconfidence = 0.0;
		}
} belief;

class KLocalization {
	public:
		//private:
		double* Distances;

		float P_observe_NotVisible;
		float P_Notobserve_NotVisible;
		float P_observe_Visible;
		float P_Notobserve_Visible;

		float numofparticlesfromObservation;

		double riza2pi;

		unsigned int robustmean;
		int depletions_counter;
		belief AgentPosition;
		//public:

		float Beta;
		float * Beta2;
		float max_observation_distance;
		float max_observation_distance_deviation;
		float min_observation_distance_deviation;

		float max_observation_bearing_deviation;
		float min_observation_bearing_deviation;

		float halfrange;

		double SpreadParticlesDeviation;
		double rotation_deviation;
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


		parts SIRParticles;
		parts AUXParticles;
		vector<feature> allfeatures;
		unsigned int partclsNum;

		vector<KMotionModel> KouretesMotions;
		map<string, vector<KMotionModel> > KMMmap;
		map<string, feature> KFeaturesmap;
		KLocalization();
		virtual ~KLocalization();

		int Initialize();
		int LoadMotionModelXML(string filename, vector<KMotionModel>& Motions, map<string, vector<KMotionModel> > & KMMmap);
		int LoadFeaturesXML(string filename, map<string, feature>& KFeaturesmap);
		///Basic SIR functions
		void Predict(parts &Particles, KMotionModel & MotionModel);
		void Update(parts &Particles, vector<KObservationModel> &Observation, KMotionModel & MotionModel, int NumofParticles, double rangemin, double rangemax);
		void Update_Ambigius(parts &Particles, vector<KObservationModel> &Observation, int NumofParticles);

		void ForceBearing(parts & Particles, vector<KObservationModel> &Observation);
		float ESS(parts &Particles); //Calculate Effective Sample Size
		void Resample(parts &Particles, int * Index, int param);
		void Propagate(parts &Particles, int * Index);

		belief LocalizationStep(int steps, string MotionType, vector<KObservationModel> & Observation, double rangemin, double rangemax);
		belief LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel> & Observation, double rangemin, double rangemax);

		KMotionModel * findBestMotionModel(int steps, string MotionType, vector<KMotionModel> & Motions, int *iterations);

		double normalize(double *Weights, int size);
		double CalculateConfidence(parts & Particles, belief & blf);
		double CalcDistDev(feature afeature, double Distance);
		double CalcBearDev(feature afeature, double Distance);

		double CalcDistMean(feature afeature, double Distance);
		double CalcBearMean(feature afeature, double Distance);
		float circular_mean_angle(float *angles, unsigned int size);
		//! Get Particles from observation
		belief getCurrentPosition();

		//bool isVisible(feature Feature, parts &Particles, int particle);
		//bool isVisible(feature Feature, partcl);

		bool isVisible(feature & Feature, parts &Particles, int particle, double rangemaxleft, double rangemaxright);
		bool isVisible(feature & Feature, partcl, double rangemaxleft, double rangemaxright);

		double normpdf(double diff, double dev);
		belief RobustMean(parts & Particles, int PercenteOfParticles);
		double * CumSum(double * Table, int size);
		double * FlipCumProd(double * Table, int size);
		int * ResampleSWR(parts & Particles, int *Index);
		int * multinomialR(parts & Particles, int *Index);

		void setBelief(double x, double y, double phi, double confidence);
		void setParticlesPose(parts & Particles, double x, double y, double phi);
		void setParticlesPoseUniformly(parts & Particles);

		int ObservationParticles(vector<KObservationModel> & Observation, parts &Paticles, int Xdim, int Ydim, int resolution, double rangemaxleft, double rangemaxright);
		int CircleIntersectionPossibleParticles(vector<KObservationModel> &Observation, parts &Particles, int numofparticlesfromObservation) ;
		void SpreadParticles(parts & Particles, double Deviation, double rotation_deviation, int Percent);
		void SpreadParticlesCirc(parts & Particles, double Deviation, double rotation_deviation, int Percent);

		/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
		/*::  This function converts decimal degrees to radians             :*/
		/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
		double deg2rad(double deg) {
			return (deg * M_PI / 180);
		}

		/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
		/*::  This function converts radians to decimal degrees             :*/
		/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
		double rad2deg(double rad) {
			return (rad * 180 / M_PI);
		}

		float randfloat() {

			return rand() / (float(RAND_MAX)/* + 1*/);
		}

		//generates a psuedo-random float between 0.0 and max
		float randfloat(float max) {
			return randfloat() * max;
		}

		//generates a psuedo-random float between min and max
		double randdouble(double min, double max) {
			if (min > max) {
				return drand48() * (min - max) + max;
			} else {
				return drand48() * (max - min) + min;
			}
		}
		//generates a psuedo-random float between min and max
		float randfloat(float min, float max) {
			if (min > max) {
				return randfloat() * (min - max) + max;
			} else {
				return randfloat() * (max - min) + min;
			}
		}

		double wrapToPi(double angle) {
			//			double angle = wrapTo0_2Pi(angle) - M_PI;
			//			if(angle>M_PI)
			//				return angle -2*M_PI;
			//			return angle;
			while (angle > M_PI)
				angle -= 2.0 * M_PI;
			while (angle < -M_PI)
				angle += 2.0 * M_PI;
			return angle;
		}
		double wrapTo2Pi(double angle) {
			//angle = fmod(M_PI * 2 + angle, M_PI * 2);

			while (angle > 2.0 * M_PI)
				angle -= 2.0 * M_PI;
			while (angle < -2.0 * M_PI)
				angle += 2.0 * M_PI;
			return angle;
		}
		double wrapTo0_2Pi(double angle) {
			//return fmod(M_PI * 2.0 + angle, M_PI * 2.0);
			while (angle > 2.0 * M_PI)
				angle -= 2.0 * M_PI;
			while (angle < 0)
				angle += 2.0 * M_PI;
			return angle;
		}

		double anglediff2(double a1, double a2) {
			return wrapToPi(wrapToPi(a1 + M_PI - a2) - M_PI);
		}

		double anglediff(double a1, double a2) {
			return fabs(wrapTo0_2Pi(a1 + M_PI - a2) - M_PI);
		}
};

#endif /* KLOCALIZATION_H_ */
