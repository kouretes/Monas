#include "KMat.h"
#include <iostream>

using namespace std;
using namespace KMat;




int main ()
{
	cout<<"TEST"<<endl;
	
	
	GenMatrix<float,3,3> test ;
	HCoords<float,3> p; //A point
	p.identity();
	p(2)=5;
	p.prettyPrint();
	
	ATMatrix<float,4> th;
	th.zero();
	th.prettyPrint();
	cout<<"Id"<<endl;
	th.identity();
	th.prettyPrint();
	cout<<"Inv"<<endl;
	th.invert();
	th.prettyPrint();
	
	rigid::rotateX(th,(float)rigid::PI/2);
	th.prettyPrint();
	
	cout<<"Transform"<<endl;
	HCoords<float,3> & apoint=th.transform(p);
	apoint.prettyPrint();
	cout<<"mult"<<endl;
	th.mult(th);
	th.prettyPrint();
	


	cout<<sizeof(GenMatrix<char,3,3>)<<endl;
	test.identity().scalar_mult(2);
	//test(1,1)=0;
	test.invert();
	test.prettyPrint();
	test.scalar_add(1.0);
	test.add(test);
	test.prettyPrint();
	test.invert();
	test.prettyPrint();

	return 0;
}
