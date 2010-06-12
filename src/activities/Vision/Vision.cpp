#include "Vision.h"
#include "architecture/archConfig.h"




#define inbounds(x,y) ( ((x)>0 &&(y)>0)&&((x)<rawImage->width-1&&(y)<rawImage->height-1) )
#define CvDist(pa,pb) sqrt(((pa).x-(pb).x )*((pa).x-(pb).x )+((pa).y-(pb).y )*((pa).y-(pb).y ) )
#define MAXSKIP 5
#define GLOBALSKIP  60

using namespace AL;
using namespace std;
//using namespace boost::posix_time;


namespace{
    ActivityRegistrar<Vision>::Type temp("Vision");
}

int  Vision::Execute()
{
	static bool calibrated = false;
	_blk->process_messages();


	if(!calibrated){
		cout<<"Start calibration"<<endl;
		float scale= ext.calibrateCamera();
		segbottom->setLumaScale(1/scale);
		segtop->setLumaScale(1/scale);
		cout<<"Calibration Done!"<<endl;
		calibrated = true;

	}else{
		testrun();
	}
    //std::cout << " Vision run" << std::endl;

    return 0;
}

void Vision::testrun()
{
    if(im!=NULL) { delete im; im=NULL;};
    if(hm!=NULL) { delete hm; hm=NULL;};
    im = _blk->read_nb<InertialSensorsMessage>("InertialSensorsMessage", "Sensors");

    if(im==NULL)//No sensor data!
    {
        cout<<"Warning!!! Vision has no sensor data!"<<endl;
        return;
    }
    if(im->sensordata_size()<7)
    {
        cout<<"Warning!!! Vision has BAD sensor data!"<<endl;
        return;
    }
    //cout << "fetchImage" << endl;
    boost::posix_time::ptime stamp = ext.fetchImage(rawImage);
    if (ext.getCamera()==1)//bottom cam
        seg=segbottom;
    else
        seg=segtop;

#ifdef DEBUGVISION
    cout << "ImageTimestamp:"<< boost::posix_time::to_iso_string(stamp) << endl;
#endif
    //boost::posix_time::ptime rtime =  time_t_epoch+(boost::posix_time::microsec(t.tv_nsec/1000)+boost::posix_time::seconds(t.tv_sec));//+sec(t.tv_sec));

    hm = _blk->read_nb<HeadJointSensorsMessage>("HeadJointSensorsMessage", "Sensors","localhost",&p.time,&stamp);//,&rtime);
    if(hm==NULL)//No sensor data!
    {
        cout<<"Warning!!! Vision has no sensor data!"<<endl;
        return;
    }
    if(hm->sensordata_size()<2)
    {
        cout<<"Warning!!! Vision has BAD sensor data!"<<endl;
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

    float exptime=ext.getExp();//Compensate for middle of image
    //float imcomp=(exptime*1000.0)/p.timediff;
    float imcomp=((stamp-p.time).total_microseconds()*1000.0)/p.timediff;
    cout<<"imcomp:"<<imcomp<<endl;

    cout<< p.timediff<<" "<<p.angX<<" "<<p.angY<<" "<<p.VangX<<" "<<p.VangY<< " "<<endl;
    //Estimate the values at excactly the timestamp of the image
    p.yaw+=p.Vyaw*imcomp;
    p.pitch+=p.Vpitch*imcomp;
    p.angX+=p.VangX*imcomp;
    p.angY+=p.VangY*imcomp;

    //Now use transformations to use the angX,angY values in the image
    KMat::ATMatrix<float,4> y,z;
    KMat::transformations::rotateY(y,p.pitch);
    KMat::transformations::rotateZ(z,p.yaw);
    y.mult(z).invert();

    KMat::HCoords<float,3> angstart;
    angstart.zero();
    angstart(1)=p.angX;
    angstart(2)=p.angY;

    if(ang!=NULL){delete ang;ang=NULL;};
    ang=&y.transform(angstart);
    ang->prettyPrint();


    angstart.zero();
    angstart(1)=p.VangX;
    angstart(2)=p.VangY;

    if(Vang!=NULL){delete Vang;Vang=NULL;};
    Vang=&y.transform(angstart);
    Vang->prettyPrint();

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

Vision::Vision() :Publisher("Vision"),
        cvHighgui(true), type(VISION_CSPACE)
{;}

void Vision::UserInit() {
    ext.Init(_com);
    cout << "Vision():" ;//<< endl;
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
    int step = 3;
    int ystep = 2;

    KSegmentator::colormask_t tempcolor;
    //int ballpixel = -1;
    unsigned int ballthreshold = 2;
    unsigned int goalposthreshold= 5;
    unsigned int cntwhitegreenpixels = 0;
    unsigned int cntwhitegreenorangepixels=0;
    unsigned int cntother=0;
    int j;
    for (int i = 0; i < rawImage->width; i = i + step)
    {
        //Thru Horizon Possibly someday

        cntwhitegreenpixels = 0;
        cntwhitegreenorangepixels=0;
        cntother=0;
        // ballpixel = -1;

        for (j = rawImage->height - 2; j > 0; j = j - ystep)
        {
            //image start from top left
            tempcolor = doSeg(i, j);

            if (tempcolor == white || tempcolor == green)
            {
                cntwhitegreenpixels++;
                cntwhitegreenorangepixels++;
            }
            else if (tempcolor==orange)
                cntwhitegreenorangepixels++;
            else
                cntother++;
            if(cntother>GLOBALSKIP)//No continuity
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
            if (tempcolor==yellow && cntwhitegreenorangepixels >= goalposthreshold)
            {
                tmpPoint.x = i;
                tmpPoint.y = j;
                ygoalpost.push_back(tmpPoint);
                cntwhitegreenorangepixels=0;
            }
            if (tempcolor==skyblue&& cntwhitegreenorangepixels >= goalposthreshold)
            {
                tmpPoint.x = i;
                tmpPoint.y = j;
                bgoalpost.push_back(tmpPoint);
                cntwhitegreenorangepixels=0;
            }

        }
    }
    balldata_t b = locateBall(ballpixels);
#ifdef DEBUGVISION
    cout << "Ballpixelsize:" << ballpixels.size() << endl;
    cout << b.x << " " << b.y << " " << b.r << endl;
#endif
#define  VFov 34.8f
#define  HFov 46.4f
#define TO_RAD 0.01745329f
    float x = (0.5 - (double) (b.x) / (double) rawImage->width) * HFov * TO_RAD;
    float y = (0.5 - (double) (b.y) / (double) rawImage->height) * VFov * TO_RAD;
    float r = b.r * (HFov / (double) rawImage->width)* TO_RAD;

    if (b.r > 0)
    {
        /*
        memory->insertData("kouretes/Ball/cx", x); // change in Head Yaw
        memory->insertData("kouretes/Ball/cy", y); // change in Head Pitch
        memory->insertData("kouretes/Ball/radius", r); // change in Head Pitch
        memory->insertData("kouretes/Ball/found", 1.0f); // change*/

        //Fill message and publish
#ifdef DEBUGVISION
        cout<<"Vision:Publish Message"<<endl;
#endif
        trckmsg.set_cx(x);
        trckmsg.set_cy(y);
        trckmsg.set_radius(r);
        trckmsg.set_topic("vision");
        publish(&trckmsg,"vision");


    }else{
		trckmsg.set_cx(x);
        trckmsg.set_cy(y);
        trckmsg.set_radius(-1);
        trckmsg.set_topic("vision");
        publish(&trckmsg,"vision");
    }/* else {
		memory->insertData("kouretes/Ball/found", .0f); // change
	}*/
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
            globalcount = 0;
        };

        if (skipcount > MAXSKIP || globalcount > GLOBALSKIP)
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
    cvWaitKey(10);
}
