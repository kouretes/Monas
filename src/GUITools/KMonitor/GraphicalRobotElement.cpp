#include "GraphicalRobotElement.h"

#include "core/architecture/configurator/Configurator.hpp"

#include "tools/toString.h"

#include <math.h>
#include <iostream>

#define TO_RAD 0.01745329f
const qreal Pi = 3.14;
#define ToDegrees	(180.0/Pi)

using namespace std;

GraphicalRobotElement::GraphicalRobotElement(KFieldScene *parent, QString host) {
	this->parentScene = parent;
	hostId = host;
	teamColor = 1;
	currentWIM.Clear();
	currentSWIM.Clear();
	currentObsm.Clear();
	ParticlesList.clear();
	PositionsList.clear();
	RobotPositions.set_capacity(100);
	UnionistLines.set_capacity(99);
	GWSRobotVisible = false;
	GWSBallVisible = false;
	GWSUnionistLineVisible = false;
	LWSRobotVisible = false;
	LWSBallVisible = false;
	LWSUnionistLineVisible = false;
	LWSVisionBallVisible = false;
	LWSFormationVisible = false;
	LWSVisionYellowLeftPostVisible = false;
	LWSVisionYellowRightPostVisible = false;
	LWSVisionYellowPostVisible = false;
	LWSParticlesVisible = false;
	LWSHFOVVisible = false;

	LWSMWCmdVisible = false;
    LWSVarianceVisible = false;


    LWSOdometryVisible = false;
    LWSRobotTraceVisible = false;


    LWSEkfMhypothesisVisible = false;
	LWSTeammatesVisible = false;

    QPen penForUnionistLine(Qt::black);
	penForUnionistLine.setWidth(1);
	QPen penForRobotDirection(Qt::black);
	penForRobotDirection.setWidth(3);
	QPen penForMotionCmdLine(Qt::darkRed);
	penForMotionCmdLine.setWidth(2);
	loadXMLlocalizationConfigParameters();
	loadXMLteamConfigParameters();

	for(int it = 0 ; it < partclsNum ; it++) {
		Particle *part = new GUIRobotPose();
		part->Direction = this->parentScene->addLine(QLineF(), QPen(Qt::black));
		part->Pose = this->parentScene->addEllipse(QRect(), QPen(Qt::cyan), QBrush(Qt::cyan));
		ParticlesList.append(part);
	}
	
	formationBall = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(QColor(255, 140, 0)));
	
	for(int it = 0 ; it < numOfPlayers ; it++) {
		QGraphicsEllipseItem *pos = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::gray));
		PositionsList.append(pos);
	}

	HFOVLines = this->parentScene->addPolygon(QPolygonF(), QPen(Qt::darkCyan), QBrush(Qt::Dense7Pattern));
	GotoPositionLine = this->parentScene->addLine(QLineF(), penForMotionCmdLine);
	GotoArrow = this->parentScene->addPolygon(QPolygonF(), QPen(Qt::darkRed), QBrush(Qt::darkRed));
	zAxisArc = this->parentScene->addEllipse(QRect(), penForMotionCmdLine, QBrush(Qt::transparent));
	UnionistLine = this->parentScene->addLine(QLineF(), penForUnionistLine);
	SharedUnionistLine = this->parentScene->addLine(QLineF(), penForUnionistLine);
	RobotDirection = this->parentScene->addLine(QLineF(), penForRobotDirection);
	Robot = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::darkGray));
	Ball = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::darkGray));
	sharedBall = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::cyan));
	Teammates[0] = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::magenta));
	Teammates[1] = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::green));
	Teammates[2] = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::blue));
	Teammates[3] = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::red));
	Teammates[4] = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::cyan));
	TeamBall = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::magenta));

    robotTraceTime = boost::posix_time::microsec_clock::universal_time();

    PoseHypothesis = new QGraphicsEllipseItem*[ekfMaxHypothesis];
    PoseHypothesisDirections = new QGraphicsLineItem*[ekfMaxHypothesis];

	for(int it = 0 ; it < ekfMaxHypothesis ; it++)
        PoseHypothesis[it] = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::magenta));
    
    for(int i = 0 ; i < ekfMaxHypothesis ; i++)
		PoseHypothesisDirections[i] = this->parentScene->addLine(QLineF(), penForRobotDirection);

    PositionUncertainty = this->parentScene->addEllipse(QRect(), QPen(Qt::red, 3), QBrush(Qt::NoBrush));
    AngleUncertainty = this->parentScene->addEllipse(QRect(), QPen(Qt::red, 3), QBrush(Qt::red));

    OdometryPoints = new QPainterPath();
    Odometry = this->parentScene->addPath(*OdometryPoints, QPen(Qt::blue, 1), QBrush(Qt::NoBrush));

	RobotTracePoints = new QPainterPath();
    RobotTrace = this->parentScene->addPath(*RobotTracePoints, QPen(Qt::red, 2), QBrush(Qt::NoBrush));

    for(int i = 0 ; i < numOfRobots ; i++)
		TeammateDirections[i] = this->parentScene->addLine(QLineF(), penForRobotDirection);

	VisionBall = this->parentScene->addEllipse(QRect(), QPen(Qt::black), QBrush(Qt::white));
	LeftYellowPost = this->parentScene->addEllipse(QRect(), QPen(Qt::yellow), QBrush(Qt::yellow));
	RightYellowPost = this->parentScene->addEllipse(QRect(), QPen(Qt::yellow), QBrush(Qt::yellow));
	YellowPost = this->parentScene->addEllipse(QRect(), QPen(Qt::yellow), QBrush(Qt::yellow));
	GREtimer = new QTimer();
	connect(GREtimer, SIGNAL(timeout()), this, SLOT(clearVisionObservations()) );
	MWCmdTimer = new QTimer();
	connect(MWCmdTimer, SIGNAL(timeout()), this, SLOT(clearMotionWalkCommand()) );
}


