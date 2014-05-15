/*
 * KLocalization.cpp
 *
 *      Author: krambo
 *		Patched: eldr4d
 */
#include <time.h>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include "core/architecture/time/SystemTime.hpp"
#include <boost/math/distributions/normal.hpp>
#include <math.h>
#include "KLocalization.h"
#include <iomanip>

//using namespace boost;
using namespace KSystem::Time;
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
	SIRParticles.size = locConfig->partclsNum;
	SIRParticles.x = new float[locConfig->partclsNum];
	SIRParticles.y = new float[locConfig->partclsNum];
	SIRParticles.phi = new float[locConfig->partclsNum];
	SIRParticles.Weight = new float[locConfig->partclsNum];
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

	float length = locConfig->fieldMaxX * 2 / 3;
	int resetParticles = 20;
  	int particlesUp = SIRParticles.size / 2 + resetParticles / 2 ;
	int particlesDown = SIRParticles.size - particlesUp ;

    //Initialize reset Particles
	for (int i = 0; i < resetParticles; i++){
		SIRParticles.x[i] = locConfig->initX[1];
		SIRParticles.y[i] = locConfig->initY[1];
		SIRParticles.phi[i] = locConfig->initPhi[1];
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}

	//Initialize top Particles
	for (int i = resetParticles; i < particlesUp; i++)
	{
		SIRParticles.x[i] = X() * length + locConfig->fieldMinX + 0.5;
		SIRParticles.y[i] = locConfig->fieldMaxY;
		SIRParticles.phi[i] = TO_RAD(270);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}

	//Initialize down Particles
	for (int i = particlesUp; i < SIRParticles.size; i++)
	{
		SIRParticles.x[i] = X() * length + locConfig->fieldMinX + 0.5;
		SIRParticles.y[i] = -locConfig->fieldMaxY;
		SIRParticles.phi[i] = TO_RAD(90);
		SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
	}
}

