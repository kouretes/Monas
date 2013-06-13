#include "KccHandler.h"
#include "ui_KccHandler.h"
#include "core/architecture/configurator/Configurator.hpp"

KccHandler::KccHandler(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::KccHandler) {
	
	ui->setupUi(this);
	availableKCCHosts = new HostsComboBox(ui->KCComboBox);
	orangeColor = orange;
	greenColor = green;
	yellowColor = yellow;
	whiteColor = white;
	redColor = red;
	blueColor = blue;
	blackColor = black;
	A = 1.4075;
	B = 0.3455;
	C = 0.7169;
	D = 1.7790;
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
	segImage = QImage( widthInPixels, heightInPixels, QImage::Format_RGB32);
	segImage.fill(0);
	realImage = QImage( widthInPixels, heightInPixels, QImage::Format_RGB32);
	realImage.fill(0);
	realImL->setPixmap(QPixmap::fromImage(realImage));
	//realImL->setStyleSheet("border: 3px solid grey");
	segImL->setPixmap(QPixmap::fromImage(segImage));
	//segImL->setStyleSheet("border: 3px solid grey");
	takeSnapshot = false;
	rScale = iScale = 1;
	zoomInScale = 1.1;
	zoomOutScale = 0.9;
	realImL->resize(rScale * realImL->pixmap()->size());
	segImL->resize(iScale * segImL->pixmap()->size() );
	adjustScrollBar(scrollImage->horizontalScrollBar(), rScale);
	adjustScrollBar(scrollImage->verticalScrollBar(), rScale);
	adjustScrollBar(scrollSeg->horizontalScrollBar(), iScale);
	adjustScrollBar(scrollSeg->verticalScrollBar(), iScale);
	ui->pbZoutReal->setEnabled(false);
	ui->pbZoutSeg->setEnabled(false);
	connect(ui->pbOrange, SIGNAL(clicked()), this, SLOT(pbOrangePressed()) );
	connect(ui->pbGreen, SIGNAL(clicked()), this, SLOT(pbGreenPressed() ));
	connect(ui->pbYellow, SIGNAL(clicked()), this, SLOT(pbYellowPressed()) );
	connect(ui->pbWhite, SIGNAL(clicked()), this, SLOT(pbWhitePressed()) );
	connect(ui->pbBlue, SIGNAL(clicked()), this, SLOT(pbBluePressed()) );
	connect(ui->pbRed, SIGNAL(clicked()), this, SLOT(pbRedPressed()) );
	connect(ui->pbBlack, SIGNAL(clicked()), this, SLOT(pbBlackPressed()) );
	this->connect(realImL, SIGNAL(clicked(QMouseEvent*)), SLOT(clickedImage(QMouseEvent*)) );
	this->connect(segImL, SIGNAL(clicked(QMouseEvent*)), SLOT(clickedImage(QMouseEvent*)) );
	connect(ui->pbUndo, SIGNAL(clicked()), this, SLOT(undoPressed()) );
	connect(ui->pbSnapshot, SIGNAL(clicked()), this, SLOT(pbSnapshotPressed()) );
	connect(ui->pbSave, SIGNAL(clicked()), this, SLOT(segSave()) );
	connect(ui->pbOpen, SIGNAL(clicked()), this, SLOT(segOpen()) );
	connect(ui->pbClear, SIGNAL(clicked()), this, SLOT(clearColorTable()) );
	connect(ui->pbZinSeg, SIGNAL(clicked()), this, SLOT(segZoomIn()) );
	connect(ui->pbZoutSeg, SIGNAL(clicked()), this, SLOT(segZoomOut()) );
	connect(ui->pbZinReal, SIGNAL(clicked()), this, SLOT(realZoomIn()) );
	connect(ui->pbZoutReal, SIGNAL(clicked()), this, SLOT(realZoomOut()) );
	connect(ui->pbCalibration, SIGNAL(clicked()), this, SLOT(manualCalibration()) );
	
	connect(availableKCCHosts, SIGNAL(SubscriptionRequest(QString)), this, SLOT(SubscriptionHandler(QString)) );
	basicSegColors[orangeColor] = qRgb(255, 140, 0);
	basicSegColors[redColor] = qRgb(255, 0, 0);
	basicSegColors[greenColor] = qRgb(0, 255, 0);
	basicSegColors[blueColor] = qRgb(0, 0, 255);
	basicSegColors[yellowColor] = qRgb(255, 255, 0);
	basicSegColors[blackColor] = qRgb(0, 0, 0);
	basicSegColors[whiteColor] = qRgb(255, 255, 255);
	choosedColor = orangeColor;
	lumaScale = 1;
	yuvColorTableOld = NULL;
	yuvColorTable = NULL;
	yuvColorTableOld = new KSegmentator(3, 2, 2);
	yuvColorTable = new KSegmentator(3, 2, 2);
	// HARDCODED CARE IF YOU CHANGE THE KMONITOR INSTALL PATH!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	colortablesPath = QDir::currentPath().append(string("/" + Configurator::Instance().getDirectoryPath() + "colortables").c_str());
	
	calibrationWidget = NULL;
}

