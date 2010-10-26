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
#define TRACESKIP 8
#define GLOBALTRACESKIP 25
//x COVERDIST=15*0.05m
#define SCANSKIP  4
#define SPARSENESS 3
#define TO_RAD 0.01745329f



#define trydelete(x) {if((x)!=NULL){delete (x);(x)=NULL;}}

#define CvDist(pa,pb) sqrt(((pa).x-(pb).x )*((pa).x-(pb).x )+((pa).y-(pb).y )*((pa).y-(pb).y ) )
//Distance from observation angle
//#define angularDistance(Hr,Ho,yaw,pitch) ( sqrt(1+sin(yaw)*sin(yaw))*((Hr)-(Ho))*kinext.cot(pitch) )
//#define apparentDistance(Hr,Ho,rad) ( sqrt ( ( (((Ho)*(Ho) ) -((Hr)*(Hr)*tan(rad)*tan(rad)))*kinext.cot(rad)*kinext.cot(rad)    )  ))



using namespace AL;
using namespace std;
//using namespace boost::posix_time;


namespace
{
ActivityRegistrar<Vision>::Type temp("Vision");
}

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
	if(intStat == 0)
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
		sprintf(fname,(ArchConfig::Instance().GetConfigPrefix()+std::string("img%03d.yuyv")).c_str(),filenum++);
	}
	while(FileExists(fname));
	ofstream frame(fname,ios_base::binary);
	frame.write(reinterpret_cast<char *>(fIplImageHeader->imageData),fIplImageHeader->width*fIplImageHeader->height*fIplImageHeader->nChannels);
	frame.close();


}


