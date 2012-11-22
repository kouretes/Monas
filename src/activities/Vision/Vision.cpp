#include "Vision.h"

#include "architecture/archConfig.h"
#include <cmath>
#include "sys/stat.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/KMat.h"
#include "hal/syscall.h"
#include <vector>

#include "hal/robot/generic_nao/robot_consts.h"

#include <vector>

#define TO_RAD 0.017453293f

#define trydelete(x) {if((x)!=NULL){delete (x);(x)=NULL;}}

using namespace std;

ACTIVITY_REGISTER(Vision);

ACTIVITY_START

Vision::Vision(Blackboard &b, XmlManager &x) :
	IActivity(b, x), vprof("Vision")
{
	;
}

void Vision::UserInit()
{
	kinext.Init();
	//Logger::Instance().WriteMsg("Vision", "ext.allocateImage()", Logger::Info);
	//cout << "Vision():" ;//<< endl;
	//rawImage = ext.allocateImage();
	_blk.updateSubscription("sensors", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk.updateSubscription("image", msgentry::SUBSCRIBE_ON_TOPIC);
	segbottom = NULL;
	segtop = NULL;
	Reset();
}

void Vision::Reset(){
	ifstream *conffile = new ifstream((ArchConfig::Instance().GetConfigPrefix() + "colortables/" + _xml.findValueForKey("vision.SegmentationBottom")).c_str());
	if(segbottom != NULL){
		delete segbottom;
		segbottom = NULL;	
	}
	segbottom = new KSegmentator(*conffile);
	conffile->close();
	delete conffile;

	if(_xml.findValueForKey("vision.SegmentationTop") == _xml.findValueForKey("vision.SegmentationBottom")) //Same file, do not load twice
		segtop = segbottom;
	else
	{
		conffile = new ifstream((ArchConfig::Instance().GetConfigPrefix() + "colortables/" + _xml.findValueForKey("vision.SegmentationTop")).c_str());
		if(segtop != NULL){
			delete segtop;
			segtop = NULL;	
		}
		segtop = new KSegmentator(*conffile);
		conffile->close();
		delete conffile;
	}

	stamp = boost::posix_time::microsec_clock::universal_time();
	seg = segbottom;
	config.sensordelay =  atoi(_xml.findValueForKey("vision.sensordelay").c_str());
	config.Dfov = atof(_xml.findValueForKey("vision.Dfov").c_str());
	config.cameraGamma = atof(_xml.findValueForKey("vision.cameraGamma").c_str());
	config.scanV = atoi(_xml.findValueForKey("vision.scanV").c_str());
	config.scanH = atoi(_xml.findValueForKey("vision.scanH").c_str());
	config.minH = atoi(_xml.findValueForKey("vision.minH").c_str());
	config.bordersize =atoi( _xml.findValueForKey("vision.bordersize").c_str());
	config.pixeltol = atoi(_xml.findValueForKey("vision.pixeltol").c_str());
	config.subsampling = atoi(_xml.findValueForKey("vision.subsampling").c_str());
	config.skipdistance = atof(_xml.findValueForKey("vision.skipdistance").c_str());
	config.seedistance = atof(_xml.findValueForKey("vision.seedistance").c_str());
	config.obstacledistance = atof(_xml.findValueForKey("vision.obstacledistance").c_str());
	config.balltolerance = atof(_xml.findValueForKey("vision.balltolerance").c_str());
	config.ballsize = atof(_xml.findValueForKey("vision.ballsize").c_str());
	config.goalheight = atof(_xml.findValueForKey("vision.goalheight").c_str());
	config.goaldist = atof(_xml.findValueForKey("vision.goaldist").c_str());
	config.widthestimateotolerance = atof(_xml.findValueForKey("vision.widthestimateotolerance").c_str());
	config.pitchoffset = atof(_xml.findValueForKey("vision.pitchoffset").c_str());
	config.pixeltol = atof(_xml.findValueForKey("vision.pixeltol").c_str());
	Logger::Instance().WriteMsg("Vision", "Reset done", Logger::Warning);
}

int  Vision::Execute()
{
	//cout<<"Vision Execute"<<endl;
	try
	{
		fetchAndProcess();
	}
	catch(KMath::KMat::SingularMatrixInvertionException &e)
	{
		Logger::Instance().WriteMsg("Vision", "Holy mother of jesus", Logger::Warning);
		return 0;
	}

#ifdef KPROFILING_ENABLED
	vprof.generate_report();
#endif
	return 0;
}

void Vision::fetchAndProcess()
{
	leds.Clear();
	obs.Clear();
	//unsigned long startt = SysCall::_GetCurrentTimeInUSec();
	boost::posix_time::ptime oldstamp = stamp;
	boost::shared_ptr<const KRawImage> img = _blk.readData<KRawImage> ("image", msgentry::HOST_ID_LOCAL_HOST, &stamp);
	if(stamp <= oldstamp)
		return ;

	if(img.get() == 0)
		return;
//	cout << "haveimage" << endl;

	//Remove constness, tricky stuff :/
	rawImage.copyFrom(img->image_rawdata().data(),
	                  img->width(), img->height(), img->bytes_per_pix());
	obs.set_image_timestamp(boost::posix_time::to_iso_string(stamp));
	//unsigned long endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"Fetch image takes:"<<endt<<endl;
	stamp += boost::posix_time::millisec(config.sensordelay);

	if (img->active_camera() == KRawImage::BOTTOM)//bottom cam
	{
		//Get Kinematics first!
		std::vector<float> val = kinext.getKinematics("CameraBottom");
		p.cameraPitch = (KMath::KMat::transformations::PI * 40.0) / 180.0;
		//cout<<"CameraPitch:"<<cameraPitch<<endl;
		seg = segbottom;
		p.cameraX = val[0];
		p.cameraY = val[1];
		p.cameraZ = val[2];//3rd element
	}
	else
	{
		//Get Kinematics first!
		std::vector<float> val = kinext.getKinematics("CameraTop");
		p.cameraX = val[0];
		p.cameraY = val[1];
		p.cameraZ = val[2];//3rd element
		p.cameraPitch = 0;
		seg = segtop;
	}

	seg->setLumaScale(pow(img->luminance_scale(), config.cameraGamma) );
	//std::cout<<img->luminance_scale()<<std::endl;
	//cout<<"Attach to Image:"<<seg<<rawImage<<endl;
	seg->attachToIplImage(rawImage);//Make segmentator aware of a new image
	//saveFrame(rawImage);
	//return;
	asvmo = _blk.readData<AllSensorValuesMessage> ("sensors", msgentry::HOST_ID_LOCAL_HOST, &timeo, &stamp, Blackboard::DATA_NEAREST_NOTNEWER);
	asvmn = _blk.readData<AllSensorValuesMessage> ("sensors", msgentry::HOST_ID_LOCAL_HOST, &timen, &stamp, Blackboard::DATA_NEAREST_NOTOLDER);
#ifdef DEBUGVISION
	cout << "ImageTimestamp:" << boost::posix_time::to_iso_string(stamp) << endl;
	cout << "Now:" << boost::posix_time::microsec_clock::universal_time() << endl;
	cout << "SensorTimestamp:" << boost::posix_time::to_iso_string(timeo) << "," << boost::posix_time::to_iso_string(timen) << endl;
	//	boost::posix_time::ptime t;
	//	_blk.readData<AllSensorValuesMessage> ("sensors", msgentry::HOST_ID_LOCAL_HOST, &t);
	//	cout << "Lasttimestamp:"<< boost::posix_time::to_iso_string(t) << endl;
#endif

	if (asvmn.get() == NULL || asvmo.get() == NULL) //No sensor data!
	{
		Logger::Instance().WriteMsg("Vision", "Warning!!! Vision has no head joint msg in all sensor (allsm) data!", Logger::Error);
		return;
	}

	float imcomp = ( (float ) ((stamp - timeo).total_nanoseconds()) ) / 1000000000.0;
	assert(imcomp >= 0);
	//each value is (n-o)*(stamp-timeo) +o
	p.yaw = asvmo->jointdata(KDeviceLists::HEAD + KDeviceLists::YAW).sensorvalue();
	p.yaw += (asvmn->jointdata(KDeviceLists::HEAD + KDeviceLists::YAW).sensorvalue() - p.yaw) * imcomp;
	p.pitch = asvmo->jointdata(KDeviceLists::HEAD + KDeviceLists::PITCH).sensorvalue();
	p.pitch += (asvmn->jointdata(KDeviceLists::HEAD + KDeviceLists::PITCH).sensorvalue() - p.pitch) * imcomp;
	//p.Vyaw = asvmo->jointdata(KDeviceLists::HEAD+KDeviceLists::YAW).sensorvaluediff();
	//p.Vpitch = asvmo->jointdata(KDeviceLists::HEAD+KDeviceLists::PITCH).sensorvaluediff();
	p.angX = asvmo->computeddata(KDeviceLists::ANGLE + KDeviceLists::AXIS_X).sensorvalue();
	p.angX += (asvmn->jointdata(KDeviceLists::HEAD + KDeviceLists::AXIS_X).sensorvalue() - p.angX) * imcomp;
	p.angY = asvmo->computeddata(KDeviceLists::ANGLE + KDeviceLists::AXIS_Y).sensorvalue();
	p.angY += (asvmn->jointdata(KDeviceLists::HEAD + KDeviceLists::AXIS_Y).sensorvalue() - p.angY) * imcomp;
	p.angY += config.pitchoffset;
	//p.VangX = asvm->computeddata(KDeviceLists::ANGLE+KDeviceLists::AXIS_X).sensorvaluediff();
	//p.VangY = asvm->computeddata(KDeviceLists::ANGLE+KDeviceLists::AXIS_Y).sensorvaluediff();
	//p.timediff = asvm->timediff();//Get time from headmessage
#ifdef DEBUGVISION
	cout << p.yaw << " " << p.pitch << " " << p.angX << " " << p.angY << imcomp << endl;
#endif
	p.focallength = sqrt(sqrd(rawImage.width) + sqrd(rawImage.height) ) / (2 * tan(config.Dfov * TO_RAD / 2));
	//Logger::Instance().WriteMsg("Vision", _toString("Focal Length ")+_toString(p.focallength), Logger::Error);
	kinext.setPose(p);
	KMath::KMat::transformations::makeRotation(simpleRot, -kinext.getRoll());
	//Now change y axis :)
	simpleRot(0, 1) = -simpleRot(0, 1);
	simpleRot(1, 1) = -simpleRot(1, 1);
	//Weird stuff. simpleRot-^-1=simpleRot'=simpleRot. therefore translaton from raw image to rotated and axis inverted
	//image and back is done by simpleRot
	//
	//		Vup.y = -cos(-kinext.getRoll());
	//		Vup.x = sin(-kinext.getRoll());
	Vup = simpleRot.slow_mult(KVecFloat2(0, 1));
	Vdn = simpleRot.slow_mult(KVecFloat2(0, -1));
	Vlt = simpleRot.slow_mult(KVecFloat2(-1, 0));
	Vrt = simpleRot.slow_mult(KVecFloat2(1, 0));
	KVecFloat2 c2d;
	KVecFloat3 c3d;
	//Publish the projection of the camera limits on the ground
	PointObject *p1;
	c2d = imageToCamera(KVecFloat2(0, 0));
	c3d = kinext.camera2dToGround(c2d);

	if(c3d(2) < 0) //Not Looking up :)
	{
		p1 = obs.add_view_limit_points();
		measurement *m = kinext.projectionDistance(c2d, 0);
		p1->set_distance(m[0].mean);
		p1->set_bearing(m[1].mean);
		//cout<<"See bot:"<<newpost.distBot.mean<<endl;
		delete[] m;
	}

	c2d = imageToCamera(KVecFloat2(0, rawImage.height));
	c3d = kinext.camera2dToGround(c2d);

	if(c3d(2) < 0) //Not Looking up :)
	{
		p1 = obs.add_view_limit_points();
		measurement *m = kinext.projectionDistance(c2d, 0);
		p1->set_distance(m[0].mean);
		p1->set_bearing(m[1].mean);
		//cout<<"See bot:"<<newpost.distBot.mean<<endl;
		delete[] m;
	}

	c2d = imageToCamera(KVecFloat2(rawImage.width, rawImage.height));
	c3d = kinext.camera2dToGround(c2d);

	if(c3d(2) < 0) //Not Looking up :)
	{
		p1 = obs.add_view_limit_points();
		measurement *m = kinext.projectionDistance(c2d, 0);
		p1->set_distance(m[0].mean);
		p1->set_bearing(m[1].mean);
		//cout<<"See bot:"<<newpost.distBot.mean<<endl;
		delete[] m;
	}

	c2d = imageToCamera(KVecFloat2(rawImage.width, 0));
	c3d = kinext.camera2dToGround(c2d);

	if(c3d(2) < 0) //Not Looking up :)
	{
		p1 = obs.add_view_limit_points();
		measurement *m = kinext.projectionDistance(c2d, 0);
		p1->set_distance(m[0].mean);
		p1->set_bearing(m[1].mean);
		//cout<<"See bot:"<<newpost.distBot.mean<<endl;
		delete[] m;
	}

	/*Vdn.x = -Vup.x;
	Vdn.y = -Vup.y;

	Vlt.x = Vup.y;//-Vup.x*cos(-45*TO_RAD)-Vup.y*sin(-45*TO_RAD);
	Vlt.y = -Vup.x;//-Vup.x*sin(-45*TO_RAD)+Vup.y*sin(-45*TO_RAD);

	Vrt.x = -Vup.y;
	Vrt.y = Vup.x;*/
	//unsigned long startt,endt;
	//startt=SysCall::_GetCurrentTimeInUSec();
	ballpixels.clear();
	ygoalpost.clear();
	bgoalpost.clear();
	obstacles.clear();
	tobeshown.clear();
	gridScan(orange);
	//endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"GridScan takes:"<<endt<<endl;
	//startt=SysCall::_GetCurrentTimeInUSec();
	balldata_t b = locateBall(ballpixels);
	//endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"locateball takes:"<<endt<<endl;
	//unsigned long endt = SysCall::_GetCurrentTimeInUSec()-startt;
	//cout<<"locateball takes:"<<endt<<endl;
	//cout<<b.r<<endl;
	locateGoalPost(ygoalpost, yellow);
	//locateGoalPost(bgoalpost, skyblue);
#ifdef DEBUGVISION
	cout << "Ballpixelsize:" << ballpixels.size() << endl;
	//cout << b.x << " " << b.y << " " << b.cr << endl;
	//KVecFloat2 & w=camToRobot(o)
	cout << "Bearing:" << b.bearing.mean << " " << b.bearing.var << endl;
	cout << "Distance:" << b.distance.mean << " " << b.distance.var << endl;
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
		float w = b.distance.mean * 3;
		w = w > 1 ? 1 : w;
		trckmsg.set_referenceyaw(yaw * w);
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
	}
	else
	{
		trckmsg.set_cx(b.x);
		trckmsg.set_cy(b.y);
		trckmsg.set_radius(-1);
		//trckmsg.set_topic("vision");
		LedValues* l = leds.add_leds();
		l->set_chain("l_eye");
		l->set_color("white");
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

		if (obs.regular_objects_size() == 1)
		{
			l->set_color("yellow");
		}
		else if (obs.regular_objects_size() == 2)
		{
			l->set_color("purple");
		}
	}
	else
	{
		LedValues* l = leds.add_leds();
		l->set_chain("r_eye");
		l->set_color("black");
	}

	_blk.publishSignal(trckmsg, "vision");
	_blk.publishSignal(leds, "leds");
	_blk.publishSignal(obs, "vision");
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

	_blk.publishSignal(result, "obstacle");
}

KVecFloat2 Vision::simpleRotation(KVecFloat2 const& i) const
{
	return simpleRot.slow_mult(i);
}

KVecFloat2 Vision::simpleRotation(KVecInt2 const& i) const
{
	return simpleRot.slow_mult(KVecFloat2(i.x, i.y));
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
	res(1) = (int) - (c(1) - rawImage.height / 2.0 + 0.5);
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

ACTIVITY_END