GraphicalRobotElement::~GraphicalRobotElement() {

	if(HFOVLines)
		delete HFOVLines;

	if(UnionistLine)
		delete UnionistLine;

	if(SharedUnionistLine)
		delete SharedUnionistLine;
		
	if(RobotDirection)
		delete RobotDirection;

	if(Robot)
		delete Robot;

	for(int i = 0 ; i < numOfRobots ; i++) {
		if(Teammates[i])
			delete Teammates[i];

		if(TeammateDirections[i])
			delete TeammateDirections[i];
	}
	
	if(TeamBall)
		delete TeamBall;

	if(Ball)
		delete Ball;
	
	if(sharedBall)
		delete sharedBall;

	if(VisionBall)
		delete VisionBall;

	if(YellowPost)
		delete YellowPost;

	if(LeftYellowPost)
		delete LeftYellowPost;

	if(RightYellowPost)
		delete RightYellowPost;

	if(GotoPositionLine)
		delete GotoPositionLine;

	if(GotoArrow)
		delete GotoArrow;

	if(zAxisArc)
		delete zAxisArc;

	if(formationBall)
		delete formationBall;

	for(int i = 0 ; i < ParticlesList.count() ; i++) {
		if(ParticlesList.at(i)->Pose)
			delete ParticlesList.at(i)->Pose;

		if(ParticlesList.at(i)->Direction)
			delete ParticlesList.at(i)->Direction;

		if(ParticlesList.at(i))
			delete ParticlesList.at(i);
	}

	for(int i = 0 ; i < PositionsList.count() ; i++) {
		if(PositionsList.at(i))
			delete PositionsList.at(i);
	}
	
	boost::circular_buffer<QGraphicsEllipseItem*>::iterator P_it;

	for(P_it = RobotPositions.begin() ; P_it != RobotPositions.end() ; ++P_it) {
		if((*P_it))
			delete(*P_it);
	}

	boost::circular_buffer<QGraphicsLineItem*>::iterator L_it;

	for(L_it = UnionistLines.begin() ; L_it != UnionistLines.end() ; ++L_it) {
		if((*L_it))
			delete(*L_it);
	}

    if(PositionUncertainty)
        delete PositionUncertainty;    

    if(AngleUncertainty)
        delete AngleUncertainty;    

    if(OdometryPoints)
        delete OdometryPoints;    

    if(Odometry)
        delete Odometry;    

    if(RobotTracePoints)
        delete RobotTracePoints;    

    if(RobotTrace)
        delete RobotTrace;    

	if (PoseHypothesis){
        for ( int i = 0; i <ekfMaxHypothesis; ++i){
            delete PoseHypothesis[i];
        }
        delete PoseHypothesis;
    }

    if (PoseHypothesisDirections){
        for ( int i = 0; i < ekfMaxHypothesis; ++i ){
            delete PoseHypothesisDirections[i];
        }
        delete PoseHypothesisDirections;
    }	
		
}

