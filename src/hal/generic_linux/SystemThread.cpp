#include "hal/SystemThread.hpp"
#include "iostream"
namespace KSystem
{

	void SystemThread :: enforceName(){

    //std::cout<<"Enforce name:"<<name<<std::endl;
#if defined(BOOST_THREAD_PLATFORM_PTHREAD)
        //char getname[1000];
        //pthread_getname_np(bThread.native_handle(),getname, 1000);
        //std::cout<<"Old name:"<<getname<<std::endl;
        if(pthread_setname_np(bThread.native_handle(), name.c_str())!=0)
            std::cout<<"Cannot enforce name:"<<name<<std::endl;
        //pthread_getname_np(bThread.native_handle(),getname, 1000);
        //std::cout<<"New name:"<<getname<<std::endl;


#endif
	}


};


