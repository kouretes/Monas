#ifndef KFIELDSCENE_H
#define KFIELDSCENE_H

#include <QGraphicsScene>
#include <QList>
#include <QLabel>

#include "tools/XMLConfig.h"
#include "messages/WorldInfo.pb.h"

#include "GraphicalRobotElement.h"

#include "messages/VisionObservations.pb.h"

class GraphicalRobotElement;
class KGraphicsView;

class KFieldScene : public QGraphicsScene {

public:
	KFieldScene(QGraphicsView *parent);
	~KFieldScene();

	QList<GraphicalRobotElement*> getRobotList() {
		return RobotList;
	}
	
	QGraphicsView* getParentGraphicsView() {
		return parent;
	}
	
	QLabel* getLabel() { 
		return formationLabel; 
	}
	
	void resizeFieldScene(int width, int height);

	void printRobotList();
	GraphicalRobotElement* findGraphicalRobotItem(QString hostId );
	GraphicalRobotElement* newGraphicalRobotItem(QString hostId);
	void removeGraphicalRobotItem(QString hostId);

	QRectF rectFromFC(float xMiddle, float yMiddle, float width, float height);
	QLineF lineFromFCA(float x, float y, float degAngle, float size);
	QRectF ballRectFromFC(WorldInfo *wim, float width, float height);
	QLineF lineRectFromFC(float x1, float y1, float x2, float y2);
	QLineF unionistLineRectFromFC(WorldInfo *wim);
    QPointF pointFromFC(float x, float y);
	QRectF visionBallRect( BallObject bob, WorldInfo wim);
	QRectF goalPostRectFromOBM(NamedObject *nob, WorldInfo *wim);
	QLineF motionCmdRectFromFC(WorldInfo *wim, float cx, float cy);
	
private:
	QLabel *formationLabel;
	
	QGraphicsRectItem *LSide;
	QGraphicsRectItem *RSide;
	QGraphicsRectItem *LSmallArea;
	QGraphicsRectItem *RSmallArea;
	QGraphicsRectItem *LGoalArea;
	QGraphicsRectItem *RGoalArea;

	QGraphicsEllipseItem *CCircle;

	QGraphicsLineItem *LCrossHPart;
	QGraphicsLineItem *LCrossVPart;
	QGraphicsLineItem *RCrossHPart;
	QGraphicsLineItem *RCrossVPart;
	QGraphicsLineItem *CCrossHPart;

	QGraphicsEllipseItem *LTPostCircle;
	QGraphicsEllipseItem *LBPostCircle;
	QGraphicsEllipseItem *RTPostCircle;
	QGraphicsEllipseItem *RBPostCircle;

	QGraphicsLineItem *LPostVPart;
	QGraphicsLineItem *RPostVPart;

	QGraphicsItem *LTPost;
	QGraphicsItem *LBPost;
	QGraphicsItem *RTPost;
	QGraphicsItem *RBPost;

	struct {
		int totalCarpetAreaWidth;
		int totalCarpetAreaHeight;
		int totalCarpetAreaMean;
		int xCentre;
		int yCentre;
		int xLLine;
		int yTLine;
		int xRLine;
		int yBLine;
		float xLSmallArea;
		int yTSmallArea;
		int xRSmallArea;
		int yBSmallArea;
		float xLGoalArea;
		int yTGoalArea;
		int xRGoalArea;
		int yBGoalArea;
		int wQCross;
		int xLCross;
		int xRCross;
		int radPost;
		int radCenterCircle;

	} config;
	
	XMLConfig *xmlconfig;

	QGraphicsView *parent;
	QList<GraphicalRobotElement*> RobotList;
	
	void loadXMLConfig(std::string fname);
	void setSvgItems();
};

#endif // KFieldScene_H
