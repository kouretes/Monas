#include "hal/robot/generic_nao/KImageExtractor.h"
#include "hal/robot/generic_nao/kAlBroker.h"

#include "messages/motion.pb.h"
using boost::posix_time::ptime;
static const  boost::posix_time::ptime time_t_epoch( boost::gregorian::date(1970,1,1));

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

KImageExtractor::KImageExtractor() :GVM_name(VISION_GVMNAME),resolution(VISION_RESOLUTION),cSpace( VISION_CSPACE)
{
	lastcam=-1;
}

void KImageExtractor::Init(Blackboard *blk)
{
	_blk = blk;
	doneSubscribe=false;
	refexpusec=MAXEXPUS;
	try
	{
		xCamProxy = ALPtr<ALVideoDeviceProxy>( new ALVideoDeviceProxy(KAlBroker::Instance().GetBroker()));

		//c = KAlBroker::Instance().GetBroker()->getProxy( "ALVideoDevice" );

		//c->callVoid( "unsubscribe", GVM_name );
		xCamProxy->unsubscribe(GVM_name);
		GVM_name= xCamProxy->subscribe(GVM_name, resolution, cSpace,VISON_FPS );
//		GVM_name = c->call<std::string>( "subscribe", GVM_name, resolution,
//										 cSpace,VISON_FPS );
		refreshValues();
		doneSubscribe=true;

		//Calculate Roundtrip time


	}
	catch (AL::ALError& e)
	{
		cout<<e.toString()<<endl;
		throw ALError("KImageExtractor", "Construct ", "Unable to create proxies and subscribe GVM");
	}

	//_com->get_message_queue()->add_publisher(this);
}


/**
 * Fetch a new Image from the hardware, it automatically fixs IplImage and enclosed binary space when needed
 * Use Allocate Image for an initial allocation of an image
 */
#ifdef KROBOT_IS_REMOTE_ON
boost::posix_time::ptime KImageExtractor::fetchImage(IplImage *img)
{
	//cout<<"KImageExtractor::fetchimage():"<<endl;
	boost::posix_time::ptime s=boost::posix_time::microsec_clock::universal_time();
	if (doneSubscribe==false)
	{
		cout<<"KImageExtractor: Warning! fetchImage()  called although GVM Subscription has failed!"<<endl;
		return boost::date_time::max_date_time;
	}
	// Now that you're done with the PREVIOUS  image, you have to release it from the V.I.M.

#ifdef RAW

	results = (c->call<ALValue> ("releaseDirectRawImageRemote", GVM_name));
#else
	results = (c->call<ALValue> ("releaseImageRemote", GVM_name));
#endif

	ALValue results;
#ifdef RAW

	results = (c->call<ALValue> ("getDirectRawImageRemote", GVM_name));
#else
	results = (c->call<ALValue> ("getImageRemote", GVM_name));
#endif
	if (results.getType() != ALValue::TypeArray && results.getSize() != 7)
	{
		throw ALError("KImageExtractor", "ImageRemote", "Invalid image returned.");
	}

	boost::posix_time::time_duration exp=boost::posix_time::microsec(getExp()/2);
	boost::posix_time::ptime stamp=time_t_epoch+(boost::posix_time::microsec((int)results[5])+boost::posix_time::seconds((int) results[4]));
	boost::posix_time::time_duration dur=s-(stamp-exp);//TODO:: rtt - round trip time
	dur-=boost::posix_time::seconds(dur.total_seconds());
	//cout<<boost::posix_time::to_simple_string(dur)<<endl;
	s-=dur;//True Timestamp !! Yeah!


	int width = (int) results[0];
	int height = (int) results[1];
	int nChannels = (int) results[2];
	//int colorSpace = (int) results[3];

	int size =width*height*nChannels;
	//cout<<time<<endl;//-((int) results[4]*1000)-(int)  results[5]<<endl;
	//Change of image data size
	assert(img!=NULL);
	//cout<<img->imageSize<<" "<<size<<endl;

	if (img->imageSize!=size )
	{

		cout<<"cvInitImage"<<endl;
		cvInitImageHeader(img,  cvSize(width,height),IPL_DEPTH_8U, nChannels);
		//img->imageData=NULL;
		cout<<" Done"<<endl;
		//img->imageData=(char*)malloc(img->imageSize);
	}

    img->imageData= (char*) (results[6].GetBinary());
	return s;
};

