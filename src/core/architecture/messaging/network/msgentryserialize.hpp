#ifndef MSGENTRYSERIALIZE_H
#define MSGENTRYSERIALIZE_H

#include "ptimeserialize.hpp"
#include "../msg.h"



#pragma pack(push,1) //Force continuity
struct serializedmsgheader
{
	uint32_t topicid;


	int64_t timestamp;
	unsigned char msgclass;

	uint16_t typeData;///Either as uinque identifier or as a length prefix for the GetTypeName() string that follows, currently the latter
	void encodeFromMsg(msgentry const& m, uint16_t atypeData)
	{
		topicid = htonl(m.topic);
		timestamp = serialize_ptime(m.timestamp);
		msgclass = m.msgclass;
		typeData = htons(atypeData);
	}

	msgentry decodeMsg() const
	{
		msgentry en;
		en.topic = ntohl(topicid);
		en.timestamp = deserialize_ptime(timestamp);
		en.msgclass = (msgentry::msgclass_t)msgclass;
		return en;
	}
	uint16_t getTypeData() const
	{
		return ntohs(typeData);
	}



};
#pragma pack(pop)

#endif /*MSGENTRYSERIALIZE_H*/
