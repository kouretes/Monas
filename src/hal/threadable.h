#ifndef THREADABLE_H
#define THREADABLE_H

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
