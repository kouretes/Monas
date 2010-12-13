#ifndef _RightDive_h_
#define _RightDive_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"


class RightDive: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "RightDive";
    }
    
    RightDive();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> RightDive_names;
    AL::ALValue RightDive_times, RightDive_keys;     
    
}; 

#endif // _RightDive_h_ 
