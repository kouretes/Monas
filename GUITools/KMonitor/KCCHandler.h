#ifndef KCCHANDLER_H_
#define KCCHANDLER_H_

#include <QObject>
#include <QLabel>
#include <QPushButton>

class KCCHandler : public QObject
{
	Q_OBJECT

public:
    KCCHandler(QLabel* label, QPushButton* button);
    ~KCCHandler();

private:
    QLabel* kcclabel;
    QPushButton* kccbutton;
};

#endif /* KCCHANDLER_H_ */
