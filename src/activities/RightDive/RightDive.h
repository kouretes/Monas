
#ifndef _RightDive_h_
#define _RightDive_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"

class RightDive : public IActivity, public Publisher {

public:

        RightDive();

	int Execute ();

	void UserInit ();

	std::string GetName ();

};

#endif // _RightDive_h_
