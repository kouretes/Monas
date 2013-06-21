#include "ConfigNode.hpp"

using namespace std;
using namespace boost::filesystem;

string ConfigNode::convertInt(int number)
{
	stringstream ss;	//create a stringstream
	ss << number;		//add number to the stream
	return ss.str();	//return a string with the contents of the stream
}

void ConfigNode::prettyPrint(string pref)
{
	cout << pref << endl;

	cout << pref << "/t/" << text << endl;


	for(map<string, string >::iterator ait = attributes.begin(); ait != attributes.end(); ++ait)
	{
		cout << pref << "Attribute:" << (*ait).first << "=" << (*ait).second << endl;
	}


	for(map<string, vector<ConfigNode> >::iterator kit = kids.begin(); kit != kids.end(); ++kit)
	{
		for(vector<ConfigNode>::iterator tit = (*kit).second.begin(); tit != (*kit).second.end(); ++tit)
		{
			cout << pref + "+" << "ConfigNode:" << (*kit).first << endl;
			(*tit).prettyPrint(pref + "+");
		}
	}
}


void ConfigNode::deleteNodesForKey(string key, int fileType)
{
	queue<string> keys = findAllSubstring(key);
	ConfigNode * secondtolast = findSecondToLastNodeForKey(keys);
	string lastkey = keys.front();
	unsigned pos;
	lastkey = extractNumber(lastkey, &pos);

	if(secondtolast != NULL && secondtolast->kids.find(lastkey) != secondtolast->kids.end() && secondtolast->kids[lastkey].front().fileType != fileType)
	{
		secondtolast->kids.erase(lastkey);
	}
}

int ConfigNode::numberOfNodesForKey(string key)
{
	queue<string> keys = findAllSubstring(key);
	//return findNodeForKey(keys);
	ConfigNode * secondtolast = findSecondToLastNodeForKey(keys);
	//Last processing, it must be a valid ConfigNode, so repeat check one more time

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

int ConfigNode::numberOfChildrenForKey(string key)
{
	//return findNodeForKey(keys);
	ConfigNode * darthVader = findNodeForKey(key); //darthVader = father

	if(darthVader == NULL)
		return 0;

	int Children = 0;
	map<string,vector<ConfigNode> >::iterator it;
	for(it = darthVader->kids.begin(); it != darthVader->kids.end(); it++){
		Children += (*it).second.size();
	}
	return Children;
}

int ConfigNode::numberOfUniqueChildrenForKey(string key){
	ConfigNode * darthVader = findNodeForKey(key); //darthVader = father

	if(darthVader == NULL)
		return 0;

	return darthVader->getChildrenCount();
}

unsigned ConfigNode::getChildrenCount() const
{
	return kids.size();
}

int ConfigNode::updateValueForKey(string key, string value)
{
	queue<string> keys = findAllSubstring(key);
	ConfigNode * secondtolast = findSecondToLastNodeForKey(keys);

	if(secondtolast == NULL)
		return -1;

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
			return -1;
		return secondtolast->fileType;
	}
	else
	{
		ConfigNode * lastNode = secondtolast->findNodeForKey(lastkey);

		if(lastNode == NULL)
			return -1;

		lastNode->text = value;
		return lastNode->fileType;
	}
	return -1;
}

string ConfigNode::findValueForKey(string key)
{
	queue<string> keys = findAllSubstring(key);
	ConfigNode * secondtolast = findSecondToLastNodeForKey(keys);

	if(secondtolast == NULL)
		return "0.0";

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
		return "0.0";
	else
		return (*secondtolast->kids.find(lastkey)).second[pos].text;
}

string ConfigNode::getText()
{
	return text;
}

string ConfigNode::keyOfNodeForSubvalue(string nodeKey, string subnodeKey, string value){
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

string ConfigNode::getAttribute(string & key)
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

std::queue<std::string> ConfigNode::findAllSubstring(string key)
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

ConfigNode* ConfigNode::findNodeForKey(string key)
{
	queue<string> keys = findAllSubstring(key);

	ConfigNode * secondtolast = findSecondToLastNodeForKey(keys);
	//Last processing, it must be a valid ConfigNode, so repeat check one more time

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

ConfigNode* ConfigNode::findSecondToLastNodeForKey(queue<string> & keys)
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

string  ConfigNode::extractNumber(string & str, unsigned * num)
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

bool ConfigNode::loadXMLFile(string filename, int fileType)
{

	if(filename.substr(filename.find_last_of(".") + 1, filename.size() - 1) != "xml")
		return false;
		
	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();

	if(!loadOkay)
		return false;	
	
	string key = filename.substr(0, filename.find_last_of("."));
	key = key.substr(key.find_last_of("/") + 1, key.size() - 1);
	
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
		ConfigNode baby ;

		for(TiXmlNode* child = doc.FirstChild(); child; child = child->NextSibling())
		{
			baby.insertRecursivePolicyAppend(child, fileType);
		}

		kids[key].push_back(baby);
	}
		
	return true;
}

void ConfigNode::insertRecursivePolicyAppend(TiXmlNode* xmlNode, int fileType)
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

	ConfigNode baby;
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
