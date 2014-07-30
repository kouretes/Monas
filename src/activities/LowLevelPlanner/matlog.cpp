/*
 * matlog.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: trs
 */

#include "matlog.h"
#include <iostream>
#include <string.h>
#include <ctime>
#include <sstream>
#include <stdexcept>      // std::out_of_range
#include <typeinfo>

template<class T>
std::string _toString(T val) {
	std::ostringstream ost;
	ost << val;
	ost.flush();
	return ost.str();
}

mat_log::mat_log(string base_log_name, SAVETYPE type, int autosave_period) {
	log_files_counter = 0;
	this->log_name = base_log_name;
	counter = 0;
	std::time_t t = std::time(NULL);
	std::tm tm = *std::localtime(&t);
	std::strftime(timebuf, TIMEBUF, "%F_%H%Ms%S", &tm);
	this->type = type;
	this->autosave_period = autosave_period;
	more_vars_added = false;
}
;

void mat_log::insert(string name, num_type value) {
	num_type *table = 0;
	try {
		num_type *table = data.at(name);    // vector::at throws an out-of-range
		table[counter] = value;	//if name does exist in the maps then
	} catch (const std::out_of_range& oor) {

		std::clog << " Creating new key entry: " << name
				<< "\t Array size (bytes): "
				<< autosave_period * sizeof(num_type) << '\n';
		table = new num_type[autosave_period];
		memset(table, 0, autosave_period * sizeof(num_type));
		table[counter] = value;
		data[name] = table;
		more_vars_added = true;
	}

}

mat_log::~mat_log() {
	if (type == RAW) {
		for (map<string, num_type *>::iterator it = data.begin();
				it != data.end(); ++it)

			delete[] (it->second);

	}
}

#ifdef MAT_IO

int mat_log::saveMatTo(string filename) {
	mat_t *matfp;
	unsigned int i;
	matvar_t *matvar;
	size_t dims[2] = { 10, 1 };
	double * matdata;

	filename += "_" + string(timebuf);

	if (filename.find(".mat") == std::string::npos)
		filename += ".mat";

	matfp = Mat_Open((filename).c_str(), MAT_ACC_RDWR | MAT_FT_MAT5);

	if (NULL == matfp) {
		std::cerr << "Error opening MAT file " << filename << std::endl;
		return -1;
	}

	for (map<string, num_type *>::iterator it = data.begin(); it != data.end();
			++it) {
		std::cout << "name " << it->first << " data size()"
				<< sizeof(it->second) << " size"
				<< sizeof(it->second) / sizeof(num_type) << "\n";

		dims[0] = sizeof(it->second) / sizeof(num_type);
		//dims[0] = ((vector<double> ) it->second).size();
		matdata = it->second;	//new double[dims[0]];
//			for (i = 0; i < dims[0]; i++)
//				matdata[i] = ((vector<double> ) it->second).at(i);

		matvar = Mat_VarCreate((it->first).c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE,
				2, dims, matdata, 0); //make variable
		Mat_VarWrite(matfp, matvar, MAT_COMPRESSION_NONE); // save variable
		Mat_VarFree(matvar); //free mat variable

		delete matdata; //free data
	}
	return 0;

}
#endif

//
int mat_log::save() {
	int ret = -1;
	if (type == RAW)
		ret = this->saveRawTo(log_name + _toString(log_files_counter++)+string(timebuf));     //
#ifdef MAT_IO
	else if (type == MAT)
		ret = this->saveMatTo(log_name + _toString(log_files_counter++));
#endif

	return ret;
}

int mat_log::saveRawTo(string filename) {

	if (more_vars_added)     //must resave variables names.txt ?
		if (!rawfile.is_open()) {
			rawfile.open((filename + ".bin").c_str(),
					ios::out | ios::app | ios::binary);
		}

	if (rawfile.fail()) {
		std::cerr << "Error opening file for logging" << filename << std::endl;
		return -1;
	}

	if (more_vars_added) {
		ofstream namesfn;
		namesfn.open((filename + "_varnames.csv").c_str(),
				std::ofstream::out | std::ofstream::app);
		//precision,
		namesfn <<  typeid(num_type).name() << ',' <<_toString(sizeof(num_type)) << ',' << _toString(autosave_period) << ',' << _toString(data.size());
		for (map<string, num_type *>::iterator it = data.begin(); //save all variables names
				it != data.end(); ++it) {
			namesfn  << ','<< (string) (it->first)  ;
		}
		namesfn.close();

	}
	for (map<string, num_type *>::iterator it = data.begin(); it != data.end();
			++it) {
		//std::cout << it->second[0] << std::endl;
		//std::cout <<typeid(float).name() << typeid(int).name() << std::endl;
		//Write per variable (autosave_period) blocks
		rawfile.write(reinterpret_cast<char*>(it->second), autosave_period * sizeof(num_type));
	}

	return 0;
}

void mat_log::periodic_save() {
	if (++counter >= autosave_period) {
		counter = 0;
		save();
		more_vars_added = false;
	}
}
