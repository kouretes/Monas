#ifndef _toString_h_
#define _toString_h_  1

#include <sstream>
#include <string>

template<class T>
std::string _toString( T val )
{
	std::ostringstream ost;
	ost << val;
	ost.flush();
	return ost.str();
}

#endif // _toString_h_
