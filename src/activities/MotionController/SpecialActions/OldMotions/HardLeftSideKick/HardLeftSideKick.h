#ifndef _HardLeftSideKick_h_
#define _HardLeftSideKick_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class HardLeftSideKick: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "HardLeftSideKick";
    }
    
    HardLeftSideKick();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    boost::shared_ptr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> HardLeftSideKick_names;
    AL::ALValue HardLeftSideKick_times, HardLeftSideKick_keys;    
    
}; 

#endif // _HardLeftSideKick_h_ 
