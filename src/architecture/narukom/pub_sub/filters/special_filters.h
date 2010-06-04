#ifndef SPECIAL_FILTERS_H
#define SPECIAL_FILTERS_H
#include "filter.h"
class AcceptAllFilter: public Filter{
    
  public:
    AcceptAllFilter(const std::string& type_name);
    virtual FilterState filter(const Tuple& a_tuple);//{return Accepted;}
};

class RejectAllFilter: public Filter{
  public:
    RejectAllFilter(const std::string& type_name);
    virtual FilterState filter(const Tuple& a_tuple);//{return Rejected;}
};
class UnknownAllFilter: public Filter{
  public:
    UnknownAllFilter(const std::string& type_name);
    virtual FilterState filter(const Tuple& a_tuple);//{return Rejected;}
};
#endif