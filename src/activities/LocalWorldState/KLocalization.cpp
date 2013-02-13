/*
 * KLocalization.cpp
 *
 *      Author: krambo
 *		Patched: eldr4d
 */
#include <time.h>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/distributions/normal.hpp>
#include <math.h>
#include "KLocalization.h"
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
using namespace boost;
using namespace KMath;
KLocalization::KLocalization()
{
}

KLocalization::~KLocalization()
{
}

int KLocalization::Initialize()
{
	//Initialize particles
	SIRParticles.size = partclsNum;
	SIRParticles.x = new float[partclsNum];
	SIRParticles.y = new float[partclsNum];
	SIRParticles.phi = new float[partclsNum];
	SIRParticles.Weight = new float[partclsNum];
	maxWeightParticleIndex = 0;
    actionOdError = 0.0f;

    //set seed (current time)
    generator.seed(static_cast<unsigned int> (std::time(0)));
	srand(time(0));

	InitializeParticles(LocalizationResetMessage::UNIFORM, false, 0, 0, 0);

	cout << "\033[22;32m All Features Loaded \033[0m " << endl;
	return 1;
}

void KLocalization::SetParticlesPoseUniformly()
{
	//uniform [0,1)
    boost::uniform_real<> uniDist(0,1);
    boost::variate_generator<randGen&, boost::uniform_real<> > X(generator, uniDist);

	float length = fieldMaxX * 2 / 3;
	int resetParticles = 20;
  	int particlesUp = SIRParticles.size / 2 + resetParticles / 2 ;
	int particlesDown = SIRParticles.size - particlesUp ;

    //Initialize reset Particles
	for (int i = 0; i < resetParticles; i++){
		SIRParticles.x[i] = initX[1];
		SIRParticles.y[i] = initY[1];
		SIRParticles.phi[i] = initPhi[1];
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}

	//Initialize top Particles
	for (int i = resetParticles; i < particlesUp; i++)
	{
		SIRParticles.x[i] = X() * length + fieldMinX + 0.5;
		SIRParticles.y[i] = fieldMaxY;
		SIRParticles.phi[i] = TO_RAD(270);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}

	//Initialize down Particles
	for (int i = particlesUp; i < SIRParticles.size; i++)
	{
		SIRParticles.x[i] = X() * length + fieldMinX + 0.5;
		SIRParticles.y[i] = -fieldMaxY;
		SIRParticles.phi[i] = TO_RAD(90);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}
}

void KLocalization::InitializeParticles(int resetType, bool kickOff, float inX, float inY, float inPhi)
{
	if(resetType == LocalizationResetMessage::PENALISED || resetType == LocalizationResetMessage::UNIFORM)
	{
		SetParticlesPoseUniformly();
	}
	else if(resetType == LocalizationResetMessage::READY)
	{
		for (int i = 0; i < SIRParticles.size; i++)
		{
			SIRParticles.x[i] = readyX;
			SIRParticles.y[i] = readyY;
			SIRParticles.phi[i] = readyPhi;
			SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
		}
	}
	else if(resetType == LocalizationResetMessage::SET)
	{
		for (int i = 0; i < SIRParticles.size; i++)
		{
			SIRParticles.x[i] = initX[(kickOff) ? 0 : 1] + ((float)rand() / (float)RAND_MAX) * 0.2 - 0.1;
			SIRParticles.y[i] = initY[(kickOff) ? 0 : 1] + ((float)rand() / (float)RAND_MAX) * 0.2 - 0.1;
			SIRParticles.phi[i] = initPhi[(kickOff) ? 0 : 1];
			SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
		}
	}else if(resetType == LocalizationResetMessage::MANUAL){
		for (int i = 0; i < SIRParticles.size; i++)
		{
			SIRParticles.x[i] = inX;
			SIRParticles.y[i] = inY;
			SIRParticles.phi[i] = inPhi;
			SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
		}
	}
}

//Sequential Importance Resampling
KLocalization::belief KLocalization::LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel>& Observations, vector<KObservationModel>& AmbiguousObservations)
{

	bool weightsChanged = false;

	//Predict - Move particles according the Prediction Model
	if(MotionModel.freshData)
		Predict(MotionModel);

	//Update - Using incoming observation
	if(Observations.size() >= 1)
	{
		Update(Observations, SIRParticles.size);
		weightsChanged = true;
	}
	else if(AmbiguousObservations.size() == 1)
	{
		UpdateAmbiguous(AmbiguousObservations, SIRParticles.size);
		weightsChanged = true;
	}

	//Resample and propagate
	//Normalize Particles after resampling
	//Find the index of the max weight in the process
	if(weightsChanged)
	{
		RouletteResampleAndNormalize();
	}

    if (weightsChanged || MotionModel.freshData){
        agentPosition=ComputeAvg();
    }
    /* Avg-Max
	agentPosition.x = SIRParticles.x[maxWeightParticleIndex]  ;
	agentPosition.y = SIRParticles.y[maxWeightParticleIndex] ;
	agentPosition.phi = SIRParticles.phi[maxWeightParticleIndex];*/

	//TODO only one value to determine confidance, Now its only distance confidence
	agentPosition.confidence = 0.0;
	return agentPosition;
}


