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

	OrState* Node_0_3_2_3_3_2 = new OrState ( "Node_findBall_one_or_more_times_", Node_0_3_2_3_3 );	//Name:findBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_2 );

	StartState* Node_0_3_2_3_3_2_1 = new StartState ( "Node_0_3_2_3_3_2_1", Node_0_3_2_3_3_2 ); //Name:0.3.2.3.3.2.1
	_states.push_back( Node_0_3_2_3_3_2_1 );

	OrState* Node_0_3_2_3_3_2_2 = new OrState ( "Node_findBall", Node_0_3_2_3_3_2 );	//Name:findBall
	_states.push_back( Node_0_3_2_3_3_2_2 );

	StartState* Node_0_3_2_3_3_2_2_1 = new StartState ( "Node_0_3_2_3_3_2_2_1", Node_0_3_2_3_3_2_2 ); //Name:0.3.2.3.3.2.2.1
	_states.push_back( Node_0_3_2_3_3_2_2_1 );

	OrState* Node_0_3_2_3_3_2_2_2 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_3_2_3_3_2_2 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_2_2_2 );

	StartState* Node_0_3_2_3_3_2_2_2_1 = new StartState ( "Node_0_3_2_3_3_2_2_2_1", Node_0_3_2_3_3_2_2_2 ); //Name:0.3.2.3.3.2.2.2.1
	_states.push_back( Node_0_3_2_3_3_2_2_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_2_2_2_2 = ActivityFactory::Instance()->CreateObject( "Scan" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_2_2_2_2 );			
	BasicState* Node_0_3_2_3_3_2_2_2_2 = new BasicState( "Node_Scan", Node_0_3_2_3_3_2_2_2, NodeActivInst_0_3_2_3_3_2_2_2_2 ); //Name:Scan
	_states.push_back( Node_0_3_2_3_3_2_2_2_2 );

	EndState* Node_0_3_2_3_3_2_2_2_3 = new EndState ( "Node_0_3_2_3_3_2_2_2_3", Node_0_3_2_3_3_2_2_2 ); //Name:0.3.2.3.3.2.2.2.3
	_states.push_back( Node_0_3_2_3_3_2_2_2_3 );

	OrState* Node_0_3_2_3_3_2_2_3 = new OrState ( "Node_ApproachBall_one_or_more_times_", Node_0_3_2_3_3_2_2 );	//Name:ApproachBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_2_2_3 );

	StartState* Node_0_3_2_3_3_2_2_3_1 = new StartState ( "Node_0_3_2_3_3_2_2_3_1", Node_0_3_2_3_3_2_2_3 ); //Name:0.3.2.3.3.2.2.3.1
	_states.push_back( Node_0_3_2_3_3_2_2_3_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_2_2_3_2 = ActivityFactory::Instance()->CreateObject( "ApproachBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_2_2_3_2 );			
	BasicState* Node_0_3_2_3_3_2_2_3_2 = new BasicState( "Node_ApproachBall", Node_0_3_2_3_3_2_2_3, NodeActivInst_0_3_2_3_3_2_2_3_2 ); //Name:ApproachBall
	_states.push_back( Node_0_3_2_3_3_2_2_3_2 );

	EndState* Node_0_3_2_3_3_2_2_3_3 = new EndState ( "Node_0_3_2_3_3_2_2_3_3", Node_0_3_2_3_3_2_2_3 ); //Name:0.3.2.3.3.2.2.3.3
	_states.push_back( Node_0_3_2_3_3_2_2_3_3 );

	EndState* Node_0_3_2_3_3_2_2_4 = new EndState ( "Node_0_3_2_3_3_2_2_4", Node_0_3_2_3_3_2_2 ); //Name:0.3.2.3.3.2.2.4
	_states.push_back( Node_0_3_2_3_3_2_2_4 );

	EndState* Node_0_3_2_3_3_2_3 = new EndState ( "Node_0_3_2_3_3_2_3", Node_0_3_2_3_3_2 ); //Name:0.3.2.3.3.2.3
	_states.push_back( Node_0_3_2_3_3_2_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_3 = ActivityFactory::Instance()->CreateObject( "MyLeftSideKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_3 );			
	BasicState* Node_0_3_2_3_3_3 = new BasicState( "Node_MyLeftSideKick", Node_0_3_2_3_3, NodeActivInst_0_3_2_3_3_3 ); //Name:MyLeftSideKick
	_states.push_back( Node_0_3_2_3_3_3 );

	OrState* Node_0_3_2_3_3_4 = new OrState ( "Node_FollowTheBall_one_or_more_times_", Node_0_3_2_3_3 );	//Name:FollowTheBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3_4 );

	StartState* Node_0_3_2_3_3_4_1 = new StartState ( "Node_0_3_2_3_3_4_1", Node_0_3_2_3_3_4 ); //Name:0.3.2.3.3.4.1
	_states.push_back( Node_0_3_2_3_3_4_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_4_2 = ActivityFactory::Instance()->CreateObject( "FollowTheBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_4_2 );			
	BasicState* Node_0_3_2_3_3_4_2 = new BasicState( "Node_FollowTheBall", Node_0_3_2_3_3_4, NodeActivInst_0_3_2_3_3_4_2 ); //Name:FollowTheBall
	_states.push_back( Node_0_3_2_3_3_4_2 );

	EndState* Node_0_3_2_3_3_4_3 = new EndState ( "Node_0_3_2_3_3_4_3", Node_0_3_2_3_3_4 ); //Name:0.3.2.3.3.4.3
	_states.push_back( Node_0_3_2_3_3_4_3 );

	EndState* Node_0_3_2_3_3_5 = new EndState ( "Node_0_3_2_3_3_5", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.5
	_states.push_back( Node_0_3_2_3_3_5 );

	OrState* Node_0_3_2_3_4 = new OrState ( "Node_centerFor", Node_0_3_2_3 );	//Name:centerFor
	_states.push_back( Node_0_3_2_3_4 );

	StartState* Node_0_3_2_3_4_1 = new StartState ( "Node_0_3_2_3_4_1", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.1
	_states.push_back( Node_0_3_2_3_4_1 );

	OrState* Node_0_3_2_3_4_2 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_3_2_3_4 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4_2 );

	StartState* Node_0_3_2_3_4_2_1 = new StartState ( "Node_0_3_2_3_4_2_1", Node_0_3_2_3_4_2 ); //Name:0.3.2.3.4.2.1
	_states.push_back( Node_0_3_2_3_4_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_2_2 = ActivityFactory::Instance()->CreateObject( "Scan" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_2_2 );			
	BasicState* Node_0_3_2_3_4_2_2 = new BasicState( "Node_Scan", Node_0_3_2_3_4_2, NodeActivInst_0_3_2_3_4_2_2 ); //Name:Scan
	_states.push_back( Node_0_3_2_3_4_2_2 );

	EndState* Node_0_3_2_3_4_2_3 = new EndState ( "Node_0_3_2_3_4_2_3", Node_0_3_2_3_4_2 ); //Name:0.3.2.3.4.2.3
	_states.push_back( Node_0_3_2_3_4_2_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_3 = ActivityFactory::Instance()->CreateObject( "WalktoPost" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_3 );			
	BasicState* Node_0_3_2_3_4_3 = new BasicState( "Node_WalktoPost", Node_0_3_2_3_4, NodeActivInst_0_3_2_3_4_3 ); //Name:WalktoPost
	_states.push_back( Node_0_3_2_3_4_3 );

	OrState* Node_0_3_2_3_4_4 = new OrState ( "Node_findBall_one_or_more_times_", Node_0_3_2_3_4 );	//Name:findBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4_4 );

	StartState* Node_0_3_2_3_4_4_1 = new StartState ( "Node_0_3_2_3_4_4_1", Node_0_3_2_3_4_4 ); //Name:0.3.2.3.4.4.1
	_states.push_back( Node_0_3_2_3_4_4_1 );

	OrState* Node_0_3_2_3_4_4_2 = new OrState ( "Node_findBall", Node_0_3_2_3_4_4 );	//Name:findBall
	_states.push_back( Node_0_3_2_3_4_4_2 );

	StartState* Node_0_3_2_3_4_4_2_1 = new StartState ( "Node_0_3_2_3_4_4_2_1", Node_0_3_2_3_4_4_2 ); //Name:0.3.2.3.4.4.2.1
	_states.push_back( Node_0_3_2_3_4_4_2_1 );

	OrState* Node_0_3_2_3_4_4_2_2 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_3_2_3_4_4_2 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4_4_2_2 );

	StartState* Node_0_3_2_3_4_4_2_2_1 = new StartState ( "Node_0_3_2_3_4_4_2_2_1", Node_0_3_2_3_4_4_2_2 ); //Name:0.3.2.3.4.4.2.2.1
	_states.push_back( Node_0_3_2_3_4_4_2_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_4_2_2_2 = ActivityFactory::Instance()->CreateObject( "Scan" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_4_2_2_2 );			
	BasicState* Node_0_3_2_3_4_4_2_2_2 = new BasicState( "Node_Scan", Node_0_3_2_3_4_4_2_2, NodeActivInst_0_3_2_3_4_4_2_2_2 ); //Name:Scan
	_states.push_back( Node_0_3_2_3_4_4_2_2_2 );

	EndState* Node_0_3_2_3_4_4_2_2_3 = new EndState ( "Node_0_3_2_3_4_4_2_2_3", Node_0_3_2_3_4_4_2_2 ); //Name:0.3.2.3.4.4.2.2.3
	_states.push_back( Node_0_3_2_3_4_4_2_2_3 );

	OrState* Node_0_3_2_3_4_4_2_3 = new OrState ( "Node_ApproachBall_one_or_more_times_", Node_0_3_2_3_4_4_2 );	//Name:ApproachBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4_4_2_3 );

	StartState* Node_0_3_2_3_4_4_2_3_1 = new StartState ( "Node_0_3_2_3_4_4_2_3_1", Node_0_3_2_3_4_4_2_3 ); //Name:0.3.2.3.4.4.2.3.1
	_states.push_back( Node_0_3_2_3_4_4_2_3_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_4_2_3_2 = ActivityFactory::Instance()->CreateObject( "ApproachBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_4_2_3_2 );			
	BasicState* Node_0_3_2_3_4_4_2_3_2 = new BasicState( "Node_ApproachBall", Node_0_3_2_3_4_4_2_3, NodeActivInst_0_3_2_3_4_4_2_3_2 ); //Name:ApproachBall
	_states.push_back( Node_0_3_2_3_4_4_2_3_2 );

	EndState* Node_0_3_2_3_4_4_2_3_3 = new EndState ( "Node_0_3_2_3_4_4_2_3_3", Node_0_3_2_3_4_4_2_3 ); //Name:0.3.2.3.4.4.2.3.3
	_states.push_back( Node_0_3_2_3_4_4_2_3_3 );

	EndState* Node_0_3_2_3_4_4_2_4 = new EndState ( "Node_0_3_2_3_4_4_2_4", Node_0_3_2_3_4_4_2 ); //Name:0.3.2.3.4.4.2.4
	_states.push_back( Node_0_3_2_3_4_4_2_4 );

	EndState* Node_0_3_2_3_4_4_3 = new EndState ( "Node_0_3_2_3_4_4_3", Node_0_3_2_3_4_4 ); //Name:0.3.2.3.4.4.3
	_states.push_back( Node_0_3_2_3_4_4_3 );

	OrState* Node_0_3_2_3_4_5 = new OrState ( "Node_kick", Node_0_3_2_3_4 );	//Name:kick
	_states.push_back( Node_0_3_2_3_4_5 );

	StartState* Node_0_3_2_3_4_5_1 = new StartState ( "Node_0_3_2_3_4_5_1", Node_0_3_2_3_4_5 ); //Name:0.3.2.3.4.5.1
	_states.push_back( Node_0_3_2_3_4_5_1 );

	ConditionConnector* Node_0_3_2_3_4_5_2  = new ConditionConnector ( "Node_0_3_2_3_4_5_2", Node_0_3_2_3_4_5 ); //Name:0.3.2.3.4.5.2
	_states.push_back( Node_0_3_2_3_4_5_2 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_5_3 = ActivityFactory::Instance()->CreateObject( "MyLeftKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_5_3 );			
	BasicState* Node_0_3_2_3_4_5_3 = new BasicState( "Node_MyLeftKick", Node_0_3_2_3_4_5, NodeActivInst_0_3_2_3_4_5_3 ); //Name:MyLeftKick
	_states.push_back( Node_0_3_2_3_4_5_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_5_4 = ActivityFactory::Instance()->CreateObject( "MyRightKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_5_4 );			
	BasicState* Node_0_3_2_3_4_5_4 = new BasicState( "Node_MyRightKick", Node_0_3_2_3_4_5, NodeActivInst_0_3_2_3_4_5_4 ); //Name:MyRightKick
	_states.push_back( Node_0_3_2_3_4_5_4 );

	EndState* Node_0_3_2_3_4_5_5 = new EndState ( "Node_0_3_2_3_4_5_5", Node_0_3_2_3_4_5 ); //Name:0.3.2.3.4.5.5
	_states.push_back( Node_0_3_2_3_4_5_5 );

	EndState* Node_0_3_2_3_4_6 = new EndState ( "Node_0_3_2_3_4_6", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.6
	_states.push_back( Node_0_3_2_3_4_6 );

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
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_1,Node_0_3_2_3_3_2    ) ); //0.3.2.3.3.1TOfindBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_findBall_one_or_more_times_TOMyLeftSideKick = new TrCond_findBall_one_or_more_times_TOMyLeftSideKick;
	_conditions.push_back( TrCondInst_findBall_one_or_more_times_TOMyLeftSideKick );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2,Node_0_3_2_3_3_3  ,TrCondInst_findBall_one_or_more_times_TOMyLeftSideKick  ) ); //findBall_one_or_more_times_TOMyLeftSideKick
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_3,Node_0_3_2_3_3_4    ) ); //MyLeftSideKickTOFollowTheBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_5 = new TrCond_FollowTheBall_one_or_more_times_TO0_3_2_3_3_5;
	_conditions.push_back( TrCondInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_5 );

		
		
	IAction* TrActionInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_5 = new TrAction_FollowTheBall_one_or_more_times_TO0_3_2_3_3_5;
	_actions.push_back( TrActionInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_5 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4,Node_0_3_2_3_3_5  ,TrCondInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_5 ,TrActionInst_FollowTheBall_one_or_more_times_TO0_3_2_3_3_5 ) ); //FollowTheBall_one_or_more_times_TO0.3.2.3.3.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_1,Node_0_3_2_3_3_2_2_2    ) ); //0.3.2.3.3.2.2.1TOScan_one_or_more_times_
		

		
		
	ICondition* TrCondInst_Scan_one_or_more_times_TOApproachBall_one_or_more_times_ = new TrCond_Scan_one_or_more_times_TOApproachBall_one_or_more_times_;
	_conditions.push_back( TrCondInst_Scan_one_or_more_times_TOApproachBall_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_2,Node_0_3_2_3_3_2_2_3  ,TrCondInst_Scan_one_or_more_times_TOApproachBall_one_or_more_times_  ) ); //Scan_one_or_more_times_TOApproachBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_3_2_2_4 = new TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_3_2_2_4;
	_conditions.push_back( TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_3_2_2_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_3,Node_0_3_2_3_3_2_2_4  ,TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_3_2_2_4  ) ); //ApproachBall_one_or_more_times_TO0.3.2.3.3.2.2.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_2_1,Node_0_3_2_3_3_2_2_2_2    ) ); //0.3.2.3.3.2.2.2.1TOScan
		

		
		
	ICondition* TrCondInst_ScanTOScan = new TrCond_ScanTOScan;
	_conditions.push_back( TrCondInst_ScanTOScan );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_2_2,Node_0_3_2_3_3_2_2_2_2  ,TrCondInst_ScanTOScan  ) ); //ScanTOScan
		

		
		
	ICondition* TrCondInst_ScanTO0_3_2_3_3_2_2_2_3 = new TrCond_ScanTO0_3_2_3_3_2_2_2_3;
	_conditions.push_back( TrCondInst_ScanTO0_3_2_3_3_2_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_2_2,Node_0_3_2_3_3_2_2_2_3  ,TrCondInst_ScanTO0_3_2_3_3_2_2_2_3  ) ); //ScanTO0.3.2.3.3.2.2.2.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_3_1,Node_0_3_2_3_3_2_2_3_2    ) ); //0.3.2.3.3.2.2.3.1TOApproachBall
		

		
		
	ICondition* TrCondInst_ApproachBallTOApproachBall = new TrCond_ApproachBallTOApproachBall;
	_conditions.push_back( TrCondInst_ApproachBallTOApproachBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_3_2,Node_0_3_2_3_3_2_2_3_2  ,TrCondInst_ApproachBallTOApproachBall  ) ); //ApproachBallTOApproachBall
		

		
		
	ICondition* TrCondInst_ApproachBallTO0_3_2_3_3_2_2_3_3 = new TrCond_ApproachBallTO0_3_2_3_3_2_2_3_3;
	_conditions.push_back( TrCondInst_ApproachBallTO0_3_2_3_3_2_2_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2_3_2,Node_0_3_2_3_3_2_2_3_3  ,TrCondInst_ApproachBallTO0_3_2_3_3_2_2_3_3  ) ); //ApproachBallTO0.3.2.3.3.2.2.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_1,Node_0_3_2_3_3_2_2    ) ); //0.3.2.3.3.2.1TOfindBall
		

		
		
	ICondition* TrCondInst_findBallTOfindBall = new TrCond_findBallTOfindBall;
	_conditions.push_back( TrCondInst_findBallTOfindBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2,Node_0_3_2_3_3_2_2  ,TrCondInst_findBallTOfindBall  ) ); //findBallTOfindBall
		

		
		
	ICondition* TrCondInst_findBallTO0_3_2_3_3_2_3 = new TrCond_findBallTO0_3_2_3_3_2_3;
	_conditions.push_back( TrCondInst_findBallTO0_3_2_3_3_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2_2,Node_0_3_2_3_3_2_3  ,TrCondInst_findBallTO0_3_2_3_3_2_3  ) ); //findBallTO0.3.2.3.3.2.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4_1,Node_0_3_2_3_3_4_2    ) ); //0.3.2.3.3.4.1TOFollowTheBall
		

		
		
	ICondition* TrCondInst_FollowTheBallTOFollowTheBall = new TrCond_FollowTheBallTOFollowTheBall;
	_conditions.push_back( TrCondInst_FollowTheBallTOFollowTheBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4_2,Node_0_3_2_3_3_4_2  ,TrCondInst_FollowTheBallTOFollowTheBall  ) ); //FollowTheBallTOFollowTheBall
		

		
		
	ICondition* TrCondInst_FollowTheBallTO0_3_2_3_3_4_3 = new TrCond_FollowTheBallTO0_3_2_3_3_4_3;
	_conditions.push_back( TrCondInst_FollowTheBallTO0_3_2_3_3_4_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_4_2,Node_0_3_2_3_3_4_3  ,TrCondInst_FollowTheBallTO0_3_2_3_3_4_3  ) ); //FollowTheBallTO0.3.2.3.3.4.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_1,Node_0_3_2_3_4_2    ) ); //0.3.2.3.4.1TOScan_one_or_more_times_
		

		
		
	ICondition* TrCondInst_Scan_one_or_more_times_TOWalktoPost = new TrCond_Scan_one_or_more_times_TOWalktoPost;
	_conditions.push_back( TrCondInst_Scan_one_or_more_times_TOWalktoPost );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_3  ,TrCondInst_Scan_one_or_more_times_TOWalktoPost  ) ); //Scan_one_or_more_times_TOWalktoPost
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_3,Node_0_3_2_3_4_4    ) ); //WalktoPostTOfindBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_findBall_one_or_more_times_TOkick = new TrCond_findBall_one_or_more_times_TOkick;
	_conditions.push_back( TrCondInst_findBall_one_or_more_times_TOkick );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4,Node_0_3_2_3_4_5  ,TrCondInst_findBall_one_or_more_times_TOkick  ) ); //findBall_one_or_more_times_TOkick
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_5,Node_0_3_2_3_4_6    ) ); //kickTO0.3.2.3.4.6
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2_1,Node_0_3_2_3_4_2_2    ) ); //0.3.2.3.4.2.1TOScan
		

		
		
	//ICondition* TrCondInst_ScanTOScan = new TrCond_ScanTOScan;
	_conditions.push_back( TrCondInst_ScanTOScan );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2_2,Node_0_3_2_3_4_2_2  ,TrCondInst_ScanTOScan  ) ); //ScanTOScan
		

		
		
	ICondition* TrCondInst_ScanTO0_3_2_3_4_2_3 = new TrCond_ScanTO0_3_2_3_4_2_3;
	_conditions.push_back( TrCondInst_ScanTO0_3_2_3_4_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2_2,Node_0_3_2_3_4_2_3  ,TrCondInst_ScanTO0_3_2_3_4_2_3  ) ); //ScanTO0.3.2.3.4.2.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_1,Node_0_3_2_3_4_4_2_2    ) ); //0.3.2.3.4.4.2.1TOScan_one_or_more_times_
		

		
		
	//ICondition* TrCondInst_Scan_one_or_more_times_TOApproachBall_one_or_more_times_ = new TrCond_Scan_one_or_more_times_TOApproachBall_one_or_more_times_;
	_conditions.push_back( TrCondInst_Scan_one_or_more_times_TOApproachBall_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_2,Node_0_3_2_3_4_4_2_3  ,TrCondInst_Scan_one_or_more_times_TOApproachBall_one_or_more_times_  ) ); //Scan_one_or_more_times_TOApproachBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_4_4_2_4 = new TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_4_4_2_4;
	_conditions.push_back( TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_4_4_2_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_3,Node_0_3_2_3_4_4_2_4  ,TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_4_4_2_4  ) ); //ApproachBall_one_or_more_times_TO0.3.2.3.4.4.2.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_2_1,Node_0_3_2_3_4_4_2_2_2    ) ); //0.3.2.3.4.4.2.2.1TOScan
		

		
		
	//ICondition* TrCondInst_ScanTOScan = new TrCond_ScanTOScan;
	_conditions.push_back( TrCondInst_ScanTOScan );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_2_2,Node_0_3_2_3_4_4_2_2_2  ,TrCondInst_ScanTOScan  ) ); //ScanTOScan
		

		
		
	ICondition* TrCondInst_ScanTO0_3_2_3_4_4_2_2_3 = new TrCond_ScanTO0_3_2_3_4_4_2_2_3;
	_conditions.push_back( TrCondInst_ScanTO0_3_2_3_4_4_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_2_2,Node_0_3_2_3_4_4_2_2_3  ,TrCondInst_ScanTO0_3_2_3_4_4_2_2_3  ) ); //ScanTO0.3.2.3.4.4.2.2.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_3_1,Node_0_3_2_3_4_4_2_3_2    ) ); //0.3.2.3.4.4.2.3.1TOApproachBall
		

		
		
	//ICondition* TrCondInst_ApproachBallTOApproachBall = new TrCond_ApproachBallTOApproachBall;
	_conditions.push_back( TrCondInst_ApproachBallTOApproachBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_3_2,Node_0_3_2_3_4_4_2_3_2  ,TrCondInst_ApproachBallTOApproachBall  ) ); //ApproachBallTOApproachBall
		

		
		
	ICondition* TrCondInst_ApproachBallTO0_3_2_3_4_4_2_3_3 = new TrCond_ApproachBallTO0_3_2_3_4_4_2_3_3;
	_conditions.push_back( TrCondInst_ApproachBallTO0_3_2_3_4_4_2_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2_3_2,Node_0_3_2_3_4_4_2_3_3  ,TrCondInst_ApproachBallTO0_3_2_3_4_4_2_3_3  ) ); //ApproachBallTO0.3.2.3.4.4.2.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_1,Node_0_3_2_3_4_4_2    ) ); //0.3.2.3.4.4.1TOfindBall
		

		
		
	//ICondition* TrCondInst_findBallTOfindBall = new TrCond_findBallTOfindBall;
	_conditions.push_back( TrCondInst_findBallTOfindBall );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2,Node_0_3_2_3_4_4_2  ,TrCondInst_findBallTOfindBall  ) ); //findBallTOfindBall
		

		
		
	ICondition* TrCondInst_findBallTO0_3_2_3_4_4_3 = new TrCond_findBallTO0_3_2_3_4_4_3;
	_conditions.push_back( TrCondInst_findBallTO0_3_2_3_4_4_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_2,Node_0_3_2_3_4_4_3  ,TrCondInst_findBallTO0_3_2_3_4_4_3  ) ); //findBallTO0.3.2.3.4.4.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_4_5_1,Node_0_3_2_3_4_5_2    ) ); //0.3.2.3.4.5.1TO0.3.2.3.4.5.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_5_2TOMyLeftKick = new TrCond_0_3_2_3_4_5_2TOMyLeftKick;
	_conditions.push_back( TrCondInst_0_3_2_3_4_5_2TOMyLeftKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_5_2,Node_0_3_2_3_4_5_3  ,TrCondInst_0_3_2_3_4_5_2TOMyLeftKick  ) ); //0.3.2.3.4.5.2TOMyLeftKick
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_5_2TOMyRightKick = new TrCond_0_3_2_3_4_5_2TOMyRightKick;
	_conditions.push_back( TrCondInst_0_3_2_3_4_5_2TOMyRightKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_5_2,Node_0_3_2_3_4_5_4  ,TrCondInst_0_3_2_3_4_5_2TOMyRightKick  ) ); //0.3.2.3.4.5.2TOMyRightKick
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_5_4,Node_0_3_2_3_4_5_5    ) ); //MyRightKickTO0.3.2.3.4.5.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_5_3,Node_0_3_2_3_4_5_5    ) ); //MyLeftKickTO0.3.2.3.4.5.5
		

		

		

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

	
