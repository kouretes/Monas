#include <iostream>
#include "tools/stat/kalman.h"

using namespace std;
using namespace KMath;
int main()
{
	Kalman1D<float> test;
	test.init(1);
	test.reset(0, 10);

	while(1)
	{
		Kalman1D<float>::Xbar t;
		t = test.read();
		cout << "Xbar:" << t(0) << "," << t(1) << endl;
		float r;
		cin >> r;
		test.predict( 0.03);
		test.update(r, r * r * 0.25);
	}
}
