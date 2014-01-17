#ifndef PENALTYSOLVER_HPP
#define PENALTYSOLVER_HPP
#include "ITwiceDiffFunction.hpp"
#include <vector>

namespace KMath
{

  template<typename T, unsigned S, template<typename  , unsigned > class K>
  class PenaltySolver : public K<T,S>
  {
  public:
    PenaltySolver() : K<T,S>(), pthreshold(1e2), mu(100)
    {
      pen.clear();
    }
    void clearCostFunctions() { this->cost.clear(); pen.clear();};
    void addPenaltyFunction(IPenaltyFunction<T,S>* p) {pen.push_back(p); addCostFunction(p);};
    void setPenaltyThreshold(T const thres) {pthreshold=thres;};
    void setMu(T const amu) {mu=amu;};
    KMat::GenMatrix<T,S,1> solve(KMat::GenMatrix<T,S,1>  const& xinit)
    {
      KMat::GenMatrix<T,S,1>  x=xinit;
      for(T t=1; t<pthreshold; t=t*mu)
      {
        //std::cout<<"---------------------------------"<<t<<std::endl;
		//x.prettyPrint();
        for(unsigned i=0; i<pen.size(); i++)
        {
          pen[i]->setT(t);
          pen[i]->processConstraintsAt(x);
        }
        x=K<T,S>::solve(x);
      }
      return x;
    }

  protected:

    std::vector<IPenaltyFunction<T,S>* > pen;
    T pthreshold;
    T mu;
  };

};
#endif // PENALTYSOLVER_HPP
