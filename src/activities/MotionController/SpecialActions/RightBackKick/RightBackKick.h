#ifndef _RightBackKick_h_
#define _RightBackKick_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/aldebaran-kme.h"

class RightBackKick: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "RightBackKick";
    }
    
    RightBackKick();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> RightBackKick_names;
    AL::ALValue RightBackKick_times, RightBackKick_keys;
    
}; 

#endif // _RightBackKick_h_ 
