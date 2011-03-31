#ifndef _KALMAN_H_
#define _KALMAN_H_
#include "../KMat.h"
template<typename V>
class Kalman1D
{
  public:
  typedef KMat::GenMatrix<V,2,1> Xbar;
 
  private:
  KMat::GenMatrix<V,2,2> P;
  V vara;
  Xbar r;
  public: 
  void init(V va) { vara=va; };
  Xbar const& predict(V dt)
  {
    r(0)=r(0)+r(1)*dt;
    KMat::GenMatrix<V,2,2> Pinc,Q;
    Pinc.zero();
    V dtd=dt*P.read(1,1);
    Pinc.get(0,0)=dt*(P.read(1,0)+P.read(1,1))+dt*dtd;
    Pinc.get(0,1)=dtd;
    Pinc.get(1,0)=dtd;
    //Pinc.prettyPrint();
    P+=Pinc;
    Q=produceQ(dt);
    Q.scalar_mult(vara);
    //Q.prettyPrint();
    P+=Q;
    //P.prettyPrint(); 
    return r;  
    
  }
  void reset(V v, V L)
  {
    r(0)=v;r(1)=0;
    P.identity();
    P.scalar_mult(L);
  }
  
  Xbar const& update(V v, V varv, V dt)
  {
    predict(dt);
    V y=v-r(0);//Residual
    V s=P.read(0,0)+ varv;
    Xbar k;
    k(0)=P.read(0,0)/s;
    k(1)=P.read(1,0)/s;
    //k.prettyPrint();
    
    r(0)+=k(0)*y;
    r(1)+=k(1)*y;
    //std::cout<<"Test"<<std::endl;
    KMat::GenMatrix<V,2,2> Pinc;
    Pinc.zero();
    Pinc.get(0,0)=-k(0)*P.read(0,0);
    Pinc.get(0,1)=-k(0)*P.read(0,1);
    Pinc.get(1,0)=-k(1)*P.read(0,0);
    Pinc.get(1,1)=-k(1)*P.read(0,1);
    P+=Pinc;
    //P.prettyPrint();
    return r;
    
    
    
  }  
  Xbar const& read() const
  {
    return r;
  }
  
  private:
  static KMat::GenMatrix<V,2,2> produceQ(V dt)
 {
   KMat::GenMatrix<V,2,2> q;
   q.zero();
   V dtsqrd=dt*dt;
   q.get(0,0)=dtsqrd*dtsqrd/4;
   q.get(0,1)=dtsqrd*dt/2;
   q.get(1,0)=q.read(0,1);
   q.get(1,1)=dtsqrd;
 
   return q;
   
 }
  
  
  
  
  
};

#endif //_KALMAN_H_
