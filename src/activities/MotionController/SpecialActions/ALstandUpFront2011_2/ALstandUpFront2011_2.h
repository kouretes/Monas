#ifndef _ALstandUpFront2011_2_h_
#define _ALstandUpFront2011_2_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class ALstandUpFront2011_2: public ISpecialAction {

  public:
    std::string GetName () {
      return "ALstandUpFront2011_2";
    }

    ALstandUpFront2011_2();

    int ExecutePost();

  private:

    void Initialize();

    AL::ALPtr<AL::ALMotionProxy> motion;

    std::vector<std::string> names;
    AL::ALValue times, keys;

};

#endif // _ALstandUpFront2011_2_h_
