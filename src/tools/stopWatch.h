#ifndef _stopWatch_h_
#define _stopWatch_h_ 1

template<class AvgPol = StatMovingAverage>
class StopWatch : public AvgPol {

    public:

        void StartTiming () {
            execStart = SysCall::_GetCurrentTimeInUSec();
        }

        double StopTiming () {
            return this->AddElement( SysCall::_GetCurrentTimeInUSec() - execStart );
        }

    private:

        unsigned long execStart;

};

#endif // _stopWatch_h_
