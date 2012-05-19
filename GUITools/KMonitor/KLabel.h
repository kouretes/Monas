#ifndef KLABEL_H_
#define KLABEL_H_

#include <QLabel>

class KRobotMap;

class KLabel : public QLabel
{
	Q_OBJECT

public:
	KLabel(QWidget* parent = 0);
	~KLabel();

protected:
	void resizeEvent(QResizeEvent* event);

//public slots:
//signals:
private:
	KRobotMap* polarMap;

};

#endif /* KLABEL_H_ */
