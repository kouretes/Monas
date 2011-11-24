#ifndef KIMAGE_H
#define KIMAGE_H
#include <string.h>
#include <cmath>

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
	void copyFrom(void *data,int w,int h,int c)
	{
		if(w*h*c!=imageSize())
		{
			clean();
			imageData=(char *)new  long long [(int)ceil(((float)w*h*c)/(sizeof(long long)) )];//Make allingment as good as possible
		}
		width=w;
		height=h;
		nChannels=c;
		imageData=( char *)data;
		//memcpy(imageData,data,height*width*nChannels);

	}
};

#endif
