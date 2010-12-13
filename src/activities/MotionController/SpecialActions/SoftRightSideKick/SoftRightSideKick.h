#ifndef _SoftRightSideKick_h_
#define _SoftRightSideKick_h_ 1

#include "../../ISpecialAction.h"
#include "hal/robot/generic_nao/aldebaran-kme.h"


class SoftRightSideKick: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "SoftRightSideKick";
    }
    
    SoftRightSideKick();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> SoftRightSideKick_names;
    AL::ALValue SoftRightSideKick_times, SoftRightSideKick_keys;     
    
}; 

#endif // _SoftRightSideKick_h_ 
