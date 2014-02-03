/*
 * matlog.h
 *
 *  Created on: Dec 5, 2012
 *      Author: trs
 */

#ifndef MATLOG_H_
#define MATLOG_H_
#include <string>
#include <vector>
#include <map>
#include <fstream>

#ifdef MAT_IO
#include "matIO/matio.h"
#include "matIO/matindex.h"
#endif

#define TIMEBUF 20 //string size to save the date,time

typedef float num_type;

using namespace std;

typedef enum  {RAW=0
#ifdef MAT_IO
	,MAT=1
#endif
	} SAVETYPE;



class mat_log
{
public:
		int log_files_counter;
		int counter;
		string log_name;
		map<string, num_type *> data;
		int autosave_period;
		char timebuf[TIMEBUF];
		 SAVETYPE type;
		ofstream rawfile;
		bool more_vars_added;
	public:

		mat_log(string base_log_name, SAVETYPE type=RAW, int autosave_period=-1);
		~mat_log();
		/// add a value to data structure
		void periodic_save();

		void initilize_datalog_variable_names(vector<string>);

		void insert(string name, num_type value);
		/// add or append a vector of values
		//	void insert(string name, vector<double> values);
		/// erase all the data
		void empty_all()
		{
			data.clear();
		}
#ifdef MAT_IO
		int saveMatTo(string filename);
#endif
		int saveRawTo(string filename);

		int save();
};


#endif /* MATLOG_H_ */
