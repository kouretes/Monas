#include "KMat.h"
#include <iostream>

using namespace std;
using namespace KMat;
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
int main ()
{
	srand(time(NULL));

	//cout<<sizeof(GenMatrix<char,3,3>)<<endl;
	GenMatrix<float,4,4> testS,res,res2 ;


	matrix<float> m(4,4), foo(4,4),foo2(4,4);

	float sum=0;
	ptime end,start;

	for (int j = 0; j < 4; j++)
		for (int k = 0; k < 4; k++)
			m.insert_element(j,k,testS(j,k)=((rand()%11)-5)/8.0);

	end = microsec_clock::universal_time();
	start = microsec_clock::universal_time();

	for (int i=0;i<100000;i++)
	{
		//for (int j=0;j<10;j++)
		//test.identity().scalar_mult(3);
		//test(1,1)=0;
		res=testS;

		res2=res;
		//res2.prettyPrint();
		for(int p=0;p<10;p++)
		{
			//for(int j=0;j<1;j++)
				res*=testS;
			//res.prettyPrint();
		}
		res2.add(res);
		for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
					sum+=res2(j,k);
//

	}
	//testS.prettyPrint();

	end = microsec_clock::universal_time();
	cout<<sum<<endl;
	cout << "10000x1000 m=m*m took " << (end-start).total_microseconds() << " microseconds " << endl;




	sum=0;
	start = microsec_clock::universal_time();
	for (int i=0;i<100000;i++)
	{


		foo=m;
		foo2=foo;

		for(int j=0;j<10;j++)
			foo=prod(foo,m);

		foo2+=foo;

		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				sum+=foo2.at_element(j,k);


	}
	end = microsec_clock::universal_time();
	cout<<sum<<endl;
	cout << "10000x1000 m=m*m took " << (end-start).total_microseconds() << " microseconds " << endl;


	return 0;
}
