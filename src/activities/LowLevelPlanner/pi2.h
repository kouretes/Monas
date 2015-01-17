#ifndef PI2_H
#define PI2_H

#include <cmath>
#include "core/elements/math/KMat.hpp"
#include "Dynamics.h"
#include <vector>
#include "CircularBuffer.hpp"

#define PI2_N  18
#define PI2_M  3
#define PI2_K  13
#define PI2_S  3
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
using namespace std;

typedef KMath::KMat::GenMatrix<float, PI2_M,1> GMx1_t;
typedef KMath::KMat::GenMatrix<float, PI2_M,PI2_M> GMxM_t;
typedef KMath::KMat::GenMatrix<float, PI2_N,1> GNx1_t;
typedef KMath::KMat::GenMatrix<float, PI2_K,PI2_N> GKxN_t;
typedef KMath::KMat::GenMatrix<float, PI2_S+1,1> GSx1_t;

#define KPROFILING_ENABLED
#include "core/architecture/time/Profiler.hpp"

typedef struct pi2config_
{
   /* int M; //Number of paramaters
    int N; //Number of time steps per rollout

    int K; //Number of rolluts per update
*/
    float R;
    float gama; //Decay rate of the exploration noise
    float sigme_e;
    KMath::KMat::GenMatrix<float, PI2_M, 1 > theta;
    KMath::KMat::GenMatrix<float, PI2_M , PI2_M> control_cost; //R
    KMath::KMat::GenMatrix<float, PI2_M , PI2_M> inv_control_cost; //R^-1

} pi2config_t;



class pi2
{
public:
    vector<float> centers, sigmas, sumGt;
    vector<GMxM_t > M;

    vector<GMx1_t > Gt;
    vector<GMx1_t > Gst;
    pi2(RobotParameters robot);
    void init_pi2();
    void calculate_action(float & ux, float &uy, Dynamics Dx, Dynamics Dy,CircularBuffer<KVecFloat3> & ZmpBuffer);
    virtual ~pi2();
	mutable KProfiling::profiler pi2prof;
	pi2config_t pi2config[2];
protected:
private:
	float denom[PI2_N];
	KMath::KMat::GenMatrix<float,PI2_N,1> ZMPReferenceX,ZMPReferenceY;

    vector<vector< GMx1_t> > Me;
    boost::mt19937 eng;
    boost::normal_distribution<float> dist ;
    boost::variate_generator<boost::mt19937,boost::normal_distribution<float> > gen;
    float Gaussian(float t,float cm,float sm);
    vector<float> GaussianV(vector<float> t,float cm,float sm);
    vector<double> normalizedG(float t, vector<float> cm, vector<float>sm);

    Dynamics rolloutSys;
    GMx1_t ng(float t,vector<float> centers ,vector<float> sigma);
    void run_rollouts(vector<vector< GMx1_t> >& Me, GKxN_t &q, GMx1_t theta, GSx1_t init_state,Dynamics & sys,GNx1_t Zref  , float expl_sigma );
    GMx1_t pi2_update(vector<vector< GMx1_t> >& Me, GKxN_t q, GMx1_t theta);

};

#endif // PI2_H
