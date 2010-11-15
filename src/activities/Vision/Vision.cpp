#include "Vision.h"
#include "architecture/archConfig.h"
#include <cmath>
#include "sys/stat.h"
#include "tools/logger.h"
#include "tools/XMLConfig.h"
#include "tools/toString.h"
#include "hal/syscall.h"

#include "messages/motion.pb.h"
//#define LONGESTDIST 6.0
//#define COVERDIST 0.03
#define TRACESKIP 5
#define GLOBALTRACESKIP 9
//x COVERDIST=15*0.05m
#define SCANSKIP  5
#define SPARSENESS 4
#define TO_RAD 0.017453293f



#define trydelete(x) {if((x)!=NULL){delete (x);(x)=NULL;}}

#define CvDist(pa,pb) sqrt(  ( ((pa).x-(pb).x )*((pa).x-(pb).x )  )+( ((pa).y-(pb).y )*((pa).y-(pb).y) )  )
//Distance from observation angle
//#define angularDistance(Hr,Ho,yaw,pitch) ( sqrt(1+sin(yaw)*sin(yaw))*((Hr)-(Ho))*kinext.cot(pitch) )
//#define apparentDistance(Hr,Ho,rad) ( sqrt ( ( (((Ho)*(Ho) ) -((Hr)*(Hr)*tan(rad)*tan(rad)))*kinext.cot(rad)*kinext.cot(rad)    )  ))



//using namespace AL;
using namespace std;
//using namespace boost::posix_time;


namespace
{
	ActivityRegistrar<Vision>::Type temp("Vision");
}
struct inttracer_s
{
	int x,y;
	int s,c;
	int e,t,i;
	bool steep;
	//int idx,idy;

	void init(int a,int b)
	{
		x=a;
		y=b;
		e=0;
	};
	void initVelocity(float dx,float dy)
	{
		int idx=abs(dx*2048);
		int idy=abs(dy*2048);
		if (abs(dx)>=abs(dy))
			steep=false;
		else
			steep=true;
		if (!steep)
		{
			s= dx>0?1:-1;
			c=dy>0?1:-1;
			t=idx;
			i=2*idy;
			//Correction


		}
		else
		{
			s= dy>0?1:-1;
			c=dx>0?1:-1;
			t=idy;
			i=2*idx;

		};
		e=0;
	};
	void setScale(int scale)
	{
		s=(s<0?-1:1)*scale;
		c=(c<0?-1:1)*scale;
	};
	void step()
	{
		e+=i;
		if (!steep)
		{
			x+=s;
			if (e>t)
			{
				y+=c;
				e-=t;
			}

		}
		else
		{
			y+=s;
			if (e>t)
			{
				x+=c;
				e-=t;
			}

		}
	}

};

typedef struct inttracer_s tracer_t;


bool Vision::debugmode = false;
TCPSocket * Vision::sock;


//#include "kobserver_goalrecognition.cpp"
bool FileExists(string strFilename)
{
	struct stat stFileInfo;
	bool blnReturn;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat(strFilename.c_str(),&stFileInfo);
	if (intStat == 0)
	{
		// We were able to get the file attributes
		// so the file obviously exists.
		blnReturn = true;
	}
	else
	{
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.
		blnReturn = false;
	}

	return(blnReturn);
}

void saveFrame(IplImage *fIplImageHeader)
{
	static int filenum=0;

	char fname[128];
	do
	{
		sprintf(fname,(ArchConfig::Instance().GetConfigPrefix()+"images/"+std::string("img%03d.yuyv")).c_str(),filenum++);
	}
	while (FileExists(fname));
	ofstream frame(fname,ios_base::binary);
	frame.write(reinterpret_cast<char *>(fIplImageHeader->imageData),fIplImageHeader->width*fIplImageHeader->height*fIplImageHeader->nChannels);
	frame.close();


}


int  Vision::Execute()
{
	//cout<<"Vision Execute"<<endl;
	static bool calibrated = false;
	boost::shared_ptr<const CalibrateCam>  cal= _blk->read_signal<CalibrateCam>("CalibrateCam");
	if (_blk->read_state<CalibrateCam>("CalibrateCam")==NULL)
	{
		CalibrateCam res;
		res.set_status(0);
		_blk->publish_state(res,"vision");
	}
	if (cal==NULL) cal= _blk->read_state<CalibrateCam>("CalibrateCam");
	if (cal!=NULL)
	{
		if ( cal->status()==0)
		{
			//cout<<"Start calibration"<<endl;
			CalibrateCam res;
			Logger::Instance().WriteMsg("Vision", "Start calibration", Logger::Info);

			float scale= ext.calibrateCamera(cal->sleeptime(),cal->exp());
			segbottom->setLumaScale(1/scale);
			segtop->setLumaScale(1/scale);
			Logger::Instance().WriteMsg("Vision", "Calibration Done", Logger::Info);
			//cout<<"Calibration Done!"<<endl;
			calibrated = true;
			res.set_status(1);
			res.set_exposure_comp(scale);
			_blk->publish_state(res,"vision");
			return 0;

		}
	}

	fetchAndProcess();
	//std::cout << " Vision run" << std::endl;
	if (debugmode)
	{
		//cout << "DEbug mode " << endl;
		recv_and_send();
	}

	bool cvHighgui;
	xmlconfig->QueryElement("cvHighgui",cvHighgui);
	if (cvHighgui)
		cvShowSegmented();
	return 0;
}

