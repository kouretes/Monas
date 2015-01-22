#include "cem.h"
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <ctime>
#include <iostream>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator
#include <vector>
using namespace std;

#define printfVector2(x) std::cout << "fVector: " << #x << "  "; printfVector_(x); std::cout << std::endl
int cemconfig::K = 0;
int cemconfig::K_e = 0;
int cemconfig::l = 0;
float cemconfig::R_val = 0.0;

void printfVector_2(vector<float> vec) {
	copy(vec.begin(), vec.end(), ostream_iterator<float>(cout, " "));
}

cem::cem(RobotParameters robot) :
		rolloutSys(robot), eng(time(0)), dist(0, 1), gen(eng, dist), cemprof(
				"cemThread") {
	//constructor
	eng.seed(time(0));
}

cem::~cem() {
	//destructor
}

void cem::init_cem() {
	ZMPReferenceX.zero();
	ZMPReferenceY.zero();

	for (int i = 0; i < AXIS; i++) {
		cemconfig[i].Q.zero();
		cemconfig[i].Q(0, 0) = 5.0;
		cemconfig[i].Q(1, 1) = 0.01;
		//Control cost matrix
		cemconfig[i].R_val = 10e-7;
		cemconfig[i].R.identity();
		cemconfig[i].R.scalar_mult(cemconfig[i].R_val); //*eye(cemconfig.M);
		cemconfig[i].R_inv.identity();
		cemconfig[i].R_inv.scalar_mult(1.0 / cemconfig[i].R_val);
		cemconfig[i].theta.zero();
		cemconfig[i].cov.identity();
	}
	cemconfig::l = 4 + floorf(3 + logf(CEM_M));
	cemconfig::K_e = floorf(cemconfig->l / 2.0);
	cemconfig::K = cemconfig->l;

	cout << "M: " << CEM_M << " N: " << CEM_N << " K: " << cemconfig::K;

	P = new float[cemconfig::K];
	for (int k = 0; k < cemconfig::K; k++)
		if (k <= cemconfig::K_e)
			P[k] = 1.0f / cemconfig::K_e;
		else
			P[k] = 0.0;

	GMx1_t e;
	e.zero();
	rollout_result ex;
	ex.e = e;
	rollouts.resize(cemconfig::K, ex);
	for (int k = 0; k < cemconfig::K; k++)
		(rollouts[k].e).zero();

	sumGt.resize(CEM_M);
	for (int m = 0; m < CEM_M; m++)
		sumGt[m] = 0;
}

float cem::Gaussian(float t, float cm, float sm) {
	return exp(-(t - cm) * (t - cm) / (2.0 * sm * sm));
}

