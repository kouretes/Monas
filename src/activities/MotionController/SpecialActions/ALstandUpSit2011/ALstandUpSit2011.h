#ifndef _ALstandUpSit2011_h_
#define _ALstandUpSit2011_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"


class ALstandUpSit2011: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "ALstandUpSit2011";
    }
    
    ALstandUpSit2011();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    std::vector<std::string> names;
    AL::ALValue times, keys;
    
}; 

#endif // _ALstandUpSit2011_h_ 
