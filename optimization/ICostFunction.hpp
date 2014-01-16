#ifndef COSTFUNCTION_HPP
#define COSTFUNCTION_HPP
#include "core/elements/math/KMat.hpp"

namespace KMath {


	template<typename T,unsigned S>
	ICostFunction
	{
	public:
		virtual void setX(KMat::GenMatrix<T,S,1> &x);
		virtual T evaluate() 	const;
		virtual KMat::GenMatrix<T,S,1> getGrad() const;
		virtual KMat::GenMatrix<T,S,S> getHess() const;

		virtual ~ICostFunction() =0;
	protected:
		ICostFunction() {};

	};



};

#endif // COSTFUNCTION_HPP
