#include "filter.h"

Filter::Filter(const std::string& type_name, bool value) : type(type_name), complement(value)
{

}
FilterState Filter::filter(const Tuple& a_tuple)
{
  return Unknown;
}

std::string Filter::get_type() const
{
  return type;
}
bool Filter::operator==(const Filter& another_filter) const
{
  return type == another_filter.get_type();
}

bool Filter::get_complement() const
{
  return complement;
}

void Filter::set_complement(bool value)
{
  complement = value;
}
