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
    PenaltyQuadraticSolver()  : pthreshold(5e-4)
    {
      pen.clear();
      //cost.clear();
     // costs.clear();
    };
    void clearCostFunctions() { pen.clear();};
    //void addCostFunction(FunctionQuadratic<T,S>* c) {cost.push_back(c);};
   // void addCostFunction(FunctionQuadraticSymmetric<T,S>* c) {costs.push_back(c);};
    void addPenaltyFunction(FunctionQuadraticPenalty<T,S,L>* p) {pen.push_back(p);};
    void setPenaltyThreshold(T const thres) {pthreshold=thres;};
    KMat::GenMatrix<T,S,1> solve(KMat::GenMatrix<T,S,S>  const& H,
								 KMat::GenMatrix<T,S,S>  const& Hinv,
								 KMat::GenMatrix<T,S,1>  const& finit,
								  KMat::GenMatrix<T,S,1>  const& xinit)
    {
      KMat::GenMatrix<T,S,1>  x=xinit,Ai,displ,tx,xstar;
      KMat::GenMatrix<T,S,1> f=finit;
      //KMat::GenMatrix<T,S,S>  temp;
      T bi;
      bool allsatisfied=false;

		x=Hinv*f;
		//KMat::solveSystemGaussSeidel(H,f,x,(float)1e-4);
		x.scalar_mult(-1);
	  //std::cout<<"init"<<std::endl;
      //x.prettyPrint();
      //xinit.prettyPrint();
      //(x-xinit).prettyPrint();
      //xinit.prettyPrint();
	  //x=xinit;



      for(unsigned iter=0; iter<2*L*pen.size(); iter++)
      {

        //x.prettyPrint();
        //Find Active Constraints
        allsatisfied=true;
        for(unsigned i=0; i<pen.size(); i++)
        {


          //pen[i]->setT(1);
          //pen[i]->replaceConstraintsAt(x);
          int m=pen[i]->getViolatedConstraint(x,pthreshold);
          if(m>=0)
		  {

		  	allsatisfied=false;
			Ai=pen[i]->getConstraint(m,bi);
			//Ai.transp().prettyPrint();
			//std::cout<<"bi:"<<bi<<std::endl;
			//std::cout<<"err:"<<(Ai.transp()*x-bi)<<std::endl;
			//(Hinv*H).prettyPrint();
			//(Hinv*Ai).prettyPrint();
			double nom=-(x.transp()*Ai)(0);
			double denom=((Ai.transp()*Hinv)*Ai)(0) ;
			//std::cout<<"Scale1:"<<(nom+bi)/denom<<std::endl;
			//std::cout<<"Scale2:"<<nom/denom+bi/(denom)<<std::endl;
			displ=(Hinv*Ai)* (nom/denom+bi/(denom) )  ;
			xstar=x+displ;

			//std::cout<<"Catstar:"<<Ai.transp()*xstar-bi<<std::endl;
			//(x-xstar).prettyPrint();
			double nom2=bi-Ai.transp()*x;
			double denom2=Ai.transp()*displ;

			//std::cout<<"nom2:"<<nom2<<std::endl;
			//std::cout<<"denom2:"<<denom2<<std::endl;
			double alpha=(pthreshold*0.9+nom2)/(denom2);//%+1e-20;
			//if(alpha>1)alpha=1;

			tx=x+displ*alpha;
			double k=xstar.transp()*H*xstar+f.transp()*xstar;
			double l=tx.transp()*H*tx+f.transp()*tx;
			double p= Ai.transp()*tx-bi;


			double  t=fabs((k-l)/(p));

			//std::cout<<"t:"<<t<<std::endl;
			if(t<pthreshold) t= pthreshold;
			//if(t<pthreshold) t=pthreshold;
			f+=Ai*t;
			//pen[i]->replaceConstraint(m,t);
			//std::cout<<"nom:"<<nom<<std::endl;
			//std::cout<<"denom:"<<denom<<std::endl;
			//std::cout<<"alpha:"<<alpha<<std::endl;
			//std::cout<<"k:"<<k<<std::endl;
			//std::cout<<"l:"<<l<<std::endl;
			//::cout<<"p:"<<p<<std::endl;
			//std::cout<<"t:"<<t<<std::endl;
			break;


		  }
          //allsatisfied=allsatisfied&pen[i]->allSatisfied();
        }
        if(allsatisfied)
		{
				if(iter>0)
					std::cout<<"-------------------"<<iter<<std::endl;
			break;
		}
        //Solve analytically

        x=Hinv*f;
        //KMat::solveSystemGaussSeidel(H,f,x,(float)1e-3);
		x.scalar_mult(-1);
        //x.prettyPrint();
      }



      return x;
    }

  protected:
    //std::vector<FunctionQuadratic<T,S> *> cost;
    //std::vector<FunctionQuadraticSymmetric<T,S> *> costs;
    std::vector<FunctionQuadraticPenalty<T,S,L>* > pen;
    T pthreshold;
  };

};
#endif // PENALTYQUADRATICSOLVER_HPP

