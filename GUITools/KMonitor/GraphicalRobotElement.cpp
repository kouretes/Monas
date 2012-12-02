#include "GraphicalRobotElement.h"

#include "architecture/archConfig.h"
#include "tools/toString.h"

#include <math.h>

#define TO_RAD 0.01745329f
const qreal Pi = 3.14;
#define ToDegrees	(180.0/Pi)

#include <iostream>
using namespace std;

GraphicalRobotElement::GraphicalRobotElement (KFieldScene *parent, QString host) {
	this->parentScene = parent;
	hostId = host;
	teamColor = 1;
	currentWIM.Clear();
	currentSWIM.Clear();
	currentObsm.Clear();
	ParticlesList.clear();
	RobotPositions.set_capacity (100);
	UnionistLines.set_capacity (99);
	GWSRobotVisible = false;
	GWSBallVisible = false;
	GWSUnionistLineVisible = false;
	LWSRobotVisible = false;
	LWSBallVisible = false;
	LWSUnionistLineVisible = false;
	LWSVisionBallVisible = false;
	LWSVisionYellowLeftPostVisible = false;
	LWSVisionYellowRightPostVisible = false;
	LWSVisionYellowPostVisible = false;
	LWSParticlesVisible = false;
	LWSHFOVVisible = false;
	LWSTraceVisible = false;
	LWSMWCmdVisible = false;
	QPen penForUnionistLine (Qt::black);
	penForUnionistLine.setWidth (1);
	QPen penForRobotDirection (Qt::black);
	penForRobotDirection.setWidth (3);
	QPen penForMotionCmdLine (Qt::darkRed);
	penForMotionCmdLine.setWidth (2);
	loadXMLConfigParameters (ArchConfig::Instance().GetConfigPrefix() + "/localizationConfig.xml");

	for (unsigned it = 0; it < partclsNum; it++) {
		Particle *part = new GUIRobotPose();
		part->Direction = this->parentScene->addLine (QLineF(), QPen (Qt::black) );
		part->Pose = this->parentScene->addEllipse (QRect(), QPen (Qt::cyan), QBrush (Qt::cyan) );
		ParticlesList.append (part);
	}

	HFOVLines = this->parentScene->addPolygon (QPolygonF(), QPen (Qt::darkCyan), QBrush (Qt::Dense7Pattern) );
	GotoPositionLine = this->parentScene->addLine (QLineF(), penForMotionCmdLine);
	GotoArrow = this->parentScene->addPolygon (QPolygonF(), QPen (Qt::darkRed), QBrush (Qt::darkRed) );
	zAxisArc = this->parentScene->addEllipse (QRect(), penForMotionCmdLine, QBrush (Qt::transparent) );
	UnionistLine = this->parentScene->addLine (QLineF(), penForUnionistLine);
	RobotDirection = this->parentScene->addLine (QLineF(), penForRobotDirection);
	Robot = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::darkGray) );
	Ball = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::darkGray) );
	Teammates[0] = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::magenta) );
	Teammates[1] = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::green) );
	Teammates[2] = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::blue) );
	Teammates[3] = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::red) );
	Teammates[4] = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::cyan) );

	for (int i = 0; i < numOfRobots; i++) {
		TeammateDirections[i] = this->parentScene->addLine (QLineF(), penForRobotDirection);
	}

	VisionBall = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::white) );
	LeftYellowPost = this->parentScene->addEllipse (QRect(), QPen (Qt::yellow), QBrush (Qt::yellow) );
	RightYellowPost = this->parentScene->addEllipse (QRect(), QPen (Qt::yellow), QBrush (Qt::yellow) );
	YellowPost = this->parentScene->addEllipse (QRect(), QPen (Qt::yellow), QBrush (Qt::yellow) );
	GREtimer = new QTimer();
	connect (GREtimer, SIGNAL (timeout() ), this, SLOT (clearVisionObservations() ) );
	MWCmdTimer = new QTimer();
	connect (MWCmdTimer, SIGNAL (timeout() ), this, SLOT (clearMotionWalkCommand() ) );
}