void Vision::recv_and_send()
{
	bool headerparsed = false;

	int headersize = 10;
	while (!headerparsed)
	{
		int ssize;
		int ss;
		ssize = 0;
		size = 200; //////////#################################################################

		incommingheader.Clear();
		//cout << "Waiting for " << size << " Bytes " << endl;
		if (ssize < size)
		{
			if ((ss = sock->recv(data + ssize, size - ssize)) < 0)
			{
				//cout << "receive error" << endl;
				break;
			}
			ssize += ss;
		}
		//cout << "Arrived " << ssize << " Bytes " << endl;
		headersize = atoi(data);
		if (headersize<1)
		{
			//cout<< "error there is no header < " << headersize << endl;
		}
		if (headersize > size)
		{
			//cout << " oups you must read more bytes in order to read the header " << headersize << endl;
		}

		headerparsed = incommingheader.ParseFromArray(data+10, headersize);

		if (!headerparsed)
		{
			//cout << " Unable to parse i was expecting" << endl;
			continue;
		}

		incommingheader.DiscardUnknownFields();
		//int alreadyparsedbytes = incommingheader.ByteSize()+10;

		//cout << "alreadyparsedbytes " << alreadyparsedbytes << " Bytes" << endl;
	}
	if (headerparsed)
	{
		string command = incommingheader.nextmsgname();
		if (command == "Stop")
		{
			debugmode = false;
			delete sock;
			sock=NULL;
			return;


			//cout << " Stopping Debug ########################" << endl;
		}
		if (command == "seg")
			sendtype = -1; // meand segmented
		if (command == "yuyv")
			sendtype = AL::kYUV422InterlacedColorSpace;

		if ((size = incommingheader.nextmsgbytesize()) > 0) //must read next message
		{
			int ssize;
			int ss;
			ssize = 0;
			while (ssize < size)
			{
				if ((ss = sock->recv(data + ssize, size - ssize)) < 0)
				{
					cout << "receive error" << endl;
					break;
				}
				ssize += ss;
			}
			//cout << "Arrived " << ssize << " Bytes Do something" << endl;
		}
	}
	img.Clear();
	if (sendtype == AL::kYUV422InterlacedColorSpace)
	{
		img.set_imagerawdata(rawImage->imageData,rawImage->imageSize );
		img.set_bytes(rawImage->imageSize);
		//cout << " Raw image  size " << img.bytes() << endl;
		img.set_height(rawImage->height);
		img.set_width(rawImage->width);
		img.set_type(AL::kYUV422InterlacedColorSpace);
	}

	if (sendtype == -1) // segmented image
	{
		char segmended[rawImage->height][rawImage->width];
		for (int i = 0; i < rawImage->width ; i++)
			for (int j = 0; j < rawImage->height ; j++)
				segmended[j][i] = doSeg(i, j);

		vector<CvPoint>::iterator i;
		//For all detected points
		//cout << "locateball" << endl;
		for (i = ballpixels.begin(); i != ballpixels.end(); i++)
		{
			segmended[(*i).y][(*i).x]=red;
		};
		for (i = ygoalpost.begin(); i != ygoalpost.end(); i++)
		{
			segmended[(*i).y][(*i).x]=blue;
		};

		img.set_imagerawdata(segmended,rawImage->width*rawImage->height);
		img.set_bytes(rawImage->width*rawImage->height);
		//cout << " Seg image  size " << img.bytes() << endl;
		img.set_height(rawImage->height);
		img.set_width(rawImage->width);
		img.set_type(-1);
	}
	outgoingheader.set_nextmsgbytesize(img.ByteSize());
	//cout << " Kimage size " << outgoingheader.nextmsgbytesize() << endl ;
	outgoingheader.set_nextmsgname(img.GetTypeName());

	int sendsize;

	int rsize = 0;
	int rs;
	//send a header
	//outgoingheader.set_mysize(sendsize = outgoingheader.ByteSize());
	//while (sendsize != outgoingheader.ByteSize())
	//{
	//	outgoingheader.set_mysize();
	//}
	sendsize = outgoingheader.ByteSize();
	outgoingheader.SerializeToArray(data+10, sendsize);
	//cout << "outgoingheader sendsize " << sendsize << endl;

	memset(data,0,10);
	strncpy(data,_toString(sendsize).c_str(),9);
	sendsize+=10; // add 10 more bytes for the header
	try
	{
		while (rsize < sendsize)
		{
			rs = sock->send(data + rsize, sendsize - rsize);
			rsize += rs;
		}
		cout << "Sended outgoingheader " << rsize << endl;
		//send the image bytes
		sendsize = img.ByteSize();

		for (int j = 0; j < rsize; j++)
		{
			cout << (int) data[j] << " ";
		}
		cout << endl;


		std::string buf;
		img.SerializeToString(&buf);
		sendsize = buf.length();
		rsize = 0;
		cout << "Will send Data " << sendsize << " " << img.GetTypeName() << endl;

		while (rsize < sendsize)
		{
			rs = sock->send((char *) buf.data() + rsize, sendsize - rsize);// UDT::send(recver, data + rsize, sendsize - rsize, 0))) {
			rsize += rs;
		}
		cout << "Sended " << rsize << endl;
	}
	catch (SocketException &e)
	{
		cerr << e.what() << endl;
		cout << "Disconnecting !!!" << endl;
		exit(0);
		debugmode = false;
	}

}

