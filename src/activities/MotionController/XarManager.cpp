#include "XarManager.h"

#include <fstream>
#include <dirent.h>
#include "core/include/Logger.hpp"
#include "core/include/Configurator.hpp"
#include "hal/robot/nao/generic_nao/kAlBroker.h"
#include "ISpecialAction.h"
#include "XarAction.h"

using namespace std;

std::vector<ISpecialAction*> XarManager::LoadActionsXAR(AL::ALFrameManagerProxy *frame)
{
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
	t = opendir((Configurator::Instance().getDirectoryPath() + "xar").c_str());

	if (t != 0)
	{
		struct dirent * files = readdir(t);

		while (files != NULL)
		{
			fileName = files->d_name;
			k = fileName.size() > 4 ? fileName.compare(fileName.size() - 4, 4, ending, 0, 4) : -1;

			if (k == 0)
			{
				try
				{
					tempMot.identifier = frame->newBehaviorFromFile((Configurator::Instance().getDirectoryPath() + "xar/" + fileName), "");
					cout << "Behavior ID " << tempMot.identifier << endl;
				}
				catch (AL::ALError& e)
				{
					LogEntry(LogLevel::FatalError,"XARManager")
						<< "Error loading file: " << fileName << e.getDescription();
					continue;
				}

				tempMot.name = fileName;
				tempMot.filename = (Configurator::Instance().getDirectoryPath() + "xar/" + fileName);
				spAct.push_back(tempMot);
				actionMap[fileName] = i;
				ISpecialAction* curAction = new XarAction(frame, tempMot.filename, tempMot.name, tempMot.identifier);
				XarActions.push_back(curAction);
				LogEntry(LogLevel::ExtraInfo,"XARManager") << "Special Action Successfully Loaded : " << fileName;
				i++;
			}

			files = readdir(t);
		}
	}
	else
	{
		LogEntry(LogLevel::FatalError,"XARManager") << "Error There is no such directory: " << Configurator::Instance().getDirectoryPath() << "xar";
	}

	return XarActions;
}

