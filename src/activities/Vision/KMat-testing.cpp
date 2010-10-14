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



using namespace boost::math;
using namespace boost::posix_time;
int main ()
{


    KMat::ATMatrix<float,4> y,z,f;
	KMat::transformations::rotateY(y,0.69f);
	KMat::transformations::rotateZ(z,(float)KMat::transformations::PI/2);

	KMat::transformations::rotateX(f,0.04f);//4f);
	f.mult(z).mult(y);
	//chain
	//y.mult(z);//.invert();
	HCoords<float,3> p;
	p(1)=1;
	p(2)=0;//0;tan(0.14);
	p(3)=0;//tan(0.14);
    HCoords<float,3> &r=f.transform(p);
    r.prettyPrint();
	ATMatrix<float,4> t;
	ATMatrix<float,4> a;
	a.identity();
	//transformations::rotateZ(a,(float)transformations::PI/2);
	transformations::rotateY(t,-(float)transformations::PI/4);

	a.mult(t);
	HCoords<float,3> coords;
	coords.zero();
	coords(1)=transformations::PI/4;
	coords.prettyPrint();
	HCoords<float,3> &n=a.transform(coords);
	n.prettyPrint();




	GenMatrix<float,3,3> testS ;
	for (int i = 1; i <= 3; i++)
		for (int k = 1; k <= 3; k++)
			testS(i,k)=i+k;
	//testS.identity();
	//cout<<sizeof(GenMatrix<char,3,3>)<<endl;
	ptime start = microsec_clock::universal_time();
	for (int i=0;i<100000;i++)
	{
		//for (int j=0;j<10;j++)
		//test.identity().scalar_mult(3);
		//test(1,1)=0;
		testS.mult(testS);
	}
	ptime end = microsec_clock::universal_time();

	cout << "1000000 m*m took " << (end-start).total_microseconds()/(float)100 << " microseconds " << endl;



	using namespace boost::numeric::ublas;

	matrix<float> m(3,3), *foo = new matrix<float>(3,3);
	for (int i = 0; i < 3; i++)
		for (int k = 0; k < 3; k++)
			m.insert_element(i,k,(float)i+k);

	start = microsec_clock::universal_time();
	for (int j =0; j < 100000; j++)
		*foo=prod(m,m);
	end = microsec_clock::universal_time();

	cout << "1000000 m*m took " << (end-start).total_microseconds()/(float)100 << " microseconds " << endl;


	return 0;
}
