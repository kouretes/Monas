#include "hal/robot/generic_nao/KImageExtractor.h"
#include "hal/robot/generic_nao/kAlBroker.h"

#include "messages/motion.pb.h"
using boost::posix_time::ptime;
static const  boost::posix_time::ptime time_t_epoch( boost::gregorian::date(1970, 1, 1));

using namespace AL;
using namespace std;

#define MAXEXPUS 33333.33333333f //1000/30

KImageExtractor::~KImageExtractor()
{
	try
	{
		xCamProxy->unsubscribe(GVM_name);
		//		c->callVoid( "unsubscribe", GVM_name );
	}
	catch (AL::ALError& e)
	{
		throw ALError("KImageExtractor", "Destruct", "Unable to unsubscribe GVM");
	}
}

KImageExtractor::KImageExtractor() : GVM_name(VISION_GVMNAME), resolution(VISION_RESOLUTION), cSpace( VISION_CSPACE)
{
	lastcam = -1;
}

void KImageExtractor::Init(Blackboard *blk)
{
	_blk = blk;
	doneSubscribe = false;
	refexpusec = MAXEXPUS;

	try
	{
		xCamProxy = ALPtr<ALVideoDeviceProxy>( new ALVideoDeviceProxy(KAlBroker::Instance().GetBroker()));
		//c = KAlBroker::Instance().GetBroker()->getProxy( "ALVideoDevice" );
		//c->callVoid( "unsubscribe", GVM_name );
		xCamProxy->unsubscribe(GVM_name);
		GVM_name = xCamProxy->subscribe(GVM_name, resolution, cSpace, VISON_FPS );
		//		GVM_name = c->call<std::string>( "subscribe", GVM_name, resolution,
		//										 cSpace,VISON_FPS );
		refreshValues();
		doneSubscribe = true;
		//Calculate Roundtrip time
	}
	catch (AL::ALError& e)
	{
		cout << e.toString() << endl;
		throw ALError("KImageExtractor", "Construct ", "Unable to create proxies and subscribe GVM");
	}

	//_com->get_message_queue()->add_publisher(this);
}


void KImageExtractor::_releaseImage()
{
#ifdef KROBOT_IS_REMOTE_ON
	xCamProxy->releaseDirectRawImageRemote(GVM_name);
#else
	xCamProxy->releaseDirectRawImage(GVM_name);
#endif
}

/**
 * Fetch a new Image from the hardware, it automatically fixs IplImage and enclosed binary space when needed
 * Use Allocate Image for an initial allocation of an image
 */
#ifdef KROBOT_IS_REMOTE_ON
boost::posix_time::ptime KImageExtractor::fetchImage(KImageDeepCopy &img)
{
	//cout<<"KImageExtractor::fetchimage():"<<endl;
	boost::posix_time::ptime s = boost::posix_time::microsec_clock::universal_time();

	if (doneSubscribe == false)
	{
		cout << "KImageExtractor: Warning! fetchImage()  called although GVM Subscription has failed!" << endl;
		return boost::date_time::max_date_time;
	}

	// Now that you're done with the PREVIOUS  image, you have to release it from the V.I.M.
	_releaseImage();
	ALValue results;
	results = xCamProxy->getDirectRawImageRemote(GVM_name);

	if (results.getType() != ALValue::TypeArray && results.getSize() != 7)
	{
		throw ALError("KImageExtractor", "ImageRemote", "Invalid image returned.");
	}

	boost::posix_time::time_duration exp = boost::posix_time::microsec(getExpUs() / 2);
	boost::posix_time::ptime stamp = time_t_epoch + (boost::posix_time::microsec((int)results[5]) + boost::posix_time::seconds((int) results[4]));
	boost::posix_time::time_duration dur = s - (stamp - exp); //TODO:: rtt - round trip time
	dur -= boost::posix_time::seconds(dur.total_seconds());
	//cout<<boost::posix_time::to_simple_string(dur)<<endl;
	s -= dur; //True Timestamp !! Yeah!
	int width = (int) results[0];
	int height = (int) results[1];
	int nChannels = (int) results[2];
	//int colorSpace = (int) results[3];
	//int size =width*height*nChannels;
	//cout<<time<<endl;//-((int) results[4]*1000)-(int)  results[5]<<endl;
	//Change of image data size
	img.copyFrom(results[6].GetBinary(), width, height, nChannels);
	_releaseImage();
	return s;
};

