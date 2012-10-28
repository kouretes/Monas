#ifndef GRAPHICALROBOTELEMENT_H_
#define GRAPHICALROBOTELEMENT_H_

#define MAX_ROT_ARC_ANGLE_DEG  180

#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QString>
#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QList>
#include <QPainter>

#include <boost/circular_buffer.hpp>
#include "messages/WorldInfo.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/motion.pb.h"

#include "KFieldScene.h"

class KFieldScene;

typedef struct GUIRobotPose{

	QGraphicsEllipseItem* Pose;
	QGraphicsLineItem* Direction;

}Particle;

class GraphicalRobotElement : public QObject
{
	Q_OBJECT

public:
	GraphicalRobotElement(KFieldScene* parent, QString host);
	~GraphicalRobotElement();

	QString getHostId(){return hostId;}

	void setCurrentWIM(WorldInfo nwim);
	void setCurrentSWIM(SharedWorldInfo nswim);
	void setCurrentGSM(GameStateMessage gsm);
	void setcurrentOBSM(ObservationMessage obm);

	void setGWSRobotVisible(bool visible){GWSRobotVisible = visible; setRobotVisible(visible);}
	bool getGWSRobotVisible(){return GWSRobotVisible;}
	void setLWSRobotVisible(bool visible){LWSRobotVisible = visible; setRobotVisible(visible);}
	bool getLWSRobotVisible(){return LWSRobotVisible;}
	void setLWSTeammatesVisible(bool visible){LWSTeammatesVisible = visible; setTeammatesVisible(visible);}
	bool getLWSTeammatesVisible(){return LWSTeammatesVisible;}
	void setRobotVisible(bool visible);
	void setTeammateVisible(int idx, bool visible);
	void setTeammatesVisible(bool visible);
	void updateRobotRect();
	void updateTeammatesRects();

	void setGWSBallVisible(bool visible){GWSBallVisible = visible; setBallVisible(visible);}
	bool getGWSBallVisible(){return GWSBallVisible;}
	void setLWSBallVisible(bool visible){LWSBallVisible = visible; setBallVisible(visible);}
	bool getLWSBallVisible(){return LWSBallVisible;}
	void setBallVisible(bool visible);
	void updateBallRect();

	void setGWSUnionistLineVisible(bool visible){GWSUnionistLineVisible = visible; setUnionistLineVisible(visible);}
	bool getGWSUnionistLineVisible(){return GWSUnionistLineVisible;}
	void setLWSUnionistLineVisible(bool visible){LWSUnionistLineVisible = visible; setUnionistLineVisible(visible);}
	bool getLWSUnionistLineVisible(){return LWSUnionistLineVisible;}
	void setUnionistLineVisible(bool visible);
	void updateUnionistLineRect();

	// Vision observations
	void setLWSVisionBallVisible(bool visible){LWSVisionBallVisible = visible; setVisionBallVisible(visible);}
	bool getLWSVisionBallVisible(){return LWSVisionBallVisible;}
	void setVisionBallVisible(bool visible);
	//void updateVisionBallRect();
	void updateVisionBallRect(ObservationMessage obm);

	void setLWSYellowLeftPostVisible(bool visible){LWSVisionYellowLeftPostVisible = visible; setYellowLeftPostVisible(visible);}
	bool getLWSYellowLeftPostVisible(){return LWSVisionYellowLeftPostVisible;}
	void setYellowLeftPostVisible(bool visible);
	void updateGoalPostsRect();

	void setLWSYellowRightPostVisible(bool visible){LWSVisionYellowRightPostVisible = visible; setYellowRightPostVisible(visible);}
	bool getLWSYellowRightPostVisible(){return LWSVisionYellowRightPostVisible;}
	void setYellowRightPostVisible(bool visible);

	void setLWSYellowPostVisible(bool visible){LWSVisionYellowPostVisible = visible; setYellowPostVisible(visible);}
	bool getLWSYellowPostVisible(){return LWSVisionYellowPostVisible;}
	void setYellowPostVisible(bool visible);

	// Particles of Localization
	void setLWSParticlesVisible(bool visible){LWSParticlesVisible = visible; setParticlesVisible(visible);}
	bool getLWSParticlesVisible(){return LWSParticlesVisible;}
	void setParticlesVisible(bool visible);
	void updateParticlesRect(LocalizationDataForGUI debugGUI);

	// HFOV
	void setLWSHFOVVisible(bool visible){LWSHFOVVisible = visible; setHFOVVisible(visible);}
	bool getLWSHFOVVisible(){return LWSHFOVVisible;}
	void setHFOVVisible(bool visible);
	void updateHFOVRect();

	// Trace
	void setLWSTraceVisible(bool visible){LWSTraceVisible = visible; setTraceVisible(visible);}
	bool getLWSTraceVisible(){return LWSTraceVisible;}
	void setTraceVisible(bool visible);

	// Motion Walk Command
	void setLWSMWCmdVisible(bool visible){LWSMWCmdVisible = visible; setMWCmdVisible(visible);}
	bool getLWSMWCmdVisible(){return LWSMWCmdVisible;}
	void setMWCmdVisible(bool visible);
	void updateMWCmdRect(MotionWalkMessage wmot);

	QTimer* getGREtimer(){return GREtimer;}
	QTimer* getMWCmdTimer(){return MWCmdTimer;}

public slots:
	void clearVisionObservations();

private slots:
	void clearMotionWalkCommand();

private:
	void loadXMLConfigParameters(std::string fname);
	void tagVisionObservations(QGraphicsEllipseItem* post,QRectF rect, QString text);
	QPolygonF calculateArrowHeadPosition(QLineF aLine);
	void updateTraceRect();


	KFieldScene* parentScene;

	QString hostId;
	int teamColor;	//1 for blue, -1 for red

	WorldInfo currentWIM;
	SharedWorldInfo currentSWIM;

	bool GWSRobotVisible;
	bool LWSRobotVisible, LWSTeammatesVisible;
	static const int numOfRobots = 5;
	QGraphicsEllipseItem* Robot, *Teammates[numOfRobots];
	QGraphicsLineItem* RobotDirection, *TeammateDirections[numOfRobots];

	bool GWSBallVisible;
	bool LWSBallVisible;
	QGraphicsEllipseItem* Ball;

	bool GWSUnionistLineVisible;
	bool LWSUnionistLineVisible;
	QGraphicsLineItem* UnionistLine;

	ObservationMessage currentObsm;

	bool LWSVisionBallVisible;
	QGraphicsEllipseItem* VisionBall;

	bool LWSVisionYellowLeftPostVisible;
	QGraphicsEllipseItem* LeftYellowPost;

	bool LWSVisionYellowRightPostVisible;
	QGraphicsEllipseItem* RightYellowPost;

	bool LWSVisionYellowPostVisible;
	QGraphicsEllipseItem* YellowPost;

	bool LWSParticlesVisible;
	QList<Particle*> ParticlesList;
	int partclsNum;

	bool LWSHFOVVisible;
	QGraphicsPolygonItem* HFOVLines;

	bool LWSTraceVisible;
	boost::circular_buffer<QGraphicsEllipseItem*> RobotPositions;
	boost::circular_buffer<QGraphicsLineItem*> UnionistLines;

	bool LWSMWCmdVisible;
	QGraphicsLineItem* GotoPositionLine;
	QGraphicsPolygonItem* GotoArrow;
	QGraphicsEllipseItem* zAxisArc;
	//QGraphicsPolygonItem* zAxisArcArrow;

	QTimer* GREtimer;
	QTimer* MWCmdTimer;
};
#endif /* GRAPHICALROBOTELEMENT_H_ */
