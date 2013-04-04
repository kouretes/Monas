#ifndef SHAREDWORLDMODEL_H
#define SHAREDWORLDMODEL_H

#include "core/include/IActivity.hpp"

#include "messages/WorldInfo.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "core/messages/Network.pb.h"
#include "math.h"
#include "core/elements/math/KMat.hpp"


ACTIVITY_START

class SharedWorldModel: public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(SharedWorldModel);
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "SharedWorldModel";
	}
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();
	boost::shared_ptr<const WorldInfo>  wim;
	boost::shared_ptr<const GameStateMessage>  gsm;
	boost::shared_ptr<const KnownHosts> h;

private:
    void ReadFieldConf();
	int findClosestRobot();
	void gather_info(int count);
	void predict();
	void update(int rid);
	void updateNoObs(int rid);

	SharedWorldInfo swi;
    static const int numOfRobots = 5;

    int testCount;
    //Kalman declarations
    static const float vara = 1000.0f;
    static const float a = -0.9f;
    static const double QRdevx = 0.01f; //squared m^2/s^2...    3σ = 0.30 m/s => σ = 0.1 m/s => σ^2 = 0.01 m^2/s^2    where 30 cm/s ~ max speed and 6σ covers 99.7% of cases
    static const double QRdevy = 0.01f;
    static const double QRdevtheta = 0.49f; //squared rad^2/s^2...  max 0.7 rad/s ~ 40 deg/s;

    float dt, dtsqrd, dtBall;

    static const int dim = 3*numOfRobots + 2;

    KMath::KMat::GenMatrix<double,dim,1> State;
    KMath::KMat::GenMatrix<double,dim,dim> P, Q, F, I, temp;
    KMath::KMat::GenMatrix<double,3,dim> H1;
    KMath::KMat::GenMatrix<double,5,dim> H2;
    KMath::KMat::GenMatrix<double,dim,3> K1;
    KMath::KMat::GenMatrix<double,dim,5> K2;
    KMath::KMat::GenMatrix<double,3,3> S1, R1;
    KMath::KMat::GenMatrix<double,5,5> S2, R2;
    KMath::KMat::GenMatrix<double,3,1> y1;
    KMath::KMat::GenMatrix<double,5,1> y2;
    KMath::KMat::GenMatrix<double,2,2> rot;

/*
    static const float QBdevx = 0.45; //squared m^2/s^2... 3σ = 2 m/s => σ ~= 0.67 m/s
    static const float QBdevdx = 0.0144; //squared m^2/s^4... 3σ = 0.35 m/s^2 => σ ~= 0.12 m/s^2
    static const float QBdevy = 0.45;
    static const float QBdevdy = 0.0144;
    static const float QBdevxdx = 1.0; //to fix
    static const float QBdevydy = 1.0; //to fix
*/
//    RobotPose robotStateEst[numOfRobots], robotPrevStateEst[numOfRobots];
//	GlobalBall ballEst, prevBallEst;
	boost::posix_time::ptime last_filter_time;
	boost::posix_time::ptime last_ball_update_time;
	boost::posix_time::ptime now;
    boost::posix_time::time_duration duration;


    //End Kalman
	int idx, id;
	static const float INIT_VALUE = -111.0f;
	float robot_x[numOfRobots], robot_y[numOfRobots], robot_phi[numOfRobots];
	float ball_x[numOfRobots], ball_y[numOfRobots], ball_speed_x[numOfRobots], ball_speed_y[numOfRobots], bd[numOfRobots];

    //field size
    float fieldMaxX, fieldMinX, fieldMaxY, fieldMinY;
    static const float ballOffset = 0.5f;
};

ACTIVITY_END
#endif


