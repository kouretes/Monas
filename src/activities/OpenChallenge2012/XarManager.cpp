#include "XarManager.h"

#include <fstream>
#include <dirent.h>
#include "tools/logger.h"
#include "hal/robot/generic_nao/kAlBroker.h"
#include "ISpecialAction.h"
#include "XarAction.h"

using namespace std;

std::vector<ISpecialAction*> XarManager::LoadActionsXAR(AL::ALPtr<AL::ALFrameManagerProxy> frame) {

	std::vector<ISpecialAction*> XarActions;
	std::vector<XarBehavior> spAct;
	std::map<std::string, int> actionMap;

	ifstream fin;
	string pose_str, ending = ".xar";
	string fileName;
	DIR * t = NULL;
	XarBehavior tempMot;

	unsigned int  i = 0;
	int k;
	string tempStr;

	t = opendir((ArchConfig::Instance().GetConfigPrefix() + "xar").c_str());
	if (t != 0) {

		struct dirent * files = readdir(t);

		while (files != NULL) {
			fileName = files->d_name;

			k = fileName.size() > 4 ? fileName.compare(fileName.size() - 4, 4, ending, 0, 4) : -1;
			if (k == 0) {
				try {
					tempMot.identifier = frame->newBehaviorFromFile((ArchConfig::Instance().GetConfigPrefix() + "xar/" + fileName), "");
					cout << "Behavior ID " << tempMot.identifier << endl;
				} catch (AL::ALError& e) {
					Logger::Instance().WriteMsg("XARManager", "Error loading file: " + fileName + e.getDescription(), Logger::FatalError);
					continue;
				}
				tempMot.name = fileName;
				tempMot.filename = (ArchConfig::Instance().GetConfigPrefix() + "xar/" + fileName);
				spAct.push_back(tempMot);
				actionMap[fileName] = i;

				ISpecialAction* curAction = new XarAction(frame,tempMot.filename,tempMot.name, tempMot.identifier);
				XarActions.push_back(curAction);

				Logger::Instance().WriteMsg("MotionController", "Special Action Successfully Loaded : " + fileName, Logger::ExtraInfo);

				i++;
			}

			files = readdir(t);
		}
	} else {
		Logger::Instance().WriteMsg("XARManager", "Error There is no such directory: " + ArchConfig::Instance().GetConfigPrefix() + "xar", Logger::FatalError);

	}
	return XarActions;

}

