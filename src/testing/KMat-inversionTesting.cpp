#ifdef NDEBUG
#undef NDEBUG
#endif // NDEBUG
#include <assert.h>
#include "core/elements/math/KMat.hpp"
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



template<unsigned M> void doTest()
{
	GenMatrix<float,M,M> r,t,q;

	for(unsigned o=0;o<M;o++)
	{
		for(unsigned p=0;p<M;p++)
		{
			r(o,p)=(10.0*rand())/RAND_MAX -5;

		}
	}
	q=r;
	q.fast_invert();
	r.prettyPrint();
	q.prettyPrint();
	t=q*r;
	(t).prettyPrint();
	assert(testIdentity(t));
	t=r*q;
	(t).prettyPrint();
	assert(testIdentity(t));

};

template<unsigned M> bool testIdentity(GenMatrix<float,M,M> const& a)
{

	for(unsigned o=0;o<M;o++)
	{
		for(unsigned p=0;p<M;p++)
		{
			if(o==p && fabs(a(o,p)-1)>1e-4)
				return false;
			if(o!=p && fabs(a(o,p)>1e-4) )
				return false;

		}
	}
	return true;
}

int main ()
{
	srand(time(NULL));
	//cout<<sizeof(GenMatrix<char,3,3>)<<endl;
	for(unsigned i=0;i<100;i++)
	{
		doTest<2>();
		doTest<3>();
		doTest<4>();
		doTest<5>();
	}


	return 0;
}
