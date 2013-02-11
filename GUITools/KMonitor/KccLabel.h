#ifndef KCCLABEL_H
#define KCCLABEL_H
#include <QLabel>
#include <QMouseEvent>
class KccLabel : public QLabel {
	Q_OBJECT
public:
	KccLabel (QWidget *parent);
	~KccLabel() {}

signals:
	void clicked (QMouseEvent *event);

protected:
	void mousePressEvent ( QMouseEvent *event ) ;

};
#endif
