#include "XmlConfigurator.h"


string convertInt(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}




void node::print(string pref)
{
	
	//cout<<"Text:"<<endl;
	cout<<pref<<endl;
	for(vector<string>::iterator tit=text.begin();tit!=text.end();++tit)
	{
		cout<<pref<<"/t/"<<(*tit)<<endl;
	}
	
	for(map<string,string >::iterator ait=attributes.begin();ait!=attributes.end();++ait)
	{
			cout<<pref<<"Attribute:"<<(*ait).first<<"="<<(*ait).second<<endl;
	}
	//cout<<"Kids:"<<endl;

	for(map<string,vector<node> >::iterator kit=kids.begin();kit!=kids.end();++kit)
	{
		
		for(vector<node>::iterator tit=(*kit).second.begin();tit!=(*kit).second.end();++tit)
		{
			cout<<pref+"+"<<"Node:"<<(*kit).first<<endl;
			(*tit).print(pref+"+");
		}
	}
}

void node::deleteNodesForKey(string key,int fileType){
	queue<string> keys = findAllSubstring(key);
	node * secondtolast=findNodeForKey(keys);
	string lastkey = keys.front();
	int pos;

	lastkey=extractNumber(lastkey,&pos);
	if(secondtolast!=NULL && secondtolast->kids.find(lastkey)!=secondtolast->kids.end() && secondtolast->kids[lastkey].front().fileType!=fileType){
		cout << "tKey = " <<  key << endl;
		secondtolast->kids.erase(lastkey);
	}	
}

