#include "robotcontroller.h"

namespace {
	ActivityRegistrar<RobotController>::Type temp("RobotController");
}
RobotController::RobotController() :
	Publisher("RobotController") {
	;
}

void RobotController::UserInit() {
	//"Initialize Robot controller"
	_com->get_message_queue()->add_publisher(this);

	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");

	gm = new GameController(&game_data, &received_data, &mx, conf.port(), conf.team_number());

	gm->StartThread();

	try {
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
		memory->insertData("button_pressed", 0);
		memory->insertData("lbumper_pressed", 0);
		memory->insertData("rbumper_pressed", 0);
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("RobotController", "Error in getting ALmemory proxy", Logger::FatalError);
	}
	gm_state.Clear();
	endwait = boost::posix_time::microsec_clock::universal_time();

	Logger::Instance().WriteMsg("RobotController", "Robot Controller Initialized", Logger::Info);
}

int RobotController::Execute() {
	static int delay;

	bool changed = false;
	mx.lock();
	if (received_data) {
		//teams[0] one team
		//teams[1] other team
		int teamindx = game_data.teams[0].teamNumber == conf.team_number() ? 0 : 1;

		new_gm_state.set_team_color(game_data.teams[teamindx].teamColour);
		new_gm_state.set_own_goal_color(game_data.teams[teamindx].goalColour);
		new_gm_state.set_game_state(game_data.state);
		new_gm_state.set_sec_game_state(game_data.secondaryState);
		new_gm_state.set_our_score(game_data.teams[teamindx].score);
		new_gm_state.set_otherteam_score(game_data.teams[teamindx == 1 ? 0 : 1].score);
		new_gm_state.set_firsthalf(game_data.firstHalf == 1);
		new_gm_state.set_kickoff(game_data.kickOffTeam == new_gm_state.team_color());
		new_gm_state.set_penalty(game_data.teams[teamindx].players[conf.player_number() - 1].penalty);
		new_gm_state.set_penalized(game_data.teams[teamindx].players[conf.player_number() - 1].penalty != 0);
		//Depreciated PLAYER_PENALISED State does not exist any more!
		new_gm_state.set_player_state((game_data.teams[teamindx].players[conf.player_number() - 1].penalty == 0) ? game_data.state : PLAYER_PENALISED);
		//TODO add time if necessary !

		received_data = false;
		mx.unlock();
		//Check if changed
		if (gm_state.game_state() != new_gm_state.game_state() || gm_state.team_color() != new_gm_state.team_color() || gm_state.own_goal_color() != new_gm_state.own_goal_color()
				|| gm_state.penalty() != new_gm_state.penalty() || gm_state.penalized() != new_gm_state.penalized() || gm_state.player_state() != new_gm_state.player_state()
				|| gm_state.sec_game_state() != new_gm_state.sec_game_state() || gm_state.our_score() != new_gm_state.our_score() || gm_state.otherteam_score()
				!= new_gm_state.otherteam_score() || gm_state.firsthalf() != new_gm_state.firsthalf() || gm_state.kickoff() != new_gm_state.kickoff()) {
			changed = true;
			gm_state.CopyFrom(new_gm_state);
		}
	} else {
		mx.unlock();
		chest_button_pressed = memory->getData("button_pressed");
		left_bumper_pressed = memory->getData("lbumper_pressed");
		right_bumper_pressed = memory->getData("rbumper_pressed");

		if ((chest_button_pressed + right_bumper_pressed + left_bumper_pressed) > 0 && (start_timer = boost::posix_time::microsec_clock::universal_time()) > endwait) {
			changed = true;
			if ((right_bumper_pressed) == 1) {
				if (gm_state.player_state() != PLAYER_PLAYING) {
					gm_state.set_kickoff((gm_state.kickoff() + 1) % 2);
				}
				memory->insertData("rbumper_pressed", 0);
			}
			if ((left_bumper_pressed) == 1) {
				if (gm_state.player_state() != PLAYER_PLAYING) {
					gm_state.set_team_color((gm_state.team_color() + 1) % 2);
				}
				memory->insertData("lbumper_pressed", 0);
			}
			if ((chest_button_pressed) == 1) {
				memory->insertData("button_pressed", 0);
				switch (gm_state.player_state()) {
				case PLAYER_INITIAL:
					gm_state.set_penalty(PENALTY_MANUAL);
					gm_state.set_game_state(STATE_PLAYING);
					gm_state.set_penalized(true);
					gm_state.set_player_state(PLAYER_PENALISED);
					break;
				case PLAYER_PLAYING:
					gm_state.set_penalty(PENALTY_MANUAL);
					gm_state.set_penalized(true);
					gm_state.set_player_state(PLAYER_PENALISED);
					break;
				case PLAYER_PENALISED:
					gm_state.set_game_state(STATE_PLAYING);
					gm_state.set_penalty(PENALTY_NONE);
					gm_state.set_penalized(false);
					gm_state.set_player_state(PLAYER_PLAYING);
					break;
				default:
					gm_state.set_game_state(PENALTY_MANUAL);
					gm_state.set_penalty(PENALTY_NONE);
					gm_state.set_penalized(false);
					gm_state.set_player_state(PLAYER_PLAYING);
				}
			}
			endwait = start_timer + boost::posix_time::milliseconds(380);
		}
	}

	if (changed) {
		sendLedUpdate();
		publish(&gm_state, "behavior");
	} else {
		if (delay++ % 100 == 0)
			sendLedUpdate();
	}

	return 0;
}

