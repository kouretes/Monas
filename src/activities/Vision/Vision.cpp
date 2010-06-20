#include "Vision.h"
#include "architecture/archConfig.h"
#include <cmath>
#include "sys/stat.h"
#include "tools/logger.h"
#include "tools/XMLConfig.h"

#define trydelete(x) {if((x)!=NULL){delete (x);(x)=NULL;}}

#define inbounds(x,y) ( ((x)>0 &&(y)>0)&&((x)<rawImage->width-1&&(y)<rawImage->height-1) )
#define CvDist(pa,pb) sqrt(((pa).x-(pb).x )*((pa).x-(pb).x )+((pa).y-(pb).y )*((pa).y-(pb).y ) )
//Distance from observation angle
#define angularDistance(Hr,Ho,theta) ( ((Hr)-(Ho))*kinext.cot(theta) )
#define apparentDistance(Hr,Ho,rad) ( sqrt ( ( (((Ho)*(Ho) ) -((Hr)*(Hr)*tan(rad)*tan(rad)))*kinext.cot(rad)*kinext.cot(rad)    )  ))

#define BALLRADIUS 0.03f



#define COVERDIST 0.03
#define TRACESKIP 5
#define GLOBALTRACESKIP 25
//x COVERDIST=15*0.05m
#define SCANSKIP  3
#define BORDERSKIP 2

#define  VFov 34.8f
#define  HFov 46.4f
#define TO_RAD 0.01745329f
using namespace AL;
using namespace std;
//using namespace boost::posix_time;


namespace
{
	ActivityRegistrar<Vision>::Type temp("Vision");
}



//#include "kobserver_goalrecognition.cpp"
bool FileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
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
   	}while(FileExists(fname));
   	ofstream frame(fname,ios_base::binary);
   	frame.write(reinterpret_cast<char *>(fIplImageHeader->imageData),fIplImageHeader->width*fIplImageHeader->height*fIplImageHeader->nChannels);
   	frame.close();


}


int  Vision::Execute()
{
	static bool calibrated = false;
	_blk->process_messages();
    CalibrateCam *cal= _blk->in_msg_nb<CalibrateCam>("CalibrateCam");
    if(cal!=NULL)
    {
        if( cal->status()==0)
        {
            //cout<<"Start calibration"<<endl;
            Logger::Instance().WriteMsg("Vision", "Start calibration", Logger::Info);
            float scale= ext.calibrateCamera(cal->sleeptime(),cal->exp());
            segbottom->setLumaScale(1/scale);
            segtop->setLumaScale(1/scale);
            Logger::Instance().WriteMsg("Vision", "Calibration Done", Logger::Info);
            //cout<<"Calibration Done!"<<endl;
            calibrated = true;
            cal->set_status(1);
            cal->set_exposure_comp(scale);
            publish(cal,"vision");
            delete cal;
            return 0;

        }
        else
        {
            delete cal;
            Logger::Instance().WriteMsg("Vision", "WTF", Logger::FatalError);
        }
    }

	testrun();
	//std::cout << " Vision run" << std::endl;

	return 0;
}

