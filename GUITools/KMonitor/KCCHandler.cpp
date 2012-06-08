#include "KCCHandler.h"

KCCHandler::KCCHandler(QLabel*label, QPushButton* button)
{
	kcclabel = label;
	kccbutton = button;

	kccbutton->setText(QString("Show MERCYYYYYYYYYY !!!!!!"));
}

KCCHandler::~KCCHandler()
{

}