void GraphicalRobotElement::loadXMLlocalizationConfigParameters() {
	partclsNum = atoi(Configurator::Instance().findValueForKey("localizationConfig.partclsNum").c_str());
	ekfMaxHypothesis = atoi(Configurator::Instance().findValueForKey("localizationConfig.EkfMaxHypothesis").c_str());
}

void GraphicalRobotElement::loadXMLteamConfigParameters() {
	numOfPlayers = atoi(Configurator::Instance().findValueForKey("teamConfig.team_max_players").c_str());
}

void GraphicalRobotElement::setCurrentWIM(WorldInfo nwim) {
	currentWIM.Clear();
    currentWIM = nwim;

    if(LWSRobotTraceVisible) {
		setLWSRobotTraceVisible(false);
		updateRobotTracePolygon();
		setLWSRobotTraceVisible(true);
	} 
	else {
		setLWSRobotTraceVisible(false);
		updateRobotTracePolygon();
	}
}

void GraphicalRobotElement::setCurrentSWIM(SharedWorldInfo nswim) {
	currentSWIM.Clear();
	currentSWIM = nswim;
}

void GraphicalRobotElement::setCurrentGSM(GameStateMessage gsm) {
	QString playerNumber = "No Available number";
	playerNumber = QString::fromStdString(_toString((gsm.player_number())));
	Robot->setToolTip(playerNumber);

    currentGSM.Clear();
    currentGSM = gsm;
 
	if(gsm.team_color() == 0) {
		Robot->setBrush(Qt::blue);
		Ball->setBrush(Qt::blue);
		teamColor = 1;
	} 
	else if(gsm.team_color() == 1) {
		Robot->setBrush(Qt::red);
		Ball->setBrush(Qt::red);
		teamColor = -1;
	}
}

void GraphicalRobotElement::setCurrentOBSM(ObservationMessage obm) {
	currentObsm.Clear();
	currentObsm = obm;
}

void GraphicalRobotElement::setCurrentEKFMHypothesisM(EKFMHypothesis ekfMHyp) {
	currentEKFMHypothesis.Clear();
	currentEKFMHypothesis = ekfMHyp;
}

void GraphicalRobotElement::setCurrentOdometryM(OdometryInfoMessage odometryM) {
	currentOdometryM.Clear();
	currentOdometryM = odometryM;
}

void GraphicalRobotElement::setRobotVisible(bool visible) {
	if(visible == false) {
		Robot->setVisible(false);
		RobotDirection->setVisible(false);
	} 
	else {
		Robot->setVisible(true);
		RobotDirection->setVisible(true);
	}
}

void GraphicalRobotElement::setTeammatesVisible(bool visible) {
	for(int i = 0 ; i < numOfRobots ; i++) {
		Teammates[i]->setVisible(visible);
		TeammateDirections[i]->setVisible(visible);
	}
	TeamBall->setVisible(visible);
}

void GraphicalRobotElement::setTeammateVisible(int idx, bool visible) {
	if(visible == false) {
		Teammates[idx]->setVisible(false);
		TeammateDirections[idx]->setVisible(false);
	} 
	else {
		Teammates[idx]->setVisible(true);
		TeammateDirections[idx]->setVisible(true);
	}
}

void GraphicalRobotElement::updateRobotRect() {

	if(this->currentWIM.has_myposition()) {

		Robot->setRect(this->parentScene->rectFromFC(this->currentWIM.myposition().x()*1000,
		                this->currentWIM.myposition().y()*1000, 150, 150));


		RobotDirection->setLine(this->parentScene->lineFromFCA(this->currentWIM.myposition().x()*1000,
		        this->currentWIM.myposition().y()*1000, this->currentWIM.myposition().phi(), 200));
	} 
	else {
		Robot->setRect( 0, 0, 0, 0);
		RobotDirection->setLine(0, 0, 0, 0);
	}
}

