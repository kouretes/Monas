#include "GraphicalRobotElement.h"

#include "tools/toString.h"

#include <math.h>

#define TO_RAD 0.01745329f

#include <iostream>
using namespace std;

GraphicalRobotElement::GraphicalRobotElement(KFieldScene* parent, QString host)
{
	this->parentScene = parent;
	hostId = host;

	currentWIM.Clear();
	currentObsm.Clear();
	ParticlesList.clear();

	GWSRobotVisible = false;
	GWSBallVisible = false;
	GWSUnionistLineVisible = false;
	LWSVisionBallVisible = false;
	LWSVisionYellowLeftPostVisible = false;
	LWSVisionYellowRightPostVisible = false;
	LWSVisionYellowPostVisible = false;
	LWSParticlesVisible = false;
	LWSHFOVVisible = false;

	QPen penForUnionistLine(Qt::black);
	penForUnionistLine.setWidth(2);

	QPen penForRobotDirection(Qt::black);
	penForRobotDirection.setWidth(3);

	for(unsigned it=0; it<30; it++)
	{
		Particle *part = new GUIRobotPose();
		part->Direction = this->parentScene->addLine(QLineF(),QPen(Qt::black));
		part->Pose = this->parentScene->addEllipse(QRect(),QPen(Qt::cyan),QBrush(Qt::cyan));
		ParticlesList.append(part);
	}

	PositiveBoundLine = this->parentScene->addLine(QLineF(),penForUnionistLine);
	NegativeBoundLine = this->parentScene->addLine(QLineF(),penForUnionistLine);

	UnionistLine = this->parentScene->addLine(QLineF(),penForUnionistLine);
	RobotDirection = this->parentScene->addLine(QLineF(),penForRobotDirection);
	Robot = this->parentScene->addEllipse(QRect(),QPen(Qt::black),QBrush(Qt::darkGray));
	Ball = this->parentScene->addEllipse(QRect(),QPen(Qt::black),QBrush(Qt::darkGray));

	VisionBall = this->parentScene->addEllipse(QRect(),QPen(Qt::black),QBrush(Qt::white));

	LeftYellowPost = this->parentScene->addEllipse(QRect(),QPen(Qt::yellow),QBrush(Qt::yellow));
	RightYellowPost = this->parentScene->addEllipse(QRect(),QPen(Qt::yellow),QBrush(Qt::yellow));
	YellowPost = this->parentScene->addEllipse(QRect(),QPen(Qt::yellow),QBrush(Qt::yellow));

	GREtimer = new QTimer();
	connect(GREtimer, SIGNAL(timeout()), this, SLOT(clearVisionObservations()));
	connect(parentScene->getParentGraphicsView(), SIGNAL(forceTimeOut()),this, SLOT(clearVisionObservations()));
}


GraphicalRobotElement::~GraphicalRobotElement()
{
	if(PositiveBoundLine)
		delete PositiveBoundLine;

	if(NegativeBoundLine)
		delete NegativeBoundLine;

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

	if(RightYellowPost)
		delete RightYellowPost;

	for(unsigned i = 0; i<ParticlesList.count();i++)
	{
		if(ParticlesList.at(i)->Pose)
			delete ParticlesList.at(i)->Pose;

		if(ParticlesList.at(i)->Direction)
			delete ParticlesList.at(i)->Direction;

		if (ParticlesList.at(i))
			delete ParticlesList.at(i);
	}

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
	}
	else
	{
		VisionBall->setRect(0, 0, 0, 0);
	}
}

void GraphicalRobotElement::setYellowLeftPostVisible(bool visible)
{
	if (visible == false)
		this->LeftYellowPost->setVisible(false);
	else
		this->LeftYellowPost->setVisible(true);
}

void GraphicalRobotElement::setYellowRightPostVisible(bool visible)
{
	if (visible == false)
		this->RightYellowPost->setVisible(false);
	else
		this->RightYellowPost->setVisible(true);
}

void GraphicalRobotElement::setYellowPostVisible(bool visible)
{
	if (visible == false)
		this->YellowPost->setVisible(false);
	else
		this->YellowPost->setVisible(true);
}

