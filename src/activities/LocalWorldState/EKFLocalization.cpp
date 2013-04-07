#include "EKFLocalization.h"


void EKFLocalization::Initialize(){
    actionOdError=0.0f;

    // Error Parameters
    e1 = 1.2f;
    e2 = 0.0f;
    e3 = 1.0f;

    kalmanModels.resize(16);
	InitializeHypothesis(LocalizationResetMessage::UNIFORM, false, 0, 0, 0);
    lastPrint = boost::posix_time::microsec_clock::universal_time();
}


void EKFLocalization::InitializeHypothesis(int resetType, bool kickOff, float inX, float inY, float inPhi)
{
	if(resetType == LocalizationResetMessage::PENALISED || resetType == LocalizationResetMessage::UNIFORM){
        numberOfModels = 3;
        kalmanModels[0].Initialize(locConfig->fieldMinX/2, locConfig->fieldMinY, TO_RAD(90), e1, e2, e3, 1 , true );
        kalmanModels[1].Initialize(locConfig->fieldMinX/2, locConfig->fieldMaxY, TO_RAD(270), e1, e2, e3, 1 , true );

        float setPositionX =  locConfig->initX[(kickOff) ? 0 : 1] ;
        float setPositionY =  locConfig->initY[(kickOff) ? 0 : 1] ;
	    float setPositionPhi = locConfig->initPhi[(kickOff) ? 0 : 1];        

        kalmanModels[2].Initialize(setPositionX,setPositionY,setPositionPhi, e1, e2, e3, 1 , true );
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

            if (Observations[0].Feature.id.compare("YellowLeft")==0){
                kalmanModels[i].associationHistory.push_back(1);
                kalmanModels[i+numberOfModels].associationHistory.push_back(4);
            }
            else{
                kalmanModels[i].associationHistory.push_back(2);
                kalmanModels[i+numberOfModels].associationHistory.push_back(3);
            }
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

                if (Observations[1].Feature.id.compare("YellowLeft")==0){
                    kalmanModels[i].associationHistory.push_back(1);
                    kalmanModels[i+numberOfModels].associationHistory.push_back(4);
                }
                else{
                    kalmanModels[i].associationHistory.push_back(2);
                    kalmanModels[i+numberOfModels].associationHistory.push_back(3);
                }

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
        distanceDev = 2;
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

            kalmanModels[i].associationHistory.push_back(1);
            kalmanModels[numberOfModels + 3*i].associationHistory.push_back(3);
            kalmanModels[numberOfModels + 3*i + 1].associationHistory.push_back(2);
            kalmanModels[numberOfModels + 3*i + 2].associationHistory.push_back(4);
        }
         numberOfModels *= 4 ;
    }

    if ( change == true) {

        sort (kalmanModels.begin(), kalmanModels.begin()+numberOfModels);

        // Merge hypothesis representing the same belief
//        if (numberOfModels>1 && kalmanModels[0].associationHistory.size()==2)
//           hypothesisMerging();

        float sumWeight = 0;

        if ( numberOfModels > 4)
            numberOfModels = 4;

        for(int i = 0;i<numberOfModels; i++) {
                sumWeight += kalmanModels[i].mWeight;
        }

        for(int i = 0; i < numberOfModels; i++) {
            kalmanModels[i].mWeight /= sumWeight;
        }
    }

   if (boost::posix_time::microsec_clock::universal_time() > lastPrint + seconds(5)){
       Logger::Instance().WriteMsg("EKFLocalization", "Distance scale factor : " + _toString(kalmanModels[0].state(3,0)) + " Drift :  " + _toString(kalmanModels[0].state(4,0)) + "Rotation scale factor : " + _toString(kalmanModels[0].state(5,0)), Logger::Info);
        lastPrint = boost::posix_time::microsec_clock::universal_time();

        Logger::Instance().WriteMsg("EKFLocalization", "Distance scale factor variance : " + _toString(kalmanModels[0].var(3,3)) + " Drift variance:  " + _toString(kalmanModels[0].var(4,4)) + "Rotation scale factor variance: " + _toString(kalmanModels[0].var(5,5)), Logger::Info);
        lastPrint = boost::posix_time::microsec_clock::universal_time();
    }

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

void EKFLocalization::hypothesisMerging(){

    vector<KalmanModel> kalmanTmp;
    int count = 0;


    for(int i=0; i<numberOfModels; i++) {
        if (kalmanModels[i].active==true){
            kalmanTmp.push_back(kalmanModels[i]);
            count ++ ;
            for(int j=i+1; j<numberOfModels; j++) {
                if ( kalmanModels[j].active == true &&
                        kalmanModels[i].associationHistory[0] == kalmanModels[j].associationHistory[0] &&
                        kalmanModels[i].associationHistory[1] == kalmanModels[j].associationHistory[1]) {

                    kalmanModels[j].active=false;
                    kalmanTmp.back().mWeight+=kalmanModels[j].mWeight;
                }
            }
        }
    }
    numberOfModels=count;
    for (int i=0;i<count;i++)
        kalmanModels[i] = kalmanTmp[i];
}