void GraphicalRobotElement::updatePoseUncertaintyRect() {

		PositionUncertainty->setRect(this->parentScene->rectFromFC(this->currentWIM.myposition().x()*1000,
		                this->currentWIM.myposition().y()*1000, sqrt(this->currentWIM.myposition().var(0))*1000 , sqrt(this->currentWIM.myposition().var(4))*1000));

        AngleUncertainty->setRect(this->parentScene->rectFromFC(this->currentWIM.myposition().x()*1000,
		                this->currentWIM.myposition().y()*1000, 150, 150));
      
        AngleUncertainty->setStartAngle(KMath::TO_DEG(KMath::wrapTo2Pi(this->currentWIM.myposition().phi()) - sqrt(this->currentWIM.myposition().var(8)))*16);
        AngleUncertainty->setSpanAngle(2*KMath::TO_DEG( sqrt(this->currentWIM.myposition().var(8)))*16);
}

void GraphicalRobotElement::updateEkfMHypothesisRects(){

    int idx;

    //std::cout << "currentEKFMHypothesis size : " << currentEKFMHypothesis.size() << std::endl ;
	if(currentEKFMHypothesis.size() != 0) {

		for(idx = 0 ; idx < currentEKFMHypothesis.size() ; idx++) {

			PoseHypothesis[idx]->setRect(this->parentScene->rectFromFC(this->currentEKFMHypothesis.kmodel(idx).x()*1000, this->currentEKFMHypothesis.kmodel(idx).y()*1000, 150, 150));
            
			PoseHypothesisDirections[idx]->setLine(this->parentScene->lineFromFCA(this->currentEKFMHypothesis.kmodel(idx).x()*1000, this->currentEKFMHypothesis.kmodel(idx).y()* 1000, this->currentEKFMHypothesis.kmodel(idx).phi(), 200));

		}
        for(idx=currentEKFMHypothesis.size() ; idx<ekfMaxHypothesis ; idx++){
            PoseHypothesis[idx]->setRect(0,0,0,0);           
			PoseHypothesisDirections[idx]->setLine(0,0,0,0);
        }

	} 
	else{
        for(int i = 0 ; i < ekfMaxHypothesis ; i++) {
		    PoseHypothesis[i]->setRect(0,0,0,0);           
		    PoseHypothesisDirections[i]->setLine(0,0,0,0);
	    }
    }
}


void GraphicalRobotElement::setEkfMHypothesisVisible(bool visible) {
	for(int i = 0 ; i < ekfMaxHypothesis ; i++) {
		PoseHypothesis[i]->setVisible(visible);
		PoseHypothesisDirections[i]->setVisible(visible);
	}
}

void GraphicalRobotElement::updateOdometryPolygon() {

    QPointF trackPoint = this->parentScene->pointFromFC(this->currentOdometryM.trackpointx()*1000, this->currentOdometryM.trackpointy()*1000);

	if( currentGSM.player_state() == PLAYER_PENALISED ||  currentGSM.player_state() ==  PLAYER_INITIAL) {
        delete(OdometryPoints);
        OdometryPoints = new QPainterPath(trackPoint);
	}
    else {
        OdometryPoints->lineTo(trackPoint);
        Odometry->setPath(*OdometryPoints);
    }
}

void GraphicalRobotElement::updateRobotTracePolygon() {
	
    QPointF robotTracePoint = this->parentScene->pointFromFC(this->currentWIM.myposition().x()*1000,this->currentWIM.myposition().y()*1000);

	if(currentGSM.player_state() == PLAYER_PENALISED || currentGSM.player_state() ==  PLAYER_INITIAL) {
        delete(RobotTracePoints);
        RobotTracePoints = new QPainterPath(robotTracePoint);
	}
    else if(boost::posix_time::microsec_clock::universal_time() >robotTraceTime + boost::posix_time::seconds(5)) {
        RobotTracePoints->lineTo(robotTracePoint);
        RobotTrace->setPath(*RobotTracePoints);
        robotTraceTime =  boost::posix_time::microsec_clock::universal_time();
    } 
}

