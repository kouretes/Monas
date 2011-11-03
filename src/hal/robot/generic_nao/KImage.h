#ifndef KIMAGE_H
#define KIMAGE_H
#include <string.h>

class KImage
{
public:
	int width;
	int height;
	int nChannels;
	char* imageData;
	KImage() :width(0),height(0),nChannels(0),imageData(0){}
	~KImage() { clean();}
	void clean() { if(imageData!=0) delete [] imageData; }
	int imageSize(){ return width*height*nChannels;};
	void copyFrom(const void *data,int w,int h,int c)
	{
		if(w*h*c!=imageSize())
		{
			clean();
			imageData=new char[w*h*c];
		}
		width=w;
		height=h;
		nChannels=c;
		memcpy(imageData,data,height*width*nChannels);

	}
};

#endif
