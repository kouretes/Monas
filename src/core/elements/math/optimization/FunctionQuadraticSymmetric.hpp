#ifndef FUNCTIONQUADRATICSYMMETRIC_HPP
#define FUNCTIONQUADRATICSYMMETRIC_HPP

#include "ITwiceDiffFunction.hpp"


namespace KMath
{
  template< typename T, unsigned S>
  class FunctionQuadraticSymmetric : public virtual ITwiceDiffFunction<T,S>
  {
  public:
    void setH( KMat::GenMatrix<T,S,S> const& h) {H2=h*2;};
    void setf( KMat::GenMatrix<T,S,1> const& af) {f=af;};
    KMat::GenMatrix<T,S,S> getH() const {return H2*0.5;};
    KMat::GenMatrix<T,S,1> getf() const {return f;};
    T evaluate() const
    {
      return ((H2*this->x).transp())*(this->x*0.5)+f.transp()*this->x;
    };
    KMat::GenMatrix<T,1,S> getJacobian() const
    {
      return (H2*(this->x)+f).transp();
    };
    KMat::GenMatrix<T,S,S> getHessian() const
    {
      return H2;
    };

  protected:
    KMat::GenMatrix<T,S,S> H2;
    KMat::GenMatrix<T,S,1> f;


  };

};

#endif // FUNCTIONQUADRATICSYMMETRIC_HPP
