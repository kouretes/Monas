#ifndef KIMAGEEXTRACTOR_H
#define KIMAGEEXTRACTOR_H

#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include "NaoCamera.h"

#include "KImage.h"

#define VISION_RESOLUTION VGA
#define VISION_CSPACE YUV
#define VISON_FPS 30

/**
 * Extract Image from hardware
 * Automatically deals with naoqi related stuff, and handles gracefully image size changes
 * TODO: Provide functionality to change resolution/framerate on the fly
 */


class KImageExtractor
{
public:
	KImageExtractor();

	void Init();

	~KImageExtractor();
	//Get new Image from hardware
	boost::posix_time::ptime fetchImage(KImageConst & img);
	//Create new space for image
	float calibrateCamera(int sleeptime = 500, int exp = 15);
	float getExpUs() const;
	int currentCameraIsBottom() const;
	float getScale() const;
	bool setCameraSetting (unsigned int id, int value);
	unsigned char swapCamera();
	void refreshValues();
	bool setNewUserPrefs();
private:
	NaoCamera * naocam;

	bool doneSubscribe;//Initializations done?
	float refexpusec, lastexpusec;

	void _releaseImage();
	void setDefaultSettings();
	void setCalibrateSettings();

};


#endif