void KLocalization::InitializeParticles(int resetType, bool kickOff, float inX, float inY, float inPhi)
{
    //Initialize particle related parameters
    boost::uniform_real<> uniDist(0,1);
	boost::variate_generator<randGen&, boost::uniform_real<> > X(generator, uniDist);
	augMCL.firstUpdate=true;
    augMCL.avgLikelihood=1;
    augMCL.shortHist=1;
    augMCL.longHist=1;
	maxWeightParticleIndex = 0;


	if(resetType == LocalizationResetMessage::PENALISED || resetType == LocalizationResetMessage::UNIFORM)
	{
		SetParticlesPoseUniformly();
	}
	else if(resetType == LocalizationResetMessage::READY)
	{
		for (int i = 0; i < SIRParticles.size; i++)
		{
			SIRParticles.x[i] = locConfig->readyX;
			SIRParticles.y[i] = locConfig->readyY;
			SIRParticles.phi[i] = locConfig->readyPhi;
			SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
		}
	}
	else if(resetType == LocalizationResetMessage::SET)
	{
		for (int i = 0; i < SIRParticles.size; i++)
		{
		    SIRParticles.x[i] = locConfig->initX[(kickOff) ? 0 : 1] + X() * 0.1;
			SIRParticles.y[i] = locConfig->initY[(kickOff) ? 0 : 1] + X() * 0.1;
			SIRParticles.phi[i] = locConfig->initPhi[(kickOff) ? 0 : 1];
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
	}else if(resetType == LocalizationResetMessage::PENALTY_MODE){
		for (int i = 0; i < SIRParticles.size; i++)
		{
			SIRParticles.x[i] = 0;
			SIRParticles.y[i] = 0;
			SIRParticles.phi[i] = 0;
			SIRParticles.Weight[i] = 1.0 / SIRParticles.size;
		}
	}

}

//Sequential Importance Resampling
Localization::belief KLocalization::LocalizationStepSIR(Localization::KMotionModel & MotionModel, vector<Localization::KObservationModel>& Observations, vector<Localization::KObservationModel>& AmbiguousObservations)
{

	weightsChanged = false;

	//Predict - Move particles according the Prediction Model
	if(MotionModel.freshData)
		Predict(MotionModel);

	//Update - Using incoming observation
	if(Observations.size() >= 1)
	{
		Update(Observations, SIRParticles.size);
        if (Observations.size()==1 && augMCL.enable == true){
			windowObservations.push_back(Observations[0]);
		}
		weightsChanged = true;
	}
	else if(AmbiguousObservations.size() == 1)
	{
		UpdateAmbiguous(AmbiguousObservations, SIRParticles.size);
        if (augMCL.enable == true){
		    windowObservations.push_back(AmbiguousObservations[0]);
        }
		weightsChanged = true;
	}

    if (augMCL.enable == true)
	    windowObservationsUpdate();

	//Resample and propagate
	//Normalize Particles after resampling
	//Find the index of the max weight in the process
	if(weightsChanged){
		RouletteResampleAndNormalize(Observations);
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

void KLocalization::windowObservationsUpdate(){
	KSystem::Time::TimeAbsolute now = KSystem::Time::SystemTime::now();

    if (windowObservations.size()>0 && windowObservations.front().observationTime < now - seconds(augMCL.winDuration))
        windowObservations.clear();
}

void KLocalization::Predict(Localization::KMotionModel & MotionModel)
{
	float tmpDist, tmpDir, tmpRot;
	boost::normal_distribution<> normDist(0,1);
    boost::variate_generator<randGen&, boost::normal_distribution<> > X(generator, normDist);

    //Move the particles with noise
	for (int i = 0; i < SIRParticles.size; i++)
	{
		tmpDist = MotionModel.Distance.val * ( 1.2 + X() * MotionModel.Distance.ratiodev);

		tmpDir = MotionModel.Direction.val * ( 1 + X() * MotionModel.Direction.ratiodev);

		tmpRot = MotionModel.Rotation.val  * ( 1 + X() * MotionModel.Rotation.ratiodev);

        SIRParticles.x[i] = SIRParticles.x[i] + cos(tmpDir + SIRParticles.phi[i]) * tmpDist;
		SIRParticles.y[i] = SIRParticles.y[i] + sin(tmpDir + SIRParticles.phi[i]) * tmpDist;
		SIRParticles.phi[i] = wrapTo2Pi(SIRParticles.phi[i] + tmpRot + actionOdError);
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

	int step = round((float) SIRParticles.size / locConfig->numberOfParticlesSpreadAfterFall);

	if (locConfig->numberOfParticlesSpreadAfterFall == 0)
		return;

	for (int i = 0; i < SIRParticles.size; i += step)
	{
		SIRParticles.x[i] += (X() - 0.5) * locConfig->spreadParticlesDeviationAfterFall;
		SIRParticles.y[i] += (X() - 0.5) * locConfig->spreadParticlesDeviationAfterFall;
		SIRParticles.phi[i] += (X() - 0.5) * TO_RAD(locConfig->rotationDeviationAfterFallInDeg);
	}
}

void KLocalization::Update(vector<Localization::KObservationModel> &Observation, int NumofParticles)
{
	//Function to update the weights of each particle regarding the ObservationDistance from an object and the direction
	float OverallWeightOwnField, OverallWeightEnemyField, OverallWeightTotal;
	float ParticlePointBearingAngle, ParticleBearing;
	float R;
	float weightSum = 0.0f;

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


        if (Observation.size()==1){
            weightSum+= (OverallWeightOwnField > OverallWeightEnemyField) ? OverallWeightOwnField : OverallWeightEnemyField ;
        }

		OverallWeightTotal = (OverallWeightTotal < 0.0001) ? 0.0001 : OverallWeightTotal;
		SIRParticles.Weight[p] = OverallWeightTotal;
	}

    if (Observation.size()==1)
	    updateLikelihoodHist(weightSum);

}
void KLocalization::UpdateAmbiguous(vector<Localization::KObservationModel> &Observation, int NumofParticles)
{
	//Function to update the weights of each particle regarding the ObservationDistance from an object and the direction
	float ParticlePointBearingAngle, ParticleBearing;
	float R;
	float xPosOfFeature 	= Observation[0].Feature.x;
	float yPosOfFeature 	= Observation[0].Feature.y;
	float obsDistEmean	= Observation[0].Distance.Emean;
	float obsDistEdev 	= Observation[0].Distance.Edev;
	float obsDistValue	= Observation[0].Distance.val;
	float obsBearingEdev 	= Observation[0].Bearing.Edev;
	float obsBearingValue 	= Observation[0].Bearing.val;

    float weightSum = 0.0f;
    float overallWeight, weight;

	//Find the best candiate for the landmark
	for (int p = 0; p < NumofParticles; p++)
	{
		overallWeight = 1;

		//Enemy Left
		R = norm2(SIRParticles.x[p] - xPosOfFeature, SIRParticles.y[p] - yPosOfFeature);
		overallWeight *= NormPdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(yPosOfFeature - SIRParticles.y[p], xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		overallWeight *= NormPdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
		weight = overallWeight;

		overallWeight = 1;
		//Enemy Right
		R = norm2(SIRParticles.x[p] - xPosOfFeature, SIRParticles.y[p] - (-yPosOfFeature));
		overallWeight *= NormPdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2((-yPosOfFeature) - SIRParticles.y[p], xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		overallWeight *= NormPdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
        weight = (overallWeight > weight ) ? overallWeight : weight;

		overallWeight = 1;
		//Own Left
		R = norm2(SIRParticles.x[p] - (-xPosOfFeature), SIRParticles.y[p] - (-yPosOfFeature));
		overallWeight *= NormPdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-yPosOfFeature - SIRParticles.y[p], -xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		overallWeight *= NormPdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);
        weight = (overallWeight > weight ) ? overallWeight : weight;

		overallWeight = 1;
		//Own Right
		R = norm2(SIRParticles.x[p] - (-xPosOfFeature), SIRParticles.y[p] - (-(-yPosOfFeature)));
		overallWeight *= NormPdf((obsDistValue - obsDistEmean) - R, obsDistEdev);
		ParticlePointBearingAngle = atan2(-(-yPosOfFeature) - SIRParticles.y[p], -xPosOfFeature - SIRParticles.x[p]);
		ParticleBearing = anglediff2(ParticlePointBearingAngle, SIRParticles.phi[p]);
		overallWeight *= NormPdf(anglediff(obsBearingValue, ParticleBearing), obsBearingEdev);

        weight = (overallWeight > weight ) ? overallWeight : weight;
        weightSum+=weight;

        weight = (weight < 0.0001) ? 0.0001 : weight;
		SIRParticles.Weight[p] = weight;
	}

    //augmented MCL update short and long likelihood
	updateLikelihoodHist(weightSum);
}

Localization::belief KLocalization::ComputeAvg(){
    Localization::belief agentPos;
    float aCos = 0.0f ,aSin = 0.0f;

    agentPos.x = 0.0f;
    agentPos.y = 0.0f;
    agentPos.phi = 0.0f;

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

void KLocalization::updateLikelihoodHist(float weightSum){
	if (augMCL.enable == true){
		if (augMCL.firstUpdate == true)
		{
			augMCL.firstUpdate = false;
			augMCL.avgLikelihood = weightSum/SIRParticles.size;
			augMCL.shortHist = augMCL.avgLikelihood;
			augMCL.longHist = augMCL.avgLikelihood;
		}
		else{
			augMCL.avgLikelihood = weightSum/SIRParticles.size;
			augMCL.shortHist = augMCL.shortHist+augMCL.afast*(augMCL.avgLikelihood-augMCL.shortHist);
			augMCL.longHist = augMCL.longHist+augMCL.aslow*(augMCL.avgLikelihood-augMCL.longHist);
		}
        //cout << "Long-Hisory average" << augMCL.longHist << endl;
        //cout << "Short-Hisory average" << augMCL.shortHist << endl;
	}
}

void KLocalization::RouletteResampleAndNormalize(vector<Localization::KObservationModel>& Observations)
{
    //uniform [0,1)
    boost::uniform_real<> uniDist(0,1);
    boost::variate_generator<randGen&, boost::uniform_real<> > X(generator, uniDist);

	float preset;
	float sum = 0.0f;
	float maxWeight = -1;
	for (int i = 0; i < SIRParticles.size; i++){
		sum += SIRParticles.Weight[i];
	}

	float r = X() * sum;
	r = r / SIRParticles.size;
	float cumSum = SIRParticles.Weight[0];
	float step = sum / ((float)SIRParticles.size);
	int i = 0;
	float tempX[SIRParticles.size];
	float tempY[SIRParticles.size];
	float tempPhi[SIRParticles.size];
	Localization::partcl genParticle;

	preset=max(0.0f, 0.5f * (1.0f - augMCL.shortHist/augMCL.longHist) );
    if (Observations.size()==2 && augMCL.enable == true ){
        genParticle = generateParticle( Observations);
    }
    else if (windowObservations.size()>1 && augMCL.enable == true ){
        genParticle = generateParticleWindow(windowObservations);
    }

	for (int m = 0; m < SIRParticles.size; m++)
	{
		//float u = r + 1.0*m/nsamples;
		while (r > cumSum)
		{
			i++;
			cumSum += SIRParticles.Weight[i];
		}

		//find max weight particle index
		if(SIRParticles.Weight[i] > maxWeight)
		{
			maxWeight = SIRParticles.Weight[i];
			maxWeightParticleIndex = m;
		}

		r += step;

        if (Observations.size()==2  && X() < preset && augMCL.enable== true && genParticle.valid == true){
            //cout << "Particle Generated from Unumbiguous Observation" << endl;
            tempX[m] = genParticle.x + X() * 0.1;
			tempY[m] = genParticle.y + X() * 0.1;
			tempPhi[m] = genParticle.phi + X() * 0.1;
        }
        else if (windowObservations.size()>1 && X() < preset && augMCL.enable == true && genParticle.valid == true){
            //cout << "Particle Generated from Umbiguous Observation" << endl;
            tempX[m] = genParticle.x + X() * 0.1;
			tempY[m] = genParticle.y + X() * 0.1;
			tempPhi[m] = genParticle.phi + X() * 0.1;
		}
		else{
			tempX[m] = SIRParticles.x[i];
			tempY[m] = SIRParticles.y[i];
			tempPhi[m] = SIRParticles.phi[i];
		}
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

vector<float> KLocalization::circleIntersection (Localization::KObservationModel& obs1 ,Localization::KObservationModel& obs2){
	float p2x,p2y,p1x,p1y,p3x,p3y;
    float bearing1,bearing2,radius1,radius2;
	float h;

    //Circles have two intersection points
    vector<float> intersPoint1(2),intersPoint2(2);

    //Goal post size
	float centreDist=fabs(obs1.Feature.y-obs2.Feature.y);

    //Circle radius
    radius1 = obs1.Distance.val;
	radius2 = obs2.Distance.val;

    //Circle centre
    p1x = obs1.Feature.x;
    p1y = obs1.Feature.y;

    //Circle centre
    p2x = obs2.Feature.x;
    p2y = obs2.Feature.y;

    //Goal posts bearing
    bearing1 = obs1.Bearing.val;
    bearing2 = obs2.Bearing.val;

    float alpha = ( pow(radius1,2) - pow(radius2,2) + pow(centreDist,2) ) / ( 2 * centreDist);
	h = sqrt( pow(radius1,2) - pow(alpha,2) );

    p3x = p1x + alpha * (p2x - p1x) / centreDist;
	p3y = p1y + alpha * (p2y - p1y) / centreDist;

    //x coordinate
	intersPoint1[0] = p3x + h *(p2y - p1y) / centreDist;
    //y coordinate
	intersPoint1[1] = p3y - h *(p2x - p1x) / centreDist;

    //x coordinate
	intersPoint2[0] = p3x - h *(p2y - p1y) / centreDist;
    //y coordinate
	intersPoint2[1] = p3y + h *(p2x - p1x) / centreDist;

    return (intersPoint1[0] > 0 && intersPoint1[0] < 3) ? intersPoint1 : intersPoint2;
}

Localization::partcl KLocalization::generateParticle(vector<Localization::KObservationModel>& Observations){

	Localization::partcl pEnemyField,pOwnField,result;
	float R1,R2,Threshold = 1;
    vector<float> intersPoint(2);

	float centreDist = fabs(locConfig->KFeaturesmap["YellowLeft"].y - locConfig->KFeaturesmap["YellowRight"].y);

    if (centreDist < fabs(Observations[0].Distance.val-Observations[1].Distance.val) ){
        //No solution
        //cout<< "No solution..." << endl;
	    result.valid = false;
	    return result;
    }

    //Position based on two observations
    intersPoint = circleIntersection(Observations[0],Observations[1]);

    //Possible robot position
	pEnemyField.x = intersPoint[0];
	pEnemyField.y = intersPoint[1];
	pEnemyField.phi = anglediff2(atan2(Observations[0].Feature.y-pEnemyField.y,Observations[0].Feature.x-pEnemyField.x),Observations[0].Bearing.val);

    //The symmetric one
    pOwnField.x = -pEnemyField.x;
  	pOwnField.y = -pEnemyField.y;
	pOwnField.phi = pEnemyField.phi + TO_RAD(180);

	//Compare with the current position
	R1 = norm2(agentPosition.x-pEnemyField.x,agentPosition.y-pEnemyField.y);
	R2 = norm2(agentPosition.x-pOwnField.x,agentPosition.y-pOwnField.y);

	if ( fabs(pOwnField.x)>locConfig->fieldMaxX || fabs(pOwnField.y)>locConfig->fieldMaxY ){
		//No solution (Out of the Field)
        //cout << "No solution (Out of the Field) ..." << endl;
	    result.valid = false;
		return result;
	}
    else if (R1 + Threshold < R2 ){
        //cout<< "Looking at enemy's goalposts" << endl;
		result = pEnemyField;
	    result.valid = true;
		return result;
	}
	else if (R2 + Threshold < R1){
        //cout<< "Looking at our goalposts" << endl;
		result = pOwnField;
	    result.valid = true;
		return result;
	}
	else{
		//Failed to generate! Risky
        //cout << "Failed to generate! Risky ..." << endl;
	    result.valid = false;
		return result;
	}
}


Localization::partcl KLocalization::generateParticleWindow(vector<Localization::KObservationModel>& Observations){
	boost::uniform_int<> uniInt(0,Observations.size()-1);
	boost::variate_generator<randGen&, boost::uniform_int<> > Y(generator,uniInt);

    vector<float> intersPoint(2);

	Localization::KObservationModel goalPost1;
	Localization::KObservationModel goalPost2;

	int tries = 0;
	float centreDist = fabs(locConfig->KFeaturesmap["YellowLeft"].y - locConfig->KFeaturesmap["YellowRight"].y);
    //cout << "centreDist : " << centreDist << endl;

    float phi1,phi2;
	Localization::partcl pEnemyField,pOwnField,result;
	float R1,R2,Threshold = 2;

	while ( tries < 3){
        //cout << "Try number : " << tries << endl;
   		goalPost1 = Observations[Y()];
		goalPost2 = Observations[Y()];

        if (centreDist > fabs(goalPost1.Distance.val-goalPost2.Distance.val) ){
            //Pick two different goal posts
		    while(goalPost1.observationTime==goalPost2.observationTime){
			    goalPost2 =  Observations[Y()];
                //cout << "Pick another pair " << endl;
            }

		    //GoalPost1 is left
		    if (wrapToPi(goalPost1.Bearing.val) > wrapToPi(goalPost2.Bearing.val)){
                goalPost1.Feature.x = locConfig->KFeaturesmap["YellowLeft"].x;
			    goalPost1.Feature.y = locConfig->KFeaturesmap["YellowLeft"].y;

			    goalPost2.Feature.x = locConfig->KFeaturesmap["YellowRight"].x;
			    goalPost2.Feature.y = locConfig->KFeaturesmap["YellowRight"].y;
		    }
		    //GoalPost1 is right
		    else{
			    goalPost1.Feature.x = locConfig->KFeaturesmap["YellowRight"].x;
			    goalPost1.Feature.y = locConfig->KFeaturesmap["YellowRight"].y;

                goalPost2.Feature.x = locConfig->KFeaturesmap["YellowLeft"].x;
			    goalPost2.Feature.y = locConfig->KFeaturesmap["YellowLeft"].y;
		    }

            //Position based on two observations
            intersPoint = circleIntersection(goalPost1,goalPost2);

            //Possible robot position
	        pEnemyField.x = intersPoint[0];
	        pEnemyField.y = intersPoint[1];

            //Check if position is consistent with observations
            phi1 = anglediff2(atan2(goalPost1.Feature.y-pEnemyField.y,goalPost1.Feature.x-pEnemyField.x),goalPost1.Bearing.val);
		    phi2 = anglediff2(atan2(goalPost2.Feature.y-pEnemyField.y,goalPost2.Feature.x-pEnemyField.x),goalPost2.Bearing.val);

		    if ( fabs(anglediff2(phi1,phi2)) < TO_RAD(5)){
			    pEnemyField.phi=phi1;

			    pOwnField.x = -pEnemyField.x;
			    pOwnField.y = -pEnemyField.y;
			    pOwnField.phi = pEnemyField.phi + TO_RAD(180);

			    R1 = norm2(agentPosition.x-pEnemyField.x,agentPosition.y-pEnemyField.y);
			    R2 = norm2(agentPosition.x-pOwnField.x,agentPosition.y-pOwnField.y);

	            if ( fabs(pOwnField.x)>locConfig->fieldMaxX || fabs(pOwnField.y)>locConfig->fieldMaxY ){
				    //No solution! (out of the field)
                    //cout << "No solution (Out of the Field) ..." << endl;
				    result.valid = false;
				    return result;
			    }
			    else if (R1 + Threshold <R2 ){
				    result.valid = true;
				    result = pEnemyField;
				    return result;
			    }
			    else if (R2 + Threshold <R1){
                    //cout<< "Looking at our goalposts" << endl;
				    result.valid = true;
				    result = pOwnField;
				    return result;
			    }
			    else{
                    //cout << "No solution (Risky) ..." << endl;
				    result.valid = false;
				    return result;
			    }
		    }
            else{
                tries++;
            }
        }
        else{
            tries++;
        }
	}

    //Failed to generate particle
	result.valid = false;
	return result;
}
