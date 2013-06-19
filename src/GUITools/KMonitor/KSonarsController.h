#ifndef KSONARSCONTROLLER_H
#define KSONARSCONTROLLER_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "HostsComboBox.h"
#include "messages/PathPlanningMessages.pb.h"

class KMapScene;

namespace Ui {
	class KSonarsController;
}

class KSonarsController : public QWidget {
	Q_OBJECT

public:
	explicit KSonarsController(QWidget *parent = 0);
	~KSonarsController();

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
	void gridInfoUpdateHandler(GridInfo, QString);

private slots:
	void newTreeElementRequested(QTreeWidgetItem *item);
	
private:
	void resizeEvent(QResizeEvent *event);
	void un_checkAllTreeElements(Qt::CheckState state);
	Ui::KSonarsController *ui;

	KMapScene *mapArea;
	HostsComboBox *availableSonarsHosts;
};

#endif // KSENSORSCONTROLLER_H
