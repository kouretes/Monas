#include "KMonitor.h"

#include <QtGui>
#include <QApplication>
#include "core/architecture/configurator/Configurator.hpp"

int main (int argc, char *argv[]) {
  	Configurator::Instance().initConfigurator("../../../../config", "", "");
	QApplication a(argc, argv);
	KMonitor w;
	w.show();
	return a.exec();
}