//output SUM the updated theta parameters
//input theta Mx1, state 1xS, L, Mx1
void cem::run_rollouts(vector<rollout_result_t> & rolls, GSx1_t init_state,
		Dynamics & sys, cemconfig_t & config, GNx1_t Zref, GMxM_t L) {
	float cum_cost;
	float u, Yfb;
	int k, t, m,l,s;
	GMx1_t theta_k, randn;
	GSx1_t State_old;
	theta_k.zero();
	randn.zero();

	State_old.zero();
	State_old = init_state;

	GMx1_t x;
	GMx1_t x_pre;
	x.zero();
	x_pre.zero();


	for (k = 0; k < cemconfig::K; k++) {
		cum_cost = 0;
		for (m = 0; m < CEM_M; m++)
			randn(m) = gen(); // * expl_sigma + 0.000001; //e= randn(cem_K,1)*expl_sigma;

		rolls[k].e = L * randn;
		theta_k = config.theta + rolls[k].e; //Add parameter noise

		//Initialize Dynamics with incoming state
		sys.State = init_state;
		for(s=0;s<CEM_S;s++)
			x(s) = sys.State(s);

		cum_cost = 0;
		Yfb=0;
		for (t = 0; t < CEM_N; t++) {

			//calculate action
			for(l=0; l<CEM_N; l++)
				if(t+1+l < CEM_N)
					x(CEM_S+l) = Zref(l);
				else
					x(CEM_S+l) =  Zref(CEM_N-1);
			x(CEM_S) = sys.zmpstateNew - x(CEM_S);

			u = x.transp() * theta_k + Yfb;

			Yfb = Yfb + x(CEM_S)*theta_k(CEM_S);

//			if(u > 50)
//				u=50;
//			if(u<-50)
//				u=-50;
			x_pre = x;
			//run simulation
			sys.RolloutUpdate(u); //get next_state
			for(s=0;s<CEM_S;s++)
				x(s) = sys.State(s);



			if (abs(x(0)) > 10) {
				cout << "fallen ";
				rolls[k].S = 100;
				break;
			}


			cum_cost += calculate_cost(sys, config,theta_k,  Zref);
			if (isnan(cum_cost)) {
				cout << " k: " << k << " t: " << t << " u: " << u << " cost: ";
				cout << cum_cost << endl;
				cout << " Theta " << endl;
				theta_k.prettyPrint();
				cout << endl << " Xpre " << endl;
				x_pre.prettyPrint();
				cout << endl << " X " << endl;
				x.prettyPrint();
				//cout << " state_cost " << state_cost << endl;
				//cout << " action_cost " << action_cost << endl;

				cout << endl << " config.Q " << endl;
				config.Q.prettyPrint();
				cout << endl << "  config.R " << endl;
				config.R.prettyPrint();
				cout << "  rolls[k].e " << endl;
				rolls[k].e.prettyPrint();
				cout << "  L " << endl;
				L.prettyPrint();
				cout << "  init_state " << endl;
				init_state.prettyPrint();
				exit(1);
			}
		}

		rolls[k].S = cum_cost;
	}
}

float cem::calculate_cost(Dynamics & sys, cemconfig_t & config,GMx1_t theta_k, GNx1_t Zref){
	float state_cost=0, action_cost=0;
	state_cost = (sys.zmpstateNew - Zref(0) )*(sys.zmpstateNew - Zref(0) ); // x.transp()* config.Q * x;
	action_cost = theta_k.transp() * config.R * theta_k;
	return  state_cost + action_cost;
}

//Get new dtheta
//rolls must be sorted
bool cem::cem_update(vector<rollout_result_t> & rolls, Dynamics & sys,
		cemconfig_t & config, float converge_value, GNx1_t ZmpRef) {
	GMx1_t SUM;
	int k = 0; //, t=0, m=0;
	SUM.zero();
	GMxM_t sigma_temp, temp;
	sigma_temp.zero();
	temp.zero();

	float old_cost;

	//Update the theta parameters
	//Update the covariance Matrix
	for (k = 0; k < cemconfig::K; k++) { //for all rollouts
		//temp = rolls[k].e*(rolls[k].e.transp());
		SUM = SUM + (config.theta + rolls[k].e).scalar_mult(P[k]);

		sigma_temp = sigma_temp
				+ (rolls[k].e * (rolls[k].e).transp()).scalar_mult(P[k]);

	}
	config.theta = SUM;
	cout << "Update  config.theta " << endl;
	config.theta.prettyPrint();

	config.cov = sigma_temp;
	cout << "Stmp: ";
	config.cov.prettyPrint();

	//GMx1_t x;
	//x.zero();
	//x(0) = sys.State(0);
	//x(1) = sys.State(1);
	//x(2) = sys.State(2);
	//state_cost = x.transp() * config.Q * x;
	//action_cost = config.theta.transp() * config.R * config.theta;

	old_cost = config.cost;
	config.cost = calculate_cost(sys, config,config.theta, ZmpRef);//state_cost + action_cost;
	cout << "new cost " << config.cost << " oldcost " << old_cost << " diff:  " << fabs(config.cost - old_cost) <<  endl;
//	if (fabs(config.cost - old_cost) < converge_value)
//		return false;
//	else
		return true;

}

