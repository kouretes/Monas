#include "Vision.h"

#include "architecture/archConfig.h"
#include <cmath>
#include "sys/stat.h"
#include "tools/logger.h"
#include "tools/XMLConfig.h"
#include "tools/toString.h"
#include "hal/syscall.h"
#include <vector>
//#include "messages/motion.pb.h"
#include "hal/robot/generic_nao/robot_consts.h"

#include <vector>

#define TO_RAD 0.017453293f

#define trydelete(x) {if((x)!=NULL){delete (x);(x)=NULL;}}

//using namespace AL;
using namespace std;
//using namespace boost::posix_time;


namespace
{
	ActivityRegistrar<Vision>::Type temp("Vision");
}

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#define VISIBLE __attribute__ ((visibility("default")))
#else
#define VISIBLE
#endif


bool Vision::debugmode = false;
TCPSocket * Vision::sock;

bool FileExists(string strFilename)
{
	struct stat stFileInfo;
	bool blnReturn;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat(strFilename.c_str(), &stFileInfo);
	if (intStat == 0)
	{
		// We were able to get the file attributes
		// so the file obviously exists.
		blnReturn = true;
	} else
	{
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.
		blnReturn = false;
	}

	return (blnReturn);
}

void saveFrame(IplImage *fIplImageHeader)
{
	static int filenum = 0;

	char fname[128];
	do
	{
		sprintf(fname, (ArchConfig::Instance().GetConfigPrefix() + "images/" + std::string("img%03d.yuyv")).c_str(), filenum++);
	} while (FileExists(fname));
	ofstream frame(fname, ios_base::binary);
	frame.write(reinterpret_cast<char *> (fIplImageHeader->imageData), fIplImageHeader->width * fIplImageHeader->height * fIplImageHeader->nChannels);
	frame.close();

}

