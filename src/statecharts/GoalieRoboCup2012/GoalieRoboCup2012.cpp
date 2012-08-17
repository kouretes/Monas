
#include "GoalieRoboCup2012.h"
#include "transitionHeaders.h"

using namespace statechart_engine;

namespace
{
	StatechartRegistrar<GoalieRoboCup2012>::Type temp("GoalieRoboCup2012");
}

GoalieRoboCup2012::GoalieRoboCup2012(Narukom* com)
{
	_statechart = new Statechart ( "Node_Goalie", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );
	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );
	OrState* Node_0_2 = new OrState ( "Node__open_group_NonPlaying_or_Playing_close_group_", Node_0 );	//Name:_gr_NonPlaying_or_Playing
	_states.push_back( Node_0_2 );
	StartState* Node_0_2_1 = new StartState ( "Node_0_2_1", Node_0_2 ); //Name:0.2.1
	_states.push_back( Node_0_2_1 );
	ConditionConnector* Node_0_2_2  = new ConditionConnector ( "Node_0_2_2", Node_0_2 ); //Name:0.2.2
	_states.push_back( Node_0_2_2 );
	OrState* Node_0_2_3 = new OrState ( "Node_NonPlaying", Node_0_2 );	//Name:NonPlaying
	_states.push_back( Node_0_2_3 );
	StartState* Node_0_2_3_1 = new StartState ( "Node_0_2_3_1", Node_0_2_3 ); //Name:0.2.3.1
	_states.push_back( Node_0_2_3_1 );
	IActivity* NodeActivInst_0_2_3_2 = ActivityFactory::Instance()->CreateObject( "NoPlay" , *Node_0_2_3->GetBlackboard(), *Node_0_2_3->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_3_2 );
	BasicState* Node_0_2_3_2 = new BasicState( "Node_NoPlay", Node_0_2_3, NodeActivInst_0_2_3_2 ); //Name:NoPlay
	_states.push_back( Node_0_2_3_2 );
	OrState* Node_0_2_3_3 = new OrState ( "Node__open_option_GoToPosition_one_or_more_times__close_option_", Node_0_2_3 );	//Name:_op_GoToPosition_one_or_more_times_
	_states.push_back( Node_0_2_3_3 );
	StartState* Node_0_2_3_3_1 = new StartState ( "Node_0_2_3_3_1", Node_0_2_3_3 ); //Name:0.2.3.3.1
	_states.push_back( Node_0_2_3_3_1 );
	ConditionConnector* Node_0_2_3_3_2  = new ConditionConnector ( "Node_0_2_3_3_2", Node_0_2_3_3 ); //Name:0.2.3.3.2
	_states.push_back( Node_0_2_3_3_2 );
	OrState* Node_0_2_3_3_3 = new OrState ( "Node_GoToPosition_one_or_more_times_", Node_0_2_3_3 );	//Name:GoToPosition_one_or_more_times_
	_states.push_back( Node_0_2_3_3_3 );
	StartState* Node_0_2_3_3_3_1 = new StartState ( "Node_0_2_3_3_3_1", Node_0_2_3_3_3 ); //Name:0.2.3.3.3.1
	_states.push_back( Node_0_2_3_3_3_1 );
	IActivity* NodeActivInst_0_2_3_3_3_2 = ActivityFactory::Instance()->CreateObject( "GoToPosition" , *Node_0_2_3_3_3->GetBlackboard(), *Node_0_2_3_3_3->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_3_3_3_2 );
	BasicState* Node_0_2_3_3_3_2 = new BasicState( "Node_GoToPosition", Node_0_2_3_3_3, NodeActivInst_0_2_3_3_3_2 ); //Name:GoToPosition
	_states.push_back( Node_0_2_3_3_3_2 );
	EndState* Node_0_2_3_3_3_3 = new EndState ( "Node_0_2_3_3_3_3", Node_0_2_3_3_3 ); //Name:0.2.3.3.3.3
	_states.push_back( Node_0_2_3_3_3_3 );
	EndState* Node_0_2_3_3_4 = new EndState ( "Node_0_2_3_3_4", Node_0_2_3_3 ); //Name:0.2.3.3.4
	_states.push_back( Node_0_2_3_3_4 );
	EndState* Node_0_2_3_4 = new EndState ( "Node_0_2_3_4", Node_0_2_3 ); //Name:0.2.3.4
	_states.push_back( Node_0_2_3_4 );
	OrState* Node_0_2_4 = new OrState ( "Node_Playing", Node_0_2 );	//Name:Playing
	_states.push_back( Node_0_2_4 );
	StartState* Node_0_2_4_1 = new StartState ( "Node_0_2_4_1", Node_0_2_4 ); //Name:0.2.4.1
	_states.push_back( Node_0_2_4_1 );
	OrState* Node_0_2_4_2 = new OrState ( "Node__open_option_Localize_one_or_more_times__close_option_", Node_0_2_4 );	//Name:_op_Localize_one_or_more_times_
	_states.push_back( Node_0_2_4_2 );
	StartState* Node_0_2_4_2_1 = new StartState ( "Node_0_2_4_2_1", Node_0_2_4_2 ); //Name:0.2.4.2.1
	_states.push_back( Node_0_2_4_2_1 );
	ConditionConnector* Node_0_2_4_2_2  = new ConditionConnector ( "Node_0_2_4_2_2", Node_0_2_4_2 ); //Name:0.2.4.2.2
	_states.push_back( Node_0_2_4_2_2 );
	OrState* Node_0_2_4_2_3 = new OrState ( "Node_Localize_one_or_more_times_", Node_0_2_4_2 );	//Name:Localize_one_or_more_times_
	_states.push_back( Node_0_2_4_2_3 );
	StartState* Node_0_2_4_2_3_1 = new StartState ( "Node_0_2_4_2_3_1", Node_0_2_4_2_3 ); //Name:0.2.4.2.3.1
	_states.push_back( Node_0_2_4_2_3_1 );
	IActivity* NodeActivInst_0_2_4_2_3_2 = ActivityFactory::Instance()->CreateObject( "Localize" , *Node_0_2_4_2_3->GetBlackboard(), *Node_0_2_4_2_3->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_4_2_3_2 );
	BasicState* Node_0_2_4_2_3_2 = new BasicState( "Node_Localize", Node_0_2_4_2_3, NodeActivInst_0_2_4_2_3_2 ); //Name:Localize
	_states.push_back( Node_0_2_4_2_3_2 );
	EndState* Node_0_2_4_2_3_3 = new EndState ( "Node_0_2_4_2_3_3", Node_0_2_4_2_3 ); //Name:0.2.4.2.3.3
	_states.push_back( Node_0_2_4_2_3_3 );
	EndState* Node_0_2_4_2_4 = new EndState ( "Node_0_2_4_2_4", Node_0_2_4_2 ); //Name:0.2.4.2.4
	_states.push_back( Node_0_2_4_2_4 );
	OrState* Node_0_2_4_3 = new OrState ( "Node__open_option_GoToPosition_one_or_more_times__close_option_", Node_0_2_4 );	//Name:_op_GoToPosition_one_or_more_times_
	_states.push_back( Node_0_2_4_3 );
	StartState* Node_0_2_4_3_1 = new StartState ( "Node_0_2_4_3_1", Node_0_2_4_3 ); //Name:0.2.4.3.1
	_states.push_back( Node_0_2_4_3_1 );
	ConditionConnector* Node_0_2_4_3_2  = new ConditionConnector ( "Node_0_2_4_3_2", Node_0_2_4_3 ); //Name:0.2.4.3.2
	_states.push_back( Node_0_2_4_3_2 );
	OrState* Node_0_2_4_3_3 = new OrState ( "Node_GoToPosition_one_or_more_times_", Node_0_2_4_3 );	//Name:GoToPosition_one_or_more_times_
	_states.push_back( Node_0_2_4_3_3 );
	StartState* Node_0_2_4_3_3_1 = new StartState ( "Node_0_2_4_3_3_1", Node_0_2_4_3_3 ); //Name:0.2.4.3.3.1
	_states.push_back( Node_0_2_4_3_3_1 );
	IActivity* NodeActivInst_0_2_4_3_3_2 = ActivityFactory::Instance()->CreateObject( "GoToPosition" , *Node_0_2_4_3_3->GetBlackboard(), *Node_0_2_4_3_3->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_4_3_3_2 );
	BasicState* Node_0_2_4_3_3_2 = new BasicState( "Node_GoToPosition", Node_0_2_4_3_3, NodeActivInst_0_2_4_3_3_2 ); //Name:GoToPosition
	_states.push_back( Node_0_2_4_3_3_2 );
	EndState* Node_0_2_4_3_3_3 = new EndState ( "Node_0_2_4_3_3_3", Node_0_2_4_3_3 ); //Name:0.2.4.3.3.3
	_states.push_back( Node_0_2_4_3_3_3 );
	EndState* Node_0_2_4_3_4 = new EndState ( "Node_0_2_4_3_4", Node_0_2_4_3 ); //Name:0.2.4.3.4
	_states.push_back( Node_0_2_4_3_4 );
	OrState* Node_0_2_4_4 = new OrState ( "Node_MakeAction", Node_0_2_4 );	//Name:MakeAction
	_states.push_back( Node_0_2_4_4 );
	StartState* Node_0_2_4_4_1 = new StartState ( "Node_0_2_4_4_1", Node_0_2_4_4 ); //Name:0.2.4.4.1
	_states.push_back( Node_0_2_4_4_1 );
	ConditionConnector* Node_0_2_4_4_2  = new ConditionConnector ( "Node_0_2_4_4_2", Node_0_2_4_4 ); //Name:0.2.4.4.2
	_states.push_back( Node_0_2_4_4_2 );
	OrState* Node_0_2_4_4_3 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_2_4_4 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_2_4_4_3 );
	StartState* Node_0_2_4_4_3_1 = new StartState ( "Node_0_2_4_4_3_1", Node_0_2_4_4_3 ); //Name:0.2.4.4.3.1
	_states.push_back( Node_0_2_4_4_3_1 );
	IActivity* NodeActivInst_0_2_4_4_3_2 = ActivityFactory::Instance()->CreateObject( "Scan" , *Node_0_2_4_4_3->GetBlackboard(), *Node_0_2_4_4_3->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_4_4_3_2 );
	BasicState* Node_0_2_4_4_3_2 = new BasicState( "Node_Scan", Node_0_2_4_4_3, NodeActivInst_0_2_4_4_3_2 ); //Name:Scan
	_states.push_back( Node_0_2_4_4_3_2 );
	EndState* Node_0_2_4_4_3_3 = new EndState ( "Node_0_2_4_4_3_3", Node_0_2_4_4_3 ); //Name:0.2.4.4.3.3
	_states.push_back( Node_0_2_4_4_3_3 );
	OrState* Node_0_2_4_4_4 = new OrState ( "Node_ApproachBall_one_or_more_times_", Node_0_2_4_4 );	//Name:ApproachBall_one_or_more_times_
	_states.push_back( Node_0_2_4_4_4 );
	StartState* Node_0_2_4_4_4_1 = new StartState ( "Node_0_2_4_4_4_1", Node_0_2_4_4_4 ); //Name:0.2.4.4.4.1
	_states.push_back( Node_0_2_4_4_4_1 );
	IActivity* NodeActivInst_0_2_4_4_4_2 = ActivityFactory::Instance()->CreateObject( "ApproachBall" , *Node_0_2_4_4_4->GetBlackboard(), *Node_0_2_4_4_4->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_4_4_4_2 );
	BasicState* Node_0_2_4_4_4_2 = new BasicState( "Node_ApproachBall", Node_0_2_4_4_4, NodeActivInst_0_2_4_4_4_2 ); //Name:ApproachBall
	_states.push_back( Node_0_2_4_4_4_2 );
	EndState* Node_0_2_4_4_4_3 = new EndState ( "Node_0_2_4_4_4_3", Node_0_2_4_4_4 ); //Name:0.2.4.4.4.3
	_states.push_back( Node_0_2_4_4_4_3 );
	IActivity* NodeActivInst_0_2_4_4_5 = ActivityFactory::Instance()->CreateObject( "Kick" , *Node_0_2_4_4->GetBlackboard(), *Node_0_2_4_4->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_4_4_5 );
	BasicState* Node_0_2_4_4_5 = new BasicState( "Node_Kick", Node_0_2_4_4, NodeActivInst_0_2_4_4_5 ); //Name:Kick
	_states.push_back( Node_0_2_4_4_5 );
	OrState* Node_0_2_4_4_6 = new OrState ( "Node_FollowBall", Node_0_2_4_4 );	//Name:FollowBall
	_states.push_back( Node_0_2_4_4_6 );
	StartState* Node_0_2_4_4_6_1 = new StartState ( "Node_0_2_4_4_6_1", Node_0_2_4_4_6 ); //Name:0.2.4.4.6.1
	_states.push_back( Node_0_2_4_4_6_1 );
	OrState* Node_0_2_4_4_6_2 = new OrState ( "Node_Stare_one_or_more_times_", Node_0_2_4_4_6 );	//Name:Stare_one_or_more_times_
	_states.push_back( Node_0_2_4_4_6_2 );
	StartState* Node_0_2_4_4_6_2_1 = new StartState ( "Node_0_2_4_4_6_2_1", Node_0_2_4_4_6_2 ); //Name:0.2.4.4.6.2.1
	_states.push_back( Node_0_2_4_4_6_2_1 );
	IActivity* NodeActivInst_0_2_4_4_6_2_2 = ActivityFactory::Instance()->CreateObject( "Stare" , *Node_0_2_4_4_6_2->GetBlackboard(), *Node_0_2_4_4_6_2->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_4_4_6_2_2 );
	BasicState* Node_0_2_4_4_6_2_2 = new BasicState( "Node_Stare", Node_0_2_4_4_6_2, NodeActivInst_0_2_4_4_6_2_2 ); //Name:Stare
	_states.push_back( Node_0_2_4_4_6_2_2 );
	EndState* Node_0_2_4_4_6_2_3 = new EndState ( "Node_0_2_4_4_6_2_3", Node_0_2_4_4_6_2 ); //Name:0.2.4.4.6.2.3
	_states.push_back( Node_0_2_4_4_6_2_3 );
	OrState* Node_0_2_4_4_6_3 = new OrState ( "Node__open_option_SpecialAction_close_option_", Node_0_2_4_4_6 );	//Name:_op_SpecialAction
	_states.push_back( Node_0_2_4_4_6_3 );
	StartState* Node_0_2_4_4_6_3_1 = new StartState ( "Node_0_2_4_4_6_3_1", Node_0_2_4_4_6_3 ); //Name:0.2.4.4.6.3.1
	_states.push_back( Node_0_2_4_4_6_3_1 );
	ConditionConnector* Node_0_2_4_4_6_3_2  = new ConditionConnector ( "Node_0_2_4_4_6_3_2", Node_0_2_4_4_6_3 ); //Name:0.2.4.4.6.3.2
	_states.push_back( Node_0_2_4_4_6_3_2 );
	IActivity* NodeActivInst_0_2_4_4_6_3_3 = ActivityFactory::Instance()->CreateObject( "SpecialAction" , *Node_0_2_4_4_6_3->GetBlackboard(), *Node_0_2_4_4_6_3->GetXmlNode());
	_activities.push_back( NodeActivInst_0_2_4_4_6_3_3 );
	BasicState* Node_0_2_4_4_6_3_3 = new BasicState( "Node_SpecialAction", Node_0_2_4_4_6_3, NodeActivInst_0_2_4_4_6_3_3 ); //Name:SpecialAction
	_states.push_back( Node_0_2_4_4_6_3_3 );
	EndState* Node_0_2_4_4_6_3_4 = new EndState ( "Node_0_2_4_4_6_3_4", Node_0_2_4_4_6_3 ); //Name:0.2.4.4.6.3.4
	_states.push_back( Node_0_2_4_4_6_3_4 );
	EndState* Node_0_2_4_4_6_4 = new EndState ( "Node_0_2_4_4_6_4", Node_0_2_4_4_6 ); //Name:0.2.4.4.6.4
	_states.push_back( Node_0_2_4_4_6_4 );
	EndState* Node_0_2_4_4_7 = new EndState ( "Node_0_2_4_4_7", Node_0_2_4_4 ); //Name:0.2.4.4.7
	_states.push_back( Node_0_2_4_4_7 );
	EndState* Node_0_2_4_5 = new EndState ( "Node_0_2_4_5", Node_0_2_4 ); //Name:0.2.4.5
	_states.push_back( Node_0_2_4_5 );
	EndState* Node_0_2_5 = new EndState ( "Node_0_2_5", Node_0_2 ); //Name:0.2.5
	_states.push_back( Node_0_2_5 );
	EndState* Node_0_3 = new EndState ( "Node_0_3", Node_0 ); //Name:0.3
	_states.push_back( Node_0_3 );
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_2_1, Node_0_2_2   ) ); //GoalieRoboCup20120.2.10.2.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_20_2_3 = new TrCond_GoalieRoboCup20120_2_20_2_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_20_2_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_2, Node_0_2_3 , TrCondInst_GoalieRoboCup20120_2_20_2_3  ) ); //GoalieRoboCup20120.2.20.2.3
	ICondition* TrCondInst_GoalieRoboCup20120_2_20_2_4 = new TrCond_GoalieRoboCup20120_2_20_2_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_20_2_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_2, Node_0_2_4 , TrCondInst_GoalieRoboCup20120_2_20_2_4  ) ); //GoalieRoboCup20120.2.20.2.4
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4, Node_0_2_5   ) ); //GoalieRoboCup20120.2.40.2.5
	ICondition* TrCondInst_GoalieRoboCup20120_2_30_2_5 = new TrCond_GoalieRoboCup20120_2_30_2_5;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_30_2_5 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_3, Node_0_2_5 , TrCondInst_GoalieRoboCup20120_2_30_2_5  ) ); //GoalieRoboCup20120.2.30.2.5
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_3_1, Node_0_2_3_2   ) ); //GoalieRoboCup20120.2.3.10.2.3.2
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_3_2, Node_0_2_3_3   ) ); //GoalieRoboCup20120.2.3.20.2.3.3
	ICondition* TrCondInst_GoalieRoboCup20120_2_3_30_2_3_4 = new TrCond_GoalieRoboCup20120_2_3_30_2_3_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_3_30_2_3_4 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_3_3, Node_0_2_3_4 , TrCondInst_GoalieRoboCup20120_2_3_30_2_3_4  ) ); //GoalieRoboCup20120.2.3.30.2.3.4
	IAction* TrActionInst_GoalieRoboCup20120_2_3_3_3_10_2_3_3_3_2 = new TrAction_GoalieRoboCup20120_2_3_3_3_10_2_3_3_3_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_3_3_3_10_2_3_3_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_3_3_3_1, Node_0_2_3_3_3_2  , TrActionInst_GoalieRoboCup20120_2_3_3_3_10_2_3_3_3_2 ) ); //GoalieRoboCup20120.2.3.3.3.10.2.3.3.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_2 = new TrCond_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_2;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_2 );
	IAction* TrActionInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_2 = new TrAction_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_3_3_3_2, Node_0_2_3_3_3_2 , TrCondInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_2 , TrActionInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_2 ) ); //GoalieRoboCup20120.2.3.3.3.20.2.3.3.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_3 = new TrCond_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_3_3_3_2, Node_0_2_3_3_3_3 , TrCondInst_GoalieRoboCup20120_2_3_3_3_20_2_3_3_3_3  ) ); //GoalieRoboCup20120.2.3.3.3.20.2.3.3.3.3
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_2_3_3_1, Node_0_2_3_3_2   ) ); //GoalieRoboCup20120.2.3.3.10.2.3.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_3_3_20_2_3_3_3 = new TrCond_GoalieRoboCup20120_2_3_3_20_2_3_3_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_3_3_20_2_3_3_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_3_3_2, Node_0_2_3_3_3 , TrCondInst_GoalieRoboCup20120_2_3_3_20_2_3_3_3  ) ); //GoalieRoboCup20120.2.3.3.20.2.3.3.3
	ICondition* TrCondInst_GoalieRoboCup20120_2_3_3_20_2_3_3_4 = new TrCond_GoalieRoboCup20120_2_3_3_20_2_3_3_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_3_3_20_2_3_3_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_3_3_2, Node_0_2_3_3_4 , TrCondInst_GoalieRoboCup20120_2_3_3_20_2_3_3_4  ) ); //GoalieRoboCup20120.2.3.3.20.2.3.3.4
	ICondition* TrCondInst_GoalieRoboCup20120_2_3_3_30_2_3_3_4 = new TrCond_GoalieRoboCup20120_2_3_3_30_2_3_3_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_3_3_30_2_3_3_4 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_3_3_3, Node_0_2_3_3_4 , TrCondInst_GoalieRoboCup20120_2_3_3_30_2_3_3_4  ) ); //GoalieRoboCup20120.2.3.3.30.2.3.3.4
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_1, Node_0_2_4_2   ) ); //GoalieRoboCup20120.2.4.10.2.4.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_20_2_4_3 = new TrCond_GoalieRoboCup20120_2_4_20_2_4_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_20_2_4_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_2, Node_0_2_4_3 , TrCondInst_GoalieRoboCup20120_2_4_20_2_4_3  ) ); //GoalieRoboCup20120.2.4.20.2.4.3
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_30_2_4_4 = new TrCond_GoalieRoboCup20120_2_4_30_2_4_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_30_2_4_4 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_3, Node_0_2_4_4 , TrCondInst_GoalieRoboCup20120_2_4_30_2_4_4  ) ); //GoalieRoboCup20120.2.4.30.2.4.4
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4, Node_0_2_4_5   ) ); //GoalieRoboCup20120.2.4.40.2.4.5
	IAction* TrActionInst_GoalieRoboCup20120_2_4_2_3_10_2_4_2_3_2 = new TrAction_GoalieRoboCup20120_2_4_2_3_10_2_4_2_3_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_2_3_10_2_4_2_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_2_3_1, Node_0_2_4_2_3_2  , TrActionInst_GoalieRoboCup20120_2_4_2_3_10_2_4_2_3_2 ) ); //GoalieRoboCup20120.2.4.2.3.10.2.4.2.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2 = new TrCond_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2 );
	IAction* TrActionInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2 = new TrAction_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_2_3_2, Node_0_2_4_2_3_2 , TrCondInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2 , TrActionInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2 ) ); //GoalieRoboCup20120.2.4.2.3.20.2.4.2.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_3 = new TrCond_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_2_3_2, Node_0_2_4_2_3_3 , TrCondInst_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_3  ) ); //GoalieRoboCup20120.2.4.2.3.20.2.4.2.3.3
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_2_4_2_1, Node_0_2_4_2_2   ) ); //GoalieRoboCup20120.2.4.2.10.2.4.2.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_2_20_2_4_2_3 = new TrCond_GoalieRoboCup20120_2_4_2_20_2_4_2_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_2_20_2_4_2_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_2_2, Node_0_2_4_2_3 , TrCondInst_GoalieRoboCup20120_2_4_2_20_2_4_2_3  ) ); //GoalieRoboCup20120.2.4.2.20.2.4.2.3
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_2_20_2_4_2_4 = new TrCond_GoalieRoboCup20120_2_4_2_20_2_4_2_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_2_20_2_4_2_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_2_2, Node_0_2_4_2_4 , TrCondInst_GoalieRoboCup20120_2_4_2_20_2_4_2_4  ) ); //GoalieRoboCup20120.2.4.2.20.2.4.2.4
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_2_30_2_4_2_4 = new TrCond_GoalieRoboCup20120_2_4_2_30_2_4_2_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_2_30_2_4_2_4 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_2_3, Node_0_2_4_2_4 , TrCondInst_GoalieRoboCup20120_2_4_2_30_2_4_2_4  ) ); //GoalieRoboCup20120.2.4.2.30.2.4.2.4
	IAction* TrActionInst_GoalieRoboCup20120_2_4_3_3_10_2_4_3_3_2 = new TrAction_GoalieRoboCup20120_2_4_3_3_10_2_4_3_3_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_3_3_10_2_4_3_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_3_3_1, Node_0_2_4_3_3_2  , TrActionInst_GoalieRoboCup20120_2_4_3_3_10_2_4_3_3_2 ) ); //GoalieRoboCup20120.2.4.3.3.10.2.4.3.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_2 = new TrCond_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_2;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_2 );
	IAction* TrActionInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_2 = new TrAction_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_3_3_2, Node_0_2_4_3_3_2 , TrCondInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_2 , TrActionInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_2 ) ); //GoalieRoboCup20120.2.4.3.3.20.2.4.3.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_3 = new TrCond_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_3_3_2, Node_0_2_4_3_3_3 , TrCondInst_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_3  ) ); //GoalieRoboCup20120.2.4.3.3.20.2.4.3.3.3
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_2_4_3_1, Node_0_2_4_3_2   ) ); //GoalieRoboCup20120.2.4.3.10.2.4.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_3_20_2_4_3_3 = new TrCond_GoalieRoboCup20120_2_4_3_20_2_4_3_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_3_20_2_4_3_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_3_2, Node_0_2_4_3_3 , TrCondInst_GoalieRoboCup20120_2_4_3_20_2_4_3_3  ) ); //GoalieRoboCup20120.2.4.3.20.2.4.3.3
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_3_20_2_4_3_4 = new TrCond_GoalieRoboCup20120_2_4_3_20_2_4_3_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_3_20_2_4_3_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_3_2, Node_0_2_4_3_4 , TrCondInst_GoalieRoboCup20120_2_4_3_20_2_4_3_4  ) ); //GoalieRoboCup20120.2.4.3.20.2.4.3.4
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_3_30_2_4_3_4 = new TrCond_GoalieRoboCup20120_2_4_3_30_2_4_3_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_3_30_2_4_3_4 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_3_3, Node_0_2_4_3_4 , TrCondInst_GoalieRoboCup20120_2_4_3_30_2_4_3_4  ) ); //GoalieRoboCup20120.2.4.3.30.2.4.3.4
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_10_2_4_4_2 = new TrCond_GoalieRoboCup20120_2_4_4_10_2_4_4_2;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_10_2_4_4_2 );
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_2_4_4_1, Node_0_2_4_4_2 , TrCondInst_GoalieRoboCup20120_2_4_4_10_2_4_4_2  ) ); //GoalieRoboCup20120.2.4.4.10.2.4.4.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_3 = new TrCond_GoalieRoboCup20120_2_4_4_20_2_4_4_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_4_2, Node_0_2_4_4_3 , TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_3  ) ); //GoalieRoboCup20120.2.4.4.20.2.4.4.3
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_4 = new TrCond_GoalieRoboCup20120_2_4_4_20_2_4_4_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_4_2, Node_0_2_4_4_4 , TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_4  ) ); //GoalieRoboCup20120.2.4.4.20.2.4.4.4
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_5 = new TrCond_GoalieRoboCup20120_2_4_4_20_2_4_4_5;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_5 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_4_2, Node_0_2_4_4_5 , TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_5  ) ); //GoalieRoboCup20120.2.4.4.20.2.4.4.5
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_6 = new TrCond_GoalieRoboCup20120_2_4_4_20_2_4_4_6;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_6 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_4_2, Node_0_2_4_4_6 , TrCondInst_GoalieRoboCup20120_2_4_4_20_2_4_4_6  ) ); //GoalieRoboCup20120.2.4.4.20.2.4.4.6
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_6, Node_0_2_4_4_7   ) ); //GoalieRoboCup20120.2.4.4.60.2.4.4.7
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_5, Node_0_2_4_4_7   ) ); //GoalieRoboCup20120.2.4.4.50.2.4.4.7
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_40_2_4_4_7 = new TrCond_GoalieRoboCup20120_2_4_4_40_2_4_4_7;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_40_2_4_4_7 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_4, Node_0_2_4_4_7 , TrCondInst_GoalieRoboCup20120_2_4_4_40_2_4_4_7  ) ); //GoalieRoboCup20120.2.4.4.40.2.4.4.7
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_30_2_4_4_7 = new TrCond_GoalieRoboCup20120_2_4_4_30_2_4_4_7;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_30_2_4_4_7 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_3, Node_0_2_4_4_7 , TrCondInst_GoalieRoboCup20120_2_4_4_30_2_4_4_7  ) ); //GoalieRoboCup20120.2.4.4.30.2.4.4.7
	IAction* TrActionInst_GoalieRoboCup20120_2_4_4_3_10_2_4_4_3_2 = new TrAction_GoalieRoboCup20120_2_4_4_3_10_2_4_4_3_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_4_3_10_2_4_4_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_3_1, Node_0_2_4_4_3_2  , TrActionInst_GoalieRoboCup20120_2_4_4_3_10_2_4_4_3_2 ) ); //GoalieRoboCup20120.2.4.4.3.10.2.4.4.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_2 = new TrCond_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_2;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_2 );
	IAction* TrActionInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_2 = new TrAction_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_3_2, Node_0_2_4_4_3_2 , TrCondInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_2 , TrActionInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_2 ) ); //GoalieRoboCup20120.2.4.4.3.20.2.4.4.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_3 = new TrCond_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_3_2, Node_0_2_4_4_3_3 , TrCondInst_GoalieRoboCup20120_2_4_4_3_20_2_4_4_3_3  ) ); //GoalieRoboCup20120.2.4.4.3.20.2.4.4.3.3
	IAction* TrActionInst_GoalieRoboCup20120_2_4_4_4_10_2_4_4_4_2 = new TrAction_GoalieRoboCup20120_2_4_4_4_10_2_4_4_4_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_4_4_10_2_4_4_4_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_4_1, Node_0_2_4_4_4_2  , TrActionInst_GoalieRoboCup20120_2_4_4_4_10_2_4_4_4_2 ) ); //GoalieRoboCup20120.2.4.4.4.10.2.4.4.4.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_2 = new TrCond_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_2;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_2 );
	IAction* TrActionInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_2 = new TrAction_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_4_2, Node_0_2_4_4_4_2 , TrCondInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_2 , TrActionInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_2 ) ); //GoalieRoboCup20120.2.4.4.4.20.2.4.4.4.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_3 = new TrCond_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_4_2, Node_0_2_4_4_4_3 , TrCondInst_GoalieRoboCup20120_2_4_4_4_20_2_4_4_4_3  ) ); //GoalieRoboCup20120.2.4.4.4.20.2.4.4.4.3
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_6_1, Node_0_2_4_4_6_2   ) ); //GoalieRoboCup20120.2.4.4.6.10.2.4.4.6.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_6_20_2_4_4_6_3 = new TrCond_GoalieRoboCup20120_2_4_4_6_20_2_4_4_6_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_6_20_2_4_4_6_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_6_2, Node_0_2_4_4_6_3 , TrCondInst_GoalieRoboCup20120_2_4_4_6_20_2_4_4_6_3  ) ); //GoalieRoboCup20120.2.4.4.6.20.2.4.4.6.3
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_6_3, Node_0_2_4_4_6_4   ) ); //GoalieRoboCup20120.2.4.4.6.30.2.4.4.6.4
	IAction* TrActionInst_GoalieRoboCup20120_2_4_4_6_2_10_2_4_4_6_2_2 = new TrAction_GoalieRoboCup20120_2_4_4_6_2_10_2_4_4_6_2_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_4_6_2_10_2_4_4_6_2_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_6_2_1, Node_0_2_4_4_6_2_2  , TrActionInst_GoalieRoboCup20120_2_4_4_6_2_10_2_4_4_6_2_2 ) ); //GoalieRoboCup20120.2.4.4.6.2.10.2.4.4.6.2.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_2 = new TrCond_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_2;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_2 );
	IAction* TrActionInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_2 = new TrAction_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_6_2_2, Node_0_2_4_4_6_2_2 , TrCondInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_2 , TrActionInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_2 ) ); //GoalieRoboCup20120.2.4.4.6.2.20.2.4.4.6.2.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_3 = new TrCond_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_6_2_2, Node_0_2_4_4_6_2_3 , TrCondInst_GoalieRoboCup20120_2_4_4_6_2_20_2_4_4_6_2_3  ) ); //GoalieRoboCup20120.2.4.4.6.2.20.2.4.4.6.2.3
	_transitions.push_back( new TransitionSegment<State, ConditionConnector>(Node_0_2_4_4_6_3_1, Node_0_2_4_4_6_3_2   ) ); //GoalieRoboCup20120.2.4.4.6.3.10.2.4.4.6.3.2
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_3 = new TrCond_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_3 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_4_6_3_2, Node_0_2_4_4_6_3_3 , TrCondInst_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_3  ) ); //GoalieRoboCup20120.2.4.4.6.3.20.2.4.4.6.3.3
	ICondition* TrCondInst_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_4 = new TrCond_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_4;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_4 );
	_transitions.push_back( new TransitionSegment<ConditionConnector, State>(Node_0_2_4_4_6_3_2, Node_0_2_4_4_6_3_4 , TrCondInst_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_4  ) ); //GoalieRoboCup20120.2.4.4.6.3.20.2.4.4.6.3.4
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2_4_4_6_3_3, Node_0_2_4_4_6_3_4   ) ); //GoalieRoboCup20120.2.4.4.6.3.30.2.4.4.6.3.4
	IAction* TrActionInst_GoalieRoboCup20120_10_2 = new TrAction_GoalieRoboCup20120_10_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_10_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_1, Node_0_2  , TrActionInst_GoalieRoboCup20120_10_2 ) ); //GoalieRoboCup20120.10.2
	ICondition* TrCondInst_GoalieRoboCup20120_20_2 = new TrCond_GoalieRoboCup20120_20_2;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_20_2 );
	IAction* TrActionInst_GoalieRoboCup20120_20_2 = new TrAction_GoalieRoboCup20120_20_2;
	_actions.push_back( TrActionInst_GoalieRoboCup20120_20_2 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2, Node_0_2 , TrCondInst_GoalieRoboCup20120_20_2 , TrActionInst_GoalieRoboCup20120_20_2 ) ); //GoalieRoboCup20120.20.2
	ICondition* TrCondInst_GoalieRoboCup20120_20_3 = new TrCond_GoalieRoboCup20120_20_3;
	_conditions.push_back( TrCondInst_GoalieRoboCup20120_20_3 );
	_transitions.push_back( new TransitionSegment<State, State>(Node_0_2, Node_0_3 , TrCondInst_GoalieRoboCup20120_20_3  ) ); //GoalieRoboCup20120.20.3
}

GoalieRoboCup2012::~GoalieRoboCup2012()
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

void GoalieRoboCup2012::Start ()
{
	_statechart->Start();
}

void GoalieRoboCup2012::Stop ()
{
	_statechart->Stop();
}