GraphicalRobotElement::~GraphicalRobotElement() {
	if (HFOVLines) {
		delete HFOVLines;
	}

	if (UnionistLine) {
		delete UnionistLine;
	}

	if (RobotDirection) {
		delete RobotDirection;
	}

	if (Robot) {
		delete Robot;
	}

	for (int i = 0; i < numOfRobots; i++) {
		if (Teammates[i]) {
			delete Teammates[i];
		}

		if (TeammateDirections[i]) {
			delete TeammateDirections[i];
		}
	}

	if (Ball) {
		delete Ball;
	}

	if (VisionBall) {
		delete VisionBall;
	}

	if (YellowPost) {
		delete YellowPost;
	}

	if (LeftYellowPost) {
		delete LeftYellowPost;
	}

	if (RightYellowPost) {
		delete RightYellowPost;
	}

	if (GotoPositionLine) {
		delete GotoPositionLine;
	}

	if (GotoArrow) {
		delete GotoArrow;
	}

	if (zAxisArc) {
		delete zAxisArc;
	}

	for (unsigned i = 0; i < ParticlesList.count(); i++) {
		if (ParticlesList.at (i)->Pose) {
			delete ParticlesList.at (i)->Pose;
		}

		if (ParticlesList.at (i)->Direction) {
			delete ParticlesList.at (i)->Direction;
		}

		if (ParticlesList.at (i) ) {
			delete ParticlesList.at (i);
		}
	}

	boost::circular_buffer<QGraphicsEllipseItem *>::iterator P_it;

	for (P_it = RobotPositions.begin(); P_it != RobotPositions.end(); ++P_it) {
		if ( (*P_it) ) {
			delete (*P_it);
		}
	}

	boost::circular_buffer<QGraphicsLineItem *>::iterator L_it;

	for (L_it = UnionistLines.begin(); L_it != UnionistLines.end(); ++L_it) {
		if ( (*L_it) ) {
			delete (*L_it);
		}
	}
}

void GraphicalRobotElement::loadXMLConfigParameters (std::string fname) {
	XMLConfig *xmlconfig = new XMLConfig (fname);
	partclsNum = 0;
	xmlconfig->QueryElement ("partclsNum", partclsNum);
}

void GraphicalRobotElement::setCurrentWIM (WorldInfo nwim) {
	currentWIM.Clear();

	if (teamColor == 1) {
		currentWIM = nwim;
	} else {
		WorldInfo MyWorld;
		MyWorld.Clear();
		MyWorld.CopyFrom (nwim);
		float value = (-1) * MyWorld.myposition().x();
		MyWorld.mutable_myposition()->set_x (value);
		value = (-1) * MyWorld.myposition().y();
		MyWorld.mutable_myposition()->set_y (value);
		value = MyWorld.myposition().phi() + Pi;
		MyWorld.mutable_myposition()->set_phi (value);
		currentWIM = MyWorld;
	}

	if (LWSTraceVisible) {
		setLWSTraceVisible (false);
		updateTraceRect();
		setLWSTraceVisible (true);
	} else {
		setLWSTraceVisible (false);
		updateTraceRect();
		setLWSTraceVisible (false);
	}
}

void GraphicalRobotElement::setCurrentSWIM (SharedWorldInfo nswim) {
	currentSWIM.Clear();
	currentSWIM = nswim;
}

void GraphicalRobotElement::setCurrentGSM (GameStateMessage gsm) {
	QString playerNumber = "No Available number";
	playerNumber = QString::fromStdString (_toString ( (gsm.player_number() ) ) );
	Robot->setToolTip (playerNumber);

	if (gsm.team_color() == 0) {
		Robot->setBrush (Qt::blue);
		Ball->setBrush (Qt::blue);
		teamColor = 1;
	} else if (gsm.team_color() == 1) {
		Robot->setBrush (Qt::red);
		Ball->setBrush (Qt::red);
		teamColor = -1;
	}
}

