#include "IActivity.h"

IActivity::IActivity ( Blackboard & b) :  _blk(b){
    this->UserInit();
}
