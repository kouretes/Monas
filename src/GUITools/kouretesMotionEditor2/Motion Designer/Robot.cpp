
#include<iostream>

#include "Robot.h"

bool Robot::loadXml(QString pFilename)
{

    QFile file(pFilename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        std::cerr << "Error: Cannot read file " << qPrintable(pFilename)
                  << ": " << qPrintable(file.errorString())
                  << std::endl;
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine,
                        &errorColumn)) {
        std::cerr << "Error: Parse error at line " << errorLine << ", "
                  << "column " << errorColumn << ": "
                  << qPrintable(errorStr) << std::endl;
        return false;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "Robot") {
        std::cerr << "Error: Not a Robot file" << std::endl;
        return false;
    }

    int jointNum=0;

    QDomNode child = root.firstChild();
    while (!child.isNull()) {

        if (child.toElement().tagName() == "manufacturer"){
            manufacturer = child.firstChild().toText().nodeValue().toStdString();
        }
        else if(child.toElement().tagName() == "type"){
            type = child.firstChild().toText().nodeValue().toStdString();

        }
        else if(child.toElement().tagName() == "JointNumber"){
            numOfJoints = child.firstChild().toText().nodeValue().toInt();

            orderedJointNames.resize(numOfJoints,"blank");
        }
        else if(child.toElement().tagName() == "Joint"){
            curJoint = new Joint();
            QDomNode curChild = child.firstChild();

            for(int i =0 ;i<8;i++){

                if(curChild.toElement().tagName() == "name"){

                    curJoint->jointName = curChild.firstChild().toText().nodeValue().toStdString();
                }
                else if(curChild.toElement().tagName() == "minBound" || curChild.toElement().tagName() == "maxBound"){


                    curJoint->bounds.push_back(curChild.firstChild().toText().nodeValue().toFloat());
                }
                else if(curChild.toElement().tagName() == "step"){
                    curJoint->step = curChild.firstChild().toText().nodeValue().toFloat();
                }
                else if(curChild.toElement().tagName() == "color"){
                    curJoint->jointColor = curChild.firstChild().toText().nodeValue().toStdString();
                }
                else if(curChild.toElement().tagName() == "coupledWith"){
                    curJoint->coupledWith = curChild.firstChild().toText().nodeValue().toStdString();
                }
                else if(curChild.toElement().tagName() == "couplingType"){
                    curJoint->couplingType = curChild.firstChild().toText().nodeValue().toStdString();
                }
                else if(curChild.toElement().tagName() == "indexOrder"){
                    orderedJointNames[ curChild.firstChild().toText().nodeValue().toInt() ] = curJoint->jointName;
                    indexOrder[curJoint->jointName] = curChild.firstChild().toText().nodeValue().toInt();
                    //std::cout<<orderedJointNames[ curChild.firstChild().toText().nodeValue().toInt() ]<<"      "<<curChild.firstChild().toText().nodeValue().toInt()<<std::endl;
                }
                curChild = curChild.nextSibling();
            }

                if(curJoint->coupledWith != "None"){
                        coupledJointsMap[jointNum].push_back(curJoint->coupledWith);
                        coupledJointsMap[jointNum].push_back(curJoint->couplingType);
                }


                joint.push_back(curJoint);

                jointNum++;

        }

        child = child.nextSibling();
    }
    return true;
}
