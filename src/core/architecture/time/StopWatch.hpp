#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP
#include "SystemClockProvider.hpp"
#include "Timer.hpp"
namespace KSystem
{
    namespace Time
    {

        template<class AvgPol = KMath::StatMovingAverage>
        class StopWatch : public AvgPol
        {

        public:
            StopWatch( ) : t(SystemClockProvider::getThreadClock()) {};
            void StartTiming ()
            {
                t.restart();
            }

            double StopTiming ()
            {
                //std::cout<<t.elapsed()<<std::endl;
                return this->AddElement( t.elapsed() );
            }

        private:
             Timer t;
        };


    }


}


#endif // STOPWATCH_HPP