void GraphicalRobotElement::setcurrentOBSM (ObservationMessage obm) {
	currentObsm.Clear();
	currentObsm = obm;
}

void GraphicalRobotElement::setRobotVisible (bool visible) {
	if (visible == false) {
		Robot->setVisible (false);
		RobotDirection->setVisible (false);
	} else {
		Robot->setVisible (true);
		RobotDirection->setVisible (true);
	}
}

void GraphicalRobotElement::setTeammatesVisible (bool visible) {
	for (int i = 0; i < numOfRobots; i++) {
		Teammates[i]->setVisible (visible);
		TeammateDirections[i]->setVisible (visible);
	}
}

void GraphicalRobotElement::setTeammateVisible (int idx, bool visible) {
	if (visible == false) {
		Teammates[idx]->setVisible (false);
		TeammateDirections[idx]->setVisible (false);
	} else {
		Teammates[idx]->setVisible (true);
		TeammateDirections[idx]->setVisible (true);
	}
}

void GraphicalRobotElement::updateRobotRect() {
	if (this->currentWIM.has_myposition() ) {
		Robot->setRect (this->parentScene->rectFromFC (this->currentWIM.myposition().x() * 1000,
		                this->currentWIM.myposition().y() * 1000, 150, 150) );
		RobotDirection->setLine (this->parentScene->lineFromFCA (this->currentWIM.myposition().x() * 1000,
		        this->currentWIM.myposition().y() * 1000, this->currentWIM.myposition().phi(), 200) );
	} else {
		Robot->setRect ( 0, 0, 0, 0);
		RobotDirection->setLine (0, 0, 0, 0);
	}
}


void GraphicalRobotElement::updateTeammatesRects() {
	int idx;

	if (currentSWIM.teammateposition_size() != 0) {
		//std::cout << currentSWIM.teammateposition_size() << "\n";
		for (idx = 0; idx < numOfRobots; idx++) {
			setTeammateVisible (idx, false);
		}

		for (idx = 0; idx < currentSWIM.teammateposition_size(); idx++) {
			//std::cout << this->currentSWIM.teammateposition(idx).pose().x() << " = x \n";
			Teammates[idx]->setRect (this->parentScene->rectFromFC (this->currentSWIM.teammateposition (idx).pose().x() * 1000, this->currentSWIM.teammateposition (idx).pose().y() * 1000, 150, 150) );
			TeammateDirections[idx]->setLine (this->parentScene->lineFromFCA (this->currentSWIM.teammateposition (idx).pose().x() * 1000, this->currentSWIM.teammateposition (idx).pose().y() * 1000, this->currentSWIM.teammateposition (idx).pose().phi(), 200) );
			setTeammateVisible (idx, true);
		}
	}
}

void GraphicalRobotElement::setBallVisible (bool visible) {
	if (visible == false) {
		this->Ball->setVisible (false);
	} else {
		this->Ball->setVisible (true);
	}
}

void GraphicalRobotElement::updateBallRect() {
	if ( (this->currentWIM.balls_size() > 0) && this->currentWIM.has_myposition() ) {
		Ball->setRect (this->parentScene->ballRectFromFC (&currentWIM, 75, 75) );
	} else {
		Ball->setRect (0, 0, 0, 0);
	}
}

void GraphicalRobotElement::setUnionistLineVisible (bool visible) {
	if (visible == false) {
		this->UnionistLine->setVisible (false);
	} else {
		this->UnionistLine->setVisible (true);
	}
}

void GraphicalRobotElement::updateUnionistLineRect() {
	if ( (this->currentWIM.balls_size() > 0) && this->currentWIM.has_myposition() ) {
		UnionistLine->setLine (this->parentScene->unionistLineRectFromFC (&currentWIM) );
	} else {
		UnionistLine->setLine (0, 0, 0, 0);
	}
}

