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

posInfo FormationGenerator::findRoleInfo(unsigned int complexRole) {
	
	unsigned int role = atoi( (_toString(complexRole).substr(0, 1)).c_str() );
	unsigned int id = atoi( (_toString(complexRole).substr(1, 1)).c_str() );;
	
	for(unsigned int i = 0 ; i < formation->size() ; i++) {
		if(formation->at(i).role == (FormationParameters::Role)role && formation->at(i).id == id)
			return formation->at(i);
	}
}

posInfo FormationGenerator::findRoleInfo(FormationParameters::Role role, unsigned int id) {
	
	for(unsigned int i = 0 ; i < formation->size() ; i++) {
		if(formation->at(i).role == role && formation->at(i).id == id)
			return formation->at(i);
	}
}

void FormationGenerator::calculatePositions() {

	if(positions == 4) {
		onBallPos = suplPos = defPos = 1;
		suprPos = 0;
	}
	else if( (positions-1) % 4 == 0 ) {
		onBallPos = suplPos = suprPos = defPos = (positions - 1)/4;
	}
	else {
		onBallPos = suplPos = suprPos = defPos = floor((positions - 1)/4);
		unsigned int rest = (positions - 1) - onBallPos - suplPos - suprPos - defPos;
		if(rest == 1)
			defPos++;
		else if(rest == 2) {
			defPos++; suplPos++;
		}
		else {
			defPos++; suplPos++; suprPos++;
		}
	}

	LogEntry(LogLevel::Info, "Behavior") << "DEF:" << defPos;
	LogEntry(LogLevel::Info, "Behavior") << "SUPL:" << suplPos;
	LogEntry(LogLevel::Info, "Behavior") << "SUPR:" << suprPos;
	LogEntry(LogLevel::Info, "Behavior") << "ATT:" << onBallPos;
}

void FormationGenerator::multiplyPositions(int nextI, int maxI, bool reverse, FormationParameters::Role role) {
	
	int pos = maxI - nextI;
	
	if(pos > 0 && pos <= 6)
		pos = 1;
	else
		return;
	
	for(int it = nextI ; it < maxI ; it++) {

		formation->at(it).role =  role;
		formation->at(it).id = pos;
		
		if(pos == 1 && reverse == false) {
			formation->at(it).X = formation->at(nextI - 1).X + 2*fabs(ONBALL_OFFSET)*cos(M_PI_2 + M_PI/4);
			formation->at(it).Y = formation->at(nextI - 1).Y + 2*fabs(ONBALL_OFFSET)*sin(M_PI_2 + M_PI/4);
		}
		else if(pos == 1 && reverse == true) {
			formation->at(it).X = formation->at(nextI - 1).X + 2*fabs(ONBALL_OFFSET)*cos(M_PI_2 - M_PI/4);
			formation->at(it).Y = formation->at(nextI - 1).Y + 2*fabs(ONBALL_OFFSET)*sin(M_PI_2 - M_PI/4);
		}
		else if(pos == 2 && reverse == false) {
			formation->at(it).X = formation->at(nextI - 1).X + 2*fabs(ONBALL_OFFSET)*cos(M_PI + M_PI/4);
			formation->at(it).Y = formation->at(nextI - 1).Y + 2*fabs(ONBALL_OFFSET)*sin(M_PI + M_PI/4);
		}
		else if(pos == 2 && reverse == true) {
			formation->at(it).X = formation->at(nextI - 1).X + 2*fabs(ONBALL_OFFSET)*cos(3*M_PI_2 + M_PI/4);
			formation->at(it).Y = formation->at(nextI - 1).Y + 2*fabs(ONBALL_OFFSET)*sin(3*M_PI_2 + M_PI/4);
		}
		else if(pos == 3) {
			formation->at(it).X = formation->at(nextI - 1).X + 2*fabs(ONBALL_OFFSET)*cos(M_PI_2);
			formation->at(it).Y = formation->at(nextI - 1).Y + 2*fabs(ONBALL_OFFSET)*sin(M_PI_2);
		}
		else if(pos == 4) {
			formation->at(it).X = formation->at(nextI - 1).X + 2*fabs(ONBALL_OFFSET)*cos(3*M_PI_2);
			formation->at(it).Y = formation->at(nextI - 1).Y + 2*fabs(ONBALL_OFFSET)*sin(3*M_PI_2);
		}
		else if(pos == 5 && reverse == false) {
			formation->at(it).X = formation->at(nextI - 1).X + 2*fabs(ONBALL_OFFSET)*cos(M_PI);
			formation->at(it).Y = formation->at(nextI - 1).Y + 2*fabs(ONBALL_OFFSET)*sin(M_PI);
		}
		else if(pos == 5 && reverse == true) {
			formation->at(it).X = formation->at(nextI - 1).X + 2*fabs(ONBALL_OFFSET)*cos(0);
			formation->at(it).Y = formation->at(nextI - 1).Y + 2*fabs(ONBALL_OFFSET)*sin(0);
		}
		
		pos++;
	}
}

