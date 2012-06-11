#include "KccHandler.h"
#include "ui_KccHandler.h"


KccHandler::KccHandler(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KccHandler)
{
    ui->setupUi(this);

	availableKCCHosts = new LWRemoteHosts(ui->KCComboBox);

	orangeColor = 32;
	greenColor = 4;
	yellowColor = 16;
	whiteColor = 64;
	redColor = 1;
	blueColor = 2;//From magic the gathering :P
	blackColor = 128;

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
	ui->pbUndo->setEnabled(false);
	
	realImL = new KccLabel(this);
    realImL->setBackgroundRole(QPalette::Base);
    realImL->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    realImL->setScaledContents(true);

    segImL = new KccLabel(this);
    segImL->setBackgroundRole(QPalette::Base);
    segImL->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    segImL->setScaledContents(true);


	scrollImage = new QScrollArea;
    scrollImage->setBackgroundRole(QPalette::Dark);
    scrollImage->setWidget(realImL);

	scrollSeg = new QScrollArea;
	scrollSeg->setBackgroundRole(QPalette::Dark);
	scrollSeg->setWidget(segImL);

    ui->scrollLayout->addWidget(scrollImage);
	ui->scrollLayout->addWidget(scrollSeg);	

	segImage = QImage ( widthInPixels, heightInPixels, QImage::Format_RGB32);
	segImage.fill(0);
	realImage = QImage ( widthInPixels, heightInPixels, QImage::Format_RGB32);
	realImage.fill(0);
	realImL->setPixmap(QPixmap::fromImage(realImage));
	//realImL->setStyleSheet("border: 3px solid grey");
	segImL->setPixmap(QPixmap::fromImage(segImage));
	//segImL->setStyleSheet("border: 3px solid grey");

	takeSnapshot = false;
	rScale=iScale=1;
	zoomInScale=1.1;
	zoomOutScale=0.9;
	
	realImL->resize(rScale*realImL->pixmap()->size());
	segImL->resize(iScale*segImL->pixmap()->size());
	
    adjustScrollBar(scrollImage->horizontalScrollBar(), rScale);
    adjustScrollBar(scrollImage->verticalScrollBar(), rScale);
	adjustScrollBar(scrollSeg->horizontalScrollBar(), iScale);
	adjustScrollBar(scrollSeg->verticalScrollBar(), iScale);

	ui->pbZoutReal->setEnabled(false);
	ui->pbZoutSeg->setEnabled(false);


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
    connect(ui->pbSnapshot, SIGNAL(clicked()), this, SLOT(pbSnapshotPressed()));
	connect(ui->pbSaveTemp, SIGNAL(clicked()), this, SLOT(tempSave()));
	connect(ui->pbOpenTemp, SIGNAL(clicked()), this, SLOT(tempOpen()));
	connect(ui->pbSave, SIGNAL(clicked()), this, SLOT(segSave()));
	connect(ui->pbOpen, SIGNAL(clicked()), this, SLOT(segOpen()));
	connect(ui->pbClear, SIGNAL(clicked()), this, SLOT(clearColorTable()));
	connect(ui->pbZinSeg, SIGNAL(clicked()), this, SLOT(segZoomIn()));
	connect(ui->pbZoutSeg, SIGNAL(clicked()), this, SLOT(segZoomOut()));
	connect(ui->pbZinReal, SIGNAL(clicked()), this, SLOT(realZoomIn()));
	connect(ui->pbZoutReal, SIGNAL(clicked()), this, SLOT(realZoomOut()));
	
	
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
	
	
	
	yuvColorTable = new KSegmentator(3,2,2);
	yuvColorTableOld = new KSegmentator(3,2,2);
}

