#ifndef KALMAN_PARTICLE_H
#define KALMAN_PARTICLE_H


#include "tools/KMat.h"

class KalmanParticle

{
	public:
	typedef 	KMath::KMat::GenMatrix<float,3,3> Pmatrix;
	typedef KVecFloat3 Pvector ;

	Pmatrix P;
	Pvector x;
	Pvector Qdiag;
	float weight;

	void init(Pvector va) { Qdiag=va; };


	void reset(Pvector pos,Pvector Var,float w)
	{
		x=pos;
		P.zero();
		P(0,0)=Var(0);
		P(1,1)=Var(1);
		P(2,2)=Var(2);
		weight=w;

	}

	void predict(Pvector displacement,Pvector dvar,float dt)
	{

		//P.scalar_mult(2);
		Pmatrix Qdt;
		Qdt=makeQ(dt);
		Qdt(0,0)+=dvar(0);
		Qdt(1,1)+=dvar(1);
		Qdt(2,2)+=dvar(2);
		//P.prettyPrint();
		P+=Qdt;
		x+=displacement;
	}
	Pmatrix makeQ(float dt)
	{
		Pmatrix Qdt;
		Qdt.zero();
		Qdt(0,0)=Qdiag(0)*dt*dt/2;
		Qdt(1,1)=Qdiag(1)*dt*dt/2;
		Qdt(2,2)=Qdiag(2)*dt*dt/2;
		//std::cout<<"Q"<<std::endl;
		//Qdt.prettyPrint();
		return Qdt;
	}


	void update(float dist, float bear,float devdist, float devbear,float ox,float oy)
	{
		float dx=x(0)-ox;//dist*sin(bear);
		float dy=(x(1)-oy);//dist*cos(bear);
		float D=sqrt(dx*dx+dy*dy);
		KMath::KMat::GenMatrix<float,2,3> H;
		KMath::KMat::GenMatrix<float,3,2> T;
		KMath::KMat::GenMatrix<float,2,2> Sinv;
		KMath::KMat::GenMatrix<float,3,2> K;

		//P.prettyPrint();
		H.zero();
		H(0,0)=dx/D;
		H(0,1)=dy/D;
		H(1,0)=-dy/D;
		H(1,1)=dx/D;
		H(1,2)=-1;
		//T.prettyPrint();
		T=P.slow_mult(H.transp());
		//T.prettyPrint();
		Sinv=H.slow_mult(T);
		//L.prettyPrint();
		Sinv(0,0)+=devdist;
		Sinv(1,1)+=devbear;
		//L.prettyPrint();
		Sinv.fast_invert();
		K=T.slow_mult(Sinv);
		//K.prettyPrint();
		KVecFloat2 r;
		//x.prettyPrint();
		//std::cout<<ox<<" "<<oy<<std::endl;
		//std::cout<<dx<<" "<<dy<<std::endl;
		//float rdx,rdy;
		//rdx=dx*cos(-x(2))-dy*sin(-x(2));
		//rdy=dx*sin(-x(2))-dx*cos(-x(2));
		bear=KMath::wrapTo0_2Pi(bear);
		float a=KMath::wrapTo0_2Pi(KMath::wrapTo0_2Pi(atan2(-dy,-dx))-x(2));
		//std::cout<<a<<std::endl;

		//std::cout<<"D:"<<dist<<"-"<<D<<std::endl;
		//std::cout<<"bear"<<bear<<"-"<<KMath::wrapTo0_2Pi(atan2(dy,dx))<<"a"<<KMath::anglediff2(bear,a)<<std::endl;
		//r=z-h(x)
		r(0)=dist-D;
		r(1)=KMath::anglediff2(bear,a);
		//r.prettyPrint();

		x+=K.slow_mult(r);
		P-=K.slow_mult(H).slow_mult(P);
		x(2)=KMath::wrapTo0_2Pi(x(2));
		//x.prettyPrint();

		//Innovation process, innovation log probability of gaussian
		weight*=exp(-(r.transp().slow_mult(Sinv.slow_mult(r)))/2)*sqrt(Sinv(0,0)*Sinv(1,1)-Sinv(0,1)*Sinv(0,1));

	}


};















#endif
