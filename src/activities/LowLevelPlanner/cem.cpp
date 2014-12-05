#include "cem.h"
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <ctime>
#include <iostream>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator
#include <vector>
using namespace std;
#define printfVector(x) std::cout << "fVector: " << #x << "  "; printfVector_(x); std::cout << std::endl

void printfVector_(vector<float> vec ){
	copy(vec.begin(), vec.end(), ostream_iterator<float>(cout, " "));
}

vector<float> linspace(float a, float b, size_t N) {
	float h = (b - a) / static_cast<float>(N - 1);
	vector<float> xs(N);
	vector<float>::iterator x;
	float val;
	for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
		*x = val;
	return xs;
}
;

cem::cem(RobotParameters robot) :
		rolloutSys(robot),eng(time(0)), dist(0, 1), gen(eng, dist), cemprof("cemThread") {
	//ctor
	eng.seed(time(0));
}

cem::~cem() {
	//dtor
}

void cem::init_cem() {
	ZMPReferenceX.zero();
	ZMPReferenceY.zero();

	for (int i = 0; i < 2; i++) {
		cemconfig[i].Q.zero();
		cemconfig[i].Q(0,0)=5.0;
		cemconfig[i].Q(1,1)=0.01;
		//Control cost matrix
		cemconfig[i].R_val = 10e-10;
		cemconfig[i].R.identity();
		cemconfig[i].R.scalar_mult(cemconfig[i].R_val);//*eye(cemconfig.M);
		cemconfig[i].R_inv.identity();
		cemconfig[i].R_inv.scalar_mult(1.0/cemconfig[i].R_val);
		cemconfig[i].theta.zero();
	}
	cemconfig::l =4 + floorf(3+log10f(cem_M));
	cemconfig::K_e = floorf(cemconfig->l/2.0);
	cemconfig::K =cemconfig->l;

	cout << "M: " + cem_M + " N: " + cem_N +" K: " + cemconfig::K;

	P = new float[cemconfig::K];
	for(int k  = 0; k < cemconfig::K; k++)
		if(k<=cemconfig::K_e)
			P(k) = 1.0f/cemconfig::K_e;
		else
			P(k)=0.0;


	sumGt.resize(cem_M);
	for (int m = 0; m < cem_M; m++)
		sumGt[m] = 0;
}

float cem::Gaussian(float t, float cm, float sm) {
	return exp(-(t - cm) * (t - cm) / (2.0 * sm * sm));
}

//output SUM the updated theta parameters
//input theta Mx1, state 1xS, L, Mx1
void  cem::run_rollouts(vector<rollout_result_t> & rolls, GMx1_t theta, GSx1_t init_state, Dynamics & sys, cemconfig_t & config, GNx1_t Zref, GMxM_t L) {
	float sum, state_cost, action_cost, cum_cost;
	float u,fS0,fS1,fS2,ZmpE2,fZmpE;
	int k,t, m;
	GMx1_t theta_k, randn;
	GSx1_t State_old;
	theta_k.zero();
	randn.zero();

	State_old.zero();
	State_old = init_state;

	GMx1_t x;
	x.zero();

	for (k = 0; k < cemconfig::K; k++) {
		cum_cost=0;
		for (m = 0; m < cem_M; m++)
			randn(m) = gen();// * expl_sigma + 0.000001; //e= randn(cem_K,1)*expl_sigma;

		rolls[k].e =  L*randn;
		theta_k = theta +rolls[k].e; //Add parameter noise

		//Initialize Dynamics with incoming state
		sys.State = init_state;
		x(0) = sys.State(0);
		x(1) = sys.State(1);
		cum_cost =0;
		for (t = 0; t < cem_N; t++) {

			//calculate action
			u = x.transp()*theta_k;
			//run simulation
			sys.RolloutUpdate(u);//get next_state
			x(0) = sys.State(0);
			x(1) = sys.State(1);

			 state_cost = x.transp()* config.Q * x;
			 action_cost = theta_k.transp() * config.R * theta_k;
			 cum_cost+=state_cost + action_cost;

		}
		rolls[k].S = cum_cost;
	}
}
//Get new dtheta
//rolls must be sorted
bool cem::cem_update(vector<rollout_result_t> & rolls , Dynamics & sys, cemconfig_t & config, float converge_value) {
	GMx1_t SUM;
	int k=0, t=0, m=0;
	SUM.zero();
	GMxM_t sigma_temp;
	sigma_temp.zero();

	float state_cost, action_cost, old_cost;


	//Update the theta parameters
	//Update the covariance Matrix
	for (k = 0; k < cemconfig::K; k++) { //for all rollouts
		sigma_temp = sigma_temp +(rolls[k].e*rolls[k].e.tansp()).scalar_mult(P(k));
		SUM = SUM + (config.theta + rolls[k].e).scalar_mult(P(k));
	}
	config.theta = SUM;
	config.cov = sigma_temp;

	GMx1_t x;
	x.zero();
	x(0) = sys.State(0);
	x(1) = sys.State(1);
	state_cost = x.transp()* config.Q * x;
	action_cost = config.theta.transp() * config.R * config.theta;

	old_cost = config.cost;
	config.cost = state_cost + action_cost;
	if(fabs(config.cost-old_cost)< converge_value)
		return true;
	else
		return false;

}