void KccHandler::clickedImage(QMouseEvent* ev){
	int x,y;
	x = ((int) ev->x()*realImage.width()/((KccLabel *)sender())->width());
	y = ((int) ev->y()*realImage.height()/((KccLabel *)sender())->height());


	*yuvColorTableOld = *yuvColorTable;

	int threshold = ui->theshSpinBox->value();
	threshold = threshold*threshold;
	int pixNum = ui->pixelSpinBox->value()-1;
	map<QYuv,char> undo;
	//vector<QRgb> allColors;
	
	lumaScale = yuvColorTable->getLumaScale();
	QYuv b;
    for(int px=-pixNum;px<pixNum+1;px++){
        for(int py=-pixNum;py<pixNum+1;py++){
            if(x+px>=0 && y+py>=0 && x+px<=widthInPixels && y+py<=heightInPixels){
				QYuv temp = yuvRealImage[y+py][x+px];
				undo[temp] = yuvColorTableOld->classifyNoPrecheck(temp.y, temp.u, temp.v);
				*(yuvColorTable->ctableAccess(temp.y/lumaScale, temp.u/lumaScale, temp.v/lumaScale))=choosedColor;		
				for(int ty = -10;ty<11;ty++){
					b.y = temp.y+ty;
					if(b.y>=0 && b.y < 256){
						for(int tu = -10;tu<11;tu++){
							b.u = temp.u+tu;
							if(b.u>=0 && b.u < 256){
								for(int tv = -10;tv<11;tv++){
									b.v = temp.v+tv;
									if(b.v>=0 && b.v < 256 && distance(temp,b)<=threshold){
										undo[b] = yuvColorTableOld->classifyNoPrecheck(b.y, b.u, b.v);
										*(yuvColorTable->ctableAccess(b.y/lumaScale, b.u/lumaScale, b.v/lumaScale))=choosedColor;		
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for(int i=0;i<widthInPixels;i++){
		for(int j=0;j<heightInPixels;j++){
			QYuv temp = yuvRealImage[j][i];
			segImage.setPixel(i,j,basicSegColors[yuvColorTable->classifyNoPrecheck(temp.y, temp.u, temp.v)]);
		}
	}
	undoVector.push_back(undo);
	ui->pbUndo->setEnabled(true);
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
		if(undoVector.size()==0)
			ui->pbUndo->setEnabled(false);
		QYuv temp;
		int size = undoList.size();
		for(map<QYuv,char>::iterator iter = undoList.begin();iter!=undoList.end();iter++){
			temp = (*iter).first;
			*(yuvColorTable->ctableAccess(temp.y/lumaScale, temp.u/lumaScale, temp.v/lumaScale)) = (*iter).second;
		}
		for(int j=0;j<heightInPixels;j++){
			for(int i=0;i<widthInPixels;i++){
				temp = yuvRealImage[j][i];
				segImage.setPixel(i,j,basicSegColors[yuvColorTable->classifyNoPrecheck(temp.y, temp.u, temp.v)]);
			}
		}
		segImL->setPixmap(QPixmap::fromImage(segImage));
		segImL->show();
	}
}

void KccHandler::changeImage(KRawImage rawImage, QString hostId){
	int tempWidth = rawImage.width();
	int tempHeight = rawImage.height();
	int channels = rawImage.bytes_per_pix();
	if((ui->rbLiveVideo->isChecked() || takeSnapshot) && tempWidth == widthInPixels && tempHeight == heightInPixels && channels ==2){
		takeSnapshot = false;
		curLuminance = rawImage.luminance_scale();
		yuvColorTable->setLumaScale(powf(curLuminance,0.45));
		
		segImage = QImage ( widthInPixels, heightInPixels, QImage::Format_RGB32);
		segImage.fill(0);
		realImage = QImage ( widthInPixels, heightInPixels, QImage::Format_RGB32);

		const char *data = rawImage.image_rawdata().data();
		transformYUVtoRGB(data, &realImage);
		QYuv temp;
		for(int i=0;i<widthInPixels;i++){
			for(int j=0;j<heightInPixels;j++){
				temp = yuvRealImage[j][i];
				segImage.setPixel(i,j,basicSegColors[yuvColorTable->classifyNoPrecheck(temp.y, temp.u, temp.v)]);
			}
		}
		segImL->setPixmap(QPixmap::fromImage(segImage));
		segImL->show();
		realImL->setPixmap(QPixmap::fromImage(realImage));
		realImL->show();
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

int KccHandler::distance(QYuv a,QYuv b){
	return pow(a.y-b.y,2)+pow(a.u-b.u,2)+pow(a.v-b.v,2);
}

void KccHandler::clearColorTable(){
	undoVector.clear();
	segImage.fill(0);
	segImL->setPixmap(QPixmap::fromImage(segImage));
	segImL->show();
	ui->pbUndo->setEnabled(false);
	for(int i=0;i<255;i++){
		for(int j=0;j<255;j++){
			for(int z=0;z<255;z++){
				*(yuvColorTable->ctableAccess(i,j,z)) = blackColor;
			}
		}
	}
}

void KccHandler::realZoomIn(){
    rScale *= zoomInScale;
    realImL->resize(rScale * realImL->pixmap()->size());

    adjustScrollBar(scrollImage->horizontalScrollBar(), rScale);
    adjustScrollBar(scrollImage->verticalScrollBar(), rScale);

    if(rScale >=3)
		ui->pbZinReal->setEnabled(false);
	ui->pbZoutReal->setEnabled(true);
	realImL->show();
}

void KccHandler::realZoomOut(){
    rScale *= zoomOutScale;
    realImL->resize(rScale * realImL->pixmap()->size());

    adjustScrollBar(scrollImage->horizontalScrollBar(), rScale);
    adjustScrollBar(scrollImage->verticalScrollBar(), rScale);

    if(rScale <1)
		ui->pbZoutReal->setEnabled(false);
	ui->pbZinReal->setEnabled(true);
	realImL->show();
}

void KccHandler::segZoomIn(){
    iScale *= zoomInScale;
    segImL->resize(iScale * segImL->pixmap()->size());

    adjustScrollBar(scrollSeg->horizontalScrollBar(), iScale);
    adjustScrollBar(scrollSeg->verticalScrollBar(), iScale);

    if(iScale >=3)
		ui->pbZinSeg->setEnabled(false);
	ui->pbZoutSeg->setEnabled(true);
	segImL->show();

}

void KccHandler::segZoomOut(){
    iScale *= zoomOutScale;
    segImL->resize(iScale * segImL->pixmap()->size());

    adjustScrollBar(scrollSeg->horizontalScrollBar(), iScale);
    adjustScrollBar(scrollSeg->verticalScrollBar(), iScale);

	if(iScale <1)
		ui->pbZoutSeg->setEnabled(false);
	ui->pbZinSeg->setEnabled(true);
	segImL->show();

}

void KccHandler::tempSave(){
	ofstream myfile;
	myfile.open ("temp.kcc");
	for(int i=0;i<256;i++)
		for(int j=0;j<256;j++)
			for(int z=0;j<256;j++)
			myfile << yuvColorTable->classifyNoPrecheck(i, j, z);
	myfile.close();
}

void KccHandler::tempOpen(){
	ifstream myReadFile;
	myReadFile.open("temp.kcc");
	if (myReadFile.is_open()) {
		for(int i=0;i<256;i++)
			for(int j=0;j<256;j++)
				for(int z=0;j<256;j++)
 					myReadFile.read((char*)(yuvColorTable->ctableAccess(i,j,z)),1);
	}
	myReadFile.close();
}

void KccHandler::segSave(){
	QString filename = QFileDialog::getSaveFileName(this,tr("Save Segmentation File"), QDir::currentPath(), tr("Documents (*.conf)"));
	ofstream myfile;
	myfile.open(filename.toStdString().c_str());
	if(myfile.is_open()){
		yuvColorTable->writeFile(myfile,"Tipote pros to paron");
	}
	myfile.close();	
}

void KccHandler::segOpen(){
	delete yuvColorTable;
	ifstream myReadFile;
	QString filename = QFileDialog::getOpenFileName(this,tr("Open Segmentation File"), QDir::currentPath(), tr("Segmentation Files (*.conf)"));
	myReadFile.open(filename.toStdString().c_str());
	if(myReadFile.is_open()){
		yuvColorTable = new KSegmentator(myReadFile);
		yuvColorTable->setLumaScale(curLuminance);
	}
	myReadFile.close();	
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

void KccHandler::pbSnapshotPressed(){
	takeSnapshot = true;
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