int VISIBLE Vision::Execute()
{
	//cout<<"Vision Execute"<<endl;
	boost::shared_ptr<const CalibrateCam> cal = _blk->readState<CalibrateCam> ("vision");
	//if (cal == NULL)
	//{
		//CalibrateCam res;
		//res.set_status(0);
		//_blk->publishState(res, "vision");
		//cout<<"---------Start calibration:"<<res.status()<<endl;
		//cal = _blk->readState<CalibrateCam> ("vision");
	//}
	if (cal != NULL)
	{
		//cout<<"=======Start calibration:"<<cal->status()<<endl;
		if (cal->status() == 0)
		{
			//cout<<"Start calibration"<<endl;
			CalibrateCam res;
			Logger::Instance().WriteMsg("Vision", "Start calibration", Logger::Info);
			float scale = ext.calibrateCamera(1000, cal->exp());
			lastrefresh=boost::posix_time::microsec_clock::universal_time()-boost::posix_time::microseconds(config.camerarefreshmillisec+10);
			Logger::Instance().WriteMsg("Vision", "Calibration Done", Logger::Info);
			//cout<<"Calibration Done!"<<endl;
			res.set_status(1);
			res.set_exposure_comp(scale);
			_blk->publishState(res, "vision");

		}
	}
#ifdef  CAPTURE_MODE
	if(frameNo++%20==0)
		saveFrame(rawImage);
#endif
	//Reload camera state periodically
	boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
	if(lastrefresh+boost::posix_time::millisec(config.camerarefreshmillisec)<now)
	{
		//cout<<"Refresh"<<endl;
		ext.refreshValues();//Reload
		if (ext.getCamera() == 1)//bottom cam
		{
			p.cameraPitch = (KMat::transformations::PI * 40.0) / 180.0;
			//cout<<"CameraPitch:"<<cameraPitch<<endl;
			seg = segbottom;

		} else
		{
			p.cameraPitch = 0;
			seg = segtop;
		}
		p.cameraPitch += config.pitchoffset;
		seg->setLumaScale(1 / ext.getScale());
		lastrefresh=now;


	}





	fetchAndProcess();
	//std::cout << " Vision run" << std::endl;
	if (debugmode)
	{
		//cout << "DEbug mode " << endl;
		recv_and_send();
	}

#ifdef KPROFILING_ENABLED
	vprof.generate_report();
#endif
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
		if (headersize < 1)
		{
			//cout<< "error there is no header < " << headersize << endl;
		}
		if (headersize > size)
		{
			//cout << " oups you must read more bytes in order to read the header " << headersize << endl;
		}

		headerparsed = incommingheader.ParseFromArray(data + 10, headersize);

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
			sock = NULL;
			return;

			//cout << " Stopping Debug ########################" << endl;
		}
		if (command == "seg")
			sendtype = -1; // meand segmented
		else if (command == "yuyv")
			sendtype = AL::kYUV422InterlacedColorSpace;
		else if (command == "ReferenceCalibration")
		{
			CalibrateCam res;
			res.set_status(0);
			res.set_exp(100000);
			_blk->publishState(res, "vision");

		}
		else if (command == "Calibration")
		{
			CalibrateCam res;
			res.set_status(0);
			//res.set_exp(100000);
			_blk->publishState(res, "vision");

		}

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
	//img.Clear();
	if (sendtype == AL::kYUV422InterlacedColorSpace)
	{
		img.set_imagerawdata(rawImage.imageData, rawImage.imageSize());
		img.set_bytes(rawImage.imageSize());
		//cout << " Raw image  size " << img.bytes() << endl;
		img.set_height(rawImage.height);
		img.set_width(rawImage.width);
		img.set_type(AL::kYUV422InterlacedColorSpace);
	}

	if (sendtype == -1) // segmented image
	{
		char segmended[rawImage.height][rawImage.width];
		for (int i = 0; i < rawImage.width; i++)
		{

			for (int j = 0; j < rawImage.height; j++)
			{
				KVecFloat2 im;
				KVecFloat3 c3d;
				im(0) = i;
				im(1) = j;
				im = imageToCamera(im);
				c3d = kinext.camera2dToGround(im);
				if (c3d(2) - 1 >= 0 && abs(c3d(2) - 1) <= 1)
				{
					segmended[j][i] = orange;

				} else
					segmended[j][i] = doSeg(i, j);

				//c3d=kinext.camera2dToGroundProjection(im,p.cameraZ-0.1);
				//if(sqrd(c3d(0))+sqrd(c3d(1))>100)
				//	segmended[j][i]=red;
			}
		}

		vector<KVecInt2>::iterator i;
		//For all detected points
		//cout << "locateball" << endl;
		for (i = ballpixels.begin(); i != ballpixels.end(); i++)
		{
			segmended[(*i).y][(*i).x] = red;
		};
		for (i = ygoalpost.begin(); i != ygoalpost.end(); i++)
		{
			segmended[(*i).y][(*i).x] = red;
		};

		 for(int k=0;k<16;k++)
		 {

		 segmended[int(64+k*Vup.y)][int(64+k*Vup.x)]=red;
		 }

		img.set_imagerawdata(segmended, rawImage.width * rawImage.height);
		img.set_bytes(rawImage.width * rawImage.height);
		//cout << " Seg image  size " << img.bytes() << endl;
		img.set_height(rawImage.height);
		img.set_width(rawImage.width);
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
	outgoingheader.SerializeToArray(data + 10, sendsize);
	//cout << "outgoingheader sendsize " << sendsize << endl;

	memset(data, 0, 10);
	strncpy(data, _toString(sendsize).c_str(), 9);
	sendsize += 10; // add 10 more bytes for the header
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
	} catch (SocketException &e)
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
	obs.Clear();
	img.Clear();
	//cout << "fetchImage" << endl;
	//unsigned long startt = SysCall::_GetCurrentTimeInUSec();

	boost::posix_time::ptime stamp = ext.fetchImage(rawImage);
	obs.set_image_timestamp(boost::posix_time::to_iso_string(stamp));
	//unsigned long endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"Fetch image takes:"<<endt<<endl;
	stamp += boost::posix_time::millisec(config.sensordelay);
	if (ext.getCamera() == 1)//bottom cam
	{
		//Get Kinematics first!
		std::vector<float> val = kinext.getKinematics("CameraBottom");

		p.cameraX = val[0];
		p.cameraY = val[1];
		p.cameraZ = val[2];//3rd element

	} else
	{
		//Get Kinematics first!
		std::vector<float> val = kinext.getKinematics("CameraTop");
		p.cameraX = val[0];
		p.cameraY = val[1];
		p.cameraZ = val[2];//3rd element
	}

	//cout<<"Attach to Image:"<<seg<<rawImage<<endl;
	seg->attachToIplImage(rawImage);//Make segmentator aware of a new image
	//saveFrame(rawImage);
	//return;
	//cout<<"Attached"<<endl;
	asvm = _blk->readData<AllSensorValuesMessage> ("sensors", "localhost", &p.time, &stamp);
