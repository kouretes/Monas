#include "XmlManager.h"

using namespace std;
using namespace boost::filesystem;

string XmlManager::convertInt(int number)
{
	stringstream ss;	//create a stringstream
	ss << number;		//add number to the stream
	return ss.str();	//return a string with the contents of the stream
}

XmlManager::XmlManager(string dirPath, string headId, string bodyId, bool administrator)
{
	root = administrator;
	headID = headId;
	bodyID = bodyId;
	fileType = 0;
	headPath = "HEAD/";
	headPath.append(headId);
	headPath.append("/");
	bodyPath = "BODY/";
	bodyPath.append(bodyId);
	bodyPath.append("/");
	directoryPath = dirPath;
	path p(dirPath);

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
				if(!loadAllFiles(filename)){
					;
				}
			}
		}
	}
	else
		cout << "Directory path \"" << dirPath << "\" not found" << endl;
	
	computeAddler32();
}

void XmlManager::prettyPrint(string pref)
{
	cout << pref << endl;

	cout << pref << "/t/" << text << endl;


	for(map<string, string >::iterator ait = attributes.begin(); ait != attributes.end(); ++ait)
	{
		cout << pref << "Attribute:" << (*ait).first << "=" << (*ait).second << endl;
	}


	for(map<string, vector<XmlManager> >::iterator kit = kids.begin(); kit != kids.end(); ++kit)
	{
		for(vector<XmlManager>::iterator tit = (*kit).second.begin(); tit != (*kit).second.end(); ++tit)
		{
			cout << pref + "+" << "XmlManager:" << (*kit).first << endl;
			(*tit).prettyPrint(pref + "+");
		}
	}
}


void XmlManager::deleteNodesForKey(string key, int fileType)
{
	queue<string> keys = findAllSubstring(key);
	XmlManager * secondtolast = findSecondToLastNodeForKey(keys);
	string lastkey = keys.front();
	unsigned pos;
	lastkey = extractNumber(lastkey, &pos);

	if(secondtolast != NULL && secondtolast->kids.find(lastkey) != secondtolast->kids.end() && secondtolast->kids[lastkey].front().fileType != fileType)
	{
		secondtolast->kids.erase(lastkey);
	}
}

int XmlManager::numberOfNodesForKey(string key)
{
	queue<string> keys = findAllSubstring(key);
	//return findNodeForKey(keys);
	XmlManager * secondtolast = findSecondToLastNodeForKey(keys);
	//Last processing, it must be a valid XmlManager, so repeat check one more time

	if(secondtolast == NULL)
		return 0;

	string lastkey = keys.front();

	if(secondtolast->kids.find(lastkey) == secondtolast->kids.end())
		return 0;
	else
	{
		return (*secondtolast->kids.find(lastkey)).second.size();
	}
}

int XmlManager::numberOfChildrenForKey(string key)
{
	//return findNodeForKey(keys);
	XmlManager * darthVader = findNodeForKey(key); //darthVader = father

	if(darthVader == NULL)
		return 0;

	int Children = 0;
	map<string,vector<XmlManager> >::iterator it;
	for(it = darthVader->kids.begin(); it != darthVader->kids.end(); it++){
		Children += (*it).second.size();
	}
	return Children;
}

int XmlManager::numberOfUniqueChildrenForKey(string key){
	XmlManager * darthVader = findNodeForKey(key); //darthVader = father

	if(darthVader == NULL)
		return 0;

	return darthVader->getChildrenCount();
}

