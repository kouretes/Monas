#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <utility>
#include "../external/tinyxml_2-5-3/tinyxml.h"

using namespace std;
class node{
protected:
	static const char DELIMITER='.';
	static const char ATTRIBUTE_DELIMITER='$';
	static const char NUMBER_DELIMITER='~';
	static const char TEXTNUMBER_DELIMITER='#';
	static const int MAIN_FILE = 0;
	static const int BODY_FILE = 1;
	static const int HEAD_FILE = 2;
	vector<string> text;
	map<string,string > attributes;
	map<string,vector<node> > kids;
	int fileType;//HeadFile = 1 BodyFile = 2

	void insertRecursivePolicyAppend(TiXmlNode* xmlNode,int fileType);
	static queue<string> findAllSubstring(string  key);
	static string  extractNumber(string & str, unsigned * num);//Get the ~ part
	static string  extractNumberText(string & str, unsigned * num);
	node * findNodeForKey(queue<string> & key) ;


public:
	node(){fileType=0;};
	void print(string pref);
	vector<string> getText();
    unsigned getChildrenCount() const;
	vector<string> getAttribute(string & key);
	node* findNodeForKey(string key) ;
	bool loadFile(string filename,int fileType);
	bool loadAllFiles(string filename);
	vector<string> findValueForKey(string key);
	bool updateValueForKey(string key, string value);
	bool updateFilesValue(string path,string value,int fileType);
	void deleteNodesForKey(string key,int fileType);
	bool burstWrite(vector<pair<string,string> > writeData);

};
