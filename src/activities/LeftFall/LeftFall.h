
#ifndef _LeftFall_h_
#define _LeftFall_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"

class LeftFall : public IActivity, public Publisher {

public:

        LeftFall();

	int Execute ();

	void UserInit ();

	std::string GetName ();

};

#endif // _LeftFall_h_