bool XmlManager::updateFilesValue(string targetKey, string value, int fileType)
{
	unsigned filePos;
	queue<string> keys = findAllSubstring(targetKey);
	string filenameWithoutDirPath = extractNumber(keys.front(), &filePos);
	string filename = directoryPath;
	filename.append(filenameWithoutDirPath);

	if(fileType == HEAD_FILE)
		filename.insert(directoryPath.size(), headPath);
	else if(fileType == BODY_FILE)
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
		key = extractNumber(key, &id);

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
			lastkey = extractNumber(lastkey, &pos);

			if(lastkey[0] == ATTRIBUTE_DELIMITER)
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

unsigned XmlManager::getChildrenCount() const
{
	return kids.size();
}

bool XmlManager::updateValueForKey(string key, string value)
{
	queue<string> keys = findAllSubstring(key);
	XmlManager * secondtolast = findSecondToLastNodeForKey(keys);

	if(secondtolast == NULL)
		return false;

	string lastkey = keys.front();
	unsigned pos;

	if(lastkey[0] == ATTRIBUTE_DELIMITER)
	{
		lastkey = extractNumber(lastkey, &pos);
		lastkey.erase(0, 1);
		map<string, string >::iterator it = secondtolast->attributes.find(lastkey);

		if(it != attributes.end())
			(*it).second = value;
		else
			return false;
		if(root)
			updateFilesValue(key, value, secondtolast->fileType);
	}
	else
	{
		XmlManager * lastNode = secondtolast->findNodeForKey(lastkey);

		if(lastNode == NULL)
			return false;

		lastNode->text = value;
		if(root)
			updateFilesValue(key, value, lastNode->fileType);
	}

	return true;
}

bool XmlManager::burstWrite(vector<pair<string, string> > writeData)
{
	bool allOk = true;
	pair<string, string> tempPair;

	while(writeData.size() != 0)
	{
		tempPair = writeData.front();
		writeData.erase(writeData.begin());
		allOk &= updateValueForKey(tempPair.first, tempPair.second);
	}	
	computeAddler32();
	return allOk;
}

string XmlManager::findValueForKey(string key)
{
	queue<string> keys = findAllSubstring(key);
	XmlManager * secondtolast = findSecondToLastNodeForKey(keys);

	if(secondtolast == NULL)
		return "";

	//Last processing, is it an attribute?
	string lastkey = keys.front();

	if(lastkey[0] == ATTRIBUTE_DELIMITER) //Its an attribute
	{
		lastkey.erase(0, 1);
		return secondtolast->getAttribute(lastkey);
	}

	//No
	unsigned pos;
	lastkey = extractNumber(lastkey, &pos);

	if(secondtolast->kids.find(lastkey) == secondtolast->kids.end() ||
	        (*secondtolast->kids.find(lastkey)).second.size() <= pos)
		return "";
	else
		return (*secondtolast->kids.find(lastkey)).second[pos].text;
}

string XmlManager::getText()
{
	return text;
}

string XmlManager::keyOfNodeForSubvalue(string nodeKey, string subnodeKey, string value){
	int itter = 0; 
	string key = "";
	for(int i = 0; i < numberOfNodesForKey(nodeKey); i++){
	    key = nodeKey + "~" + convertInt(itter);
	    if(value.compare(findValueForKey(key + subnodeKey)) == 0)
	    {
	        break;
	    }
	    else
	    {
	        key="";
	    } 
		itter++;
	}
	return key;
}

string XmlManager::getAttribute(string & key)
{
	unsigned pos;
	string akey = extractNumber(key, &pos);
	map<string, string>::iterator it = attributes.find(akey);

	if(it != attributes.end())
	{
		return (*it).second;
	}
	else
		return "";
}

std::queue<std::string> XmlManager::findAllSubstring(string key)
{
	queue<string> allKeys;
	size_t pos = key.find_first_of(DELIMITER, 0);

	while (pos != string::npos)
	{
		allKeys.push(key.substr(0, pos));
		key.erase(0, pos + 1);
		pos = key.find_first_of(DELIMITER, 0);
	}

	allKeys.push(key);
	return allKeys;
}

XmlManager* XmlManager::findNodeForKey(string key)
{
	queue<string> keys = findAllSubstring(key);

	XmlManager * secondtolast = findSecondToLastNodeForKey(keys);
	//Last processing, it must be a valid XmlManager, so repeat check one more time

	if(secondtolast == NULL)
		return NULL;

	string lastkey = keys.front();
	unsigned pos;
	lastkey = extractNumber(lastkey, &pos);

	if(secondtolast->kids.find(lastkey) == secondtolast->kids.end() ||
	        (*secondtolast->kids.find(lastkey)).second.size() <= pos)
		return NULL;
	else
	{
		return &(*secondtolast->kids.find(lastkey)).second[pos];
	}
}

XmlManager* XmlManager::findSecondToLastNodeForKey(queue<string> & keys)
{
	if(keys.size() > 1)
	{
		string key = keys.front();
		keys.pop();
		unsigned pos;
		key = extractNumber(key, &pos);

		if(kids.find(key) == kids.end() || (*kids.find(key)).second.size() < pos)
			return NULL;
		else
			return (*kids.find(key)).second[pos].findSecondToLastNodeForKey(keys);
	}

	return this;
}

string  XmlManager::extractNumber(string & str, unsigned * num)
{
	size_t pos = str.find_first_of(NUMBER_DELIMITER, 0);

	if(pos == string::npos)
	{
		if(num != NULL)
			*num = 0;

		return str;
	}
	else
	{
		string strnum = str.substr(pos + 1);
		string key = str.substr(0, pos);
		istringstream convert(strnum);

		if ( !(convert >> (*num)) ) //give the value to 'Result' using the characters in the stram
			*num = 0;

		return key;
	}
}


bool XmlManager::loadAllFiles(string filename)
{
	bool loadIsOk;

	if(filename.substr(filename.find_last_of(".") + 1, filename.size() - 1) != "xml")
		return false;

	loadIsOk = loadFile(filename, MAIN_FILE);

	//we only care about the base file
	if(loadIsOk)
	{
		loadFile(filename, BODY_FILE);
		loadFile(filename, HEAD_FILE);
	}

	return loadIsOk;
}

bool XmlManager::loadFile(string filename, int fileType)
{
	string key = filename.substr(0, filename.find_last_of("."));
	key = key.substr(key.find_last_of("/") + 1, key.size() - 1);
	if(fileType == HEAD_FILE)
		filename.insert(directoryPath.size(), headPath);
	else if(fileType == BODY_FILE)
		filename.insert(directoryPath.size(), bodyPath);

	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();

	if(!loadOkay)
		return false;	
	
	allFiles.push_back(filename);
	if(kids.find(key) != kids.end())
	{
		for(TiXmlNode* child = doc.FirstChild(); child; child = child->NextSibling())
		{
			string tempKey = child->Value();
			string tKey = key;
			tKey.append(".");
			tKey.append(tempKey);
			deleteNodesForKey(tKey, fileType);
			//ONLY ONE FILE FOR GIVEN FILENAME
			kids[key].front().insertRecursivePolicyAppend(child, fileType);
		}
	}
	else
	{
		XmlManager baby ;

		for(TiXmlNode* child = doc.FirstChild(); child; child = child->NextSibling())
		{
			baby.insertRecursivePolicyAppend(child, fileType);
		}

		kids[key].push_back(baby);
	}
		
	return true;
}

void XmlManager::insertRecursivePolicyAppend(TiXmlNode* xmlNode, int fileType)
{
	//TODO what file type for this text subNode?
	//Append text elements
	if(xmlNode->Type() == TiXmlNode::TEXT)
	{
		text = xmlNode->Value();
	}

	//If its an element, create a new child (policy append) and merge the element to it
	TiXmlElement *element = xmlNode->ToElement();

	if(element == NULL)
		return;

	XmlManager baby;
	baby.fileType = fileType; //Set fileType
	TiXmlAttribute *attr = element->FirstAttribute();

	while(attr != NULL)
	{
		string key = attr->Name();
		baby.attributes[key] = attr->Value();
		//cout << "For key = " << key << " Attr = " << attr->Value() << endl;
		attr = attr->Next();
	}

	for( TiXmlNode * child = xmlNode->FirstChild(); child; child = child->NextSibling() )
	{
		baby.insertRecursivePolicyAppend(child, fileType);
	}

	kids[element->Value()].push_back(baby);
}

void XmlManager::computeAddler32(){
	adler = adler32(0, Z_NULL, 0); //Initialize checksum
	if(root){
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
}

unsigned int XmlManager::getChecksum(){
	return adler;
}

string XmlManager::getHeadPath(){
	return headPath;
}

string XmlManager::getBodyPath(){
	return bodyPath;
}

string XmlManager::getHeadID(){
	return headID;
}

string XmlManager::getBodyID(){
	return bodyID;
}