void Vision::fetchAndProcess()
{
	leds.Clear();


	//cout << "fetchImage" << endl;
	//unsigned long startt = SysCall::_GetCurrentTimeInUSec();

	boost::posix_time::ptime stamp = ext.fetchImage(rawImage);
	obs.set_image_timestamp(boost::posix_time::to_iso_string(stamp));
	//unsigned long endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"Fetch image takes:"<<endt<<endl;
	stamp+=boost::posix_time::millisec(config.sensordelay);
	if (ext.getCamera()==1)//bottom cam
	{
		p.cameraPitch=(KMat::transformations::PI*40.0)/180.0;
		//cout<<"CameraPitch:"<<cameraPitch<<endl;
		seg=segbottom;
		//Get Kinematics first!
		std::vector<float> val = kinext.getKinematics("CameraBottom");

		p.cameraX=val[0];
		p.cameraY=val[1];
		p.cameraZ=val[2];//3rd element

	}
	else
	{
		p.cameraPitch=0;
		seg=segtop;
		//Get Kinematics first!
		std::vector<float> val = kinext.getKinematics("CameraTop");
		p.cameraX=val[0];
		p.cameraY=val[1];
		p.cameraZ=val[2];//3rd element
	}
	//cout<<"Attach to Image:"<<endl;
	seg->attachToIplImage(rawImage,type);//Make segmentator aware of a new image
	//saveFrame(rawImage);
	//return;
	//cout<<"Attached"<<endl;
//#ifdef DEBUGVISION
//	cout << "ImageTimestamp:"<< boost::posix_time::to_iso_string(stamp) << endl;
//#endif
	im = _blk->read_data<InertialSensorsMessage>("InertialSensorsMessage", "localhost",&p.time,&stamp);
//#ifdef DEBUGVISION
//	cout<<boost::posix_time::to_iso_extended_string(stamp)<<endl;
//	cout<<boost::posix_time::to_iso_extended_string(p.time)<<endl;
//#endif

	if (im==NULL)//No sensor data!
	{
		Logger::Instance().WriteMsg("Vision", "Warning!!! Vision has no sensor (IS) data!", Logger::Error);
		//cout<<"Warning!!! Vision has no sensor data!"<<endl;
		return;
	}
	if (im->sensordata_size()<7)
	{
		//cout<<"Warning!!! Vision has BAD sensor data!"<<endl;
		Logger::Instance().WriteMsg("Vision", "Warning!!! Vision has BA sensor (IS) data!", Logger::Error);
		return;
	}
	hm = _blk->read_data<HeadJointSensorsMessage>("HeadJointSensorsMessage","localhost",&p.time,&stamp);//,&rtime);
	if (hm==NULL)//No sensor data!
	{
		Logger::Instance().WriteMsg("Vision", "Warning!!! Vision has no sensor (HS) data!", Logger::Error);
		return;
	}
	if (hm->sensordata_size()<2)
	{
		Logger::Instance().WriteMsg("Vision", "Warning!!! Vision has BAD sensor (HS) data!", Logger::Error);
		return;
	}
	//Clear result message
	obs.Clear();
	//TODO: create ct!;
	p.yaw=hm->sensordata(0).sensorvalue();
	p.pitch=hm->sensordata(1).sensorvalue();

	p.Vyaw=hm->sensordata(0).sensorvaluediff();
	p.Vpitch=hm->sensordata(1).sensorvaluediff();

	p.angX=0;//im->sensordata(5).sensorvalue();
	p.angY=0;//im->sensordata(6).sensorvalue();
	p.VangX=im->sensordata(5).sensorvaluediff();//im->sensordata(5).sensortimediff();
	p.VangY=im->sensordata(6).sensorvaluediff();//im->sensordata(6).sensortimediff();

	p.timediff=hm->sensordata(0).sensortimediff();//Get time from headmessage
	//p.time = time_t_epoch;

	//float exptime=ext.getExp();//Compensate for middle of image
	//float imcomp=(exptime*1000.0)/p.timediff;


	float imcomp=((stamp-p.time).total_microseconds()*1000.0)/p.timediff;
//#ifdef DEBUGVISION
	//cout<<boost::posix_time::to_iso_string(stamp)<<endl;
	//cout<<boost::posix_time::to_iso_string(p.time)<<endl;
	//cout<<"imcomp:"<<imcomp<<endl;
//#endif
	//imcomp=imcomp;
	//cout<< p.yaw<<" "<<p.pitch<<" "<<p.Vyaw<<" "<<p.Vpitch<<" "<<imcomp<<" "<<imcomp*p.Vyaw<< " "<<endl;
	//Estimate the values at excactly the timestamp of the image
	p.yaw+=p.Vyaw*imcomp;
	p.pitch+=p.Vpitch*imcomp;
	p.angX+=p.VangX*imcomp;

	p.angY+=p.VangY*imcomp;
	//Now use transformations to use the angX,angY values in the image
	float Dfov;
	xmlconfig->QueryElement("Dfov",Dfov);

	p.focallength=sqrt(rawImage-> width*rawImage-> width+rawImage-> height*rawImage-> height)/(2*tan(Dfov*TO_RAD/2));

	//Logger::Instance().WriteMsg("Vision", _toString("Focal Length ")+_toString(p.focallength), Logger::Error);
	kinext.setPose(p);

	//unsigned long startt,endt;
	//startt=SysCall::_GetCurrentTimeInUSec();
	gridScan(orange);
	//endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"gridscan takes:"<<endt<<endl;




	//startt=SysCall::_GetCurrentTimeInUSec();
	balldata_t b = locateBall(ballpixels);
	//endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"locateball takes:"<<endt<<endl;
	//unsigned long endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"locateball takes:"<<endt<<endl;
	//cout<<b.r<<endl;
	locateGoalPost(ygoalpost,yellow);
#ifdef DEBUGVISION
	cout << "Ballpixelsize:" << ballpixels.size() << endl;
	cout << b.x << " " << b.y << " " << b.cr << endl;
	//KMat::HCoords<float,2> & w=camToRobot(o)
	cout<<"Bearing:"<<b.bearing.mean<<" "<<b.bearing.var<<endl;
	cout<<"Distance:"<<b.distance.mean<<" "<<b.distance.var<<endl;

#endif


	if (b.cr > 0)
	{
		//Fill message and publish
		trckmsg.set_cx(0);
		trckmsg.set_cy(0);
		KMat::HCoords<float,2> im;
		im(0)=b.x;
		im(1)=b.y;
		KMat::HCoords<float,2> c;
		KMat::HCoords<float,3> c3d;
		c=imageToCamera(im);
		c3d=kinext.camera2dToTorso(c);
		//c3d(0)+p.cameraX;c3d(1)+cameraY;
		float pitch,yaw;
		pitch=atan(abs(c3d(2))/sqrt(sqrd(c3d(0))+sqrd(c3d(1))));
		yaw=atan2(c3d(1),c3d(0));
		if (b.distance.mean<0.25)
		{
			trckmsg.set_referenceyaw(yaw/2);
		}
		else
		{
			trckmsg.set_referenceyaw(yaw);
		}
		trckmsg.set_referencepitch(pitch-p.cameraPitch);
		trckmsg.set_radius(b.cr);
		//trckmsg.set_topic("vision");
		BallObject ballpos;
		ballpos.set_dist(b.distance.mean);
		ballpos.set_bearing(b.bearing.mean);
		ballpos.set_ball_diameter(b.ballradius*2);
		obs.mutable_ball()->CopyFrom(ballpos);
		LedValues* l=leds.add_leds();
		l->set_chain("l_eye");
		l->set_color("red");
	}
	else
	{
		trckmsg.set_cx(b.x);
		trckmsg.set_cy(b.y);
		trckmsg.set_radius(-1);
		//trckmsg.set_topic("vision");
		LedValues* l=leds.add_leds();
		l->set_chain("l_eye");
		l->set_color("green");
	}
	_blk->publish_signal(trckmsg,"vision");
	_blk->publish_signal(leds,"communication");

	if (obs.has_ball()||obs.regular_objects_size()>0 || obs.adhoc_objects_size()>0
			||obs.corner_objects_size()>0  || obs.intersection_objects_size()>0
			||obs.line_objects_size()>0)
		_blk->publish_signal(obs,"vision");



}

bool Vision::validpixel(int x,int y)
{
	if ((x >= 0 && x < (rawImage-> width) && y >= 0 && y < (rawImage-> height)))
		return true;
	else
		return false;

}
KSegmentator::colormask_t Vision::doSeg(int x, int y)
{
	if (x >= 0 && x < (rawImage-> width) && y >= 0 && y < (rawImage-> height))
	{
		//return seg->classifyPixel(rawImage, x, y, type);
		return seg->classifyPixel(x,y);
	}
	else
	{
		return 0;
	}

}

Vision::Vision() : xmlconfig(NULL), type(VISION_CSPACE)
{
	debugmode = false;

	int max_bytedata_size = 700000;

	data = new char[max_bytedata_size]; //## TODO  FIX THIS BETTER
}

