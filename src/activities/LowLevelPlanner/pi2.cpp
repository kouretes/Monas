#include "pi2.h"
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

pi2::pi2(RobotParameters robot) :
		rolloutSys(robot),eng(time(0)), dist(0, 1), gen(eng, dist), pi2prof("Pi2Thread") {
	//ctor
	eng.seed(time(0));
}

pi2::~pi2() {
	//dtor
}

void pi2::init_pi2() {
	ZMPReferenceX.zero();
	ZMPReferenceY.zero();
    KMath::KMat::GenMatrix<float,4,1> LQRgain;
    LQRgain.zero();
    LQRgain(0)=1.0e+04 * 3.2000;
    LQRgain(1)=1.0e+04 * 0.2080;
    LQRgain(2)=1.0e+04 * 0.0109;
    LQRgain(3)=0;
    LQRgain.scalar_mult(-1);

	for (int i = 0; i < 2; i++) {
		//Decay rate of the exploration noise
		pi2config[i].gama = 0.9;
		//Variance of the exploration noise
		pi2config[i].sigme_e = 5;
		//Control cost matrix
		pi2config[i].R = 1e-8;
		pi2config[i].control_cost.identity();
		pi2config[i].control_cost.scalar_mult(pi2config[i].R);//*eye(pi2config.M);
		pi2config[i].inv_control_cost.identity();
		pi2config[i].inv_control_cost.scalar_mult(1.0/pi2config[i].R);
		pi2config[i].theta=LQRgain;//.zero();


	}
	//initial parameter matrix
	centers = linspace(0, 0.01 * PI2_N, PI2_M);
	printfVector(centers);
	//abs(pi2config.centers(1)-pi2config.centers(end))/(2*sqrt(pi2config.M))*ones(pi2config.M,1);
	float sigma = fabs(centers[0] - centers[PI2_M - 1]) / (2.0 * sqrt(PI2_M));
	sigmas.resize(0);
	sigmas.resize(PI2_M, sigma);            //= new vector<float>(PI2_M,sigma);
	printfVector(sigmas);
	//load theta
	//pi2config.theta_init=0;//*ones(pi2config.M,1);
	GMxM_t Rinv;
	Rinv.identity();

	//Rinv = pi2config[0].control_cost; //.invert_square_matrix(pi2config.control_cost);
	//
	Rinv.scalar_mult(1.0/pi2config[0].R);
	Rinv.prettyPrint();
	//Rinv=Rinv.invert_square_matrix(Rinv);
	GMx1_t gt;
	gt.zero();
	Gst.resize(PI2_N,gt);
	for(int t=0;t<PI2_N; t++)
		Gst[t].zero();

	GMxM_t Mt;
	Mt.zero();
	sumGt.resize(PI2_M);
	for (int m = 0; m < PI2_M; m++)
		sumGt[m] = 0;

	for (int t = 0; t < PI2_N; t++) {
		gt = ng(t * 0.01, centers, sigmas);

		cout << " T: " << t << endl;
		gt.prettyPrint();

		Gt.push_back(gt);
		float gtTRgt = gt.transp() * Rinv * gt;

		Mt = Rinv;
		Mt *= gt * gt.transp();

		Mt.scalar_mult(1.0 / gtTRgt);

		Mt.prettyPrint();
		cout << " M[ " << t << "]" ;
		M.push_back(Mt);
		if (t < PI2_N - 1)
			for (int m = 0; m < PI2_M; m++)
				sumGt[m] += gt(m);
	}
	//Initialize M*e vectors
	Me.resize(PI2_K);
	for (int k = 0; k < PI2_K; k++){
		Me[k].resize(PI2_N, pi2config[0].theta);
		for(int t=0;t<PI2_N; t++){
			Me[k][t].zero();
			for(int m =0; m<PI2_M;m++)
				Me[k][t](m)=gen();
		}
	}
	for (int k = 0; k < PI2_K; k++)
		for(int t=0;t<PI2_N; t++){
		   cout<< "Me[" << k << "][" << t << "]:" ;
		   Me[k][t].prettyPrint();
		}
	float sumd=0;
	int t;
	for (t = 0; t < PI2_N; t++){
		denom[t]=PI2_N-1-t;
		sumd+=denom[t];
	}
	for (t = 0; t < PI2_N; t++)
		denom[t]/=sumd;


	//(Me[2][3]).prettyPrint();
	//cout << " [1][2] : " ;
	//(Me[1][2]).prettyPrint();
	//e= randn(PI2_K,1)*expl_sigma;
}

float pi2::Gaussian(float t, float cm, float sm) {
	return exp(-(t - cm) * (t - cm) / (2.0 * sm * sm));
}

