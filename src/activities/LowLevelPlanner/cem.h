#ifndef cem_H
#define cem_H

#include <cmath>
#include "core/elements/math/KMat.hpp"
#include "Dynamics.h"
#include <vector>
#include "CircularBuffer.hpp"

#define cem_N  50 //Number of time steps per rollout
#define cem_M  2 //Number of paramaters
#define cem_S  2
#define cem_K =


#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/preprocessor/arithmetic/div.hpp>
using namespace std;

typedef KMath::KMat::GenMatrix<float, cem_M,1> GMx1_t;
typedef KMath::KMat::GenMatrix<float, cem_M,cem_M> GMxM_t;
typedef KMath::KMat::GenMatrix<float, cem_N,1> GNx1_t;


//typedef KMath::KMat::GenMatrix<float, cem_K,cem_N> GKxN_t;
typedef KMath::KMat::GenMatrix<float, cem_S+1,1> GSx1_t;

#define KPROFILING_ENABLED
#include "core/architecture/time/Profiler.hpp"

typedef struct cemconfig_
{
	static float R_val;

    KMath::KMat::GenMatrix<float, cem_M, 1 > theta;
    KMath::KMat::GenMatrix<float, cem_M , cem_M> Q; //control_cost R
    KMath::KMat::GenMatrix<float, cem_M , cem_M> R; //control_cost R
    KMath::KMat::GenMatrix<float, cem_M , cem_M> R_inv; //R^-1
    static int l;
    static int K_e;
    static int K; //Number of rolluts per update

} cemconfig_t;



class cem
{
public:
    //vector<float> centers, sigmas,
	vector<float>   sumGt;
	float P[];

    cem(RobotParameters robot);
    void init_cem();
    void calculate_action(float & ux, float &uy, Dynamics Dx, Dynamics Dy,CircularBuffer<KVecFloat3> & ZmpBuffer);
    virtual ~cem();
	mutable KProfiling::profiler cemprof;
	cemconfig_t cemconfig[2];
protected:
private:
	float denom[cem_N];
	KMath::KMat::GenMatrix<float,cem_N,1> ZMPReferenceX,ZMPReferenceY;

    boost::mt19937 eng;
    boost::normal_distribution<float> dist ;
    boost::variate_generator<boost::mt19937,boost::normal_distribution<float> > gen;
    float Gaussian(float t,float cm,float sm);
    vector<float> GaussianV(vector<float> t,float cm,float sm);
    vector<double> normalizedG(float t, vector<float> cm, vector<float>sm);

    Dynamics rolloutSys;
    GMx1_t ng(float t,vector<float> centers ,vector<float> sigma);
    //void run_rollouts(vector<vector< GMx1_t> >& Me, GKxN_t &q, GMx1_t theta, GSx1_t init_state,Dynamics & sys,GNx1_t Zref  , float expl_sigma );
    //GMx1_t cem_update(vector<vector< GMx1_t> >& Me, GKxN_t q, GMx1_t theta);

};

#endif // cem_H
