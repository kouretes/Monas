#ifndef BASICSERIALIZATION_HPP
#define BASICSERIALIZATION_HPP


//Big endian network order as per Internet protocol specifications

namespace KSystem
{

	namespace BasicSerialization
	{
		template<typename T>
		T deserialize(T const &d)
		{
			T r=static_cast<T>(0);
			const uint8_t * p=(const uint8_t*)&d;
			for(unsigned i=0;i<sizeof(T);i++)
			{
				r=static_cast<T>((p[i]<<(8*i))|r);
			}
			return r;

		};
		template<typename T>
		T serialize(T const&d)
		{
			T r=static_cast<T>(0);
			//r=(T)(r&0);
			uint8_t* p=( uint8_t*)&r;
			for(unsigned i=0;i<sizeof(T);i++)
			{
				p[i]=(d>>(8*i))&0xFF;
			}
			return r;

		};

		template<typename T>
		class PrefixCoded
		{
		    private:
		    T data;
		    public:
		    PrefixCoded(T const a) :data(a) {};
		    std::size_t getByteSize() const
		    {
		        T rem=data;
		        //uint8_t chunck;
		        std::size_t length=0;
		        do
		        {
		            //chunck=rem&0x7F;
		            rem=static_cast<T>(rem>>7);
		            length++;
		        }
		        while(rem!=0);
		        return length;
		    };

		    std::size_t writeCoded(uint8_t* buf,std::size_t len) const
		    {
		        if(len<getByteSize())
		            return 0;
                T rem=data;
		        uint8_t chunck;
		        std::size_t length=0;

		        do
		        {
		            chunck=rem&0x7F;
		            rem=static_cast<T>(rem>>7);
                    //std::cout<<"Writechunck "<<(int)chunck<<std::endl;
		            //Mark non-termination bit
		            if(rem!=0) chunck=chunck| 0x80;

		            buf[length]=chunck;
		            //std::cout<<"Writechunckread "<<(int)buf[length]<<std::endl;
		            chunck=buf[length];
		            //std::cout<<"Writechunckread2 "<<(int)buf[length]<<std::endl;
		            length++;
		        }
		        while(rem!=0&&len>=length);

		        return length;

		    };

            std::size_t readCoded(const uint8_t* buf,std::size_t len)
		    {
                data=static_cast<T>(0);
		        uint8_t chunck;

		        std::size_t length=0;
		        bool done=false;
                if(len==0) return 0;
		        do
		        {
		            chunck=buf[length];
		            //Read termination bit
                    if((chunck&0x80)==0) done=true;
                    //std::cout<<"Readchunck bit"<<(int)chunck<<std::endl;
                    chunck=chunck&0x7F;
                    //std::cout<<"Readchunck"<<(int)chunck<<std::endl;
		            data=static_cast<T>(data|((T)chunck)<<7*length);
		            length++;
		        }
		        while(!done&&len>=length);
    	        return length;
		    };
		    T get() const { return data;};


		};
	}
}



#endif //BASICSERIALIZATION_HPP
