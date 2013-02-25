#ifndef ICONDITION_H_
#define ICONDITION_H_ 1

#include "core/architecture/messaging/MessageHub.hpp"
#include "core/architecture/messaging/Blackboard.hpp"


namespace statechart_engine
{

	class ICondition
	{

	public:

		virtual bool Eval() = 0;

		void Initialize ( MessageHub*, Blackboard* );

		virtual void UserInit () = 0;

	protected:

		MessageHub* _com;

		Blackboard* _blk;
	};

}

#endif /* ICONDITION_H_ */
