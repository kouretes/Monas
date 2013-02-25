#include "FormationGenerator.h"

using std::cout;
using std::endl;
using KMath::func1ByAbsX;
using namespace FormationConsts;

FormationGenerator::FormationGenerator() { }

FormationGenerator::~FormationGenerator() { }

void FormationGenerator::Init(int teamPlayers) {

	positions = teamPlayers;
	formation = new vector<posInfo>(teamPlayers);

	for(unsigned int i = 0 ; i < formation->size() ; i++) {
		
		formation->at(i).role = (Role)i;
		
		if(i == GOALIE) { // GOALIE
			formation->at(i).X  = Field.MinX;
			formation->at(i).Y = 0;
		}
		else if(i == DEFENDER) { // DEFENDER
			formation->at(i).X  = DEFENDER_FACTOR*Field.MinX;
			formation->at(i).Y = 0;
		}
		else if(i == SUPPORTER_L) { // SUPPORT LEFT
			if(teamPlayers == 5) {
				formation->at(i).X = ONBALL_OFFSET + Field.MinX*SUPPORT_FACTOR_X;
				formation->at(i).Y = SUPPORT_FACTOR_Y*Field.MaxX;
			}
			else if(teamPlayers == 4) {
				formation->at(i).X = ONBALL_OFFSET + Field.MinX*SUPPORT_FACTOR_X;
				formation->at(i).Y = 0;
			}
		}
		else if(i == SUPPORTER_R) { // SUPPORT RIGHT
			formation->at(i).X = ONBALL_OFFSET + Field.MinX*SUPPORT_FACTOR_X;
			formation->at(i).Y = SUPPORT_FACTOR_Y*Field.MinX;
		}
		else { // ON BALL
			formation->at(i).X  = ONBALL_OFFSET;
			formation->at(i).Y = 0;
		}
	}
}

void FormationGenerator::Generate(float ballX, float ballY) { // direction is ALWAYS on the negative axis

	// determine if we are on offensive or defensive formation
	if(ballX >= 0)
		formationType = OFFENSIVE;
	else if(ballX < 0)
		formationType = DEFENSIVE;

	if(formationType == OFFENSIVE){ // OFFENSIVE

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
			else if(formation->at(i).role == SUPPORTER_L) {

				if(positions == 5) {
				
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
			else if(formation->at(i).role == SUPPORTER_R) {

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
			else if(formation->at(i).role == DEFENDER) {

				if(ballY >= 0) {
					formation->at(i).X = ballX + DEFENDER_FACTOR*Field.MinX;
					//formation->at(i).Y = -DEFENDER_MOVEMENT;
				}
				else if(ballY < 0) {
					formation->at(i).X = ballX + DEFENDER_FACTOR*Field.MinX;
					//formation->at(i).Y = DEFENDER_MOVEMENT;
				}
			}
		}
	}
	else { // DEFENSIVE

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
			else if(formation->at(i).role == SUPPORTER_R) { // used only on 5 players team
				
				// if the player is between the left and right goal posts
				if(ballY <= Field.LeftPenaltyAreaMaxY && ballY >= Field.LeftPenaltyAreaMinY && ballX > SUPPORT_BOUND*Field.MinX) {
					formation->at(i).X = ballX + SUPPORT_FACTOR_X*Field.MinX;
					formation->at(i).Y = ballY;

					// check if on ball position is on our penalty area
					if(formation->at(i).X < Field.LeftPenaltyAreaMaxX && formation->at(i).Y < Field.LeftPenaltyAreaMaxY && formation->at(i).Y > Field.LeftPenaltyAreaMinX)
						formation->at(i).X = Field.LeftPenaltyAreaMaxX;
				}
				else if(ballY <= Field.LeftPenaltyAreaMaxY && ballY >= Field.LeftPenaltyAreaMinY && ballX <= SUPPORT_BOUND*Field.MinX) {
					formation->at(i).X = ballX + SUPPORT_DEFENCE_FACTOR*Field.MaxX;
					formation->at(i).Y = ballY;
				}
				else if(ballY > Field.LeftPenaltyAreaMaxY && ballX > SUPPORT_BOUND*Field.MinX) { // if the player is above the left goal post, on the upper right corner
					formation->at(i).X = ballX + SUPPORT_FACTOR_X*Field.MinX;
					formation->at(i).Y = Field.LeftPenaltyAreaMaxY;
				}
				else if(ballY > Field.LeftPenaltyAreaMaxY && ballX < SUPPORT_BOUND*Field.MinX) { // if the player is above the left goal post, on the upper left corner
					formation->at(i).X = ballX + SUPPORT_DEFENCE_FACTOR*Field.MaxX;
					formation->at(i).Y = Field.LeftPenaltyAreaMaxY;
				}
				else if(ballY < Field.LeftPenaltyAreaMinY && ballX > SUPPORT_BOUND*Field.MinX) {
					formation->at(i).X = ballX + SUPPORT_FACTOR_X*Field.MinX;
					formation->at(i).Y = Field.LeftPenaltyAreaMinY;
				}
				else if(ballY < Field.LeftPenaltyAreaMinY && ballX < SUPPORT_BOUND*Field.MinX) {
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
			else if(formation->at(i).role == SUPPORTER_L) { // SUPPORT LEFT TO DEFENDER

				if(ballY > Field.LeftPenaltyAreaMaxY) {
					if(ballX >= SUPPORT_BOUND*Field.MinX) { // ball is on the upper right corner
						if(Field.MaxX >= 4.5)
							formation->at(i).X = DEFENDER_FACTOR*Field.MinX - (ballY - Field.LeftPenaltyAreaMaxY)/10;
						else
							formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
						formation->at(i).Y = 0.55 - (ballY - Field.LeftPenaltyAreaMaxY)/(2*Field.MaxY/3);
					}
					else {
						if(Field.MaxX >= 4.5)
							formation->at(i).X = DEFENDER_FACTOR*Field.MinX + (Field.MinX + ballX)/25;
						else
							formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
						formation->at(i).Y = 0.55 - (ballY - Field.LeftPenaltyAreaMaxY)/(2*Field.MaxY-1);
					}

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
			else if(formation->at(i).role == DEFENDER) {

				if(ballY < Field.LeftPenaltyAreaMinY) {
					if(ballX >= SUPPORT_BOUND*Field.MinX) { // ball is on the upper right corner
						if(Field.MaxX >= 4.5)
							formation->at(i).X = DEFENDER_FACTOR*Field.MinX + (ballY - Field.LeftPenaltyAreaMinY)/10;
						else
							formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
						formation->at(i).Y = -0.55 - (ballY - Field.LeftPenaltyAreaMinY)/(2*Field.MaxY/3);
					}
					else {
						if(Field.MaxX >= 4.5)
							formation->at(i).X = DEFENDER_FACTOR*Field.MinX + (Field.MinX + ballX)/25;
						else
							formation->at(i).X = DEFENDER_FACTOR*Field.MinX;
						formation->at(i).Y = -0.55 - (ballY - Field.LeftPenaltyAreaMinY)/(2*Field.MaxY-1);
					}

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
}

vector<FormationGenerator::posInfo>* FormationGenerator::getFormation() { return formation; }