#ifdef DEBUGVISION
	cout << "ImageTimestamp:"<< boost::posix_time::to_iso_string(stamp) << endl;
	cout << "SensorTimestamp:"<< boost::posix_time::to_iso_string(p.time) << endl;
#endif

	if (asvm.get() == NULL  )//No sensor data!
	{
		Logger::Instance().WriteMsg("Vision", "Warning!!! Vision has no head joint msg in all sensor (allsm) data!", Logger::Error);
		return;
	}

	if (asvm.get() == NULL )//No sensor data!
	{
		Logger::Instance().WriteMsg("Vision", "Warning!!! Vision has no intertial msg in all sensor (allsm) data!", Logger::Error);
		return;
	}

	//Clear result message

	p.yaw = asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW).sensorvalue();
	p.pitch = asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH).sensorvalue();

	p.Vyaw = asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW).sensorvaluediff();
	p.Vpitch = asvm->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH).sensorvaluediff();


	p.angX = asvm->computeddata(KDeviceLists::ANGLE+KDeviceLists::AXIS_X).sensorvalue();//asvm->sensordata(KDeviceLists::ANGLE+KDeviceLists::AXIS_X).sensorvalue();
	p.angY = asvm->computeddata(KDeviceLists::ANGLE+KDeviceLists::AXIS_Y).sensorvalue();//asvm->sensordata(KDeviceLists::ANGLE+KDeviceLists::AXIS_Y).sensorvalue();
	p.VangX = asvm->computeddata(KDeviceLists::ANGLE+KDeviceLists::AXIS_X).sensorvaluediff();
	p.VangY = asvm->computeddata(KDeviceLists::ANGLE+KDeviceLists::AXIS_Y).sensorvaluediff();

	p.timediff = asvm->timediff();//Get time from headmessage
	//p.time = time_t_epoch;

	//float exptime=ext.getExp();//Compensate for middle of image
	//float imcomp=(exptime*1000.0)/p.timediff;


	float imcomp = ((stamp - p.time).total_nanoseconds()*0.5 )/ (p.timediff);

#ifdef DEBUGVISION
	cout<< p.yaw<<" "<<p.pitch<<" "<<p.Vyaw<<" "<<p.Vpitch<<" ";
	cout<<imcomp<<" "<<p.angX<< " "<<p.angY<<p.VangX<< " "<<p.VangY<<endl;
#endif
	//Estimate the values at excactly the timestamp of the image
	p.yaw += p.Vyaw * imcomp;
	p.pitch += p.Vpitch * imcomp;
	p.angX += p.VangX * imcomp;
	p.angY += p.VangY * imcomp;
	float Dfov;
	xmlconfig->QueryElement("Dfov", Dfov);

	p.focallength = sqrt(sqrd(rawImage.width) + sqrd(rawImage.height) ) / (2 * tan(Dfov * TO_RAD / 2));

	//Logger::Instance().WriteMsg("Vision", _toString("Focal Length ")+_toString(p.focallength), Logger::Error);
	kinext.setPose(p);

	KMat::transformations::rotate(simpleRot,-kinext.getRoll());
	//Now change y axis :)
	simpleRot(0,1)=-simpleRot(0,1);
	simpleRot(1,1)=-simpleRot(1,1);
	//Weird stuff. simpleRot-^-1=simpleRot'=simpleRot. therefore translaton from raw image to rotated and axis inverted
	//image and back is done by simpleRot

