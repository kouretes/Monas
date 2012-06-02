#ifndef ROBOT_H
#define ROBOT_H


#include "Joint.h"
#include <QtGui>
#include <qdom.h>
#include <map>


class Robot{

    public:

        int numOfJoints;
        std::string manufacturer;
        std::string type;
        std::vector< Joint * > joint;
        Joint *curJoint;
        std::map< std::string,int > indexOrder;	// we map the names of the Joints with the order in the xml file
        std::vector<std::string> orderedJointNames;
        std::map< int, std::vector< std::string > > coupledJointsMap;

        bool loadXml(QString);
		
};

#endif // ROBOT_H

