#ifndef LWELEMENTLIST_H_
#define LWELEMENTLIST_H_

#include <QListWidget>
#include <QString>

class LWElementList : public QListWidget
{
    Q_OBJECT

public:
    LWElementList(QListWidget *parent = 0);
    ~LWElementList();

public slots:
   void LWELSubscriptionHandler(QString hostId);
   void LWELUnsubscriptionHandler(QString hostId);

signals:
	void LWRHSetRobotVisible(QString, bool);
	void LWRHSetBallVisible(QString, bool);
	void LWRHSetVisionBallVisible(QString, bool);

private slots:
	void newListElementRequested(QListWidgetItem* item);

private:
	void uncheckAllListElements();

	QString myCurrentLWRequestedHost;
    QListWidget* parentListWidget;

};
#endif /* LWELEMENTLIST_H_ */
