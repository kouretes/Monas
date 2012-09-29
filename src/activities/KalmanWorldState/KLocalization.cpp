/*
 * KLocalization.cpp
 *
 *      Author: vosk
 */
#include <time.h>
#include <math.h>
#include "KLocalization.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
using namespace boost;
using namespace KMath;
KLocalization::KLocalization()
{
}

KLocalization::~KLocalization()
{
}

int KLocalization::Initialize()
{
	XMLConfig * config = NULL;
	string filename = ArchConfig::Instance().GetConfigPrefix() + "/Localizationconf.xml" ;
	config = new XMLConfig(filename);

	if (config->IsLoadedSuccessfully())
	{
		bool found = true;
		float temp;
		///Parameters
		found &= config->QueryElement("robustmean", robustmean);
		found &= config->QueryElement("partclsNum", maxparticles);
		found &= config->QueryElement("SpreadParticlesDeviation", SpreadParticlesDeviation);
		found &= config->QueryElement("rotation_deviation", rotation_deviation);
		found &= config->QueryElement("PercentParticlesSpread", PercentParticlesSpread);
		found &= config->QueryElement("RotationDeviationAfterFallInDeg", RotationDeviationAfterFallInDeg);
		found &= config->QueryElement("NumberOfParticlesSpreadAfterFall", NumberOfParticlesSpreadAfterFall);

		if (found)
		{
			Logger::Instance().WriteMsg("Localization", "All Localization parameters loaded successfully" , Logger::Info);
		}
		else
		{
			Logger::Instance().WriteMsg("Localization", "Cant Find an attribute in the xml config file " , Logger::Error);
		}
	}
	else
	{
		Logger::Instance().WriteMsg("Localization", "Cant Find xml config file " + filename , Logger::Error);
	}

	if(config)
		delete config;

	config = NULL;
	filename = ArchConfig::Instance().GetConfigPrefix() + "/Field.xml" ;
	config = new XMLConfig(filename);

	if (config->IsLoadedSuccessfully())
	{
		bool found = true;
		found &= config->QueryElement("CarpetMaxX", CarpetMaxX);
		found &= config->QueryElement("CarpetMinX", CarpetMinX);
		found &= config->QueryElement("CarpetMaxY", CarpetMaxY);
		found &= config->QueryElement("CarpetMinY", CarpetMinY);
		found &= config->QueryElement("FieldMaxX", FieldMaxX);
		found &= config->QueryElement("FieldMinX", FieldMinX);
		found &= config->QueryElement("FieldMaxY", FieldMaxY);
		found &= config->QueryElement("FieldMinY", FieldMinY);

		if (found)
		{
			Logger::Instance().WriteMsg("Localization", "All Field parameters loaded successfully", Logger::Info);
		}
		else
		{
			Logger::Instance().WriteMsg("Localization", "Cant Find an attribute in the Field xml config file ", Logger::Error);
		}
	}
	else
	{
		Logger::Instance().WriteMsg("Localization", "Cant Find Field xml config file " + filename , Logger::Error);
	}

	if(config)
		delete config;

	//Initialize particles
	kalmanpoints.resize(maxparticles);
	current_particles=0;
	KalmanParticle::Pvector t;
	t(0)=0.1;//Meters per second
	t(1)=0.1;
	t(2)=0.05; //Rad per second

	for(unsigned i=0;i<kalmanpoints.size();i++)
		kalmanpoints[i].init(t);

	// Loading features,
	LoadFeaturesXML(ArchConfig::Instance().GetConfigPrefix() + "/Features.xml", KFeaturesmap);
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	readRobotConf(ArchConfig::Instance().GetConfigPrefix() + "/robotConfig.xml");
	initParticles();
	cout << "\033[22;32m All Features Loaded \033[0m " << endl;
	return 1;
}

