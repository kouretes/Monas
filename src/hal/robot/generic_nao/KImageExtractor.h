#ifndef KIMAGEEXTRACTOR_H
#define KIMAGEEXTRACTOR_H

#include <iostream>



#include "architecture/narukom/pub_sub/blackboard.h"

#include "hal/robot/generic_nao/aldebaran-vision.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opencv/cv.h>

#define VISION_RESOLUTION kVGA
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
		float calibrateCamera(int sleeptime=1500,int exp=18);
		float getExpUs();
		int getCamera();
		int swapCamera();
	private:
		AL::ALPtr<AL::ALProxy> c;//Camera proxy to naoqi
		AL::ALPtr<AL::ALVideoDeviceProxy> xCamProxy;

		//Name used when subscribing Generic Video Module
		std::string GVM_name;
		int resolution;//Current Resolution
		int cSpace;// Current Colorspace
		bool doneSubscribe;//Initializations done?
		Blackboard *_blk;
		int lastcam;
		float lastexpusec;

};


#endif

