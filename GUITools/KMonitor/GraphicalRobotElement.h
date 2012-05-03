#ifndef GRAPHICALROBOTELEMENT_H_
#define GRAPHICALROBOTELEMENT_H_

#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QString>
#include <QList>

#include <QTimer>
#include <QObject>

#include "messages/WorldInfo.pb.h"
#include "messages/Gamecontroller.pb.h"

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
	void setCurrentGSM(GameStateMessage gsm);
	void setcurrentOBSM(ObservationMessage obm);

	void setGWSRobotVisible(bool visible){GWSRobotVisible = visible; setRobotVisible(visible);}
	bool getGWSRobotVisible(){return GWSRobotVisible;}
	void setRobotVisible(bool visible);
	void updateRobotRect();

	void setGWSBallVisible(bool visible){GWSBallVisible = visible; setBallVisible(visible);}
	bool getGWSBallVisible(){return GWSBallVisible;}
	void setBallVisible(bool visible);
	void updateBallRect();

	void setGWSUnionistLineVisible(bool visible){GWSUnionistLineVisible = visible; setUnionistLineVisible(visible);}
	bool getGWSUnionistLineVisible(){return GWSUnionistLineVisible;}
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
	void updateHFOVRect(float HeadYaw);

	QTimer* getGREtimer(){return GREtimer;}

private slots:
	void clearVisionObservations();

private:
	void tagVisionObservations(QGraphicsEllipseItem* post,QRectF rect, QString text);


	KFieldScene* parentScene;

	QString hostId;

	WorldInfo currentWIM;

	bool GWSRobotVisible;
	QGraphicsEllipseItem* Robot;
	QGraphicsLineItem* RobotDirection;

	bool GWSBallVisible;
	QGraphicsEllipseItem* Ball;

	bool GWSUnionistLineVisible;
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

	bool LWSHFOVVisible;
	QGraphicsLineItem* PositiveBoundLine;
	QGraphicsLineItem* NegativeBoundLine;

	QTimer* GREtimer;



};
#endif /* GRAPHICALROBOTELEMENT_H_ */
