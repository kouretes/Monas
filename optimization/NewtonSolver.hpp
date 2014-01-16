#ifndef NEWTONSOLVER_HPP
#define NEWTONSOLVER_HPP
#include "ICostFunction.hpp"
#include <vector>

namespace KMath {

	template<typename T, unsigned S>
	class NewtonSolver
	{
		public:




		private:
			ICostFunction c;
			T threashold;

	};

};
#endif // NEWTONSOLVER_HPP
