#include "tools/toString.h"
#include <string>

using std::vector;
using std::string;

/**
 * Kouretes Standard library, used for general functions.
 */
namespace KStandard {

	/**
	 * @fn template <typename T> void print(vector<T> &toPrint)
	 * @brief Print vector elements using logger. 
	 */
	template <typename T>
	void print(vector<T> &toPrint, string topic) {
		string msg;
		for(unsigned int it = 0 ; it < toPrint.size() ; it++)
			msg = msg.append(_toString(toPrint[it])+" ");
			
		Logger::Instance().WriteMsg(topic, msg, Logger::Info);
	}
	
	/**
	 * @fn template <typename T> void print(vector< vector<T> > &toPrint)
	 * @brief Print elements of 2-dimensional vector using the above print function.
	 */
	template <typename T>
	void print(vector< vector<T> > &toPrint, string topic) {
		for(unsigned int it = 0 ; it < toPrint.size() ; it++) {
			print(toPrint[it], topic);
		}
	}
	
};
