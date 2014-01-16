#ifndef FUNCTIONAFFINE_HPP
#define FUNCTIONAFFINE_HPP

#include "core/elements/math/KMat.hpp"
#include "ICostFunction.hpp"


namespace KMath
{

	template<typename T,unsigned S>
	class FunctionAffine : public ICostFunction<T,S>
	{
		public:
		KMat::GenMatrix<T,S,S> A;
		KMat::GenMatrix<T,S,1> b;



	};


};

#endif // FUNCTIONAFFINE_HPP