void GraphicalRobotElement::setVisionBallVisible (bool visible) {
	if (visible == false) {
		this->VisionBall->setVisible (false);
	} else {
		this->VisionBall->setVisible (true);
	}
}

void GraphicalRobotElement::updateVisionBallRect (ObservationMessage obm) {
	if ( obm.has_ball() && currentWIM.has_myposition() ) {
		VisionBall->setRect (this->parentScene->visionBallRect (obm.ball(), currentWIM) );
	} else {
		VisionBall->setRect (0, 0, 0, 0);
	}
}

void GraphicalRobotElement::setYellowLeftPostVisible (bool visible) {
	if (visible == false) {
		this->LeftYellowPost->setVisible (false);
	} else {
		this->LeftYellowPost->setVisible (true);
	}
}

void GraphicalRobotElement::setYellowRightPostVisible (bool visible) {
	if (visible == false) {
		this->RightYellowPost->setVisible (false);
	} else {
		this->RightYellowPost->setVisible (true);
	}
}

void GraphicalRobotElement::setYellowPostVisible (bool visible) {
	if (visible == false) {
		this->YellowPost->setVisible (false);
	} else {
		this->YellowPost->setVisible (true);
	}
}

void GraphicalRobotElement::updateGoalPostsRect() {
	if (currentObsm.regular_objects_size() > 0 && currentWIM.has_myposition() ) {
		for (unsigned o = 0; o < currentObsm.regular_objects_size(); o++) {
			NamedObject *obj = currentObsm.mutable_regular_objects (o);
			QRectF rect = parentScene->goalPostRectFromOBM ( obj, &currentWIM);

			if (obj->object_name() == "YellowLeft") {
				LeftYellowPost->setRect (rect);
				tagVisionObservations (LeftYellowPost, rect, "L");
			} else if (obj->object_name() == "YellowRight") {
				RightYellowPost->setRect (rect);
				tagVisionObservations (RightYellowPost, rect, "R");
			} else if (obj->object_name() == "Yellow") {
				YellowPost->setRect (rect);
				tagVisionObservations (YellowPost, rect, "A");
			}
		}
	} else {
		LeftYellowPost->setRect (0, 0, 0, 0);
		RightYellowPost->setRect (0, 0, 0, 0);
		YellowPost->setRect (0, 0, 0, 0);
	}
}

void GraphicalRobotElement::tagVisionObservations (QGraphicsEllipseItem *post, QRectF rect, QString text) {
	QBrush brush;
	QPixmap pix (700, 700);
	pix.fill (Qt::yellow);
	QPainter paint (&pix);
	paint.setPen ("black");
	paint.drawText (rect, Qt::AlignCenter , text);
	brush.setTexture (pix);
	post->setBrush (brush);
}

void GraphicalRobotElement::clearVisionObservations() {
	GREtimer->stop();

	if (this->VisionBall->isVisible() ) {
		this->VisionBall->setVisible (false);
		this->VisionBall->setRect (0, 0, 0, 0);
	}

	if (this->YellowPost->isVisible() ) {
		this->YellowPost->setVisible (false);
		this->YellowPost->setRect (0, 0, 0, 0);
	}

	if (this->LeftYellowPost->isVisible() ) {
		this->LeftYellowPost->setVisible (false);
		this->LeftYellowPost->setRect (0, 0, 0, 0);
	}

	if (this->RightYellowPost->isVisible() ) {
		this->RightYellowPost->setVisible (false);
		this->RightYellowPost->setRect (0, 0, 0, 0);
	}
}


void GraphicalRobotElement::setParticlesVisible (bool visible) {
	for (unsigned i = 0; i < ParticlesList.count(); i++) {
		if (visible == false) {
			ParticlesList.at (i)->Pose->setVisible (false);
			ParticlesList.at (i)->Direction->setVisible (false);
		} else {
			ParticlesList.at (i)->Pose->setVisible (true);
			ParticlesList.at (i)->Direction->setVisible (true);
		}
	}
}

