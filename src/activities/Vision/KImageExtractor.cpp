#include "KImageExtractor.h"
#include "messages/motion.pb.cc"
#include "hal/robot/generic_nao/kAlBroker.h"

static const  boost::posix_time::ptime time_t_epoch( boost::gregorian::date(1970,1,1));

using namespace AL;
using namespace std;

KImageExtractor::~KImageExtractor()
{
	try
	{

		c->callVoid( "unsubscribe", GVM_name );
	}
	catch (AL::ALError& e)
	{
		throw ALError("KImageExtractor", "Destruct", "Unable to unsubscribe GVM");
	}
}

KImageExtractor::KImageExtractor() :Publisher("KImageExtractor"),GVM_name(VISION_GVMNAME),resolution(VISION_RESOLUTION),cSpace( VISION_CSPACE)
{
	;
}

void KImageExtractor::Init(Narukom* com)
{
	_com = com;
	doneSubscribe=false;
	try
	{
		c = KAlBroker::Instance().GetBroker()->getProxy( "ALVideoDevice" );
		dcm = KAlBroker::Instance().GetBroker()->getDcmProxy();
		c->callVoid( "unsubscribe", GVM_name );
		GVM_name = c->call<std::string>( "subscribe", GVM_name, resolution,
										 cSpace,VISON_FPS );

		doneSubscribe=true;
		//Calculate Roundtrip time
		ptime start = microsec_clock::local_time();
		c->call<int>( "getParam", kCameraSelectID);
		c->call<int>( "getParam", kCameraSelectID);
		ptime end = microsec_clock::local_time();
		rtt=(end-start).total_microseconds()/2;


	}
	catch (AL::ALError& e)
	{
		cout<<e.toString()<<endl;
		throw ALError("KImageExtractor", "Construct ", "Unable to create proxies and subscribe GVM");
	}

	_com->get_message_queue()->add_publisher(this);
}


/**
 * Fetch a new Image from the hardware, it automatically fixs IplImage and enclosed binary space when needed
 * Use Allocate Image for an initial allocation of an image
 */

