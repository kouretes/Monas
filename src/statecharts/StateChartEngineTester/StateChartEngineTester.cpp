	
#include "StateChartEngineTester.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<StateChartEngineTester>::Type temp("StateChartEngineTester");
}
	
StateChartEngineTester::StateChartEngineTester(Narukom* com) {

	
	_statechart = new Statechart ( "Node_Tester", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );

	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );

	IActivity* NodeActivInst_0_2 = ActivityFactory::Instance()->CreateObject( "Init" );
	_activities.push_back( NodeActivInst_0_2 );			
	BasicState* Node_0_2 = new BasicState( "Node_Init", Node_0, NodeActivInst_0_2 ); //Name:Init
	_states.push_back( Node_0_2 );

	OrState* Node_0_3 = new OrState ( "Node_DoNothing_one_or_more_times_", Node_0 );	//Name:DoNothing_one_or_more_times_
	_states.push_back( Node_0_3 );

	StartState* Node_0_3_1 = new StartState ( "Node_0_3_1", Node_0_3 ); //Name:0.3.1
	_states.push_back( Node_0_3_1 );

	IActivity* NodeActivInst_0_3_2 = ActivityFactory::Instance()->CreateObject( "DoNothing" );
	_activities.push_back( NodeActivInst_0_3_2 );			
	BasicState* Node_0_3_2 = new BasicState( "Node_DoNothing", Node_0_3, NodeActivInst_0_3_2 ); //Name:DoNothing
	_states.push_back( Node_0_3_2 );

	EndState* Node_0_3_3 = new EndState ( "Node_0_3_3", Node_0_3 ); //Name:0.3.3
	_states.push_back( Node_0_3_3 );

	EndState* Node_0_4 = new EndState ( "Node_0_4", Node_0 ); //Name:0.4
	_states.push_back( Node_0_4 );
	
	
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2   ) ); //StateChartEngineTester0.10.2
		

		
		
	IAction* TrActionInst_StateChartEngineTester0_20_3 = new TrAction_StateChartEngineTester0_20_3;
	_actions.push_back( TrActionInst_StateChartEngineTester0_20_3 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3  ,TrActionInst_StateChartEngineTester0_20_3 ) ); //StateChartEngineTester0.20.3
		
		
	ICondition* TrCondInst_StateChartEngineTester0_30_4 = new TrCond_StateChartEngineTester0_30_4;
	_conditions.push_back( TrCondInst_StateChartEngineTester0_30_4 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3,Node_0_4 ,TrCondInst_StateChartEngineTester0_30_4  ) ); //StateChartEngineTester0.30.4
		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_1,Node_0_3_2   ) ); //StateChartEngineTester0.3.10.3.2
		
		
	ICondition* TrCondInst_StateChartEngineTester0_3_20_3_2 = new TrCond_StateChartEngineTester0_3_20_3_2;
	_conditions.push_back( TrCondInst_StateChartEngineTester0_3_20_3_2 );

		
		
	IAction* TrActionInst_StateChartEngineTester0_3_20_3_2 = new TrAction_StateChartEngineTester0_3_20_3_2;
	_actions.push_back( TrActionInst_StateChartEngineTester0_3_20_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_2 ,TrCondInst_StateChartEngineTester0_3_20_3_2 ,TrActionInst_StateChartEngineTester0_3_20_3_2 ) ); //StateChartEngineTester0.3.20.3.2
		
		
	ICondition* TrCondInst_StateChartEngineTester0_3_20_3_3 = new TrCond_StateChartEngineTester0_3_20_3_3;
	_conditions.push_back( TrCondInst_StateChartEngineTester0_3_20_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_3 ,TrCondInst_StateChartEngineTester0_3_20_3_3  ) ); //StateChartEngineTester0.3.20.3.3
	
}

StateChartEngineTester::~StateChartEngineTester() {
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

void StateChartEngineTester::Start () {
	_statechart->Start();
}

void StateChartEngineTester::Stop () {
	_statechart->Stop();
}

	