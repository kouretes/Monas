#include "KLabel.h"
#include "KRobotMap.h"

#include <iostream>
using namespace std;

KLabel::KLabel(QWidget* parent)
	: QLabel(parent)
	, polarMap(0)
{
	this->polarMap = new KRobotMap(this);

}

KLabel::~KLabel()
{}


void KLabel::resizeEvent(QResizeEvent* event)
{
	//paintArea->resizeFieldScene(width()-20,height()-40);
	//paintArea->resizeFieldScene(width()-20, (width()-20)/1.37037) ;
	//paintArea->resizeFieldScene((height()-40)*1.37, height()-40);
	std::cout << "To neo mou width() :: " << width() << std::endl;
	std::cout << "To neo mou height() :: " << height() << std::endl;

	if (width()>height())
		polarMap->resizeRobotMap(height()-10);
	else
		polarMap->resizeRobotMap(width()-10);

}
