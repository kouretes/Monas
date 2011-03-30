#ifndef _KMEMANAGER_H_H
#define _KMEMANAGER_H_H 1

#include <vector>
#include "ISpecialAction.h"
#include <boost/date_time/posix_time/posix_time.hpp>



class KmeManager {

  public:

    static std::vector<ISpecialAction*> LoadActionsKME();
   // static std::vector<ISpecialAction*> LoadActionsKME( AL::ALPtr<AL::ALFrameManagerProxy> frame);

    struct motSequence {
      std::string seqName;
      std::vector< std::vector<float> > seqMotion;
    };

   static boost::posix_time::ptime future_time;

   static void set_end_time(boost::posix_time::ptime time){
	   future_time=time;
   }

   static bool isDCMKmeRunning(){
		if(boost::posix_time::microsec_clock::universal_time()>future_time)
			return false;
		else
			return true;
   }
};

#endif //_KMEMANAGER_H_H
