#include "robotcontroller.h"
#include "tools/XMLConfig.h"
#include <iostream>
#include <string>
#include "almodule.h"
#include "hal/robot/generic_nao/kAlBroker.h"
using std::cout;
using std::endl;
using std::string;
namespace {
    ActivityRegistrar<RobotController>::Type temp("RobotController");
}
RobotController::RobotController() : Publisher("RobotController")
{
  ;
}

void RobotController::UserInit()
{
//   cout << "Initialize Robotcontroller" << endl;
    _com->get_message_queue()->add_publisher(this);
    readConfiguration("config/team_config.xml");
    gm = new GameController(&game_data,&received_data,&mx,conf.port(),conf.team_number());
    gm->StartThread();
    gm_state.set_player(conf.player_number());
    gm_state.set_timeout(500);
    gm_state.set_topic("behavior");
    leds.set_topic("communication");
    
    // bc = new ButtonListener(pBroker,&chest_button_pressed,&left_bumper_pressed,&right_bumper_pressed,&button_mx);
  //  AL::ALModule::createModule<ButtonListener>( ((AL::ALPtr<AL::ALBroker>)KAlBroker::Instance()->GetBroker()),"ButtonListener");
    try {
            memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
            memory->insertData("button_pressed",0);
            memory->insertData("lbumper_pressed",0);
            memory->insertData("rbumper_pressed",0);


            

            //cout << "value " << chest_button_pressed << endl;

    } catch (AL::ALError& e) {
            cout << "Error in getting memory proxy" << std::endl;
    }
    cout << "end of user  init robot controller" << endl;
}

int  RobotController::extract_player_state(RoboCupGameControlData* data)
{
    int penalty  = data->teams[gm_state.team_color()].players[conf.player_number()-1].penalty;
    gm_state.set_penalized(false);
    if(data->state == STATE_PLAYING)
    {
        if(penalty != 0)
            return PLAYER_PENALISED;
    }
    return data->state;
}

int RobotController::Execute()
{
    // cout <<"add: "<< (int)(&received_data) <<  " rd: " << received_data << " cbp " << chest_button_pressed << " rfb " << right_bumper_pressed << " lfb " << left_bumper_pressed << endl;
    cout << "RC RUN" << endl;
    bool changed = false;
    mx.Lock();
    if(received_data)
    {
        //changed = true;
        int color =  game_data.teams[0].teamNumber == conf.team_number() ? game_data.teams[0].teamColour :  game_data.teams[1].teamColour ;
        int sec_state = game_data.secondaryState;
        int player_state = extract_player_state(&game_data);
        int kickoff =game_data.kickOffTeam == color;

        if(gm_state.game_state() != sec_state)
        {
            changed = true;
            gm_state.set_game_state(game_data.secondaryState);
        }
        if(gm_state.player_state() != player_state){
            changed = true;
            gm_state.set_player_state(extract_player_state(&game_data));
        }
        if(gm_state.team_color() != color)
        {
            gm_state.set_team_color(color);
            changed = true;
        }
        if(gm_state.kickoff() != kickoff)
        {
            gm_state.set_kickoff(game_data.kickOffTeam == color);
            changed = true;
        }
        received_data = false;
   //     cout << "COLOR: " << gm_state.team_color() << "st " << gm_state.game_state() << "pl " << gm_state.player_state()
   //            << "kic " << gm_state.kickoff() << "pen " << gm_state.penalized()<< endl;
        mx.Unlock();
    }
    else
    {

  
        chest_button_pressed = memory->getData("button_pressed");
        left_bumper_pressed = memory->getData("lbumper_pressed");
        right_bumper_pressed = memory->getData("rbumper_pressed");
	//cout << "chest " << chest_button_pressed << "lb " << left_bumper_pressed << "rb " << right_bumper_pressed << endl;
        mx.Unlock();
        //button_mx.Lock();
        if((chest_button_pressed+right_bumper_pressed+left_bumper_pressed) > 0)
        {
     //       cout << "Not heere " << endl;
            changed = true;
            if((right_bumper_pressed) == 1)
            {
                if(gm_state.player_state() != PLAYER_PLAYING)
                {
                    gm_state.set_kickoff( (gm_state.kickoff() + 1)%2);
                }
                memory->insertData("rbumper_pressed",0);

            }
            if((left_bumper_pressed) == 1)
            {
                 memory->insertData("lbumper_pressed",0);
                if(gm_state.player_state() != PLAYER_PLAYING)
                {
                    gm_state.set_team_color((gm_state.team_color() + 1)%2 );
                }

            }
            if((chest_button_pressed) == 1)
            {
                memory->insertData("button_pressed",0);
                switch(gm_state.player_state())
                {
                case PLAYER_INITIAL:
                    gm_state.set_player_state(PLAYER_PLAYING);
                    break;
                case PLAYER_READY:
                    gm_state.set_player_state(PLAYER_PENALISED);
                    break;
                case PLAYER_SET:
                    gm_state.set_player_state(PLAYER_PENALISED);
                    break;
                case PLAYER_PLAYING:
                    gm_state.set_player_state(PLAYER_PENALISED);
                    break;
                case PLAYER_PENALISED:
                    gm_state.set_player_state(PLAYER_PLAYING);
                    break;
                case PLAYER_LOG:
                    gm_state.set_player_state(PLAYER_READY);
                    break;
                case PLAYER_DEMO:
                    gm_state.set_player_state(PLAYER_READY);
                    break;
                case PLAYER_DEAD:
                    gm_state.set_player_state(PLAYER_READY);
                    break;
                default:
                    gm_state.set_player_state(PLAYER_READY);
                }



            //    button_mx.Unlock();
            }
            else
            {
          //      button_mx.Unlock();
            }
        }
        //button_mx.Unlock();
    }
    if(changed)
    {
        sendLedUpdate();
        publish(&gm_state);
    }

    return 0;
}

