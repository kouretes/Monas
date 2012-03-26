#include "GraphicalRobotElement.h"

#include "tools/toString.h"

#include <math.h>

#include <iostream>
using namespace std;

GraphicalRobotElement::GraphicalRobotElement(KFieldScene* parent, QString host)
{
	this->parentScene = parent;
	hostId = host;

	currentWIM.Clear();
	currentObsm.Clear();

	GWSRobotVisible = false;
	GWSBallVisible = false;
	GWSUnionistLineVisible = false;
	LWSVisionBallVisible = false;
	VisionYellowLeftPostVisible = false;
	VisionYellowRightPostVisible = false;
	VisionBlueLeftPostVisible = false;
	VisionBlueRightPostVisible = false;

	QPen penForUnionistLine(Qt::black);
	penForUnionistLine.setWidth(1);

	QPen penForRobotDirection(Qt::black);
	penForRobotDirection.setWidth(3);

	QPen penForYellowPost(Qt::darkYellow);//todo darkYellow , red
	penForYellowPost.setWidth(6);
	QPen penForBluePost(Qt::darkBlue); //todo darkBlue , black
	penForBluePost.setWidth(6);

	UnionistLine = this->parentScene->addLine(QLineF(),penForUnionistLine);
	RobotDirection = this->parentScene->addLine(QLineF(),penForRobotDirection);
	Robot = this->parentScene->addEllipse(QRect(),QPen(Qt::black),QBrush(Qt::darkGray));
	Ball = this->parentScene->addEllipse(QRect(),QPen(Qt::black),QBrush(Qt::darkGray));


	VisionBall = this->parentScene->addEllipse(QRect(),QPen(Qt::black),QBrush(Qt::white));

	YellowPost = this->parentScene->addEllipse(QRect(),QPen(Qt::red),QBrush(Qt::darkYellow)); //todo darkYellow
	LeftYellowPost = this->parentScene->addLine(QLineF(),penForYellowPost);
	YellowPost1 = this->parentScene->addEllipse(QRect(),QPen(Qt::red),QBrush(Qt::darkYellow));//todo darkYellow
	RightYellowPost = this->parentScene->addLine(QLineF(),penForYellowPost);
	BluePost = this->parentScene->addEllipse(QRect(),QPen(Qt::black),QBrush(Qt::darkBlue));//todo darkBlue
	LeftBluePost = this->parentScene->addLine(QLineF(),penForBluePost);
	BluePost1 = this->parentScene->addEllipse(QRect(),QPen(Qt::black),QBrush(Qt::darkBlue));//todo darkBlue
	RightBluePost = this->parentScene->addLine(QLineF(),penForBluePost);


	///////////////////////////////////////////////

	GREtimer = new QTimer();
	GREtimer->setInterval(500);

	connect(GREtimer, SIGNAL(timeout()), this, SLOT(resetVisionObservations()));

}


GraphicalRobotElement::~GraphicalRobotElement()
{
	if(UnionistLine)
		delete UnionistLine;

	if(RobotDirection)
		delete RobotDirection;

	if(Robot)
		delete Robot;

	if(Ball)
		delete Ball;

	if(VisionBall)
		delete VisionBall;

	if(YellowPost)
		delete YellowPost;

	if(LeftYellowPost)
		delete LeftYellowPost;

	if(YellowPost1)
		delete YellowPost1;

	if(RightYellowPost)
		delete RightYellowPost;

	if(BluePost)
		delete BluePost;

	if(LeftBluePost)
		delete LeftBluePost;

	if(BluePost1)
		delete BluePost1;

	if(RightBluePost)
		delete RightBluePost;
}

void GraphicalRobotElement::setCurrentWIM(WorldInfo nwim)
{
	currentWIM.Clear();
	currentWIM = nwim;
}

void GraphicalRobotElement::setCurrentGSM(GameStateMessage gsm)
{
	QString playerNumber = "No Available number";

	playerNumber = QString::fromStdString(_toString((gsm.player_number())));
	Robot->setToolTip(playerNumber);

	if(gsm.team_color() == 0)
	{
		Robot->setBrush(Qt::blue);
		Ball->setBrush(Qt::blue);

	}else if (gsm.team_color() == 1)
	{
		Robot->setBrush(Qt::red);
		Ball->setBrush(Qt::red);

	}

}

void GraphicalRobotElement::setcurrentOBSM(ObservationMessage obm)
{
	//GREtimer->stop();
	//cout << "GREtimer->stop()" << endl;
	currentObsm.Clear();
	currentObsm = obm;
}

void GraphicalRobotElement::setRobotVisible(bool visible)
{
	if (visible == false)
	{
		Robot->setVisible(false);
		RobotDirection->setVisible(false);

	}else
	{
		Robot->setVisible(true);
		RobotDirection->setVisible(true);
	}

}

