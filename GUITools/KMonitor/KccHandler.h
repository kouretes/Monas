#ifndef KCCHANDLER_H
#define KCCHANDLER_H

#include <QWidget>
#include <vector>
#include <map>
#include <QScrollArea>
#include <string>
#include <QDir>
#include <QDebug>
#include <QColor>
#include <QRgb>
#include <math.h>
#include <QFileDialog>
#include <QScrollBar>
#include "KccLabel.h"
using namespace std;

namespace Ui {
    class KccHandler;
}

class KccHandler : public QWidget
{
    Q_OBJECT
public:
	static const unsigned int MAX_UNDO = 10;
	static const unsigned int THRESHOLD = 10*10;
	char orangeColor;// = 'o';
	char greenColor;// = 'g';
	char yellowColor;// = 'y';
	char whiteColor;// = 'w';
	char redColor;// = 'r';
	char blueColor;// = 'u';//From magic the gathering :P
	char blackColor;// = 'b';

public:
    explicit KccHandler(QWidget *parent = 0);
    ~KccHandler();

    Ui::KccHandler* getKccUi() {return ui;}

private:
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	int distance(QRgb a,QRgb b);

public slots:
    void clickedImage(QMouseEvent* ev);
	void pbOrangePressed();
	void pbGreenPressed();
	void pbYellowPressed();
    void pbWhitePressed();
	void pbBluePressed();
	void pbRedPressed();
    void pbBlackPressed();
	void undoPressed();
	void realZoom(double sca);
	void segZoom(double sca);

private:

	double A, B, C, D;
	int startofBlock;
	int widthInPixels;
	int widthmult2;
	int heightInPixels;
	int channels;
	const char *data;

	char ***rgbColorTable;
	char ***rgbColorTableOld;
	//map<QRgb,char> rgbMap;
	char yuvImage[640][480];
	map<char,QRgb> basicSegColors;
	//char basicSegColorsChars[7];

	char choosedColor;

	vector<map<QRgb,char> > undoVector;

    KccLabel* realImL,* segImL;
	QScrollArea *segScrollArea,*realScrollArea;
	double rScale,iScale;
	int lx,ly;
	QImage segImage,realImage;
    Ui::KccHandler *ui;
};

#endif // KCCHANDLER_H
