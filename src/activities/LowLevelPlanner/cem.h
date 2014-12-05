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

typedef struct cemconfig
{
	static float R_val;

	GMx1_t theta;
	GMx1_t cov;
    GMxM_t Q; //control_cost R
    GMxM_t R; //control_cost R
    GMxM_t R_inv; //R^-1
    static int l;
    static int K_e;
    static int K; //Number of rolluts per update
    float cost;

} cemconfig_t;

struct rollout_result
{
    float S; //cost
    GMx1_t e; //noise

    rollout_result(float k, GMx1_t& s) : S(k), e(s) {}

    bool operator < (const rollout_result other) const
    {
    	return (S < other.S);
    }

}rollout_result_t;

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
    void  run_rollouts(vector<rollout_result_t> & rolls, GMx1_t theta, GSx1_t init_state, Dynamics & sys, cemconfig_t & config, GNx1_t Zref, GMxM_t L) ;
    //void run_rollouts(vector<vector< GMx1_t> >& Me, GKxN_t &q, GMx1_t theta, GSx1_t init_state,Dynamics & sys,GNx1_t Zref  , float expl_sigma );
    bool cem_update(vector<rollout_result_t> & rolls, Dynamics & sys, cemconfig_t & config, float converge_value);

};

#endif // cem_H