void GraphicalRobotElement::updateRobotRect()
{
	if(this->currentWIM.has_myposition())
	{
		Robot->setRect(this->parentScene->rectFromFC( this->currentWIM.myposition().x()*1000,
				this->currentWIM.myposition().y()*1000, 150, 150));
		RobotDirection->setLine(this->parentScene->lineFromFCA(this->currentWIM.myposition().x()*1000,
						this->currentWIM.myposition().y()*1000, this->currentWIM.myposition().phi(), 200));
	}else
	{
		Robot->setRect( 0, 0, 0, 0);
		RobotDirection->setLine(0, 0, 0, 0);
	}
}

void GraphicalRobotElement::setBallVisible(bool visible)
{
	if (visible == false)
		this->Ball->setVisible(false);
	else
		this->Ball->setVisible(true);

}

void GraphicalRobotElement::updateBallRect()
{
	if((this->currentWIM.balls_size()>0) && this->currentWIM.has_myposition())
	{
		Ball->setRect(this->parentScene->ballRectFromFC(&currentWIM, 75, 75));
	}else
	{
		Ball->setRect(0, 0, 0, 0);
	}

}

void GraphicalRobotElement::setUnionistLineVisible(bool visible)
{
	if (visible == false)
		this->UnionistLine->setVisible(false);
	else
		this->UnionistLine->setVisible(true);

}

void GraphicalRobotElement::updateUnionistLineRect()
{
	if((this->currentWIM.balls_size()>0) && this->currentWIM.has_myposition())
	{
		UnionistLine->setLine(this->parentScene->unionistLineRectFromFC(&currentWIM));
	}else
	{
		UnionistLine->setLine(0, 0, 0, 0);
	}

}

void GraphicalRobotElement::setVisionBallVisible(bool visible)
{
	if (visible == false)
		this->VisionBall->setVisible(false);
	else
		this->VisionBall->setVisible(true);

}

