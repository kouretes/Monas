	
#include "GoalieYellow.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<GoalieYellow>::Type temp("GoalieYellow");
}
	
GoalieYellow::GoalieYellow(Narukom* com) {

	
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

	OrState* Node_0_2_2_3_4 = new OrState ( "Node_ApproachBall_one_or_more_times_", Node_0_2_2_3 );	//Name:ApproachBall_one_or_more_times_
	_states.push_back( Node_0_2_2_3_4 );

	StartState* Node_0_2_2_3_4_1 = new StartState ( "Node_0_2_2_3_4_1", Node_0_2_2_3_4 ); //Name:0.2.2.3.4.1
	_states.push_back( Node_0_2_2_3_4_1 );

	IActivity* NodeActivInst_0_2_2_3_4_2 = ActivityFactory::Instance()->CreateObject( "ApproachBall" );
	_activities.push_back( NodeActivInst_0_2_2_3_4_2 );			
	BasicState* Node_0_2_2_3_4_2 = new BasicState( "Node_ApproachBall", Node_0_2_2_3_4, NodeActivInst_0_2_2_3_4_2 ); //Name:ApproachBall
	_states.push_back( Node_0_2_2_3_4_2 );

	EndState* Node_0_2_2_3_4_3 = new EndState ( "Node_0_2_2_3_4_3", Node_0_2_2_3_4 ); //Name:0.2.2.3.4.3
	_states.push_back( Node_0_2_2_3_4_3 );

	IActivity* NodeActivInst_0_2_2_3_5 = ActivityFactory::Instance()->CreateObject( "Kick" );
	_activities.push_back( NodeActivInst_0_2_2_3_5 );			
	BasicState* Node_0_2_2_3_5 = new BasicState( "Node_Kick", Node_0_2_2_3, NodeActivInst_0_2_2_3_5 ); //Name:Kick
	_states.push_back( Node_0_2_2_3_5 );

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

	OrState* Node_0_2_2_4 = new OrState ( "Node_nonPlaying", Node_0_2_2 );	//Name:nonPlaying
	_states.push_back( Node_0_2_2_4 );

	StartState* Node_0_2_2_4_1 = new StartState ( "Node_0_2_2_4_1", Node_0_2_2_4 ); //Name:0.2.2.4.1
	_states.push_back( Node_0_2_2_4_1 );

	IActivity* NodeActivInst_0_2_2_4_2 = ActivityFactory::Instance()->CreateObject( "NoPlay" );
	_activities.push_back( NodeActivInst_0_2_2_4_2 );			
	BasicState* Node_0_2_2_4_2 = new BasicState( "Node_NoPlay", Node_0_2_2_4, NodeActivInst_0_2_2_4_2 ); //Name:NoPlay
	_states.push_back( Node_0_2_2_4_2 );

	OrState* Node_0_2_2_4_3 = new OrState ( "Node__open_option_GoToPosition_one_or_more_times__close_option_", Node_0_2_2_4 );	//Name:_op_GoToPosition_one_or_more_times_
	_states.push_back( Node_0_2_2_4_3 );

	StartState* Node_0_2_2_4_3_1 = new StartState ( "Node_0_2_2_4_3_1", Node_0_2_2_4_3 ); //Name:0.2.2.4.3.1
	_states.push_back( Node_0_2_2_4_3_1 );

	ConditionConnector* Node_0_2_2_4_3_2  = new ConditionConnector ( "Node_0_3_2_4_3_2", Node_0_2_2_4_3 ); //Name:0.3.2.4.3.2
	_states.push_back( Node_0_2_2_4_3_2 );

	OrState* Node_0_2_2_4_3_3 = new OrState ( "Node_GoToPosition_one_or_more_times_", Node_0_2_2_4_3 );	//Name:GoToPosition_one_or_more_times_
	_states.push_back( Node_0_2_2_4_3_3 );

	StartState* Node_0_2_2_4_3_3_1 = new StartState ( "Node_0_2_2_4_3_3_1", Node_0_2_2_4_3_3 ); //Name:0.2.2.4.3.3.1
	_states.push_back( Node_0_2_2_4_3_3_1 );

	IActivity* NodeActivInst_0_2_2_4_3_3_2 = ActivityFactory::Instance()->CreateObject( "GoToPosition" );
	_activities.push_back( NodeActivInst_0_2_2_4_3_3_2 );			
	BasicState* Node_0_2_2_4_3_3_2 = new BasicState( "Node_GoToPosition", Node_0_2_2_4_3_3, NodeActivInst_0_2_2_4_3_3_2 ); //Name:GoToPosition
	_states.push_back( Node_0_2_2_4_3_3_2 );

	EndState* Node_0_2_2_4_3_3_3 = new EndState ( "Node_0_2_2_4_3_3_3", Node_0_2_2_4_3_3 ); //Name:0.2.2.4.3.3.3
	_states.push_back( Node_0_2_2_4_3_3_3 );

	EndState* Node_0_2_2_4_3_4 = new EndState ( "Node_0_2_2_4_3_4", Node_0_2_2_4_3 ); //Name:0.2.2.4.3.4
	_states.push_back( Node_0_2_2_4_3_4 );

	EndState* Node_0_2_2_4_4 = new EndState ( "Node_0_2_2_4_4", Node_0_2_2_4 ); //Name:0.2.2.4.4
	_states.push_back( Node_0_2_2_4_4 );

	OrState* Node_0_2_2_5 = new OrState ( "Node__open_option_Localize_one_or_more_times__close_option_", Node_0_2_2 );	//Name:_op_Localize_one_or_more_times_
	_states.push_back( Node_0_2_2_5 );

	StartState* Node_0_2_2_5_1 = new StartState ( "Node_0_2_2_5_1", Node_0_2_2_5 ); //Name:0.2.2.5.1
	_states.push_back( Node_0_2_2_5_1 );

	ConditionConnector* Node_0_2_2_5_2  = new ConditionConnector ( "Node_0_3_2_6_2", Node_0_2_2_5 ); //Name:0.3.2.6.2
	_states.push_back( Node_0_2_2_5_2 );

	OrState* Node_0_2_2_5_3 = new OrState ( "Node_Localize_one_or_more_times_", Node_0_2_2_5 );	//Name:Localize_one_or_more_times_
	_states.push_back( Node_0_2_2_5_3 );

	StartState* Node_0_2_2_5_3_1 = new StartState ( "Node_0_2_2_5_3_1", Node_0_2_2_5_3 ); //Name:0.2.2.5.3.1
	_states.push_back( Node_0_2_2_5_3_1 );

	IActivity* NodeActivInst_0_2_2_5_3_2 = ActivityFactory::Instance()->CreateObject( "Localize" );
	_activities.push_back( NodeActivInst_0_2_2_5_3_2 );			
	BasicState* Node_0_2_2_5_3_2 = new BasicState( "Node_Localize", Node_0_2_2_5_3, NodeActivInst_0_2_2_5_3_2 ); //Name:Localize
	_states.push_back( Node_0_2_2_5_3_2 );

	EndState* Node_0_2_2_5_3_3 = new EndState ( "Node_0_2_2_5_3_3", Node_0_2_2_5_3 ); //Name:0.2.2.5.3.3
	_states.push_back( Node_0_2_2_5_3_3 );

	OrState* Node_0_2_2_5_4 = new OrState ( "Node_GoToPosition_one_or_more_times_", Node_0_2_2_5 );	//Name:GoToPosition_one_or_more_times_
	_states.push_back( Node_0_2_2_5_4 );

	StartState* Node_0_2_2_5_4_1 = new StartState ( "Node_0_2_2_5_4_1", Node_0_2_2_5_4 ); //Name:0.2.2.5.4.1
	_states.push_back( Node_0_2_2_5_4_1 );

	IActivity* NodeActivInst_0_2_2_5_4_2 = ActivityFactory::Instance()->CreateObject( "GoToPosition" );
	_activities.push_back( NodeActivInst_0_2_2_5_4_2 );			
	BasicState* Node_0_2_2_5_4_2 = new BasicState( "Node_GoToPosition", Node_0_2_2_5_4, NodeActivInst_0_2_2_5_4_2 ); //Name:GoToPosition
	_states.push_back( Node_0_2_2_5_4_2 );

	EndState* Node_0_2_2_5_4_3 = new EndState ( "Node_0_2_2_5_4_3", Node_0_2_2_5_4 ); //Name:0.2.2.5.4.3
	_states.push_back( Node_0_2_2_5_4_3 );

	EndState* Node_0_2_2_5_5 = new EndState ( "Node_0_2_2_5_5", Node_0_2_2_5 ); //Name:0.2.2.5.5
	_states.push_back( Node_0_2_2_5_5 );

	EndState* Node_0_2_2_6 = new EndState ( "Node_0_2_2_6", Node_0_2_2 ); //Name:0.2.2.6
	_states.push_back( Node_0_2_2_6 );

	EndState* Node_0_2_3 = new EndState ( "Node_0_2_3", Node_0_2 ); //Name:0.2.3
	_states.push_back( Node_0_2_3 );

	EndState* Node_0_3 = new EndState ( "Node_0_3", Node_0 ); //Name:0.3
	_states.push_back( Node_0_3 );
	
	
		
		
	ICondition* TrCondInst_GoalieYellow0_20_3 = new TrCond_GoalieYellow0_20_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_20_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3 ,TrCondInst_GoalieYellow0_20_3  ) ); //GoalieYellow0.20.3
		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_2_1,Node_0_2_2_2   ) ); //GoalieYellow0.2.2.10.2.2.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_20_2_2_5 = new TrCond_GoalieYellow0_2_2_20_2_2_5;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_20_2_2_5 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_2,Node_0_2_2_5 ,TrCondInst_GoalieYellow0_2_2_20_2_2_5  ) ); //GoalieYellow0.2.2.20.2.2.5
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_20_2_2_4 = new TrCond_GoalieYellow0_2_2_20_2_2_4;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_20_2_2_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_2,Node_0_2_2_4 ,TrCondInst_GoalieYellow0_2_2_20_2_2_4  ) ); //GoalieYellow0.2.2.20.2.2.4
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4,Node_0_2_2_6   ) ); //GoalieYellow0.2.2.40.2.2.6
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3,Node_0_2_2_6   ) ); //GoalieYellow0.2.2.30.2.2.6
		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_2_3_1,Node_0_2_2_3_2   ) ); //GoalieYellow0.2.2.3.10.2.2.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_3 = new TrCond_GoalieYellow0_2_2_3_20_2_2_3_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_2,Node_0_2_2_3_3 ,TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_3  ) ); //GoalieYellow0.2.2.3.20.2.2.3.3
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_4 = new TrCond_GoalieYellow0_2_2_3_20_2_2_3_4;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_2,Node_0_2_2_3_4 ,TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_4  ) ); //GoalieYellow0.2.2.3.20.2.2.3.4
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_5 = new TrCond_GoalieYellow0_2_2_3_20_2_2_3_5;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_5 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_2,Node_0_2_2_3_5 ,TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_5  ) ); //GoalieYellow0.2.2.3.20.2.2.3.5
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_6 = new TrCond_GoalieYellow0_2_2_3_20_2_2_3_6;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_6 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_2,Node_0_2_2_3_6 ,TrCondInst_GoalieYellow0_2_2_3_20_2_2_3_6  ) ); //GoalieYellow0.2.2.3.20.2.2.3.6
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6,Node_0_2_2_3_7   ) ); //GoalieYellow0.2.2.3.60.2.2.3.7
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_5,Node_0_2_2_3_7   ) ); //GoalieYellow0.2.2.3.50.2.2.3.7
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_40_2_2_3_7 = new TrCond_GoalieYellow0_2_2_3_40_2_2_3_7;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_40_2_2_3_7 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_4,Node_0_2_2_3_7 ,TrCondInst_GoalieYellow0_2_2_3_40_2_2_3_7  ) ); //GoalieYellow0.2.2.3.40.2.2.3.7
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_30_2_2_3_7 = new TrCond_GoalieYellow0_2_2_3_30_2_2_3_7;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_30_2_2_3_7 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_3,Node_0_2_2_3_7 ,TrCondInst_GoalieYellow0_2_2_3_30_2_2_3_7  ) ); //GoalieYellow0.2.2.3.30.2.2.3.7
		

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_3_3_10_2_2_3_3_2 = new TrAction_GoalieYellow0_2_2_3_3_10_2_2_3_3_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_3_3_10_2_2_3_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_3_1,Node_0_2_2_3_3_2  ,TrActionInst_GoalieYellow0_2_2_3_3_10_2_2_3_3_2 ) ); //GoalieYellow0.2.2.3.3.10.2.2.3.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_2 = new TrCond_GoalieYellow0_2_2_3_3_20_2_2_3_3_2;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_2 );

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_2 = new TrAction_GoalieYellow0_2_2_3_3_20_2_2_3_3_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_3_2,Node_0_2_2_3_3_2 ,TrCondInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_2 ,TrActionInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_2 ) ); //GoalieYellow0.2.2.3.3.20.2.2.3.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_3 = new TrCond_GoalieYellow0_2_2_3_3_20_2_2_3_3_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_3_2,Node_0_2_2_3_3_3 ,TrCondInst_GoalieYellow0_2_2_3_3_20_2_2_3_3_3  ) ); //GoalieYellow0.2.2.3.3.20.2.2.3.3.3
		

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_3_4_10_2_2_3_4_2 = new TrAction_GoalieYellow0_2_2_3_4_10_2_2_3_4_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_3_4_10_2_2_3_4_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_4_1,Node_0_2_2_3_4_2  ,TrActionInst_GoalieYellow0_2_2_3_4_10_2_2_3_4_2 ) ); //GoalieYellow0.2.2.3.4.10.2.2.3.4.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_2 = new TrCond_GoalieYellow0_2_2_3_4_20_2_2_3_4_2;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_2 );

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_2 = new TrAction_GoalieYellow0_2_2_3_4_20_2_2_3_4_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_4_2,Node_0_2_2_3_4_2 ,TrCondInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_2 ,TrActionInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_2 ) ); //GoalieYellow0.2.2.3.4.20.2.2.3.4.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_3 = new TrCond_GoalieYellow0_2_2_3_4_20_2_2_3_4_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_4_2,Node_0_2_2_3_4_3 ,TrCondInst_GoalieYellow0_2_2_3_4_20_2_2_3_4_3  ) ); //GoalieYellow0.2.2.3.4.20.2.2.3.4.3
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_1,Node_0_2_2_3_6_2   ) ); //GoalieYellow0.2.2.3.6.10.2.2.3.6.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_6_20_2_2_3_6_3 = new TrCond_GoalieYellow0_2_2_3_6_20_2_2_3_6_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_6_20_2_2_3_6_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_2,Node_0_2_2_3_6_3 ,TrCondInst_GoalieYellow0_2_2_3_6_20_2_2_3_6_3  ) ); //GoalieYellow0.2.2.3.6.20.2.2.3.6.3
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_3,Node_0_2_2_3_6_4   ) ); //GoalieYellow0.2.2.3.6.30.2.2.3.6.4
		

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_3_6_2_10_2_2_3_6_2_2 = new TrAction_GoalieYellow0_2_2_3_6_2_10_2_2_3_6_2_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_3_6_2_10_2_2_3_6_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_2_1,Node_0_2_2_3_6_2_2  ,TrActionInst_GoalieYellow0_2_2_3_6_2_10_2_2_3_6_2_2 ) ); //GoalieYellow0.2.2.3.6.2.10.2.2.3.6.2.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2 = new TrCond_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2 );

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2 = new TrAction_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_2_2,Node_0_2_2_3_6_2_2 ,TrCondInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2 ,TrActionInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2 ) ); //GoalieYellow0.2.2.3.6.2.20.2.2.3.6.2.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_3 = new TrCond_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_2_2,Node_0_2_2_3_6_2_3 ,TrCondInst_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_3  ) ); //GoalieYellow0.2.2.3.6.2.20.2.2.3.6.2.3
		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_2_3_6_3_1,Node_0_2_2_3_6_3_2   ) ); //GoalieYellow0.2.2.3.6.3.10.2.2.3.6.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_6_3_20_2_2_3_6_3_3 = new TrCond_GoalieYellow0_2_2_3_6_3_20_2_2_3_6_3_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_6_3_20_2_2_3_6_3_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_6_3_2,Node_0_2_2_3_6_3_3 ,TrCondInst_GoalieYellow0_2_2_3_6_3_20_2_2_3_6_3_3  ) ); //GoalieYellow0.2.2.3.6.3.20.2.2.3.6.3.3
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_3_6_3_20_2_2_3_6_3_4 = new TrCond_GoalieYellow0_2_2_3_6_3_20_2_2_3_6_3_4;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_3_6_3_20_2_2_3_6_3_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_3_6_3_2,Node_0_2_2_3_6_3_4 ,TrCondInst_GoalieYellow0_2_2_3_6_3_20_2_2_3_6_3_4  ) ); //GoalieYellow0.2.2.3.6.3.20.2.2.3.6.3.4
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_3_6_3_3,Node_0_2_2_3_6_3_4   ) ); //GoalieYellow0.2.2.3.6.3.30.2.2.3.6.3.4
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4_1,Node_0_2_2_4_2   ) ); //GoalieYellow0.2.2.4.10.2.2.4.2
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4_2,Node_0_2_2_4_3   ) ); //GoalieYellow0.2.2.4.20.2.2.4.3
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4_3,Node_0_2_2_4_4   ) ); //GoalieYellow0.2.2.4.30.2.2.4.4
		

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_4_3_3_10_2_2_4_3_3_2 = new TrAction_GoalieYellow0_2_2_4_3_3_10_2_2_4_3_3_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_4_3_3_10_2_2_4_3_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4_3_3_1,Node_0_2_2_4_3_3_2  ,TrActionInst_GoalieYellow0_2_2_4_3_3_10_2_2_4_3_3_2 ) ); //GoalieYellow0.2.2.4.3.3.10.2.2.4.3.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_2 = new TrCond_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_2;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_2 );

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_2 = new TrAction_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4_3_3_2,Node_0_2_2_4_3_3_2 ,TrCondInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_2 ,TrActionInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_2 ) ); //GoalieYellow0.2.2.4.3.3.20.2.2.4.3.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_3 = new TrCond_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4_3_3_2,Node_0_2_2_4_3_3_3 ,TrCondInst_GoalieYellow0_2_2_4_3_3_20_2_2_4_3_3_3  ) ); //GoalieYellow0.2.2.4.3.3.20.2.2.4.3.3.3
		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_2_4_3_1,Node_0_2_2_4_3_2   ) ); //GoalieYellow0.2.2.4.3.10.2.2.4.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_4_3_20_2_2_4_3_3 = new TrCond_GoalieYellow0_2_2_4_3_20_2_2_4_3_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_4_3_20_2_2_4_3_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_4_3_2,Node_0_2_2_4_3_3 ,TrCondInst_GoalieYellow0_2_2_4_3_20_2_2_4_3_3  ) ); //GoalieYellow0.2.2.4.3.20.2.2.4.3.3
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_4_3_20_2_2_4_3_4 = new TrCond_GoalieYellow0_2_2_4_3_20_2_2_4_3_4;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_4_3_20_2_2_4_3_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_4_3_2,Node_0_2_2_4_3_4 ,TrCondInst_GoalieYellow0_2_2_4_3_20_2_2_4_3_4  ) ); //GoalieYellow0.2.2.4.3.20.2.2.4.3.4
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_4_3_30_2_2_4_3_4 = new TrCond_GoalieYellow0_2_2_4_3_30_2_2_4_3_4;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_4_3_30_2_2_4_3_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_4_3_3,Node_0_2_2_4_3_4 ,TrCondInst_GoalieYellow0_2_2_4_3_30_2_2_4_3_4  ) ); //GoalieYellow0.2.2.4.3.30.2.2.4.3.4
		

		
		
	IAction* TrActionInst_GoalieYellow0_2_10_2_2 = new TrAction_GoalieYellow0_2_10_2_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_10_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1,Node_0_2_2  ,TrActionInst_GoalieYellow0_2_10_2_2 ) ); //GoalieYellow0.2.10.2.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_20_2_2 = new TrCond_GoalieYellow0_2_20_2_2;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_20_2_2 );

		
		
	IAction* TrActionInst_GoalieYellow0_2_20_2_2 = new TrAction_GoalieYellow0_2_20_2_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_20_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2,Node_0_2_2 ,TrCondInst_GoalieYellow0_2_20_2_2 ,TrActionInst_GoalieYellow0_2_20_2_2 ) ); //GoalieYellow0.2.20.2.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_20_2_3 = new TrCond_GoalieYellow0_2_20_2_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_20_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2,Node_0_2_3 ,TrCondInst_GoalieYellow0_2_20_2_3  ) ); //GoalieYellow0.2.20.2.3
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2   ) ); //GoalieYellow0.10.2
		

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_5_3_10_2_2_5_3_2 = new TrAction_GoalieYellow0_2_2_5_3_10_2_2_5_3_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_5_3_10_2_2_5_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_3_1,Node_0_2_2_5_3_2  ,TrActionInst_GoalieYellow0_2_2_5_3_10_2_2_5_3_2 ) ); //GoalieYellow0.2.2.5.3.10.2.2.5.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_2 = new TrCond_GoalieYellow0_2_2_5_3_20_2_2_5_3_2;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_2 );

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_2 = new TrAction_GoalieYellow0_2_2_5_3_20_2_2_5_3_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_3_2,Node_0_2_2_5_3_2 ,TrCondInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_2 ,TrActionInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_2 ) ); //GoalieYellow0.2.2.5.3.20.2.2.5.3.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_3 = new TrCond_GoalieYellow0_2_2_5_3_20_2_2_5_3_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_3_2,Node_0_2_2_5_3_3 ,TrCondInst_GoalieYellow0_2_2_5_3_20_2_2_5_3_3  ) ); //GoalieYellow0.2.2.5.3.20.2.2.5.3.3
		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_2_5_1,Node_0_2_2_5_2   ) ); //GoalieYellow0.2.2.5.10.2.2.5.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_20_2_2_5_3 = new TrCond_GoalieYellow0_2_2_5_20_2_2_5_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_20_2_2_5_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_5_2,Node_0_2_2_5_3 ,TrCondInst_GoalieYellow0_2_2_5_20_2_2_5_3  ) ); //GoalieYellow0.2.2.5.20.2.2.5.3
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_20_2_2_5_5 = new TrCond_GoalieYellow0_2_2_5_20_2_2_5_5;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_20_2_2_5_5 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_2_5_2,Node_0_2_2_5_5 ,TrCondInst_GoalieYellow0_2_2_5_20_2_2_5_5  ) ); //GoalieYellow0.2.2.5.20.2.2.5.5
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_30_2_2_5_4 = new TrCond_GoalieYellow0_2_2_5_30_2_2_5_4;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_30_2_2_5_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_3,Node_0_2_2_5_4 ,TrCondInst_GoalieYellow0_2_2_5_30_2_2_5_4  ) ); //GoalieYellow0.2.2.5.30.2.2.5.4
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_50_2_2_3 = new TrCond_GoalieYellow0_2_2_50_2_2_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_50_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5,Node_0_2_2_3 ,TrCondInst_GoalieYellow0_2_2_50_2_2_3  ) ); //GoalieYellow0.2.2.50.2.2.3
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_4_10_2_2_5_4_2 = new TrCond_GoalieYellow0_2_2_5_4_10_2_2_5_4_2;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_4_10_2_2_5_4_2 );

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_5_4_10_2_2_5_4_2 = new TrAction_GoalieYellow0_2_2_5_4_10_2_2_5_4_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_5_4_10_2_2_5_4_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_4_1,Node_0_2_2_5_4_2 ,TrCondInst_GoalieYellow0_2_2_5_4_10_2_2_5_4_2 ,TrActionInst_GoalieYellow0_2_2_5_4_10_2_2_5_4_2 ) ); //GoalieYellow0.2.2.5.4.10.2.2.5.4.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_2 = new TrCond_GoalieYellow0_2_2_5_4_20_2_2_5_4_2;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_2 );

		
		
	IAction* TrActionInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_2 = new TrAction_GoalieYellow0_2_2_5_4_20_2_2_5_4_2;
	_actions.push_back( TrActionInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_4_2,Node_0_2_2_5_4_2 ,TrCondInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_2 ,TrActionInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_2 ) ); //GoalieYellow0.2.2.5.4.20.2.2.5.4.2
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_3 = new TrCond_GoalieYellow0_2_2_5_4_20_2_2_5_4_3;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_4_2,Node_0_2_2_5_4_3 ,TrCondInst_GoalieYellow0_2_2_5_4_20_2_2_5_4_3  ) ); //GoalieYellow0.2.2.5.4.20.2.2.5.4.3
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_40_2_2_5_5 = new TrCond_GoalieYellow0_2_2_5_40_2_2_5_5;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_40_2_2_5_5 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_4,Node_0_2_2_5_5 ,TrCondInst_GoalieYellow0_2_2_5_40_2_2_5_5  ) ); //GoalieYellow0.2.2.5.40.2.2.5.5
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_50_2_2_6 = new TrCond_GoalieYellow0_2_2_50_2_2_6;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_50_2_2_6 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5,Node_0_2_2_6 ,TrCondInst_GoalieYellow0_2_2_50_2_2_6  ) ); //GoalieYellow0.2.2.50.2.2.6
		
		
	ICondition* TrCondInst_GoalieYellow0_2_2_5_30_2_2_5_5 = new TrCond_GoalieYellow0_2_2_5_30_2_2_5_5;
	_conditions.push_back( TrCondInst_GoalieYellow0_2_2_5_30_2_2_5_5 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_5_3,Node_0_2_2_5_5 ,TrCondInst_GoalieYellow0_2_2_5_30_2_2_5_5  ) ); //GoalieYellow0.2.2.5.30.2.2.5.5
	
}

GoalieYellow::~GoalieYellow() {
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

void GoalieYellow::Start () {
	_statechart->Start();
}

void GoalieYellow::Stop () {
	_statechart->Stop();
}

	