#else
boost::posix_time::ptime KImageExtractor::fetchImage(KImageDeepCopy & img)
{
	//cout << "Remote method off" << endl;
	_releaseImage();
	// Now you can get the pointer to the video structure.
	ALImage* imageIn = (ALImage*)xCamProxy->getDirectRawImageLocal(GVM_name);
	//	imageIn = (ALImage*) (c->call<int> ("getDirectRawImageLocal", GVM_name));
	//cout << "GEt getDirectRawImageLocal " << endl;

	if (!imageIn)
	{
		throw ALError("KImageExtractor", "saveImageLocal", "Invalid image returned.");
	}

	//fLogProxy->info(getName(), imageIn->toString());
	// You can get some image information that you may find usefull
	const long long timeStamp = imageIn->fTimeStamp;
	img.copyFrom(imageIn->getFrame(), imageIn->fWidth, imageIn->fHeight, imageIn->fNbLayers);
	//apply correction factor to timestamp
	//    std::cout<<"img:"<<timeStamp<<endl;
	const long long secsonly = (timeStamp / 1000000LL);
	const long long microsecsonly = timeStamp - (secsonly * 1000000LL);
	//    cout<<"secsonly:"<<secsonly<<endl;
	return time_t_epoch + boost::posix_time::seconds(secsonly) + boost::posix_time::microseconds(microsecsonly); //+boost::posix_time::milliseconds(33.3333)
};
#endif


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