void KLocalization::initParticles()
{
	current_particles=2;
	KalmanParticle::Pvector p;
	float stdx=((FieldMaxX-FieldMinX)/4)/2;
	p(0)=stdx*stdx;
	p(1)=(0.02)*(0.02);
	p(2)=deg2rad(30)*deg2rad(30);

	KalmanParticle::Pvector initx;
	initx(0)=(FieldMaxX-FieldMinX)/4+FieldMinX;
	initx(1)=FieldMaxY;
	initx(2)=deg2rad(270);

	kalmanpoints[0].reset(initx,p,1);// (max) weight

	initx(0)=(FieldMaxX-FieldMinX)/4+FieldMinX;
	initx(1)=FieldMinY;
	initx(2)=deg2rad(90);

	kalmanpoints[1].reset(initx,p,1);// (max) weight

}

void KLocalization::setParticlesPoseUniformly()
{
	KalmanParticle::Pvector p;
	float stdx=((FieldMaxX-FieldMinX)/4)/2;
	p(0)=stdx*stdx;
	p(1)=(0.02)*(0.02);
	p(2)=deg2rad(30)*deg2rad(30);
	initParticles();


	float x=0, y=0;

	if(playerNumber == 1)
	{
		y = 0;
		x = -2.7;
	}
	else if(playerNumber == 2)
	{
		y = 1.2;
		x = -2.4;
	}
	else if(playerNumber == 3)
	{
		y = -1.2;
		x = -2.4;
	}
	else if(playerNumber == 4)
	{
		y = -0.4;
		x = -2.4;
	}
	KalmanParticle::Pvector initx;
	initx(0)=x;
	initx(1)=y;
	initx(2)=deg2rad(0);

	kalmanpoints[current_particles].reset(initx,p,2);
	current_particles++;

}

void KLocalization::initializeParticles(int playerState, bool kickOff)
{
	KalmanParticle::Pvector p;
	float stdx=((FieldMaxX-FieldMinX)/4)/2;
	p(0)=(0.02)*(0.02);
	p(1)=(0.02)*(0.02);
	p(2)=deg2rad(30)*deg2rad(30);


	setParticlesPoseUniformly();
	if(playerState == PLAYER_READY)
	{
		float phi=0, x=0, y=0;

		if(playerNumber == 1)
		{
			y = FieldMinY;
			x = -2.4;
			phi = deg2rad(90);
		}
		else if(playerNumber == 2)
		{
			y = FieldMaxY;
			x = -2.4;
			phi = deg2rad(270);
		}
		else if(playerNumber == 3)
		{
			y = FieldMinY;
			x = -1.2;
			phi = deg2rad(90);
		}
		else if(playerNumber == 4)
		{
			y = FieldMaxY;
			x = -1.2;
			phi = deg2rad(270);
		}
		KalmanParticle::Pvector initx;
		initx(0)=x;
		initx(1)=y;
		initx(2)=deg2rad(phi);

		kalmanpoints[current_particles].reset(initx,p,2);
		current_particles++;

	}
	else if(playerState == PLAYER_SET)
	{
		current_particles=1;
		KalmanParticle::Pvector initx;
		initx(0)=initX[(kickOff) ? 0 : 1];
		initx(1)=initY[(kickOff) ? 0 : 1];
		initx(2)=initPhi[(kickOff) ? 0 : 1];
		kalmanpoints[0].reset(initx,p,4);

	}
}

void KLocalization::pruneParticles()
{



}

