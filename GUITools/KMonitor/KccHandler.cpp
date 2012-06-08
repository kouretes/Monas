#include "KccHandler.h"
#include "ui_KccHandler.h"

KccHandler::KccHandler(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KccHandler)
{
    ui->setupUi(this);

	availableKCCHosts = new LWRemoteHosts(ui->KCComboBox);

	orangeColor = 'o';
	greenColor = 'g';
	yellowColor = 'y';
	whiteColor = 'w';
	redColor = 'r';
	blueColor = 'u';//From magic the gathering :P
	blackColor = 'b';

	A=1.4075;B=0.3455;C=0.7169;D=1.7790;
	widthInPixels = 640;
	heightInPixels = 480;
	widthmult2 = 2*widthInPixels;

	ui->pbOrange->setStyleSheet("* { background-color: rgb(255,140,0) }");
	ui->pbGreen->setStyleSheet("* { background-color: rgb(0,255,0) }");
	ui->pbYellow->setStyleSheet("* { background-color: rgb(255,255,0) }");
	ui->pbWhite->setStyleSheet("* { background-color: rgb(255,255,255) }");
	ui->pbBlue->setStyleSheet("* { background-color: rgb(255,0,0) }");
	ui->pbRed->setStyleSheet("* { background-color: rgb(0,0,255) }");
	ui->pbBlack->setStyleSheet("* { background-color: rgb(0,0,0) }");
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,140,0) }");
	
	realImL = new KccLabel(this);
    realImL->setBackgroundRole(QPalette::Base);
   // realImL->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //realImL->setScaledContents(true);

    segImL = new KccLabel(this);
    segImL->setBackgroundRole(QPalette::Base);
    //segImL->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   	//segImL->setScaledContents(true);
	//segImL->resize(lx,ly);


   /* ui->scrollImage->setBackgroundRole(QPalette::Dark);
    ui->scrollImage->setWidget(realImL);

	ui->scrollSeg->setBackgroundRole(QPalette::Dark);
	ui->scrollSeg->setWidget(segImL);
*/
	realImage.load("/home/nikos/Desktop/Monas/GUITools/KMonitor/photo.png");
	segImage = QImage ( realImage.width(), realImage.height(), QImage::Format_RGB32);
	segImage.fill(0);
	realImL->setPixmap(QPixmap::fromImage(realImage));
	realImL->setStyleSheet("border: 3px solid grey");
	segImL->setPixmap(QPixmap::fromImage(segImage));
	segImL->setStyleSheet("border: 3px solid grey");


	rScale=iScale=1;
	//qDebug() << realImL->pixmap()->size();
	/*ui->scrollImage->resize(rScale*realImL->pixmap()->size());
	ui->scrollSeg->resize(iScale*segImL->pixmap()->size());

	adjustScrollBar(ui->scrollImage->horizontalScrollBar(), rScale);
    adjustScrollBar(ui->scrollImage->verticalScrollBar(), rScale);
	adjustScrollBar(ui->scrollSeg->horizontalScrollBar(), iScale);
	adjustScrollBar(ui->scrollSeg->verticalScrollBar(), iScale);*/

	connect(ui->pbOrange, SIGNAL(clicked()), this, SLOT(pbOrangePressed()));
	connect(ui->pbGreen, SIGNAL(clicked()), this, SLOT(pbGreenPressed()));
	connect(ui->pbYellow, SIGNAL(clicked()), this, SLOT(pbYellowPressed()));
    connect(ui->pbWhite, SIGNAL(clicked()), this, SLOT(pbWhitePressed()));
	connect(ui->pbBlue, SIGNAL(clicked()), this, SLOT(pbBluePressed()));
	connect(ui->pbRed, SIGNAL(clicked()), this, SLOT(pbRedPressed()));
    connect(ui->pbBlack, SIGNAL(clicked()), this, SLOT(pbBlackPressed()));
    this->connect(realImL, SIGNAL(clicked(QMouseEvent*)), SLOT(clickedImage(QMouseEvent*)));
	this->connect(segImL, SIGNAL(clicked(QMouseEvent*)), SLOT(clickedImage(QMouseEvent*)));
	connect(ui->pbUndo, SIGNAL(clicked()), this, SLOT(undoPressed()));
	connect(ui->rSpin, SIGNAL(valueChanged(double)), this, SLOT(realZoom(double)));
	connect(ui->sSpin, SIGNAL(valueChanged(double)), this, SLOT(segZoom(double)));
	
	connect(this, SIGNAL(NewHostAdded(QString, QString)),availableKCCHosts, SLOT(addComboBoxItem(QString, QString)));
	connect(this, SIGNAL(OldHostRemoved(QString)), availableKCCHosts, SLOT(removeComboBoxItem(QString)));
	connect(this, SIGNAL(GameStateMsgUpdate(QIcon, QString, QString)), availableKCCHosts, SLOT(setLWRHGameStateInfo(QIcon, QString, QString)));

	connect(availableKCCHosts, SIGNAL(LWRHSubscriptionRequest(QString)), this, SLOT(SubscriptionHandler(QString)));
	connect(availableKCCHosts, SIGNAL(LWRHUnsubscriptionRequest(QString)), this, SLOT(UnsubscriptionHandler(QString)));


	basicSegColors[orangeColor] = qRgb(255,140,0);
	basicSegColors[redColor] = qRgb(255,0,0);
	basicSegColors[greenColor] = qRgb(0,255,0);
	basicSegColors[blueColor] = qRgb(0,0,255);
	basicSegColors[yellowColor] = qRgb(255,255,0);
	basicSegColors[blackColor] = qRgb(0,0,0);
	basicSegColors[whiteColor] = qRgb(255,255,255);
	choosedColor = orangeColor;
	//qDebug() << qRed(basicSegColors[yellowColor]) <<qGreen(basicSegColors[yellowColor]) <<qBlue(basicSegColors[yellowColor]);

	
	yuvColorTableOld = (char***) malloc(256*sizeof(char**));
	yuvColorTable = (char***) malloc(256*sizeof(char**));
	for (int i = 0; i < 256; i++){
		yuvColorTableOld[i] = (char**) malloc(256*sizeof(char*));
		yuvColorTable[i] = (char**) malloc(256*sizeof(char*));
		for (int j = 0; j < 256; j++){
			yuvColorTableOld[i][j] = (char*) malloc(256*sizeof(char));
			yuvColorTable[i][j] = (char*) malloc(256*sizeof(char));
		}
	}

	for(int i=0;i<255;i++){
		for(int j=0;j<255;j++){
			for(int z=0;z<255;z++){
				yuvColorTable[i][j][z] = blackColor;
			}
		}
	}
}