void cem::calculate_action(float & ux, float &uy, Dynamics Dx, Dynamics Dy, CircularBuffer<KVecFloat3> & ZmpBuffer) {
	KPROF_SCOPE(cemprof,"cem");


    //Setting the Reference Signal
	for (int i = 1; i < cem_N; i++)
	{
		if (i < ZmpBuffer.size()){
			ZMPReferenceX(i-1) = ZmpBuffer[i](0);
			ZMPReferenceY(i-1) = ZmpBuffer[i](1);
			//ZMPtheta(i-1) = ZmpBuffer[i](2);
		}
		else{

			ZMPReferenceX(i-1) = ZmpBuffer[ZmpBuffer.size() - 1](0);
			ZMPReferenceY(i-1) = ZmpBuffer[ZmpBuffer.size() - 1](1);
			//ZMPtheta(i-1)		 = ZmpBuffer[ZmpBuffer.size() - 1](2);
		}
	}
	GMxM_t L = cemconfig[0].cov.cholesky_decomposition();
	run_rollouts(vector<rollout_result_t> & rolls, GMx1_t theta, GSx1_t init_state, Dynamics & sys, cemconfig_t & config, GNx1_t Zref, GMxM_t L)
	//cem_update(vector<rollout_result_t> & rolls , Dynamics & sys, cemconfig_t & config, float converge_value)

	//run_rollouts(0, q, cemconfig[0].theta, Dx.State, rolloutSys,ZMPReferenceX, expl_sigma );
	///cemconfig[0].theta += cem_update(Me, q, cemconfig[0].theta);
//	cout << "Theta 0 : ";
//	cemconfig[0].theta.prettyPrint();
//
	//expl_sigma =0.1;// 50.0*fabs(Dy.State(0)) + 5.0*fabs(Dy.State(1))+ 6.0*fabs(Dy.State(2))+0.000005;;

    ///run_rollouts(Me, q, cemconfig[1].theta, Dy.State, rolloutSys, ZMPReferenceY, expl_sigma );
	///cemconfig[1].theta += cem_update(Me, q, cemconfig[1].theta);

    //	cout << "Theta 1 : ";
//	cemconfig[1].theta.prettyPrint();

	ux = 0;//cemconfig[0].theta.transp() * Gt[0];
	uy = 0;//cemconfig[1].theta.transp() * Gt[0];
}

//returns the vector for all the basis functions
GMx1_t cem::ng(float t, vector<float> centers, vector<float> sigma) {

	GMx1_t ret; //= new KMath::KMat::GenMatrix<float, cem_M,1>();
	ret.zero();
	float sum = 0;

	for (int i = 0; i < centers.size(); i++) {
		ret(i, 0) = Gaussian(t, centers[i], sigma[i]);
		sum += ret(i, 0);
	}

	for (int i = 0; i < centers.size(); i++)
		ret(i, 0) /= sum;

	return ret;

}

