#include "centerfor.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<centerfor>::Type temp("centerfor");
}
	
centerfor::centerfor(Narukom* com) {

	
	_statechart = new Statechart ( "Node_player", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );

	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2 = ActivityFactory::Instance()->CreateObject( "Init" );
	_activities.push_back( NodeActivInst_0_2 );			
	BasicState* Node_0_2 = new BasicState( "Node_Init", Node_0, NodeActivInst_0_2 ); //Name:Init
	_states.push_back( Node_0_2 );

	OrState* Node_0_3 = new OrState ( "Node__open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_", Node_0 );	//Name:_gr_chooseRole_or_NoPlay_one_or_more_times_
	_states.push_back( Node_0_3 );

	StartState* Node_0_3_1 = new StartState ( "Node_0_3_1", Node_0_3 ); //Name:0.3.1
	_states.push_back( Node_0_3_1 );

	OrState* Node_0_3_2 = new OrState ( "Node__open_group_chooseRole_or_NoPlay_close_group_", Node_0_3 );	//Name:_gr_chooseRole_or_NoPlay
	_states.push_back( Node_0_3_2 );

	StartState* Node_0_3_2_1 = new StartState ( "Node_0_3_2_1", Node_0_3_2 ); //Name:0.3.2.1
	_states.push_back( Node_0_3_2_1 );

	ConditionConnector* Node_0_3_2_2  = new ConditionConnector ( "Node_0_3_2_2", Node_0_3_2 ); //Name:0.3.2.2
	_states.push_back( Node_0_3_2_2 );

	OrState* Node_0_3_2_3 = new OrState ( "Node_chooseRole", Node_0_3_2 );	//Name:chooseRole
	_states.push_back( Node_0_3_2_3 );

	StartState* Node_0_3_2_3_1 = new StartState ( "Node_0_3_2_3_1", Node_0_3_2_3 ); //Name:0.3.2.3.1
	_states.push_back( Node_0_3_2_3_1 );

	ConditionConnector* Node_0_3_2_3_2  = new ConditionConnector ( "Node_0_3_2_3_2", Node_0_3_2_3 ); //Name:0.3.2.3.2
	_states.push_back( Node_0_3_2_3_2 );

	OrState* Node_0_3_2_3_3 = new OrState ( "Node_center", Node_0_3_2_3 );	//Name:center
	_states.push_back( Node_0_3_2_3_3 );

	StartState* Node_0_3_2_3_3_1 = new StartState ( "Node_0_3_2_3_3_1", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.1
	_states.push_back( Node_0_3_2_3_3_1 );

	OrState* Node_0_3_2_3_3_2 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_3_2_3_3 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_2 );

	StartState* Node_0_3_2_3_3_2_1 = new StartState ( "Node_0_3_2_3_3_2_1", Node_0_3_2_3_3_2 ); //Name:0.3.2.3.3.2.1
	_states.push_back( Node_0_3_2_3_3_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_2_2 = ActivityFactory::Instance()->CreateObject( "Scan" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_2_2 );			
	BasicState* Node_0_3_2_3_3_2_2 = new BasicState( "Node_Scan", Node_0_3_2_3_3_2, NodeActivInst_0_3_2_3_3_2_2 ); //Name:Scan
	_states.push_back( Node_0_3_2_3_3_2_2 );

	EndState* Node_0_3_2_3_3_2_3 = new EndState ( "Node_0_3_2_3_3_2_3", Node_0_3_2_3_3_2 ); //Name:0.3.2.3.3.2.3
	_states.push_back( Node_0_3_2_3_3_2_3 );

	OrState* Node_0_3_2_3_3_3 = new OrState ( "Node_AproachBall_one_or_more_times_", Node_0_3_2_3_3 );	//Name:AproachBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_3 );

	StartState* Node_0_3_2_3_3_3_1 = new StartState ( "Node_0_3_2_3_3_3_1", Node_0_3_2_3_3_3 ); //Name:0.3.2.3.3.3.1
	_states.push_back( Node_0_3_2_3_3_3_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_3_2 = ActivityFactory::Instance()->CreateObject( "AproachBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_3_2 );			
	BasicState* Node_0_3_2_3_3_3_2 = new BasicState( "Node_AproachBall", Node_0_3_2_3_3_3, NodeActivInst_0_3_2_3_3_3_2 ); //Name:AproachBall
	_states.push_back( Node_0_3_2_3_3_3_2 );

	EndState* Node_0_3_2_3_3_3_3 = new EndState ( "Node_0_3_2_3_3_3_3", Node_0_3_2_3_3_3 ); //Name:0.3.2.3.3.3.3
	_states.push_back( Node_0_3_2_3_3_3_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_4 = ActivityFactory::Instance()->CreateObject( "MyLeftSideKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_4 );			
	BasicState* Node_0_3_2_3_3_4 = new BasicState( "Node_MyLeftSideKick", Node_0_3_2_3_3, NodeActivInst_0_3_2_3_3_4 ); //Name:MyLeftSideKick
	_states.push_back( Node_0_3_2_3_3_4 );

	OrState* Node_0_3_2_3_3_5 = new OrState ( "Node_FollowTheBall_one_or_more_times_", Node_0_3_2_3_3 );	//Name:FollowTheBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_5 );

	StartState* Node_0_3_2_3_3_5_1 = new StartState ( "Node_0_3_2_3_3_5_1", Node_0_3_2_3_3_5 ); //Name:0.3.2.3.3.5.1
	_states.push_back( Node_0_3_2_3_3_5_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_5_2 = ActivityFactory::Instance()->CreateObject( "FollowTheBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_5_2 );			
	BasicState* Node_0_3_2_3_3_5_2 = new BasicState( "Node_FollowTheBall", Node_0_3_2_3_3_5, NodeActivInst_0_3_2_3_3_5_2 ); //Name:FollowTheBall
	_states.push_back( Node_0_3_2_3_3_5_2 );

	EndState* Node_0_3_2_3_3_5_3 = new EndState ( "Node_0_3_2_3_3_5_3", Node_0_3_2_3_3_5 ); //Name:0.3.2.3.3.5.3
	_states.push_back( Node_0_3_2_3_3_5_3 );

	EndState* Node_0_3_2_3_3_6 = new EndState ( "Node_0_3_2_3_3_6", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.6
	_states.push_back( Node_0_3_2_3_3_6 );

	OrState* Node_0_3_2_3_4 = new OrState ( "Node_centerFor", Node_0_3_2_3 );	//Name:centerFor
	_states.push_back( Node_0_3_2_3_4 );

	StartState* Node_0_3_2_3_4_1 = new StartState ( "Node_0_3_2_3_4_1", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.1
	_states.push_back( Node_0_3_2_3_4_1 );

	OrState* Node_0_3_2_3_4_2 = new OrState ( "Node_ScanForBall_one_or_more_times_", Node_0_3_2_3_4 );	//Name:ScanForBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4_2 );

	StartState* Node_0_3_2_3_4_2_1 = new StartState ( "Node_0_3_2_3_4_2_1", Node_0_3_2_3_4_2 ); //Name:0.3.2.3.4.2.1
	_states.push_back( Node_0_3_2_3_4_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_2_2 = ActivityFactory::Instance()->CreateObject( "ScanForBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_2_2 );			
	BasicState* Node_0_3_2_3_4_2_2 = new BasicState( "Node_ScanForBall", Node_0_3_2_3_4_2, NodeActivInst_0_3_2_3_4_2_2 ); //Name:ScanForBall
	_states.push_back( Node_0_3_2_3_4_2_2 );

	EndState* Node_0_3_2_3_4_2_3 = new EndState ( "Node_0_3_2_3_4_2_3", Node_0_3_2_3_4_2 ); //Name:0.3.2.3.4.2.3
	_states.push_back( Node_0_3_2_3_4_2_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_3 = ActivityFactory::Instance()->CreateObject( "WalktoPost" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_3 );			
	BasicState* Node_0_3_2_3_4_3 = new BasicState( "Node_WalktoPost", Node_0_3_2_3_4, NodeActivInst_0_3_2_3_4_3 ); //Name:WalktoPost
	_states.push_back( Node_0_3_2_3_4_3 );

	OrState* Node_0_3_2_3_4_4 = new OrState ( "Node__open_option_SearchBall_one_or_more_times__close_option_", Node_0_3_2_3_4 );	//Name:_op_SearchBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4_4 );

	StartState* Node_0_3_2_3_4_4_1 = new StartState ( "Node_0_3_2_3_4_4_1", Node_0_3_2_3_4_4 ); //Name:0.3.2.3.4.4.1
	_states.push_back( Node_0_3_2_3_4_4_1 );

	ConditionConnector* Node_0_3_2_3_4_4_2  = new ConditionConnector ( "Node_0_3_2_3_4_4_2", Node_0_3_2_3_4_4 ); //Name:0.3.2.3.4.4.2
	_states.push_back( Node_0_3_2_3_4_4_2 );

	OrState* Node_0_3_2_3_4_4_3 = new OrState ( "Node_SearchBall_one_or_more_times_", Node_0_3_2_3_4_4 );	//Name:SearchBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4_4_3 );

	StartState* Node_0_3_2_3_4_4_3_1 = new StartState ( "Node_0_3_2_3_4_4_3_1", Node_0_3_2_3_4_4_3 ); //Name:0.3.2.3.4.4.3.1
	_states.push_back( Node_0_3_2_3_4_4_3_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_4_3_2 = ActivityFactory::Instance()->CreateObject( "SearchBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_4_3_2 );			
	BasicState* Node_0_3_2_3_4_4_3_2 = new BasicState( "Node_SearchBall", Node_0_3_2_3_4_4_3, NodeActivInst_0_3_2_3_4_4_3_2 ); //Name:SearchBall
	_states.push_back( Node_0_3_2_3_4_4_3_2 );

	EndState* Node_0_3_2_3_4_4_3_3 = new EndState ( "Node_0_3_2_3_4_4_3_3", Node_0_3_2_3_4_4_3 ); //Name:0.3.2.3.4.4.3.3
	_states.push_back( Node_0_3_2_3_4_4_3_3 );

	EndState* Node_0_3_2_3_4_4_4 = new EndState ( "Node_0_3_2_3_4_4_4", Node_0_3_2_3_4_4 ); //Name:0.3.2.3.4.4.4
	_states.push_back( Node_0_3_2_3_4_4_4 );

	OrState* Node_0_3_2_3_4_5 = new OrState ( "Node_AproachBallFor_one_or_more_times_", Node_0_3_2_3_4 );	//Name:AproachBallFor_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4_5 );

	StartState* Node_0_3_2_3_4_5_1 = new StartState ( "Node_0_3_2_3_4_5_1", Node_0_3_2_3_4_5 ); //Name:0.3.2.3.4.5.1
	_states.push_back( Node_0_3_2_3_4_5_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_5_2 = ActivityFactory::Instance()->CreateObject( "AproachBallFor" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_5_2 );			
	BasicState* Node_0_3_2_3_4_5_2 = new BasicState( "Node_AproachBallFor", Node_0_3_2_3_4_5, NodeActivInst_0_3_2_3_4_5_2 ); //Name:AproachBallFor
	_states.push_back( Node_0_3_2_3_4_5_2 );

	EndState* Node_0_3_2_3_4_5_3 = new EndState ( "Node_0_3_2_3_4_5_3", Node_0_3_2_3_4_5 ); //Name:0.3.2.3.4.5.3
	_states.push_back( Node_0_3_2_3_4_5_3 );

	OrState* Node_0_3_2_3_4_6 = new OrState ( "Node_kick", Node_0_3_2_3_4 );	//Name:kick
	_states.push_back( Node_0_3_2_3_4_6 );

	StartState* Node_0_3_2_3_4_6_1 = new StartState ( "Node_0_3_2_3_4_6_1", Node_0_3_2_3_4_6 ); //Name:0.3.2.3.4.6.1
	_states.push_back( Node_0_3_2_3_4_6_1 );

	ConditionConnector* Node_0_3_2_3_4_6_2  = new ConditionConnector ( "Node_0_3_2_3_4_6_2", Node_0_3_2_3_4_6 ); //Name:0.3.2.3.4.6.2
	_states.push_back( Node_0_3_2_3_4_6_2 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_6_3 = ActivityFactory::Instance()->CreateObject( "MyLeftKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_6_3 );			
	BasicState* Node_0_3_2_3_4_6_3 = new BasicState( "Node_MyLeftKick", Node_0_3_2_3_4_6, NodeActivInst_0_3_2_3_4_6_3 ); //Name:MyLeftKick
	_states.push_back( Node_0_3_2_3_4_6_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_6_4 = ActivityFactory::Instance()->CreateObject( "MyRightKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_6_4 );			
	BasicState* Node_0_3_2_3_4_6_4 = new BasicState( "Node_MyRightKick", Node_0_3_2_3_4_6, NodeActivInst_0_3_2_3_4_6_4 ); //Name:MyRightKick
	_states.push_back( Node_0_3_2_3_4_6_4 );

	EndState* Node_0_3_2_3_4_6_5 = new EndState ( "Node_0_3_2_3_4_6_5", Node_0_3_2_3_4_6 ); //Name:0.3.2.3.4.6.5
	_states.push_back( Node_0_3_2_3_4_6_5 );

	EndState* Node_0_3_2_3_4_7 = new EndState ( "Node_0_3_2_3_4_7", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.7
	_states.push_back( Node_0_3_2_3_4_7 );

	EndState* Node_0_3_2_3_5 = new EndState ( "Node_0_3_2_3_5", Node_0_3_2_3 ); //Name:0.3.2.3.5
	_states.push_back( Node_0_3_2_3_5 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_4 = ActivityFactory::Instance()->CreateObject( "NoPlay" );
	_activities.push_back( NodeActivInst_0_3_2_4 );			
	BasicState* Node_0_3_2_4 = new BasicState( "Node_NoPlay", Node_0_3_2, NodeActivInst_0_3_2_4 ); //Name:NoPlay
	_states.push_back( Node_0_3_2_4 );

	EndState* Node_0_3_2_5 = new EndState ( "Node_0_3_2_5", Node_0_3_2 ); //Name:0.3.2.5
	_states.push_back( Node_0_3_2_5 );

	EndState* Node_0_3_3 = new EndState ( "Node_0_3_3", Node_0_3 ); //Name:0.3.3
	_states.push_back( Node_0_3_3 );

	EndState* Node_0_4 = new EndState ( "Node_0_4", Node_0 ); //Name:0.4
	_states.push_back( Node_0_4 );
	
	
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2    ) ); //0.1TOInit
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3    ) ); //InitTO_open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_
		

		
		
	ICondition* TrCondInst__open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_TO0_4 = new TrCond__open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_TO0_4;
	_conditions.push_back( TrCondInst__open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_TO0_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3,Node_0_4  ,TrCondInst__open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_TO0_4  ) ); //_open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_TO0.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_1,Node_0_3_2_2    ) ); //0.3.2.1TO0.3.2.2
		

		
		
	ICondition* TrCondInst_0_3_2_2TOchooseRole = new TrCond_0_3_2_2TOchooseRole;
	_conditions.push_back( TrCondInst_0_3_2_2TOchooseRole );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_3  ,TrCondInst_0_3_2_2TOchooseRole  ) ); //0.3.2.2TOchooseRole
		

		
		
	ICondition* TrCondInst_0_3_2_2TONoPlay = new TrCond_0_3_2_2TONoPlay;
	_conditions.push_back( TrCondInst_0_3_2_2TONoPlay );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_4  ,TrCondInst_0_3_2_2TONoPlay  ) ); //0.3.2.2TONoPlay
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4,Node_0_3_2_5    ) ); //NoPlayTO0.3.2.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3,Node_0_3_2_5    ) ); //chooseRoleTO0.3.2.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_1,Node_0_3_2_3_2    ) ); //0.3.2.3.1TO0.3.2.3.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOcenter = new TrCond_0_3_2_3_2TOcenter;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOcenter );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_3  ,TrCondInst_0_3_2_3_2TOcenter  ) ); //0.3.2.3.2TOcenter
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOcenterFor = new TrCond_0_3_2_3_2TOcenterFor;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOcenterFor );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_4  ,TrCondInst_0_3_2_3_2TOcenterFor  ) ); //0.3.2.3.2TOcenterFor
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4,Node_0_3_2_3_5    ) ); //centerForTO0.3.2.3.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3,Node_0_3_2_3_5    ) ); //centerTO0.3.2.3.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_1,Node_0_3_2_3_3_2    ) ); //0.3.2.3.3.1TOScan_one_or_more_times_
		

		
		
	ICondition* TrCondInst_Scan_one_or_more_times_TOAproachBall_one_or_more_times_ = new TrCond_Scan_one_or_more_times_TOAproachBall_one_or_more_times_;
	_conditions.push_back( TrCondInst_Scan_one_or_more_times_TOAproachBall_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2,Node_0_3_2_3_3_3  ,TrCondInst_Scan_one_or_more_times_TOAproachBall_one_or_more_times_  ) ); //Scan_one_or_more_times_TOAproachBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_AproachBall_one_or_more_times_TOMyLeftSideKick = new TrCond_AproachBall_one_or_more_times_TOMyLeftSideKick;
	_conditions.push_back( TrCondInst_AproachBall_one_or_more_times_TOMyLeftSideKick );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3,Node_0_3_2_3_3_4  ,TrCondInst_AproachBall_one_or_more_times_TOMyLeftSideKick  ) ); //AproachBall_one_or_more_times_TOMyLeftSideKick
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4,Node_0_3_2_3_3_5    ) ); //MyLeftSideKickTOFollowTheBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_6 = new TrCond_FollowTheBall_one_or_more_times_TO0_3_2_3_3_6;
	_conditions.push_back( TrCondInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_6 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_5,Node_0_3_2_3_3_6  ,TrCondInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_6  ) ); //FollowTheBall_one_or_more_times_TO0.3.2.3.3.6
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_1,Node_0_3_2_3_3_2_2    ) ); //0.3.2.3.3.2.1TOScan
		

		
		
	ICondition* TrCondInst_ScanTOScan = new TrCond_ScanTOScan;
	_conditions.push_back( TrCondInst_ScanTOScan );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2,Node_0_3_2_3_3_2_2  ,TrCondInst_ScanTOScan  ) ); //ScanTOScan
		

		
		
	ICondition* TrCondInst_ScanTO0_3_2_3_3_2_3 = new TrCond_ScanTO0_3_2_3_3_2_3;
	_conditions.push_back( TrCondInst_ScanTO0_3_2_3_3_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2,Node_0_3_2_3_3_2_3  ,TrCondInst_ScanTO0_3_2_3_3_2_3  ) ); //ScanTO0.3.2.3.3.2.3
		

		
		
	ICondition* TrCondInst_0_3_2_3_3_3_1TOAproachBall = new TrCond_0_3_2_3_3_3_1TOAproachBall;
	_conditions.push_back( TrCondInst_0_3_2_3_3_3_1TOAproachBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3_1,Node_0_3_2_3_3_3_2  ,TrCondInst_0_3_2_3_3_3_1TOAproachBall  ) ); //0.3.2.3.3.3.1TOAproachBall
		

		
		
	ICondition* TrCondInst_AproachBallTOAproachBall = new TrCond_AproachBallTOAproachBall;
	_conditions.push_back( TrCondInst_AproachBallTOAproachBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3_2,Node_0_3_2_3_3_3_2  ,TrCondInst_AproachBallTOAproachBall  ) ); //AproachBallTOAproachBall
		

		
		
	ICondition* TrCondInst_AproachBallTO0_3_2_3_3_3_3 = new TrCond_AproachBallTO0_3_2_3_3_3_3;
	_conditions.push_back( TrCondInst_AproachBallTO0_3_2_3_3_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3_2,Node_0_3_2_3_3_3_3  ,TrCondInst_AproachBallTO0_3_2_3_3_3_3  ) ); //AproachBallTO0.3.2.3.3.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_5_1,Node_0_3_2_3_3_5_2    ) ); //0.3.2.3.3.5.1TOFollowTheBall
		

		
		
	ICondition* TrCondInst_FollowTheBallTOFollowTheBall = new TrCond_FollowTheBallTOFollowTheBall;
	_conditions.push_back( TrCondInst_FollowTheBallTOFollowTheBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_5_2,Node_0_3_2_3_3_5_2  ,TrCondInst_FollowTheBallTOFollowTheBall  ) ); //FollowTheBallTOFollowTheBall
		

		
		
	ICondition* TrCondInst_FollowTheBallTO0_3_2_3_3_5_3 = new TrCond_FollowTheBallTO0_3_2_3_3_5_3;
	_conditions.push_back( TrCondInst_FollowTheBallTO0_3_2_3_3_5_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_5_2,Node_0_3_2_3_3_5_3  ,TrCondInst_FollowTheBallTO0_3_2_3_3_5_3  ) ); //FollowTheBallTO0.3.2.3.3.5.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_1,Node_0_3_2_3_4_2    ) ); //0.3.2.3.4.1TOScanForBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_ScanForBall_one_or_more_times_TOWalktoPost = new TrCond_ScanForBall_one_or_more_times_TOWalktoPost;
	_conditions.push_back( TrCondInst_ScanForBall_one_or_more_times_TOWalktoPost );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_3  ,TrCondInst_ScanForBall_one_or_more_times_TOWalktoPost  ) ); //ScanForBall_one_or_more_times_TOWalktoPost
		

		
		
	ICondition* TrCondInst_WalktoPostTO_open_option_SearchBall_one_or_more_times__close_option_ = new TrCond_WalktoPostTO_open_option_SearchBall_one_or_more_times__close_option_;
	_conditions.push_back( TrCondInst_WalktoPostTO_open_option_SearchBall_one_or_more_times__close_option_ );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_3,Node_0_3_2_3_4_4  ,TrCondInst_WalktoPostTO_open_option_SearchBall_one_or_more_times__close_option_  ) ); //WalktoPostTO_open_option_SearchBall_one_or_more_times__close_option_
		

		
		
	ICondition* TrCondInst__open_option_SearchBall_one_or_more_times__close_option_TOAproachBallFor_one_or_more_times_ = new TrCond__open_option_SearchBall_one_or_more_times__close_option_TOAproachBallFor_one_or_more_times_;
	_conditions.push_back( TrCondInst__open_option_SearchBall_one_or_more_times__close_option_TOAproachBallFor_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4,Node_0_3_2_3_4_5  ,TrCondInst__open_option_SearchBall_one_or_more_times__close_option_TOAproachBallFor_one_or_more_times_  ) ); //_open_option_SearchBall_one_or_more_times__close_option_TOAproachBallFor_one_or_more_times_
		

		
		
	ICondition* TrCondInst_AproachBallFor_one_or_more_times_TOkick = new TrCond_AproachBallFor_one_or_more_times_TOkick;
	_conditions.push_back( TrCondInst_AproachBallFor_one_or_more_times_TOkick );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_5,Node_0_3_2_3_4_6  ,TrCondInst_AproachBallFor_one_or_more_times_TOkick  ) ); //AproachBallFor_one_or_more_times_TOkick
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_6,Node_0_3_2_3_4_7    ) ); //kickTO0.3.2.3.4.7
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2_1,Node_0_3_2_3_4_2_2    ) ); //0.3.2.3.4.2.1TOScanForBall
		

		
		
	ICondition* TrCondInst_ScanForBallTOScanForBall = new TrCond_ScanForBallTOScanForBall;
	_conditions.push_back( TrCondInst_ScanForBallTOScanForBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2_2,Node_0_3_2_3_4_2_2  ,TrCondInst_ScanForBallTOScanForBall  ) ); //ScanForBallTOScanForBall
		

		
		
	ICondition* TrCondInst_ScanForBallTO0_3_2_3_4_2_3 = new TrCond_ScanForBallTO0_3_2_3_4_2_3;
	_conditions.push_back( TrCondInst_ScanForBallTO0_3_2_3_4_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2_2,Node_0_3_2_3_4_2_3  ,TrCondInst_ScanForBallTO0_3_2_3_4_2_3  ) ); //ScanForBallTO0.3.2.3.4.2.3
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_4_3_1TOSearchBall = new TrCond_0_3_2_3_4_4_3_1TOSearchBall;
	_conditions.push_back( TrCondInst_0_3_2_3_4_4_3_1TOSearchBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_3_1,Node_0_3_2_3_4_4_3_2  ,TrCondInst_0_3_2_3_4_4_3_1TOSearchBall  ) ); //0.3.2.3.4.4.3.1TOSearchBall
		

		
		
	ICondition* TrCondInst_SearchBallTOSearchBall = new TrCond_SearchBallTOSearchBall;
	_conditions.push_back( TrCondInst_SearchBallTOSearchBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_3_2,Node_0_3_2_3_4_4_3_2  ,TrCondInst_SearchBallTOSearchBall  ) ); //SearchBallTOSearchBall
		

		
		
	ICondition* TrCondInst_SearchBallTO0_3_2_3_4_4_3_3 = new TrCond_SearchBallTO0_3_2_3_4_4_3_3;
	_conditions.push_back( TrCondInst_SearchBallTO0_3_2_3_4_4_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_3_2,Node_0_3_2_3_4_4_3_3  ,TrCondInst_SearchBallTO0_3_2_3_4_4_3_3  ) ); //SearchBallTO0.3.2.3.4.4.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_4_4_1,Node_0_3_2_3_4_4_2    ) ); //0.3.2.3.4.4.1TO0.3.2.3.4.4.2
		

		

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_4_2,Node_0_3_2_3_4_4_3    ) ); //0.3.2.3.4.4.2TOSearchBall_one_or_more_times_
		

		

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_4_2,Node_0_3_2_3_4_4_4    ) ); //0.3.2.3.4.4.2TO0.3.2.3.4.4.4
		

		
		
	ICondition* TrCondInst_SearchBall_one_or_more_times_TO0_3_2_3_4_4_4 = new TrCond_SearchBall_one_or_more_times_TO0_3_2_3_4_4_4;
	_conditions.push_back( TrCondInst_SearchBall_one_or_more_times_TO0_3_2_3_4_4_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_3,Node_0_3_2_3_4_4_4  ,TrCondInst_SearchBall_one_or_more_times_TO0_3_2_3_4_4_4  ) ); //SearchBall_one_or_more_times_TO0.3.2.3.4.4.4
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_5_1TOAproachBallFor = new TrCond_0_3_2_3_4_5_1TOAproachBallFor;
	_conditions.push_back( TrCondInst_0_3_2_3_4_5_1TOAproachBallFor );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_5_1,Node_0_3_2_3_4_5_2  ,TrCondInst_0_3_2_3_4_5_1TOAproachBallFor  ) ); //0.3.2.3.4.5.1TOAproachBallFor
		

		
		
	ICondition* TrCondInst_AproachBallForTOAproachBallFor = new TrCond_AproachBallForTOAproachBallFor;
	_conditions.push_back( TrCondInst_AproachBallForTOAproachBallFor );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_5_2,Node_0_3_2_3_4_5_2  ,TrCondInst_AproachBallForTOAproachBallFor  ) ); //AproachBallForTOAproachBallFor
		

		
		
	ICondition* TrCondInst_AproachBallForTO0_3_2_3_4_5_3 = new TrCond_AproachBallForTO0_3_2_3_4_5_3;
	_conditions.push_back( TrCondInst_AproachBallForTO0_3_2_3_4_5_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_5_2,Node_0_3_2_3_4_5_3  ,TrCondInst_AproachBallForTO0_3_2_3_4_5_3  ) ); //AproachBallForTO0.3.2.3.4.5.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_4_6_1,Node_0_3_2_3_4_6_2    ) ); //0.3.2.3.4.6.1TO0.3.2.3.4.6.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_6_2TOMyLeftKick = new TrCond_0_3_2_3_4_6_2TOMyLeftKick;
	_conditions.push_back( TrCondInst_0_3_2_3_4_6_2TOMyLeftKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_6_2,Node_0_3_2_3_4_6_3  ,TrCondInst_0_3_2_3_4_6_2TOMyLeftKick  ) ); //0.3.2.3.4.6.2TOMyLeftKick
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_6_2TOMyRightKick = new TrCond_0_3_2_3_4_6_2TOMyRightKick;
	_conditions.push_back( TrCondInst_0_3_2_3_4_6_2TOMyRightKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_6_2,Node_0_3_2_3_4_6_4  ,TrCondInst_0_3_2_3_4_6_2TOMyRightKick  ) ); //0.3.2.3.4.6.2TOMyRightKick
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_6_4,Node_0_3_2_3_4_6_5    ) ); //MyRightKickTO0.3.2.3.4.6.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_6_3,Node_0_3_2_3_4_6_5    ) ); //MyLeftKickTO0.3.2.3.4.6.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_1,Node_0_3_2    ) ); //0.3.1TO_open_group_chooseRole_or_NoPlay_close_group_
		

		
		
	ICondition* TrCondInst__open_group_chooseRole_or_NoPlay_close_group_TO_open_group_chooseRole_or_NoPlay_close_group_ = new TrCond__open_group_chooseRole_or_NoPlay_close_group_TO_open_group_chooseRole_or_NoPlay_close_group_;
	_conditions.push_back( TrCondInst__open_group_chooseRole_or_NoPlay_close_group_TO_open_group_chooseRole_or_NoPlay_close_group_ );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_2  ,TrCondInst__open_group_chooseRole_or_NoPlay_close_group_TO_open_group_chooseRole_or_NoPlay_close_group_  ) ); //_open_group_chooseRole_or_NoPlay_close_group_TO_open_group_chooseRole_or_NoPlay_close_group_
		

		
		
	ICondition* TrCondInst__open_group_chooseRole_or_NoPlay_close_group_TO0_3_3 = new TrCond__open_group_chooseRole_or_NoPlay_close_group_TO0_3_3;
	_conditions.push_back( TrCondInst__open_group_chooseRole_or_NoPlay_close_group_TO0_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_3  ,TrCondInst__open_group_chooseRole_or_NoPlay_close_group_TO0_3_3  ) ); //_open_group_chooseRole_or_NoPlay_close_group_TO0.3.3
	
}

centerfor::~centerfor() {
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

void centerfor::Start () {
	_statechart->Start();
}

void centerfor::Stop () {
	_statechart->Stop();
}

	