
#include "Goalie.h"
#include "transitionHeaders.h"

using namespace statechart_engine;

namespace
{
	StatechartRegistrar<Goalie>::Type temp("Goalie");
}

Goalie::Goalie(Narukom* com)
{
	_statechart = new Statechart ( "Node_goalie", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );
	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );
	OrState* Node_0_3 = new OrState ( "Node__open_group_playing_or_nonPlaying_close_group__one_or_more_times_", Node_0 );	//Name:_gr_playing_or_nonPlaying_one_or_more_times_
	_states.push_back( Node_0_3 );
	StartState* Node_0_3_1 = new StartState ( "Node_0_3_1", Node_0_3 ); //Name:0.3.1
	_states.push_back( Node_0_3_1 );
	OrState* Node_0_3_2 = new OrState ( "Node__open_group_playing_or_nonPlaying_close_group_", Node_0_3 );	//Name:_gr_playing_or_nonPlaying
	_states.push_back( Node_0_3_2 );
	StartState* Node_0_3_2_1 = new StartState ( "Node_0_3_2_1", Node_0_3_2 ); //Name:0.3.2.1
	_states.push_back( Node_0_3_2_1 );
	ConditionConnector* Node_0_3_2_2  = new ConditionConnector ( "Node_0_3_2_2", Node_0_3_2 ); //Name:0.3.2.2
	_states.push_back( Node_0_3_2_2 );
	OrState* Node_0_3_2_3 = new OrState ( "Node_playing", Node_0_3_2 );	//Name:playing
	_states.push_back( Node_0_3_2_3 );
	StartState* Node_0_3_2_3_1 = new StartState ( "Node_0_3_2_3_1", Node_0_3_2_3 ); //Name:0.3.2.3.1
	_states.push_back( Node_0_3_2_3_1 );
	ConditionConnector* Node_0_3_2_3_2  = new ConditionConnector ( "Node_0_3_2_3_2", Node_0_3_2_3 ); //Name:0.3.2.3.2
	_states.push_back( Node_0_3_2_3_2 );
	OrState* Node_0_3_2_3_3 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_3_2_3 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3 );
	StartState* Node_0_3_2_3_3_1 = new StartState ( "Node_0_3_2_3_3_1", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.1
	_states.push_back( Node_0_3_2_3_3_1 );
	IActivity* NodeActivInst_0_3_2_3_3_2 = ActivityFactory::Instance()->CreateObject( "Scan" , *Node_0_3_2_3_3->GetBlackboard());
	_activities.push_back( NodeActivInst_0_3_2_3_3_2 );
	BasicState* Node_0_3_2_3_3_2 = new BasicState( "Node_Scan", Node_0_3_2_3_3, NodeActivInst_0_3_2_3_3_2 ); //Name:Scan
	_states.push_back( Node_0_3_2_3_3_2 );
	EndState* Node_0_3_2_3_3_3 = new EndState ( "Node_0_3_2_3_3_3", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.3
	_states.push_back( Node_0_3_2_3_3_3 );
	OrState* Node_0_3_2_3_4 = new OrState ( "Node_ApproachBall_one_or_more_times_", Node_0_3_2_3 );	//Name:ApproachBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4 );
	StartState* Node_0_3_2_3_4_1 = new StartState ( "Node_0_3_2_3_4_1", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.1
	_states.push_back( Node_0_3_2_3_4_1 );
	IActivity* NodeActivInst_0_3_2_3_4_2 = ActivityFactory::Instance()->CreateObject( "ApproachBall" , *Node_0_3_2_3_4->GetBlackboard());
	_activities.push_back( NodeActivInst_0_3_2_3_4_2 );
	BasicState* Node_0_3_2_3_4_2 = new BasicState( "Node_ApproachBall", Node_0_3_2_3_4, NodeActivInst_0_3_2_3_4_2 ); //Name:ApproachBall
	_states.push_back( Node_0_3_2_3_4_2 );
	EndState* Node_0_3_2_3_4_3 = new EndState ( "Node_0_3_2_3_4_3", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.3
	_states.push_back( Node_0_3_2_3_4_3 );
	IActivity* NodeActivInst_0_3_2_3_5 = ActivityFactory::Instance()->CreateObject( "Kick" , *Node_0_3_2_3->GetBlackboard());
	_activities.push_back( NodeActivInst_0_3_2_3_5 );
	BasicState* Node_0_3_2_3_5 = new BasicState( "Node_Kick", Node_0_3_2_3, NodeActivInst_0_3_2_3_5 ); //Name:Kick
	_states.push_back( Node_0_3_2_3_5 );
	OrState* Node_0_3_2_3_6 = new OrState ( "Node_followBall", Node_0_3_2_3 );	//Name:followBall
	_states.push_back( Node_0_3_2_3_6 );
	StartState* Node_0_3_2_3_6_1 = new StartState ( "Node_0_3_2_3_6_1", Node_0_3_2_3_6 ); //Name:0.3.2.3.6.1
	_states.push_back( Node_0_3_2_3_6_1 );
	OrState* Node_0_3_2_3_6_2 = new OrState ( "Node_Stare_one_or_more_times_", Node_0_3_2_3_6 );	//Name:Stare_one_or_more_times_
	_states.push_back( Node_0_3_2_3_6_2 );
	StartState* Node_0_3_2_3_6_2_1 = new StartState ( "Node_0_3_2_3_6_2_1", Node_0_3_2_3_6_2 ); //Name:0.3.2.3.6.2.1
	_states.push_back( Node_0_3_2_3_6_2_1 );
	IActivity* NodeActivInst_0_3_2_3_6_2_2 = ActivityFactory::Instance()->CreateObject( "Stare" , *Node_0_3_2_3_6_2->GetBlackboard());
	_activities.push_back( NodeActivInst_0_3_2_3_6_2_2 );
	BasicState* Node_0_3_2_3_6_2_2 = new BasicState( "Node_Stare", Node_0_3_2_3_6_2, NodeActivInst_0_3_2_3_6_2_2 ); //Name:Stare
	_states.push_back( Node_0_3_2_3_6_2_2 );
	EndState* Node_0_3_2_3_6_2_3 = new EndState ( "Node_0_3_2_3_6_2_3", Node_0_3_2_3_6_2 ); //Name:0.3.2.3.6.2.3
	_states.push_back( Node_0_3_2_3_6_2_3 );
	OrState* Node_0_3_2_3_6_3 = new OrState ( "Node__open_option_SpecialAction_close_option_", Node_0_3_2_3_6 );	//Name:_op_SpecialAction
	_states.push_back( Node_0_3_2_3_6_3 );
	StartState* Node_0_3_2_3_6_3_1 = new StartState ( "Node_0_3_2_3_6_3_1", Node_0_3_2_3_6_3 ); //Name:0.3.2.3.6.3.1
	_states.push_back( Node_0_3_2_3_6_3_1 );
	ConditionConnector* Node_0_3_2_3_6_3_2  = new ConditionConnector ( "Node_0_3_2_3_6_3_2", Node_0_3_2_3_6_3 ); //Name:0.3.2.3.6.3.2
	_states.push_back( Node_0_3_2_3_6_3_2 );
	IActivity* NodeActivInst_0_3_2_3_6_3_3 = ActivityFactory::Instance()->CreateObject( "SpecialAction" , *Node_0_3_2_3_6_3->GetBlackboard());
	_activities.push_back( NodeActivInst_0_3_2_3_6_3_3 );
	BasicState* Node_0_3_2_3_6_3_3 = new BasicState( "Node_SpecialAction", Node_0_3_2_3_6_3, NodeActivInst_0_3_2_3_6_3_3 ); //Name:SpecialAction
	_states.push_back( Node_0_3_2_3_6_3_3 );
	EndState* Node_0_3_2_3_6_3_4 = new EndState ( "Node_0_3_2_3_6_3_4", Node_0_3_2_3_6_3 ); //Name:0.3.2.3.6.3.4
	_states.push_back( Node_0_3_2_3_6_3_4 );
	EndState* Node_0_3_2_3_6_4 = new EndState ( "Node_0_3_2_3_6_4", Node_0_3_2_3_6 ); //Name:0.3.2.3.6.4
	_states.push_back( Node_0_3_2_3_6_4 );
	EndState* Node_0_3_2_3_7 = new EndState ( "Node_0_3_2_3_7", Node_0_3_2_3 ); //Name:0.3.2.3.7
	_states.push_back( Node_0_3_2_3_7 );
	OrState* Node_0_3_2_4 = new OrState ( "Node_nonPlaying", Node_0_3_2 );	//Name:nonPlaying
	_states.push_back( Node_0_3_2_4 );
	StartState* Node_0_3_2_4_1 = new StartState ( "Node_0_3_2_4_1", Node_0_3_2_4 ); //Name:0.3.2.4.1
	_states.push_back( Node_0_3_2_4_1 );
	IActivity* NodeActivInst_0_3_2_4_2 = ActivityFactory::Instance()->CreateObject( "NoPlay" , *Node_0_3_2_4->GetBlackboard());
	_activities.push_back( NodeActivInst_0_3_2_4_2 );
	BasicState* Node_0_3_2_4_2 = new BasicState( "Node_NoPlay", Node_0_3_2_4, NodeActivInst_0_3_2_4_2 ); //Name:NoPlay
	_states.push_back( Node_0_3_2_4_2 );
	OrState* Node_0_3_2_4_3 = new OrState ( "Node__open_option_GoToPosition_one_or_more_times__close_option_", Node_0_3_2_4 );	//Name:_op_GoToPosition_one_or_more_times_
	_states.push_back( Node_0_3_2_4_3 );
	StartState* Node_0_3_2_4_3_1 = new StartState ( "Node_0_3_2_4_3_1", Node_0_3_2_4_3 ); //Name:0.3.2.4.3.1
	_states.push_back( Node_0_3_2_4_3_1 );
	ConditionConnector* Node_0_3_2_4_3_2  = new ConditionConnector ( "Node_0_3_2_4_3_2", Node_0_3_2_4_3 ); //Name:0.3.2.4.3.2
	_states.push_back( Node_0_3_2_4_3_2 );
	OrState* Node_0_3_2_4_3_3 = new OrState ( "Node_GoToPosition_one_or_more_times_", Node_0_3_2_4_3 );	//Name:GoToPosition_one_or_more_times_
	_states.push_back( Node_0_3_2_4_3_3 );
	StartState* Node_0_3_2_4_3_3_1 = new StartState ( "Node_0_3_2_4_3_3_1", Node_0_3_2_4_3_3 ); //Name:0.3.2.4.3.3.1
	_states.push_back( Node_0_3_2_4_3_3_1 );
	IActivity* NodeActivInst_0_3_2_4_3_3_2 = ActivityFactory::Instance()->CreateObject( "GoToPosition" , *Node_0_3_2_4_3_3->GetBlackboard());
	_activities.push_back( NodeActivInst_0_3_2_4_3_3_2 );
	BasicState* Node_0_3_2_4_3_3_2 = new BasicState( "Node_GoToPosition", Node_0_3_2_4_3_3, NodeActivInst_0_3_2_4_3_3_2 ); //Name:GoToPosition
	_states.push_back( Node_0_3_2_4_3_3_2 );
	EndState* Node_0_3_2_4_3_3_3 = new EndState ( "Node_0_3_2_4_3_3_3", Node_0_3_2_4_3_3 ); //Name:0.3.2.4.3.3.3
	_states.push_back( Node_0_3_2_4_3_3_3 );
	EndState* Node_0_3_2_4_3_4 = new EndState ( "Node_0_3_2_4_3_4", Node_0_3_2_4_3 ); //Name:0.3.2.4.3.4
	_states.push_back( Node_0_3_2_4_3_4 );
	EndState* Node_0_3_2_4_4 = new EndState ( "Node_0_3_2_4_4", Node_0_3_2_4 ); //Name:0.3.2.4.4
	_states.push_back( Node_0_3_2_4_4 );
	EndState* Node_0_3_2_5 = new EndState ( "Node_0_3_2_5", Node_0_3_2 ); //Name:0.3.2.5
	_states.push_back( Node_0_3_2_5 );
	EndState* Node_0_3_3 = new EndState ( "Node_0_3_3", Node_0_3 ); //Name:0.3.3
	_states.push_back( Node_0_3_3 );
	EndState* Node_0_4 = new EndState ( "Node_0_4", Node_0 ); //Name:0.4
	_states.push_back( Node_0_4 );
	ICondition* TrCondInst_Goalie0_30_4 = new TrCond_Goalie0_30_4;
	_conditions.push_back( TrCondInst_Goalie0_30_4 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3, Node_0_4 , TrCondInst_Goalie0_30_4  ) ); //Goalie0.30.4
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_3_2_1, Node_0_3_2_2   ) ); //Goalie0.3.2.10.3.2.2
	ICondition* TrCondInst_Goalie0_3_2_20_3_2_3 = new TrCond_Goalie0_3_2_20_3_2_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_20_3_2_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_2, Node_0_3_2_3 , TrCondInst_Goalie0_3_2_20_3_2_3  ) ); //Goalie0.3.2.20.3.2.3
	ICondition* TrCondInst_Goalie0_3_2_20_3_2_4 = new TrCond_Goalie0_3_2_20_3_2_4;
	_conditions.push_back( TrCondInst_Goalie0_3_2_20_3_2_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_2, Node_0_3_2_4 , TrCondInst_Goalie0_3_2_20_3_2_4  ) ); //Goalie0.3.2.20.3.2.4
	ICondition* TrCondInst_Goalie0_3_2_40_3_2_5 = new TrCond_Goalie0_3_2_40_3_2_5;
	_conditions.push_back( TrCondInst_Goalie0_3_2_40_3_2_5 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_4, Node_0_3_2_5 , TrCondInst_Goalie0_3_2_40_3_2_5  ) ); //Goalie0.3.2.40.3.2.5
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3, Node_0_3_2_5   ) ); //Goalie0.3.2.30.3.2.5
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_3_2_3_1, Node_0_3_2_3_2   ) ); //Goalie0.3.2.3.10.3.2.3.2
	ICondition* TrCondInst_Goalie0_3_2_3_20_3_2_3_3 = new TrCond_Goalie0_3_2_3_20_3_2_3_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_20_3_2_3_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_3_2, Node_0_3_2_3_3 , TrCondInst_Goalie0_3_2_3_20_3_2_3_3  ) ); //Goalie0.3.2.3.20.3.2.3.3
	ICondition* TrCondInst_Goalie0_3_2_3_20_3_2_3_4 = new TrCond_Goalie0_3_2_3_20_3_2_3_4;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_20_3_2_3_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_3_2, Node_0_3_2_3_4 , TrCondInst_Goalie0_3_2_3_20_3_2_3_4  ) ); //Goalie0.3.2.3.20.3.2.3.4
	ICondition* TrCondInst_Goalie0_3_2_3_20_3_2_3_5 = new TrCond_Goalie0_3_2_3_20_3_2_3_5;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_20_3_2_3_5 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_3_2, Node_0_3_2_3_5 , TrCondInst_Goalie0_3_2_3_20_3_2_3_5  ) ); //Goalie0.3.2.3.20.3.2.3.5
	ICondition* TrCondInst_Goalie0_3_2_3_20_3_2_3_6 = new TrCond_Goalie0_3_2_3_20_3_2_3_6;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_20_3_2_3_6 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_3_2, Node_0_3_2_3_6 , TrCondInst_Goalie0_3_2_3_20_3_2_3_6  ) ); //Goalie0.3.2.3.20.3.2.3.6
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_6, Node_0_3_2_3_7   ) ); //Goalie0.3.2.3.60.3.2.3.7
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_5, Node_0_3_2_3_7   ) ); //Goalie0.3.2.3.50.3.2.3.7
	ICondition* TrCondInst_Goalie0_3_2_3_40_3_2_3_7 = new TrCond_Goalie0_3_2_3_40_3_2_3_7;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_40_3_2_3_7 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_4, Node_0_3_2_3_7 , TrCondInst_Goalie0_3_2_3_40_3_2_3_7  ) ); //Goalie0.3.2.3.40.3.2.3.7
	ICondition* TrCondInst_Goalie0_3_2_3_30_3_2_3_7 = new TrCond_Goalie0_3_2_3_30_3_2_3_7;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_30_3_2_3_7 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_3, Node_0_3_2_3_7 , TrCondInst_Goalie0_3_2_3_30_3_2_3_7  ) ); //Goalie0.3.2.3.30.3.2.3.7
	IAction* TrActionInst_Goalie0_3_2_3_3_10_3_2_3_3_2 = new TrAction_Goalie0_3_2_3_3_10_3_2_3_3_2;
	_actions.push_back( TrActionInst_Goalie0_3_2_3_3_10_3_2_3_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_3_1, Node_0_3_2_3_3_2  , TrActionInst_Goalie0_3_2_3_3_10_3_2_3_3_2 ) ); //Goalie0.3.2.3.3.10.3.2.3.3.2
	ICondition* TrCondInst_Goalie0_3_2_3_3_20_3_2_3_3_2 = new TrCond_Goalie0_3_2_3_3_20_3_2_3_3_2;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_3_20_3_2_3_3_2 );
	IAction* TrActionInst_Goalie0_3_2_3_3_20_3_2_3_3_2 = new TrAction_Goalie0_3_2_3_3_20_3_2_3_3_2;
	_actions.push_back( TrActionInst_Goalie0_3_2_3_3_20_3_2_3_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_3_2, Node_0_3_2_3_3_2 , TrCondInst_Goalie0_3_2_3_3_20_3_2_3_3_2 , TrActionInst_Goalie0_3_2_3_3_20_3_2_3_3_2 ) ); //Goalie0.3.2.3.3.20.3.2.3.3.2
	ICondition* TrCondInst_Goalie0_3_2_3_3_20_3_2_3_3_3 = new TrCond_Goalie0_3_2_3_3_20_3_2_3_3_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_3_20_3_2_3_3_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_3_2, Node_0_3_2_3_3_3 , TrCondInst_Goalie0_3_2_3_3_20_3_2_3_3_3  ) ); //Goalie0.3.2.3.3.20.3.2.3.3.3
	IAction* TrActionInst_Goalie0_3_2_3_4_10_3_2_3_4_2 = new TrAction_Goalie0_3_2_3_4_10_3_2_3_4_2;
	_actions.push_back( TrActionInst_Goalie0_3_2_3_4_10_3_2_3_4_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_4_1, Node_0_3_2_3_4_2  , TrActionInst_Goalie0_3_2_3_4_10_3_2_3_4_2 ) ); //Goalie0.3.2.3.4.10.3.2.3.4.2
	ICondition* TrCondInst_Goalie0_3_2_3_4_20_3_2_3_4_2 = new TrCond_Goalie0_3_2_3_4_20_3_2_3_4_2;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_4_20_3_2_3_4_2 );
	IAction* TrActionInst_Goalie0_3_2_3_4_20_3_2_3_4_2 = new TrAction_Goalie0_3_2_3_4_20_3_2_3_4_2;
	_actions.push_back( TrActionInst_Goalie0_3_2_3_4_20_3_2_3_4_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_4_2, Node_0_3_2_3_4_2 , TrCondInst_Goalie0_3_2_3_4_20_3_2_3_4_2 , TrActionInst_Goalie0_3_2_3_4_20_3_2_3_4_2 ) ); //Goalie0.3.2.3.4.20.3.2.3.4.2
	ICondition* TrCondInst_Goalie0_3_2_3_4_20_3_2_3_4_3 = new TrCond_Goalie0_3_2_3_4_20_3_2_3_4_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_4_20_3_2_3_4_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_4_2, Node_0_3_2_3_4_3 , TrCondInst_Goalie0_3_2_3_4_20_3_2_3_4_3  ) ); //Goalie0.3.2.3.4.20.3.2.3.4.3
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_6_1, Node_0_3_2_3_6_2   ) ); //Goalie0.3.2.3.6.10.3.2.3.6.2
	ICondition* TrCondInst_Goalie0_3_2_3_6_20_3_2_3_6_3 = new TrCond_Goalie0_3_2_3_6_20_3_2_3_6_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_6_20_3_2_3_6_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_6_2, Node_0_3_2_3_6_3 , TrCondInst_Goalie0_3_2_3_6_20_3_2_3_6_3  ) ); //Goalie0.3.2.3.6.20.3.2.3.6.3
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_6_3, Node_0_3_2_3_6_4   ) ); //Goalie0.3.2.3.6.30.3.2.3.6.4
	IAction* TrActionInst_Goalie0_3_2_3_6_2_10_3_2_3_6_2_2 = new TrAction_Goalie0_3_2_3_6_2_10_3_2_3_6_2_2;
	_actions.push_back( TrActionInst_Goalie0_3_2_3_6_2_10_3_2_3_6_2_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_6_2_1, Node_0_3_2_3_6_2_2  , TrActionInst_Goalie0_3_2_3_6_2_10_3_2_3_6_2_2 ) ); //Goalie0.3.2.3.6.2.10.3.2.3.6.2.2
	ICondition* TrCondInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_2 = new TrCond_Goalie0_3_2_3_6_2_20_3_2_3_6_2_2;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_2 );
	IAction* TrActionInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_2 = new TrAction_Goalie0_3_2_3_6_2_20_3_2_3_6_2_2;
	_actions.push_back( TrActionInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_6_2_2, Node_0_3_2_3_6_2_2 , TrCondInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_2 , TrActionInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_2 ) ); //Goalie0.3.2.3.6.2.20.3.2.3.6.2.2
	ICondition* TrCondInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_3 = new TrCond_Goalie0_3_2_3_6_2_20_3_2_3_6_2_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_6_2_2, Node_0_3_2_3_6_2_3 , TrCondInst_Goalie0_3_2_3_6_2_20_3_2_3_6_2_3  ) ); //Goalie0.3.2.3.6.2.20.3.2.3.6.2.3
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_3_2_3_6_3_1, Node_0_3_2_3_6_3_2   ) ); //Goalie0.3.2.3.6.3.10.3.2.3.6.3.2
	ICondition* TrCondInst_Goalie0_3_2_3_6_3_20_3_2_3_6_3_3 = new TrCond_Goalie0_3_2_3_6_3_20_3_2_3_6_3_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_6_3_20_3_2_3_6_3_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_3_6_3_2, Node_0_3_2_3_6_3_3 , TrCondInst_Goalie0_3_2_3_6_3_20_3_2_3_6_3_3  ) ); //Goalie0.3.2.3.6.3.20.3.2.3.6.3.3
	ICondition* TrCondInst_Goalie0_3_2_3_6_3_20_3_2_3_6_3_4 = new TrCond_Goalie0_3_2_3_6_3_20_3_2_3_6_3_4;
	_conditions.push_back( TrCondInst_Goalie0_3_2_3_6_3_20_3_2_3_6_3_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_3_6_3_2, Node_0_3_2_3_6_3_4 , TrCondInst_Goalie0_3_2_3_6_3_20_3_2_3_6_3_4  ) ); //Goalie0.3.2.3.6.3.20.3.2.3.6.3.4
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_3_6_3_3, Node_0_3_2_3_6_3_4   ) ); //Goalie0.3.2.3.6.3.30.3.2.3.6.3.4
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_4_1, Node_0_3_2_4_2   ) ); //Goalie0.3.2.4.10.3.2.4.2
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_4_2, Node_0_3_2_4_3   ) ); //Goalie0.3.2.4.20.3.2.4.3
	ICondition* TrCondInst_Goalie0_3_2_4_30_3_2_4_4 = new TrCond_Goalie0_3_2_4_30_3_2_4_4;
	_conditions.push_back( TrCondInst_Goalie0_3_2_4_30_3_2_4_4 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_4_3, Node_0_3_2_4_4 , TrCondInst_Goalie0_3_2_4_30_3_2_4_4  ) ); //Goalie0.3.2.4.30.3.2.4.4
	IAction* TrActionInst_Goalie0_3_2_4_3_3_10_3_2_4_3_3_2 = new TrAction_Goalie0_3_2_4_3_3_10_3_2_4_3_3_2;
	_actions.push_back( TrActionInst_Goalie0_3_2_4_3_3_10_3_2_4_3_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_4_3_3_1, Node_0_3_2_4_3_3_2  , TrActionInst_Goalie0_3_2_4_3_3_10_3_2_4_3_3_2 ) ); //Goalie0.3.2.4.3.3.10.3.2.4.3.3.2
	ICondition* TrCondInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2 = new TrCond_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2;
	_conditions.push_back( TrCondInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2 );
	IAction* TrActionInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2 = new TrAction_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2;
	_actions.push_back( TrActionInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_4_3_3_2, Node_0_3_2_4_3_3_2 , TrCondInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2 , TrActionInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2 ) ); //Goalie0.3.2.4.3.3.20.3.2.4.3.3.2
	ICondition* TrCondInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_3 = new TrCond_Goalie0_3_2_4_3_3_20_3_2_4_3_3_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_4_3_3_2, Node_0_3_2_4_3_3_3 , TrCondInst_Goalie0_3_2_4_3_3_20_3_2_4_3_3_3  ) ); //Goalie0.3.2.4.3.3.20.3.2.4.3.3.3
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_3_2_4_3_1, Node_0_3_2_4_3_2   ) ); //Goalie0.3.2.4.3.10.3.2.4.3.2
	ICondition* TrCondInst_Goalie0_3_2_4_3_20_3_2_4_3_3 = new TrCond_Goalie0_3_2_4_3_20_3_2_4_3_3;
	_conditions.push_back( TrCondInst_Goalie0_3_2_4_3_20_3_2_4_3_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_4_3_2, Node_0_3_2_4_3_3 , TrCondInst_Goalie0_3_2_4_3_20_3_2_4_3_3  ) ); //Goalie0.3.2.4.3.20.3.2.4.3.3
	ICondition* TrCondInst_Goalie0_3_2_4_3_20_3_2_4_3_4 = new TrCond_Goalie0_3_2_4_3_20_3_2_4_3_4;
	_conditions.push_back( TrCondInst_Goalie0_3_2_4_3_20_3_2_4_3_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_3_2_4_3_2, Node_0_3_2_4_3_4 , TrCondInst_Goalie0_3_2_4_3_20_3_2_4_3_4  ) ); //Goalie0.3.2.4.3.20.3.2.4.3.4
	ICondition* TrCondInst_Goalie0_3_2_4_3_30_3_2_4_3_4 = new TrCond_Goalie0_3_2_4_3_30_3_2_4_3_4;
	_conditions.push_back( TrCondInst_Goalie0_3_2_4_3_30_3_2_4_3_4 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2_4_3_3, Node_0_3_2_4_3_4 , TrCondInst_Goalie0_3_2_4_3_30_3_2_4_3_4  ) ); //Goalie0.3.2.4.3.30.3.2.4.3.4
	IAction* TrActionInst_Goalie0_3_10_3_2 = new TrAction_Goalie0_3_10_3_2;
	_actions.push_back( TrActionInst_Goalie0_3_10_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_1, Node_0_3_2  , TrActionInst_Goalie0_3_10_3_2 ) ); //Goalie0.3.10.3.2
	ICondition* TrCondInst_Goalie0_3_20_3_2 = new TrCond_Goalie0_3_20_3_2;
	_conditions.push_back( TrCondInst_Goalie0_3_20_3_2 );
	IAction* TrActionInst_Goalie0_3_20_3_2 = new TrAction_Goalie0_3_20_3_2;
	_actions.push_back( TrActionInst_Goalie0_3_20_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2, Node_0_3_2 , TrCondInst_Goalie0_3_20_3_2 , TrActionInst_Goalie0_3_20_3_2 ) ); //Goalie0.3.20.3.2
	ICondition* TrCondInst_Goalie0_3_20_3_3 = new TrCond_Goalie0_3_20_3_3;
	_conditions.push_back( TrCondInst_Goalie0_3_20_3_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_3_2, Node_0_3_3 , TrCondInst_Goalie0_3_20_3_3  ) ); //Goalie0.3.20.3.3
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_1, Node_0_3   ) ); //Goalie0.10.3
}

Goalie::~Goalie()
{
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

void Goalie::Start ()
{
	_statechart->Start();
}

void Goalie::Stop ()
{
	_statechart->Stop();
}