//void pi2::basis(GSx1_t & state, GMx1_t & Gst){
//	Gst(0) = state(0);
//	Gst(1) = state(1);
//	Gst(2) = state(2);
//	Gst(3) = state(0)*state(0)*state(0);
//	Gst(4) = 10e-5;
//}
//output Me = kxNxM q[KxN], theta Mx1, state 1xS, noise, Mx1
void pi2::run_rollouts(vector<vector<GMx1_t> >& Me, GKxN_t &q, GMx1_t theta,
		GSx1_t init_state, Dynamics & sys, GNx1_t Zref,float expl_sigma) {
	float u,fS0,fS1,fS2,ZmpE2,fZmpE;
	int k,t, m;
	GMx1_t theta_k, e;
	GSx1_t State_old;
	GMxM_t Mtk;
	theta_k.zero();
	e.zero();
	State_old.zero();
	Mtk.zero();
	State_old = init_state;
	float gRg;
	//cout << expl_sigma;
	for (k = 0; k < PI2_K; k++) {
		for (m = 0; m < PI2_M; m++)
			e(m) = gen() *sqrt(fabs(theta(m))*0.05)* expl_sigma + 0.1; //e= randn(PI2_K,1)*expl_sigma;

		theta_k = theta + e; //Add parameter noise
		//cout << " error e: " ;
		//e.prettyPrint();
		//Initialize Dynamics with incoming state
		sys.State = init_state;
		for (t = 0; t < PI2_N; t++) {
			//Set bassis functions
 			Gst[t](0)=sys.State(0); //print and check
 			Gst[t](1)=sys.State(1);
 			Gst[t](2)=sys.State(2);
 			Gst[t](3)=1e-5;
 			//That 0 is not good ...
 			//cout << " Gst[" << t << "]:";
 			//Gst[t].prettyPrint();
 			Mtk = pi2config[0].inv_control_cost*(Gst[t]*Gst[t].transp());
 			//cout << " Mtk pre:";
 			//Mtk.prettyPrint();
 			//pi2config[0].inv_control_cost.prettyPrint();
 			gRg = Gst[t].transp()*pi2config[0].inv_control_cost*Gst[t];
 			//cout << " gRg: " << gRg << endl;
 			Mtk.scalar_mult(1/gRg);
 			//cout << " Mtk final:";
 			//Mtk.prettyPrint();
 			Me[k][t] = Mtk*e;
 			//cout << " Me[" << k << "][" << t << "]:";
 			//Me[k][t].prettyPrint();
			//Me[k][t] = M[t] * e;
		    //calculate cost
			{
				fZmpE=fabs(0*Zref(t)-sys.zmpstateNew);
				ZmpE2 =fZmpE*fZmpE;
				fS2 = (sys.State(2))*(sys.State(2));
				fS1 = (sys.State(1))*(sys.State(1));
				fS0 = (sys.State(0))*(sys.State(0));
				q(k, t)=0.000000001;

				q(k,t) += 10e3*ZmpE2 + 1*fS1+10e-2*fS2+fS0;
				//q(k,t)*=100;
				if(fZmpE>0.05)
					q(k,t) += (ZmpE2*ZmpE2)/0.00000625;
			}
			//calculate action
			if(t==PI2_N)
				break;
			State_old=sys.State;
			u = Gst[t].transp() * theta_k; //theta*gt
			//std::cout << "U : " << u << std::endl;
			//run simulation
			sys.RolloutUpdate(u);//get next_state
		}

	}

}
//Get new dtheta
GMx1_t pi2::pi2_update(vector<vector<GMx1_t> >& Me, GKxN_t q, GMx1_t theta) {
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

	for (t=0; t<PI2_N; t++)
	{
		minS(t)=std::numeric_limits<float>::max();
		maxS(t)=std::numeric_limits<float>::min();
	}
	std::cout<<"Update!--------------------------"<<std::endl;

    for (k = 0; k < PI2_K; k++) { //for all rollouts
		control_cost_cum = 0;
		state_cost_cum = 0;


		terminal_cost=q(k, PI2_N - 1);
		S(k,  PI2_N - 1)=terminal_cost;


		for (t=PI2_N -1 -1;t>= 0; t--) {

            state_cost_cum += q(k, t);
			S(k, t) = state_cost_cum + control_cost_cum / 2.0 + terminal_cost;


			theta_hat_temp = theta + Me[k][t+1];
			//theta_hat_temp.prettyPrint();

			thRth =theta_hat_temp.transp() * pi2config[0].control_cost * theta_hat_temp; //Check if nan;
			control_cost_cum += thRth;

		}
	}

	for (t = 0; t < PI2_N;t++) {
	   for (k = 0; k < PI2_K; k++)
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
	/*  expS = exp(-pi2config.h*(S - minS*ones(1,pi2config.K))./((maxS-minS)*ones(1,pi2config.K)));

	 % the probabilty of a trajectory
	 P = expS./(sum(expS,2)*ones(1,pi2config.K));*/
    /*maxS.prettyPrint();
    minS.prettyPrint();
    (maxS-minS).prettyPrint();
    q.prettyPrint();
    S.prettyPrint();
    std::cout<< setw(15) << setprecision(10) <<S(0,0)<< std::endl;
    std::cout<< setw(15) << setprecision(10) <<S(1,0)<< std::endl;*/
	for (t = 0; t < PI2_N; t++) {
		float max_min =  maxS(t) - minS(t);
		if(max_min<1e-10)max_min=1e-10;
		//cout << " dif: " << max_min;
		for (k = 0; k < PI2_K; k++) {    //exp
			float res = -h * (S(k, t) - minS(t));

			P(k, t) = exp(res/max_min);
			expsum(t) += P(k, t);
		}
		for (k = 0; k < PI2_K; k++)
			P(k, t) = P(k, t) / (float) expsum(t);
	}
	//P.prettyPrint();

	vector<GMx1_t> dtheta;
	GMx1_t dtheta_t;
	dtheta_t.zero();
	dtheta.resize(PI2_N,dtheta_t);
	/*
	 dtheta = zeros(pi2config.M,pi2config.N);
	 dtheta_t=zeros(pi2config.M,1);

	 for t = 1:pi2config.N
	 for k = 1:pi2config.K
	 dtheta(:,t) = dtheta(:,t) + P(t,k)*Me(:,k,t);
	 end
	 end

      denom=sum(pi2config.N:-1:1);
        for m=1:pi2config.M
            dtheta_t(m)=sum(dtheta(m,1:end-1).*[pi2config.N-1:-1:1])./denom;
        end
	 */
	for (k = 0; k < PI2_K; k++)
		for (t = 0; t < PI2_N; t++)
			dtheta[t] = dtheta[t] + (Me[k][t])*(P(k, t));

// for (m = 0; m < PI2_M; m++) {
// 		dtheta_t(m) = 0;
// 		for (t = 0; t < PI2_N - 1; t++)
// 			dtheta_t(m) = dtheta_t(m) + Gt[t](m) * dtheta[t](m);
// 		dtheta_t(m) /= sumGt[m];
// 	}
	//here denom[t] =  t/sum(denom)
	for (m = 0; m < PI2_M; m++) {
		dtheta_t(m) = 0;
        GNx1_t w;
        float denom=0;
        for (t = 0; t < PI2_N - 1; t++)
        {
            w(t)=0;
            for(unsigned k=0;k<PI2_K;k++)
                w(t)+=q(k,t);
            w(t)=1./(w(t)+1);
            denom+=1.0*(PI2_N-t)*w(t);
        }
		for (t = 0; t < PI2_N - 1; t++)
			dtheta_t(m) = dtheta_t(m) + (dtheta[t](m)*w(t)*(PI2_N-t))/denom;
	}

	return dtheta_t;
}

void pi2::calculate_action(float & ux, float &uy, Dynamics Dx, Dynamics Dy, CircularBuffer<KVecFloat3> & ZmpBuffer) {
	KPROF_SCOPE(pi2prof,"pi2");
	GKxN_t q;
	q.zero();
	float expl_sigma = 1;// 50.0 * fabs(Dx.State(0)) + 5.0 * fabs(Dx.State(1))	+ 0.5 * fabs(Dx.State(2))+0.0000001;
    Dx.State.prettyPrint();

    //Setting the Reference Signal
	for (int i = 1; i < PI2_N; i++)
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
	for(int rr=1;rr<=PI2_R;rr++){
		//expl_sigma = 5*fabs(Dx.State(1))+2+5*fabs(Dx.State(2));
		//cout << "Xaxis  " << rr << endl;
		run_rollouts(Me, q, pi2config[0].theta, Dx.State, rolloutSys, ZMPReferenceX, expl_sigma );
		GMx1_t c;
		c = pi2_update(Me, q, pi2config[0].theta);
		//c.prettyPrint();
		pi2config[0].theta += c;
		//cout << "Yaxis  " << rr << endl;
		run_rollouts(Me, q, pi2config[1].theta, Dy.State, rolloutSys, ZMPReferenceY, expl_sigma );
		pi2config[1].theta += pi2_update(Me, q, pi2config[1].theta);

		expl_sigma=expl_sigma*pi2config[0].gama;
	}

	Gst[0](0)=Dx.State(0); //print and check
	Gst[0](1)=Dx.State(1);
	Gst[0](2)=Dx.State(2);
	Gst[0](3)=10e-5;
	ux = pi2config[0].theta.transp() * Gst[0];
	pi2config[0].theta.prettyPrint();
	Gst[0](0)=Dy.State(0); //print and check
	Gst[0](1)=Dy.State(1);
	Gst[0](2)=Dy.State(2);
	Gst[0](3)=10e-5;
	uy = pi2config[1].theta.transp() * Gst[0];
	pi2config[1].theta.prettyPrint();

}

//returns the vector for all the basis functions
GMx1_t pi2::ng(float t, vector<float> centers, vector<float> sigma) {

	GMx1_t ret; //= new KMath::KMat::GenMatrix<float, PI2_M,1>();
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

