#ifndef FILTER_H
#define FILTER_H
#include "../tuple.h"
#include "../../narukom_common.h"
class Filter{
  public:
    Filter(const std::string& type_name,bool complement =false);// : type(type_name){}
    virtual FilterState filter(const Tuple& a_tuple);//{return Unknown;}
    bool operator==(const Filter& another_filter) const;//{return type == another_filter.get_type();}
    std::string get_type() const;//{return type;}
    bool get_complement() const;
    void set_complement(bool value);
    
  private:
    std::string type;
    bool complement;
};
#endif