void Vision::UserInit()
{
	loadXMLConfig(ArchConfig::Instance().GetConfigPrefix()+"/vision.xml");
	if (xmlconfig->IsLoadedSuccessfully()==false)
		Logger::Instance().WriteMsg("Vision", "vision.xml Not Found", Logger::FatalError);
	if (config.cvHighgui==true)
		Logger::Instance().WriteMsg("Vision", "Enable highgui", Logger::Info);


	ext.Init(_blk);
	kinext.Init();
	Logger::Instance().WriteMsg("Vision", "ext.allocateImage()", Logger::Info);
	//cout << "Vision():" ;//<< endl;
	rawImage = ext.allocateImage();
	if(config.cvHighgui)
	{
		segIpl = cvCreateImage(cvSize(rawImage->width, rawImage->height), IPL_DEPTH_8U, 3);
		cvNamedWindow("win1", CV_WINDOW_AUTOSIZE);
	}
	ifstream *conffile = new ifstream((ArchConfig::Instance().GetConfigPrefix()+
									   "colortables/"+config.SegmentationBottom).c_str());
	segbottom = new KSegmentator(*conffile);
	conffile->close();
	delete conffile;

	conffile = new ifstream((ArchConfig::Instance().GetConfigPrefix()+
							 "colortables/"+config.SegmentationTop).c_str());
	segtop = new KSegmentator(*conffile);
	conffile->close();
	delete conffile;


	cout<<"Add Subscriber-publisher"<<endl;
	//_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	//_com->get_message_queue()->add_publisher(this);

	debugmode = false;

	int max_bytedata_size = 1000000;
	sendtype = AL::kYUV422InterlacedColorSpace;
	data = new char[max_bytedata_size]; //## TODO  FIX THIS BETTER

	pthread_create(&acceptthread, NULL, &Vision::StartServer, this);
	pthread_detach( acceptthread);

}

void Vision::loadXMLConfig(std::string fname)
{
	trydelete(xmlconfig);
	xmlconfig = new XMLConfig(fname);//ArchConfig::Instance().GetConfigPrefix()+"/vision.xml");

	xmlconfig->QueryElement("SegmentationBottom",config.SegmentationBottom);
	xmlconfig->QueryElement("SegmentationTop",config.SegmentationTop);
	xmlconfig->QueryElement("cvHighgui",config.cvHighgui);
	xmlconfig->QueryElement("sensordelay",config.sensordelay);
	xmlconfig->QueryElement("scanstep",config.scanstep);
	xmlconfig->QueryElement("skipdistance",config.skipdistance);
	xmlconfig->QueryElement("borderskip",config.borderskip);
	xmlconfig->QueryElement("seedistance",config.seedistance);
	xmlconfig->QueryElement("obstacledistance",config.obstacledistance);
	xmlconfig->QueryElement("balltolerance",config.balltolerance);
    xmlconfig->QueryElement("ballsize",config.ballsize);

    xmlconfig->QueryElement("goalheight",config.goalheight);
    xmlconfig->QueryElement("goaldist",config.goaldist);
    xmlconfig->QueryElement("goaldiam",config.goaldiam);
    xmlconfig->QueryElement("goalslopetolerance",config.goalslopetolerance);


}
void Vision::gridScan(const KSegmentator::colormask_t color)
{
	//Horizontal + Vertical Scan
	ballpixels.clear();
	ygoalpost.clear();
	bgoalpost.clear();
	obstacles.clear();


	KMat::HCoords<float,2> im;
	KMat::HCoords<float,2> c;
	KMat::HCoords<float,3> c3d;
	static int startx=0;
	int  step;
	//int points[rawImage->width];

	KSegmentator::colormask_t tempcolor;
	//int ballpixel = -1;
	unsigned int cntother=0;

	startx+=SPARSENESS-1;//Skip min num of pixels that indicate the sparseness of new colored pixels
	startx=startx%(config.scanstep>>1);
	//float stepy=-1,stepx=-tan(-kinext.getRoll());//TODO :: is this correct?
	tracer_t gtrc;
	//	Vup.y=-cos(-kinext.getRoll());
	//Vup.x=-sin(-kinext.getRoll());
	gtrc.initVelocity(-sin(-kinext.getRoll()),-cos(-kinext.getRoll()));
	step=config.scanstep;
	gtrc.setScale(step);
	//cout<<"Camera Roll:"<<-kinext.getRoll()<<endl;

	//cout<<"loop"<<endl;
	for (int i = config.borderskip+startx; i < rawImage->width-config.borderskip-1; i = i + (config.scanstep>>1))
	{
		//cout<<"wtf"<<endl;
		//Thru Horizon Possibly someday
		//cout<<"outer"<<endl;
		//cntwhitegreenpixels = 0;
		//cntwhitegreenorangepixels=0;
		cntother=0;
		bool ballfound=false;
		bool yfound=false;
		bool bfound=false;
		// ballpixel = -1;
		//int ci,cj;
		gtrc.init(i,rawImage->height - config.borderskip-1-startx);

		KMat::HCoords<int,2> lastpoint;
		lastpoint(0)=gtrc.x;
		lastpoint(1)=gtrc.y;//Copy first point
		//cout<<"================================================"<<endl;
		while (validpixel(gtrc.x,gtrc.y))
		{
			//cout<<"doseg"<<endl;
			tempcolor = doSeg(gtrc.x,gtrc.y);
			//cout<<"doseg:"<<(int)tempcolor<<endl;
			if (tempcolor == green)//
			{
				//cntwhitegreenpixels++;
				//cntwhitegreenorangepixels++;
				cntother=0;
				lastpoint(0)=gtrc.x;
				lastpoint(1)=gtrc.y;
				ballfound=false;
				yfound=false;
				bfound=false;
			}
			else if(tempcolor==white)
			{
				cntother++;//hmm... white? valid but test for distance
				lastpoint(0)=gtrc.x;
				lastpoint(1)=gtrc.y;
				ballfound=false;
				yfound=false;
				bfound=false;
			}
			else if (tempcolor==orange)
			{
				//cntwhitegreenorangepixels++;
				cntother=0;
				yfound=false;
				bfound=false;

			}
			else
			{
				//ballskip=0;
				cntother++;
				ballfound=false;
			}
			if (cntother>SCANSKIP)//No continuity, break
			{
				cntother=0;
				im(0)=lastpoint(0);
				im(1)=lastpoint(1);
				c=imageToCamera(im);

				c3d=kinext.camera2dToGround(c);
				if(c3d(2)>=0)//Looking up :)
					break;
				c3d=kinext.camera2dToGroundProjection(c,0);
				float d1=sqrt(sqrd(c3d(0))+sqrd(c3d(1)));

				if (d1>=sqrd(config.seedistance))//Looking way too far
				{
					//cout<<"Seedistance reached"<<endl;
					//c3d.prettyPrint();
					break;
				}
				im(0)=gtrc.x;
				im(1)=gtrc.y;
				c=imageToCamera(im);
				c3d=kinext.camera2dToGroundProjection(c,0);
				float d2=sqrt(sqrd(c3d(0))+sqrd(c3d(1)));

				if (abs(d1-d2)>config.skipdistance||d1>=sqrd(config.seedistance))
				{
					//cout<<"break"<<endl;
					CvPoint tmpPoint;
					tmpPoint.x = gtrc.x;
					tmpPoint.y = gtrc.y;
					obstacles.push_back(tmpPoint);
					break;
				}
			}
			if (tempcolor == orange &&ballfound==false)
			{
				CvPoint tmpPoint;
				tmpPoint.x = gtrc.x;
				tmpPoint.y = gtrc.y;
				ballpixels.push_back(tmpPoint);
				//cntwhitegreenpixels=0;
				ballfound=true;
				//continue;
				//ballpixel = j;
			}
			if(tempcolor==yellow&&yfound==false)
			{
				CvPoint tmpPoint;
				tmpPoint.x = gtrc.x;
				tmpPoint.y = gtrc.y;
				//ballpixels.push_back(tmpPoint);
				ygoalpost.push_back(tmpPoint);
				yfound=true;
			}

			//Find next pixel
			gtrc.step();
			step--;
			if (step<SPARSENESS-1) step=SPARSENESS-1;
			gtrc.setScale(step);
			//stepx=2;


		}


	}
	//publishObstacles(obstacles);
	//unsigned long startt = SysCall::_GetCurrentTimeInUSec();
/* else {
		memory->insertData("kouretes/Ball/found", .0f); // change
	}*/
}


