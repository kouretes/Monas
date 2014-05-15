#ifndef KMEMANAGER_H
#define KMEMANAGER_H

#include <vector>
#include "ISpecialAction.h"
#include "core/architecture/time/SystemTime.hpp"


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

	static KSystem::Time::TimeAbsolute future_time;

	static void set_end_time(KSystem::Time::TimeAbsolute time)
	{
		future_time = time;
	}

	static bool isDCMKmeRunning()
	{
		if(KSystem::Time::SystemTime::now() > future_time)
		{
			//	Logger::Instance().WriteMsg("KmeManager", "isDCMKmeRunning FALSE!!!!  " + _toString(future_time) + " " +  _toString(KSystem::Time::SystemTime::now()), Logger::ExtraInfo);
			return false;
		}
		else
		{
			//Logger::Instance().WriteMsg("KmeManager", "isDCMKmeRunning TRUE!!!!  " + _toString(future_time) + " " +  _toString(KSystem::Time::SystemTime::now()), Logger::ExtraInfo);
			return true;
		}
	}
};

#endif //_KMEMANAGER_H_H