#else
boost::posix_time::ptime KImageExtractor::fetchImage(IplImage *img)
{
	//cout << "Remote method off" << endl;

	// Now that you're done with the PREVIOUS (local) image, you have to release it from the V.I.M.
#ifdef RAW
	xCamProxy->releaseDirectRawImage(GVM_name);
//	c->call<int> ("releaseDirectRawImage", GVM_name);
	//cout << "releaseDirectRawImage " << endl;
#else
	c->call<int> ("releaseImage", GVM_name);
	//cout << "releaseImage " << endl;
#endif

	ALImage* imageIn = NULL;
	// Now you can get the pointer to the video structure.
#ifdef RAW
	imageIn = (ALImage*)xCamProxy->getDirectRawImageLocal(GVM_name);
//	imageIn = (ALImage*) (c->call<int> ("getDirectRawImageLocal", GVM_name));
	//cout << "GEt getDirectRawImageLocal " << endl;
#else
	imageIn = (ALImage*) (c->call<int> ("getImageLocal", GVM_name));
	//cout << "GEt getImageLocal " << endl;
#endif
	if (!imageIn)
	{
		throw ALError("KImageExtractor", "saveImageLocal", "Invalid image returned.");
	}
	//fLogProxy->info(getName(), imageIn->toString());
	// You can get some image information that you may find useful.
	int width = imageIn->fWidth;
	int height = imageIn->fHeight;
	const int nChannels = imageIn->fNbLayers;
	//		const int colorSpace = imageIn->fColorSpace;
	const long long timeStamp = imageIn->fTimeStamp;
	//		const int seconds = (int) (timeStamp / 1000000LL);
	const int size = width*height*nChannels;
	// Set the buffer we received to our IplImage header.
	//Fetch TimeStamp;

	//Change of image data size
	if ((unsigned)img->imageSize!=size*sizeof(char) )//Cast to remove compiler warning
	{
		free(img->imageData);
		cvInitImageHeader(img,  cvSize(width,height),IPL_DEPTH_8U, nChannels);
		img->imageData=NULL;
		//img->imageData=(char*)malloc(img->imageSize);
	}
    img->imageData=(char*) imageIn->getFrame();


    //apply correction factor to timestamp
//    std::cout<<"img:"<<timeStamp<<endl;
    const long long secsonly=(timeStamp / 1000000LL);
    const long long microsecsonly=timeStamp-(secsonly*1000000LL);
//    cout<<"secsonly:"<<secsonly<<endl;

    return time_t_epoch+boost::posix_time::seconds(secsonly)+boost::posix_time::microseconds(microsecsonly)+boost::posix_time::microsec(lastexpusec/2)+boost::posix_time::milliseconds(33.3333);


};
#endif


IplImage *KImageExtractor::allocateImage()
{
	if (doneSubscribe==false)
	{
		cout<<"KImageExtractor::allocateImage():WTF"<<endl;
		return NULL;

	}
	//Stupid way, but it works!
	int nChannels=getNumLayersInColorSpace(cSpace);
	int width;
	int height;
	getSizeFromResolution(resolution,width,height);
	//cout<<"cvCreteImage"<<endl;
	cout<<width<<" "<<height << " "<< nChannels<<endl;
	return cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,nChannels);
}


