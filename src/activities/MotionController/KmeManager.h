#ifndef _KMEMANAGER_H_H
#define _KMEMANAGER_H_H 1

#include <vector>
#include "ISpecialAction.h"


class KmeManager {

  public:

    static std::vector<ISpecialAction*> LoadActionsKME();

    struct motSequence {
      std::string seqName;
      std::vector< std::vector<float> > seqMotion;
    };


};

#endif //_KMEMANAGER_H_H
