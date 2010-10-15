#ifndef _LeftBackKick_h_
#define _LeftBackKick_h_ 1

#include "../../ISpecialAction.h"

#include "alptr.h"
#include "alvalue.h"
#include "almotionproxy.h"

class LeftBackKick: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "LeftBackKick";
    }
    
    LeftBackKick();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> LeftBackKick_names;
    AL::ALValue LeftBackKick_times, LeftBackKick_keys;
    
}; 

#endif // _LeftBackKick_h_ 
