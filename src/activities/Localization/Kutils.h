/*
 * Kutlis.h
 *
 *  Created on: 12 Μαρ 2010
 *      Author: trs
 */

#ifndef KUTILS_H_
#define KUTILS_H_
#include <string>
#include <iomanip>
#include <sstream>
#define TO_RAD 0.01745329f

#define TO_DEG 1.0/TO_RAD
#define DISTANCE(x1,x2,y1,y2) (sqrt(pow(x1-x2,2) + pow(y1-y2,2)))
#define DISTANCE_2(dx,dy) (sqrt(pow(dx,2) + pow(dy,2)))

//char ptr
class Kutils {
public:
		static char cptime[50];
		static std::string GetDate() {
			time_t now = time(NULL);
			strftime(cptime, 50, "%b. %d, %Y", localtime(&now)); //uses short month name
			std::string strTime = cptime;
			return strTime;
		}

		static std::string GetTime() {
			time_t now = time(NULL);
			strftime(cptime, 50, "%I:%M %p", localtime(&now));
			std::string strTime = cptime;
			return strTime;
		}

		template<typename T> static std::string to_string(const T& value) {
			std::stringstream oss;
			oss << std::setprecision(10) << value;
			return oss.str();
		};

};


#endif /* KUTILS_H_ */
