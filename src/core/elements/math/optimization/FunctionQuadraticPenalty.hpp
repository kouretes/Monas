#ifndef FUNCTIONQUADRATICPENALTY_HPP
#define FUNCTIONQUADRATICPENALTY_HPP

#include "ITwiceDiffFunction.hpp"
#include "FunctionAffine.hpp"
#include "IPenaltyFunction.hpp"


namespace KMath
{

  template<typename T,unsigned S,unsigned L>
  class FunctionQuadraticPenalty :      public virtual IPenaltyFunction<T,S>
  {
  public:
    typedef  KMat::GenMatrix<T,L,1> LVec;

    FunctionQuadraticPenalty(FunctionAffine<T,S,L> af) : f (af)
    {
      mask.zero();
      Pvec.zero();
      AtP.zero();
      //processConstraints();
    };
    void setX(KMat::GenMatrix<T,S,1> const& ax)
    {
      f.setX(ax);
      IPenaltyFunction<T,S>::setX(ax);
      processConstraints();
    };
    void setFunction(FunctionAffine<T,S,L> af)    { f=af; };
    T evaluate() const
    {
      LVec v=f.evaluate();
      v.column_mult(mask);
      return  v.norm2()*this->t;
    };
	KMat::GenMatrix<T,S,S> getH() const
	{
		return AtP*f.getA();//.scalar_mult(this->t);
	};
    KMat::GenMatrix<T,S,1> getf() const
    {
    	return (AtP*f.getb() ).scalar_mult(-2);
	};

    void processConstraintsAt(KMat::GenMatrix<T,S,1>& x )
    {
      setX(x);
      processConstraints();
	};

	bool allSatisfied()
	{
		//std::cout<<"C:::"<<mask.sum()<<std::endl;
		//mask.transp().prettyPrint();
		return mask.sum()<1;
	}

	int getViolatedConstraint(KMat::GenMatrix<T,S,1> const& ax,T const& tol)
	{
		f.setX(ax);
		LVec v=f.evaluate();
		//v.prettyPrint();

		for(unsigned i=0; i<L; i++)
			if(v(i)>tol)
			{
				//std::cout<<"v(i):"<<v(i)<<std::endl;
				return i;
			}



		return -1;
	}
	KMat::GenMatrix<T,S,1> getConstraint(unsigned i,T &bi)
	{
		KMat::GenMatrix<T,S,1> Ai;
		Ai.zero();
		bi=f.getb()(i);
		KMat::GenMatrix<T,L,S> A=f.getA();
		for(unsigned j=0;j<S;j++)
		{
			Ai(j)=A(i,j);
		}
		return Ai;
	}
    void processConstraints()
    {
	  LVec v=f.evaluate();
      mask.zero();
      Pvec.zero();
      for(unsigned i=0; i<L; i++)
        if(v(i)>0) //THIS EQUAL SIGN IS EVIL DO NOT TOUCH
        {
          mask(i)=1;
          Pvec(i)=this->t;

        }
      AtP=(f.getA().column_mult(Pvec)).transp();
    }
    void replaceConstraint(unsigned i, T t)
    {
	  //LVec v=f.evaluate();
      //mask.zero();
      //P.zero();
      //for(unsigned i=0; i<L; i++)
        //if(v(i)>1e-4) //THIS EQUAL SIGN IS EVIL DO NOT TOUCH
        {
          mask(i)=1;
          Pvec(i)=t;

        }
      AtP=(f.getA().column_mult(Pvec)).transp();
    }
    KMat::GenMatrix<T,1,S> getJacobian() const
    {

      KMat::GenMatrix<T,S,1> t1;
      KMat::GenMatrix<T,L,1> b;
      KMat::GenMatrix<T,L,S> A=f.getA();
      b=f.getb();
      t1=AtP*(A*this->x-b);
      t1.scalar_mult(2);
      return t1.transp();


    };

    KMat::GenMatrix<T,S,S> getHessian() const
    {
      KMat::GenMatrix<T,L,S> A=f.getA();

      return (AtP*A).scalar_mult(2);

    };


  protected:
    LVec mask;
    FunctionAffine<T,S,L> f;
    KMat::GenMatrix<T,S,L> AtP;
    KMat::GenMatrix<T,L,1> Pvec;

  };
};

#endif // FUNCTIONQUADRATICPENALTY_HPP
