#include "FormationGenerator.h"

using namespace KMath;
using namespace FormationConsts;
using namespace FormationParameters;

FormationGenerator::FormationGenerator() {
}

FormationGenerator::~FormationGenerator() {
	delete formation;
	formation = 0;
}

posInfo FormationGenerator::findRoleInfo(FormationParameters::Role role) {
	for(unsigned int i = 0 ; i < formation->size() ; i++) {
		if(formation->at(i).role == role)
			return formation->at(i);
	}
}

void FormationGenerator::Init(unsigned int teamPlayers) {
	positions = teamPlayers;
	formation = new vector<posInfo>(teamPlayers);
}

void FormationGenerator::XmlInitFormation(bool kickOff) {
	
	std::string formationCase = (kickOff == true) ? "KickOff" : "noKickOff";

	for(unsigned int r = 0 ; r < positions ; r++) { // for each robot on the node
		formation->at(r).X = atof(Configurator::Instance().findValueForKey("playerConfig."+formationCase+".player~"+_toString(r)+".x").c_str());
		formation->at(r).Y = atof(Configurator::Instance().findValueForKey("playerConfig."+formationCase+".player~"+_toString(r)+".y").c_str());
		formation->at(r).role = (Role)atoi(Configurator::Instance().findValueForKey("playerConfig."+formationCase+".player~"+_toString(r)+".role").c_str());
	}
}

void FormationGenerator::DynamicInitFormation(bool kickOff) { // assuming ball is on the center of the field (0,0)

	unsigned int pos = 0;
		
	for(unsigned int i = 0 ; i < roles() ; i++) {
		
		if(pos == positions)
			break;
		
		if(i == GOALIE) { // GOALIE
			formation->at(pos).X  = Field.MinX;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			pos++;
		}
		else if(i == DEFENDER && kickOff) { // DEFENDER
			formation->at(pos).X  = DEFENDER_FACTOR*Field.MinX;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			pos++;
		}
		else if(i == DEFENDER_L && !kickOff) { // LEFT DEFENDER
			formation->at(pos).X = DEFENDER_FACTOR*Field.MinX;
			formation->at(pos).Y = LEFT_DEFENDER_OFFSET;
			formation->at(pos).role = (Role)i;
			pos++;
		}
		else if(i == DEFENDER_R && !kickOff) { // RIGHT DEFENDER
			formation->at(pos).X = DEFENDER_FACTOR*Field.MinX;
			formation->at(pos).Y = RIGHT_DEFENDER_OFFSET;
			formation->at(pos).role = (Role)i;
			pos++;
		}
		else if(i == SUPPORTER && ((kickOff && positions == 4) || (!kickOff && positions == 5)) ) { // SUPPORT
			formation->at(pos).X = (ONBALL_OFFSET - Field.DiameterCCircle/2) + Field.MinX*SUPPORT_FACTOR_X;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			pos++;
		}
		else if(i == SUPPORTER_L && kickOff && positions == 5) { // LEFT SUPPORTER
			formation->at(pos).X = ONBALL_OFFSET + Field.MinX*SUPPORT_FACTOR_X;
			formation->at(pos).Y = SUPPORT_FACTOR_Y*Field.MaxX;
			formation->at(pos).role = (Role)i;
			pos++;
		}
		else if(i == SUPPORTER_R && kickOff && positions == 5) { // RIGHT SUPPORTER
			formation->at(pos).X = ONBALL_OFFSET + Field.MinX*SUPPORT_FACTOR_X;
			formation->at(pos).Y = SUPPORT_FACTOR_Y*Field.MinX;
			formation->at(pos).role = (Role)i;
			pos++;
		}
		else if(i == ONBALL && kickOff) { // ON BALL and Kick Off
			formation->at(pos).X  = ONBALL_OFFSET;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			pos++;
		}
		else if(i == ONBALL && !kickOff) { // ONBALL and no Kick Off
			formation->at(pos).X  = ONBALL_OFFSET - Field.DiameterCCircle/2;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			pos++;
		}
	}
}

