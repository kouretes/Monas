#ifndef XML_MANAGER_H
#define XML_MANAGER_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <utility>
#include <boost/filesystem.hpp>
#include <zlib.h>

#include "../external/tinyxml_2-5-3/tinyxml.h"

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
	std::vector<std::string> text;
	std::map<std::string, std::string > attributes;
	std::map<std::string, std::vector<XmlNode> > kids;
private:
	std::string headPath;
	std::string bodyPath;
	std::string headID;
	std::string bodyID;
	std::string directoryPath;
	std::vector<std::string> allFiles;
	bool root;
	unsigned int adler; //checksum
	
	int fileType;//HeadFile = 1 BodyFile = 2

	bool updateFilesValue(std::string path, std::string value, int fileType);
	void deleteNodesForKey(std::string key, int fileType);
	XmlNode * findSecondToLastNodeForKey(std::queue<std::string> & key);
	static std::string extractNumber(std::string & str, unsigned * num);//Get the ~ part
	static std::string extractNumberText(std::string & str, unsigned * num);
	static std::queue<std::string> findAllSubstring(std::string  key);
	void insertRecursivePolicyAppend(TiXmlNode* xmlNode, int fileType);
	bool updateValueForKey(std::string key, std::string value);


public:
	XmlNode()
	{
		fileType = 0;
		root = false;
	};
	std::vector<std::string> getText();
	XmlNode(std::string dirPath, std::string headId, std::string bodyId, bool administrator);
	void print(std::string pref);
	void computeAddler32();
	unsigned getChildrenCount() const;
	std::vector<std::string> getAttribute(std::string & key) ;
	bool loadFile(std::string filename, int fileType);
	bool loadAllFiles(std::string filename);
	std::vector<std::string> findValueForKey(std::string key);
	bool burstWrite(std::vector<std::pair<std::string, std::string> > writeData);
	int numberOfNodesForKey(std::string key);
	int numberOfChildrendsForKey(std::string key);
	int numberOfUniqueChildrendsForKey(std::string key);
	XmlNode* findNodeForKey(std::string key);
	std::string getHeadPath();
	std::string getBodyPath();
	std::string getHeadID();
	std::string getBodyID();
	unsigned int getChecksum();

};
#endif // XML_MANAGER_H