void KccHandler::clickedImage(QMouseEvent* ev){
	int x,y;
	x = ((int) ev->x()*realImage.width()/((KccLabel *)sender())->width());
	y = ((int) ev->y()*realImage.height()/((KccLabel *)sender())->height());

	//memcpy(rgbColorTableOld,rgbColorTable,sizeof(rgbColorTable));
	for(int i=0;i<255;i++){
		for(int j=0;j<255;j++){
			for(int z=0;z<255;z++){
				yuvColorTableOld[i][j][z] = yuvColorTable[i][j][z];
			}
		}
	}
	int pixNum = ui->pixelSpinBox->value()-1;
	map<QYuv,char> undo;
	//vector<QRgb> allColors;
    for(int px=-pixNum;px<pixNum+1;px++){
        for(int py=-pixNum;py<pixNum+1;py++){
            if(x+px>=0 && y+py>=0 && x+px<=widthInPixels && y+py<=heightInPixels){
				QYuv temp = yuvRealImage[x+px][y+py];
				//undo[temp] = yuvColorTableOld[temp.y][temp.u][temp.v];
				yuvColorTable[temp.y][temp.u][temp.v]=choosedColor;
			}
		}
	}
	for(int i=0;i<widthInPixels;i++){
		for(int j=0;j<heightInPixels;j++){
			QYuv temp = yuvRealImage[i][j];
			segImage.setPixel(i,j,basicSegColors[yuvColorTable[temp.y][temp.u][temp.v]]);
		}
	}
	undoVector.push_back(undo);
	if(undoVector.size() >= MAX_UNDO)
		undoVector.erase(undoVector.begin());
	segImL->setPixmap(QPixmap::fromImage(segImage));
	segImL->show();
}

void KccHandler::undoPressed(){
	if(undoVector.size()>0){
		//map<QRgb,char>::iterator it;
		map<QYuv,char> undoList = undoVector.back();
		undoVector.pop_back();
		QYuv temp;
		for(map<QYuv,char>::iterator iter = undoList.begin();iter!=undoList.end();iter++){
			temp = (*iter).first;
			yuvColorTable[temp.y][temp.u][temp.v] = (*iter).second;
		}
		for(int i=0;i<widthInPixels;i++){
			for(int j=0;j<heightInPixels;j++){
				temp = yuvRealImage[i][j];
				segImage.setPixel(i,j,basicSegColors[yuvColorTable[temp.y][temp.u][temp.v]]);
			}
		}
		segImL->setPixmap(QPixmap::fromImage(segImage).scaled(segImL->size()));
		segImL->show();
	}
}

