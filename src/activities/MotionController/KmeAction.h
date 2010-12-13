#ifndef _KMEACTION_H_
#define _KMEACTION_H_ 1

#include "ISpecialAction.h"

#include <string>

#include "hal/robot/generic_nao/aldebaran-kme.h"

class KmeAction : public ISpecialAction {

  public:

    std::string GetName () {
      return name;
    }

    KmeAction(std::string name, AL::ALValue actionNames,
              AL::ALValue actionAngles, AL::ALValue actionTimes );

    int ExecutePost();

  private:

    int ExecuteActionKME();

    int ExecuteActionBodyKME();

//     int PrintActionsKME();

    std::string name;

    AL::ALPtr<AL::ALMotionProxy> motion;

    AL::ALValue actionNames, actionAngles, actionTimes;

};

#endif // _KMEACTION_H_
