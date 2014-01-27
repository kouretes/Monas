#ifndef PENALTYQUADRATICSOLVER_HPP
#define PENALTYQUADRATICSOLVER_HPP
#include "FunctionQuadratic.hpp"
#include "FunctionQuadraticSymmetric.hpp"
#include "FunctionQuadraticPenalty.hpp"
#include <vector>

namespace KMath
{

  template<typename T, unsigned S,unsigned L>
  class PenaltyQuadraticSolver
  {
  public:
    PenaltyQuadraticSolver()  : pthreshold(1e30), mu(100)
    {
      pen.clear();
      cost.clear();
      costs.clear();
    };
    void clearCostFunctions() { cost.clear(); pen.clear(); costs.clear();};
    void addCostFunction(FunctionQuadratic<T,S>* c) {cost.push_back(c);};
    void addCostFunction(FunctionQuadraticSymmetric<T,S>* c) {costs.push_back(c);};
    void addPenaltyFunction(FunctionQuadraticPenalty<T,S,L>* p) {pen.push_back(p);};
    void setPenaltyThreshold(T const thres) {pthreshold=thres;};
    void setMu(T const amu) {mu=amu;};
    KMat::GenMatrix<T,S,1> solve(KMat::GenMatrix<T,S,1>  const& xinit)
    {
      KMat::GenMatrix<T,S,1>  x=xinit;
      KMat::GenMatrix<T,S,1> f;
      KMat::GenMatrix<T,S,S>  H,temp;
      bool allsatisfied=false;
      f.zero();
      H.zero();
      //Solve the unconstrained
      /*{

        f.zero();
        H.zero();
        for(unsigned i=0; i<cost.size(); i++)
        {
          f+=cost[i]->getf();
          temp=cost[i]->getH();
          H+=temp;
          H+=temp.transp();
        }
        for(unsigned i=0; i<costs.size(); i++)
        {
          f+=costs[i]->getf();
          H+=costs[i]->getH()*2;
        }

        try
        {
          //H.fast_invert();
          KMat::invert_square_symmetric_positive_definite_matrix(H);
        }
        catch(KMat::SingularMatrixInvertionException e)
        {
          std::cout<<" Not invertible"<<std::endl;
          return x;

        }
        x=H*f;
        //KMat::solveSystemGaussSeidel(H,f,x,(float)1e-4);
        x.scalar_mult(-1);
      }*/




      for(T t=0.1; t*(L*pen.size())<=pthreshold; t=t*mu)
      {
        //std::cout<<"---------------------------------"<<t<<std::endl;
        //x.prettyPrint();
        //Find Active Constraints
        allsatisfied=true;
        for(unsigned i=0; i<pen.size(); i++)
        {
          pen[i]->setT(t);
          pen[i]->replaceConstraintsAt(x);
          allsatisfied=allsatisfied&pen[i]->allSatisfied();
        }
        if(allsatisfied) {  break;}
        //Solve analytically
        f.zero();
        H.zero();
        for(unsigned i=0; i<cost.size(); i++)
        {
          f+=cost[i]->getf();
          temp=cost[i]->getH();
          H+=temp;
          H+=temp.transp();
        }
        for(unsigned i=0; i<costs.size(); i++)
        {
          f+=costs[i]->getf();
          H+=costs[i]->getH()*2;
        }
        for(unsigned i=0; i<pen.size(); i++)
        {
          f+=pen[i]->getf();
          H+=pen[i]->getH()*2;
		}

        try
        {
           //H.fast_invert();
          KMat::invert_square_symmetric_positive_definite_matrix(H);
        }
        catch(KMat::SingularMatrixInvertionException e)
        {
          std::cout<<" Not invertible"<<std::endl;
          break;

        }
        x=H*f;
        //KMat::solveSystemGaussSeidel(H,f,x,(float)1e-3);
        x.scalar_mult(-1);
        //x.prettyPrint();
      }



      return x;
    }

  protected:
    std::vector<FunctionQuadratic<T,S> *> cost;
    std::vector<FunctionQuadraticSymmetric<T,S> *> costs;
    std::vector<FunctionQuadraticPenalty<T,S,L>* > pen;
    T pthreshold;
    T mu;
  };

};
#endif // PENALTYQUADRATICSOLVER_HPP