void GraphicalRobotElement::updateParticlesRect (LocalizationDataForGUI debugGUI) {
	for (unsigned i = 0; i < debugGUI.particles_size(); i++) {
		RobotPose particle = debugGUI.particles (i);

		if (particle.has_x() && particle.has_y() && particle.has_phi() ) {
			if (teamColor == 1) {
				ParticlesList.at (i)->Pose->setRect (this->parentScene->rectFromFC ( particle.x(),
				        particle.y(), 50, 50) );
				ParticlesList.at (i)->Direction->setLine (this->parentScene->lineFromFCA (particle.x(),
				        particle.y(), particle.phi(), 200) );
			} else {
				ParticlesList.at (i)->Pose->setRect (this->parentScene->rectFromFC ( teamColor * particle.x(),
				        teamColor * particle.y(), 50, 50) );
				ParticlesList.at (i)->Direction->setLine (this->parentScene->lineFromFCA (teamColor * particle.x(),
				        teamColor * particle.y(), particle.phi() + Pi, 200) );
			}
		} else {
			ParticlesList.at (i)->Pose->setRect ( 0, 0, 0, 0);
			ParticlesList.at (i)->Direction->setLine (0, 0, 0, 0);
		}
	}
}

void GraphicalRobotElement::setHFOVVisible (bool visible) {
	if (visible == false) {
		this->HFOVLines->setVisible (false);
	} else {
		this->HFOVLines->setVisible (true);
	}
}

void GraphicalRobotElement::updateHFOVRect() {
	QPolygonF poly;

	if (this->currentWIM.has_myposition() ) {
		for (int i = 0; i < currentObsm.view_limit_points_size(); i++) {
			const PointObject p = currentObsm.view_limit_points (i);
			QLineF l = this->parentScene->lineFromFCA (
			               this->currentWIM.myposition().x() * 1000,
			               this->currentWIM.myposition().y() * 1000,
			               this->currentWIM.myposition().phi() + p.bearing(),
			               p.distance() * 1000);
			poly << l.p2();
			HFOVLines->setPolygon (poly);
		}
	} else {
		HFOVLines->setPolygon (QPolygon() );
	}
}

void GraphicalRobotElement::setTraceVisible (bool visible) {
	boost::circular_buffer<QGraphicsEllipseItem *>::iterator P_it;
	boost::circular_buffer<QGraphicsLineItem *>::iterator L_it;

	for (P_it = RobotPositions.begin(); P_it != RobotPositions.end(); ++P_it) {
		if (visible == false) {
			(*P_it)->setVisible (false);
		} else {
			(*P_it)->setVisible (true);
		}
	}

	for (L_it = UnionistLines.begin(); L_it != UnionistLines.end(); ++L_it) {
		if (visible == false) {
			(*L_it)->setVisible (false);
		} else {
			(*L_it)->setVisible (true);
		}
	}
}

void GraphicalRobotElement::updateTraceRect() {
	QGraphicsEllipseItem *Position;
	QGraphicsLineItem *UnLine;
	int RobotPositionsSize;
	QRectF rect0;
	QRectF rect1;
	QPen penForTraceLine (Qt::black);
	penForTraceLine.setWidth (1);

	if (currentWIM.has_myposition() ) {
		Position = this->parentScene->addEllipse (QRect(), QPen (Qt::black), QBrush (Qt::magenta) );
		Position->setRect (this->parentScene->rectFromFC ( currentWIM.myposition().x() * 1000,
		                   currentWIM.myposition().y() * 1000, 40, 40) );
		RobotPositions.push_back (Position);
		RobotPositionsSize = RobotPositions.size();

		if (RobotPositionsSize > 1) {
			rect0 = RobotPositions[RobotPositionsSize-1]->rect();
			rect1 = RobotPositions[RobotPositionsSize-2]->rect();
			UnLine = this->parentScene->addLine (QLineF(), penForTraceLine);
			UnLine->setLine (QLineF (rect0.x(), rect0.y(), rect1.x(), rect1.y() ) );
			UnionistLines.push_back (UnLine);
		}
	}
}