//Sequential Importance Resampling
belief KLocalization::LocalizationStepSIR(KMotionModel & MotionModel, vector<KObservationModel>& Observations, vector<KObservationModel>& AmbiguousObservations)
{
	//SIR Filter
	//int index[partclsNum];
	cout<<"SIR"<<endl;

		//Combine :)

	if(current_particles>(unsigned)maxparticles/16)
	{
		unsigned start=maxparticles/16;
		KalmanParticle::Pvector t;
		KalmanParticle::Pmatrix p;
		t=MMSE_belief(start);
		p=MMSE_variance(t,start);
		current_particles=start;//Crop
		float w=0;
		for(unsigned i=start;i<current_particles;i++)
			w+=kalmanpoints[i].weight;
		kalmanpoints[start].x=t;
		kalmanpoints[start].P=p;
		kalmanpoints[start].weight=w;

	}



	//Predict - Move particles according the Prediction Model
	if(MotionModel.freshData)
		Predict(MotionModel);

	//Update - Using incoming observation
	if(Observations.size() >= 1)
	{
		Update(Observations);
	}
	else if(AmbiguousObservations.size() == 1)
	{
		Update_Ambiguous(AmbiguousObservations);
	}
	float normweight=1;
	for(unsigned j=0;j<current_particles&&j<(unsigned)maxparticles/8;j++)
	{

		//do one run of bubble sort
		KalmanParticle t;
		normweight=kalmanpoints[0].weight;
		for(unsigned i=current_particles-1;i>0;i--)
		{

			if(kalmanpoints[i-1].weight<kalmanpoints[i].weight)
			{
				t=kalmanpoints[i-1];
				kalmanpoints[i-1]=kalmanpoints[i];
				kalmanpoints[i]=t;
			}
			normweight+=kalmanpoints[i].weight;
		}

	}

	cout<<"n"<<normweight<<"-"<<current_particles<<endl;
	//Normalize with the largest weight, which is now at pos 0;
	cout<<kalmanpoints[0].weight<<" "<<kalmanpoints[current_particles-1].weight<<endl;
	for(unsigned i=0;i<current_particles;i++)
	{
		kalmanpoints[i].weight/=normweight;
	}
	return getCurrentPosition();
;
}
belief KLocalization::getCurrentPosition() const
{
	belief AgentPosition;
	KalmanParticle::Pvector  mmseest=MMSE_belief();
	//extract estimation
	AgentPosition.x =  mmseest(0);
	AgentPosition.y = mmseest(1);
	AgentPosition.theta = mmseest(2);
	KalmanParticle::Pmatrix p=MMSE_variance(mmseest);
	std::cout<<AgentPosition.x<<" "<<AgentPosition.y<< " "<<AgentPosition.theta<<endl;
	AgentPosition.confidence=1/(p(0,0)+p(1,1)+10*p(2,2));
	return AgentPosition;

}

KalmanParticle::Pvector KLocalization::MMSE_belief(unsigned start)const
{
	KalmanParticle::Pvector r,t;
	r.zero();
	t.zero();
	for(unsigned i=start;i<current_particles;i++)
	{
		t=kalmanpoints[i].x;
		//t.prettyPrint();
		r+=t.scalar_mult(kalmanpoints[i].weight);
	}
	return r;
}


KalmanParticle::Pmatrix KLocalization::MMSE_variance(KalmanParticle::Pvector mean,unsigned start)const
{
	KalmanParticle::Pmatrix r,t;
	KalmanParticle::Pvector tv;
	r.zero();
	t.zero();
	for(unsigned i=start;i<current_particles;i++)
	{
		t=kalmanpoints[i].P;
		tv=kalmanpoints[i].x;
		tv-=mean;
		t+=tv.slow_mult(tv.transp()); //spread of the means
		r+=t.scalar_mult(kalmanpoints[i].weight);
	}
	return r;
}


