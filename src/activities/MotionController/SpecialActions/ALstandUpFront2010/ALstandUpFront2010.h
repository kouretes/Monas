#ifndef _ALstandUpFront2010_h_
#define _ALstandUpFront2010_h_ 1

#include "../../ISpecialAction.h"

#include "hal/robot/generic_nao/aldebaran-kme.h"

class ALstandUpFront2010: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "ALstandUpFront2010";
    }
    
    ALstandUpFront2010();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    AL::ALValue names, times, keys;     
    
}; 

#endif // _ALstandUpFront2010_h_ 
