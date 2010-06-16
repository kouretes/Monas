#include "KouretesAllConcurrentStatechart.h"
#include "transitionHeaders.h"

using namespace statechart_engine;

namespace {
	StatechartRegistrar<KouretesAllConcurrentStatechartStatechart>::Type temp("KouretesAllConcurrentStatechartStatechart");
}

KouretesAllConcurrentStatechartStatechart::KouretesAllConcurrentStatechartStatechart(Narukom* com) {


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

	OrState* Node_0_2_1_2 = new OrState ( "Node_Sensors_forever_", Node_0_2_1 );	//Name:Sensors_forever_
	_states.push_back( Node_0_2_1_2 );

	StartState* Node_0_2_1_2_1 = new StartState ( "Node_0_2_1_2_1", Node_0_2_1_2 ); //Name:0.2.1.2.1
	_states.push_back( Node_0_2_1_2_1 );






	IActivity* NodeActivInst_0_2_1_2_2 = ActivityFactory::Instance()->CreateObject( "Sensors" );
	_activities.push_back( NodeActivInst_0_2_1_2_2 );
	BasicState* Node_0_2_1_2_2 = new BasicState( "Node_Sensors", Node_0_2_1_2, NodeActivInst_0_2_1_2_2 ); //Name:Sensors
	_states.push_back( Node_0_2_1_2_2 );

	EndState* Node_0_2_1_3 = new EndState ( "Node_0_2_1_3", Node_0_2_1 ); //Name:0.2.1.3
	_states.push_back( Node_0_2_1_3 );

	OrState* Node_0_2_2 = new OrState ( "Node_0_2_2", Node_0_2 );	//Name:0.2.2
	_states.push_back( Node_0_2_2 );

	StartState* Node_0_2_2_1 = new StartState ( "Node_0_2_2_1", Node_0_2_2 ); //Name:0.2.2.1
	_states.push_back( Node_0_2_2_1 );

	OrState* Node_0_2_2_2 = new OrState ( "Node_MotionController_forever_", Node_0_2_2 );	//Name:MotionController_forever_
	_states.push_back( Node_0_2_2_2 );

	StartState* Node_0_2_2_2_1 = new StartState ( "Node_0_2_2_2_1", Node_0_2_2_2 ); //Name:0.2.2.2.1
	_states.push_back( Node_0_2_2_2_1 );






        IActivity* NodeActivInst_0_2_2_2_2 = ActivityFactory::Instance()->CreateObject( "MotionController" );
	_activities.push_back( NodeActivInst_0_2_2_2_2 );
	BasicState* Node_0_2_2_2_2 = new BasicState( "Node_MotionController", Node_0_2_2_2, NodeActivInst_0_2_2_2_2 ); //Name:MotionController
	_states.push_back( Node_0_2_2_2_2 );

	EndState* Node_0_2_2_3 = new EndState ( "Node_0_2_2_3", Node_0_2_2 ); //Name:0.2.2.3
	_states.push_back( Node_0_2_2_3 );

	OrState* Node_0_2_3 = new OrState ( "Node_0_2_3", Node_0_2 );	//Name:0.2.3
	_states.push_back( Node_0_2_3 );

	StartState* Node_0_2_3_1 = new StartState ( "Node_0_2_3_1", Node_0_2_3 ); //Name:0.2.3.1
	_states.push_back( Node_0_2_3_1 );

	OrState* Node_0_2_3_2 = new OrState ( "Node_Vision_forever_", Node_0_2_3 );	//Name:Vision_forever_
	_states.push_back( Node_0_2_3_2 );

	StartState* Node_0_2_3_2_1 = new StartState ( "Node_0_2_3_2_1", Node_0_2_3_2 ); //Name:0.2.3.2.1
	_states.push_back( Node_0_2_3_2_1 );






        IActivity* NodeActivInst_0_2_3_2_2 = ActivityFactory::Instance()->CreateObject( "Vision" );
	_activities.push_back( NodeActivInst_0_2_3_2_2 );
	BasicState* Node_0_2_3_2_2 = new BasicState( "Node_Vision", Node_0_2_3_2, NodeActivInst_0_2_3_2_2 ); //Name:Vision
	_states.push_back( Node_0_2_3_2_2 );

	EndState* Node_0_2_3_3 = new EndState ( "Node_0_2_3_3", Node_0_2_3 ); //Name:0.2.3.3
	_states.push_back( Node_0_2_3_3 );

	OrState* Node_0_2_4 = new OrState ( "Node_0_2_4", Node_0_2 );	//Name:0.2.4
	_states.push_back( Node_0_2_4 );

	StartState* Node_0_2_4_1 = new StartState ( "Node_0_2_4_1", Node_0_2_4 ); //Name:0.2.4.1
	_states.push_back( Node_0_2_4_1 );

	OrState* Node_0_2_4_2 = new OrState ( "Node_Behavior_forever_", Node_0_2_4 );	//Name:Behavior_forever_
	_states.push_back( Node_0_2_4_2 );

	StartState* Node_0_2_4_2_1 = new StartState ( "Node_0_2_4_2_1", Node_0_2_4_2 ); //Name:0.2.4.2.1
	_states.push_back( Node_0_2_4_2_1 );






        IActivity* NodeActivInst_0_2_4_2_2 = ActivityFactory::Instance()->CreateObject( "Behavior" );
	_activities.push_back( NodeActivInst_0_2_4_2_2 );
	BasicState* Node_0_2_4_2_2 = new BasicState( "Node_Behavior", Node_0_2_4_2, NodeActivInst_0_2_4_2_2 ); //Name:Behavior
	_states.push_back( Node_0_2_4_2_2 );

	EndState* Node_0_2_4_3 = new EndState ( "Node_0_2_4_3", Node_0_2_4 ); //Name:0.2.4.3
	_states.push_back( Node_0_2_4_3 );

	OrState* Node_0_2_5 = new OrState ( "Node_0_2_5", Node_0_2 );	//Name:0.2.5
	_states.push_back( Node_0_2_5 );

	StartState* Node_0_2_5_1 = new StartState ( "Node_0_2_5_1", Node_0_2_5 ); //Name:0.2.5.1
	_states.push_back( Node_0_2_5_1 );

	OrState* Node_0_2_5_2 = new OrState ( "Node_RobotController_forever_", Node_0_2_5 );	//Name:RobotController_forever_
	_states.push_back( Node_0_2_5_2 );

	StartState* Node_0_2_5_2_1 = new StartState ( "Node_0_2_5_2_1", Node_0_2_5_2 ); //Name:0.2.5.2.1
	_states.push_back( Node_0_2_5_2_1 );






        IActivity* NodeActivInst_0_2_5_2_2 = ActivityFactory::Instance()->CreateObject( "RobotController" );
	_activities.push_back( NodeActivInst_0_2_5_2_2 );
	BasicState* Node_0_2_5_2_2 = new BasicState( "Node_RobotController", Node_0_2_5_2, NodeActivInst_0_2_5_2_2 ); //Name:RobotController
	_states.push_back( Node_0_2_5_2_2 );

	EndState* Node_0_2_5_3 = new EndState ( "Node_0_2_5_3", Node_0_2_5 ); //Name:0.2.5.3
	_states.push_back( Node_0_2_5_3 );

	OrState* Node_0_2_6 = new OrState ( "Node_0_2_6", Node_0_2 );	//Name:0.2.6
	_states.push_back( Node_0_2_6 );

	StartState* Node_0_2_6_1 = new StartState ( "Node_0_2_6_1", Node_0_2_6 ); //Name:0.2.6.1
	_states.push_back( Node_0_2_6_1 );

	OrState* Node_0_2_6_2 = new OrState ( "Node_LedHandler_forever_", Node_0_2_6 );	//Name:LedHandler_forever_
	_states.push_back( Node_0_2_6_2 );

	StartState* Node_0_2_6_2_1 = new StartState ( "Node_0_2_6_2_1", Node_0_2_6_2 ); //Name:0.2.6.2.1
	_states.push_back( Node_0_2_6_2_1 );






        IActivity* NodeActivInst_0_2_6_2_2 = ActivityFactory::Instance()->CreateObject( "LedHandler" );
	_activities.push_back( NodeActivInst_0_2_6_2_2 );
	BasicState* Node_0_2_6_2_2 = new BasicState( "Node_LedHandler", Node_0_2_6_2, NodeActivInst_0_2_6_2_2 ); //Name:LedHandler
	_states.push_back( Node_0_2_6_2_2 );

	EndState* Node_0_2_6_3 = new EndState ( "Node_0_2_6_3", Node_0_2_6 ); //Name:0.2.6.3
	_states.push_back( Node_0_2_6_3 );

	EndState* Node_0_3 = new EndState ( "Node_0_3", Node_0 ); //Name:0.3
	_states.push_back( Node_0_3 );


	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_1,Node_0_2_1_2    ) ); //0.2.1.1TO0.2.1.2
	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_6_2,Node_0_2_6_3    ) ); //0.2.6.2TO0.2.6.3




	ICondition* TrCondInst_0_2_1_2_2TO0_2_1_2_2 = new TrCond_0_2_1_2_2TO0_2_1_2_2;
	_conditions.push_back( TrCondInst_0_2_1_2_2TO0_2_1_2_2 );



	IAction* TrActionInst_0_2_1_2_2TO0_2_1_2_2 = new TrAction_0_2_1_2_2TO0_2_1_2_2;
	_actions.push_back( TrActionInst_0_2_1_2_2TO0_2_1_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2_2,Node_0_2_1_2_2  ,TrCondInst_0_2_1_2_2TO0_2_1_2_2 ,TrActionInst_0_2_1_2_2TO0_2_1_2_2 ) ); //0.2.1.2.2TO0.2.1.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_5_2_1,Node_0_2_5_2_2    ) ); //0.2.5.2.1TO0.2.5.2.2
	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3,    ) ); //0.2TO0.3
	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2,Node_0_2_2_3    ) ); //0.2.2.2TO0.2.2.3




	ICondition* TrCondInst_0_2_6_2_2TO0_2_6_2_2 = new TrCond_0_2_6_2_2TO0_2_6_2_2;
	_conditions.push_back( TrCondInst_0_2_6_2_2TO0_2_6_2_2 );



	IAction* TrActionInst_0_2_6_2_2TO0_2_6_2_2 = new TrAction_0_2_6_2_2TO0_2_6_2_2;
	_actions.push_back( TrActionInst_0_2_6_2_2TO0_2_6_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_6_2_2,Node_0_2_6_2_2  ,TrCondInst_0_2_6_2_2TO0_2_6_2_2 ,TrActionInst_0_2_6_2_2TO0_2_6_2_2 ) ); //0.2.6.2.2TO0.2.6.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_1,Node_0_2_4_2    ) ); //0.2.4.1TO0.2.4.2




	ICondition* TrCondInst_0_2_5_2_2TO0_2_5_2_2 = new TrCond_0_2_5_2_2TO0_2_5_2_2;
	_conditions.push_back( TrCondInst_0_2_5_2_2TO0_2_5_2_2 );



	IAction* TrActionInst_0_2_5_2_2TO0_2_5_2_2 = new TrAction_0_2_5_2_2TO0_2_5_2_2;
	_actions.push_back( TrActionInst_0_2_5_2_2TO0_2_5_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_5_2_2,Node_0_2_5_2_2  ,TrCondInst_0_2_5_2_2TO0_2_5_2_2 ,TrActionInst_0_2_5_2_2TO0_2_5_2_2 ) ); //0.2.5.2.2TO0.2.5.2.2
	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_5_2,Node_0_2_5_3    ) ); //0.2.5.2TO0.2.5.3
	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2,Node_0_2_1_3    ) ); //0.2.1.2TO0.2.1.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_1,Node_0_2_3_2    ) ); //0.2.3.1TO0.2.3.2
	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_2,Node_0_2_4_3    ) ); //0.2.4.2TO0.2.4.3




	ICondition* TrCondInst_0_2_4_2_1TO0_2_4_2_2 = new TrCond_0_2_4_2_1TO0_2_4_2_2;
	_conditions.push_back( TrCondInst_0_2_4_2_1TO0_2_4_2_2 );



	IAction* TrActionInst_0_2_4_2_1TO0_2_4_2_2 = new TrAction_0_2_4_2_1TO0_2_4_2_2;
	_actions.push_back( TrActionInst_0_2_4_2_1TO0_2_4_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_2_1,Node_0_2_4_2_2  ) ); //0.2.4.2.1TO0.2.4.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_6_1,Node_0_2_6_2    ) ); //0.2.6.1TO0.2.6.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_1,Node_0_2_3_2_2    ) ); //0.2.3.2.1TO0.2.3.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_1,Node_0_2_2_2    ) ); //0.2.2.1TO0.2.2.2
        _transitions.push_back( new TransitionSegment<State,State>(Node_0_2_4_2_2,Node_0_2_4_2_2,TrCondInst_0_2_4_2_1TO0_2_4_2_2 ,TrActionInst_0_2_4_2_1TO0_2_4_2_2     ) ); //0.2.4.2.2TO0.2.4.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2_1,Node_0_2_2_2_2    ) ); //0.2.2.2.1TO0.2.2.2.2




	ICondition* TrCondInst_0_2_3_2_2TO0_2_3_2_2 = new TrCond_0_2_3_2_2TO0_2_3_2_2;
	_conditions.push_back( TrCondInst_0_2_3_2_2TO0_2_3_2_2 );



	IAction* TrActionInst_0_2_3_2_2TO0_2_3_2_2 = new TrAction_0_2_3_2_2TO0_2_3_2_2;
	_actions.push_back( TrActionInst_0_2_3_2_2TO0_2_3_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2_2,Node_0_2_3_2_2  ,TrCondInst_0_2_3_2_2TO0_2_3_2_2 ,TrActionInst_0_2_3_2_2TO0_2_3_2_2 ) ); //0.2.3.2.2TO0.2.3.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2_1,Node_0_2_1_2_2    ) ); //0.2.1.2.1TO0.2.1.2.2
	//_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_3_2,Node_0_2_3_3    ) ); //0.2.3.2TO0.2.3.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_5_1,Node_0_2_5_2    ) ); //0.2.5.1TO0.2.5.2




	ICondition* TrCondInst_0_2_2_2_2TO0_2_2_2_2 = new TrCond_0_2_2_2_2TO0_2_2_2_2;
	_conditions.push_back( TrCondInst_0_2_2_2_2TO0_2_2_2_2 );



	IAction* TrActionInst_0_2_2_2_2TO0_2_2_2_2 = new TrAction_0_2_2_2_2TO0_2_2_2_2;
	_actions.push_back( TrActionInst_0_2_2_2_2TO0_2_2_2_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2_2,Node_0_2_2_2_2  ,TrCondInst_0_2_2_2_2TO0_2_2_2_2 ,TrActionInst_0_2_2_2_2TO0_2_2_2_2 ) ); //0.2.2.2.2TO0.2.2.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_6_2_1,Node_0_2_6_2_2    ) ); //0.2.6.2.1TO0.2.6.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2    ) ); //0.1TO0.2

}

KouretesAllConcurrentStatechartStatechart::~KouretesAllConcurrentStatechartStatechart() {
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

void KouretesAllConcurrentStatechartStatechart::Start () {
	_statechart->Start();
}

void KouretesAllConcurrentStatechartStatechart::Stop () {
	_statechart->Stop();
}

