#include "hal/robot/nao/generic_nao/KImageExtractor.h"
#include "hal/robot/nao/generic_nao/kAlBroker.h"
#include <qi/os.hpp>
#include "core/include/Logger.hpp"

using KSystem::Time::TimeAbsolute;
static const  KSystem::Time::TimeAbsolute time_t_epoch( boost::gregorian::date(1970, 1, 1));

using namespace AL;
using namespace std;

#define MAXEXPUS 33333.33333333f //1000/30

KImageExtractor::~KImageExtractor()
{
	if(naocam!=NULL)
	{
		delete naocam;
		naocam= NULL;
	}
}

KImageExtractor::KImageExtractor()
{
	naocam=NULL;
	NaoCamera::userPrefs newPrefs;
	newPrefs.GAIN = atoi(Configurator::Instance().findValueForKey("camera.Gain").c_str());
	newPrefs.CONTRAST = atoi(Configurator::Instance().findValueForKey("camera.Contrast").c_str());
	newPrefs.GREEN_GAIN = atoi(Configurator::Instance().findValueForKey("camera.GreenChannelGain").c_str());
	newPrefs.RED_BALANCE = atoi(Configurator::Instance().findValueForKey("camera.RedBalance").c_str());
	newPrefs.BLUE_BALANCE = atoi(Configurator::Instance().findValueForKey("camera.BlueBalance").c_str());
	naocam = new NaoCamera(newPrefs);
}

void KImageExtractor::Init()
{
	doneSubscribe = false;
	refexpusec = MAXEXPUS;

	refreshValues();
	//_com->get_message_queue()->add_publisher(this);
}


void KImageExtractor::_releaseImage()
{
	//Nil
}

/**
 * Fetch a new Image from the hardware, it automatically fixs IplImage and enclosed binary space when needed
 * Use Allocate Image for an initial allocation of an image
 */
KSystem::Time::TimeAbsolute KImageExtractor::fetchImage(KImageConst & img)
{
	//cout << "Remote method off" << endl;
	_releaseImage();
	do
	{
		bool imageOk = false;
		// Now you can get the pointer to the video structure.
		for(int i=0; i<20; i++){
			if(naocam->captureNew()==false)
			{
				continue;
			}else{
				imageOk = true;
				break;
			}
		}
		if(!imageOk){
			LogEntry(LogLevel::Warning,"KImageExtractor")<< "Camera is dead, reinitializing NaoCamera"<<std::endl;
			delete naocam;
			NaoCamera::userPrefs newPrefs;
			newPrefs.GAIN = atoi(Configurator::Instance().findValueForKey("camera.Gain").c_str());
			newPrefs.CONTRAST = atoi(Configurator::Instance().findValueForKey("camera.Contrast").c_str());
			newPrefs.GREEN_GAIN = atoi(Configurator::Instance().findValueForKey("camera.GreenChannelGain").c_str());
			newPrefs.RED_BALANCE = atoi(Configurator::Instance().findValueForKey("camera.RedBalance").c_str());
			newPrefs.BLUE_BALANCE = atoi(Configurator::Instance().findValueForKey("camera.BlueBalance").c_str());
			naocam = new NaoCamera(newPrefs);
		}
		img.copyFrom(naocam->getImage(),NaoCamera::WIDTH,NaoCamera::HEIGHT,2);
	}
	while(naocam->getImage()==0);

	return naocam->getTimeStamp();
}

//In 16ths

inline unsigned int decodeGain(unsigned char inValue)
{
	unsigned int c;
	unsigned char v = inValue >> 4;

	for(c = 0; v; c++)//Count 1 bits
	{
		v &= v - 1; // clear the least significant bit set
	}

	return (((inValue & 0xF) + 16) * (1 << c));
}

//In 16ths
inline unsigned char encodeGain(unsigned int inValue)
{
	unsigned char y = 0;

	while(inValue > 0x1F) //Maximum 16ths value is 16+15
	{
		y++;
		inValue >>= 1;
	}

	if(inValue >= 16)
		inValue -= 16;

	return (((1 << y) - 1) << 4) | (inValue);
}

