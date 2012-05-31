
#ifndef _Init_h_
#define _Init_h_ 1

#include "architecture/executables/IActivity.h"
#include "messages/BehaviorMessages.pb.h"
ACTIVITY_START

class Init : public IActivity {

public:
    ACTIVITY_CONSTRUCTOR(Init);

	int ACTIVITY_VISIBLE Execute ();

	void ACTIVITY_VISIBLE UserInit ();

	std::string ACTIVITY_VISIBLE GetName ();

private:

	int teamColor;
//	int playernum;
//	bool readConfiguration(const std::string& file_name);
};
ACTIVITY_END
#endif // _Init_h_

