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
#include "tools/mathcommon.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "messages/RoboCupPlayerData.h"
#include "messages/WorldInfo.pb.h"
#include "LocalizationStructs.h"

using namespace std;



class KLocalization
{
public:

    //Augmented mcl
    typedef struct AMCL{
        float avgLikelihood;
        float shortHist;
        float longHist;

        //Parameters to estimate the long-term, and short-term, averages respectively 
        float aslow;
        float afast;

        int firstUpdate;
        int enable;
        int winDuration;
    }AugMCL;

    //Random genetator
    typedef boost::mt19937 randGen;

    //Agent's belief
    Localization::belief agentPosition;
    
    //Odomery error 
    float actionOdError;
    randGen generator;

    bool weightsChanged;

	// structs to store xml data
    Localization::LocConfig* locConfig;

	

	//Particle with the max weight
    int maxWeightParticleIndex;

	//The particles we are using
	Localization::parts SIRParticles;

    int partclsNum;
    AugMCL augMCL;



    vector<Localization::KObservationModel> windowObservations;

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
	Localization::belief LocalizationStepSIR(Localization::KMotionModel & motionModel, vector<Localization::KObservationModel>& observations, vector<Localization::KObservationModel>& ambiguousObservations);

     /**
	 * @brief Updates particles with the data from the odometry
	 */
	void Predict(Localization::KMotionModel & motionModel);

    /**
	 * @brief Recalculate weights of the particles using the current observation
	 */
	void Update(vector<Localization::KObservationModel> &observation, int numofParticles);

    /**
	 * @brief Recalculate weights of the particles using the current ambiguous observation
	 */
	void UpdateAmbiguous(vector<Localization::KObservationModel> &observation, int numofParticles);

    /**
	 * @brief Returns the propability of value from a normal pdf with deviation dev
	 */
    float NormPdf(float diff, float dev);

    /**
	 * @brief This function resamples the particles with the new weigths and reposition the particles given the new weights
	 */
	void RouletteResampleAndNormalize(vector<Localization::KObservationModel>& Observations);

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
    Localization::belief ComputeAvg();

    /**
	 * @brief Generate particle from a single frame
	 */
    Localization::partcl generateParticle(vector<Localization::KObservationModel>& Observations);

    /**
	 * @brief Generate particle from single observations
	 */
    Localization::partcl generateParticleWindow(vector<Localization::KObservationModel>& Observations);

	void updateLikelihoodHist(float weightSum);

    vector<float> circleIntersection (Localization::KObservationModel& obs1, Localization::KObservationModel& obs2);

    void windowObservationsUpdate();
};

#endif /* KLOCALIZATION_H_ */
