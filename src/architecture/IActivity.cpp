#include "IActivity.h"


void IActivity::Initialize ( Narukom* com, Blackboard* blk) {
    _com = com;
    _blk = blk;
    this->UserInit();
}

int IActivity::ExecuteWrapper ( volatile int* running ) {
    int ret = this->Execute();
    *running = 0;
    return ret;
}



