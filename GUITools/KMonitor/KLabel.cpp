#include "KLabel.h"
#include "KRobotMap.h"

#include <iostream>
using namespace std;

KLabel::KLabel(QWidget* parent)
	: QLabel(parent)
	, polarMap(0)
{
	//this->polarMap = new KRobotMap(this);

}

KLabel::~KLabel()
{}


void KLabel::resizeEvent(QResizeEvent* event)
{
	//std::cout << "To neo mou width() :: " << width() << std::endl;
	//std::cout << "To neo mou height() :: " << height() << std::endl;

	if (width()>height())
		polarMap->resizeRobotMap(height()-10);
	else
		polarMap->resizeRobotMap(width()-10);

}

void KLabel::removeRobotMap(QString hostId)
{
	std::cout << "KLabel::removeRobotMap" << std::endl;
	if (this->polarMap && this->polarMap->getCurrentHost() == hostId)
		delete polarMap;
}

void KLabel::LMObstaclesVisible(QString hostId, bool visible)
{
	KRobotMap* robotMap;

	if (!polarMap || polarMap->getCurrentHost().isEmpty())
	{
		robotMap = new KRobotMap(this, hostId);
		polarMap = robotMap;

	}else if (polarMap->getCurrentHost() != hostId)
	{
		removeRobotMap(polarMap->getCurrentHost());
		robotMap = new KRobotMap(this, hostId);
		polarMap = robotMap;
	}

	if (polarMap != NULL)
		polarMap->setLMObstaclesVisible(visible);
	else
		std::cout << "[51] KLabel::LMObstaclesVisible : Fatal !" << std::endl;

}

void KLabel::gridInfoUpdateHandler(GridInfo gim, QString hostId)
{
	if(polarMap && polarMap->getCurrentHost() == hostId)
	{
		for (int ring=0; ring < TotalRings; ring++)
			for (int sector=0; sector < N; sector++){
				polarMap->PolarGrid[0][ring][sector] = gim.gridcells(ring*N+sector);
			}
		polarMap->targetX = gim.targetcoordinates(0);
		polarMap->targetY = gim.targetcoordinates(1);
		polarMap->targetA = gim.targetcoordinates(2);

		for (int step = 0; step < PathLength; step++){
			polarMap->pathR[step] = gim.pathstepsring(step);
			polarMap->pathS[step] = gim.pathstepssector(step);
			polarMap->pathO[step] = gim.pathstepsorientation(step);
		}

		polarMap->updateRobotMap();

		//std::cout << "DFG " << std::endl;

	}else
	{
		std::cout << "[67]Host doesn't exist !" << hostId.toStdString() <<std::endl;
	}


}
