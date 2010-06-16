#include <iostream>

#include <cstdlib>

#include "architecture/statechartEngine/Statechart.h"
#include "architecture/statechartEngine/StartState.h"
#include "architecture/statechartEngine/EndState.h"
#include "architecture/statechartEngine/OrState.h"
#include "architecture/statechartEngine/AndState.h"
#include "architecture/statechartEngine/BasicState.h"
#include "architecture/statechartEngine/Transitions/TransitionSegment.h"
#include "architecture/statechartEngine/Transitions/ConditionConnector.h"

#include "messages/planMsg.pb.h"
#include "architecture/narukom/pub_sub/tuple.h"
#include "architecture/narukom/pub_sub/publisher.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "architecture/statechartEngine/TimoutAciton.h"

// #include "StateTest/Robocup_player.h"
using namespace std;


using namespace statechart_engine;

class CondTimeout : public ICondition {
  public:
    CondTimeout(std::string var) {
      _var = var;
    }

    void UserInit() {}

    bool Eval() {
      TimeoutMsg* msg  = _blk->read_nb<TimeoutMsg>("TimeoutMsg",_var,"localhost");
      //       cout<<"Msg: "<<msg<<endl;
      if ( msg == 0 )
        return true;
      std::string time = msg->wakeup();
      if ( time == "")
        return true;
      //       std::cout<<"Condition time:"<<time<<std::endl;
      int timeout = (boost::posix_time::microsec_clock::local_time() - boost::posix_time::from_iso_string(time) ).total_microseconds();
      //       std::cout<<"Condition timeout:"<<timeout<<std::endl;
      if ( timeout > 0)
        return true;
      return false;
    }

  private:
    string _var;
};

unsigned int fibo ( unsigned int n) {
  static int x;
  x++;
  if ( n < 2 )
    return n;
  return fibo(n-1) + fibo(n-2);
}

class Print: public IActivity {

    public:
        Print ( string str ) :
            str ( str ) {
            ;
        }
        int Execute () {
            //fibo(15);
            //usleep ( (rand()%10) * 100 );
            cout << str << endl;
            return 0;
        }
        void UserInit () { }
        string GetName() {
            return "Print";
        }
    private:
        string str;
};

class PrintAction: public IAction {

    public:
        PrintAction ( string str ) :
            str ( str ) {
            ;
        }
        int Execute () {
            cout << str << endl;
            return 0;
        }
        void UserInit () { }
    private:
        string str;
};

class IncrThink: public IAction {
    public:
      IncrThink() : t(0) {}
        int Execute() {
          t++;
          return 0;
        }
        ~IncrThink() {
          cout<<"Total Think: "<<t<<endl;
        }
        void UserInit () { }

  private:
    int t;
};

class IncrExec: public IAction {
    public:
      IncrExec() :t(0) {}
        int Execute() {
            t++;
            return 0;
        }
        ~IncrExec() {
          cout<<"Total Exec: "<<t<<endl;
        }
        void UserInit () { }

  private:
    int t;
};

class IncrAcccept: public IAction {
    public:
      IncrAcccept() :t(0){}
        int Execute() {
            //cout<<"Incr Accept"<<endl;
            t++;
            return 0;
        }
        ~IncrAcccept() {
          cout<<"Total Accept: "<<t<<endl;
        }
        void UserInit () { }

  private:
    int t;
};

class SetPlan: public Publisher, public IAction  {
    public:
        SetPlan () : Publisher("SetPlan"),_planTuple(&_thePlan) { }
        int Execute() {
            //cout<<"Publish";
            _thePlan.set_counter(0);
            publish(&_thePlan,"behavior");
            //cout<<" plan!"<<endl;
            return 0;
        }
        void UserInit () {
            _blk->add_tuple( &_planTuple );
            _com->get_message_queue()->add_publisher(this);
//            _thePlan.set_publisher("think_thread");
//            _thePlan.set_topic("Plan");
//            setQueue(_com->get_message_queue());
        }
    private:
        PlanMsg _thePlan;
        Tuple _planTuple;
};

