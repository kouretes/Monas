#ifndef CodeEtiquette_H
#define CodeEtiquette_H

//Linux includes
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pthread.h>
#include <netinet/in.h>
#include <math.h>
#include <csignal>

//Grouped Monas includes
#include "architecture/executables/IActivity.h"
#include "architecture/archConfig.h"

#include "hal/robot/generic_nao/robot_consts.h"

//DONT USE DEFINES
#define something 2 //DONT USE THIS
//DONT USE DEFINES
//defines is bad because sometimes someone else must change something and he can't understand where this define is used and for more reasons
//e-mail vosk for more bad reasons:P

//Class name: Continuos words without "_" and each word starts with a capital letter
class CodeEtiquette: public IActivity
{

public:
	//variables names: Continuos words without "_" and each word starts with a capital letter EXCEPT the first word
	//BE CAREFULL, dont use variable names that only you can decode (example LPWAverage "localization particles weight average") and don't use
	//to big variable names (example "localizationParticlesWeightAverage").
	//Find something smaller (example partclsAvgWeight)
	int something;
	float somethingElse;
	double oneMoreUseless;
	
	
	//functions names: Continuos words without "_" and each word starts with a capital letter EXCEPT the first word
	float aFunction(int firstVar, int secondVar);
	double doubleFunctionForVectorsAdd(int firstVector[15], int secondVector[15]);
	
private:
	
	//variables that only used for internal computation is better to be private
	int variableForInternalUse;
	//Same for functions
	double supportFunctionForVectorAdd(int something, int somethingElse);

};
#endif
