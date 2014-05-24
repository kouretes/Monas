#include "ImageExtractor.h"

#include "core/include/Logger.hpp"

using namespace std;





PROVIDER_REGISTER(ImageExtractor);


void ImageExtractor::UserInit()
{
	imext.Init();
	firstRun = false;
	_blk.updateSubscription("image", Messaging::MessageEntry::SUBSCRIBE_ON_TOPIC);
}

int ImageExtractor::Execute()
{
	_blk.process_messages();

	if(!firstRun){
		float scale = imext.calibrateCamera(1500, 10);
		lastrefresh = KSystem::Time::SystemTime::now() - KSystem::Time::TimeAbsolute::microseconds(camerarefreshmillisec + 10);
		firstRun = true;
	}


	ccm = _blk.readSignal<CalibrateCamMessage> ("image");
	if(ccm.get() != NULL){
		if(!imext.setNewUserPrefs()){
			LogEntry(LogLevel::Error,GetName()) <<"Failed to set the new preferences to camera";
		}
	}

	KSystem::Time::TimeAbsolute now = KSystem::Time::SystemTime::now();
	KSystem::Time::TimeAbsolute  timestamp;

	if(lastrefresh + KSystem::Time::TimeAbsolute::milliseconds(camerarefreshmillisec) < now)
	{
		imext.refreshValues();//Reload
		imext.setNewUserPrefs();

		if(imext.currentCameraIsBottom() != 1)
			imext.swapCamera();

		outmsg.set_exposure_us(imext.getExpUs());

		if(imext.currentCameraIsBottom() == 1)
			outmsg.set_active_camera(KRawImage::BOTTOM);
		else
			outmsg.set_active_camera(KRawImage::TOP);

		outmsg.set_luminance_scale(imext.getScale());
		lastrefresh = now;
	}

	timestamp = imext.fetchImage(imstore);
	outmsg.set_width((imstore.width));
	outmsg.set_height((imstore.height));
	outmsg.set_bytes_per_pix(imstore.nChannels);
	outmsg.set_luminance_scale(imext.getScale());
	if(imstore.nChannels == 2)
		outmsg.set_colorspace(KRawImage::YUYV);
	else
		outmsg.set_colorspace(KRawImage::OTHER);

	outmsg.mutable_image_rawdata()->reserve(imstore.imageSize());
	outmsg.mutable_image_rawdata()->assign(imstore.imageData, imstore.imageSize());
	//Publish msg
	Messaging::MessageEntry nmsg;
	google::protobuf::Message * newptr = outmsg.New();
	newptr->CopyFrom(outmsg);
	nmsg.msg.reset(newptr);
	nmsg.host = Messaging::MessageEntry::HOST_ID_LOCAL_HOST;
	nmsg.timestamp = timestamp.wrapTo<KSystem::Time::TimeStamp>();
	nmsg.topic = Messaging::Topics::Instance().getId("image");
	nmsg.msgclass = Messaging::MessageEntry::DATA;
	this->publish(nmsg);
	return 0;
}
