#ifndef KIMAGEEXTRACTOR_H
#define KIMAGEEXTRACTOR_H

#include <iostream>



#include "architecture/narukom/pub_sub/blackboard.h"

#include "hal/robot/generic_nao/aldebaran-vision.h"
#include <boost/date_time/posix_time/posix_time.hpp>


#include "KImage.h"

#define VISION_RESOLUTION kVGA
#define VISION_CSPACE AL::kYUV422InterlacedColorSpace
#define VISON_FPS 30
#define VISION_GVMNAME "KImageExtractor"

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
		boost::posix_time::ptime fetchImage(KImage & img);
		//Create new space for image
		float calibrateCamera(int sleeptime=500,int exp=15);
		float getExpUs() const;
		int getCamera() const;
		float getScale() const;
		int swapCamera();
		void refreshValues();
	private:
		AL::ALPtr<AL::ALVideoDeviceProxy> xCamProxy;


		//Name used when subscribing Generic Video Module
		std::string GVM_name;
		int resolution;//Current Resolution
		int cSpace;// Current Colorspace
		bool doneSubscribe;//Initializations done?
		float refexpusec,lastexpusec,lastcam;
		Blackboard *_blk;

		void _releaseImage();

};


#endif

