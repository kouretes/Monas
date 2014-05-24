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
			T r;
			r=r&0;
			unsigned char* p=&d;
			for(unsigned i=0;i<sizeof(T);i++)
			{
				r=(d[sizeof(T)-i-1]<<(8*i))|r;
			}

		};
		template<typename T>
		T serialize(T const&d)
		{
			T r;
			r=r&0;
			unsigned char* p=&r;
			for(unsigned i=0;i<sizeof(T);i++)
			{
				p[i]=(d>>(8*i))&0xFF;
			}

		};



	}
}



#endif //BASICSERIALIZATION_HPP
