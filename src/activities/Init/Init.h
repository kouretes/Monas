
#ifndef _Init_h_
#define _Init_h_ 1

#include "architecture/IActivity.h"
#include "messages/BehaviorMessages.pb.h"			
class Init : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();

private:
	PlayerNumberMessage* pnm;
	int teamColor;
	int playernum;
	bool readConfiguration(const std::string& file_name);
};

#endif // _Init_h_
	
