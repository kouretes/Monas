#include "tools/KMat.h"
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



class inttracer_s: public KVecInt2
{
	int idx,idy;
	int e,l,k;//Error, lower limit, upperlimit
	char sx,sy;//Steps : +/-1
	//int idx,idy;

	public:
	void init(int a,int b)
	{
		x=a;
		y=b;
		e=0;
	};
	void initVelocity(int dx,int dy)
	{
		sx=dx>0?1:-1; //if zero,
		sy=dy>0?1:-1; //step is irrelavamt, step is never reached

		idx=abs(dx);
		idy=abs(dy);
		l=-(-2*idy+idx);//=-2*L as in analysis
		k=-(2*idx-idy);//=-2*K as in analysis
		e=0;

	}
	void initVelocity(float dx,float dy)
	{
		int idx=dx*32768;
		int idy=dy*32768;

		initVelocity(idx,idy);
	};
	void step()
	{
		int e2=e*2;
		if(e2<=l)
		{
			y+=sy;
			e+=idx;
		}
		if(e2>=k)
		{
			x+=sx;
			e-=idy;
		}
	}

	void r_step() //Reverse step
	{
		int e2=e*2;
		if(e2>=-l)
		{
			y-=sy;
			e-=idx;
		}
		if(e2<=-k)
		{
			x-=sx;
			e+=idy;
		}
	}

	void steps(unsigned s)
	{

		while(s>0)
		{
			step();
			s--;
		}
	}
	void r_steps(unsigned s)
	{

		while(s>0)
		{
			r_step();
			s--;
		}
	}

};


typedef struct inttracer_s tracer_t;


int main ()
{
	srand(time(NULL));

	//cout<<sizeof(GenMatrix<char,3,3>)<<endl;
	GenMatrix<float,4,4> testS,res,res2 ;
	matrix<float> m(4,4), foo(4,4),foo2(4,4);
	
	tracer_t t;
	t.init(0,0);
	t.initVelocity(0.5f,0.5f);
	while(true)
	{
		float dx=rand()%6;
		float dy=rand()%6;
		t.initVelocity(dx,dy);
		t.steps(100);
		t.r_steps(100);
		if(t.x!=0||t.y!=0)
		{
			cout<<"ERROR:"<<dx<<","<<dy<<endl;
		}
	}
	
	

	KVecInt2 a,b;
	a.x=5;
	a.y=7;
	b=a;
	std::cout<<(a==b)<<(a!=b)<<std::endl;
	b.x=6;
	std::cout<<(a==b)<<(a!=b)<<std::endl;
	a.prettyPrint();
	b.prettyPrint();
	a-=b;
	a.prettyPrint();
	std::cout<<"Sizes:"<<sizeof(KVecInt2)<<" "<<sizeof(KVecFloat2)<<std::endl;

	float sum=0;
	ptime end,start;

	for (int j = 0; j < 4; j++)
		for (int k = 0; k < 4; k++)
		{
			testS(j,k)=(rand()%6)/6.0;

			m.insert_element(j,k,testS(j,k));
		}


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
