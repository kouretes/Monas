	#ifndef goalie_h_
#define goalie_h_ 1
		
#include "architecture/statechartEngine/Statechart.h"
#include "architecture/statechartEngine/StartState.h"
#include "architecture/statechartEngine/EndState.h"
#include "architecture/statechartEngine/OrState.h"
#include "architecture/statechartEngine/AndState.h"
#include "architecture/statechartEngine/BasicState.h"
#include "architecture/statechartEngine/Transitions/TransitionSegment.h"
#include "architecture/statechartEngine/Transitions/ConditionConnector.h"

#include "architecture/statechartWrapper.h"

class goalie : public StatechartWrapper {

	public:
	
		goalie (Narukom* com);
		
		~goalie ();
		
		void Start ();
		
		void Stop ();
		
	private:
	
		typedef std::vector<statechart_engine::State*> StateCont;
		typedef std::vector<statechart_engine::TransitionSegmentBase*> TransitionCont;
		
		StateCont _states;
		TransitionCont _transitions;
		
		typedef std::vector<IActivity*> ActCont;		
		ActCont _activities;
		
		typedef std::vector<statechart_engine::IEvent*> EventCont;
		typedef std::vector<statechart_engine::ICondition*> CondCont;
		typedef std::vector<statechart_engine::IAction*> ActionCont;
		
		EventCont _events;
		CondCont _conditions;
		ActionCont _actions;
		
		statechart_engine::Statechart* _statechart;
			
};

#endif // goalie_h_
	
