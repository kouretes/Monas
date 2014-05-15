#include "EKFLocalization.h"
#include "core/include/Logger.hpp"

void EKFLocalization::Initialize(){
    actionOdError=0.0f;

    // Error Parameters
    e1 = 1.2f;
    e2 = 0.2f;
    e3 = 1.f;

    kalmanModels.resize(32);
	InitializeHypothesis(LocalizationResetMessage::UNIFORM, false, 0, 0, 0);
    lastPrint = KSystem::Time::SystemTime::now();
}

void EKFLocalization::InitializeHypothesis(int resetType, bool kickOff, float inX, float inY, float inPhi)
{
	if(resetType == LocalizationResetMessage::PENALISED || resetType == LocalizationResetMessage::UNIFORM){
        numberOfModels = 3;
        kalmanModels[1].Initialize(locConfig->fieldMinX/2, locConfig->fieldMinY, TO_RAD(90), e1, e2, e3, 1.0/3 , true );
        kalmanModels[2].Initialize(locConfig->fieldMinX/2, locConfig->fieldMaxY, TO_RAD(270), e1, e2, e3, 1.0/3 , true );

        float setPositionX =  locConfig->initX[(kickOff) ? 0 : 1] ;
        float setPositionY =  locConfig->initY[(kickOff) ? 0 : 1] ;
	    float setPositionPhi = locConfig->initPhi[(kickOff) ? 0 : 1];

        kalmanModels[0].Initialize(setPositionX,setPositionY,setPositionPhi, e1, e2, e3, 1.0/3 , true );
	}
	else if(resetType == LocalizationResetMessage::READY){
        numberOfModels = 1;
		kalmanModels[0].Initialize(locConfig->readyX,locConfig->readyY,locConfig->readyPhi, e1, e2, e3, 1 , true );
	}
	else if(resetType == LocalizationResetMessage::SET){
        numberOfModels = 1;
        float setPositionX =  locConfig->initX[(kickOff) ? 0 : 1] ;
        float setPositionY =  locConfig->initY[(kickOff) ? 0 : 1] ;
	    float setPositionPhi = locConfig->initPhi[(kickOff) ? 0 : 1];

		kalmanModels[0].Initialize(setPositionX, setPositionY, setPositionPhi, e1, e2, e3, 1 , true );
	}else if(resetType == LocalizationResetMessage::MANUAL){
        numberOfModels = 1;
		kalmanModels[0].Initialize(inX, inY, inPhi, e1, e2, e3, 1 , true );
	}else if(resetType == LocalizationResetMessage::PENALTY_MODE){
        numberOfModels = 1;
		kalmanModels[0].Initialize(0, 0, 0, e1, e2, e3, 1 , true );
	}
}

