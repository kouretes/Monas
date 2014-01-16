#ifndef FUNCTIONQUADRATIC_HPP
#define FUNCTIONQUADRATIC_HPP

#include "ITwiceDiffFunction.hpp"


namespace KMath
{
  template< typename T, unsigned S>
  class FunctionQuadratic : public virtual ITwiceDiffFunction<T,S>
  {
  public:
    void setH( KMat::GenMatrix<T,S,S> const& h) {H=h;};
    void setf( KMat::GenMatrix<T,S,1> const& af) {f=af;};
    KMat::GenMatrix<T,S,S> getH() const {return H;};
    KMat::GenMatrix<T,S,1> getf() const {return f;};
    T evaluate() const
    {
      return ((H*this->x).transp())*this->x+f.transp()*this->x;
    };
    KMat::GenMatrix<T,1,S> getJacobian() const
    {
      return (H*this->x+H.transp()*this->x+f).transp();
    };
    KMat::GenMatrix<T,S,S> getHessian() const
    {
    	return H+H.transp();
		};

  protected:
    KMat::GenMatrix<T,S,S> H;
    KMat::GenMatrix<T,S,1> f;


  };

};

#endif // FUNCTIONQUADRATIC_HPP
