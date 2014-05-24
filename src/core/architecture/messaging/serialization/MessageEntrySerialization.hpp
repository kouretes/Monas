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
                c=writeChunk<typename Traits::topic_t>(c,(typename Traits::topic_t)m->topic);
			}
			private:
			template<typename TYPE>
			uint8_t* writeChunk(uint8_t*b,TYPE const&t)
			{
			    TYPE *typeval=(TYPE *)b;
                (*typeval)=BasicSerialization::serialize(t);
                b+=sizeof(TYPE);
                return b;
			}




		};



	}



#endif
