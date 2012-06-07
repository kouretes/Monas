#include "KccHandler.h"
#include "ui_KccHandler.h"

KccHandler::KccHandler(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KccHandler)
{
    ui->setupUi(this);

	orangeColor = 'o';
	greenColor = 'g';
	yellowColor = 'y';
	whiteColor = 'w';
	redColor = 'r';
	blueColor = 'u';//From magic the gathering :P
	blackColor = 'b';

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


    ui->scrollImage->setBackgroundRole(QPalette::Dark);
    ui->scrollImage->setWidget(realImL);

	ui->scrollSeg->setBackgroundRole(QPalette::Dark);
	ui->scrollSeg->setWidget(segImL);

	realImage.load("/home/eldrad/Desktop/Monas/GUITools/KMonitor/photo.png");
	segImage = QImage ( realImage.width(), realImage.height(), QImage::Format_RGB32);
	segImage.fill(0);
	realImL->setPixmap(QPixmap::fromImage(realImage));
	realImL->setStyleSheet("border: 3px solid grey");
	segImL->setPixmap(QPixmap::fromImage(segImage));
	segImL->setStyleSheet("border: 3px solid grey");


	rScale=iScale=1;
	//qDebug() << realImL->pixmap()->size();
	ui->scrollImage->resize(rScale*realImL->pixmap()->size());
	ui->scrollSeg->resize(iScale*segImL->pixmap()->size());

	adjustScrollBar(ui->scrollImage->horizontalScrollBar(), rScale);
    adjustScrollBar(ui->scrollImage->verticalScrollBar(), rScale);
	adjustScrollBar(ui->scrollSeg->horizontalScrollBar(), iScale);
	adjustScrollBar(ui->scrollSeg->verticalScrollBar(), iScale);

	connect(ui->pbOrange, SIGNAL(clicked()), this, SLOT(pbOrangePressed()));
	connect(ui->pbGreen, SIGNAL(clicked()), this, SLOT(pbGreenPressed()));
	connect(ui->pbYellow, SIGNAL(clicked()), this, SLOT(pbYellowPressed()));
    connect(ui->pbWhite, SIGNAL(clicked()), this, SLOT(pbWhitePressed()));
	connect(ui->pbBlue, SIGNAL(clicked()), this, SLOT(pbBluePressed()));
	connect(ui->pbRed, SIGNAL(clicked()), this, SLOT(pbRedPressed()));
    connect(ui->pbBlack, SIGNAL(clicked()), this, SLOT(pbBlackPressed()));
    this->connect(realImL, SIGNAL(clicked(QMouseEvent*)), SLOT(clickedImage(QMouseEvent*)));
	this->connect(segImL, SIGNAL(clicked(QMouseEvent*)), SLOT(clickedImage(QMouseEvent*)));


	rgbColorTableOld = (char***) malloc(256*sizeof(char**));
	rgbColorTable = (char***) malloc(256*sizeof(char**));
	basicSegColors[orangeColor] = qRgb(255,140,0);
	basicSegColors[redColor] = qRgb(255,0,0);
	basicSegColors[greenColor] = qRgb(0,255,0);
	basicSegColors[blueColor] = qRgb(0,0,255);
	basicSegColors[yellowColor] = qRgb(255,255,0);
	basicSegColors[blackColor] = qRgb(0,0,0);
	basicSegColors[whiteColor] = qRgb(255,255,255);
	choosedColor = orangeColor;
	//qDebug() << qRed(basicSegColors[yellowColor]) <<qGreen(basicSegColors[yellowColor]) <<qBlue(basicSegColors[yellowColor]);

	

	for (int i = 0; i < 256; i++){
		rgbColorTableOld[i] = (char**) malloc(256*sizeof(char*));
		rgbColorTable[i] = (char**) malloc(256*sizeof(char*));
		for (int j = 0; j < 256; j++){
			rgbColorTableOld[i][j] = (char*) malloc(256*sizeof(char));
			rgbColorTable[i][j] = (char*) malloc(256*sizeof(char));
		}
	}

	for(int i=0;i<255;i++){
		for(int j=0;j<255;j++){
			for(int z=0;z<255;z++){
				rgbColorTable[i][j][z] = blackColor;
			}
		}
	}
}


void MainWindow::clickedImage(QMouseEvent* ev){
	int x,y;
	x = ((int) ev->x()*realImage.width()/((myLabel *)sender())->width());
	y = ((int) ev->y()*realImage.height()/((myLabel *)sender())->height());

	//memcpy(rgbColorTableOld,rgbColorTable,sizeof(rgbColorTable));
	for(int i=0;i<255;i++){
		for(int j=0;j<255;j++){
			for(int z=0;z<255;z++){
				rgbColorTableOld[i][j][z] = rgbColorTable[i][j][z];
			}
		}
	}
	int pixNum = ui->pixSelection->value()-1;
	map<QRgb,char> undo;
	//vector<QRgb> allColors;
    for(int px=-pixNum;px<pixNum+1;px++){
        for(int py=-pixNum;py<pixNum+1;py++){
            if(x+px>=0 && y+py>=0 && x+px<=realImage.width() && y+py<=realImage.height()){
				QRgb temp = realImage.pixel(x+px,y+py);
				undo[temp]=rgbColorTableOld[qRed(temp)][qGreen(temp)][qBlue(temp)];
				rgbColorTable[qRed(temp)][qGreen(temp)][qBlue(temp)]=choosedColor;
			}
		}
	}
	for(int i=0;i<realImage.width();i++){
		for(int j=0;j<realImage.height();j++){
			QRgb temp = realImage.pixel(i,j);
			segImage.setPixel(i,j,basicSegColors[rgbColorTable[qRed(temp)][qGreen(temp)][qBlue(temp)]]);
		}
	}
	undoVector.push_back(undo);
	if(undoVector.size() >= MAX_UNDO)
		undoVector.erase(undoVector.begin());
	segImL->setPixmap(QPixmap::fromImage(segImage).scaled(segImL->size(),Qt::KeepAspectRatio));
	segImL->show();
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

KccHandler::~KccHandler()
{
    delete ui;
}