void GraphicalRobotElement::updateTeammatesRects() {
	int idx;

	if(currentSWIM.teammateposition_size() != 0) {
		//std::cout << currentSWIM.teammateposition_size() << "\n";
		for(idx = 0 ; idx < numOfRobots ; idx++)
			setTeammateVisible(idx, false);

		TeamBall->setVisible(false);
		
		for(idx = 0 ; idx < currentSWIM.teammateposition_size() ; idx++) {
			//std::cout << this->currentSWIM.teammateposition(idx).pose().x() << " = x \n";
			Teammates[idx]->setRect(this->parentScene->rectFromFC(this->currentSWIM.teammateposition(idx).pose().x()*1000, this->currentSWIM.teammateposition(idx).pose().y()* 1000, 150, 150));
			TeammateDirections[idx]->setLine(this->parentScene->lineFromFCA(this->currentSWIM.teammateposition(idx).pose().x()*1000, this->currentSWIM.teammateposition(idx).pose().y()*1000, this->currentSWIM.teammateposition(idx).pose().phi(), 200));
			setTeammateVisible(idx, true);
		}
		
		if(currentSWIM.globalballs_size() != 0) {
			TeamBall->setRect(this->parentScene->rectFromFC(this->currentSWIM.globalballs(0).x()*1000, this->currentSWIM.globalballs(0).y()*1000, 75, 75));
			TeamBall->setVisible(true);
		}
		else
			TeamBall->setRect(0, 0, 0, 0);
	}
}

void GraphicalRobotElement::setBallVisible(bool visible) {
	this->Ball->setVisible(visible);
	this->sharedBall->setVisible(visible);
}

void GraphicalRobotElement::setVarianceVisible(bool visible) {
	if(visible == false) {
		this->PositionUncertainty->setVisible(false);
		this->AngleUncertainty->setVisible(false);
	} 
	else {
		this->PositionUncertainty->setVisible(true);
		this->AngleUncertainty->setVisible(true);
	}
}

void GraphicalRobotElement::setOdometryVisible(bool visible) {
	if(visible == false)
		this->Odometry->setVisible(false);
	else
		this->Odometry->setVisible(true);
}

void GraphicalRobotElement::setRobotTraceVisible(bool visible) {
	if(visible == false)
		this->RobotTrace->setVisible(false);
	else
		this->RobotTrace->setVisible(true);
}

void GraphicalRobotElement::updateBallRect() {
	if((this->currentWIM.balls_size() > 0) && this->currentWIM.has_myposition()) {
		float a = cos(currentWIM.myposition().phi());
		float b = sin(currentWIM.myposition().phi());
		formationBallX =(currentWIM.myposition().x() + currentWIM.balls(0).relativex()*a - currentWIM.balls(0).relativey()*b);
		formationBallY =(currentWIM.myposition().y() + currentWIM.balls(0).relativex()*b + currentWIM.balls(0).relativey()*a);
		Ball->setRect(this->parentScene->ballRectFromFC(&currentWIM, 75, 75));
	} 
	else
		Ball->setRect(0, 0, 0, 0);
		
	if((this->currentSWIM.globalballs_size() > 0)) {
		std::cout << "MPIKA!" << std::endl;
		sharedBall->setRect(this->parentScene->ballRectFromFC(&currentSWIM, 75, 75));
	}
	else
		sharedBall->setRect(0, 0, 0, 0);
}

void GraphicalRobotElement::setUnionistLineVisible(bool visible) {
	this->UnionistLine->setVisible(visible);
	this->SharedUnionistLine->setVisible(visible);
}

void GraphicalRobotElement::updateUnionistLineRect() {
	if((this->currentWIM.balls_size() > 0) && this->currentWIM.has_myposition())
		UnionistLine->setLine(this->parentScene->unionistLineRectFromFC(&currentWIM));
	else
		UnionistLine->setLine(0, 0, 0, 0);
		
	if((this->currentSWIM.globalballs_size() > 0) && this->currentWIM.has_myposition())
		SharedUnionistLine->setLine(this->parentScene->unionistLineRectFromFC(&currentWIM, &currentSWIM));
}

