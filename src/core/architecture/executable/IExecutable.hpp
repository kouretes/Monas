#ifndef IEXECUTABLE_H_
#define IEXECUTABLE_H_ 1

#ifdef __GNUC__
#define IEX_DIRECTIVE_HOT   __attribute__ ((hot))
#else
#define IEX_DIRECTIVE_HOT
#endif


class IExecutable
{
public:
	virtual int IEX_DIRECTIVE_HOT  Execute()  = 0;
};


#endif /* IEXECUTABLE_H_ */