void GraphicalRobotElement::updateGoalPostsRect()
{
	if (currentObsm.regular_objects_size() > 0 && currentWIM.has_myposition())
	{

		std::cout << "---------------------" << std::endl;
		for (unsigned o = 0; o < currentObsm.regular_objects_size(); o++)
		{
			NamedObject *obj = currentObsm.mutable_regular_objects(o);
			QRectF rect = parentScene->goalPostRectFromOBM( obj, &currentWIM);

			std::cout << obj->object_name() << std::endl;
			if (obj->object_name() == "YellowLeft")
			{
				LeftYellowPost->setRect(rect);
				tagVisionObservations(LeftYellowPost, rect, "L");

			}else if (obj->object_name() == "YellowRight")
			{
				RightYellowPost->setRect(rect);
				tagVisionObservations(RightYellowPost, rect, "R");

			}else if (obj->object_name() == "Yellow")
			{
				YellowPost->setRect(rect);
				tagVisionObservations(YellowPost, rect, "A");
			}
		}
		std::cout << "---------------------" << std::endl;

	}else
	{
		LeftYellowPost->setRect(0, 0, 0, 0);
		RightYellowPost->setRect(0, 0, 0, 0);
		YellowPost->setRect(0, 0, 0, 0);
	}
}

void GraphicalRobotElement::tagVisionObservations(QGraphicsEllipseItem* post,QRectF rect, QString text)
{
	QBrush brush;
    QPixmap pix(700,700);
    pix.fill(Qt::yellow);
    QPainter paint(&pix);
    paint.setPen("black");
    paint.drawText(rect, Qt::AlignCenter ,text);
    brush.setTexture(pix);
    post->setBrush(brush);

}

void GraphicalRobotElement::clearVisionObservations()
{
	if (this->VisionBall->isVisible())
	{
		this->VisionBall->setVisible(false);
		this->VisionBall->setRect(0, 0, 0, 0); //?????
	}

	if (this->YellowPost->isVisible())
	{
		this->YellowPost->setVisible(false);
		this->YellowPost->setRect(0, 0, 0, 0);
	}

	if (this->LeftYellowPost->isVisible())
	{
		this->LeftYellowPost->setVisible(false);
		this->LeftYellowPost->setRect(0, 0, 0, 0);
	}

	if (this->RightYellowPost->isVisible())
	{
		this->RightYellowPost->setVisible(false);
		this->RightYellowPost->setRect(0, 0, 0, 0);
	}

	GREtimer->stop();
	std::cout << "GREtimer stopped." << std::endl;
}


void GraphicalRobotElement::setParticlesVisible(bool visible)
{
	for(unsigned i = 0; i<ParticlesList.count();i++)
	{
		if (visible == false)
		{
			ParticlesList.at(i)->Pose->setVisible(false);
			ParticlesList.at(i)->Direction->setVisible(false);

		}else
		{
			ParticlesList.at(i)->Pose->setVisible(true);
			ParticlesList.at(i)->Direction->setVisible(true);
		}
	}
}

void GraphicalRobotElement::updateParticlesRect(LocalizationDataForGUI debugGUI)
{

	for (unsigned i=0; i < debugGUI.particles_size(); i++)
	{
		RobotPose particle = debugGUI.particles(i);

		if(particle.has_x() && particle.has_y() && particle.has_phi())
		{
			ParticlesList.at(i)->Pose->setRect(this->parentScene->rectFromFC( particle.x(),
					particle.y(), 50, 50));
			ParticlesList.at(i)->Direction->setLine(this->parentScene->lineFromFCA(particle.x(),
					particle.y(), particle.phi(), 200));
		}else
		{
			ParticlesList.at(i)->Pose->setRect( 0, 0, 0, 0);
			ParticlesList.at(i)->Direction->setLine(0, 0, 0, 0);
		}
	}
}

void GraphicalRobotElement::setHFOVVisible(bool visible)
{
	if (visible == false)
	{
		this->PositiveBoundLine->setVisible(false);
		this->NegativeBoundLine->setVisible(false);

	}else
	{
		this->PositiveBoundLine->setVisible(true);
		this->NegativeBoundLine->setVisible(true);
	}
}

void GraphicalRobotElement::updateHFOVRect(float HeadYaw)
{
	float HeadYawPlusTheta = 0.f;

	if(this->currentWIM.has_myposition())
	{
		HeadYawPlusTheta = this->currentWIM.myposition().phi() + HeadYaw;

		PositiveBoundLine->setLine(this->parentScene->lineFromFCA( this->currentWIM.myposition().x()*1000,
				this->currentWIM.myposition().y()*1000, HeadYawPlusTheta + 20*TO_RAD, 800));
		NegativeBoundLine->setLine(this->parentScene->lineFromFCA(this->currentWIM.myposition().x()*1000,
						this->currentWIM.myposition().y()*1000,HeadYawPlusTheta - 20*TO_RAD , 800));
	}else
	{
		PositiveBoundLine->setLine(0, 0, 0, 0);
		NegativeBoundLine->setLine(0, 0, 0, 0);
	}
}

