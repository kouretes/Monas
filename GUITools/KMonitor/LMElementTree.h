#ifndef LMELEMENTTREE_H_
#define LMELEMENTTREE_H_

#include <QTreeWidget>
#include <QString>

class LMElementTree : public QTreeWidget
{
    Q_OBJECT

public:
    LMElementTree(QTreeWidget *parent = 0);
    ~LMElementTree();

public slots:
   void LMELSubscriptionHandler(QString);
   void LMELUnsubscriptionHandler(QString);

signals:
	void LMRHSetObstaclesVisible(QString, bool);
	void LMRHSetPathVisible(QString, bool);
	void LMRHSetTargCoordVisible(QString, bool);

private slots:
	void newTreeElementRequested(QTreeWidgetItem* item);

private:
	void un_checkAllTreeElements(Qt::CheckState state);

	QString myCurrentLMRequestedHost;
    QTreeWidget* parentTreeWidget;

};

#endif /* LMELEMENTTREE_H_ */
