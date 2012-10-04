#include "XmlConfigurator.h"

using namespace std;
using namespace boost::filesystem;


string convertInt(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}

/* Initilize the tree with the files from a directory */
XmlNode::XmlNode(string dirPath, string headId, string bodyId, bool administrator)
{
	root = administrator;
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
//#if BOOST_FILESYSTEM_VERSION == 2
				filename.append(iter->string());
//#else
//				filename.append(itr->path().filename().string());
//#endif
				if(!loadAllFiles(filename)){
					//cout << "Failed to load xml file \"" << itr->path().filename() << "\"" << endl;
					;
				}
			}
		}
	}
	else
		cout << "Directory path \"" << dirPath << "\" not found" << endl;
	
	computeAddler32();
}

void XmlNode::print(string pref)
{
	//cout<<"Text:"<<endl;
	cout << pref << endl;

	for(vector<string>::iterator tit = text.begin(); tit != text.end(); ++tit)
	{
		cout << pref << "/t/" << (*tit) << endl;
	}

	for(map<string, string >::iterator ait = attributes.begin(); ait != attributes.end(); ++ait)
	{
		cout << pref << "Attribute:" << (*ait).first << "=" << (*ait).second << endl;
	}

	//cout<<"Kids:"<<endl;

	for(map<string, vector<XmlNode> >::iterator kit = kids.begin(); kit != kids.end(); ++kit)
	{
		for(vector<XmlNode>::iterator tit = (*kit).second.begin(); tit != (*kit).second.end(); ++tit)
		{
			cout << pref + "+" << "XmlNode:" << (*kit).first << endl;
			(*tit).print(pref + "+");
		}
	}
}


/**
* Delete all Nodes that much a given key
**/
void XmlNode::deleteNodesForKey(string key, int fileType)
{
	queue<string> keys = findAllSubstring(key);
	XmlNode * secondtolast = findSecondToLastNodeForKey(keys);
	string lastkey = keys.front();
	unsigned pos;
	lastkey = extractNumber(lastkey, &pos);

	if(secondtolast != NULL && secondtolast->kids.find(lastkey) != secondtolast->kids.end() && secondtolast->kids[lastkey].front().fileType != fileType)
	{
		secondtolast->kids.erase(lastkey);
	}
}