void Vision::publishObstacles(std::vector<CvPoint> points)
{
	static int period=0;
	period++;
	if (period>5)
		period=0;
	vector<CvPoint>::iterator i;//candidate iterator
	VisionObstacleMessage result;
	if (!(result.obstacles_size()>2&&period==0))
		return;



	for (i = points.begin(); i != points.end(); i++)
	{

		KMat::HCoords<float,2> point;
		point(0)=(*i).x;
		point(1)=(*i).y;

		/*
				if(d>=0.3&&d<=0.9)
				{
		            float bearing=w(1)/TO_RAD+180;
		            if(w(1)>360)
		                w(1)=w(1)-360;
		            w(2)=w(2)*100;
		            ObstacleMessage *o=result.add_obstacles();
		            o->set_direction(w(1));
		            o->set_distance(w(2));




				}
		        delete &w;
		*/


	}
	_blk->publish_signal(result,"obstacle");

}

KMat::HCoords<float,2>  Vision::imageToCamera( const KMat::HCoords<float,2>  & imagep)
{

	KMat::HCoords<float,2> res;
	res(0)=imagep(0)-rawImage->width/2.0+0.5;
	res(1)=-(imagep(1)-rawImage->height/2.0 +0.5);

	return res;
}

KMat::HCoords<int,2>  Vision::cameraToImage( const KMat::HCoords<float,2>  & c)
{

	KMat::HCoords<int,2> res;

	res(0)=(int) (c(0) +rawImage->width/2.0-0.5);
	res(1)=(int) (-c(1) +rawImage->height/2.0 -0.5);


	return res;
}


//Input:  distance bearing
KMat::HCoords<float,2>  Vision::camToRobot(KMat::HCoords<float ,2> & t)
{
	KMat::HCoords<float,2>  res ;
	float a=cos(t(1))*t(0)+p.cameraX;
	float b=sin(t(1))*t(0)+p.cameraY;
	res(0)=sqrt((a)*(a)+(b)*(b));
	res(1)=atan2(b,a);
	return res;
}




/**
 * Hard Decision: Is it good enough for a ball?
 */

bool Vision::calculateValidBall(balldata_t ball, KSegmentator::colormask_t c)
{
	unsigned int ttl = 0, gd = 0;
	float innerrad = ball.cr * 0.707;
	float ratio;
	//Inner circle
	for (int i = ball.x - innerrad+1; i <= ball.x + innerrad-1; i+=SPARSENESS)
		for (int j =ball. y - innerrad+1; j <= ball.y ; j++)
		{
			if (!validpixel(i,j))
				continue;
			if (doSeg(i, j) == c)
				gd+=3;
			ttl+=3;
		}

	for (int i = ball.x+1; i <= ball.x + innerrad-1; i++)
		for (int j =ball. y+1 ; j <= ball.y + innerrad-1; j+=SPARSENESS)
		{
			if (!validpixel(i,j))
				continue;
			if (doSeg(i, j) == c)
				gd++;
			ttl++;
		}

	ratio = ((float) gd+1) / (ttl+1);
	//<<ratio<<endl;
	//cout<<"Validratio:"<<ratio<<endl;
	if (ratio < 0.75)
		return false;
	return true;

}
bool Vision::calculateValidGoalPost(goalpostdata_t goal, KSegmentator::colormask_t c)
{
	/*
	int width =goal.lr.x-goal.ll.x+1;
	unsigned int ttl = 0, gd = 0;
	float ratio;
	for (int i=goal.ll.x; i<=goal.lr.x; i++)
	{
		for (int j=goal.bottom.y+width; j>goal.bottom.y; j--)
		{
			if (!validpixel(i,j))
				continue;
			if (doSeg(i, j) == green)
				gd++;
			ttl++;
		}
	}
	ratio=gd/ttl;
	if (ratio<0.5)
		return false;


	if (goal.height>0)
	{
		goal.conf=1;

	}
	else
		goal.conf=0;
		*/
}

bool cmpgoalpostdata_t (Vision::goalpostdata_t a,  Vision::goalpostdata_t b)
{
	return a.distance.mean < b.distance.mean;
}

