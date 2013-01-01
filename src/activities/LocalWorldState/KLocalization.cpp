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

    //set seed (current time)
    generator.seed(static_cast<unsigned int> (std::time(0)));
	srand(time(0));

	initializeParticles(LocalizationResetMessage::UNIFORM, false, 0, 0, 0);

	cout << "\033[22;32m All Features Loaded \033[0m " << endl;
	return 1;
}

void KLocalization::initParticles()
{
	 //uniform [0,1)
    boost::uniform_real<> uni_dist(0,1);
    boost::variate_generator<r_gen&, boost::uniform_real<> > X(generator, uni_dist);

	float length = FieldMaxX * 2 / 3;
	int particlesUp = SIRParticles.size / 2;
	int particlesDown = SIRParticles.size - particlesUp;

	//Initialize top Particles
	for (int i = 0; i < particlesUp; i++)
	{
		SIRParticles.x[i] = X() * length + FieldMinX + 0.5;
		SIRParticles.y[i] = FieldMaxY;
		SIRParticles.phi[i] = TO_RAD(270);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}

	//Initialize down Particles
	for (int i = particlesUp; i < SIRParticles.size; i++)
	{
		SIRParticles.x[i] = X() * length + FieldMinX + 0.5;
		SIRParticles.y[i] = -FieldMaxY;
		SIRParticles.phi[i] = TO_RAD(90);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}
}

void KLocalization::setParticlesPoseUniformly()
{
	//uniform [0,1)
    boost::uniform_real<> uni_dist(0,1);
    boost::variate_generator<r_gen&, boost::uniform_real<> > X(generator, uni_dist);

	float length = FieldMaxX * 2 / 3;
	int resetParticles = 20;
    int particlesUp = SIRParticles.size / 2 - resetParticles / 2;
	int particlesDown = SIRParticles.size - particlesUp - resetParticles;

	//Initialize top Particles
	for (int i = 0; i < resetParticles; i++)
	{
		float x=0, y=0;

		if(playerNumber == 1)
		{
			y = 0;
			x = -2.7;
		}
		else if(playerNumber == 2)
		{
			y = 1.2;
			x = -2.4;
		}
		else if(playerNumber == 3)
		{
			y = -1.2;
			x = -2.4;
		}
		else if(playerNumber == 4)
		{
			y = -0.4;
			x = -2.4;
		}

		SIRParticles.x[i] = x;
		SIRParticles.y[i] = y;
		SIRParticles.phi[i] = TO_RAD(0);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}

	for (int i = resetParticles; i < particlesUp; i++)
	{
		SIRParticles.x[i] = X() * length + FieldMinX + 0.5;
		SIRParticles.y[i] = FieldMaxY;
		SIRParticles.phi[i] = TO_RAD(270);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}

	//Initialize down Particles
	for (int i = particlesUp; i < SIRParticles.size; i++)
	{
		SIRParticles.x[i] = X() * length + FieldMinX + 0.5;
		SIRParticles.y[i] = -FieldMaxY;
		SIRParticles.phi[i] = TO_RAD(90);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}
}

void KLocalization::initializeParticles(int resetType, bool kickOff, float inX, float inY, float inPhi)
{
	if(resetType == LocalizationResetMessage::PENALISED || resetType == LocalizationResetMessage::UNIFORM)
	{
		setParticlesPoseUniformly();
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
belief KLocalization::LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel>& Observations, vector<KObservationModel>& AmbiguousObservations)
{
	//SIR Filter
	//int index[partclsNum];
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
		Update_Ambiguous(AmbiguousObservations, SIRParticles.size);
		weightsChanged = true;
	}


	//Resample and propagate
	//Normalize Particles after resampling
	//Find the index of the max weight in the process
	if(weightsChanged)
	{
		rouletteResampleAndNormalize();

		if(Observations.size() >= 1)
		{
			;//ForceBearing(SIRParticles,Observations);
		}
	}

    if (weightsChanged || MotionModel.freshData){
        AgentPosition=calculateAvg();
    }
    /* Avg-Max
	AgentPosition.x = SIRParticles.x[maxWeightParticleIndex]  ;
	AgentPosition.y = SIRParticles.y[maxWeightParticleIndex] ;
	AgentPosition.phi = SIRParticles.phi[maxWeightParticleIndex];*/

	//TODO only one value to determine confidance, Now its only distance confidence
	AgentPosition.confidence = 0.0;
	return AgentPosition;
}