void GraphicalRobotElement::updateVisionBallRect(ObservationMessage obm)
{

	if( obm.has_ball() && currentWIM.has_myposition())
	{
		VisionBall->setRect(this->parentScene->visionBallRect(obm.ball(), currentWIM));
		std::cout << "Exw mpala exw exw exw exw exw exw AKOMA!!!" << std::endl;
	}
	else
	{
		VisionBall->setRect(0, 0, 0, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////

void GraphicalRobotElement::setYellowLeftPostVisible(bool visible)
{
	if (visible == false)
	{
		this->YellowPost->setVisible(false);
		this->LeftYellowPost->setVisible(false);

	} else
	{
		this->YellowPost->setVisible(true);
		this->LeftYellowPost->setVisible(true);
	}
}

void GraphicalRobotElement::setYellowRightPostVisible(bool visible)
{
	if (visible == false)
	{
		this->YellowPost1->setVisible(false);
		this->RightYellowPost->setVisible(false);

	} else
	{
		this->YellowPost1->setVisible(true);
		this->RightYellowPost->setVisible(true);
	}
}

void GraphicalRobotElement::setBlueLeftPostVisible(bool visible)
{
	if (visible == false)
	{
		this->BluePost->setVisible(false);
		this->LeftBluePost->setVisible(false);

	} else
	{
		this->BluePost->setVisible(true);
		this->LeftBluePost->setVisible(true);
	}
}

void GraphicalRobotElement::setBlueRightPostVisible(bool visible)
{
	if (visible == false)
	{
		this->BluePost1->setVisible(false);
		this->RightBluePost->setVisible(false);

	} else
	{
		this->BluePost1->setVisible(true);
		this->RightBluePost->setVisible(true);
	}
}

//todo rename to all vision obs
void GraphicalRobotElement::updateYellowLeftPostRect()
{
	float xmiddle;
	float ymiddle;

	if (currentObsm.regular_objects_size() > 0 && currentWIM.has_myposition()) {

		//cout << "#############################################################" << endl;

		for (int o = 0; o < currentObsm.regular_objects_size(); o++) {

			xmiddle = 0.f;
			ymiddle = 0.f;

			NamedObject *obj = currentObsm.mutable_regular_objects(o);

			if (obj->object_name() == "YellowLeft") {

				xmiddle = (currentWIM.myposition().x() + obj->distance()*cos((currentWIM.myposition().phi() + obj->bearing())))*1000;
				ymiddle = (currentWIM.myposition().y() + obj->distance() * sin((currentWIM.myposition().phi() + obj->bearing())))*1000;

				//e3iswseis chief ..
				//xmiddle = (currentWIM.myposition().x() + obj->distance()*cos(obj->bearing()))*1000;
				//ymiddle = (currentWIM.myposition().y() + obj->distance() * sin(obj->bearing()))*1000;



				YellowPost->setRect(this->parentScene->rectFromFC( xmiddle, ymiddle, 100, 100));
				LeftYellowPost->setLine(this->parentScene->lineFromFieldCoordinates(xmiddle, ymiddle, xmiddle, (ymiddle - 175)));



			cout << "EXWWWWWWWWWWWWWWWWWW YellowLeft @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
			/*cout << "Sta xmiddle = " << xmiddle << endl;
			cout << "Sta ymiddle = " << ymiddle << endl;
			cout << "To distance = " <<  obj->distance() << endl;*/


			} else if (obj->object_name() == "YellowRight") {

				cout << "EXWWWWWWWWWWWWWWWWWW YellowRight @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
				xmiddle = (currentWIM.myposition().x() + obj->distance()*cos((currentWIM.myposition().phi() + obj->bearing())))*1000;
				ymiddle = (currentWIM.myposition().y() + obj->distance() * sin((currentWIM.myposition().phi() + obj->bearing())))*1000;

				//cout << "To xmiddle = " << xmiddle << endl;
				//cout << "To ymiddle = " << ymiddle << endl;
				//cout << "To distance = " << obj->distance() << endl;


				YellowPost1->setRect(this->parentScene->rectFromFC( xmiddle, ymiddle, 100, 100));
				RightYellowPost->setLine(this->parentScene->lineFromFieldCoordinates(xmiddle, ymiddle, xmiddle, (ymiddle + 175)));


			} else if (obj->object_name() == "SkyblueLeft") {
				cout << "EXWWWWWWWWWWWWWWWWWW SkyblueLeft @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
				xmiddle = (currentWIM.myposition().x() + obj->distance()*cos((currentWIM.myposition().phi() + obj->bearing())))*1000;
				ymiddle = (currentWIM.myposition().y() + obj->distance() * sin((currentWIM.myposition().phi() + obj->bearing())))*1000;


				//cout << "Sta xmiddle = " << xmiddle << endl;
				//cout << "Sta ymiddle = " << ymiddle << endl;

				BluePost->setRect(this->parentScene->rectFromFC( xmiddle, ymiddle, 100, 100));
				LeftBluePost->setLine(this->parentScene->lineFromFieldCoordinates(xmiddle, ymiddle, xmiddle, (ymiddle + 175)));


			} else if (obj->object_name() == "SkyblueRight") {
				cout << "EXWWWWWWWWWWWWWWWWWW SkyblueRight @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
				xmiddle = (currentWIM.myposition().x() + obj->distance()*cos((currentWIM.myposition().phi() + obj->bearing())))*1000;
				ymiddle = (currentWIM.myposition().y() + obj->distance() * sin((currentWIM.myposition().phi() + obj->bearing())))*1000;

				BluePost1->setRect(this->parentScene->rectFromFC( xmiddle, ymiddle, 100, 100));
				RightBluePost->setLine(this->parentScene->lineFromFieldCoordinates(xmiddle, ymiddle, xmiddle, (ymiddle - 175)));



			} /*else if (obj->object_name() == "Skyblue") { //Ambigious GoalPost
				//cout << "EXWWWWWWWWWWWWWWWWWW Skyblue @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;


			} else if (obj->object_name() == "Yellow") { //Ambigious GoalPost

				//cout << "EXWWWWWWWWWWWWWWWWWW Yellow @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;

			}*/

		}

	}


}

void GraphicalRobotElement::resetVisionObservations()
{
	cout << "Kanw reset !!!!" << endl;
	if (this->YellowPost->isVisible())
	{
		this->YellowPost->setVisible(false);
	}

	if (this->LeftYellowPost->isVisible())
	{
		this->LeftYellowPost->setVisible(false);
	}

	if (this->YellowPost1->isVisible())
	{
		this->YellowPost1->setVisible(false);
	}

	if (this->RightYellowPost->isVisible())
	{
		this->RightYellowPost->setVisible(false);
	}

	if (this->BluePost->isVisible())
	{
		this->BluePost->setVisible(false);
	}

	if (this->LeftBluePost->isVisible())
	{
		this->LeftBluePost->setVisible(false);
	}

	if (this->BluePost1->isVisible())
	{
		this->BluePost1->setVisible(false);
	}

	if (this->RightBluePost->isVisible())
	{
		this->RightBluePost->setVisible(false);
	}

	GREtimer->stop();
	cout << "GREtimer->stop()!!!!!!!!!!!!!!" << endl;



}

////////////////////////////////////////////////////////////////////////////////////