void Vision::testrun()
{
	trydelete(im);
	trydelete(hm);
	im = _blk->read_nb<InertialSensorsMessage>("InertialSensorsMessage", "Sensors");

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

	//cout << "fetchImage" << endl;
	boost::posix_time::ptime stamp = ext.fetchImage(rawImage);


	if (ext.getCamera()==1)//bottom cam
	{
		cameraPitch=(KMat::transformations::PI*40.0)/180.0;
		//cout<<"CameraPitch:"<<cameraPitch<<endl;
		seg=segbottom;
		//Get Kinematics first!
		std::vector<float> val = kinext.getKinematics("CameraBottom");
		cameraX=val[0];
		cameraY=val[1];
		cameraH=val[2];//3rd element

	}
	else
	{
		cameraPitch=0;
		seg=segtop;
		//Get Kinematics first!
		std::vector<float> val = kinext.getKinematics("CameraTop");
		cameraX=val[0];
		cameraY=val[1];
		cameraH=val[2];//3rd element
	}
#ifdef DEBUGVISION
	cout << "ImageTimestamp:"<< boost::posix_time::to_iso_string(stamp) << endl;
#endif
	//boost::posix_time::ptime rtime =  time_t_epoch+(boost::posix_time::microsec(t.tv_nsec/1000)+boost::posix_time::seconds(t.tv_sec));//+sec(t.tv_sec));

	hm = _blk->read_nb<HeadJointSensorsMessage>("HeadJointSensorsMessage", "Sensors","localhost",&p.time,&stamp);//,&rtime);
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
	//TODO: create ct!;
	p.yaw=hm->sensordata(0).sensorvalue();
	p.pitch=hm->sensordata(1).sensorvalue();

	p.Vyaw=hm->sensordata(0).sensorvaluediff();
	p.Vpitch=hm->sensordata(1).sensorvaluediff();

	p.angX=im->sensordata(5).sensorvalue();
	p.angY=im->sensordata(6).sensorvalue();
	p.VangX=im->sensordata(5).sensorvaluediff();//im->sensordata(5).sensortimediff();
	p.VangY=im->sensordata(6).sensorvaluediff();//im->sensordata(6).sensortimediff();

	p.timediff=hm->sensordata(0).sensortimediff();//Get time from headmessage
	//p.time = time_t_epoch;

	//float exptime=ext.getExp();//Compensate for middle of image
	//float imcomp=(exptime*1000.0)/p.timediff;

	float imcomp=((stamp-p.time).total_microseconds()*250.0)/p.timediff;
	cout<<"imcomp:"<<imcomp<<endl;

	cout<< p.timediff<<" "<<p.angX<<" "<<p.angY<<" "<<p.VangX<<" "<<p.VangY<< " "<<endl;
	//Estimate the values at excactly the timestamp of the image
	p.yaw+=p.Vyaw*imcomp;
	p.pitch+=p.Vpitch*imcomp;
	p.angX+=p.VangX*imcomp;
	p.angY+=p.VangY*imcomp;
	cout<< p.timediff<<" "<<p.angX<<" "<<p.angY<<" "<<p.VangX<<" "<<p.VangY<< " "<<endl;
	//Now use transformations to use the angX,angY values in the image
	KMat::ATMatrix<float,4> y,z;
	KMat::transformations::rotateY(y,p.pitch+cameraPitch);
	KMat::transformations::rotateZ(z,p.yaw);
	z.mult(y).invert();

	//========== Create Corrections from torso TODO: this should be kinematics...
	KMat::HCoords<float,3> angstart;
	angstart.zero();
	angstart(1)=p.angX;
	angstart(2)=p.angY;
	trydelete(ang);

	ang=&z.transform(angstart);
#ifdef DEBUGVISION
	ang->prettyPrint();
#endif

	angstart.zero();
	angstart(1)=p.VangX;
	angstart(2)=p.VangY;
	trydelete(Vang);
	Vang=&z.transform(angstart);
	horizonAlpha=p.pitch+(*ang)(2)+cameraPitch;//When in camera coords (symbolic) shift y by this to get to horizon
#ifdef DEBUGVISION
	Vang->prettyPrint();
#endif

	//=== Create Image Correction tranfsformation  ===
	//Ct: convert from symbolic coords to image ones!!

	//Notice that the same value is used to cancel out image rotation (and NOT the inverse one)
	//Since for the x-y field of the camera the counterclockwise rotation (+) is a negative in the xyz one of the camera
	KMat::transformations::rotate(ct,(*ang)(1));//Rotation about X axis is directly applied to image
	//ct.invert();//TODO: enable this for the above, now its the inverse ! image to symbolic
	//SleepMs(1000);
	//usleep(500);
	gridScan(orange);
	if (cvHighgui)
		cvShowSegmented();




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

Vision::Vision() :Publisher("Vision"),Subscriber("Vision"),cvHighgui(false), type(VISION_CSPACE),ang(NULL),Vang(NULL),im(NULL),hm(NULL)
{

}

void Vision::UserInit()
{
    config = new XMLConfig(ArchConfig::Instance().GetConfigPrefix()+"/vision.xml");
    if(config->IsLoadedSuccessfully()==false)
        Logger::Instance().WriteMsg("Vision", "vision.xml Not Found", Logger::FatalError);
    config->QueryElement("cvHighgui",cvHighgui);
    if(cvHighgui==true)
        Logger::Instance().WriteMsg("Vision", "Enable highgui", Logger::Info);


	ext.Init(_com);
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

	ifstream *config = new ifstream((ArchConfig::Instance().GetConfigPrefix()+"/segbot.conf").c_str());
	segbottom = new KSegmentator(*config);//TODO PATH!!!
	config->close();
	delete config;
	config = new ifstream((ArchConfig::Instance().GetConfigPrefix()+"/segtop.conf").c_str());

	segtop = new KSegmentator(*config);//TODO PATH!!!
	config->close();
	delete config;


	cout<<"Add Subscriber-publisher"<<endl;
	_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->add_publisher(this);




}
void Vision::gridScan(const KSegmentator::colormask_t color)
{
	//Horizontal + Vertical Scan
	ballpixels.clear();
	ygoalpost.clear();
	bgoalpost.clear();


	CvPoint tmpPoint;


	//int points[rawImage->width];
	int step = 2;
	int ystep = 2;

	KSegmentator::colormask_t tempcolor;
	//int ballpixel = -1;
	unsigned int ballthreshold = 1;
	unsigned int goalposthreshold= 2;
	unsigned int cntwhitegreenpixels = 0;
	unsigned int cntwhitegreenorangepixels=0;
	unsigned int cntother=0;
	int j;
	//For horizon
	KMat::HCoords<float,2> point;
	bool havehorizon=true;
	float tantheta,beta;
	float d=1;//Ray distance
	float hory;
	if (abs((*ang)(1))==KMat::transformations::PI/2.0)//90 angle!!!!!
	{
		Logger::Instance().WriteMsg("Vision", "The robot seems to be taking a nap :/", Logger::Error);
		havehorizon=false;
		hory=-BORDERSKIP;
	}
	else
	{
		tantheta=tan((*ang)(1));
		beta=abs(horizonAlpha)/cos((*ang)(1));

	}
	int refresh=0;
	//cout<<"loop"<<endl;
	for (int i = BORDERSKIP; i < rawImage->width-BORDERSKIP; i = i + step)
	{
		//Thru Horizon Possibly someday
		//cout<<"outer"<<endl;
		cntwhitegreenpixels = 0;
		cntwhitegreenorangepixels=0;
		cntother=0;
		// ballpixel = -1;

		//TOOD ystep
        point(1)=i;
        point(2)=rawImage->height - BORDERSKIP;
        KMat::HCoords<float,2> &a=imageTocamera(point);
        KMat::HCoords<float,2> & b=cameraToObs(a);
        delete &a;
        //tempcolor = doSeg(i, rawImage->height - BORDERSKIP);

        d=angularDistance(cameraH,0,b(2));
        delete &b;
        //cout<<"distance:"<<d<<endl;

        float thou=atan2((double)d+COVERDIST,(double)cameraH);
        float fou=atan2((double)d,(double)cameraH);
        float diff=thou-fou;
        ystep=cos((*ang)(1))*(diff/(VFov * TO_RAD)+0.5)*rawImage->height;//Crossyour fingers
        ystep=ystep/2;
        if (ystep<=0) ystep=1;

        //cout<<ystep<<endl;
        if (havehorizon)
        {
            KMat::HCoords<float,2> &c= imageTocamera(point);
            hory=-(tantheta*c(1)+beta);
            hory=(hory/(VFov * TO_RAD)+0.5)*rawImage->height;
            //cout<<"hory"<<hory<<endl;
            delete &c;
        }
        if(hory<=0) hory=-BORDERSKIP;




		for (j = rawImage->height - BORDERSKIP-ystep; j > hory&& j> BORDERSKIP; j = j - ystep)
		{
			//cout<<"inner"<<endl;
			//image start from top left
			tempcolor = doSeg(i, j);
            point(1)=i;
            point(2)=j;
			//point.prettyPrint();
			//Reached horizon?, break

			if (havehorizon)
			{

				if (hory+2>=j)//Found horizon
				{
                    cout<<"Horizon"<<hory+2<<endl;
				    tempcolor = doSeg(i, hory-1);
					if (tempcolor==yellow && cntwhitegreenorangepixels >= goalposthreshold)
					{
						tmpPoint.x = i;
						tmpPoint.y = j;
						ygoalpost.push_back(tmpPoint);
						//cntwhitegreenorangepixels=0;
					}
					if (tempcolor==skyblue&& cntwhitegreenorangepixels >= goalposthreshold)
					{
						tmpPoint.x = i;
						tmpPoint.y = j;
						bgoalpost.push_back(tmpPoint);
						//cntwhitegreenorangepixels=0;
					}

					break;
				}


			}

			if (tempcolor == green)//tempcolor == white ||
			{
				cntwhitegreenpixels++;
				cntwhitegreenorangepixels++;
				cntother=0;
			}
			else if (tempcolor==orange)
			{
				cntwhitegreenorangepixels++;
				cntother=0;
			}
			else
				cntother++;
			if (cntother>SCANSKIP)//No continuity, break
				break;
			if (tempcolor == orange && cntwhitegreenpixels >= ballthreshold)
			{
				tmpPoint.x = i;
				tmpPoint.y = j;
				ballpixels.push_back(tmpPoint);
				cntwhitegreenpixels=0;
				//continue;
				//ballpixel = j;
			}


			ystep=(ystep/2);
			//cout<<ystep<<endl;
			if(ystep<=0) ystep=1;




		}
		//KMat::HCoords<float,2> &=imageTocamera(point);
		//KMat::HCoords<float,2> & a=cameraToObs(point);

		//float xdist=angularDistance(cameraH,0,a(2));
		//float xangle=atan2(COVERDIST,d*2)*2;
		//step=cos((*ang)(1))*xangle;
		//cout<<"xstep"<<step<<endl;
		//if (step<=0) step=1;
		//cout<<"xstep"<<step<<endl;
		//TODO xstep;
	}

	balldata_t b = locateBall(ballpixels);

#ifdef DEBUGVISION
	cout << "Ballpixelsize:" << ballpixels.size() << endl;
	cout << b.x << " " << b.y << " " << b.r << endl;
#endif

	float x = (-0.5 + (double) (b.x-0.5f) / (double) rawImage->width) * HFov * TO_RAD;
	float y = (0.5 - (double) (b.y-0.5f) / (double) rawImage->height) * VFov * TO_RAD;
	float r = b.r * (HFov / (double) rawImage->width)* TO_RAD;



	if (b.r > 0)
	{
		KMat::HCoords<float,2> t,target,o;
		t(1)=x-p.yaw;//-(*ang)(3);
		t(2)=y;//-horizonAlpha;
		//ball=ct.transform(t);
		KMat::HCoords<float,2> & ball=cameraToObs(t);

		//float totalangle=-ball(2)+p.pitch+(*ang)(2);//Angle on the image + headpitch+ rotation due to torso along pitch axis
		//ang->prettyPrint();
		o(2)=ball(1);//Bearing: from image to camera
		o(1)=angularDistance(cameraH,BALLRADIUS,ball(2));//From angle to distance;

        KMat::HCoords<float,2> & w=camToRobot(o);
#ifdef DEBUGVISION
        //KMat::HCoords<float,2> & w=camToRobot(o);
		cout<<"Distance bearing"<<endl;
		w.prettyPrint();

		cout<<"Bearing:"<<ball(2)<<endl;
		cout<<"AngularDistance: "<< angularDistance(cameraH,BALLRADIUS,ball(2))<< " "<<ball(2)<<endl;
		cout<<"ApparentDistance"<<apparentDistance(cameraH,BALLRADIUS , ((b.r+0.5)*  HFov * TO_RAD )/((float)rawImage->width ))<<endl;

#endif
		//Fill message and publish
#ifdef DEBUGVISION
		cout<<"Vision:Publish Message"<<endl;
#endif
		trckmsg.set_cx(x);
		trckmsg.set_cy(y);
		target=ct.transform(t);
		trckmsg.set_referenceyaw(p.yaw);
		trckmsg.set_referencepitch(p.pitch);//-ball(2)
		trckmsg.set_radius(r);
		//trckmsg.set_topic("vision");
		publish(&trckmsg,"vision");
		ballpos.set_dist(w(1));
		ballpos.set_bearing(w(2));
		ballpos.set_ball_diameter(b.r);
		publish(&ballpos,"vision");
		delete &ball;
		delete &w;





	}
	else
	{


		trckmsg.set_cx(x);
		trckmsg.set_cy(y);
		trckmsg.set_radius(-1);
		//trckmsg.set_topic("vision");
		publish(&trckmsg,"vision");
	}/* else {
		memory->insertData("kouretes/Ball/found", .0f); // change
	}*/
}


KMat::HCoords<float,2> & Vision::imageTocamera( KMat::HCoords<float,2>  & imagep)
{
	KMat::HCoords<float,2> &t= *(new KMat::HCoords<float,2> ());
	t(1)=(-0.5 + (double) (imagep(1)-0.5f) / (double) rawImage->width) * HFov * TO_RAD;
	t(2)=(0.5 - (double) (imagep(2)-0.5f) / (double) rawImage->height) * VFov * TO_RAD;
	return t;
}



KMat::HCoords<float,2> & Vision::cameraToObs(KMat::HCoords<float ,2> const& t)
{
	KMat::HCoords<float,2> &res=ct.transform(t);
	res(1)=-res(1)+p.yaw+(*ang)(3);
	res(2)=-res(2)+horizonAlpha;//p.pitch+(*ang)(2)+cameraPitch;

	return res;

}
//Input:  distance bearing
KMat::HCoords<float,2> & Vision::camToRobot(KMat::HCoords<float ,2> & t)
{
	KMat::HCoords<float,2> & res = * (new KMat::HCoords<float,2>());
	float a=cos(t(2))*t(1);
	float b=sin(t(2))*t(1);
	res(1)=sqrt((a+cameraX)*(a+cameraX)+(b+cameraY)*(b+cameraY));
	res(2)=atan2(b+cameraY,a+cameraX);
	return res;
}



/**
 * Hard Decision: Is it good enough for a ball?
 */
bool Vision::calculateValidBall(const CvPoint2D32f center, float radius, KSegmentator::colormask_t c)
{
	unsigned int ttl = 0, gd = 0;
	float innerrad = radius * 0.707;
	float ratio;
	//Inner circle
	for (int i = center.x - innerrad; i <= center.x + innerrad; i++)
		for (int j = center.y - innerrad; j <= center.y + innerrad; j++)
		{
			if (!inbounds(i,j))
				continue;
			if (doSeg(i, j) == c)
				gd++;
			ttl++;
		}
	ratio = ((float) gd) / ttl;
	if (ratio < 0.8)
		return false;
	//Outer circle
	gd = 0;
	ttl = 0;
	for (int i = center.x - radius; i <= center.x + radius; i++)
		for (int j = center.y - radius; j <= center.y + radius; j++)
		{


			if (!inbounds(i,j))
				continue;
			if (i > center.x - innerrad && i < center.x + innerrad)
				continue;

			if (j > center.y - innerrad && j < center.y + innerrad)
				continue;

			if (doSeg(i, j) == c)
				gd++;
			ttl++;
		}
	ratio = ((float) gd) / ttl;
	if (ratio < 0.35 || ratio > 0.65)
		return false;

	return true;

}
/*
Vision::balldata_t Vision::locateGoalPost(vector<CvPoint> cand, KSegmentator::colormask_t c)
{
    CvPoint2D32f Vup;//Vertical velocity
    Vup.y=-1;
    Vup.x=KCameraTranformation::cot((*ang)(1));

    vector<goalpostdata_t> history;

    vector<CvPoint>::iterator i;//candidate iterator
    for (i = cand.begin(); i != cand.end(); i++)
	{

		vector<goalpostdata_t>::iterator hi = history.begin();
		while (hi != history.end() && candi != cand.end())
		{

		    float left=(-(*i).y+(*hi).ll.y)*Vup.x + (*hi).ll.x-1;
		    float right=(-(*i).y+(*hi).lr.y)*Vup.x + (*hi).lr.x+1;
		    if((*i).>=left&&(*i).x<=right)//inside possible goalpost
		    {
		        i++;//Skip pixel
		        if (i == cand.end())
					break;
				bd = history.begin();
		    }
                hi++;
		}

		if (i == cand.end())
			break;

		if (!inbounds((*i).x,(*i).y))
			continue;

        //===============TRACE DOWN=======
        CvPoint pleft=   traceline((*i), cvPoint(-1, 0), c);
        CvPoint pright = traceline((*i), cvPoint(+1, 0), c);
        if(pleft.x==-1&&pright.x==-1)//WHAT THE HELL
            break;
        if(pleft.x==-1)
        {
            pleft.x=BORDERSKIP;
            pleft.y=(*i).y;
        }

        if(pright.x==-1)
        {
            pright.x=rawImage->width - BORDERSKIP;
            pright.y=(*i).y;
        }

        Cvpoint middle;
        middle.x= (pleft.x+pright)/2;
        middle.y=(*i).y;
        //Trace down
        CvPoint curr = middle;
        CvPoint latestValid = middle;
        int skipcount=0,globalskipcount=0,fieldcount;
        /////cout << "traceline:"<<start.x<<" "<<start.y<<endl;
        while (inbounds(curr.x,curr.y))
        {
            KSegmentator::colormask_t t=doSeg(curr.x, curr.y);
            if (t == c)
            {
                latestValid = curr;
                skipcount = 0;
                fieldcount=0;

            }
            else if (t == white || t == green)
			{
			    fieldcount++;
			}
            else
            {
                skipcount++;
                globalcount++;
            };

            if (skipcount > TRACESKIP || globalcount > GLOBALTRACESKIP)
            {
                if(fieldcount<TRACESKIP/2)
                    latestValid.x=-1;
                break;


            }
            curr.x += middle.x+Vup.x*(-curr.y+middle.y);
            curr.y += 1;

        }
        if(latestValid.x==-1)
            break;
        //============= Done tracing=============



		/////cout << "Wtf" << endl;
		balldata_t newdata;
		newdata.x = center.x;
		newdata.y = center.y;
		newdata.r = radius;
		history.push_back(newdata);

	}

}

CvPoint Vision::sizeTrace(CvPoint start, CvPoint vel, KSegmentator::colormask_t c)
{
	int skipcount = 0;
	int globalcount = 0;
	CvPoint curr = start;
	CvPoint latestValid = start;
	/////cout << "traceline:"<<start.x<<" "<<start.y<<endl;
	while (inbounds(curr.x,curr.y))
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
	//cout<<"ret"<<latestValid.x<<" "<<latestValid.y<<endl;
	return latestValid;
}
*/
Vision::balldata_t Vision::locateBall(vector<CvPoint> cand)
{
	//Skip first/last row/col
	vector<balldata_t> history;

	vector<CvPoint>::iterator i;
	//For all detected points
	//cout << "locateball" << endl;
	for (i = cand.begin(); i != cand.end(); i++)
	{
		vector<CvPoint> points;
		vector<balldata_t>::iterator bd = history.begin();
		while (bd != history.end() && i != cand.end())
		{
			if (CvDist(*bd,*i) < (*bd).r)
			{
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
		if (!inbounds((*i).x,(*i).y))
			continue;
		CvPoint bottom = traceline((*i), cvPoint(0, 1), orange);

		CvPoint top = traceline((*i), cvPoint(0, -1), orange);
		if (!inbounds(top.x,top.y))
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
		if (inbounds(pleft.x,pleft.y))
			points.push_back(pleft);

		CvPoint pright = traceline(middle, cvPoint(1, 0), orange);
		if (inbounds(pright.x,pright.y))
			points.push_back(pright);

		CvPoint topright = traceline(middle, cvPoint(1, -1), orange);
		if (inbounds(topright.x,topright.y))
			points.push_back(topright);

		CvPoint topleft = traceline(middle, cvPoint(-1, -1), orange);
		if (inbounds(topleft.x,topleft.y))
			points.push_back(topleft);

		CvPoint bottomright = traceline(middle, cvPoint(1, 1), orange);
		if (inbounds(bottomright.x,bottomright.y))
			points.push_back(bottomright);

		CvPoint bottomleft = traceline(middle, cvPoint(-1, 1), orange);
		if (inbounds(bottomleft.x,bottomleft.y))
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
		/////cout << "Wtf" << endl;
		balldata_t newdata;
		newdata.x = center.x;
		newdata.y = center.y;
		newdata.r = radius;
		history.push_back(newdata);

	}
	vector<balldata_t>::iterator bd = history.begin();
	balldata_t best;
	best.r = 0;
	///debugcout << "BEST found" << endl;
	while (bd != history.end())
	{
		CvPoint2D32f t;
		t.x = (*bd).x;
		t.y = (*bd).y;
		if ((*bd).r > best.r && calculateValidBall(t, (*bd).r, (KSegmentator::colormask_t) orange))
			best = *bd;
		//cout << best.x << " " << best.y << " " << endl;
		bd++;
	}
	return best;
	/*ALValue ret;

	 ret.arraySetSize(0);
	 if (best.r == 0)
	 return ret;
	 ret.arrayPush("Ball");
	 ret.arrayPush(2 * best.r);
	 vector<std::string> nameyeah;
	 nameyeah.push_back("HeadYaw");
	 vector<float> retangles = motion->call<vector<float> > ("getAngles", nameyeah, true);
	 //double headpitch = retangles[0];// motion->getAngles("HeadPitch",true);
	 ALValue headYawAngle2 = memprxy->getData(AL::ALValue("Device/SubDeviceList/HeadYaw/Position/Sensor/Value"));//motion->getAngles(AL::ALValue("HeadYaw"), true);;//motion->getAngles("HeadYaw",true);
	 float headYawAngle = retangles[0];//motion->getAngles("HeadYaw",true);
	 ///cout << "Head Yaw " << (float) headYawAngle << "Headyaw sketo " << (float)headYawAngle2<< endl;
	 //Bearing Warning not thru horizon! TODO
	 float tempdir = (headYawAngle - (0.5 - (double) (best.x) / (double) width) * HFov * TO_RAD);

	 Acx[orange] = (0.5 - (double) (best.x) / (double) width) * HFov * TO_RAD;
	 Acy[orange] = (0.5 - (double) (best.y) / (double) height) * VFov * TO_RAD;
	 ///cout << " Ball cx, cy" << tempdir << " Ball Diameter pixel" << (float)ret[1]<< endl;

	 ret.arrayPush(tempdir * 1000);
	 return ret;*/

}

CvPoint Vision::traceline(CvPoint start, CvPoint vel, KSegmentator::colormask_t c)
{
	int skipcount = 0;
	int globalcount = 0;
	CvPoint curr = start;
	CvPoint latestValid = start;
	/////cout << "traceline:"<<start.x<<" "<<start.y<<endl;
	while (inbounds(curr.x,curr.y))
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
	if (!inbounds(latestValid.x,latestValid.y))
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