void KLocalization::Predict(KMotionModel & MotionModel)
{
	float tmpDist, tmpDir, tmpRot;
	boost::normal_distribution<> norm_dist(0,1);
    boost::variate_generator<r_gen&, boost::normal_distribution<> > X(generator, norm_dist);
    
    //Move the particles with noise
	for (int i = 0; i < SIRParticles.size; i++)
	{
		tmpDist = MotionModel.Distance.val * (MotionModel.Distance.ratiomean + X() * MotionModel.Distance.ratiodev);
		tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean + X() * MotionModel.Direction.Edev;
		tmpRot = MotionModel.Rotation.val * (MotionModel.Rotation.ratiomean + X() * MotionModel.Rotation.ratiodev);

        SIRParticles.x[i] = SIRParticles.x[i] + cos(tmpDir + SIRParticles.phi[i]) * tmpDist;
		SIRParticles.y[i] = SIRParticles.y[i] + sin(tmpDir + SIRParticles.phi[i]) * tmpDist;
		SIRParticles.phi[i] = SIRParticles.phi[i] + tmpRot;
	}
}

float KLocalization::normpdf(float diff, float dev)
{
    float denominator = dev*sqrt(2*M_PI);
	float exponential = exp(-((diff - 0)*(diff-0))/(2*dev*dev));
	return exponential/denominator;
}

