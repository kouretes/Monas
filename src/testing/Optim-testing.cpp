#include <math.h>
#include "core/elements/math/KMat.hpp"
#include "core/elements/math/optimization/FunctionQuadraticPenalty.hpp"
#include "core/elements/math/optimization/PenaltySolver.hpp"
#include "core/elements/math/optimization/PenaltyQuadraticSolver.hpp"
#include "core/elements/math/optimization/ConjugateGradientSolver.hpp"
#include "core/elements/math/optimization/NewtonSolver.hpp"
#include "core/elements/math/optimization/FunctionAffine.hpp"
#include "core/elements/math/optimization/FunctionQuadratic.hpp"
#include "core/elements/math/optimization/FunctionQuadraticSymmetric.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <time.h>
#include <iostream>

using namespace std;
using namespace KMath;
using namespace KMath::KMat;
#define VAR 30
#define CON 6
int main()
{
  srand((NULL));
  FunctionAffine<float, VAR,CON> af;
  FunctionQuadraticSymmetric<float,VAR>    qf;
  GenMatrix<float, CON,VAR> A;
  GenMatrix<float, VAR,1> x;
  GenMatrix<float, CON,1> b;

  qf.setH(GenMatrix<float, VAR,VAR>().identity());
  qf.setf(GenMatrix<float, VAR,1>().zero());

  //for(unsigned i=0; i<CON; i++)
  //  for(unsigned j=0; j<VAR; j++)
  // A(i,j)=(1.0+j*j-i*j)/10.0;

  A.identity();
  b.zero();

  A(0,0)=10;
 // A(0,2)=1;
  //A(2,2)=-1;
  A(5,4)=2;
  A(0,4)=2;
  A(5,0)=-1;
  b(0)=-1;
  b(2)=-10;
  b(5)=-1;
  b.scalar_add(0);

  //A(3,2)=-1;
  //A(0,2)=-1;

  af.setA(A);
  af.setb(b);
  A.prettyPrint();
  b.prettyPrint();
  x.zero();
  x.scalar_add(100);
  //af.setX(x);
  //af.evaluate().prettyPrint();


  FunctionQuadraticPenalty<float,VAR,CON> p(af);

  //p.getJacobian().prettyPrint();
  //p.getHessian().prettyPrint();

  //p.getJacobian().prettyPrint();
  //p.getHessian().prettyPrint();

  //PenaltySolver<float,VAR,NewtonSolver> solv;
  PenaltyQuadraticSolver<float,VAR,CON> solv;


  solv.addPenaltyFunction(&p);
  //solv.addPenaltyFunction(&p);
  //solv.addPenaltyFunction(&p);
  solv.addCostFunction(&qf);
  ptime start,end;
  start=microsec_clock::universal_time();

  for(unsigned k=0; k<10000; k++)
  {
      solv.solve(x);


  }



  end=microsec_clock::universal_time();

  cout << "Solve took " << (end - start).total_microseconds() << " microseconds " << endl;
  x=solv.solve(x);

  x.prettyPrint();

  af.setX(x);
  p.setX(x);
  p.processConstraintsAt(x);
  (A*x-b).prettyPrint();
  af.evaluate().prettyPrint();
  qf.setX(x);
  std::cout<<qf.evaluate()<<std::endl;

  return 0;





  return 0;
}
