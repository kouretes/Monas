#ifndef STATECHART_H_
#define STATECHART_H_ 1

#include "OrState.h"
#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "hal/threadPool.h"

namespace statechart_engine {

    class Statechart: public statechart_engine::OrState {

        public:

            Statechart ();

            virtual ~Statechart ();

            int Activate();

           ThreadPool* GetThreadPool (); //TODO const

        private:

            Narukom _narukom;

            Blackboard _blackboard;

            ThreadPool _tp;  //TODO fix namespace


    };

}

#endif /* STATECHART_H_ */
