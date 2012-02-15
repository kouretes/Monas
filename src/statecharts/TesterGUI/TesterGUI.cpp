	
//namespace nsTesterGUI {
	
#include "TesterGUI.h"
#include "transitionHeaders.h"
		
using namespace statechart_engine;

namespace {
	StatechartRegistrar<TesterGUI>::Type temp("TesterGUI");
}
	
TesterGUI::TesterGUI(Narukom* com) {

	
	_statechart = new Statechart ( "Node_player", com );
	Statechart* Node_0 = _statechart;
	_states.push_back( Node_0 );

	StartState* Node_0_1 = new StartState ( "Node_0_1", Node_0 ); //Name:0.1
	_states.push_back( Node_0_1 );

	IActivity* NodeActivInst_0_2 = ActivityFactory::Instance()->CreateObject( "Init" );
	_activities.push_back( NodeActivInst_0_2 );			
	BasicState* Node_0_2 = new BasicState( "Node_Init", Node_0, NodeActivInst_0_2 ); //Name:Init
	_states.push_back( Node_0_2 );

	OrState* Node_0_3 = new OrState ( "Node__open_group_play_or_NoPlay_close_group__one_or_more_times_", Node_0 );	//Name:_gr_play_or_NoPlay_one_or_more_times_
	_states.push_back( Node_0_3 );

	StartState* Node_0_3_1 = new StartState ( "Node_0_3_1", Node_0_3 ); //Name:0.3.1
	_states.push_back( Node_0_3_1 );

	OrState* Node_0_3_2 = new OrState ( "Node__open_group_play_or_NoPlay_close_group_", Node_0_3 );	//Name:_gr_play_or_NoPlay
	_states.push_back( Node_0_3_2 );

	StartState* Node_0_3_2_1 = new StartState ( "Node_0_3_2_1", Node_0_3_2 ); //Name:0.3.2.1
	_states.push_back( Node_0_3_2_1 );

	ConditionConnector* Node_0_3_2_2  = new ConditionConnector ( "Node_0_3_2_2", Node_0_3_2 ); //Name:0.3.2.2
	_states.push_back( Node_0_3_2_2 );

	OrState* Node_0_3_2_3 = new OrState ( "Node_play", Node_0_3_2 );	//Name:play
	_states.push_back( Node_0_3_2_3 );

	StartState* Node_0_3_2_3_1 = new StartState ( "Node_0_3_2_3_1", Node_0_3_2_3 ); //Name:0.3.2.3.1
	_states.push_back( Node_0_3_2_3_1 );

	IActivity* NodeActivInst_0_3_2_3_2 = ActivityFactory::Instance()->CreateObject( "ChangeRobotPosition" );
	_activities.push_back( NodeActivInst_0_3_2_3_2 );			
	BasicState* Node_0_3_2_3_2 = new BasicState( "Node_ChangeRobotPosition", Node_0_3_2_3, NodeActivInst_0_3_2_3_2 ); //Name:ChangeRobotPosition
	_states.push_back( Node_0_3_2_3_2 );

	OrState* Node_0_3_2_3_3 = new OrState ( "Node__open_group_GoToPosition_one_or_more_times__close_group_", Node_0_3_2_3 );	//Name:_gr_GoToPosition_one_or_more_times_
	_states.push_back( Node_0_3_2_3_3 );

	StartState* Node_0_3_2_3_3_1 = new StartState ( "Node_0_3_2_3_3_1", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.1
	_states.push_back( Node_0_3_2_3_3_1 );

	IActivity* NodeActivInst_0_3_2_3_3_2 = ActivityFactory::Instance()->CreateObject( "GoToPosition" );
	_activities.push_back( NodeActivInst_0_3_2_3_3_2 );			
	BasicState* Node_0_3_2_3_3_2 = new BasicState( "Node_GoToPosition", Node_0_3_2_3_3, NodeActivInst_0_3_2_3_3_2 ); //Name:GoToPosition
	_states.push_back( Node_0_3_2_3_3_2 );

	EndState* Node_0_3_2_3_3_3 = new EndState ( "Node_0_3_2_3_3_3", Node_0_3_2_3_3 ); //Name:0.3.2.3.3.3
	_states.push_back( Node_0_3_2_3_3_3 );

	EndState* Node_0_3_2_3_4 = new EndState ( "Node_0_3_2_3_4", Node_0_3_2_3 ); //Name:0.3.2.3.4
	_states.push_back( Node_0_3_2_3_4 );

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
	
	
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_1,Node_0_2    ) ); //TesterGUI0.10.2
		

		

		
		
	IAction* TrActionInst_TesterGUI0_20_3 = new TrAction_TesterGUI0_20_3;
	_actions.push_back( TrActionInst_TesterGUI0_20_3 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_2,Node_0_3   ,TrActionInst_TesterGUI0_20_3 ) ); //TesterGUI0.20.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3,Node_0_4    ) ); //TesterGUI0.30.4
		

		

		
		
	IAction* TrActionInst_TesterGUI0_3_2_10_3_2_2 = new TrAction_TesterGUI0_3_2_10_3_2_2;
	_actions.push_back( TrActionInst_TesterGUI0_3_2_10_3_2_2 );

	_transitions.push_back( new TransitionSegment<State,ConditionConnector>(Node_0_3_2_1,Node_0_3_2_2   ,TrActionInst_TesterGUI0_3_2_10_3_2_2 ) ); //TesterGUI0.3.2.10.3.2.2
		

		
		
	ICondition* TrCondInst_TesterGUI0_3_2_20_3_2_3 = new TrCond_TesterGUI0_3_2_20_3_2_3;
	_conditions.push_back( TrCondInst_TesterGUI0_3_2_20_3_2_3 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_3  ,TrCondInst_TesterGUI0_3_2_20_3_2_3  ) ); //TesterGUI0.3.2.20.3.2.3
		

		
		
	ICondition* TrCondInst_TesterGUI0_3_2_20_3_2_4 = new TrCond_TesterGUI0_3_2_20_3_2_4;
	_conditions.push_back( TrCondInst_TesterGUI0_3_2_20_3_2_4 );

		

	_transitions.push_back( new TransitionSegment<ConditionConnector,State>(Node_0_3_2_2,Node_0_3_2_4  ,TrCondInst_TesterGUI0_3_2_20_3_2_4  ) ); //TesterGUI0.3.2.20.3.2.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_4,Node_0_3_2_5    ) ); //TesterGUI0.3.2.40.3.2.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3,Node_0_3_2_5    ) ); //TesterGUI0.3.2.30.3.2.5
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_1,Node_0_3_2_3_2    ) ); //TesterGUI0.3.2.3.10.3.2.3.2
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_2,Node_0_3_2_3_3    ) ); //TesterGUI0.3.2.3.20.3.2.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3,Node_0_3_2_3_4    ) ); //TesterGUI0.3.2.3.30.3.2.3.4
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_1,Node_0_3_2_3_3_2    ) ); //TesterGUI0.3.2.3.3.10.3.2.3.3.2
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2_3_3_2,Node_0_3_2_3_3_3    ) ); //TesterGUI0.3.2.3.3.20.3.2.3.3.3
		

		

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_1,Node_0_3_2    ) ); //TesterGUI0.3.10.3.2
		

		
		
	ICondition* TrCondInst_TesterGUI0_3_20_3_2 = new TrCond_TesterGUI0_3_20_3_2;
	_conditions.push_back( TrCondInst_TesterGUI0_3_20_3_2 );

		
		
	IAction* TrActionInst_TesterGUI0_3_20_3_2 = new TrAction_TesterGUI0_3_20_3_2;
	_actions.push_back( TrActionInst_TesterGUI0_3_20_3_2 );

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_2  ,TrCondInst_TesterGUI0_3_20_3_2 ,TrActionInst_TesterGUI0_3_20_3_2 ) ); //TesterGUI0.3.20.3.2
		

		
		
	ICondition* TrCondInst_TesterGUI0_3_20_3_3 = new TrCond_TesterGUI0_3_20_3_3;
	_conditions.push_back( TrCondInst_TesterGUI0_3_20_3_3 );

		

	_transitions.push_back( new TransitionSegment<State,State>(Node_0_3_2,Node_0_3_3  ,TrCondInst_TesterGUI0_3_20_3_3  ) ); //TesterGUI0.3.20.3.3
	
}

TesterGUI::~TesterGUI() {
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

void TesterGUI::Start () {
	_statechart->Start();
}

void TesterGUI::Stop () {
	_statechart->Stop();
}

//}

	