void FormationGenerator::Init(unsigned int requestedPositions) {
	std::cout << requestedPositions << std::endl;
	if(requestedPositions > MAX_POSITIONS)
		positions = MAX_POSITIONS;
	else if(requestedPositions < MIN_POSITIONS)
		positions = MIN_POSITIONS;
	else
		positions = requestedPositions;
	
	formation = new vector<posInfo>(positions);
	calculatePositions();
}

void FormationGenerator::XmlInitFormation(bool kickOff) {
	
	if(kickOff)
		formationType = OFFENSIVE;
	else
		formationType = DEFENSIVE;
	
	std::string formationCase = (kickOff == true) ? "KickOff" : "noKickOff";
	
	for(unsigned int r = 0 ; r < positions ; r++) { // for each robot on the node
		formation->at(r).X = atof(Configurator::Instance().findValueForKey("playerConfig."+formationCase+".player~"+_toString(r)+".x").c_str());
		formation->at(r).Y = atof(Configurator::Instance().findValueForKey("playerConfig."+formationCase+".player~"+_toString(r)+".y").c_str());
		formation->at(r).role = (Role)atoi(Configurator::Instance().findValueForKey("playerConfig."+formationCase+".player~"+_toString(r)+".role").c_str());
		formation->at(r).id = 0;
	}
}

void FormationGenerator::DynamicInitFormation(bool kickOff) { // assuming ball is on the center of the field (0,0)

	unsigned int pos = 0;
		
	if(kickOff)
		formationType = OFFENSIVE;
	else
		formationType = DEFENSIVE;
		
	for(unsigned int i = 0 ; i < roles() ; i++) {
		
		if(pos == positions)
			break;
		
		if(i == GOALIE) { // GOALIE
			formation->at(pos).X  = Field.MinX;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			pos++;
		}
		else if(i == DEFENDER && kickOff) { // DEFENDER
			formation->at(pos).X  = DEFENDER_FACTOR*Field.MinX;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			multiplyPositions(pos + 1, pos + defPos, true, (Role)i);
			pos += defPos;
		}
		else if(i == DEFENDER_L && !kickOff) { // LEFT DEFENDER
			formation->at(pos).X = DEFENDER_FACTOR*Field.MinX;
			formation->at(pos).Y = LEFT_DEFENDER_OFFSET;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			multiplyPositions(pos + 1, pos + suplPos, true, (Role)i);
			pos += suplPos;
		}
		else if(i == DEFENDER_R && !kickOff) { // RIGHT DEFENDER
			formation->at(pos).X = DEFENDER_FACTOR*Field.MinX;
			formation->at(pos).Y = RIGHT_DEFENDER_OFFSET;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			multiplyPositions(pos + 1, pos + defPos, true, (Role)i);
			pos += defPos;
		}
		else if(i == SUPPORTER && ((kickOff && positions == 4) || (!kickOff && positions >= 5)) ) { // SUPPORT
			formation->at(pos).X = (ONBALL_OFFSET - Field.DiameterCCircle/2) + Field.MinX*SUPPORT_FACTOR_X;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			if(positions >= 5) {
				multiplyPositions(pos + 1, pos + suprPos, false, (Role)i);
				pos += suprPos;
			}
		}
		else if(i == SUPPORTER_L && kickOff && positions >= 5) { // LEFT SUPPORTER
			formation->at(pos).X = ONBALL_OFFSET + Field.MinX*SUPPORT_FACTOR_X;
			formation->at(pos).Y = SUPPORT_FACTOR_Y*Field.MaxX;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			multiplyPositions(pos + 1, pos + suplPos, false, (Role)i);
			pos += suplPos;
		}
		else if(i == SUPPORTER_R && kickOff && positions >= 5) { // RIGHT SUPPORTER
			formation->at(pos).X = ONBALL_OFFSET + Field.MinX*SUPPORT_FACTOR_X;
			formation->at(pos).Y = SUPPORT_FACTOR_Y*Field.MinX;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			multiplyPositions(pos + 1, pos + suprPos, false, (Role)i);
			pos += suprPos;
		}
		else if(i == ONBALL && kickOff) { // ON BALL and Kick Off
			formation->at(pos).X  = ONBALL_OFFSET;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			multiplyPositions(pos + 1, pos + onBallPos, false, (Role)i);
			pos += onBallPos;
		}
		else if(i == ONBALL && !kickOff) { // ONBALL and no Kick Off
			formation->at(pos).X  = ONBALL_OFFSET - Field.DiameterCCircle/2;
			formation->at(pos).Y = 0;
			formation->at(pos).role = (Role)i;
			formation->at(pos).id = 0;
			multiplyPositions(pos + 1, pos + onBallPos, false, (Role)i);
			pos += onBallPos;
		}
	}
}

