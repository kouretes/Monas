#include "BodyBehavior.h"

namespace {
	ActivityRegistrar<BodyBehavior>::Type temp("BodyBehavior");
}

BodyBehavior::BodyBehavior() {
}

void BodyBehavior::UserInit() {
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");

	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("behavior", _blk, 0);

	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);

	amot = new MotionActionMessage();

	ballfound = 0;

	calibrated = 0;

	play = false;
	kickoff = false;

	turnforscan = true;
	scanforball = true;

	readytokick = false;
	back = 0;
	direction = 1;
	turning = false;

	obstacleFront = false;
	gameState = PLAYER_INITIAL;

	teamColor = TEAM_BLUE;
	orientation = 0;

	srand(time(0));

	Logger::Instance().WriteMsg("BodyBehavior", "Initialized", Logger::Info);
}

int BodyBehavior::Execute() {

	read_messages();

	if (hbm != 0) {
		calibrated = hbm->calibrated();
		ballfound = hbm->ballfound();
		turnforscan = hbm->turnforscan();
		scanforball = hbm->scanforball();
	}

	if (gsm != 0) {
		Logger::Instance().WriteMsg("BodyBehavior", " Player_state " + _toString(gsm->player_state()), Logger::ExtraExtraInfo);
		gameState = gsm->player_state();
		//teamColor = gsm->team_color();

		if (gameState == PLAYER_PLAYING) {
			if (calibrated == 2) {
				play = true;
				littleWalk(0.01, 0.0, 0.0, 1);
			} else if (calibrated == 0) {
				//calibrate();
			} else if (calibrated == 1) {
				// wait
			}
		} else if (gameState == PLAYER_INITIAL) {
			play = false;
		} else if (gameState == PLAYER_READY) {
			play = false;
		} else if (gameState == PLAYER_SET) {
			play = false;
			//calibrate();
			kickoff = gsm->kickoff();
			orientation = 0;
		} else if (gameState == PLAYER_FINISHED) {
			play = false;
		} else if (gameState == PLAYER_PENALISED) {
			play = false;
			//calibrate();
		}
	}

	if (gameState == PLAYER_PLAYING) {
		if (calibrated == 2) {
			play = true;
			//	littleWalk(0.01,0.0,0.0,1);
		}
		//esbhsa to calibrate()  otan calibrated==0, swsto?????
	}

	//if (play) mgltest();
	//return 1;

	if (play) {

		float X = 0.0, Y = 0.0, theta = 0.0;
		float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;

		if ((obsm != 0) && !turning) {

			if (obsm->regular_objects_size() > 0) {
				if (((obsm->regular_objects(0).object_name() == "BlueGoal") && (teamColor == TEAM_RED)) || ((obsm->regular_objects(0).object_name() == "YellowGoal") && (teamColor
						== TEAM_BLUE))) {
					double ogb = obsm->regular_objects(0).bearing();
					if (fabs(ogb) < +45 * TO_RAD) {
						orientation = 0;
					} else if (ogb > +45 * TO_RAD) {
						orientation = 1;
					} else if (ogb < -45 * TO_RAD) {
						orientation = 3;
					}
				}

				if (((obsm->regular_objects(0).object_name() == "YellowGoal") && (teamColor == TEAM_RED)) || ((obsm->regular_objects(0).object_name() == "BlueGoal") && (teamColor
						== TEAM_BLUE))) {
					double mgb = obsm->regular_objects(0).bearing();
					if (fabs(mgb) < +45 * TO_RAD) {
						orientation = 2;
					} else if (mgb > +45 * TO_RAD) {
						orientation = 3;
					} else if (mgb < -45 * TO_RAD) {
						orientation = 1;
					}
				}
				Logger::Instance().WriteMsg("BodyBehavior", "Orientation: " + _toString(orientation) + " Team Color " + _toString(teamColor), Logger::Info);
			}

			if (obsm->has_ball() && ballfound > 0) {

				int side = 1;
				bd = obsm->ball().dist();
				bb = obsm->ball().bearing();
				bx = obsm->ball().dist() * cos(obsm->ball().bearing()); //kanw tracking me to swma
				by = obsm->ball().dist() * sin(obsm->ball().bearing());
				//side = (bb > 0) ? 1 : -1;
				if (orientation == 1)
					side = -1;
				else if (orientation == 3)
					side = +1;
				else
					side = (bb > 0) ? 1 : -1;

				Logger::Instance().WriteMsg("BodyBehavior", "Measurements - Distance: " + _toString(bd) + "  Bearing: " + _toString(bb) + "  BX: " + _toString(bx) + "  BY: "
						+ _toString(by), Logger::Info);

				if (!readytokick) {
					readytokick = true;
					double gain = 1.0;
					if (bd > 0.4)
						gain = 1.5;
					double gainTheta = 0.3;
					double gainFine = 1.0;
					if (bd <= 8.0) {
						float posx = 0.13, posy = 0.035;
						if (bd > 0.25) {
							X = gain * bx;
							Y = gain * by;
							if (fabs(bb) > 3 * TO_RAD)
								theta = gainTheta * bb;
							readytokick = false;
						} else if (bd > 0.25) {
							X = gain * (bx - posx);
							Y = gain * (by - (side * posy));
							readytokick = false;
						} else {
							if (fabs(bx - posx) > 0.025) {
								X = gainFine * (bx - posx);
								readytokick = false;
							}
							if (fabs(by - (side * posy)) > 0.025) {
								Y = gainFine * (by - (side * posy));
								readytokick = false;
							}
						}
					}

					if (!readytokick) {

						if (fabs(X) > 1.0)
							X = (X > 0.0) ? 1.0 : -1.0;
						if (fabs(Y) > 1.0)
							Y = (Y > 0.0) ? 1.0 : -1.0;
						if (fabs(theta) > 1.0)
							theta = (theta > 0.0) ? 1.0 : -1.0;

						//if (om!=0) {
						//if (om->direction(1)==1) {
						//	X = 0.0;
						//}
						//else if ( (om->direction(0)==1) && (Y>0) ) {
						//	Y = 0.0;
						//}
						//else if ( (om->direction(0)==1) && (theta>0) ) {
						//	theta = 0.0;
						//	}
						//	else if ( (om->direction(2)==1) && (Y<0) ) {
						///	Y = 0.0;
						//	}
						//	else if ( (om->direction(2)==1) && (theta<0) ) {
						//	theta = 0.0;
						//}
						//}

						velocityWalk(X, Y, theta, 1.0);
					} else {
						velocityWalk(0.0, 0.0, 0.0, 1.0);
						return 1;
					}
				}
			}
		} else {
			readytokick = false;
		}

		/* Ready to take action */
		if (readytokick && !turning) {
			obstacleFront = false;
			//if (om!=0) 
			//if (om->direction(1) == 1)
			//obstacleFront = true;

			if (kickoff) {
				//if (mglRand()<0.5) {
				if ((mglRand() < 1.0) && !obstacleFront) {
					littleWalk(0.2, 0.0, 0.0, 2);
				} else {
					if (by > 0.0) {
						amot->set_command("SoftLeftSideKick");
						direction = -1;
					} else {
						amot->set_command("SoftRightSideKick");
						direction = +1;
					}
					_blk->publish_signal(*amot, "motion");
				}
				kickoff = false;
			} else {

				/* **************** Targetted Kicks ********************** */
				if (orientation == 0) {
					if (by > 0.0)
						amot->set_command("LeftKick");
					else
						amot->set_command("RightKick");
				} else if (orientation == 1) {
					amot->set_command("HardLeftSideKick");
					direction = -1;
				} else if (orientation == 3) {
					amot->set_command("HardRightSideKick");
					direction = +1;
				} else if (orientation == 2) {
					if (by > 0.0)
						amot->set_command("LeftBackKick");
					else
						amot->set_command("RightBackKick");
				} else {
					if (by > 0.0)
						amot->set_command("SoftLeftSideKick");
					else
						amot->set_command("SoftRightSideKick");
				}

				_blk->publish_signal(*amot, "motion");
				/* **************** End of Targetted Kicks ********************** */

				/* **************** Randomized Kicks ********************** */
				//if (mglRand()<0.6) {
				////if ( (mglRand()<1.0) && !obstacleFront ) {
				//if (by > 0.0)
				//amot->set_command("LeftKick");
				//else
				//amot->set_command("RightKick");
				//}
				//else if (mglRand()<0.5) {
				//if (by > 0.0) {
				//amot->set_command("HardLeftSideKick");
				//direction = -1;
				//}
				//else {
				//amot->set_command("HardRightSideKick");
				//direction = +1;
				//}
				//}
				//else {
				//if (by > 0.0)
				//amot->set_command("LeftBackKick");
				//else
				//amot->set_command("RightBackKick");
				//}
				//Publisher::publish(amot, "motion");
				//back = 0;
				/* **************** End of Randomized Kicks ********************** */
			}

			readytokick = false;
		}
		Logger::Instance().WriteMsg("Body", "rdk " + _toString(readytokick) + "turn " + _toString(turning) + "TFS " + _toString(turnforscan) + "SCFB " + _toString(scanforball), Logger::Info);

		if (!readytokick && !turning && !turnforscan && scanforball) {
			velocityWalk(0.0, 0.0, 0.0, 1.0);
		}

		if (!readytokick && !turning && turnforscan && scanforball) {
			littleWalk(0.0, 0.0, direction * 90 * TO_RAD, 5);
		}

	} else if (!play) { // Non-Play state
		velocityWalk(0.0, 0.0, 0.0, 1.0);
	}

	return 0;
}

