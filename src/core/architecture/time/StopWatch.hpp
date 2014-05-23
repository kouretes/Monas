#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

template<class AvgPol = KMath::StatMovingAverage>
class StopWatch : public AvgPol
{

public:

	void StartTiming ()
	{
		execStart = SysCall::_GetCurrentTimeInUSec();
	}

	double StopTiming ()
	{
		return this->AddElement( SysCall::_GetCurrentTimeInUSec() - execStart );
	}

private:

	unsigned long execStart;

};

#endif // STOPWATCH_HPP