void KLocalization::Predict(KMotionModel & MotionModel)
{
	double tmpDist, tmpDir, tmpRot;
	tmpDist = MotionModel.Distance.val * (MotionModel.Distance.ratiomean );//+ X.Next() * MotionModel.Distance.ratiodev);
	tmpDir = MotionModel.Direction.val + MotionModel.Direction.Emean ;//+ Y.Next() * MotionModel.Direction.Edev;
	tmpRot = MotionModel.Rotation.val * (MotionModel.Rotation.ratiomean);// + P.Next() * MotionModel.Rotation.ratiodev);

	KalmanParticle::Pvector disp,dvar;

	dvar(0)=MotionModel.Distance.Edev+MotionModel.Distance.ratiodev* MotionModel.Distance.val;
	dvar(1)=MotionModel.Distance.Edev+MotionModel.Distance.ratiodev* MotionModel.Distance.val;
	dvar(2)=MotionModel.Rotation.Edev+MotionModel.Rotation.ratiodev*MotionModel.Rotation.val;
	//Move the particles
	//cout<<"--"<<endl;
	for ( unsigned i = 0; i < current_particles; i++)
	{
		//cout<<kalmanpoints[i].x(0)<<endl;
		disp(0)= cos(tmpDir + kalmanpoints[i].x(2)) * tmpDist;
		disp(1)= sin(tmpDir + kalmanpoints[i].x(2)) * tmpDist;
		disp(2)=tmpRot;
		//cout<<kalmanpoints[i].x(0)<<endl;
		kalmanpoints[i].predict(disp,dvar);

		//cout<<kalmanpoints[i].x(2)<<endl;

	}
}


void KLocalization::spreadParticlesAfterFall()
{

	for ( unsigned i = 0; i < current_particles; i++)
	{
		kalmanpoints[i].P(0,0)*=(SpreadParticlesDeviationAfterFall);
		kalmanpoints[i].P(1,1)*=(SpreadParticlesDeviationAfterFall);
		kalmanpoints[i].P(2,2)*=(RotationDeviationAfterFallInDeg);
	}
}

void KLocalization::Update(vector<KObservationModel> &Observation)
{

	for (unsigned int i = 0; i < Observation.size(); i++)
	{


		KalmanParticle t;
		unsigned  end=current_particles;
		for (unsigned p = 0; p < end; p++)
		{
			t=kalmanpoints[p];//Temp

			kalmanpoints[p].update(
									Observation[i].Distance.val - Observation[i].Distance.Emean,
									Observation[i].Bearing.val,
									Observation[i].Distance.Edev,
									Observation[i].Bearing.Edev,
									-Observation[i].Feature.x,
									-Observation[i].Feature.y
									);

			t.update(
									Observation[i].Distance.val - Observation[i].Distance.Emean,
									Observation[i].Bearing.val,
									Observation[i].Distance.Edev,
									Observation[i].Bearing.Edev,
									Observation[i].Feature.x,
									Observation[i].Feature.y
									);
			kalmanpoints[current_particles++]=t;
		}

	}
}
void KLocalization::Update_Ambiguous(vector<KObservationModel> &Observation)
{
	//Function to update the weights of each particle regarding the ObservationDistance from an object and the direction
	double OverallWeight, ParticlePointBearingAngle, ParticleBearing, Deviation;
	double DistanceFromPastBelief, DirectionFromPastBelief;
	double AdditiveWeightTotal = 0, AdditiveOwnField = 0, AdditiveEnemyField = 0;
	double R;
	float xPosOfFeature 	= Observation[0].Feature.x;
	float yPosOfFeature 	= Observation[0].Feature.y;
	float obsDistEmean	= Observation[0].Distance.Emean;
	float obsDistEdev 	= Observation[0].Distance.Edev;
	float obsDistValue	= Observation[0].Distance.val;
	float obsBearingEdev 	= Observation[0].Bearing.Edev;
	float obsBearingValue 	= Observation[0].Bearing.val;




	for (unsigned int i = 0; i < Observation.size(); i++)
	{
			KalmanParticle t[4];

			unsigned end=current_particles;
			for (unsigned p = 0; p < end; p++)
			{
				t[0]=kalmanpoints[p];//Temp
				t[1]=kalmanpoints[p];//Temp
				t[2]=kalmanpoints[p];//Temp
				t[3]=kalmanpoints[p];//Temp
				t[0].update(
										Observation[i].Distance.val - Observation[i].Distance.Emean,
										Observation[i].Bearing.val,
										Observation[i].Distance.Edev,
										Observation[i].Bearing.Edev,
										Observation[i].Feature.x,
										Observation[i].Feature.y
										);
				t[1].update(
										Observation[i].Distance.val - Observation[i].Distance.Emean,
										Observation[i].Bearing.val,
										Observation[i].Distance.Edev,
										Observation[i].Bearing.Edev,
										-Observation[i].Feature.x,
										Observation[i].Feature.y
										);
				t[2].update(
									Observation[i].Distance.val - Observation[i].Distance.Emean,
									Observation[i].Bearing.val,
									Observation[i].Distance.Edev,
									Observation[i].Bearing.Edev,
									-Observation[i].Feature.x,
									-Observation[i].Feature.y
									);
				t[3].update(
									Observation[i].Distance.val - Observation[i].Distance.Emean,
									Observation[i].Bearing.val,
									Observation[i].Distance.Edev,
									Observation[i].Bearing.Edev,
									Observation[i].Feature.x,
									-Observation[i].Feature.y
									);

				kalmanpoints[p]=t[0];
				kalmanpoints[current_particles++]=t[1];
				kalmanpoints[current_particles++]=t[2];
				kalmanpoints[current_particles++]=t[3];
				cout<<current_particles<<endl;
		}


	}
}