//
//		Vup.y = -cos(-kinext.getRoll());
//		Vup.x = sin(-kinext.getRoll());
	Vup=simpleRot.slow_mult(KVecFloat2(0,1));


	Vdn.x = -Vup.x;
	Vdn.y = -Vup.y;

	Vlt.x = Vup.y;//-Vup.x*cos(-45*TO_RAD)-Vup.y*sin(-45*TO_RAD);
	Vlt.y = -Vup.x;//-Vup.x*sin(-45*TO_RAD)+Vup.y*sin(-45*TO_RAD);

	Vrt.x = -Vup.y;
	Vrt.y = Vup.x;
	//unsigned long startt,endt;
	//startt=SysCall::_GetCurrentTimeInUSec();
	ballpixels.clear();
	ygoalpost.clear();
	bgoalpost.clear();
	obstacles.clear();

	gridScan(orange);
	//endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"GridScan takes:"<<endt<<endl;


	//startt=SysCall::_GetCurrentTimeInUSec();
	balldata_t b =locateBall(ballpixels);
	//endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"locateball takes:"<<endt<<endl;
	//unsigned long endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"locateball takes:"<<endt<<endl;
	//cout<<b.r<<endl;
	locateGoalPost(ygoalpost, yellow);
	locateGoalPost(bgoalpost, skyblue);
#ifdef DEBUGVISION
	cout << "Ballpixelsize:" << ballpixels.size() << endl;
	//cout << b.x << " " << b.y << " " << b.cr << endl;
	//KVecFloat2 & w=camToRobot(o)
	cout<<"Bearing:"<<b.bearing.mean<<" "<<b.bearing.var<<endl;
	cout<<"Distance:"<<b.distance.mean<<" "<<b.distance.var<<endl;

#endif

	if (b.cr > 0)
	{
		//Fill message and publish
		trckmsg.set_cx(0);
		trckmsg.set_cy(0);
		KVecFloat2 im;
		im(0) = b.x;
		im(1) = b.y;
		KVecFloat2 c;
		KVecFloat3 c3d;
		c = imageToCamera(im);
		c3d = kinext.camera2dToTorso(c);
		//c3d(0)+p.cameraX;c3d(1)+cameraY;
		//if(sqrt(sqrd(c3d(0)) + sqrd(c3d(1)))<0.1)
		//	c3d.scalar_mult(2);
		float pitch, yaw;
		pitch = atan(abs(c3d(2)) / sqrt(sqrd(c3d(0)) + sqrd(c3d(1))));
		yaw = atan2(c3d(1), c3d(0));
		float w=b.distance.mean*3;
		w=w>1?1:w;
		trckmsg.set_referenceyaw(yaw*w);
		trckmsg.set_referencepitch(pitch - p.cameraPitch);
		trckmsg.set_radius(b.cr);
		//trckmsg.set_topic("vision");
		BallObject ballpos;
		ballpos.set_dist(b.distance.mean);
		ballpos.set_bearing(b.bearing.mean);
		ballpos.set_ball_diameter(b.ballradius * 2);
		obs.mutable_ball()->CopyFrom(ballpos);
		LedValues* l = leds.add_leds();
		l->set_chain("l_eye");
		l->set_color("red");
	} else
	{
		trckmsg.set_cx(b.x);
		trckmsg.set_cy(b.y);
		trckmsg.set_radius(-1);
		//trckmsg.set_topic("vision");
		LedValues* l = leds.add_leds();
		l->set_chain("l_eye");
		l->set_color("green");
	}
	if (obs.regular_objects_size() > 0)
	{
		bool yellow = false, blue = false;
		::google::protobuf::RepeatedPtrField<const ::NamedObject>::const_iterator ptr = obs.regular_objects().begin();
		while (ptr != obs.regular_objects().end())
		{

			if ((*ptr).object_name().c_str()[0] == 'Y')
				yellow = true;
			else if ((*ptr).object_name().c_str()[0] == 'S')
				blue = true;
			++ptr;

		}
		LedValues* l = leds.add_leds();
		l->set_chain("r_eye");
		if (yellow && !blue)
		{

			l->set_color("yellow");
		} else if (!yellow && blue)
		{

			l->set_color("blue");
		} else if (yellow && blue)
		{

			l->set_color("purple");
		}

	} else
	{

		LedValues* l = leds.add_leds();
		l->set_chain("r_eye");
		l->set_color("black");

	}
	_blk->publishSignal(trckmsg, "vision");
	_blk->publishSignal(leds, "leds");

	if (obs.has_ball() || obs.regular_objects_size() > 0 || obs.adhoc_objects_size() > 0 || obs.corner_objects_size() > 0 || obs.intersection_objects_size() > 0
			|| obs.line_objects_size() > 0)
		_blk->publishSignal(obs, "vision");

}

