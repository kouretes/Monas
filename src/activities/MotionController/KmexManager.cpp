#include "KmexManager.h"

#include <fstream>
#include <dirent.h>
#include "core/include/Logger.hpp"
#include "tools/toString.h"
#include "core/include/IActivity.hpp"

using namespace std;
using namespace KDeviceLists;

vector<KmexAction*> KmexManager::LoadActionsXML(const string& filename,
        map<string, boost::shared_ptr<ISpecialAction> > SpActions)
{
	vector<KmexAction*> xmlResults;
	std::map<std::string, int> jointIDs =  getJointIDs();

	int numOfMotions = Configurator::Instance().numberOfNodesForKey(filename + ".Motion");
	LogEntry(LogLevel::Info,"KmexManager")
		<< "Found "  << (numOfMotions) << " motion(s)";

	for (int itter=0;itter<numOfMotions;itter++)
	{
		string MotionName = Configurator::Instance().findValueForKey(filename + ".Motion~" + _toString(itter) + ".actionName");

		int numOfPoses = atoi(Configurator::Instance().findValueForKey(filename + ".Motion~" + _toString(itter) + ".$numOfPoses").c_str());
		float threshold = atof(Configurator::Instance().findValueForKey(filename + ".Motion~" + _toString(itter) + ".$threshold").c_str());
		int numOfJoints = atoi(Configurator::Instance().findValueForKey(filename + ".Motion~" + _toString(itter) + ".$numOfJoints").c_str());

		vector<string> jointsResults;

		vector<int> jointNum;
		for (int i = 0; i < numOfJoints; i++ )
		{
			jointsResults.push_back(Configurator::Instance().findValueForKey(filename + ".Motion~" + _toString(itter) + ".Joints~" + _toString(i)));
			jointNum.push_back(jointIDs[jointsResults[i]]);
		}


		vector<int> posesResults;
		for (int i = 0; i < numOfPoses; i++)
		{
			posesResults.push_back(atoi(Configurator::Instance().findValueForKey(filename + ".Motion~" + _toString(itter) + ".Poses~" + _toString(i)).c_str()));
		}

		SpAssocCont::iterator it = SpActions.find(MotionName);

		if (it == SpActions.end())
			LogEntry(LogLevel::Error,"KmexManager") <<"SpAction " << MotionName << " not found!";
		else
		{
			boost::shared_ptr<ISpecialAction> ptr = it->second;
			KmeAction* ptrdcmkme = (KmeAction*) ptr.get();
			AL::ALValue actionAngles = ptrdcmkme->ReturnALValues();
			int totalPoses = actionAngles[21].getSize() - 1;
			vector<vector <float> > anglesResults;
			vector<float> tmpAngles;

			for (int i = 0; i < numOfPoses; i++)
			{
				for (int j = 0; j < numOfJoints; j++)
				{
					tmpAngles.push_back(actionAngles[jointNum[j]][posesResults[i]]);
				}

				anglesResults.push_back(tmpAngles);
				tmpAngles.clear();
			}

			vector<vector <float> > anglesVelocity;
			vector <float> newVel;
			float temp;

			for (int i = 0; i < numOfPoses; i++)
			{
				for (int j = 0; j < numOfJoints; j++)
				{
					if(posesResults[i] == 0)
						temp = 0;
					else
						temp = (float)actionAngles[jointNum[j]][posesResults[i]] - (float)actionAngles[jointNum[j]][posesResults[i] - 1];

					newVel.push_back(temp);
				}

				anglesVelocity.push_back(newVel);
				newVel.clear();
			}

			KmexAction* curAction = new KmexAction(MotionName, numOfPoses, posesResults, totalPoses, threshold, numOfJoints, jointNum, anglesResults, anglesVelocity);
			xmlResults.push_back(curAction);
		}
	}

	return xmlResults;
}
