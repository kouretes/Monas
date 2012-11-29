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

class LocalWorldController : public QWidget
{
    Q_OBJECT

public:
    explicit LocalWorldController(QWidget *parent = 0);
    ~LocalWorldController();

protected:
    void changeEvent(QEvent *e);
    
signals:
	void NewHostAdded(QString,QString);
	void OldHostRemoved(QString);
	void GameStateMsgUpdate(QString,QString,QString);
	void LWRHSubscriptionRequest(QString);
	void LWRHUnsubscriptionRequest(QString);
	
public slots:
	void addComboBoxItem(QString, QString);
	void removeComboBoxItem(QString);
	void setLWRHGameStateInfo(QString, QString, QString);
	void SubscriptionHandler(QString);
	void UnsubscriptionHandler(QString);
	void setKGFCGameStateInfo(GameStateMessage, QString);
	void worldInfoUpdateHandler(WorldInfo, QString);
	void sharedWorldInfoUpdateHandler(SharedWorldInfo, QString);
	void observationMessageUpdateHandler(ObservationMessage, QString);
	void localizationDataUpdateHandler(LocalizationDataForGUI, QString);
	void motionCommandUpdateHandler(MotionWalkMessage, QString);

	
private slots:
	void newTreeElementRequested(QTreeWidgetItem* item);



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
	void LWSGVTraceVisible(QString, bool);
	void LWSGVMWCmdVisible(QString, bool);
	void LWSGVTeammatesVisible(QString, bool);

protected:
	void resizeEvent(QResizeEvent* event);

private:
	QString myCurrentLWRequestedHost;
	KFieldScene* paintArea;
    HostsComboBox* availableLWHosts;
    Ui::LocalWorldController *ui;
};

#endif // LOCALWORLDCONTROLLER_H
