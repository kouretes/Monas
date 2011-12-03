#include "goalie070611.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<goalie070611>::Type temp("goalie070611");
}
	
goalie070611::goalie070611(Narukom* com) {

	
	_statechart = new Statechart ( "Node_player", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );

	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2 = ActivityFactory::Instance()->CreateObject( "Init" );
	_activities.push_back( NodeActivInst_0_2 );			
	BasicState* Node_0_2 = new BasicState( "Node_Init", Node_0, NodeActivInst_0_2 ); //Name:Init
	_states.push_back( Node_0_2 );

	OrState* Node_0_3 = new OrState ( "Node_decision_one_or_more_times_", Node_0 );	//Name:decision_one_or_more_times_
	_states.push_back( Node_0_3 );

	StartState* Node_0_3_1 = new StartState ( "Node_0_3_1", Node_0_3 ); //Name:0.3.1
	_states.push_back( Node_0_3_1 );

	OrState* Node_0_3_2 = new OrState ( "Node_decision", Node_0_3 );	//Name:decision
	_states.push_back( Node_0_3_2 );

	StartState* Node_0_3_2_1 = new StartState ( "Node_0_3_2_1", Node_0_3_2 ); //Name:0.3.2.1
	_states.push_back( Node_0_3_2_1 );

	ConditionConnector* Node_0_3_2_2  = new ConditionConnector ( "Node_0_3_2_2", Node_0_3_2 ); //Name:0.3.2.2
	_states.push_back( Node_0_3_2_2 );

	OrState* Node_0_3_2_3 = new OrState ( "Node_play", Node_0_3_2 );	//Name:play
	_states.push_back( Node_0_3_2_3 );

	StartState* Node_0_3_2_3_1 = new StartState ( "Node_0_3_2_3_1", Node_0_3_2_3 ); //Name:0.3.2.3.1
	_states.push_back( Node_0_3_2_3_1 );

	ConditionConnector* Node_0_3_2_3_2  = new ConditionConnector ( "Node_0_3_2_3_2", Node_0_3_2_3 ); //Name:0.3.2.3.2
	_states.push_back( Node_0_3_2_3_2 );

	OrState* Node_0_3_2_3_3 = new OrState ( "Node_searchBallOrReturnToPosition", Node_0_3_2_3 );	//Name:searchBallOrReturnToPosition
	_states.push_back( Node_0_3_2_3_3 );

	StartState* Node_0_3_2_3_3_1 = new StartState ( "Node_0_3_2_3_3_1", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.1
	_states.push_back( Node_0_3_2_3_3_1 );

	ConditionConnector* Node_0_3_2_3_3_2  = new ConditionConnector ( "Node_0_3_2_3_3_2", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.2
	_states.push_back( Node_0_3_2_3_3_2 );

	OrState* Node_0_3_2_3_3_3 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_3_2_3_3 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_3 );

	StartState* Node_0_3_2_3_3_3_1 = new StartState ( "Node_0_3_2_3_3_3_1", Node_0_3_2_3_3_3 ); //Name:0.3.2.3.3.3.1
	_states.push_back( Node_0_3_2_3_3_3_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_3_2 = ActivityFactory::Instance()->CreateObject( "Scan" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_3_2 );			
	BasicState* Node_0_3_2_3_3_3_2 = new BasicState( "Node_Scan", Node_0_3_2_3_3_3, NodeActivInst_0_3_2_3_3_3_2 ); //Name:Scan
	_states.push_back( Node_0_3_2_3_3_3_2 );

	EndState* Node_0_3_2_3_3_3_3 = new EndState ( "Node_0_3_2_3_3_3_3", Node_0_3_2_3_3_3 ); //Name:0.3.2.3.3.3.3
	_states.push_back( Node_0_3_2_3_3_3_3 );

	OrState* Node_0_3_2_3_3_4 = new OrState ( "Node_GoToPosition_one_or_more_times_", Node_0_3_2_3_3 );	//Name:GoToPosition_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_4 );

	StartState* Node_0_3_2_3_3_4_1 = new StartState ( "Node_0_3_2_3_3_4_1", Node_0_3_2_3_3_4 ); //Name:0.3.2.3.3.4.1
	_states.push_back( Node_0_3_2_3_3_4_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_4_2 = ActivityFactory::Instance()->CreateObject( "GoToPosition" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_4_2 );			
	BasicState* Node_0_3_2_3_3_4_2 = new BasicState( "Node_GoToPosition", Node_0_3_2_3_3_4, NodeActivInst_0_3_2_3_3_4_2 ); //Name:GoToPosition
	_states.push_back( Node_0_3_2_3_3_4_2 );

	EndState* Node_0_3_2_3_3_4_3 = new EndState ( "Node_0_3_2_3_3_4_3", Node_0_3_2_3_3_4 ); //Name:0.3.2.3.3.4.3
	_states.push_back( Node_0_3_2_3_3_4_3 );

	EndState* Node_0_3_2_3_3_5 = new EndState ( "Node_0_3_2_3_3_5", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.5
	_states.push_back( Node_0_3_2_3_3_5 );

	OrState* Node_0_3_2_3_4 = new OrState ( "Node_ApproachBall_one_or_more_times_", Node_0_3_2_3 );	//Name:ApproachBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4 );

	StartState* Node_0_3_2_3_4_1 = new StartState ( "Node_0_3_2_3_4_1", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.1
	_states.push_back( Node_0_3_2_3_4_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_2 = ActivityFactory::Instance()->CreateObject( "ApproachBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_2 );			
	BasicState* Node_0_3_2_3_4_2 = new BasicState( "Node_ApproachBall", Node_0_3_2_3_4, NodeActivInst_0_3_2_3_4_2 ); //Name:ApproachBall
	_states.push_back( Node_0_3_2_3_4_2 );

	EndState* Node_0_3_2_3_4_3 = new EndState ( "Node_0_3_2_3_4_3", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.3
	_states.push_back( Node_0_3_2_3_4_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_5 = ActivityFactory::Instance()->CreateObject( "Kick" );
	_activities.push_back( NodeActivInst_0_3_2_3_5 );			
	BasicState* Node_0_3_2_3_5 = new BasicState( "Node_Kick", Node_0_3_2_3, NodeActivInst_0_3_2_3_5 ); //Name:Kick
	_states.push_back( Node_0_3_2_3_5 );

	OrState* Node_0_3_2_3_6 = new OrState ( "Node_followTheBall", Node_0_3_2_3 );	//Name:followTheBall
	_states.push_back( Node_0_3_2_3_6 );

	StartState* Node_0_3_2_3_6_1 = new StartState ( "Node_0_3_2_3_6_1", Node_0_3_2_3_6 ); //Name:0.3.2.3.6.1
	_states.push_back( Node_0_3_2_3_6_1 );

	OrState* Node_0_3_2_3_6_2 = new OrState ( "Node_Stare_one_or_more_times_", Node_0_3_2_3_6 );	//Name:Stare_one_or_more_times_
	_states.push_back( Node_0_3_2_3_6_2 );

	StartState* Node_0_3_2_3_6_2_1 = new StartState ( "Node_0_3_2_3_6_2_1", Node_0_3_2_3_6_2 ); //Name:0.3.2.3.6.2.1
	_states.push_back( Node_0_3_2_3_6_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_6_2_2 = ActivityFactory::Instance()->CreateObject( "Stare" );
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

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_6_3_3 = ActivityFactory::Instance()->CreateObject( "SpecialAction" );
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

			
	
	
	

	IActivity* NodeActivInst_0_3_2_4_2 = ActivityFactory::Instance()->CreateObject( "NoPlay" );
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

			
	
	
	

	IActivity* NodeActivInst_0_3_2_4_3_3_2 = ActivityFactory::Instance()->CreateObject( "GoToPosition" );
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
	
	
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2    ) ); //0.1TOInit
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3    ) ); //InitTOdecision_one_or_more_times_
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3,Node_0_4    ) ); //decision_one_or_more_times_TO0.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_1,Node_0_3_2_2    ) ); //0.3.2.1TO0.3.2.2
		

		
		
	ICondition* TrCondInst_0_3_2_2TOplay = new TrCond_0_3_2_2TOplay;
	_conditions.push_back( TrCondInst_0_3_2_2TOplay );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_3  ,TrCondInst_0_3_2_2TOplay  ) ); //0.3.2.2TOplay
		

		
		
	ICondition* TrCondInst_0_3_2_2TOnonPlaying = new TrCond_0_3_2_2TOnonPlaying;
	_conditions.push_back( TrCondInst_0_3_2_2TOnonPlaying );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_4  ,TrCondInst_0_3_2_2TOnonPlaying  ) ); //0.3.2.2TOnonPlaying
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4,Node_0_3_2_5    ) ); //nonPlayingTO0.3.2.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3,Node_0_3_2_5    ) ); //playTO0.3.2.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_1,Node_0_3_2_3_2    ) ); //0.3.2.3.1TO0.3.2.3.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOsearchBallOrReturnToPosition = new TrCond_0_3_2_3_2TOsearchBallOrReturnToPosition;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOsearchBallOrReturnToPosition );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_3  ,TrCondInst_0_3_2_3_2TOsearchBallOrReturnToPosition  ) ); //0.3.2.3.2TOsearchBallOrReturnToPosition
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOApproachBall_one_or_more_times_ = new TrCond_0_3_2_3_2TOApproachBall_one_or_more_times_;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOApproachBall_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_4  ,TrCondInst_0_3_2_3_2TOApproachBall_one_or_more_times_  ) ); //0.3.2.3.2TOApproachBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOKick = new TrCond_0_3_2_3_2TOKick;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_5  ,TrCondInst_0_3_2_3_2TOKick  ) ); //0.3.2.3.2TOKick
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOfollowTheBall = new TrCond_0_3_2_3_2TOfollowTheBall;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOfollowTheBall );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_6  ,TrCondInst_0_3_2_3_2TOfollowTheBall  ) ); //0.3.2.3.2TOfollowTheBall
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6,Node_0_3_2_3_7    ) ); //followTheBallTO0.3.2.3.7
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_5,Node_0_3_2_3_7    ) ); //KickTO0.3.2.3.7
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4,Node_0_3_2_3_7    ) ); //ApproachBall_one_or_more_times_TO0.3.2.3.7
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3,Node_0_3_2_3_7    ) ); //searchBallOrReturnToPositionTO0.3.2.3.7
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_3_1,Node_0_3_2_3_3_2    ) ); //0.3.2.3.3.1TO0.3.2.3.3.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_3_2TOScan_one_or_more_times_ = new TrCond_0_3_2_3_3_2TOScan_one_or_more_times_;
	_conditions.push_back( TrCondInst_0_3_2_3_3_2TOScan_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_3_2,Node_0_3_2_3_3_3  ,TrCondInst_0_3_2_3_3_2TOScan_one_or_more_times_  ) ); //0.3.2.3.3.2TOScan_one_or_more_times_
		

		
		
	ICondition* TrCondInst_0_3_2_3_3_2TOGoToPosition_one_or_more_times_ = new TrCond_0_3_2_3_3_2TOGoToPosition_one_or_more_times_;
	_conditions.push_back( TrCondInst_0_3_2_3_3_2TOGoToPosition_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_3_2,Node_0_3_2_3_3_4  ,TrCondInst_0_3_2_3_3_2TOGoToPosition_one_or_more_times_  ) ); //0.3.2.3.3.2TOGoToPosition_one_or_more_times_
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4,Node_0_3_2_3_3_5    ) ); //GoToPosition_one_or_more_times_TO0.3.2.3.3.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3,Node_0_3_2_3_3_5    ) ); //Scan_one_or_more_times_TO0.3.2.3.3.5
		

		
		
	ICondition* TrCondInst_0_3_2_3_3_3_1TOScan = new TrCond_0_3_2_3_3_3_1TOScan;
	_conditions.push_back( TrCondInst_0_3_2_3_3_3_1TOScan );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3_1,Node_0_3_2_3_3_3_2  ,TrCondInst_0_3_2_3_3_3_1TOScan  ) ); //0.3.2.3.3.3.1TOScan
		

		
		
	ICondition* TrCondInst_ScanTOScan = new TrCond_ScanTOScan;
	_conditions.push_back( TrCondInst_ScanTOScan );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3_2,Node_0_3_2_3_3_3_2  ,TrCondInst_ScanTOScan  ) ); //ScanTOScan
		

		
		
	ICondition* TrCondInst_ScanTO0_3_2_3_3_3_3 = new TrCond_ScanTO0_3_2_3_3_3_3;
	_conditions.push_back( TrCondInst_ScanTO0_3_2_3_3_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3_2,Node_0_3_2_3_3_3_3  ,TrCondInst_ScanTO0_3_2_3_3_3_3  ) ); //ScanTO0.3.2.3.3.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4_1,Node_0_3_2_3_3_4_2    ) ); //0.3.2.3.3.4.1TOGoToPosition
		

		
		
	ICondition* TrCondInst_GoToPositionTOGoToPosition = new TrCond_GoToPositionTOGoToPosition;
	_conditions.push_back( TrCondInst_GoToPositionTOGoToPosition );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4_2,Node_0_3_2_3_3_4_2  ,TrCondInst_GoToPositionTOGoToPosition  ) ); //GoToPositionTOGoToPosition
		

		
		
	ICondition* TrCondInst_GoToPositionTO0_3_2_3_3_4_3 = new TrCond_GoToPositionTO0_3_2_3_3_4_3;
	_conditions.push_back( TrCondInst_GoToPositionTO0_3_2_3_3_4_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4_2,Node_0_3_2_3_3_4_3  ,TrCondInst_GoToPositionTO0_3_2_3_3_4_3  ) ); //GoToPositionTO0.3.2.3.3.4.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_1,Node_0_3_2_3_4_2    ) ); //0.3.2.3.4.1TOApproachBall
		

		
		
	ICondition* TrCondInst_ApproachBallTOApproachBall = new TrCond_ApproachBallTOApproachBall;
	_conditions.push_back( TrCondInst_ApproachBallTOApproachBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_2  ,TrCondInst_ApproachBallTOApproachBall  ) ); //ApproachBallTOApproachBall
		

		
		
	ICondition* TrCondInst_ApproachBallTO0_3_2_3_4_3 = new TrCond_ApproachBallTO0_3_2_3_4_3;
	_conditions.push_back( TrCondInst_ApproachBallTO0_3_2_3_4_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_3  ,TrCondInst_ApproachBallTO0_3_2_3_4_3  ) ); //ApproachBallTO0.3.2.3.4.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_1,Node_0_3_2_3_6_2    ) ); //0.3.2.3.6.1TOStare_one_or_more_times_
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_2,Node_0_3_2_3_6_3    ) ); //Stare_one_or_more_times_TO_open_option_SpecialAction_close_option_
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_3,Node_0_3_2_3_6_4    ) ); //_open_option_SpecialAction_close_option_TO0.3.2.3.6.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_2_1,Node_0_3_2_3_6_2_2    ) ); //0.3.2.3.6.2.1TOStare
		

		
		
	ICondition* TrCondInst_StareTOStare = new TrCond_StareTOStare;
	_conditions.push_back( TrCondInst_StareTOStare );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_2_2,Node_0_3_2_3_6_2_2  ,TrCondInst_StareTOStare  ) ); //StareTOStare
		

		
		
	ICondition* TrCondInst_StareTO0_3_2_3_6_2_3 = new TrCond_StareTO0_3_2_3_6_2_3;
	_conditions.push_back( TrCondInst_StareTO0_3_2_3_6_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_2_2,Node_0_3_2_3_6_2_3  ,TrCondInst_StareTO0_3_2_3_6_2_3  ) ); //StareTO0.3.2.3.6.2.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_6_3_1,Node_0_3_2_3_6_3_2    ) ); //0.3.2.3.6.3.1TO0.3.2.3.6.3.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_6_3_2TOSpecialAction = new TrCond_0_3_2_3_6_3_2TOSpecialAction;
	_conditions.push_back( TrCondInst_0_3_2_3_6_3_2TOSpecialAction );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_6_3_2,Node_0_3_2_3_6_3_3  ,TrCondInst_0_3_2_3_6_3_2TOSpecialAction  ) ); //0.3.2.3.6.3.2TOSpecialAction
		

		
		
	ICondition* TrCondInst_0_3_2_3_6_3_2TO0_3_2_3_6_3_4 = new TrCond_0_3_2_3_6_3_2TO0_3_2_3_6_3_4;
	_conditions.push_back( TrCondInst_0_3_2_3_6_3_2TO0_3_2_3_6_3_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_6_3_2,Node_0_3_2_3_6_3_4  ,TrCondInst_0_3_2_3_6_3_2TO0_3_2_3_6_3_4  ) ); //0.3.2.3.6.3.2TO0.3.2.3.6.3.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_3_3,Node_0_3_2_3_6_3_4    ) ); //SpecialActionTO0.3.2.3.6.3.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4_1,Node_0_3_2_4_2    ) ); //0.3.2.4.1TONoPlay
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4_2,Node_0_3_2_4_3    ) ); //NoPlayTO_open_option_GoToPosition_one_or_more_times__close_option_
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4_3,Node_0_3_2_4_4    ) ); //_open_option_GoToPosition_one_or_more_times__close_option_TO0.3.2.4.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4_3_3_1,Node_0_3_2_4_3_3_2    ) ); //0.3.2.4.3.3.1TOGoToPosition
		

		
		
	TrCondInst_GoToPositionTOGoToPosition = new TrCond_GoToPositionTOGoToPosition;
	_conditions.push_back( TrCondInst_GoToPositionTOGoToPosition );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4_3_3_2,Node_0_3_2_4_3_3_2  ,TrCondInst_GoToPositionTOGoToPosition  ) ); //GoToPositionTOGoToPosition
		

		
		
	ICondition* TrCondInst_GoToPositionTO0_3_2_4_3_3_3 = new TrCond_GoToPositionTO0_3_2_4_3_3_3;
	_conditions.push_back( TrCondInst_GoToPositionTO0_3_2_4_3_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4_3_3_2,Node_0_3_2_4_3_3_3  ,TrCondInst_GoToPositionTO0_3_2_4_3_3_3  ) ); //GoToPositionTO0.3.2.4.3.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_4_3_1,Node_0_3_2_4_3_2    ) ); //0.3.2.4.3.1TO0.3.2.4.3.2
		

		
		
	ICondition* TrCondInst_0_3_2_4_3_2TOGoToPosition_one_or_more_times_ = new TrCond_0_3_2_4_3_2TOGoToPosition_one_or_more_times_;
	_conditions.push_back( TrCondInst_0_3_2_4_3_2TOGoToPosition_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_4_3_2,Node_0_3_2_4_3_3  ,TrCondInst_0_3_2_4_3_2TOGoToPosition_one_or_more_times_  ) ); //0.3.2.4.3.2TOGoToPosition_one_or_more_times_
		

		
		
	ICondition* TrCondInst_0_3_2_4_3_2TO0_3_2_4_3_4 = new TrCond_0_3_2_4_3_2TO0_3_2_4_3_4;
	_conditions.push_back( TrCondInst_0_3_2_4_3_2TO0_3_2_4_3_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_4_3_2,Node_0_3_2_4_3_4  ,TrCondInst_0_3_2_4_3_2TO0_3_2_4_3_4  ) ); //0.3.2.4.3.2TO0.3.2.4.3.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4_3_3,Node_0_3_2_4_3_4    ) ); //GoToPosition_one_or_more_times_TO0.3.2.4.3.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_1,Node_0_3_2    ) ); //0.3.1TOdecision
		

		
		
	ICondition* TrCondInst_decisionTOdecision = new TrCond_decisionTOdecision;
	_conditions.push_back( TrCondInst_decisionTOdecision );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_2  ,TrCondInst_decisionTOdecision  ) ); //decisionTOdecision
		

		
		
	ICondition* TrCondInst_decisionTO0_3_3 = new TrCond_decisionTO0_3_3;
	_conditions.push_back( TrCondInst_decisionTO0_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_3  ,TrCondInst_decisionTO0_3_3  ) ); //decisionTO0.3.3
	
}

goalie070611::~goalie070611() {
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

void goalie070611::Start () {
	_statechart->Start();
}

void goalie070611::Stop () {
	_statechart->Stop();
}

	
