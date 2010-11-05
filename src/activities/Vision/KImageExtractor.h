#ifndef KIMAGEEXTRACTOR_H
#define KIMAGEEXTRACTOR_H

#include <iostream>

#include <albroker.h>
#include <alproxy.h>

#include "architecture/narukom/pub_sub/blackboard.h"


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
#define VISION_CSPACE AL::kYUV422InterlacedColorSpace
#define VISON_FPS 30
#define VISION_GVMNAME "KImageExtractor"


//#define REMOTE_ON 1
#define  RAW



/**
 * Extract Image from hardware
 * Automatically deals with naoqi related stuff, and handles gracefully image size changes
 * TODO: Provide functionality to change resolution/framerate on the fly
 */
class KImageExtractor
{
	public:
		KImageExtractor();

		void Init(Blackboard *blk);

		~KImageExtractor();
		//Get new Image from hardware
		boost::posix_time::ptime fetchImage(IplImage *img);
		//Create new space for image
		IplImage *allocateImage();
		float calibrateCamera(int sleeptime=1500,int exp=15);
		float getExp();
		int getCamera();
		int swapCamera();
	private:
		AL::ALPtr<AL::ALProxy> c;//Camera proxy to naoqi

		//Name used when subscribing Generic Video Module
		std::string GVM_name;
		int resolution;//Current Resolution
		int cSpace;// Current Colorspace
		bool doneSubscribe;//Initializations done?
		Blackboard *_blk;
		int lastcam;

};


#endif