boost::posix_time::ptime KImageExtractor::fetchImage(IplImage *img)
{
	//cout<<"KImageExtractor::fetchimage():"<<endl;
	boost::posix_time::ptime s=boost::posix_time::microsec_clock::local_time();
	if (doneSubscribe==false)
	{
		cout<<"KImageExtractor: Warning! fetchImage()  called although GVM Subscription has failed!"<<endl;
		return boost::date_time::max_date_time;
	}

#ifdef REMOTE_ON
	//		cout << "Remote method on" << endl;
	//		sleep(1);
	ALValue results;
#ifdef RAW

	results = (c->call<ALValue> ("getDirectRawImageRemote", GVM_name));
#else
	results = (c->call<ALValue> ("getImageRemote", GVM_name));
#endif
	if (results.getType() != ALValue::TypeArray && results.getSize() != 7)
	{
		throw ALError("KImageExtractor", "saveImageRemote", "Invalid image returned.");
	}
	//const int size = results[6].getSize();
	// You can get some image information that you may find useful.
	//	const int width = (int) results[0];
	//	const int height = (int) results[1];
	//	const int nbLayers = (int) results[2];
	//	const int colorSpace = (int) results[3];
	//const long long timeStamp = ((long long) (int) results[4]) * 1000000LL + ((long long) (int) results[5]);
	//	const int seconds = (int) (timeStamp / 1000000LL);
	// Set the buffer we received to our IplImage header.
	//fIplImageHeader->imageData = (char*) (results[6].GetBinary());
	//cout << "Size" << size << endl;
	//cout<<"dcmtime"<<endl;
	//int time=0 ;
	//time = dcm->getTime(0);
	//cout<<"dcm2"<<endl;
	//alt.getLocalTime();
	//cout<<alt.getHour()<<" "<<alt.getMinute()<<" "<<alt.getSecond()<<" "<<alt.getMs()<<endl;
	//boost::posix_time::ptime e=boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration exp=boost::posix_time::microsec(getExp()/2);
	boost::posix_time::ptime stamp=time_t_epoch+(boost::posix_time::microsec((int)results[5])+boost::posix_time::seconds((int) results[4]));
	boost::posix_time::time_duration dur=s-(stamp-exp);//TODO:: rtt - round trip time
	dur-=boost::posix_time::seconds(dur.total_seconds());
	//cout<<boost::posix_time::to_simple_string(dur)<<endl;
	s-=dur;//True Timestamp !! Yeah!


	int width = (int) results[0];
	int height = (int) results[1];
	int nChannels = (int) results[2];
	int colorSpace = (int) results[3];

	int size =width*height*nChannels;
	//cout<<time<<endl;//-((int) results[4]*1000)-(int)  results[5]<<endl;
	//Change of image data size
	assert(img!=NULL);
	//cout<<img->imageSize<<" "<<size<<endl;

	if (img->imageSize!=size )
	{
		//cout<<img->width<<" "<<img->height<<endl;
		cout<<"KImageExtractor::fetchImage():allocating new imagedata"<<endl;
		//cout<<"Delete old"<<endl;
		//delete img->imageData;
		//img->imageData=NULL;
		cout<<"cvInitImage"<<endl;
		cvInitImageHeader(img,  cvSize(width,height),IPL_DEPTH_8U, nChannels);
		//img->imageData=NULL;
		cout<<" Done"<<endl;
		//img->imageData=(char*)malloc(img->imageSize);
	}


	if (img->imageData != NULL)
	{
		//free( fIplImageHeader->imageData)
		memcpy(img->imageData, (char*) (results[6].GetBinary()), results[6].getSize() * sizeof(unsigned char));
	}
	else
	{
		img->imageData = new char[img->imageSize];
		memcpy(img->imageData, (char*) (results[6].GetBinary()), results[6].getSize() * sizeof(char));
	}
#else
	//cout << "Remote method off" << endl;

	//sleep(1);
	ALImage* imageIn = NULL;
	// Now you can get the pointer to the video structure.
#ifdef RAW
	imageIn = (ALImage*) (c->call<int> ("getDirectRawImageLocal", GVM_name));
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
	boost::posix_time::ptime stamp=time_t_epoch+(boost::posix_time::microsec(timeStamp));
	boost::posix_time::time_duration dur=s-stamp;
	dur-=boost::posix_time::seconds(dur.total_seconds());
	// cout<<boost::posix_time::to_simple_string(dur)<<endl;
	s-=dur;//True Timestamp !! Yeah!


	//Change of image data size
	if (img->imageSize!=size*sizeof(char) )
	{
		free(img->imageData);
		cvInitImageHeader(img,  cvSize(width,height),IPL_DEPTH_8U, nChannels);
		img->imageData=NULL;
		//img->imageData=(char*)malloc(img->imageSize);
	}

	if (img->imageData!=NULL)
{
	//free( fIplImageHeader->imageData);
	memcpy ( img->imageData, (char*) imageIn->getFrame(), size*sizeof(char) );
	}
	else
	{
		img->imageData = new char[size];
		memcpy ( img->imageData, (char*) imageIn->getFrame(), size*sizeof(char) );
	}
	//fIplImageHeader->imageData = (char*) imageIn->getFrame();
	//saveIplImage(fIplImageHeader, name, pImageFormat, seconds);
	// Now that you're done with the (local) image, you have to release it from the V.I.M.
#ifdef RAW
	c->call<int> ("releaseDirectRawImage", GVM_name);
	//cout << "releaseDirectRawImage " << endl;
#else
	c->call<int> ("releaseImage", GVM_name);
	//cout << "releaseImage " << endl;
#endif
#endif
	return s;
};

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
	std:: vector <std::string> names;
	std:: vector <float>pos;
	int redchroma,bluechroma,gain,e;
	int rchromaL,bchromaL,gainL,eL;
	int rchromaR,bchromaR,gainR,eR;
	//int expvalue=(e*510.8)/33.0;
	MotionHeadMessage hmot;
	hmot.add_parameter(0.0f);
	hmot.add_parameter(0.0f);
	hmot.set_topic("motion");
	hmot.set_command("setHead");

	//hmot->set_parameter(0, 0.9f * overshootfix * (cx));
	//hmot->set_parameter(1, -0.9f * overshootfix * (cy));
	//Publisher::publish( hmot);

	float scale=1;
	cout<<"Calibrate Start:"<<endl;

	cout<<"Auto:"<<endl;
	try
	{

		c->callVoid( "setParam", kCameraSelectID, 1);
		c->callVoid( "setParam", kCameraExposureCorrectionID,0);
		SleepMs(1);

		//Move head to the left
		hmot.set_parameter(0,1.57);
		hmot.set_parameter(1,0.22);
		publish(&hmot,"motion");

		SleepMs(100);

		c->callVoid( "setParam", kCameraAutoGainID, 1);
		c->callVoid( "setParam", kCameraAutoExpositionID,1);
		c->callVoid( "setParam", kCameraAutoWhiteBalanceID,1);

		//Wait for autoconf
		SleepMs(sleeptime);

		//Get Bottom camera settings

		gainL=c->call<int>( "getParam", kCameraGainID);
		eL=c->call<int>( "getParam", kCameraExposureID);


		c->callVoid( "setParam", kCameraAutoGainID, 0);
		c->callVoid( "setParam", kCameraAutoExpositionID,0);
		c->callVoid( "setParam", kCameraAutoWhiteBalanceID,0);


		cout<<"Left settings:"<<" "<< gainL<< endl;

		hmot.set_parameter(0,-1.57);
		hmot.set_parameter(1,0.22);
		publish(&hmot,"motion");
		//m->callVoid("setAngles",names,pos,0.8);
		SleepMs(100);

		c->callVoid( "setParam", kCameraAutoGainID, 1);
		c->callVoid( "setParam", kCameraAutoExpositionID,1);
		c->callVoid( "setParam", kCameraAutoWhiteBalanceID,1);
		//wait for autoconf
		SleepMs(sleeptime);



		//GET BOTTOM CAMERA SETTINGS!!!

		gainR=c->call<int>( "getParam", kCameraGainID);
		eR=c->call<int>( "getParam", kCameraExposureID);

		c->callVoid( "setParam", kCameraAutoGainID, 0);
		c->callVoid( "setParam", kCameraAutoExpositionID,0);
		//Since now we`ll need again the wb correction, leave it on
		//c->callVoid( "setParam", kCameraAutoWhiteBalanceID,0);

		cout<<"Right settings:"<<" "<< gainR<< endl;
		//============================
		// Final Exposure  settings!
		//============================

		if (eL<eR)
		{
			gain=gainL;
			e=eL;
		}
		else
		{
			gain=gainR;
			e=eR;
		}


		if ((exp*510)/33 <e)
		{
			scale=log((exp*510)/33)/log(e);
			cout<<"Scaling  exposure:"<<(exp*510)/33<<" "<<e<<endl;
			e=(exp*510)/33;

		}
		cout<<"Scaling  exposure:"<<(exp*510)/33<<" "<<e<<endl;
		gain=gainL<gainR?gainL:gainR;

		//redchroma=128-((128.0-redchroma)*0.95);
		//bluechroma=128-((128.0-bluechroma)*0.95);
		//gain=((gain&31)/2)|(gain&32);
		cout<<"Final Exposure settings:"<< exp<< gain<< endl;


		//Start white balance calibration
		//wait for autoconf
		SleepMs(sleeptime);

		rchromaR=c->call<int>( "getParam", kCameraRedChromaID);
		bchromaR=c->call<int>( "getParam", kCameraBlueChromaID);

//        c->callVoid( "setParam", kCameraAutoWhiteBalanceID,0);
		cout<<"Right white balance settings:"<<rchromaR<<" "<<bchromaR<<endl;


		//Move head to the left
		hmot.set_parameter(0,1.57);
		hmot.set_parameter(1,0.22);
		publish(&hmot,"motion");

		SleepMs(100);
		//wait for autoconf
		SleepMs(sleeptime);
		rchromaL=c->call<int>( "getParam", kCameraRedChromaID);
		bchromaL=c->call<int>( "getParam", kCameraBlueChromaID);

		c->callVoid( "setParam", kCameraAutoWhiteBalanceID,0);
		cout<<"Left white balance settings:"<<rchromaL<<" "<<bchromaL<<endl;
		redchroma=(rchromaL+rchromaR)/2;
		bluechroma=(bchromaL+bchromaR)/2;

		cout<<"Final White Balance Settings"<<redchroma<<" "<<bluechroma;


		c->callVoid( "setParam", kCameraSelectID, 1);
		SleepMs(10);
		//SET BOTTOM CAMERA SETTINGS
		c->callVoid( "setParam", kCameraAutoGainID, 0);
		c->callVoid( "setParam", kCameraAutoExpositionID, 0);
		c->callVoid( "setParam", kCameraAutoWhiteBalanceID, 0);


		c->callVoid( "setParam", kCameraBlueChromaID,bluechroma);
		c->callVoid( "setParam", kCameraRedChromaID,redchroma);
		c->callVoid( "setParam", kCameraGainID,gain);
		c->callVoid( "setParam", kCameraExposureID,e);
		c->callVoid( "setParam", kCameraExposureCorrectionID,-6);


		//c->callVoid( "setParam", kCameraSelectID, 0);
		//SleepMs(10);
		c->callVoid( "setParam", kCameraSelectID, 0);
		SleepMs(100);
		//SET TOP CAMERA SETTINGS

		c->callVoid( "setParam", kCameraAutoGainID, 0);
		c->callVoid( "setParam", kCameraAutoExpositionID, 0);
		c->callVoid( "setParam", kCameraAutoWhiteBalanceID, 0);


		c->callVoid( "setParam", kCameraBlueChromaID,bluechroma);
		c->callVoid( "setParam", kCameraRedChromaID,redchroma);
		c->callVoid( "setParam", kCameraGainID,gain);
		//c->callVoid( "setParam", kCameraAutoExpositionID,2);
		c->callVoid( "setParam", kCameraExposureID,e);
		c->callVoid( "setParam", kCameraExposureCorrectionID,-6);



		SleepMs(100);
		//Start with bottom cam
		c->callVoid( "setParam", kCameraSelectID, 1);


	}
	catch (AL::ALError &e)
	{
		cout<<"No Autosettings available ... webots?"<<endl;
		std::cout << e.toString() << std::endl;
		//exit(0);
	}
	//seg->setLumaScale(0.338914);
	cout<<"done"<<endl;
	//pos.clear();
	//pos.push_back(0);
	//pos.push_back(-0.1);
	//m->callVoid("setAngles",names,pos,0.8);
	hmot.set_parameter(0,0);
	hmot.set_parameter(1,-0.1);
	publish(&hmot,"motion");
	return scale;
}

int KImageExtractor::getCamera()
{
	return c->call<int>( "getParam", kCameraSelectID);
}

int KImageExtractor::swapCamera()
{
	int old=c->call<int>( "getParam", kCameraSelectID);
	old=(old==1)?0:1;
	c->callVoid( "setParam", kCameraSelectID,old);
	return old;
}

float KImageExtractor::getExp()
{
	int a=c->call<int>( "getParam", kCameraExposureID);
	return a*33.0/510.0;
}

