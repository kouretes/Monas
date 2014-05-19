#ifndef MessageEntrySERIALIZE_HPP
#define MessageEntrySERIALIZE_HPP

#include "ptimeserialize.hpp"
#include "core/include/MessageEntry.hpp"



#pragma pack(push,1) //Force continuity
struct serializedmsgheader
{
	uint32_t topicid;


	int64_t timestamp;
	unsigned char msgclass;

	uint16_t typeData;///Either as uinque identifier or as a length prefix for the GetTypeName() string that follows, currently the latter
	void encodeFromMsg(Messaging::MessageEntry const& m, uint16_t atypeData)
	{
		topicid = htonl(m.topic);
		timestamp = serialize_ptime(m.timestamp);
		msgclass = m.msgclass;
		typeData = htons(atypeData);
	}

	Messaging::MessageEntry decodeMsg() const
	{
		Messaging::MessageEntry en;
		en.topic = ntohl(topicid);
		en.timestamp = deserialize_ptime(timestamp);
		en.msgclass = (Messaging::MessageEntry::msgclass_t)msgclass;
		return en;
	}
	uint16_t getTypeData() const
	{
		return ntohs(typeData);
	}



};
#pragma pack(pop)

#endif /*MessageEntrySERIALIZE_H*/