VISIBLE Vision::Vision() :
	xmlconfig(NULL), vprof("Vision"),type(VISION_CSPACE)
{
	debugmode = false;

	int max_bytedata_size = 700000;

	data = new char[max_bytedata_size]; //## TODO  FIX THIS BETTER
}

void VISIBLE Vision::UserInit()
{
#ifdef CAPTURE_MODE
	frameNo=0;
#endif
	loadXMLConfig(ArchConfig::Instance().GetConfigPrefix() + "/vision.xml");
	if (xmlconfig->IsLoadedSuccessfully() == false)
		Logger::Instance().WriteMsg("Vision", "vision.xml Not Found", Logger::FatalError);

	ext.Init(_blk);
	kinext.Init();
	Logger::Instance().WriteMsg("Vision", "ext.allocateImage()", Logger::Info);
	//cout << "Vision():" ;//<< endl;
	//rawImage = ext.allocateImage();

	ifstream *conffile = new ifstream((ArchConfig::Instance().GetConfigPrefix() + "colortables/" + config.SegmentationBottom).c_str());
	segbottom = new KSegmentator(*conffile);
	conffile->close();
	delete conffile;
	if(config.SegmentationTop==config.SegmentationBottom)//Same file, do not load twice
		segtop=segbottom;
	else
	{
		conffile = new ifstream((ArchConfig::Instance().GetConfigPrefix() + "colortables/" + config.SegmentationTop).c_str());
		segtop = new KSegmentator(*conffile);
		conffile->close();
		delete conffile;

	}
	lastrefresh=boost::posix_time::microsec_clock::universal_time()-boost::posix_time::microseconds(config.camerarefreshmillisec+10);
	seg=segbottom;



	//_com->get_message_queue()->add_subscriber(_blk);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("vision", 0);
	//_com->get_message_queue()->add_publisher(this);

	debugmode = false;

	int max_bytedata_size = 1000000;
	sendtype = AL::kYUV422InterlacedColorSpace;
	data = new char[max_bytedata_size]; //## TODO  FIX THIS BETTER

	pthread_create(&acceptthread, NULL, &Vision::StartServer, this);
	pthread_detach(acceptthread);

}

