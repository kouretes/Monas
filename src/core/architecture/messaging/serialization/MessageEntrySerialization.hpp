#ifndef MESSAGEENTRYSERIALIZATION_HPP
#define MESSAGEENTRYSERIALIZATION_HPP
#include "core/architecture/messaging/MessageEntry.hpp"
#include <google/protobuf/text_format.h>
#include "ISerialization.hpp"

	namespace Messaging
	{

		template<class Traits>
		class MessageEntryBlockSerializer : public Traits, public KSystem::ISerializer
		{
			private:
			const Messaging::MessageEntry * m;
			ISerializer::PrefixCoder<Traits::codedMsgClass,MessageEntry::msgclass_t> classcoder;
			ISerializer::PrefixCoder<Traits::codedTimeStamp,typename Traits::timestamp_t::rep> timestampcoder;
			ISerializer::PrefixCoder<Traits::codedTypeName,std::size_t> typenamecoder;
			ISerializer::PrefixCoder<Traits::codedTopic,std::size_t> topiccoder;

			std::size_t cached_size;

			public:
			MessageEntryBlockSerializer(MessageEntry const& am) : m(&am),cached_size(0)
			{
			    KSystem::Time::TimeAbsolute ts=KSystem::Time::SystemTime::unwrap(m->timestamp);
			    classcoder.set(m->msgclass);
			    timestampcoder.set(ts.wrapTo<typename Traits::timestamp_t>().raw());
			    if(m->msg)
			        typenamecoder.set(m->msg->GetTypeName().size());
                else
                    typenamecoder.set(0);
			    topiccoder.set(m->topic);
			};
			virtual std::size_t getByteSize()
			{
			   if(m==NULL)
			        return 0;
                std::size_t r=0;
                r+=classcoder.getSize();//MSG CLass
                if(Traits::hasTopic==true)
                    r+=topiccoder.getSize();
                if(Traits::hasTimeStamp==true)
                    r+=timestampcoder.getSize();
                r+=typenamecoder.getSize();
                if(m->msg) // Not null
                {
                    r+=typenamecoder.uncoded;//Size of message type
                    r+=m->msg->ByteSize();

                }
                cached_size=r;
                return r;
			}


			virtual std::size_t writeSerialized(uint8_t* buf,std::size_t len)
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
                //c=writeChunk<MessageEntry::msgclass_t>(c,len-(c-buf),m->msgclass);
                c=classcoder.write(c,len-(c-buf));
                if(Traits::hasTopic==true)
                    c=topiccoder.write(c,len-(c-buf)); //writeChunk<typename Traits::topic_t>(c,len-(c-buf),(typename Traits::topic_t)m->topic);
                if(Traits::hasTimeStamp==true)
                    c=timestampcoder.write(c,len-(c-buf));
                c=typenamecoder.write(c,len-(c-buf));
                 if(m->msg)
                 {

                    c=writeBytes(c,(const uint8_t*)m->msg->GetTypeName().data(),typenamecoder.uncoded);
                    std::size_t msgbytes=m->msg->ByteSize();
                    //std::cout<<"Message bytesize"<<msgbytes<<std::endl;
                    //std::cout<<"Message bytesize"<<msgbytes<<" "<<(c-buf)<<" "<<len<<std::endl;
                    if(m->msg->SerializePartialToArray(c, len-(c-buf)))
                    {
                        c+=msgbytes;
                        //std::cout<<"Serialization succeded"<<std::endl;
                    }

                 }
                 return c-buf;

			};


		};


		template<class Traits>
		class MessageEntryBlockDeserializer : public Traits, public KSystem::IBlockDeserializer
		{
			private:
			Messaging::MessageEntry  m;
			ISerializer::PrefixCoder<Traits::codedMsgClass,MessageEntry::msgclass_t> classcoder;
			ISerializer::PrefixCoder<Traits::codedTimeStamp,typename Traits::timestamp_t::rep> timestampcoder;
			ISerializer::PrefixCoder<Traits::codedTypeName,std::size_t> typenamecoder;
			ISerializer::PrefixCoder<Traits::codedTopic,std::size_t> topiccoder;

			public:
			MessageEntryBlockDeserializer() {            };


			virtual std::size_t readSerialized(const uint8_t* buf,std::size_t len)
			{

			    const uint8_t* c =buf;
			    //std::cout<<"Msgbef:"<<m.msgclass<<std::endl;
                //c=readChunk<MessageEntry::msgclass_t>(c,len-(c-buf),m.msgclass);
                c=classcoder.read(c,len-(c-buf));
                m.msgclass=classcoder.uncoded;
                //std::cout<<"Msgafter:"<<m.msgclass<<std::endl;
                if(Traits::hasTopic==true)
                {
                    c=topiccoder.read(c,len-(c-buf)); //writeChunk<typename Traits::topic_t>(c,len-(c-buf),(typename Traits::topic_t)m->topic);
                    m.topic=topiccoder.uncoded;
                }
                if(Traits::hasTimeStamp==true)
                {
                    KSystem::Time::TimeAbsolute ts=KSystem::Time::SystemTime::now();
                    c=timestampcoder.read(c,len-(c-buf));
                    ts.unwrap(typename Traits::timestamp_t(timestampcoder.uncoded));
                    m.timestamp=ts.wrapTo<KSystem::Time::TimeStamp>();
                }
                c=typenamecoder.read(c,len-(c-buf));
                if(typenamecoder.uncoded>0)
                {
                    //std::cout<<"Decoding Message"<<std::endl;
                    std::string messagename=std::string((const char*)c,typenamecoder.uncoded);
                    c+=typenamecoder.uncoded;
                    google::protobuf::Message *proto=Messaging::getFromFactory(messagename);
                    //std::cout<<"Found message typename:"<<messagename<<std::endl;
                    //std::cout<<len<<" "<<(c-buf)<<std::endl;
                    if(proto==NULL)
                        return 0;
                    if(!proto->ParsePartialFromArray(c,len- (c-buf)))
                    {
                        //std::cout<<"Parse ok"<<std::endl;

                        delete proto;
                        return 0;
                    }
                    //std::string serialized;
                    //google::protobuf::TextFormat::PrintToString(*proto,&serialized);
                    //std::cout<<"MESSAGE:------"<<std::endl;
                    //std::cout<<serialized<<std::endl;
                    //std::cout<<"MESSAGEEND:------"<<std::endl;
                    std::cout<<"True message size:"<<(len-(c-buf))<<std::endl;
                    c+=len- (c-buf);
                    m.msg.reset(proto);



                }
                return c-buf;
			};
			Messaging::MessageEntry getMessageEntry() const { return m;};



		};
    }


#endif