Vision::goalpostdata_t Vision::locateGoalPost(vector<CvPoint> cand, KSegmentator::colormask_t c)
{

	CvPoint2D32f Vup,Vdn,Vlt,Vrt;//Vertical velocity
	Vup.y=-cos(-kinext.getRoll());
	Vup.x=-sin(-kinext.getRoll());

	Vdn.x=-Vup.x;
	Vdn.y=-Vup.y;

	Vlt.x=Vup.y;
	Vlt.y=-Vup.x;

	Vrt.x=-Vup.y;
	Vrt.y=Vup.x;


	vector<goalpostdata_t> history;

	vector<CvPoint>::iterator i;//candidate iterator
	for (i = cand.begin(); i != cand.end(); i++)
	{

		vector<goalpostdata_t>::iterator hi = history.begin();
		while (hi != history.end() && i != cand.end())
		{

			if((*hi).contains(*i))
			{
				cout<<"s";
				i++;//Skip pixel
				hi = history.begin();

			}
			else
				hi++;
		}

		if (i == cand.end())
			break;

		if (!validpixel((*i).x,(*i).y))
			continue;
		GoalPostdata newpost;
		traceResult trcrs;

		//Find an initial width
		trcrs=   traceline((*i), cvPoint(-1, 0), c);
		CvPoint pleft=trcrs.p;
		trcrs= traceline((*i), cvPoint(+1, 0), c);

		CvPoint pright=trcrs.p;

		CvPoint m;m.x=(pleft.x+pright.x)>>1;m.y=(pleft.y+pright.y)>>1;//middle point, avg x,y
		//Up=============
		trcrs= traceline(m,Vdn,c);
		newpost.bot=trcrs.p;
		newpost.haveBot=trcrs.smartsuccess;

		trcrs= traceline(newpost.bot,cvPoint(-1,-1),c);
		//trcrs= traceline(trcrs.p,Vdn,c);
		newpost.ll=trcrs.p;

		trcrs= traceline(newpost.bot,cvPoint(1,-1),c);
		//trcrs= traceline(trcrs.p,Vdn,c);
		newpost.lr=trcrs.p;
		//Up=============

		trcrs= traceline(m,Vup,c);
		newpost.top=trcrs.p;
		newpost.haveTop=trcrs.smartsuccess;

		trcrs= traceline(newpost.top,cvPoint(-1,1),c);
		//trcrs= traceline(trcrs.p,Vup,c);
		newpost.tl=trcrs.p;

		trcrs= traceline(newpost.top,cvPoint(1,1),c);
		//trcrs= traceline(trcrs.p,Vup,c);
		newpost.tr=trcrs.p;

		//cout<<"Smarts:"<<newpost.haveBot<< " "<<newpost.haveTop<<endl;
		if(newpost.tr.x-newpost.tl.x<SCANSKIP || newpost.lr.x-newpost.ll.x<SCANSKIP ||
			newpost.ll.y-newpost.tl.y<SCANSKIP || newpost.lr.y-newpost.tr.y<SCANSKIP)
		{
			cout <<"Goal size test failed"<<endl;
			continue;
		}

		/*
		CvPoint2D32f avg;
		avg.x=(newpost.tl.x-newpost.ll.x + newpost.tr.x-newpost.lr.x)/2;
		avg.y=(newpost.tl.y-newpost.ll.y + newpost.tr.y-newpost.lr.y)/2;
		avg.x/=sqrt(sqrd(avg.x)+sqrd(avg.y));
		avg.y/=sqrt(sqrd(avg.x)+sqrd(avg.y));
		//cout<<(avg.y/Vup.y)<<" "<<(Vup.x/avg.x)<<endl;
		float th1=atan(avg.y/avg.x);
		float th2=atan(Vup.y/Vup.x);
		float dth=abs(th1-th2);
		if(dth>1.5)dth-=1.5;
		if(abs(dth/th2)>config.goalslopetolerance)
		{
			cout<<"Slope test failed"<<endl;
			continue;
		}*/
		if(newpost.haveBot)
		{
			KMat::HCoords<float,2> c,i;
			KMat::HCoords<float,3> c3d;
			i(0)=newpost.bot.x;
			i(1)=newpost.bot.y;
			c=imageToCamera(i);
			c3d=kinext.camera2dTo3d(c);
			if(c3d(2)>=0)
			{
				newpost.haveBot=false;
			}
			else
			{
				measurement *a=kinext.projectionDistance(c,0);
				newpost.distBot=a[0];
				newpost.distBot.mean+=config.goaldiam/2.0;
				newpost.bBot=a[1];
				delete[] a;
			}
		}
		if(newpost.haveTop)
		{
			KMat::HCoords<float,2> c,i;
			KMat::HCoords<float,3> c3d;
			i(0)=newpost.top.x;
			i(1)=newpost.top.y;
			c=imageToCamera(i);
			c3d=kinext.camera2dTo3d(c);
			if(c3d(2)<=0)
			{
				newpost.haveTop=false;
			}
			else
			{
				measurement *a=kinext.projectionDistance(c,config.goalheight);
				newpost.distTop=a[0];
				newpost.distTop.mean+=config.goaldiam/2.0;
				newpost.bTop=a[1];
				delete[] a;

			}

		}

		if(newpost.haveTop&&newpost.haveBot)
		{
			KMat::HCoords<float,2> c1,c2,i1,i2;
			i1(0)=newpost.bot.x;
			i1(1)=newpost.bot.y;
			i2(0)=newpost.top.x;
			i2(1)=newpost.top.y;
			c1=imageToCamera(i1);
			c2=imageToCamera(i2);
			float hAng=kinext.vectorAngle(c1,c2);

			//Analytical solution: we know the angle of a triangle ,the oposite side and the parts that it is split by
			//The corresponding triangle height. What is the height it self?

			float t=tan(KMat::transformations::PI-hAng);
			if(t>=0)
			{
				cout<<"Goal Post t failed"<<endl;
				continue;
			}
			//Single solution of a  trionym
			float g=config.goalheight;
			float h=p.cameraZ;


			//cout<<g<<" "<<h<<" "<<t<<endl;
			newpost.distHeight.mean= - (g+sqrt( 4*t*t*h*(g-h)+g*g  ))/(2*t)+config.goaldiam/2;

			if(  !(newpost.distBot.mean<newpost.distHeight.mean&&newpost.distTop.mean>newpost.distHeight.mean) &&
				 !(newpost.distBot.mean>newpost.distHeight.mean&&newpost.distTop.mean<newpost.distHeight.mean)
				)
			{
				cout<<"Distance logic test failed!"<<endl;
				continue;
			}
		}
		else
			newpost.distHeight.mean=-1;


		if(newpost.haveBot&&newpost.haveTop)
		{

			newpost.distance.mean=newpost.distHeight.mean;
			newpost.distance.var=(newpost.distTop.var*newpost.distBot.var)/(newpost.distTop.var+newpost.distBot.var);
			newpost.bearing.mean=(newpost.bBot.mean*newpost.bTop.var+newpost.bTop.mean*newpost.bBot.var)/(newpost.bBot.var+newpost.bTop.var);
			newpost.bearing.var=newpost.bTop.var*newpost.bBot.var/(newpost.bBot.var+newpost.bTop.var);
			cout<<"bwm:"<<newpost.bBot.mean<<" "<<newpost.bTop.mean<<" "<< newpost.bearing.mean<<endl;

		}
		else if(newpost.haveTop)
			newpost.distance=newpost.distTop;
		else if(newpost.haveBot)
			newpost.distance=newpost.distBot;
		else
			continue;
		cout<<"Haves:"<<newpost.haveBot<<" "<<newpost.haveTop<<endl;
		cout<<"Distance"<<newpost.distance.mean<<" "<<newpost.distance.var<<endl;
		cout<<"Bearing"<<newpost.bearing.mean<<" "<<newpost.bearing.var<<endl;







		//if(!validpixel(pleft.x,pleft.y)||!validpixel(pright.x,pright.y))//WHAT THE HELL
		//	continue;



		//if(calculateValidGoalPost(newpost,c))
		history.push_back(newpost);

	}
	std::sort (history.begin(), history.end(), cmpgoalpostdata_t);
	//if(history.size()==2)
	//{
	   // NamedObject *gp=obs->add_regular_objects();
		//if(c==yellow)
		 //   gp->set_

	//}

}

