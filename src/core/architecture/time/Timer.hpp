#ifndef TIMER_HPP
#define TIMER_HPP
#include "IClock.hpp"
namespace KSystem
{
    namespace Time
    {
        class Timer
        {
            private:
            ClockValue start;
            IClock *clk;

            Timer& operator=(Timer const&t);
            public:
                Timer(Timer const& t)
                {
                    clk=NULL;
                    if(t.clk!=NULL)
                        clk=t.clk->Clone();

                }

                Timer(IClock *aclk)
                {
                    clk=aclk;
                }

                ~Timer()
                {
                    delete clk;

                }

                void restart()
                {

                    start=clk->now();

                }
                float elapsed() const
                {
                    ClockValue endt=clk->now();
                    //std::cout<<"Start:"<<start.p<<std::endl;
                    //std::cout<<"End:"<<endt.p<<std::endl;
                    return (endt-start).toFloat();
                }
        };

    }
}

#endif // TIMER_HPP
