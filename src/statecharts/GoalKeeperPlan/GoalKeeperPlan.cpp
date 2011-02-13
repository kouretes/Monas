#include "GoalKeeperPlan.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<GoalKeeperPlan>::Type temp("GoalKeeperPlan");
}
	
GoalKeeperPlan::GoalKeeperPlan(Narukom* com) {

	
	_statechart = new Statechart ( "Node_player", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );

	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );

	AndState* Node_0_2 = new AndState ( "Node_0_2", Node_0 ); //Name:0.2
	_states.push_back( Node_0_2 );

	OrState* Node_0_2_1 = new OrState ( "Node_0_2_1", Node_0_2 );	//Name:0.2.1
	_states.push_back( Node_0_2_1 );

	StartState* Node_0_2_1_1 = new StartState ( "Node_0_2_1_1", Node_0_2_1 ); //Name:0.2.1.1
	_states.push_back( Node_0_2_1_1 );

	OrState* Node_0_2_1_2 = new OrState ( "Node_RobotController_one_or_more_times_", Node_0_2_1 );	//Name:RobotController_one_or_more_times_
	_states.push_back( Node_0_2_1_2 );

	StartState* Node_0_2_1_2_1 = new StartState ( "Node_0_2_1_2_1", Node_0_2_1_2 ); //Name:0.2.1.2.1
	_states.push_back( Node_0_2_1_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2_1_2_2 = ActivityFactory::Instance()->CreateObject( "RobotController" );
	_activities.push_back( NodeActivInst_0_2_1_2_2 );			
	BasicState* Node_0_2_1_2_2 = new BasicState( "Node_RobotController", Node_0_2_1_2, NodeActivInst_0_2_1_2_2 ); //Name:RobotController
	_states.push_back( Node_0_2_1_2_2 );

	EndState* Node_0_2_1_2_3 = new EndState ( "Node_0_2_1_2_3", Node_0_2_1_2 ); //Name:0.2.1.2.3
	_states.push_back( Node_0_2_1_2_3 );

	EndState* Node_0_2_1_3 = new EndState ( "Node_0_2_1_3", Node_0_2_1 ); //Name:0.2.1.3
	_states.push_back( Node_0_2_1_3 );

	OrState* Node_0_2_2 = new OrState ( "Node_0_2_2", Node_0_2 );	//Name:0.2.2
	_states.push_back( Node_0_2_2 );

	StartState* Node_0_2_2_1 = new StartState ( "Node_0_2_2_1", Node_0_2_2 ); //Name:0.2.2.1
	_states.push_back( Node_0_2_2_1 );

	OrState* Node_0_2_2_2 = new OrState ( "Node_LedHandler_one_or_more_times_", Node_0_2_2 );	//Name:LedHandler_one_or_more_times_
	_states.push_back( Node_0_2_2_2 );

	StartState* Node_0_2_2_2_1 = new StartState ( "Node_0_2_2_2_1", Node_0_2_2_2 ); //Name:0.2.2.2.1
	_states.push_back( Node_0_2_2_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2_2_2_2 = ActivityFactory::Instance()->CreateObject( "LedHandler" );
	_activities.push_back( NodeActivInst_0_2_2_2_2 );			
	BasicState* Node_0_2_2_2_2 = new BasicState( "Node_LedHandler", Node_0_2_2_2, NodeActivInst_0_2_2_2_2 ); //Name:LedHandler
	_states.push_back( Node_0_2_2_2_2 );

	EndState* Node_0_2_2_2_3 = new EndState ( "Node_0_2_2_2_3", Node_0_2_2_2 ); //Name:0.2.2.2.3
	_states.push_back( Node_0_2_2_2_3 );

	EndState* Node_0_2_2_3 = new EndState ( "Node_0_2_2_3", Node_0_2_2 ); //Name:0.2.2.3
	_states.push_back( Node_0_2_2_3 );

	OrState* Node_0_2_3 = new OrState ( "Node_0_2_3", Node_0_2 );	//Name:0.2.3
	_states.push_back( Node_0_2_3 );

	StartState* Node_0_2_3_1 = new StartState ( "Node_0_2_3_1", Node_0_2_3 ); //Name:0.2.3.1
	_states.push_back( Node_0_2_3_1 );

	OrState* Node_0_2_3_2 = new OrState ( "Node_play_one_or_more_times_", Node_0_2_3 );	//Name:play_one_or_more_times_
	_states.push_back( Node_0_2_3_2 );

	StartState* Node_0_2_3_2_1 = new StartState ( "Node_0_2_3_2_1", Node_0_2_3_2 ); //Name:0.2.3.2.1
	_states.push_back( Node_0_2_3_2_1 );

	OrState* Node_0_2_3_2_2 = new OrState ( "Node_play", Node_0_2_3_2 );	//Name:play
	_states.push_back( Node_0_2_3_2_2 );

	StartState* Node_0_2_3_2_2_1 = new StartState ( "Node_0_2_3_2_2_1", Node_0_2_3_2_2 ); //Name:0.2.3.2.2.1
	_states.push_back( Node_0_2_3_2_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_2 = ActivityFactory::Instance()->CreateObject( "Stand" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_2 );			
	BasicState* Node_0_2_3_2_2_2 = new BasicState( "Node_Stand", Node_0_2_3_2_2, NodeActivInst_0_2_3_2_2_2 ); //Name:Stand
	_states.push_back( Node_0_2_3_2_2_2 );

	OrState* Node_0_2_3_2_2_3 = new OrState ( "Node__open_option_SendCalibration_close_option_", Node_0_2_3_2_2 );	//Name:_op_SendCalibration
	_states.push_back( Node_0_2_3_2_2_3 );

	StartState* Node_0_2_3_2_2_3_1 = new StartState ( "Node_0_2_3_2_2_3_1", Node_0_2_3_2_2_3 ); //Name:0.2.3.2.2.3.1
	_states.push_back( Node_0_2_3_2_2_3_1 );

	ConditionConnector* Node_0_2_3_2_2_3_2  = new ConditionConnector ( "Node_0_2_3_2_2_3_2", Node_0_2_3_2_2_3 ); //Name:0.2.3.2.2.3.2
	_states.push_back( Node_0_2_3_2_2_3_2 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_3_3 = ActivityFactory::Instance()->CreateObject( "SendCalibration" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_3_3 );			
	BasicState* Node_0_2_3_2_2_3_3 = new BasicState( "Node_SendCalibration", Node_0_2_3_2_2_3, NodeActivInst_0_2_3_2_2_3_3 ); //Name:SendCalibration
	_states.push_back( Node_0_2_3_2_2_3_3 );

	EndState* Node_0_2_3_2_2_3_4 = new EndState ( "Node_0_2_3_2_2_3_4", Node_0_2_3_2_2_3 ); //Name:0.2.3.2.2.3.4
	_states.push_back( Node_0_2_3_2_2_3_4 );

	OrState* Node_0_2_3_2_2_4 = new OrState ( "Node__open_option_active_close_option_", Node_0_2_3_2_2 );	//Name:_op_active
	_states.push_back( Node_0_2_3_2_2_4 );

	StartState* Node_0_2_3_2_2_4_1 = new StartState ( "Node_0_2_3_2_2_4_1", Node_0_2_3_2_2_4 ); //Name:0.2.3.2.2.4.1
	_states.push_back( Node_0_2_3_2_2_4_1 );

	ConditionConnector* Node_0_2_3_2_2_4_2  = new ConditionConnector ( "Node_0_2_3_2_2_4_2", Node_0_2_3_2_2_4 ); //Name:0.2.3.2.2.4.2
	_states.push_back( Node_0_2_3_2_2_4_2 );

	OrState* Node_0_2_3_2_2_4_3 = new OrState ( "Node_active", Node_0_2_3_2_2_4 );	//Name:active
	_states.push_back( Node_0_2_3_2_2_4_3 );

	StartState* Node_0_2_3_2_2_4_3_1 = new StartState ( "Node_0_2_3_2_2_4_3_1", Node_0_2_3_2_2_4_3 ); //Name:0.2.3.2.2.4.3.1
	_states.push_back( Node_0_2_3_2_2_4_3_1 );

	AndState* Node_0_2_3_2_2_4_3_2 = new AndState ( "Node_0_2_3_2_2_4_3_2", Node_0_2_3_2_2_4_3 ); //Name:0.2.3.2.2.4.3.2
	_states.push_back( Node_0_2_3_2_2_4_3_2 );

	OrState* Node_0_2_3_2_2_4_3_2_1 = new OrState ( "Node_0_2_3_2_2_4_3_2_1", Node_0_2_3_2_2_4_3_2 );	//Name:0.2.3.2.2.4.3.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_1 );

	StartState* Node_0_2_3_2_2_4_3_2_1_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_1_1", Node_0_2_3_2_2_4_3_2_1 ); //Name:0.2.3.2.2.4.3.2.1.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_1_1 );

	OrState* Node_0_2_3_2_2_4_3_2_1_2 = new OrState ( "Node_Sensors_one_or_more_times_", Node_0_2_3_2_2_4_3_2_1 );	//Name:Sensors_one_or_more_times_
	_states.push_back( Node_0_2_3_2_2_4_3_2_1_2 );

	StartState* Node_0_2_3_2_2_4_3_2_1_2_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_1_2_1", Node_0_2_3_2_2_4_3_2_1_2 ); //Name:0.2.3.2.2.4.3.2.1.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_1_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_4_3_2_1_2_2 = ActivityFactory::Instance()->CreateObject( "Sensors" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_4_3_2_1_2_2 );			
	BasicState* Node_0_2_3_2_2_4_3_2_1_2_2 = new BasicState( "Node_Sensors", Node_0_2_3_2_2_4_3_2_1_2, NodeActivInst_0_2_3_2_2_4_3_2_1_2_2 ); //Name:Sensors
	_states.push_back( Node_0_2_3_2_2_4_3_2_1_2_2 );

	EndState* Node_0_2_3_2_2_4_3_2_1_2_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_1_2_3", Node_0_2_3_2_2_4_3_2_1_2 ); //Name:0.2.3.2.2.4.3.2.1.2.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_1_2_3 );

	EndState* Node_0_2_3_2_2_4_3_2_1_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_1_3", Node_0_2_3_2_2_4_3_2_1 ); //Name:0.2.3.2.2.4.3.2.1.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_1_3 );

	OrState* Node_0_2_3_2_2_4_3_2_2 = new OrState ( "Node_0_2_3_2_2_4_3_2_2", Node_0_2_3_2_2_4_3_2 );	//Name:0.2.3.2.2.4.3.2.2
	_states.push_back( Node_0_2_3_2_2_4_3_2_2 );

	StartState* Node_0_2_3_2_2_4_3_2_2_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_2_1", Node_0_2_3_2_2_4_3_2_2 ); //Name:0.2.3.2.2.4.3.2.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_2_1 );

	OrState* Node_0_2_3_2_2_4_3_2_2_2 = new OrState ( "Node_Vision_one_or_more_times_", Node_0_2_3_2_2_4_3_2_2 );	//Name:Vision_one_or_more_times_
	_states.push_back( Node_0_2_3_2_2_4_3_2_2_2 );

	StartState* Node_0_2_3_2_2_4_3_2_2_2_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_2_2_1", Node_0_2_3_2_2_4_3_2_2_2 ); //Name:0.2.3.2.2.4.3.2.2.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_2_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_4_3_2_2_2_2 = ActivityFactory::Instance()->CreateObject( "Vision" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_4_3_2_2_2_2 );			
	BasicState* Node_0_2_3_2_2_4_3_2_2_2_2 = new BasicState( "Node_Vision", Node_0_2_3_2_2_4_3_2_2_2, NodeActivInst_0_2_3_2_2_4_3_2_2_2_2 ); //Name:Vision
	_states.push_back( Node_0_2_3_2_2_4_3_2_2_2_2 );

	EndState* Node_0_2_3_2_2_4_3_2_2_2_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_2_2_3", Node_0_2_3_2_2_4_3_2_2_2 ); //Name:0.2.3.2.2.4.3.2.2.2.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_2_2_3 );

	EndState* Node_0_2_3_2_2_4_3_2_2_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_2_3", Node_0_2_3_2_2_4_3_2_2 ); //Name:0.2.3.2.2.4.3.2.2.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_2_3 );

	OrState* Node_0_2_3_2_2_4_3_2_3 = new OrState ( "Node_0_2_3_2_2_4_3_2_3", Node_0_2_3_2_2_4_3_2 );	//Name:0.2.3.2.2.4.3.2.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_3 );

	StartState* Node_0_2_3_2_2_4_3_2_3_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_3_1", Node_0_2_3_2_2_4_3_2_3 ); //Name:0.2.3.2.2.4.3.2.3.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_1 );

	OrState* Node_0_2_3_2_2_4_3_2_3_2 = new OrState ( "Node_decision_one_or_more_times_", Node_0_2_3_2_2_4_3_2_3 );	//Name:decision_one_or_more_times_
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2 );

	StartState* Node_0_2_3_2_2_4_3_2_3_2_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_3_2_1", Node_0_2_3_2_2_4_3_2_3_2 ); //Name:0.2.3.2.2.4.3.2.3.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_1 );

	OrState* Node_0_2_3_2_2_4_3_2_3_2_2 = new OrState ( "Node_decision", Node_0_2_3_2_2_4_3_2_3_2 );	//Name:decision
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2 );

	StartState* Node_0_2_3_2_2_4_3_2_3_2_2_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_3_2_2_1", Node_0_2_3_2_2_4_3_2_3_2_2 ); //Name:0.2.3.2.2.4.3.2.3.2.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_1 );

	ConditionConnector* Node_0_2_3_2_2_4_3_2_3_2_2_2  = new ConditionConnector ( "Node_0_2_3_2_2_4_3_2_3_2_2_2", Node_0_2_3_2_2_4_3_2_3_2_2 ); //Name:0.2.3.2.2.4.3.2.3.2.2.2
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_2 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_3 = ActivityFactory::Instance()->CreateObject( "SearchBall" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_3 );			
	BasicState* Node_0_2_3_2_2_4_3_2_3_2_2_3 = new BasicState( "Node_SearchBall", Node_0_2_3_2_2_4_3_2_3_2_2, NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_3 ); //Name:SearchBall
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_3 );

	OrState* Node_0_2_3_2_2_4_3_2_3_2_2_4 = new OrState ( "Node_takeAction", Node_0_2_3_2_2_4_3_2_3_2_2 );	//Name:takeAction
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4 );

	StartState* Node_0_2_3_2_2_4_3_2_3_2_2_4_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_1", Node_0_2_3_2_2_4_3_2_3_2_2_4 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_1 );

	AndState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2 = new AndState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2", Node_0_2_3_2_2_4_3_2_3_2_2_4 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.2
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2 );

	OrState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1 = new OrState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1", Node_0_2_3_2_2_4_3_2_3_2_2_4_2 );	//Name:0.2.3.2.2.4.3.2.3.2.2.4.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1 );

	StartState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_1", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.2.1.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_1_2 = ActivityFactory::Instance()->CreateObject( "TrackBall" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_1_2 );			
	BasicState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_2 = new BasicState( "Node_TrackBall", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1, NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_1_2 ); //Name:TrackBall
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_2 );

	EndState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_3", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.2.1.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_3 );

	OrState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2 = new OrState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2", Node_0_2_3_2_2_4_3_2_3_2_2_4_2 );	//Name:0.2.3.2.2.4.3.2.3.2.2.4.2.2
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2 );

	StartState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_1", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.2.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_1 );

	OrState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2 = new OrState ( "Node_action", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2 );	//Name:action
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2 );

	StartState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_1 = new StartState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_1", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.1
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_1 );

	ConditionConnector* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2  = new ConditionConnector ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.2
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3 = ActivityFactory::Instance()->CreateObject( "RightFall" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3 );			
	BasicState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3 = new BasicState( "Node_RightFall", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2, NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3 ); //Name:RightFall
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4 = ActivityFactory::Instance()->CreateObject( "LeftFall" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4 );			
	BasicState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4 = new BasicState( "Node_LeftFall", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2, NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4 ); //Name:LeftFall
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5 = ActivityFactory::Instance()->CreateObject( "RightDive" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5 );			
	BasicState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5 = new BasicState( "Node_RightDive", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2, NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5 ); //Name:RightDive
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5 );

			
	
	
	

	IActivity* NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6 = ActivityFactory::Instance()->CreateObject( "LeftDive" );
	_activities.push_back( NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6 );			
	BasicState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6 = new BasicState( "Node_LeftDive", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2, NodeActivInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6 ); //Name:LeftDive
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6 );

	EndState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_7 = new EndState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_7", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.7
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_7 );

	EndState* Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_3", Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.2.2.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_3 );

	EndState* Node_0_2_3_2_2_4_3_2_3_2_2_4_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_3_2_2_4_3", Node_0_2_3_2_2_4_3_2_3_2_2_4 ); //Name:0.2.3.2.2.4.3.2.3.2.2.4.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_4_3 );

	EndState* Node_0_2_3_2_2_4_3_2_3_2_2_5 = new EndState ( "Node_0_2_3_2_2_4_3_2_3_2_2_5", Node_0_2_3_2_2_4_3_2_3_2_2 ); //Name:0.2.3.2.2.4.3.2.3.2.2.5
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_2_5 );

	EndState* Node_0_2_3_2_2_4_3_2_3_2_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_3_2_3", Node_0_2_3_2_2_4_3_2_3_2 ); //Name:0.2.3.2.2.4.3.2.3.2.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_2_3 );

	EndState* Node_0_2_3_2_2_4_3_2_3_3 = new EndState ( "Node_0_2_3_2_2_4_3_2_3_3", Node_0_2_3_2_2_4_3_2_3 ); //Name:0.2.3.2.2.4.3.2.3.3
	_states.push_back( Node_0_2_3_2_2_4_3_2_3_3 );

	EndState* Node_0_2_3_2_2_4_3_3 = new EndState ( "Node_0_2_3_2_2_4_3_3", Node_0_2_3_2_2_4_3 ); //Name:0.2.3.2.2.4.3.3
	_states.push_back( Node_0_2_3_2_2_4_3_3 );

	EndState* Node_0_2_3_2_2_4_4 = new EndState ( "Node_0_2_3_2_2_4_4", Node_0_2_3_2_2_4 ); //Name:0.2.3.2.2.4.4
	_states.push_back( Node_0_2_3_2_2_4_4 );

	EndState* Node_0_2_3_2_2_5 = new EndState ( "Node_0_2_3_2_2_5", Node_0_2_3_2_2 ); //Name:0.2.3.2.2.5
	_states.push_back( Node_0_2_3_2_2_5 );

	EndState* Node_0_2_3_2_3 = new EndState ( "Node_0_2_3_2_3", Node_0_2_3_2 ); //Name:0.2.3.2.3
	_states.push_back( Node_0_2_3_2_3 );

	EndState* Node_0_2_3_3 = new EndState ( "Node_0_2_3_3", Node_0_2_3 ); //Name:0.2.3.3
	_states.push_back( Node_0_2_3_3 );

	OrState* Node_0_2_4 = new OrState ( "Node_0_2_4", Node_0_2 );	//Name:0.2.4
	_states.push_back( Node_0_2_4 );

	StartState* Node_0_2_4_1 = new StartState ( "Node_0_2_4_1", Node_0_2_4 ); //Name:0.2.4.1
	_states.push_back( Node_0_2_4_1 );

	OrState* Node_0_2_4_2 = new OrState ( "Node_MotionController_one_or_more_times_", Node_0_2_4 );	//Name:MotionController_one_or_more_times_
	_states.push_back( Node_0_2_4_2 );

	StartState* Node_0_2_4_2_1 = new StartState ( "Node_0_2_4_2_1", Node_0_2_4_2 ); //Name:0.2.4.2.1
	_states.push_back( Node_0_2_4_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2_4_2_2 = ActivityFactory::Instance()->CreateObject( "MotionController" );
	_activities.push_back( NodeActivInst_0_2_4_2_2 );			
	BasicState* Node_0_2_4_2_2 = new BasicState( "Node_MotionController", Node_0_2_4_2, NodeActivInst_0_2_4_2_2 ); //Name:MotionController
	_states.push_back( Node_0_2_4_2_2 );

	EndState* Node_0_2_4_2_3 = new EndState ( "Node_0_2_4_2_3", Node_0_2_4_2 ); //Name:0.2.4.2.3
	_states.push_back( Node_0_2_4_2_3 );

	EndState* Node_0_2_4_3 = new EndState ( "Node_0_2_4_3", Node_0_2_4 ); //Name:0.2.4.3
	_states.push_back( Node_0_2_4_3 );

	EndState* Node_0_3 = new EndState ( "Node_0_3", Node_0 ); //Name:0.3
	_states.push_back( Node_0_3 );
	
	
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_1,Node_0_2_3_2_2_2    ) ); //0.2.3.2.2.1TO0.2.3.2.2.2
	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_3_2_2_4_3_2_3_2_2_1,Node_0_2_3_2_2_4_3_2_3_2_2_2    ) ); //0.2.3.2.2.4.3.2.3.2.2.1TO0.2.3.2.2.4.3.2.3.2.2.2
		

		
		
	ICondition* TrCondInst_0_2_2_2_2TO0_2_2_2_3 = new TrCond_0_2_2_2_2TO0_2_2_2_3;
	_conditions.push_back( TrCondInst_0_2_2_2_2TO0_2_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2_2,Node_0_2_2_2_3  ,TrCondInst_0_2_2_2_2TO0_2_2_2_3  ) ); //0.2.2.2.2TO0.2.2.2.3
		

		
		
	ICondition* TrCondInst_0_2_2_2_2TO0_2_2_2_2 = new TrCond_0_2_2_2_2TO0_2_2_2_2;
	_conditions.push_back( TrCondInst_0_2_2_2_2TO0_2_2_2_2 );

		
		
	IAction* TrActionInst_0_2_2_2_2TO0_2_2_2_2 = new TrAction_0_2_2_2_2TO0_2_2_2_2;
	_actions.push_back( TrActionInst_0_2_2_2_2TO0_2_2_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2_2,Node_0_2_2_2_2  ,TrCondInst_0_2_2_2_2TO0_2_2_2_2 ,TrActionInst_0_2_2_2_2TO0_2_2_2_2 ) ); //0.2.2.2.2TO0.2.2.2.2
		

		
		
	ICondition* TrCondInst_0_2_2_2TO0_2_2_3 = new TrCond_0_2_2_2TO0_2_2_3;
	_conditions.push_back( TrCondInst_0_2_2_2TO0_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2,Node_0_2_2_3  ,TrCondInst_0_2_2_2TO0_2_2_3  ) ); //0.2.2.2TO0.2.2.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_7    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.4TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.7
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4,Node_0_2_3_2_2_4_3_2_3_2_2_5    ) ); //0.2.3.2.2.4.3.2.3.2.2.4TO0.2.3.2.2.4.3.2.3.2.2.5
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_2,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_3    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.1.2TO0.2.3.2.2.4.3.2.3.2.2.4.2.1.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4,Node_0_2_3_2_2_5    ) ); //0.2.3.2.2.4TO0.2.3.2.2.5
		

		
		
	ICondition* TrCondInst_0_2_3_2_1TO0_2_3_2_2 = new TrCond_0_2_3_2_1TO0_2_3_2_2;
	_conditions.push_back( TrCondInst_0_2_3_2_1TO0_2_3_2_2 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_1,Node_0_2_3_2_2  ,TrCondInst_0_2_3_2_1TO0_2_3_2_2  ) ); //0.2.3.2.1TO0.2.3.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2,Node_0_2_3_2_2_4_3_3    ) ); //0.2.3.2.2.4.3.2TO0.2.3.2.2.4.3.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_1,Node_0_2_3_2    ) ); //0.2.3.1TO0.2.3.2
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6 = new TrCond_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6  ,TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6  ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.2TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.6
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5 = new TrCond_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5  ,TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5  ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.2TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.5
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4 = new TrCond_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4  ,TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_4  ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.2TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.4
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3 = new TrCond_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3  ,TrCondInst_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3  ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.2TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.3
		

		
		
	ICondition* TrCondInst_0_2_1_2_2TO0_2_1_2_3 = new TrCond_0_2_1_2_2TO0_2_1_2_3;
	_conditions.push_back( TrCondInst_0_2_1_2_2TO0_2_1_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2_2,Node_0_2_1_2_3  ,TrCondInst_0_2_1_2_2TO0_2_1_2_3  ) ); //0.2.1.2.2TO0.2.1.2.3
		

		
		
	ICondition* TrCondInst_0_2_1_2_2TO0_2_1_2_2 = new TrCond_0_2_1_2_2TO0_2_1_2_2;
	_conditions.push_back( TrCondInst_0_2_1_2_2TO0_2_1_2_2 );

		
		
	IAction* TrActionInst_0_2_1_2_2TO0_2_1_2_2 = new TrAction_0_2_1_2_2TO0_2_1_2_2;
	_actions.push_back( TrActionInst_0_2_1_2_2TO0_2_1_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2_2,Node_0_2_1_2_2  ,TrCondInst_0_2_1_2_2TO0_2_1_2_2 ,TrActionInst_0_2_1_2_2TO0_2_1_2_2 ) ); //0.2.1.2.2TO0.2.1.2.2
		

		
		
	ICondition* TrCondInst_0_2_4_2_2TO0_2_4_2_3 = new TrCond_0_2_4_2_2TO0_2_4_2_3;
	_conditions.push_back( TrCondInst_0_2_4_2_2TO0_2_4_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_2_2,Node_0_2_4_2_3  ,TrCondInst_0_2_4_2_2TO0_2_4_2_3  ) ); //0.2.4.2.2TO0.2.4.2.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_1,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_1_2    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.1.1TO0.2.3.2.2.4.3.2.3.2.2.4.2.1.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_3,Node_0_2_3_2_2_4_3_2_3_2_2_5    ) ); //0.2.3.2.2.4.3.2.3.2.2.3TO0.2.3.2.2.4.3.2.3.2.2.5
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_1,Node_0_2_1_2    ) ); //0.2.1.1TO0.2.1.2
		

		
		
	ICondition* TrCondInst_0_2_4_2_2TO0_2_4_2_2 = new TrCond_0_2_4_2_2TO0_2_4_2_2;
	_conditions.push_back( TrCondInst_0_2_4_2_2TO0_2_4_2_2 );

		
		
	IAction* TrActionInst_0_2_4_2_2TO0_2_4_2_2 = new TrAction_0_2_4_2_2TO0_2_4_2_2;
	_actions.push_back( TrActionInst_0_2_4_2_2TO0_2_4_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_2_2,Node_0_2_4_2_2  ,TrCondInst_0_2_4_2_2TO0_2_4_2_2 ,TrActionInst_0_2_4_2_2TO0_2_4_2_2 ) ); //0.2.4.2.2TO0.2.4.2.2
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_3TO0_2_3_2_2_4 = new TrCond_0_2_3_2_2_3TO0_2_3_2_2_4;
	_conditions.push_back( TrCondInst_0_2_3_2_2_3TO0_2_3_2_2_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_3,Node_0_2_3_2_2_4  ,TrCondInst_0_2_3_2_2_3TO0_2_3_2_2_4  ) ); //0.2.3.2.2.3TO0.2.3.2.2.4
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_1,Node_0_2_3_2_2_4_3_2_3_2_2_4_2    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.1TO0.2.3.2.2.4.3.2.3.2.2.4.2
		

		
		
	ICondition* TrCondInst_0_2_3_2TO0_2_3_3 = new TrCond_0_2_3_2TO0_2_3_3;
	_conditions.push_back( TrCondInst_0_2_3_2TO0_2_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2,Node_0_2_3_3  ,TrCondInst_0_2_3_2TO0_2_3_3  ) ); //0.2.3.2TO0.2.3.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_3    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2    ) ); //0.1TO0.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_5,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_7    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.5TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.7
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2_1,Node_0_2_2_2_2    ) ); //0.2.2.2.1TO0.2.2.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_3_3,Node_0_2_3_2_2_3_4    ) ); //0.2.3.2.2.3.3TO0.2.3.2.2.3.4
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_3_2TO0_2_3_2_2_3_4 = new TrCond_0_2_3_2_2_3_2TO0_2_3_2_2_3_4;
	_conditions.push_back( TrCondInst_0_2_3_2_2_3_2TO0_2_3_2_2_3_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_3_2,Node_0_2_3_2_2_3_4  ,TrCondInst_0_2_3_2_2_3_2TO0_2_3_2_2_3_4  ) ); //0.2.3.2.2.3.2TO0.2.3.2.2.3.4
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_3_2TO0_2_3_2_2_3_3 = new TrCond_0_2_3_2_2_3_2TO0_2_3_2_2_3_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_3_2TO0_2_3_2_2_3_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_3_2,Node_0_2_3_2_2_3_3  ,TrCondInst_0_2_3_2_2_3_2TO0_2_3_2_2_3_3  ) ); //0.2.3.2.2.3.2TO0.2.3.2.2.3.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_1_1,Node_0_2_3_2_2_4_3_2_1_2    ) ); //0.2.3.2.2.4.3.2.1.1TO0.2.3.2.2.4.3.2.1.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_1_2_1,Node_0_2_3_2_2_4_3_2_1_2_2    ) ); //0.2.3.2.2.4.3.2.1.2.1TO0.2.3.2.2.4.3.2.1.2.2
	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_3_2_2_3_1,Node_0_2_3_2_2_3_2    ) ); //0.2.3.2.2.3.1TO0.2.3.2.2.3.2
		

		
		
	ICondition* TrCondInst_0_2_1_2TO0_2_1_3 = new TrCond_0_2_1_2TO0_2_1_3;
	_conditions.push_back( TrCondInst_0_2_1_2TO0_2_1_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2,Node_0_2_1_3  ,TrCondInst_0_2_1_2TO0_2_1_3  ) ); //0.2.1.2TO0.2.1.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_2_2_1,Node_0_2_3_2_2_4_3_2_2_2_2    ) ); //0.2.3.2.2.4.3.2.2.2.1TO0.2.3.2.2.4.3.2.2.2.2
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_1_2TO0_2_3_2_2_4_3_2_1_3 = new TrCond_0_2_3_2_2_4_3_2_1_2TO0_2_3_2_2_4_3_2_1_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_1_2TO0_2_3_2_2_4_3_2_1_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_1_2,Node_0_2_3_2_2_4_3_2_1_3  ,TrCondInst_0_2_3_2_2_4_3_2_1_2TO0_2_3_2_2_4_3_2_1_3  ) ); //0.2.3.2.2.4.3.2.1.2TO0.2.3.2.2.4.3.2.1.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_1,Node_0_2_3_2_2_4_3_2_3_2_2    ) ); //0.2.3.2.2.4.3.2.3.2.1TO0.2.3.2.2.4.3.2.3.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_1,Node_0_2_4_2    ) ); //0.2.4.1TO0.2.4.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_3,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_7    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.3TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.7
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_2_1,Node_0_2_3_2_2_4_3_2_2_2    ) ); //0.2.3.2.2.4.3.2.2.1TO0.2.3.2.2.4.3.2.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_1,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.1TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4 = new TrCond_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_4_3_2_3_2_2_2,Node_0_2_3_2_2_4_3_2_3_2_2_4  ,TrCondInst_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_4  ) ); //0.2.3.2.2.4.3.2.3.2.2.2TO0.2.3.2.2.4.3.2.3.2.2.4
		

		
		
	ICondition* TrCondInst_0_2_3_2_2TO0_2_3_2_3 = new TrCond_0_2_3_2_2TO0_2_3_2_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2TO0_2_3_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2,Node_0_2_3_2_3  ,TrCondInst_0_2_3_2_2TO0_2_3_2_3  ) ); //0.2.3.2.2TO0.2.3.2.3
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_3 = new TrCond_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_4_3_2_3_2_2_2,Node_0_2_3_2_2_4_3_2_3_2_2_3  ,TrCondInst_0_2_3_2_2_4_3_2_3_2_2_2TO0_2_3_2_2_4_3_2_3_2_2_3  ) ); //0.2.3.2.2.4.3.2.3.2.2.2TO0.2.3.2.2.4.3.2.3.2.2.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_2,Node_0_2_3_2_2_3    ) ); //0.2.3.2.2.2TO0.2.3.2.2.3
		

		
		
	ICondition* TrCondInst_0_2_3_2_2TO0_2_3_2_2 = new TrCond_0_2_3_2_2TO0_2_3_2_2;
	_conditions.push_back( TrCondInst_0_2_3_2_2TO0_2_3_2_2 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2,Node_0_2_3_2_2  ,TrCondInst_0_2_3_2_2TO0_2_3_2_2  ) ); //0.2.3.2.2TO0.2.3.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_1,Node_0_2_3_2_2_4_3_2    ) ); //0.2.3.2.2.4.3.1TO0.2.3.2.2.4.3.2
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_2_2TO0_2_3_2_2_4_3_2_2_3 = new TrCond_0_2_3_2_2_4_3_2_2_2TO0_2_3_2_2_4_3_2_2_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_2_2TO0_2_3_2_2_4_3_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_2_2,Node_0_2_3_2_2_4_3_2_2_3  ,TrCondInst_0_2_3_2_2_4_3_2_2_2TO0_2_3_2_2_4_3_2_2_3  ) ); //0.2.3.2.2.4.3.2.2.2TO0.2.3.2.2.4.3.2.2.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2,Node_0_2_3_2_2_4_3_2_3_2_2_4_3    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2TO0.2.3.2.2.4.3.2.3.2.2.4.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2_1,Node_0_2_1_2_2    ) ); //0.2.1.2.1TO0.2.1.2.2
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_3 = new TrCond_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_1_2_2,Node_0_2_3_2_2_4_3_2_1_2_3  ,TrCondInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_3  ) ); //0.2.3.2.2.4.3.2.1.2.2TO0.2.3.2.2.4.3.2.1.2.3
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3TO0_2_3_2_2_4_4 = new TrCond_0_2_3_2_2_4_3TO0_2_3_2_2_4_4;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3TO0_2_3_2_2_4_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3,Node_0_2_3_2_2_4_4  ,TrCondInst_0_2_3_2_2_4_3TO0_2_3_2_2_4_4  ) ); //0.2.3.2.2.4.3TO0.2.3.2.2.4.4
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_2 = new TrCond_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_2;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_2 );

		
		
	IAction* TrActionInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_2 = new TrAction_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_2;
	_actions.push_back( TrActionInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_1_2_2,Node_0_2_3_2_2_4_3_2_1_2_2  ,TrCondInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_2 ,TrActionInst_0_2_3_2_2_4_3_2_1_2_2TO0_2_3_2_2_4_3_2_1_2_2 ) ); //0.2.3.2.2.4.3.2.1.2.2TO0.2.3.2.2.4.3.2.1.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_1,Node_0_2_3_2_2_4_3_2_3_2    ) ); //0.2.3.2.2.4.3.2.3.1TO0.2.3.2.2.4.3.2.3.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_1,Node_0_2_2_2    ) ); //0.2.2.1TO0.2.2.2
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_2TO0_2_3_2_2_4_4 = new TrCond_0_2_3_2_2_4_2TO0_2_3_2_2_4_4;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_2TO0_2_3_2_2_4_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_4_2,Node_0_2_3_2_2_4_4  ,TrCondInst_0_2_3_2_2_4_2TO0_2_3_2_2_4_4  ) ); //0.2.3.2.2.4.2TO0.2.3.2.2.4.4
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_6,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_7    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.6TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.7
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_3 = new TrCond_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_2_2_2,Node_0_2_3_2_2_4_3_2_2_2_3  ,TrCondInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_3  ) ); //0.2.3.2.2.4.3.2.2.2.2TO0.2.3.2.2.4.3.2.2.2.3
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_2TO0_2_3_2_2_4_3 = new TrCond_0_2_3_2_2_4_2TO0_2_3_2_2_4_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_2TO0_2_3_2_2_4_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_2_3_2_2_4_2,Node_0_2_3_2_2_4_3  ,TrCondInst_0_2_3_2_2_4_2TO0_2_3_2_2_4_3  ) ); //0.2.3.2.2.4.2TO0.2.3.2.2.4.3
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2 = new TrCond_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2 );

		
		
	IAction* TrActionInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2 = new TrAction_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2;
	_actions.push_back( TrActionInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_2_2_2,Node_0_2_3_2_2_4_3_2_2_2_2  ,TrCondInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2 ,TrActionInst_0_2_3_2_2_4_3_2_2_2_2TO0_2_3_2_2_4_3_2_2_2_2 ) ); //0.2.3.2.2.4.3.2.2.2.2TO0.2.3.2.2.4.3.2.2.2.2
		

		
		
	ICondition* TrCondInst_0_2_4_2TO0_2_4_3 = new TrCond_0_2_4_2TO0_2_4_3;
	_conditions.push_back( TrCondInst_0_2_4_2TO0_2_4_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_2,Node_0_2_4_3  ,TrCondInst_0_2_4_2TO0_2_4_3  ) ); //0.2.4.2TO0.2.4.3
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2TO0_2_3_2_2_4_3_2_3_3 = new TrCond_0_2_3_2_2_4_3_2_3_2TO0_2_3_2_2_4_3_2_3_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2TO0_2_3_2_2_4_3_2_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2,Node_0_2_3_2_2_4_3_2_3_3  ,TrCondInst_0_2_3_2_2_4_3_2_3_2TO0_2_3_2_2_4_3_2_3_3  ) ); //0.2.3.2.2.4.3.2.3.2TO0.2.3.2.2.4.3.2.3.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_2_1,Node_0_2_4_2_2    ) ); //0.2.4.2.1TO0.2.4.2.2
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2_2TO0_2_3_2_2_4_3_2_3_2_3 = new TrCond_0_2_3_2_2_4_3_2_3_2_2TO0_2_3_2_2_4_3_2_3_2_3;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2_2TO0_2_3_2_2_4_3_2_3_2_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2,Node_0_2_3_2_2_4_3_2_3_2_3  ,TrCondInst_0_2_3_2_2_4_3_2_3_2_2TO0_2_3_2_2_4_3_2_3_2_3  ) ); //0.2.3.2.2.4.3.2.3.2.2TO0.2.3.2.2.4.3.2.3.2.3
		

		
		
	ICondition* TrCondInst_0_2_3_2_2_4_3_2_3_2_2TO0_2_3_2_2_4_3_2_3_2_2 = new TrCond_0_2_3_2_2_4_3_2_3_2_2TO0_2_3_2_2_4_3_2_3_2_2;
	_conditions.push_back( TrCondInst_0_2_3_2_2_4_3_2_3_2_2TO0_2_3_2_2_4_3_2_3_2_2 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2_4_3_2_3_2_2,Node_0_2_3_2_2_4_3_2_3_2_2  ,TrCondInst_0_2_3_2_2_4_3_2_3_2_2TO0_2_3_2_2_4_3_2_3_2_2  ) ); //0.2.3.2.2.4.3.2.3.2.2TO0.2.3.2.2.4.3.2.3.2.2
	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_3_2_2_4_1,Node_0_2_3_2_2_4_2    ) ); //0.2.3.2.2.4.1TO0.2.3.2.2.4.2
	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_1,Node_0_2_3_2_2_4_3_2_3_2_2_4_2_2_2_2    ) ); //0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.1TO0.2.3.2.2.4.3.2.3.2.2.4.2.2.2.2
		

		
		
	ICondition* TrCondInst_0_2TO0_3 = new TrCond_0_2TO0_3;
	_conditions.push_back( TrCondInst_0_2TO0_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3  ,TrCondInst_0_2TO0_3  ) ); //0.2TO0.3
	
}

GoalKeeperPlan::~GoalKeeperPlan() {
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

void GoalKeeperPlan::Start () {
	_statechart->Start();
}

void GoalKeeperPlan::Stop () {
	_statechart->Stop();
}

	