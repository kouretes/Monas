#ifndef XML_MANAGER_H
#define XML_MANAGER_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <utility>
#include "../external/tinyxml_2-5-3/tinyxml.h"
#include "boost/filesystem.hpp"

using namespace std;

class XmlNode
{
public:
	static const char DELIMITER = '.';
	static const char ATTRIBUTE_DELIMITER = '$';
	static const char NUMBER_DELIMITER = '~';
	static const char TEXTNUMBER_DELIMITER = '#';
	static const int MAIN_FILE = 0;
	static const int BODY_FILE = 1;
	static const int HEAD_FILE = 2;
	vector<string> text;
	map<string, string > attributes;
	map<string, vector<XmlNode> > kids;
private:
	string headPath;
	string bodyPath;
	string directoryPath;
	int fileType;//HeadFile = 1 BodyFile = 2

	vector<string> getText();
	XmlNode* findNodeForKey(string key);
	bool updateFilesValue(string path, string value, int fileType);
	void deleteNodesForKey(string key, int fileType);
	XmlNode * findNodeForKey(queue<string> & key);
	static string extractNumber(string & str, unsigned * num);//Get the ~ part
	static string extractNumberText(string & str, unsigned * num);
	static queue<string> findAllSubstring(string  key);
	void insertRecursivePolicyAppend(TiXmlNode* xmlNode, int fileType);


public:
	XmlNode()
	{
		fileType = 0;
	};
	XmlNode(string dirPath, string headId, string bodyId);
	void print(string pref);
	unsigned getChildrenCount() const;
	vector<string> getAttribute(string & key) ;
	bool loadFile(string filename, int fileType);
	bool loadAllFiles(string filename);
	vector<string> findValueForKey(string key);
	bool updateValueForKey(string key, string value);
	bool burstWrite(vector<pair<string, string> > writeData);
	int numberOfNodesForKey(string key);

};
#endif // XML_MANAGER_H
