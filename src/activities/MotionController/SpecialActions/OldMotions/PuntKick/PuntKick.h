#ifndef _PuntKick_h_
#define _PuntKick_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class PuntKick: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "PuntKick";
    }
    
    PuntKick();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    boost::shared_ptr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> PuntKick_names;
    AL::ALValue PuntKick_times, PuntKick_keys;    
    
}; 

#endif // _PuntKick_h_ 