void GraphicalRobotElement::setVisionBallVisible(bool visible) {
	if(visible == false)
		this->VisionBall->setVisible(false);
	else
		this->VisionBall->setVisible(true);
}

void GraphicalRobotElement::updateVisionBallRect(ObservationMessage obm) {
	if( obm.has_ball() && currentWIM.has_myposition())
		VisionBall->setRect(this->parentScene->visionBallRect(obm.ball(), currentWIM));
	else
		VisionBall->setRect(0, 0, 0, 0);
}

void GraphicalRobotElement::setYellowLeftPostVisible(bool visible) {
	if(visible == false)
		this->LeftYellowPost->setVisible(false);
	else
		this->LeftYellowPost->setVisible(true);
}

void GraphicalRobotElement::setYellowRightPostVisible(bool visible) {
	if(visible == false)
		this->RightYellowPost->setVisible(false);
	else
		this->RightYellowPost->setVisible(true);
}

void GraphicalRobotElement::setYellowPostVisible(bool visible) {
	if(visible == false)
		this->YellowPost->setVisible(false);
	else
		this->YellowPost->setVisible(true);
}

void GraphicalRobotElement::updateGoalPostsRect() {

	if(currentObsm.regular_objects_size() > 0 && currentWIM.has_myposition()) {
		for(int o = 0 ; o < currentObsm.regular_objects_size() ; o++) {
			NamedObject *obj = currentObsm.mutable_regular_objects(o);
			QRectF rect = parentScene->goalPostRectFromOBM(obj, &currentWIM);

			if(obj->object_name() == "YellowLeft") {
				LeftYellowPost->setRect(rect);
				tagVisionObservations(LeftYellowPost, rect, "L");
			} 
			else if(obj->object_name() == "YellowRight") {
				RightYellowPost->setRect(rect);
				tagVisionObservations(RightYellowPost, rect, "R");
			} 
			else if(obj->object_name() == "Yellow") {
				YellowPost->setRect(rect);
				tagVisionObservations(YellowPost, rect, "A");
			}
		}
	}
	else {
		LeftYellowPost->setRect(0, 0, 0, 0);
		RightYellowPost->setRect(0, 0, 0, 0);
		YellowPost->setRect(0, 0, 0, 0);
	}
}

void GraphicalRobotElement::tagVisionObservations(QGraphicsEllipseItem *post, QRectF rect, QString text) {
	QBrush brush;
	QPixmap pix(700, 700);
	pix.fill(Qt::yellow);
	QPainter paint(&pix);
	paint.setPen("black");
	paint.drawText(rect, Qt::AlignCenter, text);
	brush.setTexture(pix);
	post->setBrush(brush);
}

void GraphicalRobotElement::tagRoles(QGraphicsEllipseItem *post, QRectF rect, const QString& text, const QColor& color) {
	QBrush brush;
	QPixmap pix(700, 700);
	pix.fill(color);
	QPainter paint(&pix);
	paint.drawText(rect, Qt::AlignCenter, text);
	brush.setTexture(pix);
	post->setBrush(brush);
}

void GraphicalRobotElement::clearVisionObservations() {
	GREtimer->stop();

	if(this->VisionBall->isVisible()) {
		this->VisionBall->setVisible(false);
		this->VisionBall->setRect(0, 0, 0, 0);
	}

	if(this->YellowPost->isVisible()) {
		this->YellowPost->setVisible(false);
		this->YellowPost->setRect(0, 0, 0, 0);
	}

	if(this->LeftYellowPost->isVisible()) {
		this->LeftYellowPost->setVisible(false);
		this->LeftYellowPost->setRect(0, 0, 0, 0);
	}

	if(this->RightYellowPost->isVisible()) {
		this->RightYellowPost->setVisible(false);
		this->RightYellowPost->setRect(0, 0, 0, 0);
	}
}


