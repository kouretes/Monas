#include "KCamView.h"
#include "ui_KCamView.h"
#include "architecture/archConfig.h"
#include "tools/toString.h"
using namespace std;
KCamView::KCamView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KCamView)
{
    ui->setupUi(this);
	rawImageRequested = false;
	segImageRequested = false;
	connect(ui->camList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(newListElementRequested(QListWidgetItem*)));
	
	
	availableCamHosts = new HostsComboBox(ui->camComboBox);
	connect(this, SIGNAL(NewHostAdded(QString, QString)),availableCamHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(this, SIGNAL(OldHostRemoved(QString)), availableCamHosts, SLOT(removeComboBoxItem(QString)));
	connect(this, SIGNAL(GameStateMsgUpdate(QString, QString, QString)), availableCamHosts, SLOT(setLWRHGameStateInfo(QString, QString, QString)));

	connect(availableCamHosts, SIGNAL(LWRHSubscriptionRequest(QString)), this, SLOT(SubscriptionHandler(QString)));
	connect(availableCamHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), this, SLOT(UnsubscriptionHandler(QString)));
	
	orangeColor = 32;
	redColor = 1;
	greenColor = 4;
	blueColor = 2;
	yellowColor = 16;
	blackColor = 128;
	whiteColor = 64;

	basicSegColors[orangeColor] = qRgb(255,140,0);
	basicSegColors[redColor] = qRgb(255,0,0);
	basicSegColors[greenColor] = qRgb(0,255,0);
	basicSegColors[blueColor] = qRgb(0,0,255);
	basicSegColors[yellowColor] = qRgb(255,255,0);
	basicSegColors[blackColor] = qRgb(0,0,0);
	basicSegColors[whiteColor] = qRgb(255,255,255);

	loadXMLConfigParameters(ArchConfig::Instance().GetConfigPrefix() + "/vision.xml");
}

KCamView::~KCamView()
{
    delete ui;
}

