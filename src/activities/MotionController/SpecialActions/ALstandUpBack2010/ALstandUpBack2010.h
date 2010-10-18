#ifndef _ALstandUpBack2010_h_
#define _ALstandUpBack2010_h_ 1

#include "../../ISpecialAction.h"

#include "alptr.h"
#include "alvalue.h"
#include "almotionproxy.h"

class ALstandUpBack2010: public ISpecialAction {
  
  public:
    std::string GetName () {
      return "ALstandUpBack2010";
    }
    
    ALstandUpBack2010();
    
    int ExecutePost();
    
  private:
    
    void Initialize();
    
    AL::ALPtr<AL::ALMotionProxy> motion;
    
    AL::ALValue names, times, keys;    
    
}; 

#endif // _ALstandUpBack2010_h_ 
