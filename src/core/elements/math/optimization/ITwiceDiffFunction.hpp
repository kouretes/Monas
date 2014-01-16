#ifndef ITWICEDIFFFUNCTION_HPP
#define ITWICEDIFFFUNCTION_HPP
#include "core/elements/math/KMat.hpp"
#include "IDiffFunction.hpp"

namespace KMath {


  template<typename T,unsigned S>
  class ITwiceDiffFunction : public virtual IDiffFunction<T,S,1>
  {
  public:
    virtual KMat::GenMatrix<T,S,S> getHessian() const =0;

    virtual ~ITwiceDiffFunction()  {}; //Pure virtual destructor requires a function body
  protected:
    ITwiceDiffFunction() {};

  };
};

#endif // ITWICEDIFFFUNCTION_HPP
