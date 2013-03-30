#ifndef JOINTSLIDERS_H
#define JOINTSLIDERS_H

#include <QWidget>

#include "Robot.h"
#include "sliderinput.h"
#include <iostream>

class JointSliders: public QWidget
{

		Q_OBJECT
	public:
		JointSliders(Robot *, QWidget *parent = 0);
		void setUpWidget(Robot *);

		std::vector<sliderInput *> sliders;
		void connect_sliders();

		signals:
		void poseChanged();
};

#endif // JOINTSLIDERS_H
