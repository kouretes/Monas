#ifndef _representation_h_
#define _representation_h_ 1

#include <string>

#include "architecture/blackboard.h"

class Representation {

    public:
      
      Representation( const std::string RepName , Blackboard * blk) 
            : RepName( RepName ) , blk( blk ) {
          blk->Representations[RepName] = this; }//FIXME

      virtual ~Representation() {
          blk->Representations.erase( RepName );  
      }

      std::string GetRepName () { return RepName; }

    private:

      const std::string RepName;

      Blackboard * const blk;

};

#define DEFAULT_REP_CONSTRUCTOR(TYPE) \
          TYPE ( Blackboard * blk ) : Representation( #TYPE, blk ) { ; }

#endif  // _representation_h_
