#include "jointsliders.h"

JointSliders::JointSliders(Robot *robot, QWidget* parent) :
	QWidget(parent)
{
	setUpWidget(robot);
}

void JointSliders::setUpWidget(Robot * robot)
{

	QGridLayout *glayout = new QGridLayout;
	int curJoint;
	sliders.resize(robot->numOfJoints);
	int row = 0;
	int col = 0;

	for (int i = 0; i < robot->numOfJoints; i++)
	{

		if (row >= robot->numOfJoints / 2)
		{
			row = 0;
			col++;
		}

		curJoint = robot->indexOrder[robot->joint[i]->jointName];

		sliders[curJoint] = new sliderInput;

		glayout->addWidget(sliders[curJoint], row++, col);
		sliders[curJoint]->setTitle(robot->joint[i]->jointName.c_str());
		sliders[curJoint]->setStyleSheet("QGroupBox {"
			"background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
			"stop: 0 #E0E0E0, stop: 1 " + QString(robot->joint[i]->jointColor.c_str()) + ");"
			"border: 2px solid gray;"
			"border-radius: 2px;"
			"margin-top: 1ex; }");

		sliders[curJoint]->hSlider->setRange((int) (robot->joint[i]->bounds[0] * 10000), (int) (robot->joint[i]->bounds[1] * 10000));
		sliders[curJoint]->hSlider->setValue((int) (0.5 * (robot->joint[i]->bounds[0] * 10000 + robot->joint[i]->bounds[1] * 10000)));
		sliders[curJoint]->hSlider->setSingleStep(1);

		sliders[curJoint]->dblSpinBox->setRange((double) robot->joint[i]->bounds[0], (double) robot->joint[i]->bounds[1]);
		sliders[curJoint]->dblSpinBox->setValue((double) (0.5 * (robot->joint[i]->bounds[0] + robot->joint[i]->bounds[1])));
		sliders[curJoint]->dblSpinBox->setSingleStep(0.01);

		
		std::cout << "new joint " << robot->joint[i]->jointName << std::endl;

}
setLayout(glayout);
}


void JointSliders::connect_sliders(){
	
	
	for (int curJoint = 0; curJoint < sliders.size(); curJoint++)
	{
	connect	(sliders[curJoint]->hSlider,SIGNAL(valueChanged(int)),this, SIGNAL(poseChanged()));	
	}
}