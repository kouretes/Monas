	
#include "GoaliePenalty.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<GoaliePenalty>::Type temp("GoaliePenalty");
}
	
GoaliePenalty::GoaliePenalty(Narukom* com) {

	
	_statechart = new Statechart ( "Node_goalie", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );

	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );

	OrState* Node_0_2 = new OrState ( "Node__open_group_playing_or_nonPlaying_close_group__one_or_more_times_", Node_0 );	//Name:_gr_playing_or_nonPlaying_one_or_more_times_
	_states.push_back( Node_0_2 );

	StartState* Node_0_2_1 = new StartState ( "Node_0_2_1", Node_0_2 ); //Name:0.2.1
	_states.push_back( Node_0_2_1 );

	OrState* Node_0_2_2 = new OrState ( "Node__open_group_playing_or_nonPlaying_close_group_", Node_0_2 );	//Name:_gr_playing_or_nonPlaying
	_states.push_back( Node_0_2_2 );

	StartState* Node_0_2_2_1 = new StartState ( "Node_0_2_2_1", Node_0_2_2 ); //Name:0.2.2.1
	_states.push_back( Node_0_2_2_1 );

	ConditionConnector* Node_0_2_2_2  = new ConditionConnector ( "Node_0_3_2_2", Node_0_2_2 ); //Name:0.3.2.2
	_states.push_back( Node_0_2_2_2 );

	OrState* Node_0_2_2_3 = new OrState ( "Node_playing", Node_0_2_2 );	//Name:playing
	_states.push_back( Node_0_2_2_3 );

	StartState* Node_0_2_2_3_1 = new StartState ( "Node_0_2_2_3_1", Node_0_2_2_3 ); //Name:0.2.2.3.1
	_states.push_back( Node_0_2_2_3_1 );

	ConditionConnector* Node_0_2_2_3_2  = new ConditionConnector ( "Node_0_3_2_3_2", Node_0_2_2_3 ); //Name:0.3.2.3.2
	_states.push_back( Node_0_2_2_3_2 );

	OrState* Node_0_2_2_3_3 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_2_2_3 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_2_2_3_3 );

	StartState* Node_0_2_2_3_3_1 = new StartState ( "Node_0_2_2_3_3_1", Node_0_2_2_3_3 ); //Name:0.2.2.3.3.1
	_states.push_back( Node_0_2_2_3_3_1 );

	IActivity* NodeActivInst_0_2_2_3_3_2 = ActivityFactory::Instance()->CreateObject( "Scan" );
	_activities.push_back( NodeActivInst_0_2_2_3_3_2 );			
	BasicState* Node_0_2_2_3_3_2 = new BasicState( "Node_Scan", Node_0_2_2_3_3, NodeActivInst_0_2_2_3_3_2 ); //Name:Scan
	_states.push_back( Node_0_2_2_3_3_2 );

	EndState* Node_0_2_2_3_3_3 = new EndState ( "Node_0_2_2_3_3_3", Node_0_2_2_3_3 ); //Name:0.2.2.3.3.3
	_states.push_back( Node_0_2_2_3_3_3 );

	OrState* Node_0_2_2_3_6 = new OrState ( "Node_followBall", Node_0_2_2_3 );	//Name:followBall
	_states.push_back( Node_0_2_2_3_6 );

	StartState* Node_0_2_2_3_6_1 = new StartState ( "Node_0_2_2_3_6_1", Node_0_2_2_3_6 ); //Name:0.2.2.3.6.1
	_states.push_back( Node_0_2_2_3_6_1 );

	OrState* Node_0_2_2_3_6_2 = new OrState ( "Node_Stare_one_or_more_times_", Node_0_2_2_3_6 );	//Name:Stare_one_or_more_times_
	_states.push_back( Node_0_2_2_3_6_2 );

	StartState* Node_0_2_2_3_6_2_1 = new StartState ( "Node_0_2_2_3_6_2_1", Node_0_2_2_3_6_2 ); //Name:0.2.2.3.6.2.1
	_states.push_back( Node_0_2_2_3_6_2_1 );

	IActivity* NodeActivInst_0_2_2_3_6_2_2 = ActivityFactory::Instance()->CreateObject( "Stare" );
	_activities.push_back( NodeActivInst_0_2_2_3_6_2_2 );			
	BasicState* Node_0_2_2_3_6_2_2 = new BasicState( "Node_Stare", Node_0_2_2_3_6_2, NodeActivInst_0_2_2_3_6_2_2 ); //Name:Stare
	_states.push_back( Node_0_2_2_3_6_2_2 );

	EndState* Node_0_2_2_3_6_2_3 = new EndState ( "Node_0_2_2_3_6_2_3", Node_0_2_2_3_6_2 ); //Name:0.2.2.3.6.2.3
	_states.push_back( Node_0_2_2_3_6_2_3 );

	OrState* Node_0_2_2_3_6_3 = new OrState ( "Node__open_option_SpecialAction_close_option_", Node_0_2_2_3_6 );	//Name:_op_SpecialAction
	_states.push_back( Node_0_2_2_3_6_3 );

	StartState* Node_0_2_2_3_6_3_1 = new StartState ( "Node_0_2_2_3_6_3_1", Node_0_2_2_3_6_3 ); //Name:0.2.2.3.6.3.1
	_states.push_back( Node_0_2_2_3_6_3_1 );

	ConditionConnector* Node_0_2_2_3_6_3_2  = new ConditionConnector ( "Node_0_3_2_3_6_3_2", Node_0_2_2_3_6_3 ); //Name:0.3.2.3.6.3.2
	_states.push_back( Node_0_2_2_3_6_3_2 );

	IActivity* NodeActivInst_0_2_2_3_6_3_3 = ActivityFactory::Instance()->CreateObject( "SpecialAction" );
	_activities.push_back( NodeActivInst_0_2_2_3_6_3_3 );			
	BasicState* Node_0_2_2_3_6_3_3 = new BasicState( "Node_SpecialAction", Node_0_2_2_3_6_3, NodeActivInst_0_2_2_3_6_3_3 ); //Name:SpecialAction
	_states.push_back( Node_0_2_2_3_6_3_3 );

	EndState* Node_0_2_2_3_6_3_4 = new EndState ( "Node_0_2_2_3_6_3_4", Node_0_2_2_3_6_3 ); //Name:0.2.2.3.6.3.4
	_states.push_back( Node_0_2_2_3_6_3_4 );

	EndState* Node_0_2_2_3_6_4 = new EndState ( "Node_0_2_2_3_6_4", Node_0_2_2_3_6 ); //Name:0.2.2.3.6.4
	_states.push_back( Node_0_2_2_3_6_4 );

	EndState* Node_0_2_2_3_7 = new EndState ( "Node_0_2_2_3_7", Node_0_2_2_3 ); //Name:0.2.2.3.7
	_states.push_back( Node_0_2_2_3_7 );

	IActivity* NodeActivInst_0_2_2_4 = ActivityFactory::Instance()->CreateObject( "NoPlay" );
	_activities.push_back( NodeActivInst_0_2_2_4 );			
	BasicState* Node_0_2_2_4 = new BasicState( "Node_NoPlay", Node_0_2_2, NodeActivInst_0_2_2_4 ); //Name:NoPlay
	_states.push_back( Node_0_2_2_4 );

	EndState* Node_0_2_2_5 = new EndState ( "Node_0_2_2_5", Node_0_2_2 ); //Name:0.2.2.5
	_states.push_back( Node_0_2_2_5 );

	EndState* Node_0_2_3 = new EndState ( "Node_0_2_3", Node_0_2 ); //Name:0.2.3
	_states.push_back( Node_0_2_3 );

	EndState* Node_0_3 = new EndState ( "Node_0_3", Node_0 ); //Name:0.3
	_states.push_back( Node_0_3 );
	
	
		
		
	ICondition* TrCondInst_GoaliePenalty0_20_3 = new TrCond_GoaliePenalty0_20_3;
	_conditions.push_back( TrCondInst_GoaliePenalty0_20_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3 ,TrCondInst_GoaliePenalty0_20_3  ) ); //GoaliePenalty0.20.3
		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_2_1,Node_0_2_2_2   ) ); //GoaliePenalty0.2.2.10.2.2.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_20_2_2_3 = new TrCond_GoaliePenalty0_2_2_20_2_2_3;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_20_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_2,Node_0_2_2_3 ,TrCondInst_GoaliePenalty0_2_2_20_2_2_3  ) ); //GoaliePenalty0.2.2.20.2.2.3
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3,Node_0_2_2_5   ) ); //GoaliePenalty0.2.2.30.2.2.5
		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_2_3_1,Node_0_2_2_3_2   ) ); //GoaliePenalty0.2.2.3.10.2.2.3.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_20_2_2_3_3 = new TrCond_GoaliePenalty0_2_2_3_20_2_2_3_3;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_20_2_2_3_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_2,Node_0_2_2_3_3 ,TrCondInst_GoaliePenalty0_2_2_3_20_2_2_3_3  ) ); //GoaliePenalty0.2.2.3.20.2.2.3.3
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_20_2_2_3_6 = new TrCond_GoaliePenalty0_2_2_3_20_2_2_3_6;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_20_2_2_3_6 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_2,Node_0_2_2_3_6 ,TrCondInst_GoaliePenalty0_2_2_3_20_2_2_3_6  ) ); //GoaliePenalty0.2.2.3.20.2.2.3.6
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6,Node_0_2_2_3_7   ) ); //GoaliePenalty0.2.2.3.60.2.2.3.7
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_30_2_2_3_7 = new TrCond_GoaliePenalty0_2_2_3_30_2_2_3_7;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_30_2_2_3_7 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_3,Node_0_2_2_3_7 ,TrCondInst_GoaliePenalty0_2_2_3_30_2_2_3_7  ) ); //GoaliePenalty0.2.2.3.30.2.2.3.7
		

		
		
	IAction* TrActionInst_GoaliePenalty0_2_2_3_3_10_2_2_3_3_2 = new TrAction_GoaliePenalty0_2_2_3_3_10_2_2_3_3_2;
	_actions.push_back( TrActionInst_GoaliePenalty0_2_2_3_3_10_2_2_3_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_3_1,Node_0_2_2_3_3_2  ,TrActionInst_GoaliePenalty0_2_2_3_3_10_2_2_3_3_2 ) ); //GoaliePenalty0.2.2.3.3.10.2.2.3.3.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2 = new TrCond_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2 );

		
		
	IAction* TrActionInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2 = new TrAction_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2;
	_actions.push_back( TrActionInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_3_2,Node_0_2_2_3_3_2 ,TrCondInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2 ,TrActionInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2 ) ); //GoaliePenalty0.2.2.3.3.20.2.2.3.3.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_3 = new TrCond_GoaliePenalty0_2_2_3_3_20_2_2_3_3_3;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_3_2,Node_0_2_2_3_3_3 ,TrCondInst_GoaliePenalty0_2_2_3_3_20_2_2_3_3_3  ) ); //GoaliePenalty0.2.2.3.3.20.2.2.3.3.3
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_1,Node_0_2_2_3_6_2   ) ); //GoaliePenalty0.2.2.3.6.10.2.2.3.6.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_6_20_2_2_3_6_3 = new TrCond_GoaliePenalty0_2_2_3_6_20_2_2_3_6_3;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_6_20_2_2_3_6_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_2,Node_0_2_2_3_6_3 ,TrCondInst_GoaliePenalty0_2_2_3_6_20_2_2_3_6_3  ) ); //GoaliePenalty0.2.2.3.6.20.2.2.3.6.3
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_3,Node_0_2_2_3_6_4   ) ); //GoaliePenalty0.2.2.3.6.30.2.2.3.6.4
		

		
		
	IAction* TrActionInst_GoaliePenalty0_2_2_3_6_2_10_2_2_3_6_2_2 = new TrAction_GoaliePenalty0_2_2_3_6_2_10_2_2_3_6_2_2;
	_actions.push_back( TrActionInst_GoaliePenalty0_2_2_3_6_2_10_2_2_3_6_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_2_1,Node_0_2_2_3_6_2_2  ,TrActionInst_GoaliePenalty0_2_2_3_6_2_10_2_2_3_6_2_2 ) ); //GoaliePenalty0.2.2.3.6.2.10.2.2.3.6.2.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_2 = new TrCond_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_2;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_2 );

		
		
	IAction* TrActionInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_2 = new TrAction_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_2;
	_actions.push_back( TrActionInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_2_2,Node_0_2_2_3_6_2_2 ,TrCondInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_2 ,TrActionInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_2 ) ); //GoaliePenalty0.2.2.3.6.2.20.2.2.3.6.2.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_3 = new TrCond_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_3;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_2_2,Node_0_2_2_3_6_2_3 ,TrCondInst_GoaliePenalty0_2_2_3_6_2_20_2_2_3_6_2_3  ) ); //GoaliePenalty0.2.2.3.6.2.20.2.2.3.6.2.3
		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_2_3_6_3_1,Node_0_2_2_3_6_3_2   ) ); //GoaliePenalty0.2.2.3.6.3.10.2.2.3.6.3.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_6_3_20_2_2_3_6_3_3 = new TrCond_GoaliePenalty0_2_2_3_6_3_20_2_2_3_6_3_3;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_6_3_20_2_2_3_6_3_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_6_3_2,Node_0_2_2_3_6_3_3 ,TrCondInst_GoaliePenalty0_2_2_3_6_3_20_2_2_3_6_3_3  ) ); //GoaliePenalty0.2.2.3.6.3.20.2.2.3.6.3.3
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_3_6_3_20_2_2_3_6_3_4 = new TrCond_GoaliePenalty0_2_2_3_6_3_20_2_2_3_6_3_4;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_3_6_3_20_2_2_3_6_3_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_6_3_2,Node_0_2_2_3_6_3_4 ,TrCondInst_GoaliePenalty0_2_2_3_6_3_20_2_2_3_6_3_4  ) ); //GoaliePenalty0.2.2.3.6.3.20.2.2.3.6.3.4
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_3_3,Node_0_2_2_3_6_3_4   ) ); //GoaliePenalty0.2.2.3.6.3.30.2.2.3.6.3.4
		

		
		
	IAction* TrActionInst_GoaliePenalty0_2_10_2_2 = new TrAction_GoaliePenalty0_2_10_2_2;
	_actions.push_back( TrActionInst_GoaliePenalty0_2_10_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1,Node_0_2_2  ,TrActionInst_GoaliePenalty0_2_10_2_2 ) ); //GoaliePenalty0.2.10.2.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_20_2_2 = new TrCond_GoaliePenalty0_2_20_2_2;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_20_2_2 );

		
		
	IAction* TrActionInst_GoaliePenalty0_2_20_2_2 = new TrAction_GoaliePenalty0_2_20_2_2;
	_actions.push_back( TrActionInst_GoaliePenalty0_2_20_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2,Node_0_2_2 ,TrCondInst_GoaliePenalty0_2_20_2_2 ,TrActionInst_GoaliePenalty0_2_20_2_2 ) ); //GoaliePenalty0.2.20.2.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_20_2_3 = new TrCond_GoaliePenalty0_2_20_2_3;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_20_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2,Node_0_2_3 ,TrCondInst_GoaliePenalty0_2_20_2_3  ) ); //GoaliePenalty0.2.20.2.3
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2   ) ); //GoaliePenalty0.10.2
		
		
	ICondition* TrCondInst_GoaliePenalty0_2_2_20_2_2_4 = new TrCond_GoaliePenalty0_2_2_20_2_2_4;
	_conditions.push_back( TrCondInst_GoaliePenalty0_2_2_20_2_2_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_2,Node_0_2_2_4 ,TrCondInst_GoaliePenalty0_2_2_20_2_2_4  ) ); //GoaliePenalty0.2.2.20.2.2.4
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4,Node_0_2_2_5   ) ); //GoaliePenalty0.2.2.40.2.2.5
	
}

GoaliePenalty::~GoaliePenalty() {
    for ( StateCont::iterator it = _states.begin(); it != _states.end(); it++ )
    	delete (*it);
    for ( TransitionCont::iterator it = _transitions.begin(); it != _transitions.end(); it++ )
    	delete (*it);
    for ( ActCont::iterator it = _activities.begin(); it != _activities.end(); it++ )
    	delete (*it);
    for ( EventCont::iterator it = _events.begin(); it != _events.end(); it++ )
    	delete (*it);
   	for ( CondCont::iterator it = _conditions.begin(); it != _conditions.end(); it++ )
    	delete (*it);
    for ( ActionCont::iterator it = _actions.begin(); it != _actions.end(); it++ )
    	delete (*it);
}

void GoaliePenalty::Start () {
	_statechart->Start();
}

void GoaliePenalty::Stop () {
	_statechart->Stop();
}

	