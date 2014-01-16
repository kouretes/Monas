#ifndef CONJUGATEGRADIENTSOLVER_HPP
#define CONJUGATEGRADIENTSOLVER_HPP
#include "ITwiceDiffFunction.hpp"
#include <vector>

namespace KMath {

  template<typename T, unsigned S>
  class ConjugateGradientSolver
  {
  public:
    ConjugateGradientSolver() : threshold(1)
    {
      cost.clear();
    }
    void clearCostFunctions() {cost.clear();};
    void addCostFunction(ITwiceDiffFunction<T,S>* c) {cost.push_back(c);};
    void setThreshold(T const thres) {threshold=thres;};

    KMat::GenMatrix<T,S,1> solve(KMat::GenMatrix<T,S,1>  const& xinit)
    {
      //T valOld;
      //T valNew;
      T tau=1;
      T c=0;
      KMat::GenMatrix<T,S,1>  x=xinit,dx;
      KMat::GenMatrix<T,1,S> jacob;
      KMat::GenMatrix<T,S,S> hess;
      bool firstrun=true;
      bool solution=false;
      dx.zero();


      while(!solution)
      {
        firstrun=true;
        c=0;
        dx.zero();
        for(unsigned j=0; j<S; j++)
        {
          std::cout<<"Iter"<< j <<std::endl;
          x.prettyPrint();
          jacob.zero();
          hess.zero();
          setX(x);
          for(unsigned i=0; i<cost.size(); i++)
          {
            jacob+=cost[i]->getJacobian();
            std::cout<<"-- :"<<jacob.norm2()<<std::endl;
            hess+=cost[i]->getHessian();
          }
          std::cout<<jacob.norm2()*1e10<<std::endl;

          if(jacob.norm2()<threshold ) {solution=true;  break; };
          if(!firstrun)
          {
            c=(T(jacob*hess*dx))/(T(((hess*dx).transp())*dx));

          }
          firstrun=false;
          std::cout<<"dxbef:"<<dx(0)<<std::endl;
          std::cout<<"grad:"<<jacob(0,0)<<std::endl;
          std::cout<<"c:"<<c<<std::endl;
          dx=dx*c-jacob.transp();


          T denom=((hess*dx).transp())*dx;
          if(denom<threshold ) {solution=true; break;}
          std::cout <<"denom:"<<denom<<std::endl;
          tau = - (T(jacob*dx)) / denom;
          std::cout<<"tau:"<<tau<<std::endl;
          std::cout<<"dx:"<<dx(0)<<std::endl;
          x+=dx*tau;
          std::cout<<"x:"<<x(0)<<std::endl;

        }

      }
      //std::cout<<"CGx:"<<x(0)<<std::endl;
      return x;

    }

  protected:
    void setX(KMat::GenMatrix<T,S,1> const& x)
    {
      //T r=0;
      for(unsigned i=0; i<cost.size(); i++)
      {
        cost[i]->setX(x);
        //T current=cost[i]->evaluate();
        //r+=current;
        //std::cout<<"---  "<<current<<std::endl;
      }
      //return r;
    }
    std::vector<ITwiceDiffFunction<T,S>* > cost;
    T threshold;

  };

};
#endif // CONJUGATEGRADIENTSOLVER_HPP
