#ifndef _HardRightSideKick_h_
#define _HardRightSideKick_h_ 1

#include "../../ISpecialAction.h"

#include "alptr.h"
#include "alvalue.h"
#include "almotionproxy.h"

class HardRightSideKick: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "HardRightSideKick";
    }
    
    HardRightSideKick();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> HardRightSideKick_names;
    AL::ALValue HardRightSideKick_times, HardRightSideKick_keys;    
    
}; 

#endif // _HardRightSideKick_h_ 
