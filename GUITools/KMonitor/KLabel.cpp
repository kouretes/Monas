#include "KLabel.h"
#include "KRobotView.h"

#include <iostream>
using namespace std;

KLabel::KLabel(QWidget* parent)
	: QLabel(parent)
	, robotView(0)
{
	robotView = new KRobotView(this, "Anybody");
}

KLabel::~KLabel()
{}


void KLabel::resizeEvent(QResizeEvent* event)
{
	if (width()>height())
		robotView->resizeRobotView(height()-10);
	else
		robotView->resizeRobotView(width()-10);

}

void KLabel::resetRobotView(QString hostId)
{
	if (robotView->getCurrentHost() == hostId)
		robotView->resetKRobotView("");
}

void KLabel::LVRawImageVisible(QString hostId, bool visible)
{
	if (robotView->getCurrentHost() != hostId)
		robotView->resetKRobotView(hostId);

	if (robotView != NULL)
		robotView->setLRVRawImageVisible(visible);
	else
		std::cout << "[41] KLabel::LVRawImageVisible : Fatal !" << std::endl;


}

void KLabel::kRawImageUpdateHandler(KRawImage rawImage, QString hostId)
{
	if( robotView && robotView->getCurrentHost() == hostId)
	{

		if (robotView->getLRVRawImageVisible())
		{
			robotView->setRVRawImageVisible(false);
			robotView->updateRawRobotView(rawImage);
			robotView->setRVRawImageVisible(true);
		}
		else if(robotView->getLRVSegImageVisible())
		{
			robotView->setRVSegImageVisible(false);
			robotView->updateSegRobotView(rawImage);
			robotView->setRVSegImageVisible(true);
		}

	}else
	{
		std::cout << "[47]Host doesn't exist !" << hostId.toStdString() <<std::endl;
	}
}

void KLabel::LVSegImageVisible(QString hostId, bool visible)
{
	if (robotView->getCurrentHost() != hostId)
		robotView->resetKRobotView(hostId);

	if (robotView != NULL)
		robotView->setLRVSegImageVisible(visible);
	else
		std::cout << "[72] KLabel::LVSegImageVisible : Fatal !" << std::endl;


}

// todo delete this better :p
void KLabel::KCCRawImageUpdateHandler(KRawImage rawImage, QString hostId)
{
	LVRawImageVisible(hostId, true);
	kRawImageUpdateHandler(rawImage, hostId);
}