/**
*Num of Nodes for a key
**/
int XmlNode::numberOfNodesForKey(string key)
{
	queue<string> keys = findAllSubstring(key);
	//return findNodeForKey(keys);
	XmlNode * secondtolast = findSecondToLastNodeForKey(keys);
	//Last processing, it must be a valid XmlNode, so repeat check one more time

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


int XmlNode::numberOfChildrendsForKey(string key)
{
	//return findNodeForKey(keys);
	XmlNode * darthVader = findNodeForKey(key); //darthVader = father

	if(darthVader == NULL)
		return 0;

	int childrends = 0;
	map<string,vector<XmlNode> >::iterator it;
	for(it = darthVader->kids.begin(); it != darthVader->kids.end(); it++){
		childrends += (*it).second.size();
	}
	return childrends;
}
int XmlNode::numberOfUniqueChildrendsForKey(string key){
	XmlNode * darthVader = findNodeForKey(key); //darthVader = father

	if(darthVader == NULL)
		return 0;

	int childrends = darthVader->kids.size();

}
/**
* Updates a value to a file.
**/
bool XmlNode::updateFilesValue(string path, string value, int fileType)
{
	unsigned filePos, textpos;
	path = extractNumberText(path, &textpos);
	queue<string> keys = findAllSubstring(path);
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
					if(numOfCurText == textpos)
					{
						found = true;
						child->SetValue(value);
						break;
					}

					numOfCurText++;
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


/**
 * Get the number of children Nodes
 **/
unsigned XmlNode::getChildrenCount() const
{
	return kids.size();
}

/**
* Updates an attribute or a text to a given value.
**/
bool XmlNode::updateValueForKey(string key, string value)
{
	unsigned textpos;
	key = extractNumberText(key, &textpos);
	queue<string> keys = findAllSubstring(key);
	XmlNode * secondtolast = findSecondToLastNodeForKey(keys);

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
		XmlNode * lastNode = secondtolast->findNodeForKey(lastkey);

		if(lastNode == NULL)
			return false;

		lastNode->text[textpos] = value;
		key.append(string(1, TEXTNUMBER_DELIMITER));
		key.append(convertInt(textpos));
		if(root)
			updateFilesValue(key, value, lastNode->fileType);
	}

	return true;
}

/**
* Update a set of keys,values at once
**/
bool XmlNode::burstWrite(vector<pair<string, string> > writeData)
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

/**
* Returns a vector of string(values) for the given key.
**/
vector<string> XmlNode::findValueForKey(string key)
{
	queue<string> keys = findAllSubstring(key);
	XmlNode * secondtolast = findSecondToLastNodeForKey(keys);

	if(secondtolast == NULL)
		return vector<string>();

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
		return vector<string>();
	else
		return (*secondtolast->kids.find(lastkey)).second[pos].text;
}

/**
* Returns ..... Feel in the blanks
**/
vector<string> XmlNode::getText()
{
	return text;
}


/**
* Return the attribute for the given key
**/
vector<string> XmlNode::getAttribute(string & key)
{
	unsigned pos;
	string akey = extractNumber(key, &pos);
	map<string, string>::iterator it = attributes.find(akey);

	if(it != attributes.end())
	{
		vector<string> attr;
		attr.push_back((*it).second);
		return attr;
	}
	else
		return vector<string>();
}

/**
* Break the given key in subkeys.
* The DELIMITER are removed
**/
queue<string> XmlNode::findAllSubstring(string  key)
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
/**
 * Process the queue of strings and returns the second-to-last
 *  XmlNode in the queue, the last one can be tested as either an attribute
 * or as  XmlNode
 **/
XmlNode* XmlNode::findNodeForKey(string key)
{
	queue<string> keys = findAllSubstring(key);

	XmlNode * secondtolast = findSecondToLastNodeForKey(keys);
	//Last processing, it must be a valid XmlNode, so repeat check one more time

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

XmlNode* XmlNode::findSecondToLastNodeForKey(queue<string> & keys)
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

/**
* Extract the number from the given key and return
* the key without the delimiter and number
**/
string  XmlNode::extractNumber(string & str, unsigned * num)
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

/**
* Extract the number from the given key and return
* the key without the delimiter and number
**/
string  XmlNode::extractNumberText(string & str, unsigned  * num)
{
	size_t pos = str.find_last_of(TEXTNUMBER_DELIMITER, str.size());

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
		int temp;

		if ( !(convert >> temp)) //give the value to 'Result' using the characters in the stram
			temp = 0;

		if(num != NULL)
			*num = temp;

		return key;
	}
}

/**
* Load the main file and search under BODY and HEAD directory
* for the secondary files
**/
bool XmlNode::loadAllFiles(string filename)
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

/**
* Load the given file
**/
bool XmlNode::loadFile(string filename, int fileType)
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
		XmlNode baby ;

		for(TiXmlNode* child = doc.FirstChild(); child; child = child->NextSibling())
		{
			baby.insertRecursivePolicyAppend(child, fileType);
		}

		kids[key].push_back(baby);
	}
		
	return true;
}
/**
* Insert recursive all the Nodes from the xml file to our tree
**/
void XmlNode::insertRecursivePolicyAppend(TiXmlNode* xmlNode, int fileType)
{
	//Todo what file type for this text subNode?
	//Append text elements
	if(xmlNode->Type() == TiXmlNode::TEXT)
	{
		text.push_back(xmlNode->Value());
	}

	//If its an element, create a new child (policy append) and merge the element to it
	TiXmlElement *element = xmlNode->ToElement();

	if(element == NULL)
		return;

	XmlNode baby;
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

void XmlNode::computeAddler32(){
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

//Get checksum of all files
unsigned int XmlNode::getChecksum(){
	return adler;
}

string XmlNode::getHeadPath(){
	return headPath;
}
string XmlNode::getBodyPath(){
	return bodyPath;
}