Vision::balldata_t Vision::locateBall(vector<CvPoint> cand)
{
	//Skip first/last row/col
	KMat::HCoords<float,2> point;
	vector<balldata_t> history;
	vector<balldata_t> banlist;


	vector<CvPoint>::iterator i;
	//For all detected points
	//cout << "locateball" << endl;
	for (i = cand.begin(); i != cand.end(); i++)
	{
		vector<CvPoint> points;
		vector<balldata_t>::iterator bd = history.begin();
		while (bd != history.end() )
		{
			if (CvDist(*bd,*i) <= (*bd).cr+1)
			{
				//cout<<"skip"<<endl;
				i++;//Skip pixels
				if (i == cand.end())
					break;
				bd = history.begin();
			}
			else
				bd++;
		}
		if (i == cand.end())
			break;

		bd= banlist.begin();
		while (bd != banlist.end() )
		{
			if (CvDist(*bd,*i) <= (*bd).cr+1)
			{
				//cout<<"ban list"<<endl;
				i++;//Skip pixels
				if (i == cand.end())
					break;
				bd = banlist.begin();
			}
			else
				bd++;
		}
		if (i == cand.end())
			break;
		if (!validpixel((*i).x,(*i).y))
			continue;
		//cout<<"Ball Tracing:";
		traceResult trcrs;

		trcrs = traceline((*i), cvPoint(0, 1), orange);//Stupid
		CvPoint b=trcrs.p;
		trcrs = traceline((*i), cvPoint(0, -1), orange);//smart
		if(trcrs.smartsuccess==false)
			continue;//No top
		CvPoint t=trcrs.p;
		CvPoint m;
		m.x=(*i).x;
		m.y=(b.y+t.y)/2;

		trcrs = traceline(m, cvPoint(1, 0), orange);//Stupid
		CvPoint r=trcrs.p;

		trcrs = traceline(m, cvPoint(-1, 0), orange);//Stupid
		CvPoint l=trcrs.p;

		m.x=(l.x+r.x)/2;

		//Repeat up down :)
		trcrs = traceline(m, cvPoint(0, 1), orange);//Stupid
		b=trcrs.p;

		trcrs = traceline(m, cvPoint(0, -1), orange);//Smart

		if (trcrs.smartsuccess==false)
			continue;
		t=trcrs.p;
        m.y=(b.y+t.y)/2;

		trcrs = traceline(m, cvPoint(1, -1), orange);//Prefer  r
		if (trcrs.smartsuccess==false)
			continue;
		r=trcrs.p;

		//cout<<"R:"<<r.x<<","<<r.y<<endl;
		trcrs = traceline(m, cvPoint(-1, -1), orange);//Prefer top l
		if (trcrs.smartsuccess==false)
			continue;
		l=trcrs.p;

		CvPoint2D32f center=centerOfCircle(l,t,r);
		if(center.x==-1)
			continue;
		float radius = CvDist(center,t);
		//cout << "Wtf" << endl;
		balldata_t newdata;
		newdata.x = center.x;
		newdata.y = center.y;
		newdata.cr = radius;
		//Looks like a ball?
		point(0)=center.x;
		point(1)=center.y;

		//Estimate Radius:
		KMat::HCoords<float,3> c3d;
		KMat::HCoords<float,2> c1,c2;
		KMat::HCoords<float,2> im1,im2;
		im1(0)=center.x;
		im1(1)=center.y;
		im2(0)=center.x+(radius)*0.707;
		im2(1)=center.y+(radius)*0.707;
		//im1.prettyPrint();
		//im2.prettyPrint();
		c1=imageToCamera(im1);
		c2=imageToCamera(im2);
		float tantths=kinext.vectorAngle(c1,c2);
		c3d=kinext.camera2dToGround(c1);//Transform center vector  to ground coordinates
		float par=tantths*tantths*( (sqrd(c3d(0))+sqrd(c3d(1)))/sqrd(c3d(2)))+tantths*tantths;
		if (par>1)//Something is terribly wrong!
		{
			Logger::Instance().WriteMsg("Vision", "Ball size estimation went boggus!", Logger::Error);
			continue;
		}
		float rest=p.cameraZ*(par-sqrt(par))/(par-1);
		//cout<<"rest:"<<rest<<endl;
		newdata.ballradius=rest;
		measurement d1=kinext.angularDistance(c1,c2,rest);
		float w=d1.mean;
		w=w<1?1:w;
		rest= rest/w + (config.ballsize/2.0*(w-1))/w;
		if (abs( (rest*2-config.ballsize)/config.ballsize)>config.balltolerance)//Wrong diameter ball
		{
			Logger::Instance().WriteMsg("Vision", "Ball size estimation check failed", Logger::Info);
			banlist.push_back(newdata);
			continue;
		}
		if (!calculateValidBall(newdata,(KSegmentator::colormask_t) orange))
			continue;
		measurement* a=kinext.projectionDistance(c1,rest);
		measurement d2=a[0];
		measurement bearing=a[1];
		//cout<<"proj bearing:"<<bearing.mean<<" "<<bearing.var<<endl;
		delete[] a;
		measurement distance;

		distance.mean=(d1.mean*d2.var+d2.mean*d1.var)/(d1.var+d2.var);
		distance.var=d1.var*d2.var/(d1.var+d2.var);
		KMat::HCoords<float,2> polar;
		//cout<<"ttldist:"<<distance.mean<<endl;
		polar(0)=distance.mean;
		polar(1)=bearing.mean;
		//Convert to robot space
		KMat::HCoords<float,2> robotpolar=camToRobot(polar);
		distance.mean=polar(0);
		bearing.mean=polar(1);
		newdata.distance=distance;
		newdata.bearing=bearing;
		history.push_back(newdata);

	}
	vector<balldata_t>::iterator bd = history.begin();
	balldata_t best;
	best.cr = 0;
	best.distance.var =10000;
	//debugcout << "BEST found" << endl;
	while (bd != history.end())
	{
		//cout << best.x << " " << best.y << " "<<best.d<< endl;
		//cout << (*bd).x << " " << (*bd).y << " "<<(*bd).d<< endl;
		//cout<<(*bd).d<<endl;
		if ((*bd).distance.var*abs((*bd).ballradius*2.0-config.ballsize)/config.ballsize < best.distance.var*abs(best.ballradius*2.0-config.ballsize)/config.ballsize )
			best = *bd;
		//cout << best.x << " " << best.y << " "<<best.d<< endl;
		bd++;
	}

#ifdef DEBUGVISION
	//KMat::HCoords<float,2> & w=camToRobot(o)
	Logger::Instance().WriteMsg("Vision", "Distance:"+_toString(best.distance.mean), Logger::Info);
	cout<<"Bearing:"<<best.bearing.mean<<" "<<best.bearing.var<<endl;
	cout<<"Distance:"<<best.distance.mean<<" "<<best.distance.var<<endl;

#endif
	return best;


}
CvPoint2D32f Vision::centerOfCircle(CvPoint l, CvPoint m, CvPoint r)
{
	CvPoint2D32f center;
	center.x=-1;
	if (m.x==l.x||m.x==r.x)//Some points form  a vertical line, swap and retry
	{
		CvPoint tmp;
		tmp=m;
		m=l;
		l=m;
	}

	if (m.x==l.x||m.x==r.x)//3 Points are collinear :)
		return center;
	float ma=((float)(m.y-l.y))/((float)(m.x-l.x));
	float mb=((float)(r.y-m.y))/((float)(r.x-m.x));
	if (mb==ma||ma==0)
		return center;
	center.x= ( ma*mb*((float)(l.y-r.y))+mb*((float)(l.x+m.x))-ma*((float)(m.x+r.x)) )/ (2.0*(mb-ma));
	center.y=-(center.x -((float)(l.x+m.x))/2)/ma+((float)(l.y+m.y))/2.0;

	return center;
}