void FormationGenerator::Generate(float ballX, float ballY) { // direction is ALWAYS on the negative axis
	
	if(ballX > Field.MaxX || ballX < Field.MinX || ballY > Field.MaxY || ballY < Field.MinY) { // in that case there is no use to generate a formation
		return;
	}
	
	// determine if we are on offensive or defensive formation
	if(ballX >= 0)
		formationType = OFFENSIVE;
	else if(ballX < 0)
		formationType = DEFENSIVE;
	
	if(formationType == OFFENSIVE) { // OFFENSIVE FORMATION

		for(unsigned int i = 0 ; i < formation->size() ; i++) {
			
			if(formation->at(i).role == ONBALL) {
				
				formation->at(i).X = ballX + ONBALL_OFFSET;
				formation->at(i).Y = ballY;
				formation->at(i).id = 0;
				
				// check if the position is out of field bounds on both axis!
				if(formation->at(i).X >= Field.MaxX)
					formation->at(i).X = Field.MaxX - 0.1;
				else if(formation->at(i).X <= Field.MinX)
					formation->at(i).X = Field.MinX + 0.1;

				if(formation->at(i).Y >= Field.MaxY)
					formation->at(i).Y = Field.MaxY - 0.1;
				else if(formation->at(i).Y <= Field.MinY)
					formation->at(i).Y = Field.MinY + 0.1;
				
				multiplyPositions(i + 1, i + onBallPos, false, formation->at(i).role);
				i += (onBallPos - 1);
			}
			else if(formation->at(i).role == SUPPORTER_L || (formation->at(i).role == SUPPORTER && positions == 4) || formation->at(i).role == DEFENDER_L) {

				if(positions >= 5) {					
					formation->at(i).role = SUPPORTER_L;
					formation->at(i).id = 0;

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
					formation->at(i).id = 0;

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
				
				multiplyPositions(i + 1, i + suplPos, false, formation->at(i).role);
				i += (suplPos - 1);
				
			}
			else if((formation->at(i).role == SUPPORTER_R || formation->at(i).role == SUPPORTER) && positions >= 5) {
				
				formation->at(i).role = SUPPORTER_R;
				formation->at(i).id = 0;

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
				
				multiplyPositions(i + 1, i + suprPos, false, formation->at(i).role);
				i += (suprPos - 1);
				
			}
			else if(formation->at(i).role == DEFENDER || formation->at(i).role == DEFENDER_R) {
				
				formation->at(i).role = DEFENDER;
				formation->at(i).id = 0;
				
				if(ballY >= 0) {
					formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
					formation->at(i).Y = -DEFENDER_MOVEMENT;
				}
				else if(ballY < 0) {
					formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
					formation->at(i).Y = DEFENDER_MOVEMENT;
				}
				
				multiplyPositions(i + 1, i + defPos, true, formation->at(i).role);
				i += (defPos - 1);
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
				formation->at(i).id = 0;

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
				
				multiplyPositions(i + 1, i + onBallPos, false, formation->at(i).role);
				i += (onBallPos - 1);

			}
			else if((formation->at(i).role == SUPPORTER_R || formation->at(i).role == SUPPORTER) && positions >= 5) { // used only on 5 players team
				
				formation->at(i).role = SUPPORTER;
				formation->at(i).id = 0;

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
				
				multiplyPositions(i + 1, i + suprPos, false, formation->at(i).role);
				i += (suprPos - 1);

			}
			else if(formation->at(i).role == SUPPORTER_L || (formation->at(i).role == SUPPORTER && positions == 4)|| formation->at(i).role == DEFENDER_L) { // LEFT DEFENDER
				
				formation->at(i).role = DEFENDER_L;
				formation->at(i).id = 0;
				
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
				
				multiplyPositions(i + 1, i + suplPos, true, formation->at(i).role);
				i += (suplPos - 1);
			}
			else if(formation->at(i).role == DEFENDER || formation->at(i).role == DEFENDER_R) {
				
				formation->at(i).role = DEFENDER_R;
				formation->at(i).id = 0;

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
				
				multiplyPositions(i + 1, i + defPos, true, formation->at(i).role);
				i += (defPos - 1);
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
}

