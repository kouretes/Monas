#include "KalmanModel.h"

KalmanModel::KalmanModel() : associationHistory(2){
    
}

void KalmanModel::Initialize(float x, float y, float phi, float e1, float e2, float e3, float w , bool a){

    active = a;
    // State initialization  
    state.zero();  
    state(0,0) = x;
    state(1,0) = y;
    state(2,0) = phi;
    state(3,0) = e1;
    state(4,0) = e2;
    state(5,0) = e3;
    mWeight = w;

    // Variance initialization
    var.zero();

    var(0,0) = 0.4 * 0.4;
    var(1,1) = 0.1 * 0.1;
    var(2,2) = TO_RAD(10) * TO_RAD(10);

    var(3,3) = 0.025;
    var(4,4) = 0.025;
    var(5,5) = 0.025;
}

void KalmanModel::Predict(Localization::KMotionModel & MotionModel){
	float tmpDist, tmpDir, tmpRot;

	tmpDir = MotionModel.Direction.val;
    tmpDist = MotionModel.Distance.val;
    tmpRot = MotionModel.Rotation.val;
      
    matrix6_6 gt;
    matrix6_3 vt;
    matrix3_3 mt;  
    matrix6_6 fict;

    fict.zero();

    // Small amount of noise for error parameters
    fict(0,0) = 4e-6;
    fict(1,1) = 4e-6;
    fict(2,2) = 4e-6;
    fict(3,3) = tmpDist  * tmpDist * 0.02;
    fict(4,4) = tmpDist  * tmpDist * 0.02;
    fict(5,5) = tmpDist  * tmpDist * 0.002; 

    gt.zero();
    mt.zero();
    vt.zero();

    // Linearizing odometry model
    gt(0,0) = 1;
    gt(0,1) = 0;
    gt(0,2) = - state(3,0) * tmpDist * sin(state(2,0) + tmpDir);

    gt(0,3) =  tmpDist * cos(state(2,0) + tmpDir);
    gt(0,4) = 0;
    gt(0,5) = 0;

    gt(1,0) = 0;
    gt(1,1) = 1;
    gt(1,2) =  state(3,0) * tmpDist * cos(state(2,0) + tmpDir);
    
    gt(1,3) =  tmpDist * sin(state(2,0) + tmpDir);
    gt(1,4) = 0;
    gt(1,5) = 0;

    gt(2,0) = 0;
    gt(2,1) = 0;
    gt(2,2) = 1;
    gt(2,3) = 0;
    gt(2,4) = tmpDist;
    gt(2,5) = tmpRot;

    /////////////////////////////////
    gt(3,0) = 0;
    gt(3,1) = 0;
    gt(3,2) = 0;
    gt(3,3) = 1;
    gt(3,4) = 0;
    gt(3,5) = 0;

    gt(4,0) = 0;
    gt(4,1) = 0;
    gt(4,2) = 0;
    gt(4,3) = 0;
    gt(4,4) = 1;
    gt(4,5) = 0;

    gt(5,0) = 0;
    gt(5,1) = 0;
    gt(5,2) = 0;
    gt(5,3) = 0;
    gt(5,4) = 0;
    gt(5,5) = 1;

    //gt.prettyPrint();

    mt(0,0) = tmpDist   * tmpDist *  0.6 *  0.6;
    mt(1,1) = tmpDist   * tmpDist *  0.6 *  0.6;
    mt(2,2) = tmpRot  * tmpRot * 0.3 * 0.3 +  tmpDist * tmpDist * 0.1 ;

    //mt.prettyPrint()
    vt(0,0) = - state(3,0) * tmpDist * sin(state(2,0) + tmpDir);
    vt(0,1) = state(3,0) * cos(state(2,0) + tmpDir); 
    vt(0,2) = 0; 
    
    vt(1,0) = state(3,0) * tmpDist * cos(state(2,0) + tmpDir);
    vt(1,1) = state(3,0) * sin(state(2,0) + tmpDir); 
    vt(1,2) = 0;

    vt(2,0) = 0;
    vt(2,1) = state(4,0); 
    vt(2,2) = state(5,0);

    vt(3,0) = 0;
    vt(3,1) = 0; 
    vt(3,2) = 0;

    vt(4,0) = 0;
    vt(4,1) = 0; 
    vt(4,2) = 0;

    vt(5,0) = 0;
    vt(5,1) = 0; 
    vt(5,2) = 0;

    //vt.prettyPrint();

    //Compute new state and variance

    state(0,0) += cos(tmpDir + state(2,0)) * tmpDist *  state(3,0) ;
    state(1,0) += sin(tmpDir + state(2,0)) * tmpDist *  state(3,0);

    state(2,0) += state(5,0) * tmpRot + state(4,0) * tmpDist;
    state(2,0) = wrapTo0_2Pi(state(2,0));

    //odometry.prettyPrint();
    //state.prettyPrint();

    var =(((gt.slow_mult(var)).slow_mult(gt.transp())).add((vt.slow_mult(mt)).slow_mult(vt.transp()))).add(fict);
    //var.prettyPrint();
}