class TimeoutEvent: public IEvent {
    public:
        TimeoutEvent(int,string) {;}
};

class PlanASelected: public ICondition {
    public:
        bool Eval() {
            //usleep(200000);
            _blk->process_messages();
	    //_thePlan =0;
            _thePlan =_blk->read_nb<PlanMsg>("PlanMsg","SetPlan");
            if ( !_thePlan )
                return false;
            //cout<<"Selected plan: "<<_thePlan->counter()<<endl;
            //return _thePlan->counter()==0;
            bool ret = _thePlan->counter()==0;
            delete _thePlan;
            return ret;
        }
        void UserInit () {
            _com->get_message_queue()->add_subscriber(_blk);
            _com->get_message_queue()->subscribe("behavior",_blk,2);
        }
    private:
        PlanMsg* _thePlan;
};

class PlanBSelected: public ICondition {
    public:
        bool Eval() { return true; }
        void UserInit () { }
};

int main () {
    cout<<"Hello from main"<<endl;

    srand ( time(NULL) );

//    Statechart s;
//
//    StartState str1 ( & s );
//
//    OrState Or1 ( & s );
//
//    StartState Or1Str ( & Or1 );
//
//    ConditionConnector C1 ( &s );
//
//    TransitionSegment<State,ConditionConnector> tr1 (&str1,&C1);
//
//    TransitionSegment<ConditionConnector,State> tr2 (&C1,&Or1);
//
//    TransitionSegment<State,State> tr3 (&Or1,&str1);


//    TransitionSegment<State,State> tr1 (&str1,&Or1);
//
//    TransitionSegment<State,State> tr2 (&Or1Str,&str1);

//    cout<<"Bye from main"<<endl;

#if 1

cout<<"Before Narukom"<<endl;

    Narukom _com;

cout<<"After Narukom"<<endl;

usleep(1000000);



    Statechart rb_player( "rb_player", &_com);

    StartState pl_start ( "pl_start", &rb_player );
    EndState pl_final ( "pl_final", &rb_player );

    AndState player ( "player", &rb_player, new PrintAction ( "Player" ) );

    //______________________________________________________________________________________________________________
    //______________________________________________________________________________________________________________
    //______________________________________________________________________________________________________________

    OrState think ( "think", &player, new IncrThink () );
    StartState think_start ( "think_start", &think );

    //______________________________________________________________________________________________________________
    OrState get_objects ( "get_objects", &think, new PrintAction ( "Get Objects HState" ) );

    StartState get_objects_start ( "get_objects_start", &get_objects );

    BasicState segment ( "segment", &get_objects, new Print ( "Segment" ) );
    BasicState detect ( "detect", &get_objects, new Print ( "Detect" ) );
    BasicState publish_objs ( "publish_objs", &get_objects, new Print ( "Publish Objs" ) );

    EndState get_objects_final ( "get_objects_final", &get_objects );
    //______________________________________________________________________________________________________________



    BasicState self_localize ( "self_localize", &think, new Print ( "Self Localize" ) );
    BasicState publish_self_pos ( "publish_self_pos", &think, new Print ( "Publish Self Position" ) );

    BasicState ball_localize ( "ball_localize", &think, new Print ( "Ball Localize" ) );
    BasicState publish_ball_pos ( "publish_ball_pos", &think, new Print ( "Publish Ball Position" ) );

    BasicState select_plan ( "select_plan", &think, new Print ( "Select PLan" ) );
    SetPlan SetThePlan;//TODO
    BasicState publish_plan ( "publish_plan", &think, &SetThePlan );

    EndState think_final ( "think_final", &think );

    //______________________________________________________________________________________________________________
    //______________________________________________________________________________________________________________
    //______________________________________________________________________________________________________________



    OrState execute ( "execute", &player, new PrintAction ( "Execute HState" ) );//IncrExec () );
    StartState execute_start ( "execute_start", &execute );

    ConditionConnector execute_junction ( "execute_junction", &execute );

    //______________________________________________________________________________________________________________
    OrState planA ( "planA", &execute, new PrintAction ( "PlanA HState" ) );

    StartState planA_start ( "planA_start", &planA );

    BasicState planA_go_towards_ball ( "planA_go_towards_ball", &planA, new Print ( "PlanA Go Towards Ball" ) );
    ConditionConnector planA_junction ( "planA_junction", &planA );
    BasicState planA_kick_ball ( "planA_kick_ball", &planA, new Print ( "PlanA Kick Ball" ) );

    EndState planA_final ( "planA_final", &planA );

    //______________________________________________________________________________________________________________
    OrState planB ( "planB", &execute, new PrintAction ( "PlanB HState" ) );

    StartState planB_start ( "planB_start", &planB );

    BasicState planB_go_towards_ball ( "planB_go_towards_ball", &planB, new Print ( "PlanB Go Towards Ball" ) );
    ConditionConnector planB_junction ( "planB_junction", &planB );
    BasicState planB_pass_ball ( "planB_pass_ball", &planB, new Print ( "PlanB Pass Ball" ) );

    EndState planB_final ( "planB_final", &planB );

    //______________________________________________________________________________________________________________
    OrState planC ( "planC", &execute, new PrintAction ( "PlanC HState" ) );

    StartState planC_start ( "planC_start", &planC );

    BasicState planC_go_towards_goal ( "planC_go_towards_goal", &planC, new Print ( "PlanC Go Towards Goal" ) );
    ConditionConnector planC_junction_A ( "planC_junction_A", &planC );
    BasicState planC_go_towards_ball ( "planC_go_towards_ball", &planC, new Print ( "PlanC Go Towards Ball" ) );
    ConditionConnector planC_junction_B ( "planC_junction_B", &planC );
    BasicState planC_kick_ball ( "planC_kick_ball", &planC, new Print ( "planC Kick Ball" ) );

    EndState planC_final ( "planC_final", &planC );

    EndState execute_final ( "execute_final", &execute );



    //______________________________________________________________________________________________________________
    //______________________________________________________________________________________________________________
    //______________________________________________________________________________________________________________


    OrState accept_notices ( "accept_notices", &player,  new PrintAction ( "Accept Notices HState") );//TODO new IncrAcccept () );
    StartState accept_notices_start ( "accept_notices_start", &accept_notices );
    BasicState accept_notices_state ( "accept_notices_state", &accept_notices,  new Print ( "Accept Notices" ) );
    EndState accept_notices_final ( "accept_notices_final", &accept_notices );


    TransitionSegment<State, State> tr1( &pl_start, &player );
    //TransitionSegment<State, State> tr2( &player, &pl_final, new PrintAction ( "Statechart Problem" ) ); //, new GameOver () ); TODO

    //_____________________________________________
    //TransitionSegment<State, State> tr3( &think, &think, new TimeoutEvent ( 20, "think_timeout" ), new PrintAction ( "NextFrame Think" ) );

    TransitionSegment<State,State> tr3(&think, &think,new CondTimeout("Think_tout"),new TimeoutAction("Think_tout",3500));


    IncrThink ithink;
    TransitionSegment<State, State> tr4( &think_start, &get_objects, &ithink );

    TransitionSegment<State, State> tr5( &get_objects_start, &segment );
    TransitionSegment<State, State> tr6( &segment, &detect );
    TransitionSegment<State, State> tr7( &detect, &publish_objs );
    TransitionSegment<State, State> tr8( &publish_objs, &get_objects_final );

    TransitionSegment<State, State> tr9( &get_objects, &self_localize );
    TransitionSegment<State, State> tr10( &self_localize, &publish_self_pos );
    TransitionSegment<State, State> tr11( &publish_self_pos, &ball_localize );
    TransitionSegment<State, State> tr12( &ball_localize, &publish_ball_pos );
    TransitionSegment<State, State> tr13( &publish_ball_pos, &select_plan );
    TransitionSegment<State, State> tr14( &select_plan, &publish_plan );

    TransitionSegment<State, State> tr15( &publish_plan, &think_final );




    //_____________________________________________
    //TransitionSegment<State, State> tr16( &execute, &execute );
//            , new TimeoutEvent ( 40, "execute_timeout" ), new Print ( "NextFrame Execute" ) );

TransitionSegment<State,State> tr16(&execute, &execute,new CondTimeout("Exec_tout"),new TimeoutAction("Exec_tout",2500));

    IncrExec incexec;
    TransitionSegment<State, ConditionConnector> tr17( &execute_start, &execute_junction, &incexec );

    TransitionSegment<ConditionConnector, State> tr18( &execute_junction, &planA, new PlanASelected () );
    TransitionSegment<ConditionConnector, State> tr19( &execute_junction, &planB, new PlanBSelected () );
    TransitionSegment<ConditionConnector, State> tr20( &execute_junction, &planC );

    //------------- PLAN A ----------
    TransitionSegment<State, State> tr21( &planA_start, &planA_go_towards_ball );
    TransitionSegment<State, ConditionConnector> tr22( &planA_go_towards_ball, &planA_junction );
    TransitionSegment<ConditionConnector, State> tr23( &planA_junction, &planA_kick_ball );
    TransitionSegment<ConditionConnector, State> tr24( &planA_junction, &planA_final );
    TransitionSegment<State, State> tr25( &planA_kick_ball, &planA_final );

    //------------- PLAN B ----------
    TransitionSegment<State, State> tr26( &planB_start, &planB_go_towards_ball );
    TransitionSegment<State, ConditionConnector> tr27( &planB_go_towards_ball, &planB_junction );
    TransitionSegment<ConditionConnector, State> tr28( &planB_junction, &planB_pass_ball );
    TransitionSegment<ConditionConnector, State> tr29( &planB_junction, &planB_final );
    TransitionSegment<State, State> tr30( &planB_pass_ball, &planB_final );


    //------------- PLAN C ----------
    TransitionSegment<State, State> tr31( &planC_start, &planC_go_towards_goal );
    TransitionSegment<State, ConditionConnector> tr32( &planC_go_towards_goal, &planC_junction_A );
    TransitionSegment<ConditionConnector, State> tr33( &planC_junction_A, &planC_go_towards_ball );
    TransitionSegment<ConditionConnector, State> tr34( &planC_junction_A, &planC_final );
    TransitionSegment<State, ConditionConnector> tr35( &planC_go_towards_ball, &planC_junction_B );
    TransitionSegment<ConditionConnector, State> tr36( &planC_junction_B, &planC_kick_ball );
    TransitionSegment<ConditionConnector, State> tr37( &planC_junction_B, &planC_final );
    TransitionSegment<State, State> tr38( &planC_kick_ball, &planC_final );

    TransitionSegment<State, State> tr39( &planA, &execute_final );
    TransitionSegment<State, State> tr40( &planB, &execute_final );
    TransitionSegment<State, State> tr41( &planC, &execute_final );



    //_____________________________________________
//     TransitionSegment<State,State> tr42(&accept_notices, &accept_notices,new TimeoutEvent(80,"accept_timeout"), new PrintAction("NextFrame Comm"));

    TransitionSegment<State,State> tr42(&accept_notices, &accept_notices,new CondTimeout("Comm_tout"),new TimeoutAction("Comm_tout",1000));

    IncrAcccept incAcc;
    TransitionSegment<State, State> tr43( &accept_notices_start, &accept_notices_state,&incAcc );

    // works
    //new Transition(accept_notices_state, accept_notices_start,new TimeoutEvent(1000), new Print("NextFrame Comm"));

    // OK----
    TransitionSegment<State, State> tr44( &accept_notices_state,&accept_notices_final );



#endif

cout<<"Before Start"<<endl;

usleep ( 1000000 );

     rb_player.Start();

// while(1)
    usleep(3000000);

    rb_player.Stop();

    usleep(1000000);




  //Robocup_playerStatechart stc;

//   stc.Start();
//
//   usleep(10000000);
//   stc.Stop();

  return 0;
}
