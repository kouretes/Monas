#ifndef IDIFFFUNCTION_HPP
#define IDIFFFUNCTION_HPP
#include "core/elements/math/KMat.hpp"

namespace KMath
{
  template< typename T, unsigned S, unsigned L>
  class IDiffFunction
  {
  public:
    virtual void setX(KMat::GenMatrix<T,S,1> const& ax) {x=ax;};
    virtual KMat::GenMatrix<T,L,1> evaluate()  const =0;
    virtual KMat::GenMatrix<T,L,S> getJacobian() const =0;
    virtual ~IDiffFunction()  {}; //Pure virtual destructor requires a function body
  protected:
    IDiffFunction() {};
    KMat::GenMatrix<T,S,1> x;

  };

  //Partial Specialization for scalar function
  template< typename T, unsigned S>
  class IDiffFunction<T,S,1>
  {
  public:
    virtual void setX(KMat::GenMatrix<T,S,1> const& ax) {x=ax;};
    virtual T evaluate()  const =0;
    virtual KMat::GenMatrix<T,1,S> getJacobian() const =0;
    virtual ~IDiffFunction()  {}; //Pure virtual destructor requires a function body
  protected:
    IDiffFunction() {};
    mutable KMat::GenMatrix<T,S,1> x;

  };
};
#endif // IDIFFFUNCTION_HPP
