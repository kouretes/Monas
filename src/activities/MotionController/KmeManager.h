#ifndef _KMEMANAGER_H_H
#define _KMEMANAGER_H_H 1

#include <vector>
#include "ISpecialAction.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "tools/logger.h"
#include "tools/toString.h"


class KmeManager
{

public:

	static std::vector<ISpecialAction*> LoadActionsKME();
	// static std::vector<ISpecialAction*> LoadActionsKME( boost::shared_ptr<AL::ALFrameManagerProxy> frame);

	struct motSequence
	{
		std::string seqName;
		std::vector< std::vector<float> > seqMotion;
	};

	static boost::posix_time::ptime future_time;

	static void set_end_time(boost::posix_time::ptime time)
	{
		future_time = time;
	}

	static bool isDCMKmeRunning()
	{
		if(boost::posix_time::microsec_clock::universal_time() > future_time)
		{
			//	Logger::Instance().WriteMsg("KmeManager", "isDCMKmeRunning FALSE!!!!  " + _toString(future_time) + " " +  _toString(boost::posix_time::microsec_clock::universal_time()), Logger::ExtraInfo);
			return false;
		}
		else
		{
			//Logger::Instance().WriteMsg("KmeManager", "isDCMKmeRunning TRUE!!!!  " + _toString(future_time) + " " +  _toString(boost::posix_time::microsec_clock::universal_time()), Logger::ExtraInfo);
			return true;
		}
	}
};

#endif //_KMEMANAGER_H_H
