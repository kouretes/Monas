#ifndef NEWTONSOLVER_HPP
#define NEWTONSOLVER_HPP
#include "ITwiceDiffFunction.hpp"
#include <vector>

namespace KMath {

  template<typename T, unsigned S>
  class NewtonSolver
  {
  public:
    NewtonSolver() : threshold(1e-4), beta(0.05), alpha(0.001)
    {
      cost.clear();
    }
    void clearCostFunctions() {cost.clear();};
    void addCostFunction(ITwiceDiffFunction<T,S>* c) {cost.push_back(c);};
    void setThreshold(T const thres) {threshold=thres;};
    void setBackTrackBeta(T const b) { beta=b;};
    void setBackTrackAlpha(T const a) { alpha=a;};

    KMat::GenMatrix<T,S,1> solve(KMat::GenMatrix<T,S,1>  const& xinit)
    {
      T valOld;
      T valNew;
      T tau=1;
      KMat::GenMatrix<T,S,1>  x=xinit,xnew,dx,taudx;
      KMat::GenMatrix<T,1,S> jacob;
      KMat::GenMatrix<T,S,S> hess;
      bool shouldbreak=false;

      while(!shouldbreak)
      {
      	//std::cout<<"Loop"<<std::endl;
        jacob.zero();
        hess.zero();
        valOld=evaluateAt(x);
        for(unsigned i=0; i<cost.size(); i++)
        {
          jacob+=cost[i]->getJacobian();
          hess+=cost[i]->getHessian();
        }

		try
        {
		  //KMat::GenMatrix<float,S,S> h=hess;
          //hess.fast_invert();
          KMat::invert_square_symmetric_positive_definite_matrix(hess);
          //(hess*h).prettyPrint();
        }
        catch(KMat::SingularMatrixInvertionException e)
        {
          std::cout<<"Hessian Not invertible"<<std::endl;
          break;

        }

        dx=hess*(jacob.transp());
        dx.scalar_mult(-1); //Descent duh
        //std::cout<<"Dx:"<<std::endl;
        //dx.prettyPrint();
        //Termination
        if(jacob*((jacob*hess).transp())<threshold*2) break;

        //BackTracking
        do
        {

          taudx=dx;
          taudx.scalar_mult(tau);

          //std::cout<<"x"<<std::endl;
          //x.prettyPrint();

					//std::cout<<"tdx"<<std::endl;
          //taudx.prettyPrint();

          xnew=x+taudx;
          //std::cout<<"xnew"<<std::endl;
          //xnew.prettyPrint();

          valNew=evaluateAt(xnew);
          tau=tau*beta;
          //std::cout<<"BACKTRACK "<<tau<<" "<<valOld<<" "<<valNew<<std::endl;
          if(tau<threshold) //STELIO XESE MAS
          {
          	shouldbreak=true;
          	break;
          	//int* p=0;
            //*p=0;
          }
					//(jacob*taudx*alpha ).prettyPrint();

        }
        while ( valOld+jacob*taudx*alpha +threshold*threshold < valNew);

        //std::cout<<"Iter"<<valOld<<" "<<valNew<<std::endl;
       // x.prettyPrint();

        x=xnew;
        //x.prettyPrint();
      }
      evaluateAt(x);
      return x;

    }

  protected:
    T evaluateAt(KMat::GenMatrix<T,S,1> const& x)
    {
      T r=0;
      for(unsigned i=0; i<cost.size(); i++)
      {
        cost[i]->setX(x);
        T current=cost[i]->evaluate();
        r+=current;
        //std::cout<<"---  "<<current<<std::endl;
      }
      return r;
    }
    std::vector<ITwiceDiffFunction<T,S>* > cost;
    T threshold;
    T beta;
    T alpha;

  };

};
#endif // NEWTONSOLVER_HPP
