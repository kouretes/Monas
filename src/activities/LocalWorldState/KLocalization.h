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



class KLocalization
{
public:

    /**
	 * @struct ftr
	 * @brief A structure used to store statistical data about a recognized feature
	 */
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

    /**
	 * @struct pvar
	 * @brief Special structure to keep the data of a particle
	 */
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

     /**
	 * @struct var
	 * @brief Usualy particles are processed in arrays.So in this structure a number of arrays store individually a property
	 */
    typedef struct var
    {
	    float *x;
	    float *y;
	    float *phi;
	    float *Weight;
	    float WeightSum;
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

    /**
	 * @struct rvar
	 * @brief Random variable structure
	 */
    typedef struct rvar
    {
	    float val;
	    float Emean;
	    float Edev;
	    float ratiomean;
	    float ratiodev;
    } randvar;

    /**
	 * @struct MM
	 * @brief Structure to keep motion model info
	 */
    typedef struct MM
    {
	    bool freshData;
	    string type;
	    randvar Distance;
	    randvar Direction;
	    randvar Rotation;
    } KMotionModel;


    /**
	 * @struct OM
	 * @brief Structure to keep observation model info
	 */
    typedef struct OM
    {
	    feature Feature;
	    randvar Distance;
	    randvar Bearing;
    } KObservationModel;


    /**
	 * @struct blf
	 * @brief  Structure to store the belief of the robot about its position
	 */
    typedef struct blf
    {
	    float x;
	    float y;
	    float phi;
	    float confidence;
	    float weightconfidence;
    } belief;

    //Random genetator
    typedef boost::mt19937 randGen;

    //Agent's belief
    belief agentPosition;
    
    //Odomery error 
    float actionOdError;
    randGen generator;


	float numberOfParticlesSpreadAfterFall;
	float rotationDeviation;
	float spreadParticlesDeviationAfterFall;
	float spreadParticlesDeviation;
	float rotationDeviationAfterFallInDeg;
	int percentParticlesSpread;

	//Field
	float carpetMaxX;
	float carpetMinX;
	float carpetMaxY;
	float carpetMinY;
	float fieldMaxX;
	float fieldMinX;
	float fieldMaxY;
	float fieldMinY;

	//Team
	float initX[2], initY[2], initPhi[2];
    float readyX,readyY,readyPhi;
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

    /**
	 * @brief Activity initialization
	 */
	int Initialize();

    /**
	 * @brief Get robot configuration from xml file
	 */
	bool ReadRobotConf(const string& file_name);

     /**
	 * @brief The step of the localization SIR filter
	 */
	belief LocalizationStepSIR(KMotionModel & motionModel, vector<KObservationModel>& observations, vector<KObservationModel>& ambiguousObservations);

     /**
	 * @brief Updates particles with the data from the odometry
	 */
	void Predict(KMotionModel & motionModel);

    /**
	 * @brief Recalculate weights of the particles using the current observation
	 */
	void Update(vector<KObservationModel> &observation, int numofParticles);

    /**
	 * @brief Recalculate weights of the particles using the current ambiguous observation
	 */
	void UpdateAmbiguous(vector<KObservationModel> &observation, int numofParticles);

    /**
	 * @brief Returns the propability of value from a normal pdf with deviation dev
	 */
    float NormPdf(float diff, float dev);

    /**
	 * @brief This function resamples the particles with the new weigths and reposition the particles given the new weights
	 */
	void RouletteResampleAndNormalize();

    /**
	 * @brief Sets the particles to the initial positions
	 */
	void SetParticlesPoseUniformly();

     /**
	 * @brief Initialize particles 
	 */
	void InitializeParticles(int resetType, bool kickOff, float inX, float inY, float inPhi);

    /**
	 * @brief Spreads the particles after the fall of the robot (change the orientation)
	 */
	void SpreadParticlesAfterFall();
    
    /**
	 * @brief Computes an average over the particles
	 */
    belief ComputeAvg();
};

#endif /* KLOCALIZATION_H_ */
