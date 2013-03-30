#ifndef IACTION_H_
#define IACTION_H_ 1

#include "core/architecture/messaging/MessageHub.hpp"
#include "core/architecture/messaging/Blackboard.hpp"

#include "core/architecture/executable/IExecutable.hpp"

namespace statechart_engine
{

	class Statechart;

	class IAction: public IExecutable
	{

	public:

		void Initialize ( MessageHub*, Blackboard*, Statechart* );

		virtual void UserInit () = 0;

	protected:

		MessageHub* _com;

		Blackboard* _blk;

		Statechart* _s;

	};

}

#endif /* IACTION_H_ */
