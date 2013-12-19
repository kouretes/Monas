#include "core/elements/math/Quaternion.hpp"
#include <iostream>

using namespace std;
using namespace KMath::KMat;
using namespace std;
#include <list>
#include <boost/math/distributions/normal.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdlib>
#include <time.h>

using namespace boost::math;
using namespace boost::posix_time;
using namespace boost::numeric::ublas;




typedef KMath::Quaternion<double> QuaternionDouble;
int main ()
{
	KMath::KMat::GenMatrix<double,3,3> r;
	r.identity();
	QuaternionDouble q;
	q.fromAxisAngle(0,0,0,0);
	q.prettyPrint();
	r*=q.castToMatrix();
	r.prettyPrint();


	q.fromAxisAngle(1,0,0,M_PI);
	q.prettyPrint();
	q.castToMatrix().prettyPrint();
	r*=q.castToMatrix();
	r.prettyPrint();

	q.fromAxisAngle(-1,0,0,M_PI);
	q.prettyPrint();
	q.castToMatrix().prettyPrint();
	r*=q.castToMatrix();
	r.prettyPrint();


	q.fromAxisAngle(1,0,0,-M_PI);
	q.prettyPrint();
	q.castToMatrix().prettyPrint();
	r*=q.castToMatrix();
	r.prettyPrint();

	q.fromAxisAngle(-1,0,0,-M_PI);
	q.prettyPrint();
	q.castToMatrix().prettyPrint();
	r*=q.castToMatrix();

	q.fromAxisAngle(-1,0,5,-M_PI);
	q.prettyPrint();
	q.quaternion_product(q.conjugate());
	q.prettyPrint();
	q.castToMatrix().prettyPrint();
	r*=q.castToMatrix();
	r.prettyPrint();

	return 0;
}
