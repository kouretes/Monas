#include "Configurator.hpp"

#include <boost/thread/locks.hpp>

using namespace std;
using namespace boost::filesystem;

void ConfiguratorClass::initConfigurator(string dirPath, string headId, string bodyId)
{
	boost::upgrade_lock<boost::shared_mutex> lock(mutexrw);
	boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
  	
  	dirPath.append("/");
	headID = headId;
	bodyID = bodyId;
	headPath = "HEAD/";
	headPath.append(headId);
	headPath.append("/");
	bodyPath = "BODY/";
	bodyPath.append(bodyId);
	bodyPath.append("/");
	directoryPath = dirPath;
	path p(dirPath);

	root.emptyNode();
	if (is_directory(p))
	{
		vector<boost::filesystem::path> v;
        copy(boost::filesystem::directory_iterator(p), directory_iterator(), back_inserter(v));
        sort(v.begin(), v.end());
		for (vector<boost::filesystem::path>::iterator iter = v.begin(); iter != v.end(); ++iter)//directory_iterator itr(p); itr != directory_iterator(); ++itr)
		{
			if (is_regular_file(*iter))
			{
				string filename;
				filename.append(iter->string());
				if(!loadAllXMLFiles(filename)){
					;
				}
			}
		}
	}
	else
		cout << "Directory path \"" << dirPath << "\" not found" << endl;
	
	computeAddler32();
}

void ConfiguratorClass::prettyPrint()
{
	root.prettyPrint("");
}

int ConfiguratorClass::numberOfNodesForKey(string key)
{
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return root.numberOfNodesForKey(key);
}

int ConfiguratorClass::numberOfChildrenForKey(string key)
{
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return root.numberOfChildrenForKey(key);
}

int ConfiguratorClass::numberOfUniqueChildrenForKey(string key)
{
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return root.numberOfUniqueChildrenForKey(key);
}

bool ConfiguratorClass::burstWrite(vector<pair<string, string> > writeData)
{	
	boost::upgrade_lock<boost::shared_mutex> lock(mutexrw);
	boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
	
	bool allOk = true;
	pair<string, string> tempPair;

	while(writeData.size() != 0)
	{
		tempPair = writeData.front();
		writeData.erase(writeData.begin());
		int returnType = root.updateValueForKey(tempPair.first, tempPair.second);
		if(returnType==-1){
			allOk=false;
		}else{
			updateXMLFilesValue(tempPair.first, tempPair.second, returnType);
		}
	}	
	computeAddler32();
	return allOk;
}

bool ConfiguratorClass::updateXMLFilesValue(string targetKey, string value, int fileType)
{
	unsigned filePos;
	queue<string> keys = ConfigNode::findAllSubstring(targetKey);
	string filenameWithoutDirPath = ConfigNode::extractNumber(keys.front(), &filePos);
	string filename = directoryPath;
	filename.append(filenameWithoutDirPath);

	if(fileType == ConfigNode::HEAD_FILE)
		filename.insert(directoryPath.size(), headPath);
	else if(fileType == ConfigNode::BODY_FILE)
		filename.insert(directoryPath.size(), bodyPath);

	keys.pop();
	filename.append(".xml");
	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();
	TiXmlNode* firstChild = doc.FirstChild();
	bool found = false;

	if(!loadOkay)
		return false;

	do
	{
		string key = keys.front();
		keys.pop();
		unsigned id, idCount = 0;
		//Find the correct sibling
		TiXmlNode *oldChild = firstChild;
		key = ConfigNode::extractNumber(key, &id);

		for(TiXmlNode* child = firstChild; child; child = child->NextSibling() )
		{
			if(child->Value() == key)
			{
				if(id == idCount)
				{
					firstChild = child;
					break;
				}

				idCount++;
			}
		}

		//No child found
		if(oldChild == firstChild)
			return false;

		//change the text
		if(keys.size() == 0)
		{
			unsigned numOfCurText = 0;

			for(TiXmlNode* child = firstChild->FirstChild(); child; child = child->NextSibling() )
			{
				if(child->Type() == TiXmlNode::TEXT)
				{
					found = true;
					child->SetValue(value);
					break;
				}
			}
		}

		//check if the last key is attribute and change it
		if(keys.size() == 1)
		{
			string lastkey = keys.front();
			unsigned pos;
			lastkey = ConfigNode::extractNumber(lastkey, &pos);

			if(lastkey[0] == ConfigNode::ATTRIBUTE_DELIMITER)
			{
				keys.pop();
				lastkey.erase(0, 1);
				TiXmlElement *elem = firstChild->ToElement();

				if(elem == NULL)
					return false;

				elem->SetAttribute(lastkey, value);
				found = true;
			}
		}

		firstChild = firstChild->FirstChild();
	}
	while(keys.size() != 0);

	if(!found)
		return false;

	doc.SaveFile(filename);
	return true;
}

string ConfiguratorClass::findValueForKey(string key)
{
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return root.findValueForKey(key);
}

string ConfiguratorClass::keyOfNodeForSubvalue(string nodeKey, string subnodeKey, string value)
{
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return root.keyOfNodeForSubvalue(nodeKey, subnodeKey, value);
}

ConfigNode* ConfiguratorClass::findNodeForKey(string key)
{
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return root.findNodeForKey(key);
}

bool ConfiguratorClass::loadAllXMLFiles(string filename)
{
	bool loadIsOk;
	
	string filenameHead = filename;
	string filenameBody = filename;
	
	filenameHead.insert(directoryPath.size(), headPath);
	filenameBody.insert(directoryPath.size(), bodyPath);
	
	loadIsOk = loadXMLFile(filename, ConfigNode::MAIN_FILE);

	//we only care about the base file
	if(loadIsOk)
	{
		loadXMLFile(filenameHead, ConfigNode::BODY_FILE);
		loadXMLFile(filenameBody, ConfigNode::HEAD_FILE);
	}

	return loadIsOk;
}

bool ConfiguratorClass::loadXMLFile(string filename, int fileType)
{	
	bool success = root.loadXMLFile(filename,fileType);
	if(success){
		allFiles.push_back(filename);
	}
	return success;
}

void ConfiguratorClass::computeAddler32(){
	adler = adler32(0, Z_NULL, 0); //Initialize checksum
	for(vector<string>::iterator iter = allFiles.begin(); iter != allFiles.end(); iter++){
		ifstream xmlFile((*iter).c_str());
		string str;
		xmlFile.seekg(0, ios::end);   
		str.reserve(xmlFile.tellg());
		xmlFile.seekg(0, ios::beg);
		str.assign((istreambuf_iterator<char>(xmlFile)), istreambuf_iterator<char>());
		adler = adler32(adler, (Bytef *)str.c_str(), str.length());
		xmlFile.close();
	}	
}

unsigned int ConfiguratorClass::getChecksum(){
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return adler;
}

string ConfiguratorClass::getHeadPath(){
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return headPath;
}

string ConfiguratorClass::getBodyPath(){
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return bodyPath;
}

string ConfiguratorClass::getHeadID(){
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return headID;
}

string ConfiguratorClass::getBodyID(){
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return bodyID;
}

string ConfiguratorClass::getDirectoryPath(){
	boost::shared_lock<boost::shared_mutex> lock(mutexrw);
	return directoryPath;
}