void Vision::loadXMLConfig(std::string fname)
{
	trydelete(xmlconfig);
	xmlconfig = new XMLConfig(fname);//ArchConfig::Instance().GetConfigPrefix()+"/vision.xml");

	xmlconfig->QueryElement("camerarefreshmillisec", config.camerarefreshmillisec);

	xmlconfig->QueryElement("SegmentationBottom", config.SegmentationBottom);
	xmlconfig->QueryElement("SegmentationTop", config.SegmentationTop);
	xmlconfig->QueryElement("sensordelay", config.sensordelay);



	//xmlconfig->QueryElement("scanstep",config.scanstep);
	xmlconfig->QueryElement("scanV", config.scanV);
	xmlconfig->QueryElement("scanH", config.scanH);
	xmlconfig->QueryElement("minH", config.minH);
	xmlconfig->QueryElement("skipdistance", config.skipdistance);
	xmlconfig->QueryElement("bordersize", config.bordersize);
	xmlconfig->QueryElement("subsampling", config.subsampling);
	xmlconfig->QueryElement("seedistance", config.seedistance);
	xmlconfig->QueryElement("obstacledistance", config.obstacledistance);
	xmlconfig->QueryElement("balltolerance", config.balltolerance);
	xmlconfig->QueryElement("ballsize", config.ballsize);
	xmlconfig->QueryElement("pixeltol", config.pixeltol);

	xmlconfig->QueryElement("goalheight", config.goalheight);
	xmlconfig->QueryElement("goaldist", config.goaldist);
	xmlconfig->QueryElement("goaldiam", config.goaldiam);
	xmlconfig->QueryElement("goalslopetolerance", config.goalslopetolerance);

	xmlconfig->QueryElement("widthestimateotolerance", config.widthestimateotolerance);

	xmlconfig->QueryElement("pitchoffset", config.pitchoffset);
}

void Vision::publishObstacles(std::vector<KVecInt2> points) const
{
	static int period = 0;
	period++;
	if (period > 5)
		period = 0;
	vector<KVecInt2>::iterator i;//candidate iterator
	VisionObstacleMessage result;
	if (!(result.obstacles_size() > 2 && period == 0))
		return;

	for (i = points.begin(); i != points.end(); i++)
	{

		KVecFloat2 point;
		point(0) = (*i).x;
		point(1) = (*i).y;

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
	_blk->publishSignal(result, "obstacle");

}

KVecFloat2 Vision::simpleRotation(KVecFloat2 const& i) const
{
	return simpleRot.slow_mult(i);
}


KVecFloat2 Vision::simpleRotation(KVecInt2 const& i) const
{
	KVecFloat2 t;
	t.x=i.x;
	t.y=i.y;
	return simpleRotation(t);
}

KVecFloat2 Vision::imageToCamera( KVecFloat2 const & imagep) const
{

	KVecFloat2 res;
	res(0) = imagep(0) - rawImage.width / 2.0 + 0.5;
	res(1) = -(imagep(1) - rawImage.height / 2.0 + 0.5);

	return res;
}

KVecFloat2 Vision::imageToCamera( KVecInt2 const & imagep) const
{

	KVecFloat2 res;
	res(0) = imagep(0) - rawImage.width / 2.0 + 0.5;
	res(1) = -(imagep(1) - rawImage.height / 2.0 + 0.5);

	return res;
}

KVecInt2 Vision::cameraToImage( KVecFloat2 const& c) const
{

	KVecInt2 res;

	res(0) = (int) (c(0) + rawImage.width / 2.0 - 0.5);
	res(1) = (int) (-c(1) + rawImage.height / 2.0 - 0.5);

	return res;
}

//Input:  distance bearing
KVecFloat2 Vision::camToRobot(KVecFloat2 const & t) const
{
	KVecFloat2 res;
	float a = cos(t(1)) * t(0) + p.cameraX;
	float b = sin(t(1)) * t(0) + p.cameraY;
	res(0) = sqrt((a) * (a) + (b) * (b));
	res(1) = atan2(b, a);
	return res;
}

void * Vision::StartServer(void * s)
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
				//cout << " REturned null";
				return NULL;
			}
			cout << "Handling client ";
			try
			{
				cout << sock->getForeignAddress() << ":";
			} catch (SocketException e)
			{
				cerr << "Unable to get foreign address" << endl;
			}
			try
			{
				cout << sock->getForeignPort();
			} catch (SocketException e)
			{
				cerr << "Unable to get foreign port" << endl;
			}
			cout << endl;
			debugmode = true;

		} else
		{
			sleep(5);
		}
	}

	return NULL;
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif
