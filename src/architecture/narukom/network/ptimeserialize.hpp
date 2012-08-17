#ifndef PTIMESERIALIZE_H
#define PTIMESERIALIZE_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <endian.h>
#include <bits/byteswap.h>

const boost::posix_time::ptime serializeepoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));



# if __BYTE_ORDER == __BIG_ENDIAN
#define htonll(x) (x)
#define ntohll(x) (x)
#else
#define htonll(x) __bswap_64(x)
#define ntohll(x) __bswap_64(x)
#endif



inline int64_t serialize_ptime(boost::posix_time::ptime const& t)
{
	boost::posix_time::time_duration d = serializeepoch - t;
	return htonll(d.total_microseconds());
};

inline boost::posix_time::ptime deserialize_ptime(int64_t const& l)
{
	return serializeepoch + boost::posix_time::microseconds(ntohll(l));
};


#endif /*PTIMESERIALIZE_H*/
