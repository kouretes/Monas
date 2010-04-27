#include "ICondition.h"

namespace statechart_engine {

    void ICondition::Initialize ( Narukom* com, Blackboard* blk) {
        _com = com;
        _blk = blk;
        this->UserInit();
    }

}