void KLocalization::Predict(KMotionModel & MotionModel)
{
	float tmpDist, tmpDir, tmpRot;
	boost::normal_distribution<> normDist(0,1);
    boost::variate_generator<randGen&, boost::normal_distribution<> > X(generator, normDist);
    
    //Move the particles with noise
	for (int i = 0; i < SIRParticles.size; i++)
	{
		tmpDist = MotionModel.Distance.val * (MotionModel.Distance.ratiomean + X() * MotionModel.Distance.ratiodev);
		tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean + X() * MotionModel.Direction.Edev;
		tmpRot = MotionModel.Rotation.val * (MotionModel.Rotation.ratiomean + X() * MotionModel.Rotation.ratiodev);

        SIRParticles.x[i] = SIRParticles.x[i] + cos(tmpDir + SIRParticles.phi[i]) * tmpDist;
		SIRParticles.y[i] = SIRParticles.y[i] + sin(tmpDir + SIRParticles.phi[i]) * tmpDist;
		SIRParticles.phi[i] = SIRParticles.phi[i] + tmpRot + actionOdError;
	}
}

float KLocalization::NormPdf(float diff, float dev)
{
    float denominator = dev*sqrt(2*M_PI);
	float exponential = exp(-((diff - 0)*(diff-0))/(2*dev*dev));
	return exponential/denominator;
}

void KLocalization::SpreadParticlesAfterFall()
{
	boost::uniform_real<> uniDist(0,1);
    boost::variate_generator<randGen&, boost::uniform_real<> > X(generator, uniDist);

	int step = round((float) SIRParticles.size / numberOfParticlesSpreadAfterFall);

	if (numberOfParticlesSpreadAfterFall == 0)
		return;

	for (int i = 0; i < SIRParticles.size; i += step)
	{
		SIRParticles.x[i] += (X() - 0.5) * spreadParticlesDeviationAfterFall;
		SIRParticles.y[i] += (X() - 0.5) * spreadParticlesDeviationAfterFall;
		SIRParticles.phi[i] += (X() - 0.5) * TO_RAD(rotationDeviationAfterFallInDeg);
	}
}

