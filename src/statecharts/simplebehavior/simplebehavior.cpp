#include "simplebehavior.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<simplebehavior>::Type temp("simplebehavior");
}
	
simplebehavior::simplebehavior(Narukom* com) {

	
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

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_3 = ActivityFactory::Instance()->CreateObject( "AproachBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_3 );			
	BasicState* Node_0_3_2_3_3 = new BasicState( "Node_AproachBall", Node_0_3_2_3, NodeActivInst_0_3_2_3_3 ); //Name:AproachBall
	_states.push_back( Node_0_3_2_3_3 );

	OrState* Node_0_3_2_3_4 = new OrState ( "Node_kickBall", Node_0_3_2_3 );	//Name:kickBall
	_states.push_back( Node_0_3_2_3_4 );

	StartState* Node_0_3_2_3_4_1 = new StartState ( "Node_0_3_2_3_4_1", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.1
	_states.push_back( Node_0_3_2_3_4_1 );

	ConditionConnector* Node_0_3_2_3_4_2  = new ConditionConnector ( "Node_0_3_2_3_4_2", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.2
	_states.push_back( Node_0_3_2_3_4_2 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_3 = ActivityFactory::Instance()->CreateObject( "KickOff" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_3 );			
	BasicState* Node_0_3_2_3_4_3 = new BasicState( "Node_KickOff", Node_0_3_2_3_4, NodeActivInst_0_3_2_3_4_3 ); //Name:KickOff
	_states.push_back( Node_0_3_2_3_4_3 );

	OrState* Node_0_3_2_3_4_4 = new OrState ( "Node_kick", Node_0_3_2_3_4 );	//Name:kick
	_states.push_back( Node_0_3_2_3_4_4 );

	StartState* Node_0_3_2_3_4_4_1 = new StartState ( "Node_0_3_2_3_4_4_1", Node_0_3_2_3_4_4 ); //Name:0.3.2.3.4.4.1
	_states.push_back( Node_0_3_2_3_4_4_1 );

	ConditionConnector* Node_0_3_2_3_4_4_2  = new ConditionConnector ( "Node_0_3_2_3_4_4_2", Node_0_3_2_3_4_4 ); //Name:0.3.2.3.4.4.2
	_states.push_back( Node_0_3_2_3_4_4_2 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_4_3 = ActivityFactory::Instance()->CreateObject( "MyLeftKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_4_3 );			
	BasicState* Node_0_3_2_3_4_4_3 = new BasicState( "Node_MyLeftKick", Node_0_3_2_3_4_4, NodeActivInst_0_3_2_3_4_4_3 ); //Name:MyLeftKick
	_states.push_back( Node_0_3_2_3_4_4_3 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_4_4_4 = ActivityFactory::Instance()->CreateObject( "MyRightKick" );
	_activities.push_back( NodeActivInst_0_3_2_3_4_4_4 );			
	BasicState* Node_0_3_2_3_4_4_4 = new BasicState( "Node_MyRightKick", Node_0_3_2_3_4_4, NodeActivInst_0_3_2_3_4_4_4 ); //Name:MyRightKick
	_states.push_back( Node_0_3_2_3_4_4_4 );

	EndState* Node_0_3_2_3_4_4_5 = new EndState ( "Node_0_3_2_3_4_4_5", Node_0_3_2_3_4_4 ); //Name:0.3.2.3.4.4.5
	_states.push_back( Node_0_3_2_3_4_4_5 );

	EndState* Node_0_3_2_3_4_5 = new EndState ( "Node_0_3_2_3_4_5", Node_0_3_2_3_4 ); //Name:0.3.2.3.4.5
	_states.push_back( Node_0_3_2_3_4_5 );

			
	
	
	

	IActivity* NodeActivInst_0_3_2_3_5 = ActivityFactory::Instance()->CreateObject( "ScanForBall" );
	_activities.push_back( NodeActivInst_0_3_2_3_5 );			
	BasicState* Node_0_3_2_3_5 = new BasicState( "Node_ScanForBall", Node_0_3_2_3, NodeActivInst_0_3_2_3_5 ); //Name:ScanForBall
	_states.push_back( Node_0_3_2_3_5 );

	EndState* Node_0_3_2_3_6 = new EndState ( "Node_0_3_2_3_6", Node_0_3_2_3 ); //Name:0.3.2.3.6
	_states.push_back( Node_0_3_2_3_6 );

			
	
	
	

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
	
	
		

		
		
	ICondition* TrCondInst_0_1TOInit = new TrCond_0_1TOInit;
	_conditions.push_back( TrCondInst_0_1TOInit );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2  ,TrCondInst_0_1TOInit  ) ); //0.1TOInit
		

		
		
	ICondition* TrCondInst_InitTOdecision_one_or_more_times_ = new TrCond_InitTOdecision_one_or_more_times_;
	_conditions.push_back( TrCondInst_InitTOdecision_one_or_more_times_ );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3  ,TrCondInst_InitTOdecision_one_or_more_times_  ) ); //InitTOdecision_one_or_more_times_
		

		
		
	ICondition* TrCondInst_decision_one_or_more_times_TO0_4 = new TrCond_decision_one_or_more_times_TO0_4;
	_conditions.push_back( TrCondInst_decision_one_or_more_times_TO0_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3,Node_0_4  ,TrCondInst_decision_one_or_more_times_TO0_4  ) ); //decision_one_or_more_times_TO0.4
		

		
		
	ICondition* TrCondInst_0_3_2_1TO0_3_2_2 = new TrCond_0_3_2_1TO0_3_2_2;
	_conditions.push_back( TrCondInst_0_3_2_1TO0_3_2_2 );

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_1,Node_0_3_2_2  ,TrCondInst_0_3_2_1TO0_3_2_2  ) ); //0.3.2.1TO0.3.2.2
		

		
		
	ICondition* TrCondInst_0_3_2_2TOplay = new TrCond_0_3_2_2TOplay;
	_conditions.push_back( TrCondInst_0_3_2_2TOplay );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_3  ,TrCondInst_0_3_2_2TOplay  ) ); //0.3.2.2TOplay
		

		
		
	ICondition* TrCondInst_0_3_2_2TONoPlay = new TrCond_0_3_2_2TONoPlay;
	_conditions.push_back( TrCondInst_0_3_2_2TONoPlay );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_4  ,TrCondInst_0_3_2_2TONoPlay  ) ); //0.3.2.2TONoPlay
		

		
		
	ICondition* TrCondInst_NoPlayTO0_3_2_5 = new TrCond_NoPlayTO0_3_2_5;
	_conditions.push_back( TrCondInst_NoPlayTO0_3_2_5 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4,Node_0_3_2_5  ,TrCondInst_NoPlayTO0_3_2_5  ) ); //NoPlayTO0.3.2.5
		

		
		
	ICondition* TrCondInst_playTO0_3_2_5 = new TrCond_playTO0_3_2_5;
	_conditions.push_back( TrCondInst_playTO0_3_2_5 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3,Node_0_3_2_5  ,TrCondInst_playTO0_3_2_5  ) ); //playTO0.3.2.5
		

		
		
	ICondition* TrCondInst_0_3_2_3_1TO0_3_2_3_2 = new TrCond_0_3_2_3_1TO0_3_2_3_2;
	_conditions.push_back( TrCondInst_0_3_2_3_1TO0_3_2_3_2 );

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_1,Node_0_3_2_3_2  ,TrCondInst_0_3_2_3_1TO0_3_2_3_2  ) ); //0.3.2.3.1TO0.3.2.3.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOAproachBall = new TrCond_0_3_2_3_2TOAproachBall;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOAproachBall );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_3  ,TrCondInst_0_3_2_3_2TOAproachBall  ) ); //0.3.2.3.2TOAproachBall
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOkickBall = new TrCond_0_3_2_3_2TOkickBall;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOkickBall );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_4  ,TrCondInst_0_3_2_3_2TOkickBall  ) ); //0.3.2.3.2TOkickBall
		

		
		
	ICondition* TrCondInst_0_3_2_3_2TOScanForBall = new TrCond_0_3_2_3_2TOScanForBall;
	_conditions.push_back( TrCondInst_0_3_2_3_2TOScanForBall );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_2,Node_0_3_2_3_5  ,TrCondInst_0_3_2_3_2TOScanForBall  ) ); //0.3.2.3.2TOScanForBall
		

		
		
	ICondition* TrCondInst_ScanForBallTO0_3_2_3_6 = new TrCond_ScanForBallTO0_3_2_3_6;
	_conditions.push_back( TrCondInst_ScanForBallTO0_3_2_3_6 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_5,Node_0_3_2_3_6  ,TrCondInst_ScanForBallTO0_3_2_3_6  ) ); //ScanForBallTO0.3.2.3.6
		

		
		
	ICondition* TrCondInst_kickBallTO0_3_2_3_6 = new TrCond_kickBallTO0_3_2_3_6;
	_conditions.push_back( TrCondInst_kickBallTO0_3_2_3_6 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4,Node_0_3_2_3_6  ,TrCondInst_kickBallTO0_3_2_3_6  ) ); //kickBallTO0.3.2.3.6
		

		
		
	ICondition* TrCondInst_AproachBallTO0_3_2_3_6 = new TrCond_AproachBallTO0_3_2_3_6;
	_conditions.push_back( TrCondInst_AproachBallTO0_3_2_3_6 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3,Node_0_3_2_3_6  ,TrCondInst_AproachBallTO0_3_2_3_6  ) ); //AproachBallTO0.3.2.3.6
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_1TO0_3_2_3_4_2 = new TrCond_0_3_2_3_4_1TO0_3_2_3_4_2;
	_conditions.push_back( TrCondInst_0_3_2_3_4_1TO0_3_2_3_4_2 );

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_4_1,Node_0_3_2_3_4_2  ,TrCondInst_0_3_2_3_4_1TO0_3_2_3_4_2  ) ); //0.3.2.3.4.1TO0.3.2.3.4.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_2TOKickOff = new TrCond_0_3_2_3_4_2TOKickOff;
	_conditions.push_back( TrCondInst_0_3_2_3_4_2TOKickOff );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_3  ,TrCondInst_0_3_2_3_4_2TOKickOff  ) ); //0.3.2.3.4.2TOKickOff
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_2TOkick = new TrCond_0_3_2_3_4_2TOkick;
	_conditions.push_back( TrCondInst_0_3_2_3_4_2TOkick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_2,Node_0_3_2_3_4_4  ,TrCondInst_0_3_2_3_4_2TOkick  ) ); //0.3.2.3.4.2TOkick
		

		
		
	ICondition* TrCondInst_kickTO0_3_2_3_4_5 = new TrCond_kickTO0_3_2_3_4_5;
	_conditions.push_back( TrCondInst_kickTO0_3_2_3_4_5 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4,Node_0_3_2_3_4_5  ,TrCondInst_kickTO0_3_2_3_4_5  ) ); //kickTO0.3.2.3.4.5
		

		
		
	ICondition* TrCondInst_KickOffTO0_3_2_3_4_5 = new TrCond_KickOffTO0_3_2_3_4_5;
	_conditions.push_back( TrCondInst_KickOffTO0_3_2_3_4_5 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_3,Node_0_3_2_3_4_5  ,TrCondInst_KickOffTO0_3_2_3_4_5  ) ); //KickOffTO0.3.2.3.4.5
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_4_1TO0_3_2_3_4_4_2 = new TrCond_0_3_2_3_4_4_1TO0_3_2_3_4_4_2;
	_conditions.push_back( TrCondInst_0_3_2_3_4_4_1TO0_3_2_3_4_4_2 );

		

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_3_4_4_1,Node_0_3_2_3_4_4_2  ,TrCondInst_0_3_2_3_4_4_1TO0_3_2_3_4_4_2  ) ); //0.3.2.3.4.4.1TO0.3.2.3.4.4.2
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_4_2TOMyLeftKick = new TrCond_0_3_2_3_4_4_2TOMyLeftKick;
	_conditions.push_back( TrCondInst_0_3_2_3_4_4_2TOMyLeftKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_4_2,Node_0_3_2_3_4_4_3  ,TrCondInst_0_3_2_3_4_4_2TOMyLeftKick  ) ); //0.3.2.3.4.4.2TOMyLeftKick
		

		
		
	ICondition* TrCondInst_0_3_2_3_4_4_2TOMyRightKick = new TrCond_0_3_2_3_4_4_2TOMyRightKick;
	_conditions.push_back( TrCondInst_0_3_2_3_4_4_2TOMyRightKick );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_3_4_4_2,Node_0_3_2_3_4_4_4  ,TrCondInst_0_3_2_3_4_4_2TOMyRightKick  ) ); //0.3.2.3.4.4.2TOMyRightKick
		

		
		
	ICondition* TrCondInst_MyRightKickTO0_3_2_3_4_4_5 = new TrCond_MyRightKickTO0_3_2_3_4_4_5;
	_conditions.push_back( TrCondInst_MyRightKickTO0_3_2_3_4_4_5 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_4,Node_0_3_2_3_4_4_5  ,TrCondInst_MyRightKickTO0_3_2_3_4_4_5  ) ); //MyRightKickTO0.3.2.3.4.4.5
		

		
		
	ICondition* TrCondInst_MyLeftKickTO0_3_2_3_4_4_5 = new TrCond_MyLeftKickTO0_3_2_3_4_4_5;
	_conditions.push_back( TrCondInst_MyLeftKickTO0_3_2_3_4_4_5 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_4_4_3,Node_0_3_2_3_4_4_5  ,TrCondInst_MyLeftKickTO0_3_2_3_4_4_5  ) ); //MyLeftKickTO0.3.2.3.4.4.5
		

		
		
	ICondition* TrCondInst_0_3_1TOdecision = new TrCond_0_3_1TOdecision;
	_conditions.push_back( TrCondInst_0_3_1TOdecision );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_1,Node_0_3_2  ,TrCondInst_0_3_1TOdecision  ) ); //0.3.1TOdecision
		

		
		
	ICondition* TrCondInst_decisionTOdecision = new TrCond_decisionTOdecision;
	_conditions.push_back( TrCondInst_decisionTOdecision );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_2  ,TrCondInst_decisionTOdecision  ) ); //decisionTOdecision
		

		
		
	ICondition* TrCondInst_decisionTO0_3_3 = new TrCond_decisionTO0_3_3;
	_conditions.push_back( TrCondInst_decisionTO0_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_3  ,TrCondInst_decisionTO0_3_3  ) ); //decisionTO0.3.3
	
}

simplebehavior::~simplebehavior() {
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

void simplebehavior::Start () {
	_statechart->Start();
}

void simplebehavior::Stop () {
	_statechart->Stop();
}

	
