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
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/circular_buffer.hpp>

#include "core/elements/math/Common.hpp"

#include <algorithm>
#include <vector>

#include "messages/Debug.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/WorldInfo.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/Motion.pb.h"

#include "KFieldScene.h"

using std::vector;

class KFieldScene;

typedef struct GUIRobotPose {

	QGraphicsEllipseItem *Pose;
	QGraphicsLineItem *Direction;

} Particle;

class GraphicalRobotElement : public QObject {
	Q_OBJECT

public:
	GraphicalRobotElement(KFieldScene *parent, QString host);
	~GraphicalRobotElement();

	QString getHostId() {
		return hostId;
	}

	void setCurrentWIM(WorldInfo nwim);
	void setCurrentSWIM(SharedWorldInfo nswim);
	void setCurrentGSM(GameStateMessage gsm);
	void setCurrentOBSM(ObservationMessage obm);
	void setCurrentEKFMHypothesisM(EKFMHypothesis ekfMHyp);
    void setCurrentOdometryM(OdometryInfoMessage odometryM);
	void setCurrentMDG(MappingDataForGUI nmdg);
	void setCurrentPSODG(PSODataForGUI psodg);
	
	void setGWSRobotVisible(bool visible) {
		GWSRobotVisible = visible;
		setRobotVisible(visible);
	}
	
	bool getGWSRobotVisible() {
		return GWSRobotVisible;
	}
	
	void setLWSRobotVisible(bool visible) {
		LWSRobotVisible = visible;
		setRobotVisible(visible);
	}
	
	bool getLWSRobotVisible() {
		return LWSRobotVisible;
	}
	
	void setLWSTeammatesVisible(bool visible) {
		LWSTeammatesVisible = visible;
		setTeammatesVisible(visible);
	}
	
	bool getLWSTeammatesVisible() {
		return LWSTeammatesVisible;
	}
	
	void setLWSFormationVisible(bool visible) {
		LWSFormationVisible = visible;
		setFormationVisible(visible);
	}
	
	bool getLWSFormationVisible() {
		return LWSFormationVisible;
	}
	
	void setLWSPSOPositionsVisible(bool visible) {
		LWSPSOPositionsVisible = visible;
		setPSOPositionsVisible(visible);
	}
	
	bool getLWSPSOPositionsVisible() {
		return LWSPSOPositionsVisible;
	}
	
	void setLWSMappingVisible(bool visible) {
		LWSMappingVisible = visible;
		setMappingVisible(visible);
	}
	
	bool getLWSMappingVisible() {
		return LWSMappingVisible;
	}
	
	void setRobotVisible(bool visible);
	void setTeammateVisible(int idx, bool visible);
	void setTeammatesVisible(bool visible);
	void setFormationVisible(bool visible);
	void setPSOPositionsVisible(bool visible);
	void setMappingVisible(bool visible);
	void updateRobotRect();
	void updateTeammatesRects();

    void updatePoseUncertaintyRect();
    void updateEkfMHypothesisRects();
    void updateOdometryPolygon();

    void setVarianceVisible(bool visible);
    void setEkfMHypothesisVisible(bool visible);
    void setOdometryVisible(bool visible);
    void setRobotTraceVisible(bool visible);

	void updateFormationRects(FormationDataForGUI debugGUI);
	void updatePSOPositionsRects(PSODataForGUI debugGUI);
	void updateMappingLines(MappingDataForGUI debugGUI);
	
	void setGWSBallVisible(bool visible) {
		GWSBallVisible = visible;
		setBallVisible(visible);
	}
	
	bool getGWSBallVisible() {
		return GWSBallVisible;
	}
	
	void setGWSSharedBallVisible(bool visible) {
		GWSSharedBallVisible = visible;
		setGlobalSharedBallVisible(visible);
	}
	
	void setLWSSharedBallVisible(bool visible) {
		LWSSharedBallVisible = visible;
		setLocalSharedBallVisible(visible);
	}
	
	bool getGWSSharedBallVisible() {
		return GWSSharedBallVisible;
	}
	
	bool getLWSSharedBallVisible() {
		return LWSSharedBallVisible;
	}
	
	void setLWSBallVisible(bool visible) {
		LWSBallVisible = visible;
		setBallVisible(visible);
	}
	
	bool getLWSBallVisible() {
		return LWSBallVisible;
	}
	
	void setBallVisible(bool visible);
	void updateBallRect();
	void setGlobalSharedBallVisible(bool visible);
	void setLocalSharedBallVisible(bool visible);
	void updateLWSSharedBallRect();
	void updateGWSSharedBallRect();
	
