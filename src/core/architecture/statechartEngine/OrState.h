#ifndef ORSTATE_H_
#define ORSTATE_H_ 1

#include "State.h"

namespace statechart_engine
{

	class OrState: public State
	{

	public:
		OrState ( std::string name, State* parent, IAction* entryAction = 0, IAction* exitAction = 0 );

		virtual ~OrState ();

		virtual int Activate ();
		int DeActivate ();

		bool Step ( IEvent* ev = 0, IParameter* param = 0);

		void SetActive ( State* );

		void SetStartState ( State* );

		virtual Blackboard* AddChild ( State* );

	protected:

		State* _activeState;

		State* _startState;

	};

}

#endif /* ORSTATE_H_ */