void RobotController::sendLedUpdate()
{
    LedValues* chest_led = leds.add_leds();
    LedValues* rfoot_led = leds.add_leds();
    LedValues* lfoot_led = leds.add_leds();
    chest_led->set_chain("chest");
    rfoot_led->set_chain("r_foot");
    lfoot_led->set_chain("l_foot");
    if(gm_state.player_state() == PLAYER_INITIAL || gm_state.player_state() == PLAYER_FINISHED)
        chest_led->set_color("off");
    else if(gm_state.player_state() == PLAYER_READY)
        chest_led->set_color("blue");
    else if(gm_state.player_state() == PLAYER_SET)
        chest_led->set_color("yellow");
    else if(gm_state.player_state() == PLAYER_PLAYING)
      chest_led->set_color("green");
    else if(gm_state.player_state() == PLAYER_PENALISED)
            chest_led->set_color("red");
    else
        chest_led->set_color("white");

    if(gm_state.kickoff())
        rfoot_led->set_color("on");
    else
        rfoot_led->set_color("off");

    if(gm_state.team_color() == TEAM_BLUE)
        lfoot_led->set_color("blue");
    else
        lfoot_led->set_color("red");


    publish(&leds);
    leds.clear_leds();
}

bool RobotController::readConfiguration(const std::string& file_name)
{
    XMLConfig config(file_name);
    int player = 1;
    int team_number = 15;
    bool found = false;
    found = config.QueryElement("player",player);
    if(!found)
        cout << "Configuration file has no player setting to default value: " << player << endl;
    found = config.QueryElement("team_number",team_number);
    if(!found)
        cout << " configuaration file has no team_number setting to default value: " << team_number << endl;
    string color = "blue";
    int team_color = TEAM_BLUE;
    found = config.QueryElement("team_color",color);
    if(!found)
        cout << "configuration file has no team_color setting to default value: " << color << endl;
    if(color == "blue")
        team_color = TEAM_BLUE;
    else if(color == "red")
        team_color = TEAM_RED;
    else
        cout <<    "undefined color in configuration setting to blue..." << endl;



    int port = GAMECONTROLLER_PORT;
    found = config.QueryElement("gm_port",port);
    if(!found)
        cout << "Configuration file has no gm_port setting to default value " << port << endl;
    conf.set_player_number(player);
    conf.set_port(port);
    conf.set_team_number(team_number);
    conf.set_color(team_color);
    return true;
}
