#ifndef PTIMESERIALIZE_HPP
#define PTIMESERIALIZE_HPP

#include "core/architecture/time/TimeTypes.hpp"
#include <endian.h>
#include <bits/byteswap.h>

const KSystem::Time::TimeAbsolute serializeepoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));



# if __BYTE_ORDER == __BIG_ENDIAN
#define htonll(x) (x)
#define ntohll(x) (x)
#else
#define htonll(x) __bswap_64(x)
#define ntohll(x) __bswap_64(x)
#endif



inline int64_t serialize_ptime(KSystem::Time::TimeAbsolute const& t)
{
	KSystem::Time::TimeDuration d = serializeepoch - t;
	return htonll(d.total_microseconds());
};

inline KSystem::Time::TimeAbsolute deserialize_ptime(int64_t const& l)
{
	return serializeepoch + KSystem::Time::microseconds(ntohll(l));
};


#endif /*PTIMESERIALIZE_H*/
