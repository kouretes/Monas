#ifndef IEXECUTABLE_HPP
#define IEXECUTABLE_HPP

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
