#ifndef GLOBALWORLDCONTROLLER_H
#define GLOBALWORLDCONTROLLER_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QCheckBox>
#include <QString>
#include <QList>
#include "tools/toString.h"
#include "KFieldScene.h"

namespace Ui {
    class GlobalWorldController;
}

class GlobalWorldController : public QWidget
{
    Q_OBJECT

public:
    explicit GlobalWorldController(QWidget *parent = 0);
    ~GlobalWorldController();

public slots:
	void addNewItem(QString hostId, QString hostName);
	void removeOldItem(QString hostId);
	void updateGameState(QString iconPath, QString stateMSG, QString hostId);
	void setKGFCGameStateInfo(GameStateMessage, QString);
	void worldInfoUpdateHandler(WorldInfo, QString);

signals:
	void GWRHSubscriptionRequest(QString);
	void GWRHUnsubscriptionRequest(QString);

private slots:
	void mainCheckBoxHandler(int state);
	void subCheckBox1Handler(int state);
	void subCheckBox2Handler(int state);

private:
	QString GWhostNameFinder(QString hostId);
	QTreeWidgetItem* GWhostFinder(QString hostId);
	void printGWRequests();

private:
	void GWSGVUnionistLineVisible(GraphicalRobotElement *robotElement);
	void GWSGVRobotVisible(QString, bool);
	void GWSGVBallVisible(QString, bool);
	void removeGraphicalElement(QString);
protected:
	void resizeEvent(QResizeEvent* event);
private:
    Ui::GlobalWorldController *ui;

	KFieldScene* paintArea;

	typedef struct Elements{

		QString hostId;
		QString hostName;
		bool requestedPosition;
		bool requestedBall;

		Elements()
		{
				hostId = QString("");
				hostName = QString("");
				requestedPosition = false;
				requestedBall = false;
		}

		Elements(QString host,QString name, bool position, bool ball)
		{
			hostId = host;
			hostName = name;
			requestedPosition = position;
			requestedBall = ball;
		}

		void setPositionRequest(bool request)
		{
			requestedPosition = request;
		}

		void setBallRequest(bool request)
		{
			requestedBall = request;
		}

	}requestedElements;
	QList<requestedElements*> GWRequests;
};

#endif // GLOBALWORLDCONTROLLER_H