void KccHandler::changeImage(KRawImage rawImage, QString hostId){
	int tempWidth = rawImage.width();
	int tempHeight = rawImage.height();
	int channels = rawImage.bytes_per_pix();
	if(tempWidth == widthInPixels && tempHeight == heightInPixels && channels ==2){

		segImage = QImage ( widthInPixels, heightInPixels, QImage::Format_RGB32);
		segImage.fill(0);
		realImage = QImage ( widthInPixels, heightInPixels, QImage::Format_RGB32);

		const char *data = rawImage.image_rawdata().data();
		transformYUVtoRGB(data, &realImage);
		QYuv temp;
		for(int i=0;i<widthInPixels;i++){
			for(int j=0;j<heightInPixels;j++){
				temp = yuvRealImage[i][j];
				segImage.setPixel(i,j,basicSegColors[yuvColorTable[temp.y][temp.u][temp.v]]);
			}
		}
		segImL->setPixmap(QPixmap::fromImage(segImage).scaled(segImL->size()));
		segImL->show();
		realImL->setPixmap(QPixmap::fromImage(realImage).scaled(realImL->size()));
		realImL->show();
		segZoom(1);
		realZoom(1);
	}
}

void KccHandler::transformYUVtoRGB(const char *yuvImage, QImage *rgbImage){
	unsigned char y, cu, cv;
	int r, g, b, u, v;
	int startofBlock;
	for (int hCoord = 0; hCoord < heightInPixels; hCoord++)
	{
		for (int wCoord = 0; wCoord < widthInPixels; wCoord++)
		{
			y = *(yuvImage + hCoord*widthmult2 + (wCoord<<1));//Y is right where we want it
			//a block is a yuyv sequence, and from that block extract the second (Y) and 4th byte (V)
			startofBlock =hCoord * widthmult2 + ((wCoord>>1)<<2); //every 2 pixels (i/2) swap block (size of block=4)
			// cout<<"sob"<<endl;
			cu=*(yuvImage + startofBlock+1);
			// cout<<"u"<<endl;
			cv= *(yuvImage + startofBlock+3);

			yuvRealImage[hCoord][wCoord].y = y;
			yuvRealImage[hCoord][wCoord].u = cu;
			yuvRealImage[hCoord][wCoord].v = cv;

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

			rgbImage->setPixel(wCoord, hCoord, qRgb(r,g,b));
		}
	}
}

void KccHandler::realZoom(double sca){
	rScale = sca;
	realImL->resize(rScale*QPixmap::fromImage(realImage).size());
/*	adjustScrollBar(ui->scrollImage->horizontalScrollBar(), rScale);
	adjustScrollBar(ui->scrollImage->verticalScrollBar(), rScale);*/
}

void KccHandler::segZoom(double sca){
	iScale = sca;
	segImL->resize(iScale*QPixmap::fromImage(realImage).size());
/*	adjustScrollBar(ui->scrollSeg->horizontalScrollBar(), iScale);
	adjustScrollBar(ui->scrollSeg->verticalScrollBar(), iScale);*/
}

void KccHandler::pbOrangePressed(){
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,140,0) }");
	choosedColor = orangeColor;
}

void KccHandler::pbGreenPressed(){
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(0,255,0) }");
	choosedColor = greenColor;
}

void KccHandler::pbYellowPressed(){
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,255,0) }");
	choosedColor = yellowColor;
}

void KccHandler::pbWhitePressed(){
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,255,255) }");
	choosedColor = whiteColor;
}

void KccHandler::pbBluePressed(){
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,0,0) }");
	choosedColor = redColor;
}

void KccHandler::pbRedPressed(){
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(0,0,255) }");
	choosedColor = blueColor;
}

void KccHandler::pbBlackPressed(){
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(0,0,0) }");
	choosedColor = blackColor;
}

void KccHandler::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor*scrollBar->value()+((factor - 1)*scrollBar->pageStep()/2)));
}

/*		Signal Forwarding */
void KccHandler::addComboBoxItem(QString data1, QString data2){
	emit NewHostAdded(data1,data2);
}

void KccHandler::removeComboBoxItem(QString data1){
	OldHostRemoved(data1);
}

void KccHandler::setLWRHGameStateInfo(QIcon data1, QString data2, QString data3){
	emit GameStateMsgUpdate(data1,data2,data3);
}

void KccHandler::SubscriptionHandler(QString data1){
	emit LWRHSubscriptionRequest(data1);
}

void KccHandler::UnsubscriptionHandler(QString data1){
	emit LWRHUnsubscriptionRequest(data1);
}

KccHandler::~KccHandler()
{
    delete ui;
}