void KLocalization::spreadParticlesAfterFall()
{
	boost::uniform_real<> uni_dist(0,1);
    boost::variate_generator<r_gen&, boost::uniform_real<> > X(generator, uni_dist);

	int step = round((float) SIRParticles.size / NumberOfParticlesSpreadAfterFall);

	if (NumberOfParticlesSpreadAfterFall == 0)
		return;

	for (int i = 0; i < SIRParticles.size; i += step)
	{
		SIRParticles.x[i] += (X() - 0.5) * SpreadParticlesDeviationAfterFall;
		SIRParticles.y[i] += (X() - 0.5) * SpreadParticlesDeviationAfterFall;
		SIRParticles.phi[i] += (X() - 0.5) * TO_RAD(RotationDeviationAfterFallInDeg);
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
			OverallWeightEnemyField *= normpdf((Observation[i].Distance.val - Observation[i].Distance.Emean) - R, Observation[i].Distance.Edev);
			ParticlePointBearingAngle = atan2(Observation[i].Feature.y - SIRParticles.y[p], Observation[i].Feature.x - SIRParticles.x[p]);
			ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
			OverallWeightEnemyField *= normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Observation[i].Bearing.Edev);
			//we take the symetric yellow now, so we put a - to the x and y of the observation
			R = KMath::norm2(SIRParticles.x[p] - (-Observation[i].Feature.x), SIRParticles.y[p] - (-Observation[i].Feature.y));
			OverallWeightOwnField *= normpdf((Observation[i].Distance.val - Observation[i].Distance.Emean) - R, Observation[i].Distance.Edev);
			ParticlePointBearingAngle = atan2((-Observation[i].Feature.y) - SIRParticles.y[p], (-Observation[i].Feature.x) - SIRParticles.x[p]);
			ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
			OverallWeightOwnField *= normpdf(anglediff(Observation[i].Bearing.val, ParticleBearing), Observation[i].Bearing.Edev);
		}

		OverallWeightTotal = (OverallWeightOwnField > OverallWeightEnemyField) ? OverallWeightOwnField : OverallWeightEnemyField;
		//totalWeight = OverallWeightYellowYellow + OverallWeight;
		OverallWeightTotal = (OverallWeightTotal < 0.0001) ? 0.0001 : OverallWeightTotal;
		SIRParticles.Weight[p] = OverallWeightTotal;
	}
}
void KLocalization::Update_Ambiguous(vector<KObservationModel> &Observation, int NumofParticles)
{
	//Function to update the weights of each particle regarding the ObservationDistance from an object and the direction
	float OverallWeight=0,WeightTotal=0, ParticlePointBearingAngle, ParticleBearing;
	float Weight = 0;
	float R;
	float xPosOfFeature 	= Observation[0].Feature.x;
	float yPosOfFeature 	= Observation[0].Feature.y;
	float obsDistEmean	= Observation[0].Distance.Emean;
	float obsDistEdev 	= Observation[0].Distance.Edev;
	float obsDistValue	= Observation[0].Distance.val;
	float obsBearingEdev 	= Observation[0].Bearing.Edev;
	float obsBearingValue 	= Observation[0].Bearing.val;

	//Find the best candidate for the landmark
	for (int p = 0; p < NumofParticles; p++)
	{
        Weight=1;
		//Enemy Left
		R = norm2(SIRParticles.x[p] - xPosOfFeature, SIRParticles.y[p] - yPosOfFeature);
		Weight *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(yPosOfFeature - SIRParticles.y[p], xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		Weight *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		OverallWeight = (OverallWeight>Weight) ? OverallWeight : Weight ;

		Weight = 1;
		//Enemy Right
		R = norm2(SIRParticles.x[p] - xPosOfFeature, SIRParticles.y[p] - (-yPosOfFeature));
		Weight *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2((-yPosOfFeature) - SIRParticles.y[p], xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		Weight *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		OverallWeight = (OverallWeight>Weight) ? OverallWeight : Weight ;

		Weight = 1;
		//Own Left
		R = norm2(SIRParticles.x[p] - (-xPosOfFeature), SIRParticles.y[p] - (-yPosOfFeature));
		Weight *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-yPosOfFeature - SIRParticles.y[p], -xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		Weight *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		OverallWeight = (OverallWeight>Weight) ? OverallWeight : Weight ;

		Weight = 1;
		//Own Right
		R = norm2(SIRParticles.x[p] - (-xPosOfFeature), SIRParticles.y[p] - (-(-yPosOfFeature)));
		Weight *= normpdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-(-yPosOfFeature) - SIRParticles.y[p], -xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		Weight *= normpdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		OverallWeight = (OverallWeight>Weight) ? OverallWeight : Weight ;

		WeightTotal = (OverallWeight < 0.0001) ? 0.0001 : OverallWeight;
		SIRParticles.Weight[p] = WeightTotal;
	}
}

belief KLocalization::calculateAvg(){
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


void KLocalization::rouletteResampleAndNormalize()
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

//Bad idea for 2 yellow goals
void KLocalization::ForceBearing(vector<KObservationModel> &Observation)
{
	//Calculate the bearing from each particle from each Observation
	//Force Bearing under some criteria
	float ParticlePointBearingAngle;
    int index;

	if (Observation.size() > 1)
	{
		float * angles = new float[Observation.size()];

		//for the real one
		do
		{
			index = rand() % SIRParticles.size;
		}
		while(index == maxWeightParticleIndex);

		for (unsigned int o = 0; o < Observation.size(); o++)
		{
			ParticlePointBearingAngle = atan2(Observation[o].Feature.y - SIRParticles.y[index], Observation[o].Feature.x - SIRParticles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Observation[o].Bearing.val);
		}

		SIRParticles.phi[index] = circular_mean_angle(angles, Observation.size());

		//for the symetric one
		do
		{
			index = rand() % SIRParticles.size;
		}
		while(index == maxWeightParticleIndex);

		for (unsigned int o = 0; o < Observation.size(); o++)
		{
			ParticlePointBearingAngle = atan2(-Observation[o].Feature.y - SIRParticles.y[index], -Observation[o].Feature.x - SIRParticles.x[index]);
			angles[o] = anglediff2(ParticlePointBearingAngle, Observation[o].Bearing.val);
		}

		SIRParticles.phi[index] = circular_mean_angle(angles, Observation.size());
		delete angles;
	}
	else if (Observation.size() == 1)
	{
		//for the real one
		do
		{
			index = rand() % SIRParticles.size;
		}
		while(index == maxWeightParticleIndex);

		ParticlePointBearingAngle = atan2(Observation[0].Feature.y - SIRParticles.y[index], Observation[0].Feature.x - SIRParticles.x[index]);
		SIRParticles.phi[index] = anglediff2(ParticlePointBearingAngle, Observation[0].Bearing.val);

		//for the symetric one
		do
		{
			index = rand() % SIRParticles.size;
		}
		while(index == maxWeightParticleIndex);

		ParticlePointBearingAngle = atan2(-Observation[0].Feature.y - SIRParticles.y[index], -Observation[0].Feature.x - SIRParticles.x[index]);
		SIRParticles.phi[index] = anglediff2(ParticlePointBearingAngle, Observation[0].Bearing.val);
	}
}
