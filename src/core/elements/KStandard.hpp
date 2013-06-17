#include "tools/toString.h"
#include <string>

using std::vector;
using std::string;

/**
 * Kouretes Standard library, used for general functions.
 */
namespace KStandard {

	/**
	 * @fn template <typename T> void print(vector<T> &toPrint, string topic)
	 * @brief Print vector elements using logger.
	 */
	template <typename T>
	void print(vector<T> &toPrint, string topic) {
		LogEntry msg(LogLevel::Info,topic);
		for(unsigned int it = 0 ; it < toPrint.size() ; it++)
			msg <<(toPrint[it]) <<" ";
	}

	/**
	 * @fn template <typename T> void print(vector<T> &toPrint, string topic, unsigned int index)
	 * @brief Print vector elements using logger.
	 */
	template <typename T>
	void print(vector<T> &toPrint, string topic, unsigned int index) {
		LogEntry msg(LogLevel::Info,topic);
		msg<< "[" << (index) << "] ";
		for(unsigned int it = 0 ; it < toPrint.size() ; it++)
			msg <<(toPrint[it]) <<" ";
	}

	/**
	 * @fn template <typename T> void print(vector< vector<T> > &toPrint, string topic)
	 * @brief Print elements of 2-dimensional vector using the above print function.
	 */
	template <typename T>
	void print(vector< vector<T> > &toPrint, string topic) {
		for(unsigned int it = 0 ; it < toPrint.size() ; it++) {
			print(toPrint[it], topic, it+1);
		}
	}

};