void BodyBehavior::read_messages() {

	gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	bmsg = _blk->read_signal<BallTrackMessage> ("BallTrackMessage");
	obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
	//om   = _blk->read_signal<ObstacleMessage> ("ObstacleMessage");
	hbm = _blk->read_state<HeadBehaviorMessage> ("HeadBehaviorMessage");

	Logger::Instance().WriteMsg("BodyBehavior", "read_messages ", Logger::ExtraExtraInfo);
	boost::shared_ptr<const CalibrateCam> c = _blk->read_state<CalibrateCam> ("CalibrateCam");
	if (c != NULL) {
		if (c->status() == 1)
			calibrated = 2;
	}
}

double BodyBehavior::mglRand() {
	return rand() / double(RAND_MAX);
}

void BodyBehavior::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

void BodyBehavior::littleWalk(double x, double y, double th, int s) {
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	sleep(s);
	_blk->publish_signal(*wmot, "motion");
}

bool BodyBehavior::readConfiguration(const std::string& file_name) {
	XMLConfig config(file_name);

	int playernum = -1;
	if (!config.QueryElement("player", playernum))
		Logger::Instance().WriteMsg("BodyBehavior", "Configuration file has no player, setting to default value: " + _toString(playernum), Logger::Error);

	//If color is changed default configuration color does need to be changed
	std::string color = "blue";
	teamColor = TEAM_BLUE;
	if (!config.QueryElement("default_team_color", color))
		Logger::Instance().WriteMsg("BodyBehavior", "Configuration file has no team_color, setting to default value: " + color, Logger::Error);
	if (color == "blue")
		teamColor = TEAM_BLUE;
	else if (color == "red")
		teamColor = TEAM_RED;
	else
		Logger::Instance().WriteMsg("BodyBehavior", "Undefined color in configuration, setting to default value: " + color, Logger::Error);

	return true;
}