	void setGWSUnionistLineVisible(bool visible) {
		GWSUnionistLineVisible = visible;
		setUnionistLineVisible(visible);
	}
	
	bool getGWSUnionistLineVisible() {
		return GWSUnionistLineVisible;
	}
	
	void setGWSSharedUnionistLineVisible(bool visible) {
		GWSSharedUnionistLineVisible = visible;
		setSharedUnionistLineVisible(visible);
	}
	
	bool getGWSSharedUnionistLineVisible() {
		return GWSSharedUnionistLineVisible;
	}
	
	void setLWSUnionistLineVisible(bool visible) {
		LWSUnionistLineVisible = visible;
		setUnionistLineVisible(visible);
	}
	
	bool getLWSUnionistLineVisible() {
		return LWSUnionistLineVisible;
	}
	
	void setUnionistLineVisible(bool visible);
	void updateUnionistLineRect();
	void setSharedUnionistLineVisible(bool visible);
	void updateSharedUnionistLineRect();
	
	// Vision observations
	void setLWSVisionBallVisible(bool visible) {
		LWSVisionBallVisible = visible;
		setVisionBallVisible(visible);
	}
	
	bool getLWSVisionBallVisible() {
		return LWSVisionBallVisible;
	}

    void setLWSVarianceVisible(bool visible) {
		LWSVarianceVisible = visible;
		setVarianceVisible(visible);
	}

    bool getLWSVarianceVisible(){
		return LWSVarianceVisible;
    }

    void setLWSEkfMHypothesisVisible(bool visible) {
		LWSEkfMhypothesisVisible = visible;
		setEkfMHypothesisVisible(visible);
	}

    bool getLWSEkfMHypothesisVisible(){
		return LWSEkfMhypothesisVisible;
    }

    void setLWSOdometryVisible(bool visible) {
		LWSOdometryVisible = visible;
		setOdometryVisible(visible);
	}
    
    bool getLWSOdometryVisible(){
		return LWSOdometryVisible;
    }

    void setLWSRobotTraceVisible(bool visible) {
		LWSRobotTraceVisible = visible;
		setRobotTraceVisible(visible);
	}
    
    bool getLWSRobotTraceVisible(){
		return LWSRobotTraceVisible;
    }

	void setVisionBallVisible(bool visible);
	void updateVisionBallRect(ObservationMessage obm);

	void setLWSYellowLeftPostVisible(bool visible) {
		LWSVisionYellowLeftPostVisible = visible;
		setYellowLeftPostVisible(visible);
	}
	
	bool getLWSYellowLeftPostVisible() {
		return LWSVisionYellowLeftPostVisible;
	}
	
	void setYellowLeftPostVisible(bool visible);
	void updateGoalPostsRect();

	void setLWSYellowRightPostVisible(bool visible) {
		LWSVisionYellowRightPostVisible = visible;
		setYellowRightPostVisible(visible);
	}
	
	bool getLWSYellowRightPostVisible() {
		return LWSVisionYellowRightPostVisible;
	}
	
	void setYellowRightPostVisible(bool visible);

	void setLWSYellowPostVisible(bool visible) {
		LWSVisionYellowPostVisible = visible;
		setYellowPostVisible(visible);
	}
	
	bool getLWSYellowPostVisible() {
		return LWSVisionYellowPostVisible;
	}
	
	void setYellowPostVisible(bool visible);

	// Particles of Localization
	void setLWSParticlesVisible(bool visible) {
		LWSParticlesVisible = visible;
		setParticlesVisible(visible);
	}
	
	bool getLWSParticlesVisible() {
		return LWSParticlesVisible;
	}
	
	void setParticlesVisible(bool visible);
	void updateParticlesRect(LocalizationDataForGUI debugGUI);

	// HFOV
	void setLWSHFOVVisible(bool visible) {
		LWSHFOVVisible = visible;
		setHFOVVisible(visible);
	}
	
	bool getLWSHFOVVisible() {
		return LWSHFOVVisible;
	}
	void setHFOVVisible(bool visible);
	void updateHFOVRect();


	// Motion Walk Command
	void setLWSMWCmdVisible(bool visible) {
		LWSMWCmdVisible = visible;
		setMWCmdVisible(visible);
	}
	
	bool getLWSMWCmdVisible() {
		return LWSMWCmdVisible;
	}
	
