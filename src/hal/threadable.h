#ifndef _threadable_h_
#define _threadable_h_ 1

class Threadable
{

public:

	Threadable()
	{
		;
	}

	virtual int ThreadMain ( ) = 0;

};

#endif // _threadable_h_ 
