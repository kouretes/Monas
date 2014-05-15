#ifndef IMAGEEXTRACTOR_H
#define IMAGEEXTRACTOR_H

#include <vector>
#include <map>
#include <string>



#include "core/include/IProvider.hpp"
#include "core/include/Blackboard.hpp"
#include "hal/robot/nao/generic_nao/KImageExtractor.h"
#include "messages/Kimage.pb.h"



PROVIDER_START
class ImageExtractor:   public IProvider
{
public:
	static const unsigned camerarefreshmillisec = 1000;
	ImageExtractor(KSystem::ThreadConfig &c, MessageHub&n):
		EndPoint("ImageExtractor"),
		IProvider("ImageExtractor", c, n), _blk("ImageExtractorBlackboard"), imext()
	{
		_blk.attachTo(n);
		lastrefresh = KSystem::Time::SystemTime::now() - KSystem::Time::microseconds(camerarefreshmillisec + 10);
		UserInit();
	};
	int PROVIDER_VISIBLE IEX_DIRECTIVE_HOT Execute();

	void PROVIDER_VISIBLE UserInit();
	const std::string PROVIDER_VISIBLE GetName() const
	{
		return "ImageExtractor";
	}
	PROVIDER_VISIBLE ~ImageExtractor()
	{
	}
private:
	bool firstRun;

	Blackboard _blk;

	KImageExtractor imext;

	KImageConst imstore;
	KRawImage outmsg;

	boost::shared_ptr<const CalibrateCamMessage>  ccm;

	KSystem::Time::TimeAbsolute lastrefresh;
};
PROVIDER_END
#endif

