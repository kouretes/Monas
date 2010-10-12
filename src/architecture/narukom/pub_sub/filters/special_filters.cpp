#include "special_filters.h"

AcceptAllFilter::AcceptAllFilter(const std::string& type_name): Filter(type_name)
{

}
FilterState AcceptAllFilter::filter(const msgentry& a_tuple)
{
    return Accepted;
}

FilterState RejectAllFilter::filter(const msgentry& a_tuple)
{
    return Rejected;
}

RejectAllFilter::RejectAllFilter(const std::string& type_name): Filter(type_name)
{

}

FilterState UnknownAllFilter::filter(const msgentry& a_tuple)
{
    return Unknown;
}

UnknownAllFilter::UnknownAllFilter(const std::string& type_name): Filter(type_name)
{

}
