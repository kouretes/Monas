#ifndef CONDITIONCONNECTOR_H_
#define CONDITIONCONNECTOR_H_ 1

#include "TransitionConnector.h"

namespace statechart_engine
{

	class ConditionConnector: public statechart_engine::TransitionConnector
	{

	public:

		ConditionConnector ( std::string name, State* parent );

		virtual ~ConditionConnector ();

		bool CanStep ( IEvent* ev,
		               Containers::TransitionContainerIteratorConst& index );

		bool Step ( IEvent*ev, IParameter* param,
		            Containers::TransitionContainerIteratorConst& index );
	};

}

#endif /* CONDITIONCONNECTOR_H_ */
