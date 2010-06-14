#include "IActivity.h"


void IActivity::Initialize ( Narukom* com, Blackboard* blk) {
    _com = com;
    _blk = blk;
    this->UserInit();
}