void KccHandler::clickedImage(QMouseEvent *ev) {

	int x, y;
	x =((int)ev->x()*realImage.width() / ((KccLabel*)sender())->width() );
	y =((int)ev->y()*realImage.height() / ((KccLabel*)sender())->height() );

	for(int i = 0 ; i < 256 ; i++)
		for(int j = 0 ; j < 256 ; j++)
			for(int z = 0 ; z < 256 ; z++) {
				*(yuvColorTableOld->ctableAccessDirect(i, j, z)) = *(yuvColorTable->ctableAccessDirect(i, j, z));
			}

	int threshold = ui->theshSpinBox->value();
	threshold = threshold*threshold;
	int pixNum = ui->pixelSpinBox->value() - 1;
	map<QYuv, unsigned char> undo;
	lumaScale = 1 / yuvColorTable->getLumaScale();
	qDebug() << lumaScale ;
	QYuv b, temp2;

	for(int px = -pixNum ; px < pixNum + 1 ; px++) {
		for(int py = -pixNum ; py < pixNum + 1 ; py++) {
			if(x + px >= 0 && y + py >= 0 && x + px <= widthInPixels && y + py <= heightInPixels) {
				QYuv temp = yuvRealImage[y+py][x+px];
				temp.y =  adjustY(temp.y);
				temp.u =  adjustU(temp.u);
				temp.v =  adjustV(temp.v);
				undo[temp] = *(yuvColorTableOld->ctableAccessDirect(temp.v, temp.u, temp.y));
				*(yuvColorTable->ctableAccessDirect(temp.v, temp.u, temp.y)) = choosedColor;

				for(int ty = -10; ty < 11 ; ty++) {
					b.y = temp.y + ty;

					if(temp.y + ty >= 0 && temp.y + ty < 256) {
						for(int tu = -10 ; tu < 11 ; tu++) {
							b.u = temp.u + tu;

							if(temp.u + tu >= 0 && temp.u + tu < 256) {
								for(int tv = -10 ; tv < 11 ; tv++) {
									b.v = temp.v + tv;

									if(temp.v + tv >= 0 && temp.v + tv < 256 && distance(temp, b) <= threshold) {
										undo[b] = *(yuvColorTableOld->ctableAccessDirect(b.v, b.u, b.y));
										*(yuvColorTable->ctableAccessDirect(b.v, b.u, b.y) ) = choosedColor;
										//qDebug() << *(yuvColorTable->ctableAccess(b.v,b.u,b.y));
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for(int i = 0 ; i < widthInPixels ; i++) {
		for(int j = 0 ; j < heightInPixels ; j++) {
			QYuv temp = yuvRealImage[j][i];
			segImage.setPixel(i, j, basicSegColors[yuvColorTable->classifyNoPrecheck(temp.y, temp.u, temp.v)]);
		}
	}

	undoVector.push_back(undo);
	ui->pbUndo->setEnabled(true);

	if(undoVector.size() >= MAX_UNDO)
		undoVector.erase(undoVector.begin());

	segImL->setPixmap(QPixmap::fromImage(segImage));
	segImL->show();
}

void KccHandler::undoPressed() {

	if(undoVector.size() > 0) {
		//map<QRgb,char>::iterator it;
		map<QYuv, unsigned char> undoList = undoVector.back();
		undoVector.pop_back();

		if(undoVector.size() == 0)
			ui->pbUndo->setEnabled(false);

		QYuv temp;
		int size = undoList.size();

		for(map<QYuv, unsigned char>::iterator iter = undoList.begin() ; iter != undoList.end() ; iter++) {
			temp =(*iter).first;
			*(yuvColorTable->ctableAccessDirect(temp.v, temp.u, temp.y)) =(*iter).second;
		}

		for(int i = 0 ; i < widthInPixels ; i++) {
			for(int j = 0 ; j < heightInPixels ; j++) {
				temp = yuvRealImage[j][i];
				segImage.setPixel(i, j, basicSegColors[yuvColorTable->classifyNoPrecheck(temp.y, temp.u, temp.v)]);
			}
		}

		segImL->setPixmap(QPixmap::fromImage(segImage));
		segImL->show();
	}
}

void KccHandler::changeImage(KRawImage rawImage, QString hostId) {
	int tempWidth = rawImage.width();
	int tempHeight = rawImage.height();
	int channels = rawImage.bytes_per_pix();

	if((ui->rbLiveVideo->isChecked() || takeSnapshot) && tempWidth == widthInPixels && tempHeight == heightInPixels && channels == 2) {
		takeSnapshot = false;
		curLuminance = rawImage.luminance_scale();
		yuvColorTable->setLumaScale(powf(curLuminance, 0.42));
		lumaScale = 1 / yuvColorTable->getLumaScale();
		segImage = QImage(widthInPixels, heightInPixels, QImage::Format_RGB32);
		segImage.fill(0);
		realImage = QImage( widthInPixels, heightInPixels, QImage::Format_RGB32);
		const char *data = rawImage.image_rawdata().data();
		transformYUVtoRGB(data, &realImage);
		QYuv temp;

		for(int i = 0 ; i < widthInPixels ; i++) {
			for(int j = 0 ; j < heightInPixels ; j++) {
				temp = yuvRealImage[j][i];
				segImage.setPixel(i, j, basicSegColors[yuvColorTable->classifyNoPrecheck(temp.y, temp.u, temp.v)]);
			}
		}

		segImL->setPixmap(QPixmap::fromImage(segImage));
		segImL->show();
		realImL->setPixmap(QPixmap::fromImage(realImage));
		realImL->show();
	}
}

void KccHandler::transformYUVtoRGB(const char *yuvImage, QImage *rgbImage) {
	unsigned char y, cu, cv;
	int r, g, b, u, v;
	int startofBlock;

	for(int hCoord = 0 ; hCoord < heightInPixels ; hCoord++) {
		for(int wCoord = 0 ; wCoord < widthInPixels ; wCoord++) {
			y = *(yuvImage + hCoord * widthmult2 + (wCoord << 1)); // Y is right where we want it
			// a block is a yuyv sequence, and from that block extract the second(Y) and 4th byte(V)
			startofBlock = hCoord*widthmult2 + ((wCoord >> 1) << 2); // every 2 pixels(i/2) swap block(size of block=4)
			// cout<<"sob"<<endl;
			cu = *(yuvImage + startofBlock + 1);
			// cout<<"u"<<endl;
			cv = *(yuvImage + startofBlock + 3);
			yuvRealImage[hCoord][wCoord].y = y;
			yuvRealImage[hCoord][wCoord].u = cu;
			yuvRealImage[hCoord][wCoord].v = cv;
			u = cu - 128;
			v = cv - 128;
			//cout<<y<<" "<<u<<" "<< v<<" " <<endl;
			r = y + A * v;
			g = y - B * u - C * v;
			b = y + D * u;
			//cout<<r<<" "<<g<<" "<< b<<endl;
			r = r < 0 ? 0 : r;
			g = g < 0 ? 0 : g;
			b = b < 0 ? 0 : b;
			r = r > 255 ? 255 : r;
			g = g > 255 ? 255 : g;
			b = b > 255 ? 255 : b;
			rgbImage->setPixel(wCoord, hCoord, qRgb(r, g, b));
		}
	}
}

unsigned char KccHandler::adjustY(unsigned char y) {
	int r = y * lumaScale;
	r = r > 255 ? 255 : r;
	r = r < 0 ? 0 : r;
	return(unsigned char)r;
}

unsigned char KccHandler::adjustU(unsigned char u) {
	int r =(u - 128) * lumaScale + 128;
	r = r > 255 ? 255 : r;
	r = r < 0 ? 0 : r;
	return(unsigned char)r;
}

unsigned char KccHandler::adjustV(unsigned char v) {
	int r =(v - 128) * lumaScale + 128;
	r = r > 255 ? 255 : r;
	r = r < 0 ? 0 : r;
	return(unsigned char)r;
}

int KccHandler::distance(QYuv a, QYuv b) {
	return pow(a.y - b.y, 2) + pow(a.u - b.u, 2) + pow(a.v - b.v, 2);
}

void KccHandler::clearColorTable() {
	realImL->blockSignals(true);
	segImL->blockSignals(true);
	undoVector.clear();
	segImage.fill(0);
	segImL->setPixmap(QPixmap::fromImage(segImage));
	segImL->show();
	ui->pbUndo->setEnabled(false);

	for(int i = 0 ; i < 256 ; i++) {
		for(int j = 0 ; j < 256 ; j++) {
			for(int z = 0 ; z < 256 ; z++) {
				*(yuvColorTable->ctableAccessDirect(i, j, z)) = blackColor;
			}
		}
	}

	realImL->blockSignals(false);
	segImL->blockSignals(false);
}

void KccHandler::realZoomIn() {
	rScale *= zoomInScale;
	realImL->resize(rScale * realImL->pixmap()->size());
	adjustScrollBar(scrollImage->horizontalScrollBar(), rScale);
	adjustScrollBar(scrollImage->verticalScrollBar(), rScale);

	if(rScale >= 3)
		ui->pbZinReal->setEnabled(false);

	ui->pbZoutReal->setEnabled(true);
	realImL->show();
}

void KccHandler::realZoomOut() {
	rScale *= zoomOutScale;
	realImL->resize(rScale * realImL->pixmap()->size());
	adjustScrollBar(scrollImage->horizontalScrollBar(), rScale);
	adjustScrollBar(scrollImage->verticalScrollBar(), rScale);

	if(rScale < 1)
		ui->pbZoutReal->setEnabled(false);

	ui->pbZinReal->setEnabled(true);
	realImL->show();
}

void KccHandler::segZoomIn() {
	iScale *= zoomInScale;
	segImL->resize(iScale * segImL->pixmap()->size());
	adjustScrollBar(scrollSeg->horizontalScrollBar(), iScale);
	adjustScrollBar(scrollSeg->verticalScrollBar(), iScale);

	if(iScale >= 3)
		ui->pbZinSeg->setEnabled(false);

	ui->pbZoutSeg->setEnabled(true);
	segImL->show();
}

void KccHandler::segZoomOut() {
	iScale *= zoomOutScale;
	segImL->resize(iScale * segImL->pixmap()->size());
	adjustScrollBar(scrollSeg->horizontalScrollBar(), iScale);
	adjustScrollBar(scrollSeg->verticalScrollBar(), iScale);

	if(iScale < 1)
		ui->pbZoutSeg->setEnabled(false);
		
	ui->pbZinSeg->setEnabled(true);
	segImL->show();
}

void KccHandler::segSave() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Segmentation File"), colortablesPath, tr("Documents(*.conf)"));
	ofstream myfile(filename.toStdString().c_str(), ios_base::out | ios_base::binary | ios_base::trunc);

	if(myfile.is_open())
		yuvColorTable->writeFile(myfile, "No Comments");

	myfile.close();
}

void KccHandler::segOpen() {
	KSegmentator *temp = yuvColorTable;
	ifstream myReadFile;
	undoVector.clear();
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Segmentation File"), colortablesPath, tr("Segmentation Files(*.conf)"));
	myReadFile.open(filename.toStdString().c_str());

	if(myReadFile.is_open()) {
		yuvColorTable = new KSegmentator(myReadFile);
		yuvColorTable->setLumaScale(powf(curLuminance, 0.42));
	}

	for(int i = 0 ; i < widthInPixels ; i++) {
		for(int j = 0 ; j < heightInPixels ; j++) {
			QYuv temp = yuvRealImage[j][i];
			segImage.setPixel(i, j, basicSegColors[yuvColorTable->classifyNoPrecheck(temp.y, temp.u, temp.v)]);
		}
	}

	segImL->setPixmap(QPixmap::fromImage(segImage));
	segImL->show();
	myReadFile.close();

	if(temp != NULL) {
		temp = NULL;
		delete temp;
	}
}

void KccHandler::manualCalibration(){
	if(calibrationWidget == NULL){
		calibrationWidget = new KccCameraSettings();
	 	calibrationWidget->show();
	}else{
	 	calibrationWidget->raise();
		calibrationWidget->activateWindow();
	}
}

void KccHandler::pbOrangePressed() {
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,140,0) }");
	choosedColor = orangeColor;
}

void KccHandler::pbGreenPressed() {
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(0,255,0) }");
	choosedColor = greenColor;
}

void KccHandler::pbYellowPressed() {
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,255,0) }");
	choosedColor = yellowColor;
}

void KccHandler::pbWhitePressed() {
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,255,255) }");
	choosedColor = whiteColor;
}

void KccHandler::pbBluePressed() {
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(255,0,0) }");
	choosedColor = redColor;
}

