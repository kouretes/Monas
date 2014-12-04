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
		cemconfig[i].Q[0][0]=5.0;
		cemconfig[i].Q[1][1]=0.01;
		//Control cost matrix
		cemconfig[i].R_val = 10e-10;
		cemconfig[i].R.identity();
		cemconfig[i].R.scalar_mult(cemconfig[i].R_val);//*eye(cemconfig.M);
		cemconfig[i].R_inv.identity();
		cemconfig[i].R_inv.scalar_mult(1.0/cemconfig[i].R_val);
		cemconfig[i].theta.zero();
	}
	cemconfig->l =4 + floorf(3+log10f(cem_M));
	cemconfig->K_e = floorf(cemconfig->l/2.0);
	cemconfig->K =cemconfig->l;

	cout << "M: " + cem_M + " N: " + cem_N +" K: " + cemconfig->K;

	P = new float[cemconfig->K];
	for(int k  = 0; k < cemconfig->K; k++)
		if(k<=cemconfig->K_e)
			P(k) = 1.0f/cemconfig->K_e;
		else
			P(k)=0.0;


	sumGt.resize(cem_M);
	for (int m = 0; m < cem_M; m++)
		sumGt[m] = 0;
}

float cem::Gaussian(float t, float cm, float sm) {
	return exp(-(t - cm) * (t - cm) / (2.0 * sm * sm));
}

