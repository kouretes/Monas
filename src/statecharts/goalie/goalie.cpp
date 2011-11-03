
#include "goalie.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<goalie>::Type temp("goalie");
}
	
goalie::goalie(Narukom* com) {

	
	_statechart = new Statechart ( "Node_player", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );

	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2 = ActivityFactory::Instance()->CreateObject( "Init" );
	_activities.push_back( NodeActivInst_0_2 );			
	BasicState* Node_0_2 = new BasicState( "Node_Init", Node_0, NodeActivInst_0_2 ); //Name:Init
	_states.push_back( Node_0_2 );

	OrState* Node_0_3 = new OrState ( "Node__open_group_decision_one_or_more_times__close_group_", Node_0 );	//Name:_gr_decision_one_or_more_times_
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

	OrState* Node_0_3_2_3_3 = new OrState ( "Node_Scan_one_or_more_times_", Node_0_3_2_3 );	//Name:Scan_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3 );

	StartState* Node_0_3_2_3_3_1 = new StartState ( "Node_0_3_2_3_3_1", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.1
	_states.push_back( Node_0_3_2_3_3_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3_2 = ActivityFactory::Instance()->CreateObject( "Scan" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_2 );			
	BasicState* Node_0_3_2_3_3_2 = new BasicState( "Node_Scan", Node_0_3_2_3_3, NodeActivInst_0_3_2_3_3_2 ); //Name:Scan
	_states.push_back( Node_0_3_2_3_3_2 );

	EndState* Node_0_3_2_3_3_3 = new EndState ( "Node_0_3_2_3_3_3", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.3
	_states.push_back( Node_0_3_2_3_3_3 );

	OrState* Node_0_3_2_3_4 = new OrState ( "Node_Stare_one_or_more_times_", Node_0_3_2_3 );	//Name:Stare_one_or_more_times_
	_states.push_back( Node_0_3_2_3_4 );

	StartState* Node_0_3_2_3_4_1 = new StartState ( "Node_0_3_2_3_4_1", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.1
	_states.push_back( Node_0_3_2_3_4_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_2 = ActivityFactory::Instance()->CreateObject( "Stare" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_2 );			
	BasicState* Node_0_3_2_3_4_2 = new BasicState( "Node_Stare", Node_0_3_2_3_4, NodeActivInst_0_3_2_3_4_2 ); //Name:Stare
	_states.push_back( Node_0_3_2_3_4_2 );

	EndState* Node_0_3_2_3_4_3 = new EndState ( "Node_0_3_2_3_4_3", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.3
	_states.push_back( Node_0_3_2_3_4_3 );

	OrState* Node_0_3_2_3_5 = new OrState ( "Node_ApproachBall_one_or_more_times_", Node_0_3_2_3 );	//Name:ApproachBall_one_or_more_times_
	_states.push_back( Node_0_3_2_3_5 );

	StartState* Node_0_3_2_3_5_1 = new StartState ( "Node_0_3_2_3_5_1", Node_0_3_2_3_5 ); //Name:0.3.2.3.5.1
	_states.push_back( Node_0_3_2_3_5_1 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_5_2 = ActivityFactory::Instance()->CreateObject( "ApproachBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_5_2 );			
	BasicState* Node_0_3_2_3_5_2 = new BasicState( "Node_ApproachBall", Node_0_3_2_3_5, NodeActivInst_0_3_2_3_5_2 ); //Name:ApproachBall
	_states.push_back( Node_0_3_2_3_5_2 );

	EndState* Node_0_3_2_3_5_3 = new EndState ( "Node_0_3_2_3_5_3", Node_0_3_2_3_5 ); //Name:0.3.2.3.5.3
	_states.push_back( Node_0_3_2_3_5_3 );

	OrState* Node_0_3_2_3_6 = new OrState ( "Node_kick", Node_0_3_2_3 );	//Name:kick
	_states.push_back( Node_0_3_2_3_6 );

	StartState* Node_0_3_2_3_6_1 = new StartState ( "Node_0_3_2_3_6_1", Node_0_3_2_3_6 ); //Name:0.3.2.3.6.1
	_states.push_back( Node_0_3_2_3_6_1 );

	ConditionConnector* Node_0_3_2_3_6_2  = new ConditionConnector ( "Node_0_3_2_3_6_2", Node_0_3_2_3_6 ); //Name:0.3.2.3.6.2
	_states.push_back( Node_0_3_2_3_6_2 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_6_3 = ActivityFactory::Instance()->CreateObject( "MyLeftKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_6_3 );			
	BasicState* Node_0_3_2_3_6_3 = new BasicState( "Node_MyLeftKick", Node_0_3_2_3_6, NodeActivInst_0_3_2_3_6_3 ); //Name:MyLeftKick
	_states.push_back( Node_0_3_2_3_6_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_6_4 = ActivityFactory::Instance()->CreateObject( "MyRightKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_6_4 );			
	BasicState* Node_0_3_2_3_6_4 = new BasicState( "Node_MyRightKick", Node_0_3_2_3_6, NodeActivInst_0_3_2_3_6_4 ); //Name:MyRightKick
	_states.push_back( Node_0_3_2_3_6_4 );

	EndState* Node_0_3_2_3_6_5 = new EndState ( "Node_0_3_2_3_6_5", Node_0_3_2_3_6 ); //Name:0.3.2.3.6.5
	_states.push_back( Node_0_3_2_3_6_5 );

	EndState* Node_0_3_2_3_7 = new EndState ( "Node_0_3_2_3_7", Node_0_3_2_3 ); //Name:0.3.2.3.7
	_states.push_back( Node_0_3_2_3_7 );

			
	
	
	

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
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3    ) ); //InitTO_open_group_decision_one_or_more_times__close_group_
		

		
		
	ICondition* TrCondInst__open_group_decision_one_or_more_times__close_group_TO0_4 = new TrCond__open_group_decision_one_or_more_times__close_group_TO0_4;
	_conditions.push_back( TrCondInst__open_group_decision_one_or_more_times__close_group_TO0_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3,Node_0_4  ,TrCondInst__open_group_decision_one_or_more_times__close_group_TO0_4  ) ); //_open_group_decision_one_or_more_times__close_group_TO0.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_1,Node_0_3_2_2    ) ); //0.3.2.1TO0.3.2.2
		

		
		
	ICondition* TrCondInst_0_3_2_2TOplay = new TrCond_0_3_2_2TOplay;
	_conditions.push_back( TrCondInst_0_3_2_2TOplay );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_3  ,TrCondInst_0_3_2_2TOplay  ) ); //0.3.2.2TOplay
		

		
		
	ICondition* TrCondInst_0_3_2_2TONoPlay = new TrCond_0_3_2_2TONoPlay;
	_conditions.push_back( TrCondInst_0_3_2_2TONoPlay );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_4  ,TrCondInst_0_3_2_2TONoPlay  ) ); //0.3.2.2TONoPlay
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4,Node_0_3_2_5    ) ); //NoPlayTO0.3.2.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3,Node_0_3_2_5    ) ); //playTO0.3.2.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_1,Node_0_3_2_3_2    ) ); //0.3.2.3.1TO0.3.2.3.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOScan_one_or_more_times_ = new TrCond_0_3_2_3_2TOScan_one_or_more_times_;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOScan_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_3  ,TrCondInst_0_3_2_3_2TOScan_one_or_more_times_  ) ); //0.3.2.3.2TOScan_one_or_more_times_
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOStare_one_or_more_times_ = new TrCond_0_3_2_3_2TOStare_one_or_more_times_;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOStare_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_4  ,TrCondInst_0_3_2_3_2TOStare_one_or_more_times_  ) ); //0.3.2.3.2TOStare_one_or_more_times_
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOApproachBall_one_or_more_times_ = new TrCond_0_3_2_3_2TOApproachBall_one_or_more_times_;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOApproachBall_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_5  ,TrCondInst_0_3_2_3_2TOApproachBall_one_or_more_times_  ) ); //0.3.2.3.2TOApproachBall_one_or_more_times_
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOkick = new TrCond_0_3_2_3_2TOkick;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOkick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_6  ,TrCondInst_0_3_2_3_2TOkick  ) ); //0.3.2.3.2TOkick
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6,Node_0_3_2_3_7    ) ); //kickTO0.3.2.3.7
		

		
		
	ICondition* TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_7 = new TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_7;
	_conditions.push_back( TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_7 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_5,Node_0_3_2_3_7  ,TrCondInst_ApproachBall_one_or_more_times_TO0_3_2_3_7  ) ); //ApproachBall_one_or_more_times_TO0.3.2.3.7
		

		
		
	ICondition* TrCondInst_Stare_one_or_more_times_TO0_3_2_3_7 = new TrCond_Stare_one_or_more_times_TO0_3_2_3_7;
	_conditions.push_back( TrCondInst_Stare_one_or_more_times_TO0_3_2_3_7 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4,Node_0_3_2_3_7  ,TrCondInst_Stare_one_or_more_times_TO0_3_2_3_7  ) ); //Stare_one_or_more_times_TO0.3.2.3.7
		

		
		
	ICondition* TrCondInst_Scan_one_or_more_times_TO0_3_2_3_7 = new TrCond_Scan_one_or_more_times_TO0_3_2_3_7;
	_conditions.push_back( TrCondInst_Scan_one_or_more_times_TO0_3_2_3_7 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3,Node_0_3_2_3_7  ,TrCondInst_Scan_one_or_more_times_TO0_3_2_3_7  ) ); //Scan_one_or_more_times_TO0.3.2.3.7
		

		
		
	ICondition* TrCondInst_0_3_2_3_3_1TOScan = new TrCond_0_3_2_3_3_1TOScan;
	_conditions.push_back( TrCondInst_0_3_2_3_3_1TOScan );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_1,Node_0_3_2_3_3_2  ,TrCondInst_0_3_2_3_3_1TOScan  ) ); //0.3.2.3.3.1TOScan
		

		
		
	ICondition* TrCondInst_ScanTOScan = new TrCond_ScanTOScan;
	_conditions.push_back( TrCondInst_ScanTOScan );

		
	//IAction* babinos4 = new Mitsos4;

	
		