void GraphicalRobotElement::setParticlesVisible(bool visible) {
	for(int i = 0 ; i < ParticlesList.count() ; i++) {
		if(visible == false) {
			ParticlesList.at(i)->Pose->setVisible(false);
			ParticlesList.at(i)->Direction->setVisible(false);
		} 
		else {
			ParticlesList.at(i)->Pose->setVisible(true);
			ParticlesList.at(i)->Direction->setVisible(true);
		}
	}
}

void GraphicalRobotElement::updateParticlesRect(LocalizationDataForGUI debugGUI) {
	for(int i = 0 ; i < debugGUI.particles_size() ; i++) {
		RobotPose particle = debugGUI.particles(i);

		if(particle.has_x() && particle.has_y() && particle.has_phi()) {
			if(teamColor == 1) {
				ParticlesList.at(i)->Pose->setRect(this->parentScene->rectFromFC(particle.x(),
				        particle.y(), 50, 50));
				ParticlesList.at(i)->Direction->setLine(this->parentScene->lineFromFCA(particle.x(),
				        particle.y(), particle.phi(), 200));
			} 
			else {
				ParticlesList.at(i)->Pose->setRect(this->parentScene->rectFromFC(teamColor*particle.x(),
				        teamColor*particle.y(), 50, 50));
				ParticlesList.at(i)->Direction->setLine(this->parentScene->lineFromFCA(teamColor*particle.x(),
				        teamColor*particle.y(), particle.phi() + Pi, 200));
			}
		} 
		else {
			ParticlesList.at(i)->Pose->setRect(0, 0, 0, 0);
			ParticlesList.at(i)->Direction->setLine(0, 0, 0, 0);
		}
	}
}

void GraphicalRobotElement::setFormationVisible(bool visible) {
	if(visible == false) {
		this->parentScene->getLabel()->setVisible(false);
		formationBall->setVisible(false);
	}
	else {
		this->parentScene->getLabel()->setVisible(true);
		formationBall->setVisible(true);
	}

	for(int i = 0 ; i < PositionsList.count() ; i++) {
		if(visible == false)
			PositionsList.at(i)->setVisible(false);		
		else
			PositionsList.at(i)->setVisible(true);
	}
}

void GraphicalRobotElement::updateFormationRects(FormationDataForGUI debugGUI) {
	
	formationBall->setRect(this->parentScene->rectFromFC( formationBallX*1000, formationBallY*1000, 80, 80));
	this->parentScene->getLabel()->setText("Ball Position:\n"+QString::fromStdString("X: ")+QString::number((formationBallX),'f',3)+QString::fromStdString("\nY: ")
					+QString::number((formationBallY),'f',3)+"\n");
	
	for(int i = 0 ; i < debugGUI.positions_size() ; i++) {
		PositionInfo posInfo = debugGUI.positions(i);

		if(posInfo.has_x() && posInfo.has_y() && posInfo.has_role()) {
			
			PositionsList.at(i)->setRect(this->parentScene->rectFromFC(posInfo.x()*1000, posInfo.y()*1000, 150, 150));
			
			if(posInfo.role() == PositionInfo::GOALIE) {
				tagRoles(PositionsList.at(i), PositionsList.at(i)->rect(), "G", Qt::green);	
			}
			else if(posInfo.role() == PositionInfo::DEFENDER) {			
				tagRoles(PositionsList.at(i), PositionsList.at(i)->rect(), "D", Qt::blue);
			}
			else if(posInfo.role() == PositionInfo::DEFENDER_L) {			
				tagRoles(PositionsList.at(i), PositionsList.at(i)->rect(), "DL", Qt::blue);
			}
			else if(posInfo.role() == PositionInfo::DEFENDER_R) {			
				tagRoles(PositionsList.at(i), PositionsList.at(i)->rect(), "DR", Qt::blue);
			}
			else if(posInfo.role() == PositionInfo::ONBALL) {			
				tagRoles(PositionsList.at(i), PositionsList.at(i)->rect(), "OB", Qt::red);
			}
			else if(posInfo.role() == PositionInfo::SUPPORTER_L) {			
				tagRoles(PositionsList.at(i), PositionsList.at(i)->rect(), "LS", Qt::yellow);
			}
			else if(posInfo.role() == PositionInfo::SUPPORTER) {			
				tagRoles(PositionsList.at(i), PositionsList.at(i)->rect(), "S", Qt::yellow);
			}
			else if(posInfo.role() == PositionInfo::SUPPORTER_R) {
				tagRoles(PositionsList.at(i), PositionsList.at(i)->rect(), "RS", Qt::yellow);
			}
		} 
	}
}

