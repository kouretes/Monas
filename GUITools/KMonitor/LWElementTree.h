#ifndef LWELEMENTTREE_H_
#define LWELEMENTTREE_H_

#include <QTreeWidget>
#include <QString>

class LWElementTree : public QTreeWidget
{
    Q_OBJECT

public:
    LWElementTree(QTreeWidget *parent = 0);
    ~LWElementTree();

public slots:
	void LWELSubscriptionHandler(QString hostId);
	void LWELUnsubscriptionHandler(QString hostId);

signals:
	void LWRHSetRobotVisible(QString, bool);
	void LWRHSetBallVisible(QString, bool);
	void LWRHSetVisionBallVisible(QString, bool);
	void LWRHSetVisionGoalPostsVisible(QString, bool);
	void LWRHSetParticlesVisible(QString, bool);
	void LWRHSetHFOVVisible(QString, bool);
	void LWRHSetTraceVisible(QString, bool);
	void LWRHSetMWCmdVisible(QString, bool);

private slots:
	void newTreeElementRequested(QTreeWidgetItem* item);

private:
	void un_checkAllTreeElements(Qt::CheckState state);

	QString myCurrentLWRequestedHost;
    QTreeWidget* parentTreeWidget;

};

#endif /* LWELEMENTTREE_H_ */