//	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2,Node_0_3_2_3_3_2  ,TrCondInst_ScanTOScan, babinos4  ) ); //ScanTOScan
		
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2,Node_0_3_2_3_3_2  ,TrCondInst_ScanTOScan ) ); //ScanTOScan
		
		
	ICondition* TrCondInst_ScanTO0_3_2_3_3_3 = new TrCond_ScanTO0_3_2_3_3_3;
	_conditions.push_back( TrCondInst_ScanTO0_3_2_3_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2,Node_0_3_2_3_3_3  ,TrCondInst_ScanTO0_3_2_3_3_3  ) ); //ScanTO0.3.2.3.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_1,Node_0_3_2_3_4_2    ) ); //0.3.2.3.4.1TOStare
		

		
		
	ICondition* TrCondInst_StareTOStare = new TrCond_StareTOStare;
	_conditions.push_back( TrCondInst_StareTOStare );

	//IAction* babinos2 = new Mitsos2;
		

	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_2  ,TrCondInst_StareTOStare, babinos2   ) ); //StareTOStare
		_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_2  ,TrCondInst_StareTOStare   ) ); //StareTOStare

		
		
	ICondition* TrCondInst_StareTO0_3_2_3_4_3 = new TrCond_StareTO0_3_2_3_4_3;
	_conditions.push_back( TrCondInst_StareTO0_3_2_3_4_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_3  ,TrCondInst_StareTO0_3_2_3_4_3  ) ); //StareTO0.3.2.3.4.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_5_1,Node_0_3_2_3_5_2    ) ); //0.3.2.3.5.1TOApproachBall
		

		
		
	ICondition* TrCondInst_ApproachBallTOApproachBall = new TrCond_ApproachBallTOApproachBall;
	_conditions.push_back( TrCondInst_ApproachBallTOApproachBall );

		//	IAction* babinos3 = new Mitsos3;

	
		

	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_5_2,Node_0_3_2_3_5_2  ,TrCondInst_ApproachBallTOApproachBall, babinos3  ) ); //ApproachBallTOApproachBall
		_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_5_2,Node_0_3_2_3_5_2  ,TrCondInst_ApproachBallTOApproachBall  ) ); //ApproachBallTOApproachBall

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_5_2,Node_0_3_2_3_5_3    ) ); //ApproachBallTO0.3.2.3.5.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_6_1,Node_0_3_2_3_6_2    ) ); //0.3.2.3.6.1TO0.3.2.3.6.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_6_2TOMyLeftKick = new TrCond_0_3_2_3_6_2TOMyLeftKick;
	_conditions.push_back( TrCondInst_0_3_2_3_6_2TOMyLeftKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_6_2,Node_0_3_2_3_6_3  ,TrCondInst_0_3_2_3_6_2TOMyLeftKick  ) ); //0.3.2.3.6.2TOMyLeftKick
		

		
		
	ICondition* TrCondInst_0_3_2_3_6_2TOMyRightKick = new TrCond_0_3_2_3_6_2TOMyRightKick;
	_conditions.push_back( TrCondInst_0_3_2_3_6_2TOMyRightKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_6_2,Node_0_3_2_3_6_4  ,TrCondInst_0_3_2_3_6_2TOMyRightKick  ) ); //0.3.2.3.6.2TOMyRightKick
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_4,Node_0_3_2_3_6_5    ) ); //MyRightKickTO0.3.2.3.6.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_6_3,Node_0_3_2_3_6_5    ) ); //MyLeftKickTO0.3.2.3.6.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_1,Node_0_3_2    ) ); //0.3.1TOdecision
		

		
		
	ICondition* TrCondInst_decisionTOdecision = new TrCond_decisionTOdecision;
	_conditions.push_back( TrCondInst_decisionTOdecision );

	//	IAction* babinos = new Mitsos;

	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_2  ,TrCondInst_decisionTOdecision  ) ); //decisionTOdecision
			

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_2  ,TrCondInst_decisionTOdecision  ) ); //decisionTOdecision
		

		
		
	ICondition* TrCondInst_decisionTO0_3_3 = new TrCond_decisionTO0_3_3;
	_conditions.push_back( TrCondInst_decisionTO0_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_3  ,TrCondInst_decisionTO0_3_3  ) ); //decisionTO0.3.3
	
}

goalie::~goalie() {
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

void goalie::Start () {
	_statechart->Start();
}

void goalie::Stop () {
	_statechart->Stop();
}

	
