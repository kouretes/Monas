#include "IAction.h"

namespace statechart_engine {

    void IAction::Initialize ( Narukom* com, Blackboard* blk) {
        _com = com;
        _blk = blk;
        this->UserInit();
    }

}
