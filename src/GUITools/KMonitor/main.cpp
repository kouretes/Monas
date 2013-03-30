#include "KMonitor.h"

#include <QtGui>
#include <QApplication>

int main (int argc, char *argv[]) {
	QApplication a (argc, argv);
	KMonitor w;
	w.show();
	return a.exec();
}