//output Me = kxNxM q[KxN], theta Mx1, state 1xS, noise, Mx1
void cem::run_rollouts(vector<vector<GMx1_t> >& Me, GKxN_t &q, GMx1_t theta,
		GSx1_t init_state, Dynamics & sys, GNx1_t Zref,float expl_sigma) {
	float u,fS0,fS1,fS2,ZmpE2,fZmpE;
	int k,t, m;
	GMx1_t theta_k, e;
	GSx1_t State_old;
	theta_k.zero();
	e.zero();
	State_old.zero();
	State_old = init_state;

	//cout << expl_sigma;
	for (k = 0; k < cemconfig->K; k++) {
		for (m = 0; m < cem_M; m++)
			e(m) = gen() * expl_sigma + 0.000001; //e= randn(cem_K,1)*expl_sigma;

		theta_k = theta + e; //Add parameter noise

		//Initialize Dynamics with incoming state
		sys.State = init_state;
		for (t = 0; t < cem_N; t++) {
			//Me[k][t] = M[t] * e;
		    //calculate cost
			{
				fZmpE=fabs(0*Zref(t)-sys.zmpstateNew);
				ZmpE2 =fZmpE*fZmpE;
				fS2 = (sys.State(2))*(sys.State(2));
				fS1 = (sys.State(1))*(sys.State(1));
				fS0 = (sys.State(0))*(sys.State(0));
				q(k, t)=0.000000001;
//				if (ZmpE2 > 0.01)
//					q(k, t) += 9.0 *(ZmpE2 - 0.01 +1.0)*(t+1);
//				if (fS0 > 0.009)
//					q(k, t) += 6.0 *(fS0 - 0.009 +1.0)*(t+1);
//				q(k,t) += 35*(Zref(t)-sys.zmpstateNew)*(Zref(t)-sys.zmpstateNew);
//				q(k,t) += 45*(50*fS0)*(50*fS0);
//				q(k,t) += 0.09*(sys.State(2))*(sys.State(2));
//				q_2(j)=(50*x(1)).^2;
//			    q(j)= 35*q_1(j)*j+ 15*(q_2(j))*j+0.08*(x(3)^2);%

//				if (fS0 > 0.02)
//					q(k, t) += 900.0 * (fS0 - 0.02 +1)*t;
//				 q_1(j)=abs(zmpy(j)- 0*ZMPY(j));
//				    q_2(j)=(x(2)).^2;
//				    q_3(j)=(x(1)- zmpy(j))^2;
//				%     %q_2(j)=abs(x(1)-xold(1)).^2;
//				%
//				    q(j) = 0.3*q_1(j) +8*q_1(j).^2 + 10*q_2(j) + 0.9*q_3(j);%+x(2)^2;%+ q_1(j)*200*(x(2)^2)+0.3*(x(3)^2)*q_1(j) + 0.002 *(x(3)^2);%; +2.5*q_2(j) + 0.5*q_3(j);%+2.9*(x(2)^2);%+ q_3(j)*0.05;% + 2*((x(2))^2)+ 2*q_3(j);%  +2*(x(3)-xold(3))^2  ;%+0.0001*abs(x(3));%(x-xold)'*(model.Q)*(x-xold);
//				%     q(j)=q(j)*j;
//				%
				q(k,t) += 2000*fS0 + 10*fS1 + 0.05*fS2; //630.0*ZmpE2 +
				q(k,t)*=(t+1);
			//	if(fZmpE>0.05)
				//	q(k,t) += 10.0*(fZmpE);
			}
			//calculate action
			if(t==cem_N)
				break;
			State_old=sys.State;
			//u = Gt[t].transp() * theta_k; //theta*gt
			//run simulation
			sys.RolloutUpdate(u);//get next_state
		}
	}
}
//Get new dtheta
GMx1_t cem::cem_update(vector<vector<GMx1_t> >& Me, GKxN_t q, GMx1_t theta) {
	int k=0, t=0, m=0;
	float thRth=0, control_cost_cum=0, state_cost_cum=0;
	float terminal_cost;
	float h = 10.0;
	GKxN_t S;
	GNx1_t maxS, minS;
	GMx1_t theta_hat_temp;
	S.zero();
	theta_hat_temp.zero();
	maxS.zero();
	minS.zero();

	for (t=0; t<cem_N; t++)
	{
		minS(t)=std::numeric_limits<float>::max();
		maxS(t)=std::numeric_limits<float>::min();
	}

    for (k = 0; k < cemconfig->K; k++) { //for all rollouts
		control_cost_cum = 0;
		state_cost_cum = 0;


		terminal_cost=q(k, cem_N - 1);
		S(k,  cem_N - 1)=terminal_cost;


		for (t=cem_N -1 -1;t>= 0; t--) {

            state_cost_cum += q(k, t);
			S(k, t) = state_cost_cum + control_cost_cum / 2.0 + terminal_cost;


			theta_hat_temp = theta + Me[k][t+1];
			//theta_hat_temp.prettyPrint();

			thRth =theta_hat_temp.transp() * cemconfig[0].R * theta_hat_temp; //Check if nan;
			control_cost_cum += thRth;

		}
	}

	for (t = 0; t < cem_N;t++) {
	   for (k = 0; k < cemconfig->K; k++)
	   {
            if (S(k, t) > maxS(t))
                maxS(t) =(float)S(k, t);
            if (S(k, t) < minS(t))
                minS(t) =(float)S(k, t);
        }
	}

	GKxN_t P;
	P.zero();
	GNx1_t expsum;
	expsum.zero();
	/*  expS = exp(-cemconfig.h*(S - minS*ones(1,cemconfig.K))./((maxS-minS)*ones(1,cemconfig.K)));

	 % the probabilty of a trajectory
	 P = expS./(sum(expS,2)*ones(1,cemconfig.K));*/

	for (t = 0; t < cem_N; t++) {
		float max_min =  maxS(t) - minS(t);
		if(max_min<1e-10)
			max_min=1e-10;
		//cout << " dif: " << max_min;
		for (k = 0; k < cemconfig->K; k++) {    //exp
			float res = -h * (S(k, t) - minS(t));

			P(k, t) = exp(res/max_min);
			expsum(t) += P(k, t);
		}
		for (k = 0; k < cemconfig->K; k++)
			P(k, t) = P(k, t) / (float) expsum(t);
	}

	vector<GMx1_t> dtheta;
	GMx1_t dtheta_t;
	dtheta_t.zero();
	dtheta.resize(cem_N,dtheta_t);
	/*
	 dtheta = zeros(cemconfig.M,cemconfig.N);
	 dtheta_t=zeros(cemconfig.M,1);

	 for t = 1:cemconfig.N
	 for k = 1:cemconfig.K
	 dtheta(:,t) = dtheta(:,t) + P(t,k)*Me(:,k,t);
	 end
	 end

	 for m=1:cemconfig.M
	 dtheta_t(m) =  (Globalgt(1:end-1,m)'*dtheta(m,1:end-1)')/sum(Globalgt(1:end-1,1));
	 end
	 r=r+1;
	 %disp([' **************** New Theta  ' num2str(r)] );
	 theta(:,r) = theta(:,r-1) + dtheta_t;
	 */
	for (k = 0; k < cemconfig->K; k++)
		for (t = 0; t < cem_N; t++)
			dtheta[t] = dtheta[t] + (Me[k][t])*(P(k, t));

	for (m = 0; m < cem_M; m++) {
		dtheta_t(m) = 0;
		for (t = 0; t < cem_N - 1; t++)
			;//dtheta_t(m) = dtheta_t(m) + Gt[t](m) * dtheta[t](m);
		dtheta_t(m) /= sumGt[m];
	}
	return dtheta_t;
}

void cem::calculate_action(float & ux, float &uy, Dynamics Dx, Dynamics Dy, CircularBuffer<KVecFloat3> & ZmpBuffer) {
	KPROF_SCOPE(cemprof,"cem");
	GKxN_t q;
	q.zero();
	float expl_sigma = 15;// 50.0 * fabs(Dx.State(0)) + 5.0 * fabs(Dx.State(1))	+ 0.5 * fabs(Dx.State(2))+0.0000001;


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

    expl_sigma =10*fabs(Dx.State(0))+ 20*fabs(Dx.State(1))+5+6*fabs(Dx.State(2));

	///run_rollouts(0, q, cemconfig[0].theta, Dx.State, rolloutSys,ZMPReferenceX, expl_sigma );
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