void KCamView::kRawImageUpdateHandler(KRawImage rawImage, QString hostId)
{
	QImage* image;
	currentImage = rawImage;
	if (rawImageRequested)
	{
		image = YUVPixelFormat2RGB32(rawImage);
		ui->imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
	else if(segImageRequested)
	{
		image = YUV2RGBPlusPixSegmentation(rawImage);
		ui->imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}

void KCamView::newListElementRequested(QListWidgetItem* item)
{
	if(ui->camList->row(item) == 0 )
	{
		if(item->checkState() == 0)
		{
			rawImageRequested = false;
			QImage emptyImage;
			ui->imageLabel->setPixmap(QPixmap::fromImage(emptyImage));
		}else{
			rawImageRequested = true;
			segImageRequested = false;
			ui->camList->item(1)->setCheckState(Qt::Unchecked);
			kRawImageUpdateHandler(currentImage,"");
		}

	}else if(ui->camList->row(item) == 1)
	{
		if(item->checkState() == 0)
		{
			segImageRequested = false;
			QImage emptyImage;
			ui->imageLabel->setPixmap(QPixmap::fromImage(emptyImage));
		}else
		{
			segImageRequested = true;
			rawImageRequested = false;
			ui->camList->item(0)->setCheckState(Qt::Unchecked);
			kRawImageUpdateHandler(currentImage,"");
		}
	}
}

void KCamView::loadXMLConfigParameters(std::string fname)
{
	XMLConfig* xmlconfig = new XMLConfig(fname);
	std::string colorTableConf;
	ifstream myReadFile;
	QString filename;

	xmlconfig->QueryElement("SegmentationBottom", colorTableConf);
	filename = QString::fromStdString(ArchConfig::Instance().GetConfigPrefix()
				+ "colortables/" + colorTableConf);
	myReadFile.open(filename.toStdString().c_str());

	if(myReadFile.is_open())
		yuvColorTable = new KSegmentator(myReadFile);

	myReadFile.close();
}

QImage* KCamView::YUVPixelFormat2RGB32(KRawImage rawImage)
{
	double A=1.4075, B=0.3455, C=0.7169, D=1.7790;
	int startofBlock;
	unsigned char y, cu, cv;
	int r, g, b, u, v;

	int widthInPixels = rawImage.width();
	int widthmult2 = widthInPixels*2;
	int heightInPixels = rawImage.height();
	int channels = rawImage.bytes_per_pix();
	const char *data = rawImage.image_rawdata().data();

	QImage *qimg = new QImage(widthInPixels, heightInPixels, QImage::Format_ARGB32);

	for (int hCoord = 0; hCoord < heightInPixels; hCoord++)
	{
		for (int wCoord = 0; wCoord < widthInPixels; wCoord++)
		{
			if(channels == 2)
			{
				y = *(data + hCoord*widthmult2 + (wCoord<<1));//Y is right where we want it
				//a block is a yuyv sequence, and from that block extract the second (Y) and 4th byte (V)
				startofBlock =hCoord * widthmult2 + ((wCoord>>1)<<2); //every 2 pixels (i/2) swap block (size of block=4)
				// cout<<"sob"<<endl;
				cu=*(data + startofBlock+1);
				// cout<<"u"<<endl;
				cv= *(data + startofBlock+3);

				u=cu-128;
				v=cv-128;
				//cout<<y<<" "<<u<<" "<< v<<" " <<endl;
				r=y+A*v;
				g=y-B*u-C*v;
				b=y+D*u;
				//cout<<r<<" "<<g<<" "<< b<<endl;
				r=r<0?0:r;
				g=g<0?0:g;
				b=b<0?0:b;
				r=r>255?255:r;
				g=g>255?255:g;
				b=b>255?255:b;

				qimg->setPixel(wCoord, hCoord, qRgb(r,g,b));
			}else
			{
				// what ??
			}
		}
	}
	return qimg;
}

QImage* KCamView::YUV2RGBPlusPixSegmentation(KRawImage rawImage)
{
	double A=1.4075, B=0.3455, C=0.7169, D=1.7790;
	int startofBlock;
	unsigned char y, cu, cv;
	int r, g, b, u, v;

	int widthInPixels = rawImage.width();
	int widthmult2 = widthInPixels*2;
	int heightInPixels = rawImage.height();
	int channels = rawImage.bytes_per_pix();
	const char *data = rawImage.image_rawdata().data();

	float curLuminance = rawImage.luminance_scale();
	yuvColorTable->setLumaScale(powf(curLuminance,0.42));

	QImage *segImage = new QImage( widthInPixels, heightInPixels, QImage::Format_RGB32);

	QYuv temp;
	for (int hCoord = 0; hCoord < heightInPixels; hCoord++)
	{
		for (int wCoord = 0; wCoord < widthInPixels; wCoord++)
		{
			if(channels == 2)
			{
				y = *(data + hCoord*widthmult2 + (wCoord<<1));//Y is right where we want it
				//a block is a yuyv sequence, and from that block extract the second (Y) and 4th byte (V)
				startofBlock =hCoord * widthmult2 + ((wCoord>>1)<<2); //every 2 pixels (i/2) swap block (size of block=4)
				// cout<<"sob"<<endl;
				cu=*(data + startofBlock+1);
				// cout<<"u"<<endl;
				cv= *(data + startofBlock+3);

				temp.y = y;
				temp.u = cu;
				temp.v = cv;
				segImage->setPixel(wCoord, hCoord, basicSegColors[yuvColorTable->classifyNoPrecheck(temp.y, temp.u, temp.v)]);

			}else
			{
				// what ??
			}
		}
	}

	return segImage;

}
//TODO
/*void KLabel::resizeEvent(QResizeEvent* event)
{
	if (width()>height())
		robotView->resizeRobotView(height()-10);
	else
		robotView->resizeRobotView(width()-10);

}*/

void KCamView::addComboBoxItem(QString data1, QString data2){
	emit NewHostAdded(data1,data2);
}

void KCamView::removeComboBoxItem(QString data1){
	emit OldHostRemoved(data1);
}

void KCamView::setLWRHGameStateInfo(QString data1, QString data2, QString data3){
	emit GameStateMsgUpdate(data1,data2,data3);
}

void KCamView::SubscriptionHandler(QString data1){
	emit LWRHSubscriptionRequest(data1);
}

void KCamView::UnsubscriptionHandler(QString data1){
	emit LWRHUnsubscriptionRequest(data1);
}

void KCamView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
