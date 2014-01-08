#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <utility>
#include <zlib.h>

#include <boost/filesystem.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "../external/tinyxml_2-5-3/tinyxml.h"
#include "ConfigNode.hpp"
#include "core/elements/Singleton.hpp"

/**
 * This is the ConfiguratorClass for the Monas architecvture

 * @author Kofinas Nikos aka eldr4d, 2012 kouretes team

 * \file NAOKinematics.h
 * \bug nothing known
*/

/**
 * @class ConfiguratorClass
 * This class handles every valid xml file expect the files with multiple instance of text inside a tag.
 **/
class ConfiguratorClass
{
public:
	ConfigNode root;
private:
	std::string headPath;	/**< The path to the head files */
	std::string bodyPath;	/**< The path to the body files */
	std::string headID;		/**< The headId of the instance */
	std::string bodyID;		/**< The bodyId of the instance */
	std::string directoryPath;	/**< The directory pathe where the xml files lives :P*/
	std::vector<std::string> allFiles;	/**< All the xml files that have been read from this instance*/
	unsigned int adler; 	/**< Checksum of the instance*/

	boost::shared_mutex mutexrw;

	/**
	* Update a value in a corresponding file for a given key and a given fileType
	* @param targetKey: the key of the node/attribute
	* @param value: the new value
	* @param fileType: the file type
	**/
	bool updateXMLFilesValue(std::string targetKey, std::string value, int fileType);

	/**
	* Compute addler32 for same data verification with external source
	**/
	void computeAddler32();

	/**
	* Load a file for a specific filetype
	* @param filename: the name of the file to be loaded
	* @param filetype: main, head or body file
	**/
	bool loadXMLFile(std::string filename, int fileType);

	/**
	* Load a file for all the filetypes
	* @param filename: the name of the file to be loaded
	**/
	bool loadAllXMLFiles(std::string filename);
public:
	/**
	*Initilize the tree with the files from a directory
	* @param dirPath: the directory with the xml files
	* @param headId: set the head id
	* @param bodyId: set the body id
	* @param administrator: is this instance root or not
	**/
	void initConfigurator(std::string dirPath, std::string headId, std::string bodyId);

	/**
	*Empty Constructor
	**/
	ConfiguratorClass(){};

	/**
	* Print the xml tree
	**/
	void prettyPrint();

	/**
	* Find a value for a specific key (text value or attribute value
	* text value key example: firstnode.secondnode~3.lastnode~2
	* attribute value key example: firstnode.secondnode~3.lastnode~2.$attribute
	* @param key: the key of the value
	**/
	std::string findValueForKey(std::string key);

	/**
	* Find a key of a node where a subnode value of this node equales the target value
	* E.g. there are 3 nodes with the name outerNodes and they have subnodes spam.interest
	* retutn the key of the node where spam.interest.text equals targetValue (works with attributes too)
	* @param nodeKey: the outerNode key
	* @param subnodeKey: the key of the subnode with the interesting value
	* @param value: the target value
	**/
	std::string keyOfNodeForSubvalue(std::string nodeKey, std::string subnodeKey, std::string value);

	/**
	* Write multiple pairs of keys and data
	* @param writeData: a pair of key with the correspondig data value
	**/
	bool burstWrite(std::vector<std::pair<std::string, std::string> > writeData);

	/**
	* Find the node count for a specific key
	* @param key: the generic key of the node (without e.g. ~2)
	**/
	int numberOfNodesForKey(std::string key);

	/**
	* Find the children count of a father
	* @param key: the key of the father
	**/
	int numberOfChildrenForKey(std::string key);

	/**
	* Find the count for only the children with different name of a father
	* E.g. if there are two children with the name action, the count will be increased only by one
	* @param key: the key of the father
	**/
	int numberOfUniqueChildrenForKey(std::string key);

	/**
	* Return the corresponding ConfiguratorNode for a given key
	* @param key: the key of the node
	**/
	ConfigNode* findNodeForKey(std::string key);

	/**
	* Return the path where the configuration files are stored
	**/
	std::string getDirectoryPath();

	/**
	* Return the path where the head files are stored
	**/
	std::string getHeadPath();

	/**
	* Return the path where the body files are stored
	**/
	std::string getBodyPath();

	/**
	* Return the headId of the current instance
	**/
	std::string getHeadID();

	/**
	* Return the bodyId of the current instance
	**/
	std::string getBodyID();

	/**
	* Return the checksum of the current instance
	**/
	unsigned int getChecksum();
};

typedef Singleton<ConfiguratorClass> Configurator;
#endif // CONFIGURATORCLASS_H