Localization::blf EKFLocalization::LocalizationStep(Localization::KMotionModel & MotionModel, vector<Localization::KObservationModel>& Observations, vector<Localization::KObservationModel>& AmbiguousObservations){

    float featureX;
    float featureY;
    float distanceVal;
    float bearingVal;
    float distanceDev;
    float bearingDev;
    bool change = false;

    if(MotionModel.freshData){
        for(int i=0; i<numberOfModels; i++) {
            kalmanModels[i].Predict(MotionModel);
        }
    }

    if(Observations.size() >= 1  )
    {

        change = true;
        featureX = Observations[0].Feature.x;
        featureY =  Observations[0].Feature.y;
        distanceVal = Observations[0].Distance.val;
        bearingVal = Observations[0].Bearing.val;
        distanceDev = Observations[0].Distance.Edev;
        bearingDev = Observations[0].Bearing.Edev;

        for(int i=0; i<numberOfModels; i++) {
            KalmanModel modelClone = kalmanModels[i];
            kalmanModels[i].Update(featureX,featureY,distanceVal,bearingVal,distanceDev,bearingDev);

            modelClone.Update(-featureX,-featureY,distanceVal,bearingVal,distanceDev,bearingDev);
            kalmanModels[i+numberOfModels] = modelClone;
        }

        if (Observations.size() == 2){
            featureX = Observations[1].Feature.x;
            featureY =  Observations[1].Feature.y;
            distanceVal = Observations[1].Distance.val;
            bearingVal = Observations[1].Bearing.val;
            distanceDev =Observations[1].Distance.Edev;
            bearingDev = Observations[1].Bearing.Edev;

            for(int i=0; i<numberOfModels; i++) {
                kalmanModels[i].Update(featureX,featureY,distanceVal,bearingVal,distanceDev,bearingDev);
                kalmanModels[i+numberOfModels].Update(-featureX,-featureY,distanceVal,bearingVal,distanceDev,bearingDev);
            }
        }
        numberOfModels *= 2;
    }

    else if(AmbiguousObservations.size() == 1 )
    {

        change = true;
        featureX = AmbiguousObservations[0].Feature.x;
        featureY =  AmbiguousObservations[0].Feature.y;
        distanceVal = AmbiguousObservations[0].Distance.val;
        bearingVal = AmbiguousObservations[0].Bearing.val;
        distanceDev = AmbiguousObservations[0].Distance.Edev;
        bearingDev = AmbiguousObservations[0].Bearing.Edev;

        for(int i=0; i<numberOfModels; i++) {

            KalmanModel modelClone1 = kalmanModels[i];
            KalmanModel modelClone2 = kalmanModels[i];
            KalmanModel modelClone3 = kalmanModels[i];

            kalmanModels[i].Update(featureX,featureY,distanceVal,bearingVal,distanceDev,bearingDev);
            modelClone1.Update(-featureX,featureY,distanceVal,bearingVal,distanceDev,bearingDev);
            modelClone2.Update(featureX,-featureY,distanceVal,bearingVal,distanceDev,bearingDev);
            modelClone3.Update(-featureX,-featureY,distanceVal,bearingVal,distanceDev,bearingDev);

            kalmanModels[numberOfModels + 3*i] = modelClone1;
            kalmanModels[numberOfModels + 3*i + 1] = modelClone2;
            kalmanModels[numberOfModels + 3*i + 2] = modelClone3;

        }
         numberOfModels *= 4 ;
    }

    if ( change == true) {

        sort (kalmanModels.begin(), kalmanModels.begin()+numberOfModels);

        float sumWeight = 0;

        for(int i = 0;i<numberOfModels; i++) {
                sumWeight += kalmanModels[i].mWeight;
        }

        for(int i = 0; i < numberOfModels; i++) {
            kalmanModels[i].mWeight /= sumWeight;
        }

        // Merge hypothesis representing the same belief
        double threshold = 0.7;
        double dt = 0.5;
        //cout << "new round" << endl;
        while( numberOfModels > 4 ){
            //cout << "step"<< endl;
            hypothesisMerging(threshold);
            //cout << "Number Of Models after merging : " << numberOfModels << endl;
            threshold += dt;
        }

    }

    /*
   if (KSystem::Time::SystemTime::now() > lastPrint + seconds(5)){
	   LogEntry(LogLevel::Info,"EKFLocalization")
				<<"Distance scale factor : " << (kalmanModels[0].state(3,0))
				<<" Drift :  " << (kalmanModels[0].state(4,0))
				<<"Rotation scale factor : " << (kalmanModels[0].state(5,0));
        lastPrint = KSystem::Time::SystemTime::now();
		LogEntry(LogLevel::Info,"EKFLocalization")
				<<"Distance scale factor variance : "<< (kalmanModels[0].var(3,3))
				<<" Drift variance:  " << (kalmanModels[0].var(4,4))
				<<"Rotation scale factor variance: " << (kalmanModels[0].var(5,5));
        lastPrint = KSystem::Time::SystemTime::now();
    }*/

    agentPosition.x = kalmanModels[0].state(0,0);
    agentPosition.y = kalmanModels[0].state(1,0);
    agentPosition.phi = kalmanModels[0].state(2,0);

    var = kalmanModels[0].var;

    //var.prettyPrint();
    return agentPosition;
}

void EKFLocalization::IncreaseUncertaintyAfterFall(){
    for(int i = 0; i < numberOfModels; i++) {
        kalmanModels[i].var(0,0) += 0.2;
        kalmanModels[i].var(1,1) += 0.2;
        kalmanModels[i].var(2,2) += 0.2;
    }
}

void EKFLocalization::hypothesisMerging(float thres){

    vector<KalmanModel> kalmanTmp;
    int numberOfM = 0;
    double k;

    KMath::KMat::GenMatrix<float,6,1> st1;
    KMath::KMat::GenMatrix<float,6,1> st2;
    KMath::KMat::GenMatrix<float,6,6> varInv;

    for(int i=0; i<numberOfModels; i++) {
        if (kalmanModels[i].active==true){
            kalmanTmp.push_back(kalmanModels[i]);
            numberOfM ++ ;

            varInv = kalmanModels[i].var;
            invert_square_matrix(varInv);

            for(int j=i+1; j<numberOfModels; j++) {

                st1 = kalmanModels[i].state;
                st2 = kalmanModels[j].state;

                st1.sub(st2);
                k = (kalmanModels[i].mWeight * kalmanModels[j].mWeight)/(kalmanModels[i].mWeight + kalmanModels[j].mWeight) * st1.transp().slow_mult(varInv).slow_mult(st1);

                if ( k < 0.1 * thres ){
                    kalmanModels[j].active=false;
                    kalmanTmp.back().mWeight+=kalmanModels[j].mWeight;
                }
            }
        }
    }
    numberOfModels=numberOfM;
    for (int i=0;i<numberOfModels;i++)
        kalmanModels[i] = kalmanTmp[i];
}
