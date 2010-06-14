#ifndef KIMAGEEXTRACTOR_H
#define KIMAGEEXTRACTOR_H

#include <iostream>

#include <albroker.h>
#include <alproxy.h>

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/publisher.h"

#include "alxplatform.h"

//#include <almemoryfastaccess.h>
#include "almotionproxy.h"
#include "alvideodeviceproxy.h"
#include "alvision/alvisiondefinitions.h"
#include "alvision/alimage.h"
//#include "albrokermanager.h"
#include "alptr.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opencv/cv.h>

#define VISION_RESOLUTION kQVGA
#define VISION_CSPACE kYUV422InterlacedColorSpace
#define VISON_FPS 5
#define VISION_GVMNAME "KImageExtractor"


#define REMOTE_ON 1
#define  RAW



/**
 * Extract Image from hardware
 * Automatically deals with naoqi related stuff, and handles gracefully image size changes
 * TODO: Provide functionality to change resolution/framerate on the fly
 */
class KImageExtractor : public Publisher
{
	public:
		KImageExtractor();

		void Init(Narukom* com);

		~KImageExtractor();
		//Get new Image from hardware
		boost::posix_time::ptime fetchImage(IplImage *img);
		//Create new space for image
		IplImage *allocateImage();
		float calibrateCamera(int sleeptime=1500,int exp=18);
		float getExp();
		int getCamera();
	private:
		AL::ALPtr<AL::ALProxy> c;//Camera proxy to naoqi
		AL::ALPtr<AL::DCMProxy> dcm;

		//Name used when subscribing Generic Video Module
		std::string GVM_name;
		int resolution;//Current Resolution
		int cSpace;// Current Colorspace
		bool doneSubscribe;//Initializations done?
		unsigned rtt;
		Narukom* _com;

};


#endif

