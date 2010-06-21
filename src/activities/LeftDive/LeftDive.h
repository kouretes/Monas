
#ifndef _LeftDive_h_
#define _LeftDive_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"

class LeftDive : public IActivity, public Publisher {

public:

        LeftDive();

	int Execute ();

	void UserInit ();

	std::string GetName ();

};

#endif // _LeftDive_h_