Vision::traceResult Vision::traceline(CvPoint start, CvPoint vel, KSegmentator::colormask_t c)
{
	int skipcount = 0;
	int globalcount = 0;
	CvPoint curr = start;
	CvPoint latestValid = start;
	/////cout << "traceline:"<<start.x<<" "<<start.y<<endl;
	while (validpixel(curr.x,curr.y))
	{
		if (doSeg(curr.x, curr.y) != c)
		{
			skipcount++;
			globalcount++;
		}
		else
		{
			latestValid = curr;
			skipcount = 0;
		};

		if (skipcount > TRACESKIP || globalcount > GLOBALTRACESKIP)
		{
			//curr=start;
			break;
		}
		curr.x += vel.x;
		curr.y += vel.y;

	}
	traceResult r;
	if (!validpixel(curr.x,curr.y))
		r.smartsuccess=false;
	else
		r.smartsuccess=true;

	//cout<<"ret"<<latestValid.x<<" "<<latestValid.y<<endl;

	r.p=latestValid;

	return r;
}


Vision::traceResult Vision::traceline(CvPoint start, CvPoint2D32f vel, KSegmentator::colormask_t c)
{
	int skipcount = 0;
	int globalcount = 0;
	//CvPoint curr = start;
	CvPoint latestValid = start;
	tracer_t curr;
	curr.init(start.x,start.y);
	curr.initVelocity(vel.x,vel.y);
	/////cout << "traceline:"<<start.x<<" "<<start.y<<endl;
	while (validpixel(curr.x,curr.y))
	{
		if (doSeg(curr.x, curr.y) != c)
		{
			skipcount++;
			globalcount++;
		}
		else
		{
			latestValid.x = curr.x;
			latestValid.y = curr.y;
			skipcount = 0;
		};

		if (skipcount > TRACESKIP || globalcount > GLOBALTRACESKIP)
		{
			//curr=start;
			break;
		}
		curr.step();

	}
	traceResult r;
	if (!validpixel(curr.x,curr.y))
		r.smartsuccess=false;
	else
		r.smartsuccess=true;

	//cout<<"ret"<<latestValid.x<<" "<<latestValid.y<<endl;

	r.p=latestValid;

	return r;
}


void Vision::cvShowSegmented()
{

	char * segImage = segIpl->imageData;
	for (int i = 2; i < rawImage->width - 2; i++)
	{
		for (int j = 2; j < rawImage->height - 2; j++)
		{

			//*(imgA+i*width+j)=seg->classifyPixel(yuv);


			//segImage[j][i] = seg-/>classifyPixel(yuv);//classifyPixel(fIplImageHeader, i, j, type);
			//cout<<"Test3"<<endl;
			KSegmentator::colormask_t k = doSeg(i, j);//sImage[j][i];//fIplImageHeader,i,j,kYUVColorSpace
			//				if (k == 5)
			// cout << "Pixel at i " << i << " j " << j << " value " << (int) k << endl;
			int width = rawImage->width;
			switch (k)
			{

				case red://RED
					segImage[j * 3 * width + i * 3 + 2] = 255;
					segImage[j * 3 * width + i * 3 + 1] = 0;
					segImage[j * 3 * width + i * 3] = 0;
					break;
				case blue://BlUE
					segImage[j * 3 * width + i * 3 + 2] = 0;
					segImage[j * 3 * width + i * 3 + 1] = 0;
					segImage[j * 3 * width + i * 3] = 255;
					break;
				case green://GREEN
					segImage[j * 3 * width + i * 3 + 2] = 60;
					segImage[j * 3 * width + i * 3 + 1] = 120;
					segImage[j * 3 * width + i * 3] = 60;
					break;
				case skyblue://SkyBlue
					segImage[j * 3 * width + i * 3 + 2] = 0;
					segImage[j * 3 * width + i * 3 + 1] = 107;
					segImage[j * 3 * width + i * 3] = 228;
					break;
				case yellow://Yellow
					segImage[j * 3 * width + i * 3 + 2] = 255;
					segImage[j * 3 * width + i * 3 + 1] = 255;
					segImage[j * 3 * width + i * 3] = 0;
					break;
				case orange://Orange
					segImage[j * 3 * width + i * 3 + 2] = 255;
					segImage[j * 3 * width + i * 3 + 1] = 180;
					segImage[j * 3 * width + i * 3] = 0;
					break;
				case white://
					segImage[j * 3 * width + i * 3 + 2] = 255;
					segImage[j * 3 * width + i * 3 + 1] = 255;
					segImage[j * 3 * width + i * 3] = 255;
					break;
				default:
					segImage[j * 3 * width + i * 3 + 2] = 0;
					segImage[j * 3 * width + i * 3 + 1] = 0;
					segImage[j * 3 * width + i * 3] = 0;
					break;
			}
			//cout<< hsl[0]<<","<<hsl[1]<<","<<hsl[2]<<endl<<endl;

		}
		//cout<<endl;
	}
	cvShowImage("win1", segIpl);


	int k=cvWaitKey(10);
	if (k=='s')
	{
		saveFrame(rawImage);
		Logger::Instance().WriteMsg("Vision", "Save frame", Logger::Error);
	}
	if (k=='c')
	{
		Logger::Instance().WriteMsg("Vision", "Change Cam", Logger::Info);
		ext.swapCamera();

	}
}


void * Vision::StartServer(void * astring)
{

	unsigned short port = 9000;

	TCPServerSocket servSock(port);

	cout << "Vision server is ready at port: " << port << endl;

	while (true)
	{
		if (!debugmode)
		{
			if ((sock = servSock.accept()) < 0)
			{
				cout << " REturned null";
				return NULL;
			}
			cout << "Handling client ";
			try
			{
				cout << sock->getForeignAddress() << ":";
			}
			catch (SocketException e)
			{
				cerr << "Unable to get foreign address" << endl;
			}
			try
			{
				cout << sock->getForeignPort();
			}
			catch (SocketException e)
			{
				cerr << "Unable to get foreign port" << endl;
			}
			cout << endl;
			debugmode = true;

		}
		else
		{
			sleep(5);
		}
	}

	return NULL;
}