//Load xml files
int KLocalization::LoadFeaturesXML(string filename, map<string, feature>& KFeaturesmap)
{
	TiXmlDocument doc2(filename.c_str());
	bool loadOkay = doc2.LoadFile();

	if (!loadOkay)
	{
		Logger::Instance().WriteMsg("KLocalization", "Feature loading failed!!!!", Logger::Error);
		return -1;
	}

	TiXmlNode * Ftr;
	TiXmlElement * Attr;
	string ID;
	double x, y, w;
	feature temp;

	for (Ftr = doc2.FirstChild()->NextSibling(); Ftr != 0; Ftr = Ftr->NextSibling())
	{
		if(Ftr->ToComment() == NULL)
		{
			Attr = Ftr->ToElement();
			Attr->Attribute("x", &x);
			Attr->Attribute("y", &y);
			Attr->Attribute("weight", &w);
			ID = Attr->Attribute("ID");
			temp.set(x, y, ID, w);
			KFeaturesmap[temp.id] = temp;
		}
	}

	return 0;
}

int KLocalization::readConfiguration(const std::string& file_name)
{
	XMLConfig config(file_name);

	if (!config.QueryElement("player", playerNumber))
	{
		Logger::Instance().WriteMsg("KLocalization", "Configuration file has no player, setting to default value: " + _toString(playerNumber), Logger::Error);
		playerNumber = 1;
	}

	return 1;
}

bool KLocalization::readRobotConf(const std::string& file_name)
{
	XML config(file_name);
	typedef std::vector<XMLNode<std::string, float, std::string> > NodeCont;
	NodeCont teamPositions, robotPosition ;
	Logger::Instance().WriteMsg("Localization",  " readRobotConfiguration "  , Logger::Info);

	for (int i = 0; i < 2; i++)
	{
		string kickoff = (i == 0) ? "KickOff" : "noKickOff";	//KICKOFF==0, NOKICKOFF == 1
		bool found = false;
		teamPositions = config.QueryElement<std::string, float, std::string>(kickoff);

		if (teamPositions.size() != 0)
			robotPosition = config.QueryElement<std::string, float, std::string>("robot", &(teamPositions[0]));

		for (NodeCont::iterator it = robotPosition.begin(); it != robotPosition.end(); it++)
		{
			if (it->attrb["number"] == playerNumber)
			{
				initPhi[i] = 0.0;
				initX[i] = (it->attrb["posx"]);
				initY[i] = (it->attrb["posy"]);
				found = true;
				break;
			}
			else
			{
				initPhi[i] = 0.0;
				initX[i] = 0.0;
				initY[i] = 0.0;
			}
		}
	}

	return true;
}


