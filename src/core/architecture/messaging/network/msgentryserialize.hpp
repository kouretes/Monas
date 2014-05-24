#ifndef MessageEntrySERIALIZE_HPP
#define MessageEntrySERIALIZE_HPP

#include "core/elements/BasicSerialization.hpp"
#include "core/include/MessageEntry.hpp"

/*

#pragma pack(push,1) //Force continuity
struct serializedmsgheader
{
	uint32_t topicid;


	int64_t timestamp;
	unsigned char msgclass;

	uint16_t typeData;///Either as uinque identifier or as a length prefix for the GetTypeName() string that follows, currently the latter
	void encodeFromMsg(Messaging::MessageEntry const& m, uint16_t atypeData)
	{
		topicid = KSystem::BasicSerialization::serialize(m.topic);
		timestamp = KSystem::BasicSerialization::serialize(m.timestamp);
		msgclass = m.msgclass;
		typeData = KSystem::BasicSerialization::serialize(atypeData);
	}

	Messaging::MessageEntry decodeMsg() const
	{
		Messaging::MessageEntry en;
		en.topic = KSystem::BasicSerialization::deserialize(topicid);
		en.timestamp = KSystem::BasicSerialization::deserialize(timestamp);
		en.msgclass = (Messaging::MessageEntry::msgclass_t)msgclass;
		typeData=KSystem::BasicSerialization::deserialize(typeData);
		return en;
	}
	uint16_t getTypeData() const
	{
		return typeData;
	}



};
#pragma pack(pop)
*/
#endif /*MessageEntrySERIALIZE_H*/