float KImageExtractor::calibrateCamera(int sleeptime,int exp)
{
#ifdef WEBOTS
	return 1.0f;
#endif
	std:: vector <std::string> names;
	std:: vector <float>pos;
	int redchroma,bluechroma,gain,e;
	int rchromaL,bchromaL,gainL,eL;
	int rchromaR,bchromaR,gainR,eR;
	//int expvalue=(e*510.8)/33.0;
	MotionHeadMessage hmot;
	hmot.add_parameter(0.0f);
	hmot.add_parameter(0.0f);
	hmot.set_command("setHeadInstant");
#ifdef RAW
	xCamProxy->releaseDirectRawImage(GVM_name);
//	c->call<int> ("releaseDirectRawImage", GVM_name);
	//cout << "releaseDirectRawImage " << endl;
#else
	c->call<int> ("releaseImage", GVM_name);
	//cout << "releaseImage " << endl;
#endif


	cout<<"Calibrate Start:"<<endl;
	try
	{
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
		xCamProxy->setParam( kCameraExposureCorrectionID,0);
//		c->callVoid( "setParam", kCameraExposureCorrectionID,0);
		SleepMs(10);

		//Move head to the left
		hmot.set_parameter(0,1.57);
		hmot.set_parameter(1,0.22);
		_blk->publishSignal(hmot,"motion");
		_blk->publish_all();

		SleepMs(100);
		xCamProxy->setParam(  kCameraAutoGainID, 1);
//		c->callVoid( "setParam", kCameraAutoGainID, 1);
		xCamProxy->setParam(  kCameraAutoExpositionID,1);
//		c->callVoid( "setParam", kCameraAutoExpositionID,1);
		xCamProxy->setParam(  kCameraAutoWhiteBalanceID,1);
//		c->callVoid( "setParam", kCameraAutoWhiteBalanceID,1);

		//Wait for autoconf
		SleepMs(sleeptime);

		//Get Bottom camera settings
		gainL=xCamProxy->getParam( kCameraGainID);
//		gainL=xCamProxy->getParam(kCameraGainID);
		eL=xCamProxy->getParam(kCameraExposureID);

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

		cout<<"Left settings:"<<" "<< gainL<< endl;

		hmot.set_parameter(0,-1.57);
		hmot.set_parameter(1,0.22);
		_blk->publishSignal(hmot,"motion");
        _blk->publish_all();
		//m->callVoid("setAngles",names,pos,0.8);
		SleepMs(100);

//		c->callVoid( "setParam", kCameraAutoGainID, 1);
//		c->callVoid( "setParam", kCameraAutoExpositionID,1);
//		c->callVoid( "setParam", kCameraAutoWhiteBalanceID,1);
		xCamProxy->setParam(  kCameraAutoGainID, 1);
		SleepMs(5);
		xCamProxy->setParam(  kCameraAutoExpositionID,1);
		SleepMs(5);
		xCamProxy->setParam(  kCameraAutoWhiteBalanceID,1);
		//wait for autoconf
		SleepMs(sleeptime);



		//GET BOTTOM CAMERA SETTINGS!!!

		gainR=xCamProxy->getParam(kCameraGainID);
		SleepMs(5);
		eR=xCamProxy->getParam(kCameraExposureID);
		SleepMs(5);

		xCamProxy->setParam( kCameraAutoGainID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoExpositionID,0);
		SleepMs(5);
		//Since now we`ll need again the wb correction, leave it on
		//xCamProxy->setParam( kCameraAutoWhiteBalanceID,0);

		cout<<"Right settings:"<<" "<< gainR<< endl;
		//============================
		// Final Exposure  settings!
		//============================

		e=(eL+eR)/2;


		if ((exp*510.0f)/33.33333333f <e)
		{
			refexpusec=e*MAXEXPUS/510.0;
			e=exp*510.0f/33.33333333f;

		}
		lastexpusec=e*MAXEXPUS/510.0f;
		cout<<"Exposure Scaling:"<<getScale()<<", from"<<refexpusec<<"usec to "<<lastexpusec<<"usec"<<endl;
		//cout<<"Scaling  exposure:"<<(exp*510)/33<<" "<<e<<endl;
		gain=gainL<gainR?gainL:gainR;

		//redchroma=128-((128.0-redchroma)*0.95);
		//bluechroma=128-((128.0-bluechroma)*0.95);
		//gain=((gain&31)/2)|(gain&32);
		cout<<"Final Exposure settings:"<<"Exposure Value:"<<e<<" Gain Value:"<<gain<< endl;


		//Start white balance calibration
		//wait for autoconf
		SleepMs(sleeptime);

		rchromaR=xCamProxy->getParam(kCameraRedChromaID);
		SleepMs(5);
		bchromaR=xCamProxy->getParam(kCameraBlueChromaID);
		SleepMs(5);

//        xCamProxy->setParam( kCameraAutoWhiteBalanceID,0);
		cout<<"Right white balance settings:"<<rchromaR<<" "<<bchromaR<<endl;


		//Move head to the left
		hmot.set_parameter(0,1.57);
		hmot.set_parameter(1,0.22);
		_blk->publishSignal(hmot,"motion");
        _blk->publish_all();

		SleepMs(100);
		//wait for autoconf
		SleepMs(sleeptime);
		rchromaL=xCamProxy->getParam(kCameraRedChromaID);
		SleepMs(5);
		bchromaL=xCamProxy->getParam(kCameraBlueChromaID);

		xCamProxy->setParam( kCameraAutoWhiteBalanceID,0);
		SleepMs(5);
		cout<<"Left white balance settings:"<<rchromaL<<" "<<bchromaL<<endl;
		redchroma=(rchromaL+rchromaR)/2;
		bluechroma=(bchromaL+bchromaR)/2;

		cout<<"Final White Balance Settings"<<redchroma<<" "<<bluechroma;


		xCamProxy->setParam( kCameraSelectID, 0);
		SleepMs(150);
		//SET TOP CAMERA SETTINGS
		xCamProxy->setParam( kCameraAutoGainID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoExpositionID, 0);
		SleepMs(5);
		xCamProxy->setParam( kCameraAutoWhiteBalanceID, 0);
		SleepMs(5);


		xCamProxy->setParam( kCameraBlueChromaID,bluechroma);
		SleepMs(5);
		xCamProxy->setParam( kCameraRedChromaID,redchroma);
		SleepMs(5);
		xCamProxy->setParam( kCameraGainID,gain);
		SleepMs(5);
		xCamProxy->setParam( kCameraExposureID,e);
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


		xCamProxy->setParam( kCameraBlueChromaID,bluechroma);
		SleepMs(5);
		xCamProxy->setParam( kCameraRedChromaID,redchroma);
		SleepMs(5);
		xCamProxy->setParam( kCameraGainID,gain);
		SleepMs(5);
		xCamProxy->setParam( kCameraExposureID,e);
		//c->callVoid( "setParam", kCameraExposureCorrectionID,-6);
		SleepMs(150);
		//Start with bottom cam
		//xCamProxy->setParam( kCameraSelectID, 1);


	}
	catch (AL::ALError &e)
	{
		cout<<"No Autosettings available ... ?!?"<<endl;
		std::cout << e.toString() << std::endl;
		//exit(0);
	}
	lastcam=1;

	cout<<"done"<<endl;

	hmot.set_parameter(0,0);
	hmot.set_parameter(1,-0.1);
	_blk->publishSignal(hmot,"motion");
    _blk->publish_all();
	return getScale();
}
void KImageExtractor::refreshValues()
{
	lastcam=xCamProxy->getParam(kCameraSelectID);
#ifndef WEBOTS

	int a=xCamProxy->getParam(kCameraExposureID);
	lastexpusec= a*MAXEXPUS/510.0f;
#endif

}
int KImageExtractor::getCamera() const
{

	return lastcam;
}

int KImageExtractor::swapCamera()
{
	int old=xCamProxy->getParam(kCameraSelectID);
	old=(old==1)?0:1;
	xCamProxy->setParam( kCameraSelectID,old);
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
	return log(lastexpusec)/log(refexpusec);
#endif
}