void GraphicalRobotElement::setHFOVVisible(bool visible) {
	if(visible == false)
		this->HFOVLines->setVisible(false);
	else
		this->HFOVLines->setVisible(true);
}

void GraphicalRobotElement::updateHFOVRect() {
	QPolygonF poly;

	if(this->currentWIM.has_myposition()) {
		for(int i = 0 ; i < currentObsm.view_limit_points_size() ; i++) {
			const PointObject p = currentObsm.view_limit_points(i);
			QLineF l = this->parentScene->lineFromFCA(
			               this->currentWIM.myposition().x()*1000,
			               this->currentWIM.myposition().y()*1000,
			               this->currentWIM.myposition().phi() + p.bearing(),
			               p.distance()*1000);
			poly << l.p2();
			HFOVLines->setPolygon(poly);
		}
	} 
	else
		HFOVLines->setPolygon(QPolygon());
}

void GraphicalRobotElement::setMWCmdVisible(bool visible) {
	if(visible == false) {
		GotoPositionLine->setVisible(false);
		GotoArrow->setVisible(false);
		zAxisArc->setVisible(false);
	} 
	else {
		GotoPositionLine->setVisible(true);
		GotoArrow->setVisible(true);
		zAxisArc->setVisible(true);
	}
}

void GraphicalRobotElement::updateMWCmdRect(MotionWalkMessage wmot) {
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

	if(this->currentWIM.has_myposition()) {
		arrowLine = this->parentScene->motionCmdRectFromFC(&currentWIM, wmot.parameter(0), wmot.parameter(1));
		arrowHead = calculateArrowHeadPosition(arrowLine);
		angleOrient = ToDegrees*currentWIM.myposition().phi();
		angleArrow =  MAX_ROT_ARC_ANGLE_DEG * wmot.parameter(2);
		startAngle = angleOrient*16;
		spanAngle = angleArrow*16;
		GotoPositionLine->setLine(arrowLine);
		GotoArrow->setPolygon(arrowHead);
		zAxisArc->setRect(this->parentScene->rectFromFC( this->currentWIM.myposition().x()*1000,
		                   this->currentWIM.myposition().y()*1000, 1500, 1500) );
		zAxisArc->setStartAngle(startAngle);
		zAxisArc->setSpanAngle(spanAngle);
	} 
	else {
		GotoPositionLine->setLine(0, 0, 0, 0);
		GotoArrow->setPolygon(arrowHead);
		zAxisArc->setRect(0, 0, 0, 0);
	}
}

QPolygonF GraphicalRobotElement::calculateArrowHeadPosition(QLineF aLine) {
	int arrowSize = 10;
	QPolygonF polyF;
	QLineF Line;
	Line.setP1(aLine.p2());
	Line.setP2(aLine.p1());
	double angle = ::acos(Line.dx() / Line.length());

	if(Line.dy() >= 0)
		angle =(Pi * 2) - angle;

	QPointF arrowP1 = Line.p1() + QPointF(sin(angle + Pi / 3)*arrowSize,
	                  cos(angle + Pi / 3)*arrowSize);
	QPointF arrowP2 = Line.p1() + QPointF(sin(angle + Pi - Pi / 3)*arrowSize,
	                  cos(angle + Pi - Pi / 3)*arrowSize);
	polyF.clear();
	polyF << Line.p1() << arrowP1 << arrowP2;
	return polyF;
}

void GraphicalRobotElement::clearMotionWalkCommand() {
	if(this->GotoPositionLine->isVisible()) {
		this->GotoPositionLine->setVisible(false);
		this->GotoArrow->setVisible(false);
		this->zAxisArc->setVisible(false);
		this->GotoPositionLine->setLine(0, 0, 0, 0);
		this->GotoArrow->setPolygon(QPolygon());
		this->zAxisArc->setRect(0, 0, 0, 0);
	}
	MWCmdTimer->stop();
}

