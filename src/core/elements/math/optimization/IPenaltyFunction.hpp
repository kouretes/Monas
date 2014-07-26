#ifndef IPENALTYFUNCTION_HPP
#define IPENALTYFUNCTION_HPP

#include "ITwiceDiffFunction.hpp"
namespace KMath
{

	template<typename T,unsigned S>
  class IPenaltyFunction : public virtual ITwiceDiffFunction<T,S>
  {
  public:
		void setT(T at) {t=at;};
		virtual void processConstraintsAt(KMat::GenMatrix<T,S,1> &x ) =0;
		virtual bool allSatisfied()=0;
  protected:
		IPenaltyFunction() : t(1){};
		T t;

  };

};

#endif // IPENALTYFUNCTION_HPP