float KImageExtractor::calibrateCamera(int sleeptime, int exp)
{
#ifdef WEBOTS
	return 1.0f;
#endif
	std:: vector <std::string> names;
	std:: vector <float>pos;
	int redchroma, bluechroma, gain, e;
	int rchromaL, bchromaL, gainL, eL;
	int rchromaR, bchromaR, gainR, eR;
	//int expvalue=(e*510.8)/33.0;
	MotionHeadMessage hmot;
	hmot.add_parameter(0.0f);
	hmot.add_parameter(0.0f);
	hmot.set_command("setHeadInstant");
	_releaseImage();
	cout << "Calibrate Start:" << endl;

	try
	{
		xCamProxy->setParam( kCameraSelectID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoGainID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoExpositionID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoWhiteBalanceID, 0);
		SleepMs(5);
		//xCamProxy->setParam( kCameraSelectID, 1);
		//		c->callVoid( "setParam", kCameraSelectID, 1);
		xCamProxy->setParam( kCameraExposureCorrectionID, 0);
		//		c->callVoid( "setParam", kCameraExposureCorrectionID,0);
		SleepMs(10);
		xCamProxy->setParam( kCameraSelectID, 1);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoGainID, 1);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoExpositionID, 1);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoWhiteBalanceID, 1);
		SleepMs(5);
		//xCamProxy->setParam( kCameraSelectID, 1);
		//		c->callVoid( "setParam", kCameraSelectID, 1);
		xCamProxy->setParam( kCameraExposureCorrectionID, 0);
		//		c->callVoid( "setParam", kCameraExposureCorrectionID,0);
		SleepMs(10);
		//Move head to the left
		hmot.set_parameter(0, 1.57);
		hmot.set_parameter(1, 0.22);
		_blk->publishSignal(hmot, "motion");
		_blk->publish_all();
		SleepMs(100);
		//Wait for autoconf
		SleepMs(sleeptime);
		//Get Bottom camera settings
		gainL = xCamProxy->getParam( kCameraGainID);
		//		gainL=xCamProxy->getParam(kCameraGainID);
		eL = xCamProxy->getParam(kCameraExposureID);
		//		eL=xCamProxy->getParam(kCameraExposureID);
		xCamProxy->setParam( kCameraAutoGainID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoExpositionID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoWhiteBalanceID, 0);
		SleepMs(5);
		//		c->callVoid( "setParam", kCameraAutoGainID, 0);
		//		c->callVoid( "setParam", kCameraAutoExpositionID,0);
		//		c->callVoid( "setParam", kCameraAutoWhiteBalanceID,0);
		cout << "Left settings:" << " " << gainL << endl;
		hmot.set_parameter(0, -1.57);
		hmot.set_parameter(1, 0.22);
		_blk->publishSignal(hmot, "motion");
		_blk->publish_all();
		//m->callVoid("setAngles",names,pos,0.8);
		SleepMs(100);
		//		c->callVoid( "setParam", kCameraAutoGainID, 1);
		//		c->callVoid( "setParam", kCameraAutoExpositionID,1);
		//		c->callVoid( "setParam", kCameraAutoWhiteBalanceID,1);
		xCamProxy->setParam(  kCameraAutoGainID, 1);
		SleepMs(5);
		xCamProxy->setParam(  kCameraAutoExpositionID, 1);
		SleepMs(5);
		xCamProxy->setParam(  kCameraAutoWhiteBalanceID, 1);
		//wait for autoconf
		SleepMs(sleeptime);
		//GET BOTTOM CAMERA SETTINGS!!!
		gainR = xCamProxy->getParam(kCameraGainID);
		SleepMs(5);
		eR = xCamProxy->getParam(kCameraExposureID);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoGainID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoExpositionID, 0);
		SleepMs(5);
		//Since now we`ll need again the wb correction, leave it on
		//xCamProxy->setParam( kCameraAutoWhiteBalanceID,0);
		cout << "Right settings:" << " " << gainR << endl;
		//============================
		// Final Exposure  settings!
		//============================
		e = (eL + eR) / 2;
		refexpusec = e * MAXEXPUS / 510.0;

		if ((exp * 510.0f) / 33.33333333f < e)
		{
			e = exp * 510.0f / 33.33333333f;
		}

		lastexpusec = e * MAXEXPUS / 510.0f;
		cout << "Exposure Scaling:" << getScale() << ", from" << refexpusec << "usec to " << lastexpusec << "usec" << endl;
		//cout<<"Scaling  exposure:"<<(exp*510)/33<<" "<<e<<endl;
		gain = encodeGain((decodeGain(gainL) + decodeGain(gainR)) / 2);
		//redchroma=128-((128.0-redchroma)*0.95);
		//bluechroma=128-((128.0-bluechroma)*0.95);
		//gain=((gain&31)/2)|(gain&32);
		cout << "Final Exposure settings:" << "Exposure Value:" << e << " Gain Value:" << gain << endl;
		//Start white balance calibration
		//wait for autoconf
		SleepMs(sleeptime);
		rchromaR = xCamProxy->getParam(kCameraRedChromaID);
		SleepMs(5);
		bchromaR = xCamProxy->getParam(kCameraBlueChromaID);
		SleepMs(5);
		//        xCamProxy->setParam( kCameraAutoWhiteBalanceID,0);
		cout << "Right white balance settings:" << rchromaR << " " << bchromaR << endl;
		//Move head to the left
		hmot.set_parameter(0, 1.57);
		hmot.set_parameter(1, 0.22);
		_blk->publishSignal(hmot, "motion");
		_blk->publish_all();
		//wait for autoconf
		SleepMs(sleeptime);
		rchromaL = xCamProxy->getParam(kCameraRedChromaID);
		SleepMs(5);
		bchromaL = xCamProxy->getParam(kCameraBlueChromaID);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoWhiteBalanceID, 0);
		SleepMs(5);
		cout << "Left white balance settings:" << rchromaL << " " << bchromaL << endl;
		redchroma = (rchromaL + rchromaR) / 2;
		bluechroma = (bchromaL + bchromaR) / 2;
		cout << "Final White Balance Settings" << redchroma << " " << bluechroma;
		xCamProxy->setParam( kCameraSelectID, 0);
		SleepMs(150);
		//SET BOTTOM CAMERA SETTINGS
		xCamProxy->setParam( kCameraAutoGainID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoExpositionID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoWhiteBalanceID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraBlueChromaID, bluechroma);
		SleepMs(5);
		xCamProxy->setParam( kCameraRedChromaID, redchroma);
		SleepMs(5);
		xCamProxy->setParam( kCameraGainID, gain);
		SleepMs(5);
		xCamProxy->setParam( kCameraExposureID, e);
		SleepMs(5);
		//c->callVoid( "setParam", kCameraExposureCorrectionID,-6);
		SleepMs(150);
		//c->callVoid( "setParam", kCameraSelectID, 0);
		//SleepMs(10);
		xCamProxy->setParam( kCameraSelectID, 1);
		SleepMs(150);
		//SET BOTTOM CAMERA SETTINGS
		xCamProxy->setParam( kCameraAutoGainID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoExpositionID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoWhiteBalanceID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraBlueChromaID, bluechroma);
		SleepMs(5);
		xCamProxy->setParam( kCameraRedChromaID, redchroma);
		SleepMs(5);
		xCamProxy->setParam( kCameraGainID, gain);
		SleepMs(5);
		xCamProxy->setParam( kCameraExposureID, e);
		//c->callVoid( "setParam", kCameraExposureCorrectionID,-6);
		SleepMs(150);
		//Start with bottom cam
		//xCamProxy->setParam( kCameraSelectID, 1);
	}
	catch (AL::ALError &e)
	{
		cout << "No Autosettings available ... ?!?" << endl;
		std::cout << e.toString() << std::endl;
		//exit(0);
	}

	lastcam = 1;
	cout << "done" << endl;
	hmot.set_parameter(0, 0);
	hmot.set_parameter(1, -0.1);
	_blk->publishSignal(hmot, "motion");
	_blk->publish_all();
	return getScale();
}
void KImageExtractor::refreshValues()
{
	lastcam = xCamProxy->getParam(kCameraSelectID);
#ifndef WEBOTS
	int a = xCamProxy->getParam(kCameraExposureID);
	lastexpusec = a * MAXEXPUS / 510.0f;
#endif
}
int KImageExtractor::getCamera() const
{
	return lastcam;
}

int KImageExtractor::swapCamera()
{
	int old = xCamProxy->getParam(kCameraSelectID);
	old = (old == 1) ? 0 : 1;
	xCamProxy->setParam( kCameraSelectID, old);
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
#ifdef WEBOTS
	return 1.0f;
#else
	return refexpusec / lastexpusec;
#endif
}
