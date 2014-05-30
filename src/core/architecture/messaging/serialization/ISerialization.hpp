#ifndef ISERIALIZATION_HPP
#define ISERIALIZATION_HPP
#include "core/elements/BasicSerialization.hpp"
#include <stdint.h>
#include <stdio.h>

namespace KSystem
{
    class ISerializationBase
    {
        public:
        template<typename TYPE>
        static uint8_t* writeChunk(uint8_t*b,std::size_t len,TYPE const&t)
        {
            TYPE typeval;
            (void)len;
            //std::cout<<"Serializing"<<t<<std::endl;
            typeval=BasicSerialization::serialize(t);
            //std::cout<<"Serialized"<<typeval<<std::endl;
            memcpy(b,&typeval,sizeof(TYPE));//Copy to potentially unaligned address...

            b+=sizeof(TYPE);
            return b;
        }

        template<typename T>
        static uint8_t* writeChunk(uint8_t*b,std::size_t len,BasicSerialization::PrefixCoded<T> const&t)
        {
            std::size_t l;
            //T uncoded=t.get();
            //std::cout<<"Coding:"<<t.get()<<std::endl;
            l=t.writeCoded(b,len);
            //BasicSerialization::PrefixCoded<T> decoder(uncoded);
           // decoder.readCoded(b,len);
            //uncoded=decoder.get();
            //std::cout<<"Coding decode:"<<uncoded<<std::endl;
            b+=l;
            return b;
        }

        static uint8_t* writeBytes(uint8_t*b, const uint8_t*bytes,std::size_t size)
        {
            memcpy(b, bytes, size);
            b += size;
            return b;
        }

        template<typename TYPE>
        static const uint8_t* readChunk(const uint8_t*b,std::size_t len,TYPE &t)
        {
            TYPE aligned;
             (void)len;
            memcpy(&aligned,b,sizeof(TYPE));//Copy to aligned address...
            //std::cout<<"Read Serialized:"<<aligned<<std::endl;

            t=BasicSerialization::deserialize(aligned);

            //std::cout<<"Deserialized:"<<t<<std::endl;
            b+=sizeof(TYPE);
            return b;
        }
        template<typename T>
        static const uint8_t* readChunk(const uint8_t*b,std::size_t len,BasicSerialization::PrefixCoded<T> &t)
        {
            std::size_t l;
            l=t.readCoded(b,len);
            //std::cout<<"Length:"<<l<<std::endl;
            b+=l;
            return b;
        }


        template<typename T>
        static  std::size_t getSize(T const a){ return sizeof(a);};

        template<bool code,typename T> class PrefixCoder {};
        template<typename T> class PrefixCoder<false,T>
        {
            public :
            T uncoded;
            PrefixCoder() { };
            void set(T c){uncoded=c;};
            uint8_t* write(uint8_t*b,std::size_t len) const
            {
                return writeChunk<T>(b,len,uncoded);
            }
           const  uint8_t* read(const uint8_t*b,std::size_t len)
            {
                return readChunk<T>(b,len,uncoded);
            }

            static std::size_t getSize(){ return sizeof(T);};
        };
        template<typename T> class PrefixCoder<true,T>
        {
            public :
            BasicSerialization::PrefixCoded<T> coded;
            T uncoded;
            PrefixCoder():coded(static_cast<T>(0)){};
            void set(T c){ coded=BasicSerialization::PrefixCoded<T>(c);uncoded=c;};

            uint8_t* write(uint8_t*b,std::size_t len) const
            {
                return writeChunk<T>(b,len,coded);
            }
            const  uint8_t* read(const uint8_t*b,std::size_t len)
            {
                const uint8_t *r=readChunk<T>(b,len,coded);
                uncoded=coded.get();
                return r;
            }

            std::size_t getSize() { return coded.getByteSize();};
        };



    };

    class ISerializer : public ISerializationBase
    {
        public:
        virtual ~ISerializer() {};
        virtual std::size_t writeSerialized(uint8_t* buf,std::size_t len)=0;
        virtual std::size_t getByteSize()=0;

    };


    class IBlockDeserializer : protected ISerializationBase
    {
        public:
        virtual ~IBlockDeserializer() {};
        virtual std::size_t readSerialized(const uint8_t* buf,std::size_t len)=0;

    };

    class IStreamDeserializer
    {
        public:
        virtual ~IStreamDeserializer() {};
        virtual std::size_t peakSize(const uint8_t* buf,std::size_t len)=0;
        virtual std::size_t readSerialized(uint8_t* buf,std::size_t len)=0;
    };



}











#endif //ISERIALIZATION_HPP