void FormationGenerator::Generate(float ballX, float ballY, bool ballFound) { // direction is ALWAYS on the negative axis
	
	// determine if we are on offensive, defensive or static formation
	if(ballFound == true) {
		
		if(ballX > Field.MaxX || ballX < Field.MinX || ballY > Field.MaxY || ballY < Field.MinY) { // in that case there is no use to generate a formation
			return;
		}
		
		if(ballX >= 0)
			formationType = OFFENSIVE;
		else if(ballX < 0)
			formationType = DEFENSIVE;
	}
	else {
		formationType = STATIC;
	}
	
	if(formationType == OFFENSIVE) { // OFFENSIVE FORMATION

		for(unsigned int i = 0 ; i < formation->size() ; i++) {
			
			if(formation->at(i).role == ONBALL) {
				
				formation->at(i).X = ballX + ONBALL_OFFSET;
				formation->at(i).Y = ballY;

				// check if the position is out of field bounds on both axis!
				if(formation->at(i).X >= Field.MaxX)
					formation->at(i).X = Field.MaxX - 0.1;
				else if(formation->at(i).X <= Field.MinX)
					formation->at(i).X = Field.MinX + 0.1;

				if(formation->at(i).Y >= Field.MaxY)
					formation->at(i).Y = Field.MaxY - 0.1;
				else if(formation->at(i).Y <= Field.MinY)
					formation->at(i).Y = Field.MinY + 0.1;
			}
			else if(formation->at(i).role == SUPPORTER_L || (formation->at(i).role == SUPPORTER && positions == 4) || formation->at(i).role == DEFENDER_L) {

				if(positions == 5) {					
					formation->at(i).role = SUPPORTER_L;

					// we are inside the basic lane so follow the attacker
					if(ballY >= MIDDLE_LANE_FACTOR*Field.MinY && ballY <= MIDDLE_LANE_FACTOR*Field.MaxY) { // ball is located inside the basic lane

						formation->at(i).X = ballX + SUPPORT_FACTOR_X*Field.MinX;
						formation->at(i).Y = func1ByAbsX(formation->at(i).X);

						if(formation->at(i).Y > SUPPORT_FACTOR_Y*Field.MaxY) // support bound!
							formation->at(i).Y = SUPPORT_FACTOR_Y*Field.MaxY;
					}
					else if(ballY > MIDDLE_LANE_FACTOR*Field.MaxY && ballX >= SUPPORT_BOUND*Field.MaxX) { // we are above basic lane on the upper right corner side
						formation->at(i).X = ballX;
						formation->at(i).Y = 0;
					}
					else if(ballY > MIDDLE_LANE_FACTOR*Field.MaxY && ballX < SUPPORT_BOUND*Field.MaxX) { // we are above basic lane and behind the penalty dot (left corner side)
						formation->at(i).X = ballX + SUPPORT_FORWARDING_FACTOR*Field.MaxX;
						formation->at(i).Y = 0;
					}
					else if(ballY < MIDDLE_LANE_FACTOR*Field.MinY && ballX >= SUPPORT_BOUND*Field.MaxX) { // we are below basic lane on the lower right corner side
						formation->at(i).X = ballX + SUPPORT_FACTOR_X*Field.MinX;
						formation->at(i).Y = Field.RightPenaltyAreaMaxY;
					}
					else if(ballY < MIDDLE_LANE_FACTOR*Field.MinY && ballX < SUPPORT_BOUND*Field.MaxX) { // we are below basic lane on the lower left corner side
						formation->at(i).X = ballX;
						formation->at(i).Y = 0;
					}
				}
				else if(positions == 4) {
					formation->at(i).role = SUPPORTER;

					// we are inside the basic lane so follow the attacker
					if(ballY >= MIDDLE_LANE_FACTOR*Field.MinY && ballY <= 0) { // when ball is located inside the basic lane : case 1
						formation->at(i).X = ballX;
						formation->at(i).Y = Field.RightPenaltyAreaMaxY;
					}
					else if(ballY > 0 && ballY <= MIDDLE_LANE_FACTOR*Field.MaxY) { // when ball is located inside the basic lane : case 2
						formation->at(i).X = ballX;
						formation->at(i).Y = Field.RightPenaltyAreaMinY;
					}
					else if(ballY > MIDDLE_LANE_FACTOR*Field.MaxY && ballX >= SUPPORT_BOUND*Field.MaxX) { // we are above basic lane on the upper right corner side
						formation->at(i).X = ballX + SUPPORT_FORWARDING_FACTOR*Field.MinX;
						formation->at(i).Y = Field.RightPenaltyAreaMinY;
					}
					else if(ballY > MIDDLE_LANE_FACTOR*Field.MaxY && ballX < SUPPORT_BOUND*Field.MaxX) { // we are above basic lane and behind the penalty dot (left corner side)
						formation->at(i).X = ballX + SUPPORT_FORWARDING_FACTOR*Field.MaxX;
						formation->at(i).Y = 0;
					}
					else if(ballY < MIDDLE_LANE_FACTOR*Field.MinY && ballX >= SUPPORT_BOUND*Field.MaxX) { // we are below basic lane on the lower right corner side
						formation->at(i).X = ballX + SUPPORT_FORWARDING_FACTOR*Field.MinX;
						formation->at(i).Y = Field.RightPenaltyAreaMaxY;
					}
					else if(ballY < MIDDLE_LANE_FACTOR*Field.MinY && ballX < SUPPORT_BOUND*Field.MaxX) { // we are below basic lane on the lower left corner side
						formation->at(i).X = ballX + SUPPORT_FORWARDING_FACTOR*Field.MaxX;
						formation->at(i).Y = 0;
					}
				}

				// check if the position is out of field bounds on both axis!
				if(formation->at(i).X >= Field.MaxX)
					formation->at(i).X = Field.MaxX - 0.1;
				else if(formation->at(i).X <= Field.MinX)
					formation->at(i).X = Field.MinX + 0.1;

				if(formation->at(i).Y >= Field.MaxY)
					formation->at(i).Y = Field.MaxY - 0.1;
				else if(formation->at(i).Y <= Field.MinY)
					formation->at(i).Y = Field.MinY + 0.1;
				
			}
			else if((formation->at(i).role == SUPPORTER_R || formation->at(i).role == SUPPORTER) && positions == 5) {
				
				formation->at(i).role = SUPPORTER_R;

				// we are inside the basic lane so follow the attacker
				if(ballY >= MIDDLE_LANE_FACTOR*Field.MinY && ballY <= MIDDLE_LANE_FACTOR*Field.MaxY) { // ball is located inside the basic lane

					formation->at(i).X = ballX + SUPPORT_FACTOR_X*Field.MinX;
					formation->at(i).Y = -func1ByAbsX(formation->at(i).X);

					if(formation->at(i).Y < SUPPORT_FACTOR_Y*Field.MinY) // support bound!
						formation->at(i).Y = SUPPORT_FACTOR_Y*Field.MinY;
				}
				else if(ballY > MIDDLE_LANE_FACTOR*Field.MaxY && ballX >= SUPPORT_BOUND*Field.MaxX) { // we are above basic lane on the upper right corner side
					formation->at(i).X = ballX + SUPPORT_FACTOR_X*Field.MinX;
					formation->at(i).Y = Field.RightPenaltyAreaMinY;;
				}
				else if(ballY > MIDDLE_LANE_FACTOR*Field.MaxY && ballX < SUPPORT_BOUND*Field.MaxX) { // we are above basic lane and behind the penalty dot (left corner side)
					formation->at(i).X = ballX;
					formation->at(i).Y = 0;
				}
				else if(ballY < MIDDLE_LANE_FACTOR*Field.MinY && ballX >= SUPPORT_BOUND*Field.MaxX) { // we are below basic lane on the lower right corner side
					formation->at(i).X = ballX;
					formation->at(i).Y = 0;
				}
				else if(ballY < MIDDLE_LANE_FACTOR*Field.MinY && ballX < SUPPORT_BOUND*Field.MaxX) { // we are below basic lane on the lower left corner side
					formation->at(i).X = ballX + SUPPORT_FORWARDING_FACTOR*Field.MaxX;
					formation->at(i).Y = 0;
				}

				// check if the position is out of field bounds on both axis!
				if(formation->at(i).X >= Field.MaxX)
					formation->at(i).X = Field.MaxX - 0.1;
				else if(formation->at(i).X <= Field.MinX)
					formation->at(i).X = Field.MinX + 0.1;

				if(formation->at(i).Y >= Field.MaxY)
					formation->at(i).Y = Field.MaxY - 0.1;
				else if(formation->at(i).Y <= Field.MinY)
					formation->at(i).Y = Field.MinY + 0.1;
				
			}
			else if(formation->at(i).role == DEFENDER || formation->at(i).role == DEFENDER_R) {
				
				formation->at(i).role = DEFENDER;

				if(ballY >= 0) {
					formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
					formation->at(i).Y = -DEFENDER_MOVEMENT;
				}
				else if(ballY < 0) {
					formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
					formation->at(i).Y = DEFENDER_MOVEMENT;
				}
			}
			else if(formation->at(i).role == GOALIE) {
				
				formation->at(i).X  = Field.MinX;
				formation->at(i).Y = 0;		
			}
		}
	}
	else if(formationType == DEFENSIVE) { // DEFENSIVE

		for(unsigned int i = 0 ; i < formation->size() ; i++) {

			if(formation->at(i).role == ONBALL) {

				formation->at(i).X = ballX + ONBALL_OFFSET;
				formation->at(i).Y = ballY;

				// check if on ball position is on our penalty area
				if(formation->at(i).X < Field.LeftPenaltyAreaMaxX && formation->at(i).Y < Field.LeftPenaltyAreaMaxY && formation->at(i).Y > Field.LeftPenaltyAreaMinX)
					formation->at(i).X = Field.LeftPenaltyAreaMaxX;

				// check if the position is out of Field bounds on both axis!
				if(formation->at(i).X >= Field.MaxX)
					formation->at(i).X = Field.MaxX - 0.1;
				else if(formation->at(i).X <= Field.MinX)
					formation->at(i).X = Field.MinX + 0.1;

				if(formation->at(i).Y >= Field.MaxY)
					formation->at(i).Y = Field.MaxY - 0.1;
				else if(formation->at(i).Y <= Field.MinY)
					formation->at(i).Y = Field.MinY + 0.1;

			}
			else if((formation->at(i).role == SUPPORTER_R || formation->at(i).role == SUPPORTER) && positions == 5) { // used only on 5 players team
				
				formation->at(i).role = SUPPORTER;

				// if the player is between the left and right goal posts
				if(ballY <= Field.LeftPenaltyAreaMaxY && ballY >= Field.LeftPenaltyAreaMinY) {
					formation->at(i).X = ballX + SUPPORT_DEFENCE_FACTOR*Field.MaxX;
					formation->at(i).Y = ballY;

					// check if on ball position is on our penalty area
					if(formation->at(i).X < Field.LeftPenaltyAreaMaxX && formation->at(i).Y < Field.LeftPenaltyAreaMaxY && formation->at(i).Y > Field.LeftPenaltyAreaMinX)
						formation->at(i).X = Field.LeftPenaltyAreaMaxX;
				}
				else if(ballY > Field.LeftPenaltyAreaMaxY) { // if the player is above the left goal post
					formation->at(i).X = ballX + SUPPORT_DEFENCE_FACTOR*Field.MaxX;
					formation->at(i).Y = Field.LeftPenaltyAreaMaxY;
				}
				else if(ballY < Field.LeftPenaltyAreaMinY) { // if the player is below the right goal post
					formation->at(i).X = ballX + SUPPORT_DEFENCE_FACTOR*Field.MaxX;
					formation->at(i).Y = Field.LeftPenaltyAreaMinY;
				}

				// check if the position is out of field bounds on both axis!
				if(formation->at(i).X >= Field.MaxX)
					formation->at(i).X = Field.MaxX - 0.1;
				else if(formation->at(i).X <= Field.MinX)
					formation->at(i).X = Field.MinX + 0.1;

				if(formation->at(i).Y >= Field.MaxY)
					formation->at(i).Y = Field.MaxY - 0.1;
				else if(formation->at(i).Y <= Field.MinY)
					formation->at(i).Y = Field.MinY + 0.1;

			}
			else if(formation->at(i).role == SUPPORTER_L || (formation->at(i).role == SUPPORTER && positions == 4)|| formation->at(i).role == DEFENDER_L) { // LEFT DEFENDER
				
				formation->at(i).role = DEFENDER_L;

				if(ballY > Field.LeftPenaltyAreaMaxY) { // ball is above the left goal post
					
					if(Field.MaxX >= 4.5) // for bigger fields add an offset to X axis
						formation->at(i).X = DEFENDER_FACTOR*Field.MinX - (ballY - Field.LeftPenaltyAreaMaxY)/25;
					else
						formation->at(i).X = DEFENDER_FACTOR*Field.MinX;

					formation->at(i).Y = 0.55 - (ballY - Field.LeftPenaltyAreaMaxY)/(2*Field.MaxY - 1);
					
					// check if on ball position is on our penalty area
					if(formation->at(i).X < Field.LeftPenaltyAreaMaxX && formation->at(i).Y < Field.LeftPenaltyAreaMaxY && formation->at(i).Y > Field.LeftPenaltyAreaMinX)
						formation->at(i).X = Field.LeftPenaltyAreaMaxX;

					// bound the defender on y axis, so that never go beside zero (0)
					if(formation->at(i).Y < 0)
						formation->at(i).Y = 0;
				}
				else {
					formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
					formation->at(i).Y = 0.55;
				}
			}
			else if(formation->at(i).role == DEFENDER || formation->at(i).role == DEFENDER_R) {
				
				formation->at(i).role = DEFENDER_R;

				if(ballY < Field.LeftPenaltyAreaMinY) { // ball is below the right goal post

					if(Field.MaxX >= 4.5) // for bigger fields add an offset to X axis
						formation->at(i).X = DEFENDER_FACTOR*Field.MinX + (ballY - Field.LeftPenaltyAreaMinY)/25;
					else
						formation->at(i).X = DEFENDER_FACTOR*Field.MinX;

					formation->at(i).Y = -0.55 - (ballY - Field.LeftPenaltyAreaMinY)/(2*Field.MaxY - 1);

					// check if on ball position is on our penalty area
					if(formation->at(i).X < Field.LeftPenaltyAreaMaxX && formation->at(i).Y < Field.LeftPenaltyAreaMaxY && formation->at(i).Y > Field.LeftPenaltyAreaMinX)
						formation->at(i).X = Field.LeftPenaltyAreaMaxX;

					// bound the defender on y axis, so that never go beside zero (0)
					if(formation->at(i).Y > 0)
						formation->at(i).Y = 0;
				}
				else {
					formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
					formation->at(i).Y = -0.55;
				}
			}
			else if(formation->at(i).role == GOALIE) {

				 if(ballY < Field.LeftPenaltyAreaMinY) {
					 if(ballX >= SUPPORT_BOUND*Field.MinX) { // ball is on the lower right corner
						 formation->at(i).X = Field.MinX - (ballY - Field.LeftPenaltyAreaMinY)/(2*Field.MaxY);
						 formation->at(i).Y = (ballY - Field.LeftPenaltyAreaMinY)/(Field.MaxY);
					 }
					 else {
						 formation->at(i).X = Field.MinX + 0.2;
						 formation->at(i).Y = -0.6;
					 }
				}
				else if(ballY > Field.LeftPenaltyAreaMaxY) {
					if(ballX >= SUPPORT_BOUND*Field.MinX) { // ball is on the upper right corner
						 formation->at(i).X = Field.MinX + (ballY - Field.LeftPenaltyAreaMaxY)/(2*Field.MaxY);
						 formation->at(i).Y = (ballY - Field.LeftPenaltyAreaMaxY)/(Field.MaxY);
					 }
					 else {
						 formation->at(i).X = Field.MinX + 0.2;
						 formation->at(i).Y = 0.6;
					 }
				}
				else {
					formation->at(i).X = Field.MinX;
					formation->at(i).Y = 0;
				}
			}
		}
	}
	else if(formationType == STATIC) { // TODO
	}

}

