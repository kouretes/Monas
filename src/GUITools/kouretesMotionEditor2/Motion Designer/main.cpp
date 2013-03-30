#include <QtGui/QApplication>
#include <QSplashScreen>
#include <QTimer>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;


    //QSplashScreen *splash = new QSplashScreen;
    //splash->setPixmap(QPixmap(":/images/splash.png"));
    //splash->show();

    //QTimer::singleShot(1000, splash, SLOT(close()));
    QTimer::singleShot(1000, &w, SLOT(myShow()));

    return a.exec();
}
