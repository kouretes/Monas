
#ifndef _RightFall_h_
#define _RightFall_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"

class RightFall : public IActivity, public Publisher {

public:

        RightFall();

	int Execute ();

	void UserInit ();

	std::string GetName ();

};

#endif // _RightFall_h_