bool node::updateFilesValue(string path,string value,int textpos,int fileType){
	queue<string> keys = findAllSubstring(path);
	int filePos;
	string filename = extractNumber(keys.front(),&filePos);
	if(fileType == HEAD_FILE)
		filename.insert(0,"HEAD/");
	else if(fileType == BODY_FILE)
		filename.insert(0,"BODY/");
	keys.pop();
	filename.append(".xml");
	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();
	TiXmlNode* firstChild = doc.FirstChild();
	bool found = false;
	if(!loadOkay)
		return false;
	do{
		string key = keys.front();
		keys.pop();
		int id,idCount=0;
		//Find the correct sibling
		TiXmlNode *oldChild = firstChild;
		key = extractNumber(key,&id);
		for(TiXmlNode* child = firstChild; child; child = child->NextSibling() ){
			if(child->Value() == key){
				if(id==idCount){
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
		if(keys.size()==0){
			int numOfCurText = 0;
			for(TiXmlNode* child = firstChild->FirstChild(); child; child = child->NextSibling() ){
				if(child->Type()==TiXmlNode::TEXT){
					if(numOfCurText==textpos){
						found = true;
						child->SetValue(value);
						break;
					}
					numOfCurText++;
				}
			}
		}
		//check if the last key is attribute and change it
		if(keys.size()==1){
			string lastkey = keys.front();
			int pos;
			lastkey=extractNumber(lastkey,&pos);
			if(lastkey[0] == ATTRIBUTE_DELIMITER){
				keys.pop();
				lastkey.erase(0,1);	
				TiXmlElement *elem = firstChild->ToElement();
				if(elem == NULL)
					return false;
				elem->SetAttribute(lastkey,value);
				found = true;
			}
		}
		firstChild = firstChild->FirstChild();
	}while(keys.size()!=0);
	if(!found)
		return false;
	doc.SaveFile(filename);
	return true;
}

bool node::updateValueForKey(string key, string value, int textpos){
	queue<string> keys = findAllSubstring(key);
	node * secondtolast=findNodeForKey(keys);
	if(secondtolast==NULL)
		return false;
	string lastkey = keys.front();
	int pos;
	lastkey=extractNumber(lastkey,&pos);
	if(lastkey[0] == ATTRIBUTE_DELIMITER){
		lastkey.erase(0,1);		
		map<string,string >::iterator it = secondtolast->attributes.find(lastkey);
		if(it != attributes.end())
			(*it).second=value;
		else
			return false;
		updateFilesValue(key,value,textpos,secondtolast->fileType);
	}else{
		node * lastNode = secondtolast->findNodeForKey(lastkey);
		if(lastNode == NULL)
			return false;
		lastNode->text[textpos] = value;
		updateFilesValue(key,value,textpos,lastNode->fileType);
	}
	return true;
}

vector<string> node::findValueForKey(string key){
	queue<string> keys = findAllSubstring(key);

	node * secondtolast=findNodeForKey(keys);
	if(secondtolast==NULL)
		return vector<string>();
	//Last processing, is it an attribute?
	string lastkey=keys.front();

	if(lastkey[0]==ATTRIBUTE_DELIMITER) //Its an attribute
	{
		lastkey.erase(0,1);
		return secondtolast->getAttribute(lastkey);
	}
	//No
	int pos;
	lastkey=extractNumber(lastkey,&pos);
	if(secondtolast->kids.find(lastkey)==secondtolast->kids.end()||
			(*secondtolast->kids.find(lastkey)).second.size()<=pos)
		return vector<string>();
	else
		return (*secondtolast->kids.find(lastkey)).second[pos].text;
	
}
	
	
vector<string> node::getText(){
	return text;
}


vector<string> node::getAttribute(string & key){
	
	int pos;
	string akey=extractNumber(key,&pos);		

  	map<string,string>::iterator it = attributes.find(akey);
 
	if(it != attributes.end()){
		vector<string> attr;
		attr.push_back((*it).second);
		return attr;
		
	}else
		return vector<string>();
}

queue<string> node::findAllSubstring(string  key){
	queue<string> allKeys;
	size_t pos = key.find_first_of(DELIMITER,0);
	while (pos!=string::npos)
	{
	    allKeys.push(key.substr(0,pos));
	    key.erase(0,pos+1);
	    pos = key.find_first_of(DELIMITER,0);
	}
	allKeys.push(key);
	return allKeys;

}

node* node::findNodeForKey(string key){
	queue<string> keys = findAllSubstring(key);
	//return findNodeForKey(keys);
	
	node * secondtolast=findNodeForKey(keys);
	//Last processing, it must be a valid node, so repeat check one more time
	
	if(secondtolast==NULL)
		return NULL;
	string lastkey=keys.front();
	int pos;
	lastkey=extractNumber(lastkey,&pos);		
	
	if(secondtolast->kids.find(lastkey)==secondtolast->kids.end()||
			(*secondtolast->kids.find(lastkey)).second.size()<=pos)
		return NULL;
	else
	{
		return &(*secondtolast->kids.find(lastkey)).second[pos];
	}
}

/**
 * Process the queue of strings and returns the second-to-last
 *  Node in the queue, the last one can be tested as either an attribute
 * or as  node 
 **/
 
string  node::extractNumber(string & str, int * num)
{
	size_t pos = str.find_first_of(NUMBER_DELIMITER,0);
	
	if(pos==string::npos)
	{
		*num=0;
		return str;
	}
	else
	{	
		string strnum=str.substr(pos+1);
		string key=str.substr(0,pos);
		istringstream convert(strnum);
		
		if ( !(convert >> (*num)) ) //give the value to 'Result' using the characters in the stram
			*num = 0;
		return key;
		
	}
		
	
}

node* node::findNodeForKey(queue<string> & keys){
		
	if(keys.size()>1)
	{
		string key=keys.front();
		keys.pop();
		int pos;
		key=extractNumber(key,&pos);	
		if(kids.find(key)==kids.end()||(*kids.find(key)).second.size()<pos)
			return NULL;
		else
			return (*kids.find(key)).second[pos].findNodeForKey(keys);
			
	}
	return this;
}

bool node::loadAllFiles(string filename){
	bool loadIsOk = true;
	loadIsOk &= loadFile(filename,MAIN_FILE);
	loadIsOk &= loadFile(filename,BODY_FILE);
	loadIsOk &= loadFile(filename,HEAD_FILE);
	return loadIsOk;
}

bool node::loadFile(string filename,int fileType){
	string key = filename.substr(0,filename.find_first_of("."));
	if(fileType == HEAD_FILE)
		filename.insert(0,"HEAD/");//NA MPEI TO HEAD ID
	else if(fileType == BODY_FILE)
		filename.insert(0,"BODY/");//NA MPEI TO BODY ID
	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();
	if(!loadOkay)
		return false;
	if(kids.find(key) != kids.end()){
		for(TiXmlNode* child = doc.FirstChild(); child; child = child->NextSibling()){
			string tempKey = child->Value();
			string tKey = key;
			tKey.append(".");
			tKey.append(tempKey);
			deleteNodesForKey(tKey,fileType);	
			//ONLY ONE FILE FOR GIVEN FILENAME
			kids[key].front().insertRecursivePolicyAppend(child,fileType);
		}
	}else{
		node baby ;
		for(TiXmlNode* child = doc.FirstChild(); child; child = child->NextSibling()){
			baby.insertRecursivePolicyAppend(child,fileType);
		}
		kids[key].push_back(baby);
	}
	return true;
}

void node::insertRecursivePolicyAppend(TiXmlNode* xmlNode,int fileType){
	
	//Todo what file type for this text subnode?
	//Append text elements
	if(fileType)
		cout << xmlNode->Value() << endl;
	if(xmlNode->Type()==TiXmlNode::TEXT)
	{
		text.push_back(xmlNode->Value());
	}
	//If its an element, create a new child (policy append) and merge the element to it
	TiXmlElement *element = xmlNode->ToElement();
	if(element==NULL)
		return;
	node baby;
	baby.fileType=fileType; //Set fileType
	TiXmlAttribute *attr = element->FirstAttribute();
	while(attr!=NULL){
		string key = attr->Name();
		baby.attributes[key]=attr->Value();
		//cout << "For key = " << key << " Attr = " << attr->Value() << endl;
		attr = attr->Next();
	}
	for( TiXmlNode * child = xmlNode->FirstChild(); child; child = child->NextSibling() )
	{
			
		baby.insertRecursivePolicyAppend(child,fileType);
	}
	kids[element->Value()].push_back(baby);		
}


int main(){
	string filename = "test.xml";
	node root;
	root.loadAllFiles(filename);

	cout << root.kids.size() << endl;
	node *temp =root.findNodeForKey("test.iter_string");
	root.print("");
	cout<< "1) " << temp->text[0]<<endl;
	cout<< "2) " << root.findValueForKey("test.agent.$time").size()<<endl;
	cout<< "3) " << root.findValueForKey("test~0.agent~0.$time")[0]<<endl;
	cout<< "4) " << root.findValueForKey("test~0.agent~0")[0]<<endl;
	cout<< "5) " << root.findValueForKey("test~0.agent~0")[1]<<endl;
	//cout<< "6) " << root.findValueForKey("test~0.agent~1.module~1")[0]<<endl;
	cout<< "7) " << root.findValueForKey("test~0.map_double").size()<<endl;
	
	if(!root.updateValueForKey("test.agent.$time", "lala",0))
		cout << "Ton poulo" << endl;
	cout<< "8) " << root.findValueForKey("test~0.agent~0.$time")[0]<<endl;

	/*node *temp = root.findNodeForKey("test.iter_string");
	cout << temp->getText() << endl;
	cout << root.findValueForKey("test.map_int~2.$correctTheASDF1") << endl;
	if(root.updateValueForKey("test.map_int~2.$correctTheASDF1","Kofi"))
		cout << "Updated succesfull" << endl;
	cout << root.findValueForKey("test.iter_int~2.iter~2") << endl;
	*/
	return 0;
}

