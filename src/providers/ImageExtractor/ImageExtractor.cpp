#include "ImageExtractor.h"

#include "tools/logger.h"
#include "tools/toString.h"


using namespace std;





PROVIDER_REGISTER(ImageExtractor);


void ImageExtractor::UserInit() {
    imext.Init(this);
    Blackboard:updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
}



int ImageExtractor::Execute() {
    Blackboard::process_messages();

    boost::shared_ptr<const KCalibrateCam> cal = Blackboard::readState<KCalibrateCam> ("vision");

	if (cal != NULL)
	{
		//cout<<"=======Start calibration:"<<cal->status()<<endl;
		if (cal->status() == 0)
		{
			//cout<<"Start calibration"<<endl;
			KCalibrateCam res;
			Logger::Instance().WriteMsg("ImageExtractor", "Start calibration", Logger::Info);
			float scale = imext.calibrateCamera(cal->sleeptime(), cal->exp());
			lastrefresh=boost::posix_time::microsec_clock::universal_time()-boost::posix_time::microseconds(camerarefreshmillisec+10);
			Logger::Instance().WriteMsg("ImageExtractor", "Calibration Done", Logger::Info);
			//cout<<"Calibration Done!"<<endl;
			res.set_status(1);
			Blackboard::publishState(res, "vision");

		}
	}
	Blackboard::publish_all();

	boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::ptime  timestamp;
	if(lastrefresh+boost::posix_time::millisec(camerarefreshmillisec)<now)
	{
		//cout<<"Refresh"<<endl;
        imext.refreshValues();//Reload
		outmsg.set_exposure_us(imext.getExpUs());
		if(imext.getCamera()==1)
            outmsg.set_active_camera(KRawImage::BOTTOM);
		else
            outmsg.set_active_camera(KRawImage::TOP);
		outmsg.set_exposure_us(imext.getScale());
		lastrefresh=now;
	}
	timestamp=imext.fetchImage(imstore);
    outmsg.set_width((imstore.width));
    outmsg.set_height((imstore.height));
    outmsg.set_bytes_per_pix(imstore.nChannels);
    if(imstore.nChannels==2)
        outmsg.set_colorspace(KRawImage::YUYV);
    else
        outmsg.set_colorspace(KRawImage::OTHER);

    outmsg.mutable_image_rawdata()->assign(imstore.imageData,imstore.imageSize());

    //Publish msg

    msgentry nmsg;

    google::protobuf::Message * newptr=outmsg.New();
    newptr->CopyFrom(outmsg);
    nmsg.msg.reset(newptr);

    nmsg.host=msgentry::HOST_ID_LOCAL_HOST;
    nmsg.timestamp=timestamp;
    nmsg.topic=Topics::Instance().getId("vision");
    nmsg.msgclass=msgentry::DATA;

    this->publish(nmsg);




	return 0;
}
