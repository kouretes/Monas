#ifndef LVELEMENTLIST_H_
#define LVELEMENTLIST_H_

#include <QListWidget>
#include <QString>

class LVElementList : public QListWidget
{
    Q_OBJECT

public:
    LVElementList(QListWidget *parent = 0);
    ~LVElementList();

public slots:
   void LVELSubscriptionHandler(QString);
   void LVELUnsubscriptionHandler(QString);

signals:
	void LVRHSetRawImageVisible(QString, bool);

private slots:
	void newListElementRequested(QListWidgetItem* item);

private:
	void uncheckAllListElements();

	QString myCurrentLVRequestedHost;
    QListWidget* parentListWidget;

};

#endif /* LVElementList_H_ */
