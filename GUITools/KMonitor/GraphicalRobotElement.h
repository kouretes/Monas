#ifndef GRAPHICALROBOTELEMENT_H_
#define GRAPHICALROBOTELEMENT_H_

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QString>

#include <QTimer>
#include <QObject>

#include "messages/WorldInfo.pb.h"
#include "messages/Gamecontroller.pb.h"

#include "KFieldScene.h"

class KFieldScene;

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

	void setLWSVisionBallVisible(bool visible){LWSVisionBallVisible = visible; setVisionBallVisible(visible);}
	bool getLWSVisionBallVisible(){return LWSVisionBallVisible;}
	void setVisionBallVisible(bool visible);
	void updateVisionBallRect(ObservationMessage obm);

	void setLWSYellowLeftPostVisible(bool visible){VisionYellowLeftPostVisible = visible; setYellowLeftPostVisible(visible);}
	bool getLWSYellowLeftPostVisible(){return VisionYellowLeftPostVisible;}
	void setYellowLeftPostVisible(bool visible);
	void updateYellowLeftPostRect();

	void setLWSYellowRightPostVisible(bool visible){VisionYellowRightPostVisible = visible; setYellowRightPostVisible(visible);}
	bool getLWSYellowRightPostVisible(){return VisionYellowRightPostVisible;}
	void setYellowRightPostVisible(bool visible);
	//void updateYellowRightPostRect();

	void setLWSBlueLeftPostVisible(bool visible){VisionBlueLeftPostVisible = visible; setBlueLeftPostVisible(visible);}
	bool getLWSBlueLeftPostVisible(){return VisionBlueLeftPostVisible;}
	void setBlueLeftPostVisible(bool visible);
	//void updateBlueLeftPostRect();

	void setLWSBlueRightPostVisible(bool visible){VisionBlueRightPostVisible = visible; setBlueRightPostVisible(visible);}
	bool getLWSBlueRightPostVisible(){return VisionBlueRightPostVisible;}
	void setBlueRightPostVisible(bool visible);
	//void updateYellowRightPostRect();

	QTimer* getGREtimer(){return GREtimer;}




private slots:
	void resetVisionObservations();

private:

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

	bool VisionYellowLeftPostVisible;
	QGraphicsEllipseItem* YellowPost;
	QGraphicsLineItem* LeftYellowPost;

	bool VisionYellowRightPostVisible;
	QGraphicsEllipseItem* YellowPost1;
	QGraphicsLineItem* RightYellowPost;

	bool VisionBlueLeftPostVisible;
	QGraphicsEllipseItem* BluePost;
	QGraphicsLineItem* LeftBluePost;

	bool VisionBlueRightPostVisible;
	QGraphicsEllipseItem* BluePost1;
	QGraphicsLineItem* RightBluePost;

	QTimer *GREtimer;


};
#endif /* GRAPHICALROBOTELEMENT_H_ */