void KalmanModel::Update(float featureX,float featureY,float distanceVal,float bearingVal,float distanceDev,float bearingDev){
    
    float q;
    matrix6_6 identityM;

    float observationDistance;
    float observationBearing;

    matrix2_2 st;
    matrix6_2 kt;
    matrix2_1 obs;
    matrix2_6 ht;

    matrix2_2 qt,stInv;
    matrix2_1 meas;
    matrix2_1 diff;

    stInv.zero();
    // Observation variance
    qt.zero();
    qt(0,0) = distanceDev * distanceDev;
    

    qt(1,1) = bearingDev  *  bearingDev;

    //qt.prettyPrint();

    // Observation distance and bearing
    obs.zero();
    obs(0,0) = distanceVal;
    obs(1,0) = bearingVal;

    //obs.prettyPrint();
    
    diff.zero();
    st.zero();
    kt.zero();
    meas.zero();

    ht.zero();

    q = pow( state(0,0) - featureX, 2 ) + pow( ( state(1,0) - featureY ), 2);
      
    observationDistance = sqrt(q);
    //cout << "expected distance" << observationDistance << endl;

    observationBearing = anglediff2(atan2(featureY - state(1,0) ,featureX - state(0,0)),state(2,0));
    //cout << "expected bearing" << observationBearing << endl;

    meas(0,0) = observationDistance;
    meas(1,0) = observationBearing;

    //meas.prettyPrint();

    ht(0,0) = - ( featureX - state(0,0) ) / sqrt(q);
    ht(0,1) = - ( featureY - state(1,0) ) / sqrt(q);
    ht(0,2) = 0;
    ht(0,3) = 0;
    ht(0,4) = 0;
    ht(0,5) = 0;
    ht(1,0) = ( featureY - state(1,0) ) / q;
    ht(1,1) = - ( featureX - state(0,0) ) / q;
    ht(1,2) = -1;
    ht(1,3) = 0;
    ht(1,4) = 0;
    ht(1,5) = 0;
    
    //ht.prettyPrint();

    st = ((ht.slow_mult(var)).slow_mult(ht.transp())).add(qt);

    //st.prettyPrint();
    stInv = st;
    invert_square_matrix(stInv);

    diff(0,0) = obs(0,0) - meas(0,0);
    diff(1,0) = anglediff2(obs(1,0),meas(1,0));
   
    //diff.prettyPrint();
    kt = (var.slow_mult(ht.transp())).slow_mult(stInv);

    state = state.add(kt.slow_mult(diff));
    var = ((identityM.identity().sub(kt.slow_mult(ht)))).slow_mult(var);
  
    double w = 1.0/(2*M_PI) * 1/sqrt((st(0,0) * st(1,1)-st(0,1)*st(1,0))) * exp(-1.0/2 *(diff.transp()).slow_mult(stInv).slow_mult(diff));

    mWeight *=  w;     
}