void KImageExtractor::setDefaultSettings()
{
/*
	naocam->setControlSetting(V4L2_CID_AUTOEXPOSURE , 0);
	naocam->setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE, 0);
	naocam->setControlSetting(V4L2_CID_AUTOGAIN, 0);
	naocam->setControlSetting(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
	naocam->setControlSetting(V4L2_CID_GAIN,0xF0);
	naocam->setControlSetting(V4L2_CID_EXPOSURE,);
	naocam->setControlSetting(V4L2_CID_RED_BALANCE,redchroma );
	naocam->setControlSetting(V4L2_CID_BLUE_BALANCE,bluechroma);*/

//	naocam->setControlSetting(V4L2_CID_SAT_AUTO, 0);
//	naocam->setControlSetting(V4L2_CID_HUE_AUTO, 0);
//	naocam->setControlSetting(V4L2_CID_HUE, 0);
//	naocam->setControlSetting(V4L2_CID_SATURATION, 255);
//	naocam->setControlSetting(V4L2_CID_BRIGHTNESS, 128);
//	naocam->setControlSetting(V4L2_CID_CONTRAST, 96);
}

void KImageExtractor::setCalibrateSettings()
{
/*
	naocam-> setControlSetting(V4L2_CID_AUTOEXPOSURE ,1);
	naocam->setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE, 1);
	naocam-> setControlSetting(V4L2_CID_AUTOGAIN, 1);
	naocam-> setControlSetting(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_AUTO);*/
//	naocam->setControlSetting(V4L2_CID_HUE, 0);
//	naocam->setControlSetting(V4L2_CID_SATURATION, 255);
//	naocam->setControlSetting(V4L2_CID_BRIGHTNESS, 128);
//	naocam->setControlSetting(V4L2_CID_CONTRAST, 96);

}

float KImageExtractor::calibrateCamera(int sleeptime, int exp)
{
	_releaseImage();

	naocam->switchToUpper();
	naocam->setControlSetting(V4L2_CID_EXPOSURE,exp*510.0f / 33.33333333f);


	naocam->switchToLower();
	naocam->setControlSetting(V4L2_CID_EXPOSURE,exp*510.0f / 33.33333333f);
	lastexpusec=exp;
		return 1;


}

void KImageExtractor::refreshValues()
{
	int a= naocam->getControlSetting(V4L2_CID_EXPOSURE);
	lastexpusec = a * MAXEXPUS / 510.0f;
}

bool KImageExtractor::setNewUserPrefs(){
	NaoCamera::userPrefs newPrefs;
	newPrefs.GAIN = atoi(Configurator::Instance().findValueForKey("camera.Gain").c_str());
	newPrefs.CONTRAST = atoi(Configurator::Instance().findValueForKey("camera.Contrast").c_str());
	newPrefs.GREEN_GAIN = atoi(Configurator::Instance().findValueForKey("camera.GreenChannelGain").c_str());
	newPrefs.RED_BALANCE = atoi(Configurator::Instance().findValueForKey("camera.RedBalance").c_str());
	newPrefs.BLUE_BALANCE = atoi(Configurator::Instance().findValueForKey("camera.BlueBalance").c_str());
	return naocam->setUserSettings(newPrefs);
}

int KImageExtractor::currentCameraIsBottom() const
{
	return naocam->getCurrentCamera()==NAO_LOWER_CAMERA;
}

unsigned char KImageExtractor::swapCamera()
{

	int old=naocam->getCurrentCamera();
	old = (old == NAO_LOWER_CAMERA) ? NAO_UPPER_CAMERA : NAO_LOWER_CAMERA;
	naocam->switchCamera(old);
	return old;
}

float KImageExtractor::getExpUs() const
{
	return lastexpusec;
}

float KImageExtractor::getScale() const
{
	return 1.0f;
}
