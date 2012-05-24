#ifndef LMELEMENTLIST_H_
#define LMELEMENTLIST_H_

#include <QListWidget>
#include <QString>

class LMElementList : public QListWidget
{
    Q_OBJECT

public:
    LMElementList(QListWidget *parent = 0);
    ~LMElementList();

public slots:
   void LMELSubscriptionHandler(QString);
   void LMELUnsubscriptionHandler(QString);

signals:
	void LMRHSetObstaclesVisible(QString, bool);
	void LMRHSetPathVisible(QString, bool);
	void LMRHSetTargCoordVisible(QString, bool);

private slots:
	void newListElementRequested(QListWidgetItem* item);

private:
	void uncheckAllListElements();

	QString myCurrentLMRequestedHost;
    QListWidget* parentListWidget;

};

#endif /* LMELEMENTLIST_H_ */