void RobotController::sendLedUpdate() {

	LedValues* chest_led = leds.add_leds();
	LedValues* rfoot_led = leds.add_leds();
	LedValues* lfoot_led = leds.add_leds();
	chest_led->set_chain("chest");
	rfoot_led->set_chain("r_foot");
	lfoot_led->set_chain("l_foot");
	if (gm_state.player_state() == PLAYER_INITIAL || gm_state.player_state() == PLAYER_FINISHED)
		chest_led->set_color("off");
	else if (gm_state.player_state() == PLAYER_READY)
		chest_led->set_color("blue");
	else if (gm_state.player_state() == PLAYER_SET)
		chest_led->set_color("yellow");
	else if (gm_state.player_state() == PLAYER_PLAYING)
		chest_led->set_color("green");
	else if (gm_state.player_state() == PLAYER_PENALISED)
		chest_led->set_color("red");
	else
		chest_led->set_color("on");

	if (gm_state.kickoff())
		rfoot_led->set_color("on");
	else
		rfoot_led->set_color("off");

	if (gm_state.team_color() == TEAM_BLUE)
		lfoot_led->set_color("blue");
	else
		lfoot_led->set_color("red");

	publish(&leds, "communication");
	leds.clear_leds();
}

bool RobotController::readConfiguration(const std::string& file_name) {
	XMLConfig config(file_name);
	conf.Clear(); //Initialize with default values in .proto
	gm_state.set_team_color(conf.default_color());

	int value;
	if (!config.QueryElement("player", value))
		Logger::Instance().WriteMsg("RobotController", "Configuration file has no player, setting to default value: " + _toString(conf.player_number()), Logger::Error);
	else
		conf.set_player_number(value);

	if (!config.QueryElement("team_number", value))
		Logger::Instance().WriteMsg("RobotController", "Configuration file has no team_number, setting to default value: " + _toString(conf.team_number()), Logger::Error);
	else
		conf.set_team_number(value);

	//If color is changed default configuration color does need to be changed
	std::string color = "blue";
	if (!config.QueryElement("default_team_color", color))
		Logger::Instance().WriteMsg("RobotController", "Configuration file has no team_color, setting to default value: " + _toString(gm_state.team_color()), Logger::Error);
	if (color == "blue")
		gm_state.set_team_color(TEAM_BLUE);
	else if (color == "red")
		gm_state.set_team_color(TEAM_RED);
	else
		Logger::Instance().WriteMsg("RobotController", "undefined color in configuration, setting to default value: " + _toString(gm_state.team_color()), Logger::Error);

	if (!config.QueryElement("gm_port", value))
		Logger::Instance().WriteMsg("RobotController", "Configuration file has no gm_port, setting to default value: " + _toString(conf.port()), Logger::Error);
	else
		conf.set_port(value);

	return true;
}