void cem::run_rollouts_and_update( Dynamics & InputSys,
	cemconfig_t & config, float converge_value, GNx1_t Zref){
	GMxM_t L;
	L.zero();
	L = config.cov;
	try {
		L = cholesky_decomposition(L);
	} catch (KMath::KMat::SingularMatrixInvertionException e) {
		cerr << " SingularMatrixInvertionException 0: " << e.what() << endl;
		return;
	}
	//cout << "After Chol" << endl;
	//L.prettyPrint();

	int count_conv = 0;
	do {
		run_rollouts(rollouts, InputSys.State, rolloutSys, config, Zref, L);
		sort(rollouts.begin(), rollouts.end());
		cout << " Convergence loop " << count_conv++ << endl;
	} while (cem_update(rollouts, InputSys, config, converge_value,Zref )); //while not converged

}


void cem::calculate_action(float & ux, float &uy, Dynamics Dx, Dynamics Dy,
		CircularBuffer<KVecFloat3> & ZmpBuffer) {
	KPROF_SCOPE(cemprof,"cem");

	//Setting the Reference Signal
	for (int i = 1; i < CEM_N; i++) {
		if (i < ZmpBuffer.size()) {
			ZMPReferenceX(i - 1) = ZmpBuffer[i](0);
			ZMPReferenceY(i - 1) = ZmpBuffer[i](1);
			//ZMPtheta(i-1) = ZmpBuffer[i](2);
		} else {

			ZMPReferenceX(i - 1) = ZmpBuffer[ZmpBuffer.size() - 1](0);
			ZMPReferenceY(i - 1) = ZmpBuffer[ZmpBuffer.size() - 1](1);
			//ZMPtheta(i-1)		 = ZmpBuffer[ZmpBuffer.size() - 1](2);
		}
	}
	//cout << "before Chol" << endl;

	//cemconfig[0].cov.prettyPrint();
	cout <<" Axis 0 " << endl;
	run_rollouts_and_update(Dx,cemconfig[0], 10e-5,ZMPReferenceX);

//	L = cemconfig[1].cov;
//	try {
//		L = cholesky_decomposition(L);
//	} catch (KMath::KMat::SingularMatrixInvertionException e) {
//		cerr << " SingularMatrixInvertionException 1: " << e.what() << endl;
//		return;
//	}
//	count_conv = 0;
//	do {
//		run_rollouts(rollouts, Dy.State, rolloutSys, cemconfig[1],
//				ZMPReferenceY, L);
//		sort(rollouts.begin(), rollouts.end());
//		cout << " Convergence loop " << count_conv++ << endl;
//	} while (cem_update(rollouts, Dy, cemconfig[1], 10e-5,ZMPReferenceY(0)));

//	cemconfig[1].theta.prettyPrint();
	GMx1_t x;
	x.zero();
	x(0) = Dx.State(0);
	x(1) = Dx.State(1);
	x(2) = Dx.State(2);

	ux = cemconfig[0].theta.transp() * x;

	x(0) = Dy.State(0);
	x(1) = Dy.State(1);
	x(2) = Dy.State(2);
	uy = cemconfig[1].theta.transp() * x;
}

//returns the vector for all the basis functions
GMx1_t cem::ng(float t, vector<float> centers, vector<float> sigma) {

	GMx1_t ret; //= new KMath::KMat::GenMatrix<float, CEM_M,1>();
	ret.zero();
	float sum = 0;
	unsigned int i;
	for (i = 0; i < centers.size(); i++) {
		ret(i, 0) = Gaussian(t, centers[i], sigma[i]);
		sum += ret(i, 0);
	}

	for (i = 0; i < centers.size(); i++)
		ret(i, 0) /= sum;

	return ret;

}

