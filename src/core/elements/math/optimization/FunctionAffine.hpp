#ifndef FUNCTIONAFFINE_HPP
#define FUNCTIONAFFINE_HPP

#include "core/elements/math/KMat.hpp"
#include "IDiffFunction.hpp"

namespace KMath
{
  template<typename T,unsigned S,unsigned L>
  class FunctionAffine : public virtual IDiffFunction<T,S,L>
  {
  public:
    void setA( KMat::GenMatrix<T,L,S> const& a) {A=a;};
    void setb( KMat::GenMatrix<T,L,1> const& ab) {b=ab;};
	KMat::GenMatrix<T,L,S> getA() const {return A;};
    KMat::GenMatrix<T,L,1> getb() const {return b;};
    KMat::GenMatrix<T,L,1> evaluate() const { return A*this->x-b;}
    KMat::GenMatrix<T,L,S> getJacobian() const {return A;};
  protected:
    KMat::GenMatrix<T,L,S> A;
    KMat::GenMatrix<T,L,1> b;
  };
  //Partial Specialization for L=1 scalar Affines
  template<typename T,unsigned S>
  class FunctionAffine<T,S,1> : public virtual IDiffFunction<T,S,1>
  {
  public:
    void setA( KMat::GenMatrix<T,1,S> const& a) {A=a;};
    void setb( T const& ab) {b=ab;};
    KMat::GenMatrix<T,1,S> getA() const {return A;};
    T getb() const {return b;};
    T evaluate() const { return A*this->x-b;}
    KMat::GenMatrix<T,1,S> getJacobian() const {return A;};
  protected:
    KMat::GenMatrix<T,1,S> A;
    T b;
  };
};

#endif // FUNCTIONAFFINE_HPP
