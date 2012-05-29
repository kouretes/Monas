#include "IProvider.h"


void IProvider::Initialize ( Narukom* com, Blackboard* blk) {
    _com = com;
    _blk = blk;
    this->UserInit();
    this->StartThread();
}