void GraphicalRobotElement::setMWCmdVisible (bool visible) {
	if (visible == false) {
		GotoPositionLine->setVisible (false);
		GotoArrow->setVisible (false);
		zAxisArc->setVisible (false);
		//zAxisArcArrow->setVisible(false);
	} else {
		GotoPositionLine->setVisible (true);
		GotoArrow->setVisible (true);
		zAxisArc->setVisible (true);
		//zAxisArcArrow->setVisible(true);
	}
}

void GraphicalRobotElement::updateMWCmdRect (MotionWalkMessage wmot) {
	QPolygonF arrowHead, arrowHead1;
	QLineF arrowLine;
	double angleOrient;
	double angleArrow;
	int startAngle;
	int spanAngle;
	/*	std::cout << "cx :: " << wmot.parameter(0) << std::endl;
		std::cout << "cy :: " << wmot.parameter(1) << std::endl;
		std::cout << "ct :: " << wmot.parameter(2) << std::endl;
		std::cout << "f :: " << wmot.parameter(3) << std::endl;*/
	arrowHead.clear();
	arrowHead1.clear();

	if (this->currentWIM.has_myposition() ) {
		arrowLine = this->parentScene->motionCmdRectFromFC (&currentWIM, wmot.parameter (0), wmot.parameter (1) );
		arrowHead = calculateArrowHeadPosition (arrowLine);
		angleOrient = ToDegrees * currentWIM.myposition().phi();
		angleArrow =  MAX_ROT_ARC_ANGLE_DEG * wmot.parameter (2);
		startAngle = angleOrient * 16;
		spanAngle = angleArrow * 16;
		GotoPositionLine->setLine (arrowLine);
		GotoArrow->setPolygon (arrowHead);
		zAxisArc->setRect (this->parentScene->rectFromFC ( this->currentWIM.myposition().x() * 1000,
		                   this->currentWIM.myposition().y() * 1000, 1500, 1500) );
		zAxisArc->setStartAngle (startAngle);
		zAxisArc->setSpanAngle (spanAngle);
	} else {
		GotoPositionLine->setLine (0, 0, 0, 0);
		GotoArrow->setPolygon (arrowHead);
		zAxisArc->setRect ( 0, 0, 0, 0);
	}
}

QPolygonF GraphicalRobotElement::calculateArrowHeadPosition (QLineF aLine) {
	int arrowSize = 10;
	QPolygonF polyF;
	QLineF Line;
	Line.setP1 (aLine.p2() );
	Line.setP2 (aLine.p1() );
	double angle = ::acos (Line.dx() / Line.length() );

	if (Line.dy() >= 0) {
		angle = (Pi * 2) - angle;
	}

	QPointF arrowP1 = Line.p1() + QPointF (sin (angle + Pi / 3) * arrowSize,
	                  cos (angle + Pi / 3) * arrowSize);
	QPointF arrowP2 = Line.p1() + QPointF (sin (angle + Pi - Pi / 3) * arrowSize,
	                  cos (angle + Pi - Pi / 3) * arrowSize);
	polyF.clear();
	polyF << Line.p1() << arrowP1 << arrowP2;
	return polyF;
}

void GraphicalRobotElement::clearMotionWalkCommand() {
	if (this->GotoPositionLine->isVisible() ) {
		this->GotoPositionLine->setVisible (false);
		this->GotoArrow->setVisible (false);
		this->zAxisArc->setVisible (false);
		this->GotoPositionLine->setLine (0, 0, 0, 0);
		this->GotoArrow->setPolygon (QPolygon() );
		this->zAxisArc->setRect (0, 0, 0, 0);
	}

	MWCmdTimer->stop();
}

