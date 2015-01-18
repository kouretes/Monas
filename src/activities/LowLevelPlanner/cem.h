#ifndef CEM_H
#define CEM_H

#include <cmath>
#include "core/elements/math/KMat.hpp"
#include "Dynamics.h"
#include <vector>
#include "CircularBuffer.hpp"

#define CEM_N  10 //Number of time steps per rollout
#define CEM_M  3  //Number of paramaters
#define CEM_S  3  //Number of state Variables
#define AXIS 2

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/preprocessor/arithmetic/div.hpp>
using namespace std;

typedef KMath::KMat::GenMatrix<float, CEM_M, 1> GMx1_t;
typedef KMath::KMat::GenMatrix<float, CEM_M, CEM_M> GMxM_t;
typedef KMath::KMat::GenMatrix<float, CEM_N, 1> GNx1_t;

//typedef KMath::KMat::GenMatrix<float, CEM_K,CEM_N> GKxN_t;
typedef KMath::KMat::GenMatrix<float, CEM_S + 1, 1> GSx1_t;

#define KPROFILING_ENABLED
#include "core/architecture/time/Profiler.hpp"

typedef struct cemconfig {
	static float R_val;

	GMx1_t theta;
	GMxM_t cov;
	GMxM_t Q; //control_cost R
	GMxM_t R; //control_cost R
	GMxM_t R_inv; //R^-1
	static int l;
	static int K_e;
	static int K; //Number of rolluts per update
	float cost;

} cemconfig_t;

typedef struct rollout_result {
	float S; //cost
	GMx1_t e; //noise
	rollout_result() {
		S = -1;
		e.zero();
	}
	;

	rollout_result(float k, GMx1_t& s) :
			S(k), e(s) {
	}
	;

	bool operator <(const rollout_result other) const {
		return (S < other.S);
	}
	void print() {
		std::cout << "cost: " << S << std::endl;
		//std::cout << "noise: ";
		//e.prettyPrint();
	}

} rollout_result_t;

class cem {
public:
	vector<float> sumGt;
	float * P;

	cem(RobotParameters robot);
	void init_cem();
	void calculate_action(float & ux, float &uy, Dynamics Dx, Dynamics Dy,
			CircularBuffer<KVecFloat3> & ZmpBuffer);
	virtual ~cem();
	mutable KProfiling::profiler cemprof;
	cemconfig_t cemconfig[AXIS];

private:
	vector<rollout_result_t> rollouts;

	float denom[CEM_N];
	KMath::KMat::GenMatrix<float, CEM_N, 1> ZMPReferenceX, ZMPReferenceY;

	boost::mt19937 eng;
	boost::normal_distribution<float> dist;
	boost::variate_generator<boost::mt19937, boost::normal_distribution<float> > gen;
	Dynamics rolloutSys;

	float Gaussian(float t, float cm, float sm);

	vector<float> GaussianV(vector<float> t, float cm, float sm);

	vector<double> normalizedG(float t, vector<float> cm, vector<float> sm);


	GMx1_t ng(float t, vector<float> centers, vector<float> sigma);

	void run_rollouts(vector<rollout_result_t> & rolls, GSx1_t init_state,
			Dynamics & sys, cemconfig_t & config, GNx1_t Zref, GMxM_t L);

	bool cem_update(vector<rollout_result_t> & rolls, Dynamics & sys,
			cemconfig_t & config, float converge_value, float Zref);
	void run_rollouts_and_update(Dynamics & sys,
			cemconfig_t & config, float converge_value, GNx1_t Zref);
	float calculate_cost(Dynamics & sys, cemconfig_t & config,GMx1_t theta_k, float Zref);
};

#endif // CEM_H
