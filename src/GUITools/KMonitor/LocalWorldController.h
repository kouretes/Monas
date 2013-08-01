#ifndef LOCALWORLDCONTROLLER_H
#define LOCALWORLDCONTROLLER_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QCheckBox>
#include <QString>
#include <QList>
#include <map>
#include "tools/toString.h"
#include "KFieldScene.h"
#include "HostsComboBox.h"

namespace Ui {
	class LocalWorldController;
}

class LocalWorldController : public QWidget {
	Q_OBJECT

public:
	explicit LocalWorldController(QWidget *parent = 0);
	~LocalWorldController();

protected:
	void changeEvent(QEvent *e);

signals:
	void SubscriptionRequest(QString);

public slots:
	void addComboBoxItem(QString, QString);
	void removeComboBoxItem(QString);
	void setGameStateInfo(QString, QString, QString);
	void changeToHost(QString);
	void SubscriptionHandler(QString);

	void setKGFCGameStateInfo(GameStateMessage, QString);
	void worldInfoUpdateHandler(WorldInfo, QString);
	void sharedWorldInfoUpdateHandler(SharedWorldInfo, QString);
	void observationMessageUpdateHandler(ObservationMessage, QString);
	void localizationDataUpdateHandler(LocalizationDataForGUI, QString);
	void motionCommandUpdateHandler(MotionWalkMessage, QString);
	void formationDataUpdateHandler(FormationDataForGUI, QString);
	void PSODataUpdateHandler(PSODataForGUI, QString);
	void MappingDataUpdateHandler(MappingDataForGUI, QString);
	
    void EKFMHypothesisUpdateHandler(EKFMHypothesis, QString);
    void OdometryUpdateHandler(OdometryInfoMessage OdometryM, QString host);

private slots:
	void newTreeElementRequested(QTreeWidgetItem *item);

private:
	void LWSGVUnionistLineVisible(GraphicalRobotElement *robotElement);
	void un_checkAllTreeElements(Qt::CheckState state);


	void removeGraphicalElement(QString);

	void LWSGVRobotVisible(QString, bool);
	void LWSGVBallVisible(QString, bool);
	void LWSGVVisionBallVisible(QString, bool);
	void LWSGVVisionGoalPostsVisible(QString, bool);
	void LWSGVParticlesVisible(QString, bool);
	void LWSGVHFOVVisible(QString, bool);
	void LWSGVRobotTraceVisible(QString, bool);
	void LWSGVMWCmdVisible(QString, bool);
	void LWSGVTeammatesVisible(QString, bool);
    void LWSGVVarianceVisible(QString, bool);
    void LWSGVEkfMHypothesisVisible(QString, bool);
    void LWSGVOdometryVisible(QString, bool);
	void LWSGVFormationVisible(QString, bool);
	void LWSGVSharedBallVisible(QString, bool);
	void LWSGVPSOPositionsVisible(QString, bool);
	void LWSGVMappingBFVisible(QString, bool);
	
protected:
	void resizeEvent(QResizeEvent *event);

private:
	QString myCurrentLWRequestedHost;
	KFieldScene *paintArea;
	HostsComboBox *availableLWHosts;
	Ui::LocalWorldController *ui;
};

#endif // LOCALWORLDCONTROLLER_H
