#include "MyActivitySChart.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<MyActivitySChartStatechart>::Type temp("MyActivitySChartStatechart");
}
	
MyActivitySChartStatechart::MyActivitySChartStatechart(Narukom* com) {

	
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

	OrState* Node_0_2_1_2 = new OrState ( "Node_MyActivity_forever_", Node_0_2_1 );	//Name:MyActivity_forever_
	_states.push_back( Node_0_2_1_2 );

	StartState* Node_0_2_1_2_1 = new StartState ( "Node_0_2_1_2_1", Node_0_2_1_2 ); //Name:0.2.1.2.1
	_states.push_back( Node_0_2_1_2_1 );

			
	
	
	

	IActivity* NodeActivInst_0_2_1_2_2 = ActivityFactory::Instance()->CreateObject( "MyActivity" );
	_activities.push_back( NodeActivInst_0_2_1_2_2 );			
	BasicState* Node_0_2_1_2_2 = new BasicState( "Node_MyActivity", Node_0_2_1_2, NodeActivInst_0_2_1_2_2 ); //Name:MyActivity
	_states.push_back( Node_0_2_1_2_2 );

	EndState* Node_0_2_1_3 = new EndState ( "Node_0_2_1_3", Node_0_2_1 ); //Name:0.2.1.3
	_states.push_back( Node_0_2_1_3 );

	OrState* Node_0_2_2 = new OrState ( "Node_0_2_2", Node_0_2 );	//Name:0.2.2
	_states.push_back( Node_0_2_2 );

	StartState* Node_0_2_2_1 = new StartState ( "Node_0_2_2_1", Node_0_2_2 ); //Name:0.2.2.1
	_states.push_back( Node_0_2_2_1 );

	OrState* Node_0_2_2_2 = new OrState ( "Node_MyActivity_forever_", Node_0_2_2 );	//Name:MyActivity_forever_
	_states.push_back( Node_0_2_2_2 );
	
	//-------------------------------------------------------------
		
	StartState* Node_0_2_2_2_1 = new StartState ( "Node_0_2_2_2_1", Node_0_2_2_2 ); //Name:0.2.1.2.1
	_states.push_back( Node_0_2_2_2_1 );

			

	IActivity* NodeActivInst_0_2_2_2_2 = ActivityFactory::Instance()->CreateObject( "MyActivity" );
	_activities.push_back( NodeActivInst_0_2_2_2_2 );
	
	
	BasicState* Node_0_2_2_2_2 = new BasicState( "Node_MyActivity", Node_0_2_2_2, NodeActivInst_0_2_2_2_2 ); //Name:MyActivity
	_states.push_back( Node_0_2_2_2_2 );
	
	
	//-----------------------------------------------------------------------------

	EndState* Node_0_2_2_3 = new EndState ( "Node_0_2_2_3", Node_0_2_2 ); //Name:0.2.2.3
	_states.push_back( Node_0_2_2_3 );

	EndState* Node_0_3 = new EndState ( "Node_0_3", Node_0 ); //Name:0.3
	_states.push_back( Node_0_3 );
	
	
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3    ) ); //0.2TO0.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2_1,Node_0_2_1_2_2    ) ); //0.2.1.2.1TO0.2.1.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2,Node_0_2_2_3    ) ); //0.2.2.2TO0.2.2.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_1,Node_0_2_2_2    ) ); //0.2.2.1TO0.2.2.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2,Node_0_2_1_3    ) ); //0.2.1.2TO0.2.1.3
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_1,Node_0_2_1_2    ) ); //0.2.1.1TO0.2.1.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2    ) ); //0.1TO0.2
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_1_2_2,Node_0_2_1_2_2    ) ); //0.2.1.2.2TO0.2.1.2.2
	
	
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2_1,Node_0_2_2_2_2    ) ); //0.2.1.2.1TO0.2.1.2.2	
	
	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2_2_2_2,Node_0_2_2_2_2    ) ); //0.2.1.2.2TO0.2.1.2.2
	
}

MyActivitySChartStatechart::~MyActivitySChartStatechart() {
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

void MyActivitySChartStatechart::Start () {
	_statechart->Start();
}

void MyActivitySChartStatechart::Stop () {
	_statechart->Stop();
}

	