void KccHandler::pbRedPressed() {
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(0,0,255) }");
	choosedColor = blueColor;
}

void KccHandler::pbBlackPressed() {
	ui->SelectedColorLabel->setStyleSheet("* { background-color: rgb(0,0,0) }");
	choosedColor = blackColor;
}

void KccHandler::pbSnapshotPressed() {
	takeSnapshot = true;
}

void KccHandler::adjustScrollBar(QScrollBar *scrollBar, double factor) {
	scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}

/*
 * Signal Forwarding 
 */
 
void KccHandler::changeToHost(QString data1) {
	availableKCCHosts->changeItem(data1);
}

void KccHandler::addComboBoxItem(QString data1, QString data2) {
	availableKCCHosts->addComboBoxItem(data1, data2);
}

void KccHandler::removeComboBoxItem(QString data1) {
	availableKCCHosts->removeComboBoxItem(data1);
}

void KccHandler::setGameStateInfo(QString data1, QString data2, QString data3) {
	availableKCCHosts->setGameStateInfo(data1, data2, data3);
}

void KccHandler::SubscriptionHandler(QString data1) {
	emit SubscriptionRequest(data1);
}

KccHandler::~KccHandler() {
	if(calibrationWidget != NULL){
		calibrationWidget->close();
	}
	delete ui;
}