int  Vision::Execute()
{
	static bool calibrated = false;
	boost::shared_ptr<const CalibrateCam>  cal= _blk->read_signal<CalibrateCam>("CalibrateCam");
	if(_blk->read_state<CalibrateCam>("CalibrateCam")==NULL)
	{
		CalibrateCam res;
		res.set_status(0);
		_blk->publish_state(res,"vision");
	}
	if(cal==NULL) cal= _blk->read_state<CalibrateCam>("CalibrateCam");
	if(cal!=NULL)
	{
		if( cal->status()==0)
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

	testrun();
	//std::cout << " Vision run" << std::endl;
	if(debugmode)
	{
		cout << "DEbug mode " << endl;
		recv_and_send();
	}
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
		cout << "Waiting for " << size << " Bytes " << endl;
		if (ssize < size)
		{
			if ((ss = sock->recv(data + ssize, size - ssize)) < 0)
			{
				cout << "receive error" << endl;
				break;
			}
			ssize += ss;
		}
		cout << "Arrived " << ssize << " Bytes " << endl;
		headersize = atoi(data);
		if(headersize<1){
			cout<< "error there is no header < " << headersize << endl;
		}
		if(headersize > size)
		{
			cout << " oups you must read more bytes in order to read the header " << headersize << endl;
		}

		headerparsed = incommingheader.ParseFromArray(data+10, headersize);

		if (!headerparsed)
		{
			cout << " Unable to parse i was expecting" << endl;
			continue;
		}

		incommingheader.DiscardUnknownFields();
		int alreadyparsedbytes = incommingheader.ByteSize()+10;

		cout << "alreadyparsedbytes " << alreadyparsedbytes << " Bytes" << endl;
	}
	if(headerparsed) {
		string command = incommingheader.nextmsgname();
		if (command == "Stop") {
			debugmode = false;
			cout << " Stopping Debug ########################" << endl;
		}
		if ((size = incommingheader.nextmsgbytesize()) > 0) //must read next message
		{
			int ssize;
			int ss;
			ssize = 0;
			while (ssize < size) {
				if ((ss = sock->recv(data + ssize, size - ssize)) < 0) {
					cout << "receive error" << endl;
					break;
				}
				ssize += ss;
			}
			cout << "Arrived " << ssize << " Bytes Do something" << endl;
		}
	}

	img.set_imagerawdata(rawImage->imageData );
	img.set_bytes(rawImage->imageSize);
	img.set_height(rawImage->height);
	img.set_width(rawImage->width);
	img.set_type(kYUV422InterlacedColorSpace);

	outgoingheader.set_nextmsgbytesize(img.ByteSize());
	outgoingheader.set_nextmsgname(img.GetTypeName());

	int sendsize;

	int rsize = 0;
	int rs;
	//send a header
	outgoingheader.set_mysize(sendsize = outgoingheader.ByteSize());
	while (sendsize != outgoingheader.ByteSize()) {
		outgoingheader.set_mysize(sendsize = outgoingheader.ByteSize());
	}
	outgoingheader.SerializeToArray(data+10, sendsize);
	cout << "outgoingheader sendsize " << sendsize << endl;

	memset(data,0,10);
	strncpy(data,_toString(sendsize).c_str(),9);
	sendsize+=10; // add 10 more bytes for the header
	try {
		while (rsize < sendsize) {
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
		cout << "Will send Data" << sendsize << " " << img.GetTypeName() << endl;

		while (rsize < sendsize) {
			rs = sock->send((char *) buf.data() + rsize, sendsize - rsize);// UDT::send(recver, data + rsize, sendsize - rsize, 0))) {
			rsize += rs;
		}
		cout << "Sended " << rsize << endl;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		cout << "Disconnecting !!!" << endl;
		debugmode = false;
	}

}

void Vision::testrun()
{
	static unsigned delay = 0;
	static bool has_ball = false;
	leds.Clear();

	int sensordelay;
	config->QueryElement("sensordelay",sensordelay);


	cout << "fetchImage" << endl;
	unsigned long startt = SysCall::_GetCurrentTimeInUSec();

	boost::posix_time::ptime stamp = ext.fetchImage(rawImage);
	unsigned long endt = SysCall::_GetCurrentTimeInUSec()-startt;
	cout<<"Fetch image takes:"<<endt<<endl;
	stamp+=boost::posix_time::millisec(sensordelay);
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
//#ifdef DEBUGVISION
//	cout << "ImageTimestamp:"<< boost::posix_time::to_iso_string(stamp) << endl;
//#endif
	//boost::posix_time::ptime rtime =  time_t_epoch+(boost::posix_time::microsec(t.tv_nsec/1000)+boost::posix_time::seconds(t.tv_sec));//+sec(t.tv_sec));
//	hm = _blk->read_nb<HeadJointSensorsMessage>("HeadJointSensorsMessage", "Sensors");
	//im = _blk->read_nb<InertialSensorsMessage>("InertialSensorsMessage", "Sensors",&p.time,&stamp);
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
//	cout<<boost::posix_time::to_iso_string(stamp)<<endl;
//	cout<<boost::posix_time::to_iso_string(p.time)<<endl;
//	cout<<"imcomp:"<<imcomp<<endl;
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
	config->QueryElement("Dfov",Dfov);

	p.focallength=sqrt(rawImage-> width*rawImage-> width+rawImage-> height*rawImage-> height)/(2*tan(Dfov*TO_RAD/2));

	//Logger::Instance().WriteMsg("Vision", _toString("Focal Length ")+_toString(p.focallength), Logger::Error);
	kinext.setPose(p);

	gridScan(orange);
	obs.set_image_timestamp(boost::posix_time::to_iso_string(stamp));

	if(obs.has_ball()||obs.regular_objects_size()>0 || obs.adhoc_objects_size()>0
			||obs.corner_objects_size()>0  || obs.intersection_objects_size()>0
			||obs.line_objects_size()>0)
		_blk->publish_signal(obs,"vision");
	LedValues* l=leds.add_leds();
	if(has_ball != obs.has_ball())
	{
		has_ball = obs.has_ball();
		if(has_ball)
		{
			l->set_chain("l_eye");
			l->set_color("red");
		}
		else
		{
			l->set_chain("l_eye");
			l->set_color("green");

		}
		_blk->publish_signal(leds,"communication");
	}
	else
	{
		if(++delay %= 20)
		{
			if(has_ball)
			{
				l->set_chain("l_eye");
				l->set_color("red");
			}
			else
			{
				l->set_chain("l_eye");
				l->set_color("green");

			}
		}
	}
	bool cvHighgui;
	config->QueryElement("cvHighgui",cvHighgui);
	if (cvHighgui)
		cvShowSegmented();




}

bool Vision::validpixel(int x,int y)
{
	if((x >= 0 && x < (rawImage-> width) && y >= 0 && y < (rawImage-> height)))
		return true;
	else
		return false;

}
KSegmentator::colormask_t Vision::doSeg(int x, int y)
{
	if (x >= 0 && x < (rawImage-> width) && y >= 0 && y < (rawImage-> height))
	{
		return seg->classifyPixel(rawImage, x, y, type);
		//return sgm->getColor(y, x, (uInt8 *) fIplImageHeader->imageData, false);
	}
	else
	{
		return 0;
	}

}

Vision::Vision() : type(VISION_CSPACE)
{
	debugmode = false;

	int max_bytedata_size = 700000;

	data = new char[max_bytedata_size]; //## TODO  FIX THIS BETTER
}

void Vision::UserInit()
{
	config = new XMLConfig(ArchConfig::Instance().GetConfigPrefix()+"/vision.xml");
	if(config->IsLoadedSuccessfully()==false)
		Logger::Instance().WriteMsg("Vision", "vision.xml Not Found", Logger::FatalError);
	bool cvHighgui;
	config->QueryElement("cvHighgui",cvHighgui);

	if(cvHighgui==true)
		Logger::Instance().WriteMsg("Vision", "Enable highgui", Logger::Info);


	ext.Init(_blk);
	kinext.Init();
	Logger::Instance().WriteMsg("Vision", "ext.allocateImage()", Logger::Info);
	//cout << "Vision():" ;//<< endl;
	rawImage = ext.allocateImage();
	if (cvHighgui)
	{
		segIpl = cvCreateImage(cvSize(rawImage->width, rawImage->height), IPL_DEPTH_8U, 3);
		cvNamedWindow("win1", CV_WINDOW_AUTOSIZE);
	}
	//memory = pbroker->getMemoryProxy();
	std::string fname;
	config->QueryElement("SegmentationBottom",fname);
	ifstream *conffile = new ifstream((ArchConfig::Instance().GetConfigPrefix()+
									   "colortables/"+fname).c_str());
	segbottom = new KSegmentator(*conffile);
	conffile->close();
	delete conffile;

	config->QueryElement("SegmentationTop",fname);
	conffile = new ifstream((ArchConfig::Instance().GetConfigPrefix()+
							 "colortables/"+fname).c_str());


	segtop = new KSegmentator(*conffile);
	conffile->close();
	delete conffile;


	cout<<"Add Subscriber-publisher"<<endl;
	//_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	//_com->get_message_queue()->add_publisher(this);

	debugmode = false;

	int max_bytedata_size = 100000;

	data = new char[max_bytedata_size]; //## TODO  FIX THIS BETTER
	//Turn off leds
	pthread_create(&acceptthread, NULL, &Vision::StartServer, this);
	pthread_detach( acceptthread);
// 	publish(&leds,"communication");
}
void Vision::gridScan(const KSegmentator::colormask_t color)
{
	//Horizontal + Vertical Scan
	ballpixels.clear();
	ygoalpost.clear();
	bgoalpost.clear();
	obstacles.clear();


	KMat::HCoords<int,2> im;
	KMat::HCoords<float,2> c;
	KMat::HCoords<float,3> c3d;
	static int startx=0;
	float  skipdistance,seedistance,obstacledistance;
	int  step,borderskip;
	//config->QueryElement("ballsize",ballsize);
	config->QueryElement("scanstep",step);
	config->QueryElement("skipdistance",skipdistance);
	config->QueryElement("borderskip",borderskip);
	config->QueryElement("seedistance",seedistance);
	config->QueryElement("obstacledistance",obstacledistance);

	//int points[rawImage->width];

	KSegmentator::colormask_t tempcolor;
	//int ballpixel = -1;
	unsigned int cntwhitegreenpixels = 0;
	unsigned int cntwhitegreenorangepixels=0;
	unsigned int cntother=0;

	startx++;
	startx=startx%step;
	float stepy=-step,stepx=-step*tan(-kinext.getRoll());//TODO :: is this correct?
	//cout<<"Camera Roll:"<<-kinext.getRoll()<<endl;

	//cout<<"loop"<<endl;
	for (int i = borderskip+startx; i < rawImage->width-borderskip-1; i = i + step)
	{
		//cout<<"wtf"<<endl;
		//Thru Horizon Possibly someday
		//cout<<"outer"<<endl;
		cntwhitegreenpixels = 0;
		cntwhitegreenorangepixels=0;
		cntother=0;
		bool ballfound=false;
		// ballpixel = -1;
		//int ci,cj;
		im(0)=i;
		im(1)= rawImage->height - borderskip-1;
		//c=imageToCamera(im);
		//c.prettyPrint();
		c3d=kinext.camera2dToGroundProjection(c,0);
		//Hit maximum "see" distance
		//if(sqrd(c3d(0))+sqrd(c3d(1))>=sqrd(seedistance))//Looking way too far
		//    continue;

		KMat::HCoords<int,2> lastpoint;
		lastpoint=im;//Copy first point

		while(validpixel(im(0),im(1)))
		{
			//cout<<"doseg"<<endl;
			tempcolor = doSeg(im(0), im(1));
			if (tempcolor == white ||tempcolor == green)//
			{
				cntwhitegreenpixels++;
				cntwhitegreenorangepixels++;
				cntother=0;
				lastpoint=im;
			}
			else if (tempcolor==orange)
			{
				cntwhitegreenorangepixels++;
				cntother=0;

			}
			else
			{
				//ballskip=0;
				cntother++;
			}
			if (cntother>SCANSKIP)//No continuity, break
			{
				cntother=0;
				c=imageToCamera(lastpoint);
				c3d=kinext.camera2dToGroundProjection(c,0);
				float d1=sqrt(sqrd(c3d(0))+sqrd(c3d(1)));

				if(d1>=sqrd(seedistance))//Looking way too far
				{
					//cout<<"Seedistance reached"<<endl;
					//c3d.prettyPrint();
					break;
				}
				c=imageToCamera(im);
				c3d=kinext.camera2dToGroundProjection(c,0);
				float d2=sqrt(sqrd(c3d(0))+sqrd(c3d(1)));

				if(abs(d1-d2)>skipdistance)
				{
					//cout<<"break"<<endl;
					CvPoint tmpPoint;
					tmpPoint.x = lastpoint(0);
					tmpPoint.y = lastpoint(1);
					obstacles.push_back(tmpPoint);
					break;
				}
			}
			if (tempcolor == orange &&ballfound==false)
			{
				CvPoint tmpPoint;
				tmpPoint.x = im(0);
				tmpPoint.y = im(1);
				ballpixels.push_back(tmpPoint);
				cntwhitegreenpixels=0;
				ballfound=true;
				//continue;
				//ballpixel = j;
			}
			//Find next pixel

			im(0)+=stepx;
			im(1)+=stepy;
			//stepx=2;
			//stepy-=2;

			//cout<<ci<<","<<cj<<endl;
			//c=imageToCamera(im);
			// c3d=kinext.camera2dToGroundProjection(c,0);
			// float d=sqrt(sqrd(c3d(0))+sqrd(c3d(1)));
			//float corr=1+ skipdistance/d;

			//if(d>=sqrd(seedistance))//Looking way too far
			//{
			//cout<<"Seedistance reached"<<endl;
			//c3d.prettyPrint();
			// break;
			//}

		}

		//KMat::HCoords<float,2> &=imageToCameraAngles(point);
		//KMat::HCoords<float,2> & a=cameraToObs(point);

		//float xdist=angularDistance(cameraZ,0,a(2));
		//float xangle=atan2(COVERDIST,d*2)*2;
		//step=cos((*ang)(1))*xangle;
		//cout<<"xstep"<<step<<endl;
		//if (step<=0) step=1;
		//cout<<"xstep"<<step<<endl;
		//TODO xstep;
	}
	//publishObstacles(obstacles);
	//unsigned long startt = SysCall::_GetCurrentTimeInUSec();
	balldata_t b = locateBall(ballpixels);
	//unsigned long endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"locateball takes:"<<endt<<endl;
	//cout<<b.r<<endl;

#ifdef DEBUGVISION
	cout << "Ballpixelsize:" << ballpixels.size() << endl;
	cout << b.x << " " << b.y << " " << b.cr << endl;
#endif


#ifdef DEBUGVISION
	//KMat::HCoords<float,2> & w=camToRobot(o)
	cout<<"Bearing:"<<b.bearing.mean<<" "<<b.bearing.var<<endl;
	cout<<"Distance:"<<b.distance.mean<<" "<<b.distance.var<<endl;

#endif


	if (b.cr > 0)
	{


		//Fill message and publish
		trckmsg.set_cx(0);
		trckmsg.set_cy(0);
		KMat::HCoords<int,2> im;
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
		trckmsg.set_referenceyaw(yaw);
		trckmsg.set_referencepitch(pitch-p.cameraPitch);//-ball(2)
		trckmsg.set_radius(b.cr);
		//trckmsg.set_topic("vision");
		_blk->publish_signal(trckmsg,"vision");
		BallObject ballpos;
		ballpos.set_dist(b.distance.mean);
		ballpos.set_bearing(b.bearing.mean);
		ballpos.set_ball_diameter(b.ballradius*2);
		obs.mutable_ball()->CopyFrom(ballpos);


	}
	else
	{


		trckmsg.set_cx(b.x);
		trckmsg.set_cy(b.y);
		trckmsg.set_radius(-1);
		//trckmsg.set_topic("vision");
		_blk->publish_signal(trckmsg,"vision");
	}/* else {
		memory->insertData("kouretes/Ball/found", .0f); // change
	}*/
}


void Vision::publishObstacles(std::vector<CvPoint> points)
{
	static int period=0;
	period++;
	if(period>5)
		period=0;
	vector<CvPoint>::iterator i;//candidate iterator
	VisionObstacleMessage result;
	if(!(result.obstacles_size()>2&&period==0))
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

KMat::HCoords<float,2>  Vision::imageToCamera( const KMat::HCoords<int,2>  & imagep)
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


/*
KMat::HCoords<float,2> & Vision::cameraToObs(KMat::HCoords<float ,2> const& t)
{
	KMat::HCoords<float,2> &res=ct.transform(t);

	res(1)=-res(1)+p.yaw+(*ang)(3);
	res(2)=-res(2)+horizonAlpha;//p.pitch+(*ang)(2)+cameraPitch;

	return res;

}*/

//Input:  distance bearing
KMat::HCoords<float,2>  Vision::camToRobot(KMat::HCoords<float ,2> & t)
{
	KMat::HCoords<float,2>  res ;
	float a=cos(t(1))*t(0);
	float b=sin(t(1))*t(0);
	res(0)=sqrt((a+p.cameraX)*(a+p.cameraX)+(b+p.cameraY)*(b+p.cameraY));
	res(1)=atan2(b+p.cameraY,a+p.cameraX);
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
	for (int i = ball.x - innerrad; i <= ball.x + innerrad; i+=SPARSENESS)
		for (int j =ball. y - innerrad; j <= ball.y ; j+=SPARSENESS)
		{
			if (!validpixel(i,j))
				continue;
			if (doSeg(i, j) == c)
				gd+=3;
			ttl+=3;
		}

	for (int i = ball.x - innerrad; i <= ball.x + innerrad; i+=SPARSENESS)
		for (int j =ball. y ; j <= ball.y + innerrad; j+=SPARSENESS)
		{
			if (!validpixel(i,j))
				continue;
			if (doSeg(i, j) == c)
				gd++;
			ttl++;
		}

	ratio = ((float) gd+1) / (ttl+1);
	//<<ratio<<endl;
	if (ratio < 0.5)
		return false;



	//float appdist=apparentDistance(cameraZ,BALLRADIUS,((ball.r+0.5)*  HFov * TO_RAD )/((float)rawImage->width ));
	//cout<<appdist<<endl;
	//if(fabs((ball.d-appdist)/ball.d) > 0.50)//Wtf
	//return false;
	//cout<<"ball accepted"<<ball.r<<endl;
	return true;

}
bool Vision::calculateValidGoalPost(goalpostdata_t goal, KSegmentator::colormask_t c)
{
	int width =goal.lr.x-goal.ll.x+1;
	unsigned int ttl = 0, gd = 0;
	float ratio;
	for(int i=goal.ll.x; i<=goal.lr.x; i++)
	{
		for(int j=goal.bottom.y+width; j>goal.bottom.y; j--)
		{
			if (!validpixel(i,j))
				continue;
			if (doSeg(i, j) == green)
				gd++;
			ttl++;
		}
	}
	ratio=gd/ttl;
	if(ratio<0.5)
		return false;


	if(goal.height>0)
	{
		goal.conf=1;

	}
	else
		goal.conf=0;
}

bool cmpgoalpostdata_t (Vision::goalpostdata_t a,  Vision::goalpostdata_t b)
{
	return a.d < b.d;
}

Vision::goalpostdata_t Vision::locateGoalPost(vector<CvPoint> cand, KSegmentator::colormask_t c)
{
	/*

	    CvPoint2D32f Vup;//Vertical velocity
	    Vup.y=-1;
	    Vup.x=-tan((*ang)(1));

	    vector<goalpostdata_t> history;

	    vector<CvPoint>::iterator i;//candidate iterator
	    for (i = cand.begin(); i != cand.end(); i++)
		{

			vector<goalpostdata_t>::iterator hi = history.begin();
			while (hi != history.end() && i != cand.end())
			{

			    float left=((*i).y-(*hi).ll.y)*Vup.x + (*hi).ll.x-1;
			    float right=((*i).y-(*hi).lr.y)*Vup.x + (*hi).lr.x+1;
			    if((*i).x>=left&&(*i).x<=right)//inside possible goalpost
			    {
			        i++;//Skip pixel
			        if (i == cand.end())
						break;
					hi = history.begin();
			    }
	                hi++;
			}

			if (i == cand.end())
				break;

			if (!validpixel((*i).x,(*i).y))
				continue;
	        GoalPostdata newpost;

	        //Find width
	        CvPoint pleft=   traceline((*i), cvPoint(-1, 0), c);
	        CvPoint pright = traceline((*i), cvPoint(+1, 0), c);
	        if(pleft.x==-1&&pright.x==-1)//WHAT THE HELL
	            continue;
	        if(pleft.x==-1)
	        {
	            pleft.x=0;
	            pleft.y=(*i).y;
	        }

	        if(pright.x==-1)
	        {
	            pright.x=rawImage->width -1;
	            pright.y=(*i).y;
	        }

	        CvPoint middle;
	        middle.x= (pleft.x+pright.x)/2;
	        middle.y=(*i).y;
	        //Trace down
	        CvPoint curr=middle;
	        CvPoint latestValid=middle;


	        int skipcount=0,globalskipcount=0,fieldcount=0;
	        /////cout << "traceline:"<<start.x<<" "<<start.y<<endl;


	        while (validpixel(curr.x,curr.y))
	        {
	            KSegmentator::colormask_t t=doSeg(curr.x, curr.y);
	            if (t == c)
	            {
	                latestValid = curr;
	                skipcount = 0;
	                fieldcount=0;

	            }
	            else if ( t == green)
				{
				    fieldcount++;
				}
	            else
	            {
	                skipcount++;
	                globalskipcount++;
	            };

	            if (skipcount > TRACESKIP || globalskipcount > GLOBALTRACESKIP)
	            {

	                break;

	            }

	            curr.y += 1;
	            curr.x += middle.x+Vup.x*(curr.y-middle.y);


	        }
	        if(fieldcount<SCANSKIP)//No go
	                continue;

	        middle=latestValid;
	        //============= Done tracing=============

	        //Now back up again so that you skip the curvature at the bottom
	        int len=1;
	        int lastlen;
	        //curr.x, curr.y is our bottom point
	        curr=middle;
	        do
	        {
	            lastlen=len;
	            CvPoint l = traceline(curr, cvPoint(-1, 0), c);
	            CvPoint r = traceline(curr, cvPoint(+1, 0), c);
	            if(l.x==-1&&r.x==-1)//WHAT THE HELL
	            {
	                len=0;
	                break;
	            }
	            if(l.x==-1)
	            {
	                l.x=0;
	                l.y=(*i).y;
	            }

	            if(r.x==-1)
	            {
	                r.x=rawImage->width -1;
	                r.y=(*i).y;
	            }
	            len=r.x-l.x+1;
	            curr.y--;//Go up
	            newpost.ll=l;
	            newpost.lr=r;


	        }
	        while(len>lastlen);
	        if(len==0)
	            continue;

	        newpost.bottom=newpost.ll;
	        newpost.bottom.x=(newpost.ll.x+newpost.ll.y)/2;


	        //Now trace up the height

	        skipcount=0;
	        globalskipcount=0;
	        while (validpixel(curr.x,curr.y))
	        {
	            KSegmentator::colormask_t t=doSeg(curr.x, curr.y);
	            if (t == c)
	            {
	                latestValid = curr;
	                skipcount = 0;
	            }
	            else
	            {
	                skipcount++;
	                globalskipcount++;
	            };

	            if (skipcount > TRACESKIP || globalskipcount > GLOBALTRACESKIP)
	            {
	                curr.x=-1;
	                break;

	            }

	            curr.y += -1;
	            curr.x += middle.x+Vup.x*(curr.y-middle.y);
	        }
	        if (!validpixel(latestValid.x,latestValid.y))
	            newpost.height=-1;
	        else
	        {
	            newpost.top=latestValid;
	            newpost.height=CvDist(newpost.bottom,latestValid);
	        }



	        KMat::HCoords<float,2> point;
	        point(1)=newpost.bottom.x;
	        point(2)=newpost.bottom.y;
	        KMat::HCoords<float,2> &a=imageToCameraAngles(point);
	        KMat::HCoords<float,2> & b=cameraToObs(a);
	        delete &a;
			newpost.d=angularDistance(p.cameraZ,BALLRADIUS,point(1),b(2));

			delete &b;



			newpost.d=angularDistance(p.cameraZ,BALLRADIUS,point(1),b(2));
			//cout<<"ball dist:"<<newdata.d<<endl;

			if(newpost.d<=0||newpost.d>=LONGESTDIST)
	            continue;

	        if(calculateValidGoalPost(newpost,c))
	            history.push_back(newpost);

		}
		std::sort (history.begin(), history.end(), cmpgoalpostdata_t);
		//if(history.size()==2)
		//{
		   // NamedObject *gp=obs->add_regular_objects();
		    //if(c==yellow)
	         //   gp->set_

		//}
	*/
}
/*
CvPoint Vision::sizeTrace(CvPoint start, CvPoint vel, KSegmentator::colormask_t c)
{
	int skipcount = 0;
	int globalcount = 0;
	CvPoint curr = start;
	CvPoint latestValid
 = start;
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
			latest3 = curr;
			skipcount = 0;
		};

		if (skipcount > TRACESKIP || globalcount > GLOBALTRACESKIP)
			break;
		curr.x += vel.x;
		curr.y += vel.y;

	}
	//cout<<"ret"<<latestValid.x<<" "<<latestValid.y<<endl;
	return latestValid;
}
*/
Vision::balldata_t Vision::locateBall(vector<CvPoint> cand)
{
	//Skip first/last row/col
	KMat::HCoords<float,2> point;
	vector<balldata_t> history;

	vector<CvPoint>::iterator i;
	//For all detected points
	//cout << "locateball" << endl;
	for (i = cand.begin(); i != cand.end(); i++)
	{
		vector<CvPoint> points;
		vector<balldata_t>::iterator bd = history.begin();
		while (bd != history.end() )
		{
			if (CvDist(*bd,*i) <= sqrd((*bd).cr))
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
		if (!validpixel((*i).x,(*i).y))
			continue;

		CvPoint bottom = traceline((*i), cvPoint(0, 1), orange);
		if (!validpixel(bottom.x,bottom.y))
			continue; //No top pixel :)
		CvPoint t = traceline((*i), cvPoint(0, -1), orange);
		if (!validpixel(t.x,t.y))
			continue; //No top pixel :)

		CvPoint r = traceline(bottom, cvPoint(1, 0), orange);//Prefer  r
		if (!validpixel(r.x,r.y))
			r = traceline(bottom, cvPoint(1, -1), orange);

		if (!validpixel(r.x,r.y))// No right pixel available?!?
			continue;
		CvPoint l = traceline(bottom, cvPoint(-1, 0), orange);//Prefer top l
		if (!validpixel(l.x,l.y))
			l = traceline(bottom, cvPoint(-1, -1), orange);

		if (!validpixel(l.x,l.y))// No left pixel available?!?
			continue;

		CvPoint2D32f center;
		//cout<<t.x<<" "<<t.y<<endl;
		//cout<<l.x<<" "<<l.y<<endl;
		//cout<<r.x<<" "<<r.y<<endl;
		if(t.x==l.x||t.x==r.x)
			continue;
		float ma=(t.y-r.y)/(t.x-l.x);
		float mb=(r.y-t.y)/(r.x-t.x);
		if(mb==ma)
			continue;
		center.x= ( ma*mb*(l.y-r.y)+mb*(l.x+t.x)-ma*(t.x+r.x) ) / (2.0*(mb-ma));
		//cout<<center.x<<endl;
		//-1*(m_Center.x() - (pt1->x()+pt2->x())/2)/aSlope +  (pt1->y()+pt2->y())/2;
		center.y=-(center.x -(l.x+t.x)/2)/ma+(l.y+t.y)/2.0;
		//cout<<center.y<<endl;
		float radius = CvDist(center,t)-1;
		/*
				CvPoint top = traceline((*i), cvPoint(0, -1), orange);
				if (!validpixel(top.x,top.y))
					continue;
				points.push_back(bottom);
				points.push_back(top);

				CvPoint middle;
				middle.x = top.x;
				middle.y = (top.y + bottom.y) / 2;

				CvPoint2D32f center;
				center.x = 0;
				center.y = 0;

				//cout << "Trace X " << (*i).x << " y " << "Y " << (*i).y << endl;
				CvPoint pleft = traceline(middle, cvPoint(-1, 0), orange);
				if (validpixel(pleft.x,pleft.y))
					points.push_back(pleft);

				CvPoint pright = traceline(middle, cvPoint(1, 0), orange);
				if (validpixel(pright.x,pright.y))
					points.push_back(pright);

				CvPoint topright = traceline(middle, cvPoint(1, -1), orange);
				if (validpixel(topright.x,topright.y))
					points.push_back(topright);

				CvPoint topleft = traceline(middle, cvPoint(-1, -1), orange);
				if (validpixel(topleft.x,topleft.y))
					points.push_back(topleft);

				CvPoint bottomright = traceline(middle, cvPoint(1, 1), orange);
				if (validpixel(bottomright.x,bottomright.y))
					points.push_back(bottomright);

				CvPoint bottomleft = traceline(middle, cvPoint(-1, 1), orange);
				if (validpixel(bottomleft.x,bottomleft.y))
					points.push_back(bottomleft);
				//TODO: Something smarter? like circle from 3 points?
				//Iterate for center
				for (unsigned int j = 0; j < points.size(); j++)
				{
					center.x += points[j].x;
					center.y += points[j].y;
				}
				center.x /= points.size();
				center.y /= points.size();
				float radius = 0;
				//cout << "Find Center:" << center.x << " " << center.y << " " << endl;
				//Iterate for radius
				for (unsigned int j = 0; j < points.size(); j++)
				{
					radius += CvDist(center,points[j]);//sqrt((center.x-points[i].x)*)(center.x-points[i].x)+(center.y-points[i].y)*)(center.y-points[i].y));
					//center.y+=points[i].y;
				}
				radius /= points.size();
		*/
		cout<<radius<<endl;
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
		KMat::HCoords<int,2> im1,im2;
		im1(0)=center.x;
		im1(1)=center.y;
		im2(0)=center.x+(radius)*0.707;
		im2(1)=center.y+(radius)*0.707;
		c1=imageToCamera(im1);
		c2=imageToCamera(im2);
		float tantths=kinext.vectorAngle(c1,c2);
		c3d=kinext.camera2dToGround(c1);//Transform center vector  to ground coordinates
		double par=tantths*tantths*( (sqrd(c3d(0))+sqrd(c3d(1)))/sqrd(c3d(2)))+tantths*tantths;
		if(par>1)//Something is terribly wrong!
		{
			Logger::Instance().WriteMsg("Vision", "Ball size estimation went boggus!", Logger::Error);
			continue;
		}
		float rest=p.cameraZ*(par-sqrt(par))/(par-1);
		float balltolerance,ballsize;
		config->QueryElement("balltolerance",balltolerance);
		config->QueryElement("ballsize",ballsize);
		newdata.ballradius=rest;
		//cout<<"rest:"<<rest<<endl;
		if(abs( (rest*2-ballsize)/ballsize)>balltolerance)//Wrong diameter ball
		{
			//Logger::Instance().WriteMsg("Vision", "Ball size estimation check failed", Logger::Error);
			continue;
		}
		if(!calculateValidBall(newdata,(KSegmentator::colormask_t) orange))
			continue;
		measurement d1=kinext.angularDistance(c1,c2,rest);
		//cout<<"angular:"<<d1.mean<<" "<<d1.var<<endl;
		measurement2 a=kinext.projectionDistance(c1,rest);
		measurement d2=(*a)[0];
		//cout<<"projection:"<<d2.mean<<" "<<d2.var<<endl;
		measurement bearing=(*a)[1];
		//cout<<"proj bearing:"<<bearing.mean<<" "<<bearing.var<<endl;
		measurement distance;

		distance.mean=(d1.mean*d2.var+d2.mean*d1.var)/(d1.var+d2.var);
		distance.var=d1.var*d2.var/(d1.var+d2.var);
		KMat::HCoords<float,2> polar;
		polar(0)=distance.mean;
		polar(1)=bearing.mean;
		KMat::HCoords<float,2> robotpolar=camToRobot(polar);
		distance.mean=polar(0);
		bearing.mean=polar(1);
		newdata.distance=distance;
		newdata.bearing=bearing;


		history.push_back(newdata);

		delete[] a;
		//cout<<history<<endl;

	}
	vector<balldata_t>::iterator bd = history.begin();
	balldata_t best;
	best.cr = 0;
	best.distance.var =10000;
	///debugcout << "BEST found" << endl;
	while (bd != history.end())
	{
		//cout << best.x << " " << best.y << " "<<best.d<< endl;
		//cout << (*bd).x << " " << (*bd).y << " "<<(*bd).d<< endl;
		//cout<<(*bd).d<<endl;
		if ((*bd).distance.var < best.distance.var )
			best = *bd;
		//cout << best.x << " " << best.y << " "<<best.d<< endl;
		bd++;
	}
	/*
	#ifdef DEBUGVISION
	        //KMat::HCoords<float,2> & w=camToRobot(o)
	        cout<<"Bearing:"<<best.bearing.mean<<" "<<best.bearing.var<<endl;
	        cout<<"Distance:"<<best.distance.mean<<" "<<best.distance.var<<endl;

	#endif*/
	return best;


}

CvPoint Vision::traceline(CvPoint start, CvPoint vel, KSegmentator::colormask_t c)
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
			break;
		curr.x += vel.x;
		curr.y += vel.y;

	}
	if (!validpixel(latestValid.x,latestValid.y))
	{
		latestValid.x = -1;
		latestValid.y = -1;
	}
	//cout<<"ret"<<latestValid.x<<" "<<latestValid.y<<endl;
	return latestValid;
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

	/*std:: vector <std::string> names;
	 std:: vector <float> pos;
	 names.push_back("HeadYaw");
	 names.push_back("HeadPitch");
	 pos=m->call<vector<float> >("getAngles",names,true);
	 pos[0]=pos[0]+0.1;
	 pos[1]=pos[1]+0.1;
	 m->callVoid("setAngles",names,pos,0.8);*/
	int k=cvWaitKey(10);
	if(k=='s')
	{
		saveFrame(rawImage);
		Logger::Instance().WriteMsg("Vision", "Save frame", Logger::Error);
	}
	if(k=='c')
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


