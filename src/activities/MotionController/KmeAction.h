#ifndef _KMEACTION_H_
#define _KMEACTION_H_ 1

#include "ISpecialAction.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "hal/robot/generic_nao/aldebaran-kme.h"

class KmeAction : public ISpecialAction {

  public:

    std::string GetName () {
      return name;
    }

    KmeAction(std::string name, AL::ALValue actionNames,
              AL::ALValue actionAngles, AL::ALValue actionTimes );

    int ExecutePost();
    AL::ALValue ReturnALValues();
    boost::posix_time::ptime ExecuteDCM();

    boost::posix_time::ptime ExecuteFrameDCM(unsigned int frameStart, unsigned int frameEnd);

  private:

    int ExecuteActionKME();

    int ExecuteActionBodyKME();

   void DcmInit();


   //     int PrintActionsKME();
    AL::ALValue commands;

    std::string name;

    boost::shared_ptr<AL::ALMotionProxy> motion;
	AL::DCMProxy *dcm;

    AL::ALValue actionNames, actionAngles, actionTimes;

};

#endif // _KMEACTION_H_