	void setMWCmdVisible(bool visible);
	void updateMWCmdRect(MotionWalkMessage wmot);

	QTimer *getGREtimer() {
		return GREtimer;
	}
	
	QTimer *getMWCmdTimer() {
		return MWCmdTimer;
	}

public slots:
	void clearVisionObservations();

private slots:
	void clearMotionWalkCommand();

private:
	void loadXMLlocalizationConfigParameters();
	void loadXMLteamConfigParameters();
	void tagVisionObservations(QGraphicsEllipseItem *post, QRectF rect, QString text);
	void tagRoles(QGraphicsEllipseItem *post, QRectF rect, const QString& text, const QColor& color);
	QPolygonF calculateArrowHeadPosition(QLineF aLine);
	void updateRobotTracePolygon();

	inline static bool comparator(const TeammatePose &robot1, const TeammatePose &robot2) {
		return (robot1.robotid() < robot2.robotid());
	}
	
	inline void sortRobotsbyId(vector<TeammatePose> &robots) {
		std::sort(robots.begin(), robots.end(), comparator);
	}
	
	boost::posix_time::ptime robotTraceTime;

	KFieldScene *parentScene;

	QString hostId;
	int teamColor;	// 1 for blue, -1 for red

	WorldInfo currentWIM;
	SharedWorldInfo currentSWIM;
    GameStateMessage currentGSM;
    GameStateMessage prevGSM;
    EKFMHypothesis currentEKFMHypothesis;
    MappingDataForGUI currentMDG;
    PSODataForGUI currentPSODG;

    OdometryInfoMessage currentOdometryM;
    bool LWSVarianceVisible;
    bool LWSOdometryVisible;
    bool LWSRobotTraceVisible;
    bool LWSEkfMhypothesisVisible;

	bool GWSRobotVisible;
	bool LWSRobotVisible, LWSTeammatesVisible;
	static const int numOfRobots = 5;

    int ekfMaxHypothesis;
	QGraphicsEllipseItem *Robot, *AngleUncertainty,*PositionUncertainty,*Teammates[numOfRobots], *GWSSharedBall, *LWSSharedBall;
	QGraphicsLineItem *RobotDirection, *TeammateDirections[numOfRobots],**PoseHypothesisDirections;
    QGraphicsEllipseItem **PoseHypothesis;
	
    QGraphicsPathItem* Odometry;
    QPainterPath* OdometryPoints;

    QGraphicsPathItem* RobotTrace;
    QPainterPath* RobotTracePoints;

    bool LWSFormationVisible, LWSPSOPositionsVisible, LWSMappingVisible;
	QList<QGraphicsEllipseItem *> PositionsList;
	QList<QGraphicsEllipseItem *> PSOPositionsList;
	QList<QGraphicsLineItem *> MappingLinesList;
	QList<QGraphicsLineItem *> PSOLinesList;
	int numOfPositions;

	bool GWSBallVisible;
	bool GWSSharedBallVisible, LWSSharedBallVisible;
	bool LWSBallVisible;
	QGraphicsEllipseItem *Ball;

    int t;
	bool GWSUnionistLineVisible;
	bool GWSSharedUnionistLineVisible;
	bool LWSUnionistLineVisible;
	QGraphicsLineItem *UnionistLine;
	QGraphicsLineItem *SharedUnionistLine;
	
	ObservationMessage currentObsm;

	bool LWSVisionBallVisible;
	QGraphicsEllipseItem *VisionBall;

	bool LWSVisionYellowLeftPostVisible;
	QGraphicsEllipseItem *LeftYellowPost;

	bool LWSVisionYellowRightPostVisible;
	QGraphicsEllipseItem *RightYellowPost;

	bool LWSVisionYellowPostVisible;
	QGraphicsEllipseItem *YellowPost;

	bool LWSParticlesVisible;
	QList<Particle *> ParticlesList;
	int partclsNum;

	bool LWSHFOVVisible;
	QGraphicsPolygonItem *HFOVLines;

	bool LWSRobotTraceTraceVisible;
	boost::circular_buffer<QGraphicsEllipseItem *> RobotPositions;
	boost::circular_buffer<QGraphicsLineItem *> UnionistLines;

	bool LWSMWCmdVisible;
	QGraphicsLineItem *GotoPositionLine;
	QGraphicsPolygonItem *GotoArrow;
	QGraphicsEllipseItem *zAxisArc;

	QTimer *GREtimer;
	QTimer *MWCmdTimer;
};

#endif // GRAPHICALROBOTELEMENT_H_