void KLocalization::Update(vector<KObservationModel> &Observation, int NumofParticles)
{
	//Function to update the weights of each particle regarding the ObservationDistance from an object and the direction
	float OverallWeightOwnField, OverallWeightEnemyField, OverallWeightTotal;
	float ParticlePointBearingAngle, ParticleBearing;
	float R;

	for (int p = 0; p < NumofParticles; p++)
	{
		OverallWeightOwnField = 1.0;
		OverallWeightEnemyField = 1.0;
		OverallWeightTotal = 0;

		for (unsigned int i = 0; i < Observation.size(); i++)
		{
			R = KMath::norm2(SIRParticles.x[p] - Observation[i].Feature.x, SIRParticles.y[p] - Observation[i].Feature.y);
			OverallWeightEnemyField *= NormPdf((Observation[i].Distance.val - Observation[i].Distance.Emean) - R, Observation[i].Distance.Edev);
			ParticlePointBearingAngle = atan2(Observation[i].Feature.y - SIRParticles.y[p], Observation[i].Feature.x - SIRParticles.x[p]);
			ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
			OverallWeightEnemyField *= NormPdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Observation[i].Bearing.Edev);
			//we take the symetric yellow now, so we put a - to the x and y of the observation
			R = KMath::norm2(SIRParticles.x[p] - (-Observation[i].Feature.x), SIRParticles.y[p] - (-Observation[i].Feature.y));
			OverallWeightOwnField *= NormPdf((Observation[i].Distance.val - Observation[i].Distance.Emean) - R, Observation[i].Distance.Edev);
			ParticlePointBearingAngle = atan2((-Observation[i].Feature.y) - SIRParticles.y[p], (-Observation[i].Feature.x) - SIRParticles.x[p]);
			ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
			OverallWeightOwnField *= NormPdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Observation[i].Bearing.Edev);
		}

		OverallWeightTotal = (OverallWeightOwnField > OverallWeightEnemyField) ? OverallWeightOwnField : OverallWeightEnemyField;

		OverallWeightTotal = (OverallWeightTotal < 0.0001) ? 0.0001 : OverallWeightTotal;
		SIRParticles.Weight[p] = OverallWeightTotal;
	}
}
void KLocalization::UpdateAmbiguous(vector<KObservationModel> &Observation, int NumofParticles)
{
	//Function to update the weights of each particle regarding the ObservationDistance from an object and the direction
	float ParticlePointBearingAngle, ParticleBearing;
	float AdditiveWeightTotal = 0, AdditiveOwnField = 0, AdditiveEnemyField = 0;
	float R;
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
		R = norm2(SIRParticles.x[p] - xPosOfFeature, SIRParticles.y[p] - yPosOfFeature);
		AdditiveEnemyField *= NormPdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(yPosOfFeature - SIRParticles.y[p], xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		AdditiveEnemyField *= NormPdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveEnemyField;
		AdditiveEnemyField = 1;
		//Enemy Right
		R = norm2(SIRParticles.x[p] - xPosOfFeature, SIRParticles.y[p] - (-yPosOfFeature));
		AdditiveEnemyField *= NormPdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2((-yPosOfFeature) - SIRParticles.y[p], xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		AdditiveEnemyField *= NormPdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveEnemyField;
		//Own Left
		R = norm2(SIRParticles.x[p] - (-xPosOfFeature), SIRParticles.y[p] - (-yPosOfFeature));
		AdditiveOwnField *= NormPdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-yPosOfFeature - SIRParticles.y[p], -xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		AdditiveOwnField *= NormPdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveOwnField;
		AdditiveOwnField = 1;
		//Own Right
		R = norm2(SIRParticles.x[p] - (-xPosOfFeature), SIRParticles.y[p] - (-(-yPosOfFeature)));
		AdditiveOwnField *= NormPdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-(-yPosOfFeature) - SIRParticles.y[p], -xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		AdditiveOwnField *= NormPdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		AdditiveWeightTotal += AdditiveOwnField;
		AdditiveWeightTotal = (AdditiveWeightTotal < 0.0001) ? 0.0001 : AdditiveWeightTotal;
		SIRParticles.Weight[p] = AdditiveWeightTotal;
	}
}

KLocalization::belief KLocalization::ComputeAvg(){
    belief agentPos;
    float aCos=0,aSin=0;

    agentPos.x=0;
    agentPos.y=0;
    agentPos.phi=0;

	for (int i = 0; i < SIRParticles.size; i++){
        agentPos.x+=SIRParticles.x[i];
        agentPos.y+=SIRParticles.y[i];
        aSin+=sin(SIRParticles.phi[i]);
        aCos+=cos(SIRParticles.phi[i]);
    }

    agentPos.x/=SIRParticles.size;
    agentPos.y/=SIRParticles.size;
    agentPos.phi=atan2( 1.0/SIRParticles.size * aSin , 1.0/SIRParticles.size * aCos);

    return agentPos;
}


void KLocalization::RouletteResampleAndNormalize()
{

	float sum = 0;
	float max = -1;
	for (int i = 0; i < SIRParticles.size; i++){
		sum += SIRParticles.Weight[i];
	}

	float r = (rand() / ((float)RAND_MAX))*sum;
	r = r / SIRParticles.size;
	float cumSum = SIRParticles.Weight[0];
	float step = sum / ((float)SIRParticles.size);
	int i = 0;
	float tempX[SIRParticles.size];
	float tempY[SIRParticles.size];
	float tempPhi[SIRParticles.size];

	for (int m = 0; m < SIRParticles.size; m++)
	{
		//float u = r + 1.0*m/nsamples;
		while (r > cumSum)
		{
			i++;
			cumSum += SIRParticles.Weight[i];
		}

        //find max weight particle index
        if(SIRParticles.Weight[i] > max)
		{
			max = SIRParticles.Weight[i];
			maxWeightParticleIndex = m;
		}

		r += step;
		tempX[m] = SIRParticles.x[i];
		tempY[m] = SIRParticles.y[i];
		tempPhi[m] = SIRParticles.phi[i];
	}

	float newWeight = 1.0 / SIRParticles.size;

	// lets propagate them
	for (i = 0; i < SIRParticles.size; i++)
	{
		SIRParticles.phi[i] = tempPhi[i];
		SIRParticles.Weight[i] = newWeight;
		SIRParticles.x[i] = tempX[i];
		SIRParticles.y[i] = tempY[i];
	}
}
