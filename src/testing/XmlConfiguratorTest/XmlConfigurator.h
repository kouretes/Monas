#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <utility>
#include "tinyxml.h"

using namespace std;
class node{
public:
	static const char DELIMITER='.';
	static const char ATTRIBUTE_DELIMITER='$';
	static const char NUMBER_DELIMITER='~';
	static const int MAIN_FILE = 0;
	static const int BODY_FILE = 1;
	static const int HEAD_FILE = 2;
	vector<string> text;
	map<string,string > attributes;
	map<string,vector<node> > kids;
	int fileType;//HeadFile = 1 BodyFile = 2
	
	void insertRecursivePolicyAppend(TiXmlNode* xmlNode,int fileType);
	static queue<string> findAllSubstring(string  key);
	static  string  extractNumber(string & str, int * num);//Get the ~ part
	node *  findNodeForKey(queue<string> & key) ;
	
	
public:
	node(){fileType=0;};
	void print(string pref);
	vector<string> getText();
	vector<string> getAttribute(string & key) ;
	node* findNodeForKey(string key) ;
	bool loadFile(string filename,int fileType);
	bool loadAllFiles(string filename);
	vector<string> findValueForKey(string key);
	bool updateValueForKey(string key, string value,int textpos);
	bool updateFilesValue(string path,string value,int textpos,int fileType);
	void deleteNodesForKey(string key,int fileType);
	bool rapidSet();
	
};
