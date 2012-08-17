#ifndef _LeftDive_h_
#define _LeftDive_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class LeftDive: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "LeftDive";
    }
    
    LeftDive();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    boost::shared_ptr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> LeftDive_names;
    AL::ALValue LeftDive_times, LeftDive_keys;    
    
}; 

#endif // _LeftDive_h_ 
