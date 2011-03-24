#ifndef _CUMULATIVE_H_
#define _CUMULATIVE_H_
#include <cstddef>
#include "tools/KMat.h"


class sample_counter
{
	public:
	static const size_t MAXI=65534;
	static const size_t MINI=32767;
	size_t i;
	sample_counter() :i(0) {}
	void inc(){ i=i+1>=MAXI?MINI:i+1; }



};
template<typename T>
class cumulative_central_moments
{
	private:
	T avg;
	T sqavg;
	public:
	cumulative_central_moments() : avg(0),sqavg(0){}
	T read_mean() { return avg; }
	T read_squaremean() { return sqavg; }
	T read_var()  {  return fabs(sqavg-avg*avg);}
	void update(const T  v,sample_counter const& s ) { avg+=(v-avg)/((T)s.i); sqavg+=((v*v)-sqavg)/((T)s.i);}

};



#endif //_CUMULATIVE_H_
