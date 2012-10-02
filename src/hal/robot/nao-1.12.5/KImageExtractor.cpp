#include "hal/robot/generic_nao/KImageExtractor.h"
#include "hal/robot/generic_nao/kAlBroker.h"
#include <qi/os.hpp>
#include "messages/motion.pb.h"
using boost::posix_time::ptime;
static const  boost::posix_time::ptime time_t_epoch( boost::gregorian::date(1970, 1, 1));

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
	naocam = new NaoCamera();
}

void KImageExtractor::Init(Blackboard *blk)
{
	_blk = blk;
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
boost::posix_time::ptime KImageExtractor::fetchImage(KImageConst & img)
{
	//cout << "Remote method off" << endl;
	_releaseImage();
	do
	{
		// Now you can get the pointer to the video structure.
		if(naocam->captureNew()==false)
		{
			return boost::posix_time::ptime();
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
#ifdef WEBOTS
	return 1.0f;
#else
	return lastexpusec;
#endif
}


float KImageExtractor::getScale() const
{
	return 1.0f;
#ifdef WEBOTS
	return 1.0f;
#else
	return refexpusec / lastexpusec;
#endif
}
