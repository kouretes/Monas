#include "KmeManager.h"

#include <fstream>
#include <dirent.h>
#include "core/include/Logger.hpp"
#include "core/include/Configurator.hpp"

#include "hal/robot/nao/generic_nao/kAlBroker.h"
#include "ISpecialAction.h"
#include "KmeAction.h"

using namespace std;
KSystem::Time::TimeAbsolute KmeManager::future_time = KSystem::Time::SystemTime::now();

std::vector<ISpecialAction*> KmeManager::LoadActionsKME()
{
	std::vector<ISpecialAction*> KmeActions;
	AL::ALValue actionNames, actionAngles, actionTimes;
	boost::shared_ptr<AL::ALMotionProxy> motion;

	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError, "KMEManager")<< "Error in getting motion proxy";
	}

	std::vector<motSequence> spAct;
	std::map<std::string, int> actionMap;
	ifstream fin;
	string pose_str, ending = ".kme";
	string fileName;
	DIR * t = NULL;
	motSequence tempMotSeq;
	vector<float> row;
	unsigned int pos = 0, i = 0 ;
	int k;
	string tempStr;
	t = opendir((Configurator::Instance().getDirectoryPath() + "kme").c_str());
	struct dirent * files = readdir(t);

	while(files != NULL)
	{
		fileName = files->d_name;
		k = fileName.size() > 4 ? fileName.compare( fileName.size() - 4 , 4, ending, 0 , 4) : -1;

		if( k == 0 )
		{
			spAct.push_back(tempMotSeq);
			fin.open((Configurator::Instance().getDirectoryPath() + "kme/" + fileName).c_str(), ios::in);

			if ( fin.is_open() )
			{
				while(!(fin >> pose_str).eof())
				{
					spAct[i].seqName = fileName;
					pos = pose_str.find_first_of("%");
					pose_str.erase(0, pos + 1);
					pos = pose_str.find_first_of("%");

					while( pos != string::npos)
					{
						tempStr = pose_str.substr(0, pos);
						row.push_back(atof(tempStr.c_str()));
						pose_str.erase(0, pos + 1);
						pos = pose_str.find_first_of("%", 1);
					}

					tempStr = pose_str;
					row.push_back(atof(tempStr.c_str()));
					spAct[i].seqMotion.push_back(row);
					row.clear();
				}

				fin.close();
				actionMap[spAct[i].seqName] = i;
				LogEntry(LogLevel::ExtraInfo, "KMEManager")
						<<  "Special Action Succesfully Loaded : " <<  spAct[i].seqName;
				std::vector<std::string> jointNames;
				jointNames = motion->getJointNames("Body");
				unsigned int joints = 22;
				actionNames.arraySetSize(joints);
				actionAngles.arraySetSize(joints);
				actionTimes.arraySetSize(joints);
				unsigned int poses = spAct[i].seqMotion.size();

				for (unsigned int l = 0; l < joints; l++)
				{
					actionAngles[l].arraySetSize(poses);
					actionTimes[l].arraySetSize(poses);
				}

				for (unsigned int l = 0; l < joints; l++)
				{
					float time = 0.0;
					actionNames[l] = jointNames[l];

					for (unsigned int k = 0; k < poses; k++)
					{
						actionAngles[l][k] = spAct[i].seqMotion[k][l];		//l for the joints and k for the poses actionAngles[joints][poses]
						time += spAct[i].seqMotion[k][22];
						actionTimes[l][k] = time;
					}
				}

				ISpecialAction* curAction = new KmeAction(spAct[i].seqName, actionNames, actionAngles, actionTimes);
				KmeActions.push_back(curAction);
				i++;
			}
			else
			{
				LogEntry(LogLevel::ExtraInfo, "KMEManager")
					<< "Special Action Failed to Load : " << spAct[i].seqName;
			}
		}

		files = readdir(t);
	}

	return KmeActions;
}



