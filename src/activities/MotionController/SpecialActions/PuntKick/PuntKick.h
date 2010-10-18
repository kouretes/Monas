#ifndef _PuntKick_h_
#define _PuntKick_h_ 1

#include "../../ISpecialAction.h"

#include "alptr.h"
#include "alvalue.h"
#include "almotionproxy.h"

class PuntKick: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "PuntKick";
    }
    
    PuntKick();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> PuntKick_names;
    AL::ALValue PuntKick_times, PuntKick_keys;    
    
}; 

#endif // _PuntKick_h_ 
