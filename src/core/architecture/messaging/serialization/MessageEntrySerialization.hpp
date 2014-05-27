#ifndef MESSAGEENTRYSERIALIZATION_HPP
#define MESSAGEENTRYSERIALIZATION_HPP
#include "core/architecture/messaging/MessageEntry.hpp"
#include "core/elements/BasicSerialization.hpp"

	namespace Messaging
	{

		template<class Traits>
		class MessageEntrySerializer : public Traits
		{
			private:
			const Messaging::MessageEntry * m;
			std::size_t cached_size;

			public:
			MessageEntrySerializer(MessageEntry const& am) : m(&am),cached_size(0) {};
			std::size_t getByteSize()
			{
			   if(m==NULL)
			        return 0;
                std::size_t r=0;
                if(Traits::hasTopic==true)
                    r+=sizeof(typename Traits::topic_t);//TOPIC

                r+=sizeof(MessageEntry::msgclass);//MSG CLass

                if(Traits::hasTimeStamp==true)
                    r+=sizeof(typename Traits::timestamp_t);  //TimeStamp

                r+=sizeof(typename Traits::typename_t);
                if(m->msg) // Not null
                {

                    r+=m->msg->GetTypeName().size();
                    r+=m->msg->ByteSize();

                }
                cached_size=r;
                return r;
			}

			std::size_t writeSerialized(uint8_t* buf,std::size_t len)
			{
			    if(cached_size>0)
                {
                    if(len<cached_size) return 0;
                }
                else
                {
                    if(len<getByteSize()) return 0;
                }
                uint8_t*c=buf;

                if(Traits::hasTopic==true)
                    c=writeChunk<typename Traits::topic_t>(c,(typename Traits::topic_t)m->topic);

                c=writeChunk<MessageEntry::msgclass_t>(c,(MessageEntry::msgclass_t)m->msgclass);

                 if(Traits::hasTimeStamp==true)
                 {
                    KSystem::Time::TimeAbsolute ts=KSystem::Time::SystemTime::unwrap(m->timestamp);
                    c=writeChunk<typename Traits::timestamp_t::rep>(c,ts.wrapTo<typename Traits::timestamp_t>().raw());
                 }
                 typename Traits::typename_t type=0;
                 if(m->msg) type=m->msg->GetTypeName().size();
                 c=writeChunk<typename Traits::typename_t>(c,type);
                 if(m->msg)
                 {

                    c=writeBytes(c,(const uint8_t*)m->msg->GetTypeName().data(),type);
                    std::size_t msgbytes=m->msg->ByteSize();
                    m->msg->SerializePartialToArray(c, msgbytes);
                    c+=msgbytes;
                 }
                 return c-buf;


			}
			private:
			template<typename TYPE>
			static uint8_t* writeChunk(uint8_t*b,TYPE const&t)
			{
			    TYPE *typeval=(TYPE *)b;
                (*typeval)=BasicSerialization::serialize(t);
                b+=sizeof(TYPE);
                return b;
			}
            static uint8_t* writeBytes(uint8_t*b, const uint8_t*bytes,std::size_t size)
			{
			    memcpy(b, bytes, size);
		        b += size;
		        return b;
			}



		};



	}



#endif
