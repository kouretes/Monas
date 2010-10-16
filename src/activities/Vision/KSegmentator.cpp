#include "KSegmentator.h"
#include "tools/logger.h"
#include <iostream>





using namespace std;
//YUV to RGB STANDARD constants, all are integer aproximated with 8 bit precision :)
//#define A 359 //1.402<<8
//#define B 89 //0.344<<8
//#define C 183 //0.714<<8
//#define D 452 //1.772<<8

#define max(x,y) ((x)>(y))?(x):(y)
#define min(x,y) ((x)<(y))?(x):(y)
//#define crop(x) ( ((x)<0)?0:(x)) & 0xFFFF

#define table_subscript(y,u,v) ( (y)*(256>>ures)*(256>>vres)*sizeof(colormask_t)+(u)*(256>>vres)*sizeof(colormask_t)+(v)*sizeof(colormask_t))

// uses S= 1-S of HSV  and L from HSL!!!! so saturation INCREASES as you move to the grayscale!!!
// Done only from computational reasons!!!!!
/**
 * h is an integer between 0-3600 (360.0 degrees, with .1 degree resolution)
 *  S and Y are 1 byte long
 *  Saturation comes from 1-S of HSI colorspace ,so saturation INCREASES as you move towards to grayscale
 */

void KSegmentator::setLumaScale(float s)
{
	lumascale=s;
	cout<<"KSegmentator:setLumaScale():"<<s<<endl;
}
void KSegmentator::yuv2hsy(unsigned char yuv[3], unsigned  int hsy[3])
{


	float r,g,b;
	float  mx,mn;
	int y,u,v;
	const  double A=1.4075,B=0.3455,C=0.7169,D=1.7790;

	y=yuv[0];//-16;//crop(yuv[0]-16);
	u=yuv[1]-128;
	v=yuv[2]-128;
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

	mx=max(max(r,g),b);
	mn=min(min(r,g),b);

	//cout<<r<<" "<<g<<" "<< b<<endl;

	if (mx==mn)
		hsy[0]=3800;//H is undefined return something off the scale
	else
	{
		if (mx==r)
		{
			hsy[0]=(600.0*(g-b))/(mx-mn)+3600.0;
			if (hsy[0]>3600.0)//wrap around
				hsy[0]-=3600.0;
		}
		else if (mx==g)
			hsy[0]=(600.0*(b-r))/(mx-mn)+1200.0;
		else
			hsy[0]=(600.0*(r-g))/(mx-mn)+2400.0;


	}

	hsy[2]=yuv[0]; //Use luma from yuv as a great perceptually relevant estimate of luminance
	hsy[1]=(mn*256)/yuv[0];




}

/*
 * Read corresponding value from ctable
 * the values are shifted (divided) by YRES URES VRES repsectively: colorspace compression!
 */
KSegmentator::colormask_t KSegmentator:: classifyPixel(unsigned char yuv[3])
{
	//return calculatePixel(yuv);fact
	//cout<<(int)*(ctable+table_subscript((int)yuv[0]>>YRES,(int)yuv[1]>>URES,(int)yuv[2]>>VRES))<<":"<<(int)calculatePixel(yuv)<<endl;
	if (ctable!=NULL)
	{
		unsigned char data[3];//TODO: Scaling should work on all YCbCr standards, given then the properties of multiplication! :)
		int y=yuv[0]*lumascale;
		y=y>255?255:y;
		data[0]=y;
		data[1]=(yuv[1]-128)*lumascale+128;
        data[2]=(yuv[2]-128)*lumascale+128;

		//data[1]=data[1]>255?255:data[1];
		//data[2]=data2]>255?255:data[2];
		//cout<<(int)data[0]<<" ";
		//return *(ctable+table_subscript((int)yuv[0]>>yres,(int)yuv[1]>>ures,(int)yuv[2]>>vres));
		return *(ctable+table_subscript((int)data[0]>>yres,(int)data[1]>>ures,(int)data[2]>>vres));
	}
	else
		return 0;


}
/**
 * Calculate a pixel value from the rules!
 * How it works?  First of all HSY  is NOT a standard colospace
 * uses S= 1-S of HSI   so saturation INCREASES as you move to the grayscale!!!  Y from YUV!!!!
 * First rule is BLACK: when l< blackRadius then color is black: region is a bottom slice from the HSL cylinder
 *
 * second rule is colors: for every color class there is a "slice" (as in: pizza slice) in the cylinder that the hue must belong to
 * for every colorspace only colors that are INSIDE a circle are returned as true colors
 * the circle is defined with the center at s=0 and l=1 (ideal fully saturated color) - remember s is defined BACKWARDS
 * and the radius is configurable as cclass.Radius see the configuration file
 *
 * Third rule is white: white is a sphere with the center at s=1,l=1 TOP center of HSL and radius is whiteRadius
 * Final rule is gray: a cylinder concentric to the HSL cylinder: extends from s=1 (center) to grayRadius

 * Black is exluded first since the color radii typically overal (and even extend beyond) the bottom of the HSL
 * Colors are excluded after, so that different radii for each color represent a different color-to-white threshold
 * White is excluded, since no other things overlap with this
 * gray is excluded last, since the TOP part of the gray cylinder represend white!
 */
KSegmentator::colormask_t KSegmentator:: calculatePixel(unsigned char yuv[3])
{
	unsigned int hsy[3];
	int dists,distl;
	KSegmentator::yuv2hsy(yuv,hsy);


	//cout<<hsl[0]<<" "<<hsl[1]<<" "<<hsl[2]<<endl;
	//Black

	if (hsy[2]<blackRadius)
		return blackMask;


	//Color Radius
	dists=24-hsy[1];
	distl=128-hsy[2];//(0x80<hsy[2])?(hsy[2]-0x80):hsy[2];
	//cout<<"colors.size():"<<colors.size()<<endl;
	for (int i=0;i<colors.size();i++)
	{
		//cout<<colors[i].HueMin<<endl;

		if (colors[i].HueMin < hsy[0] && hsy[0]<colors[i].HueMax && ( dists*dists+distl*distl)<colors[i].Radius)
		{
			//cout<<" "<<((int)colors[i].HueMin)<< " ";
			return colors[i].mask;
		}

	}

	//White Radius
	dists=256-((int)hsy[1]);
	distl=256-((int)hsy[2]);
	if (dists*dists+distl*distl<whiteRadius)
	{
		//cout<<hsy[0]<<" "<<hsy[1]<<" "<<hsy[2]<<endl;
		return whiteMask;
	}

	//Gray
	if (hsy[1]>grayRadius)
	{
		//cout<<"g ";
		//cout<<hsy[0]<<" "<<hsy[1]<<" "<<hsy[2]<<endl;
		return grayMask;
	}
	return 0;



}


/**
 *Clasify a pixel of the image: i is row and j is column, ie i is 0-640 and j is 0-480 in a 640x480 image
		 * type is KYUVColorspace or KYUV422InterlacedColorspace ONLY
 */
KSegmentator::colormask_t KSegmentator::  classifyPixel(IplImage*data, int i,int j,int type)
{

	// You can get some informations about the image.
	int width = data->width;
	int height =  data->height;
	int nbLayers = data->nChannels;
	//int colorSpace = (int) image[3];

	const char* dataPointer= data->imageData;
	if (type==AL::kYUV422InterlacedColorSpace)//YUYV nbLayers is 2, average 2 bytes per pixel
	{
		unsigned char yuv[3];
		yuv[0]=*(dataPointer+j*width*nbLayers+i*nbLayers);//Y is right where we want it

		//a block is a yuyv sequence, and from that block extract the second (Y) and 4th byte (V)
		int startofBlock =j*(width*2)+((i/2)*4); //every 2 pixels (i/2) swap block (size of block=4)
		// cout<<"sob"<<endl;
		yuv[1]=*(dataPointer+startofBlock+1);
		// cout<<"u"<<endl;
		yuv[2]= *(dataPointer+startofBlock+3);



		//if(i==j)
		return classifyPixel(yuv);

	}
	else if (type==AL::kYUVColorSpace)//YUV
	{
		unsigned char yuv[3];
		yuv[0]=*(dataPointer+j*width*nbLayers+i*nbLayers);
		yuv[1]=*(dataPointer+j*width*nbLayers+i*nbLayers+1);
		yuv[2]=*(dataPointer+j*width*nbLayers+i*nbLayers+2);
		//if(i==j)
		return classifyPixel(yuv);

	}
	else
	{
		Logger::Instance().WriteMsg("KSegmentator", "ONLY YUV422 AND YUV IMPLEMENTED :P",Logger::Error);

	}

	return 0;
}
/**
 * Create a colortable and store it in ctable
 */
void KSegmentator::filluptable()
{
	int y,u,v;
	unsigned char data[3];
	ctable =  (colormask_t*)malloc((256>>yres)*(256>>ures)*(256>>vres)*sizeof(colormask_t));
	for (y=0;y<256>>yres;y++)
		for (u=0;u<256>>ures;u++)
			for (v=0;v<256>>vres;v++)
			{
				//INTERESTING POINT: value to sample for that region is actually the MIDDLE value
				// so if Y is 0-16-32..YRES=4
				//Sampled values are  7 - 23- 39 etc
				// This is done in HOPE of representing better the region at hand
				data[0]=(y<<yres)+((1<<yres)-1)/2;
				data[1]=(u<<ures)+((1<<ures)-1)/2;
				data[2]=(v<<vres)+((1<<vres)-1)/2;
				*(ctable+table_subscript(y,u,v))=calculatePixel(data);
				if ( *(ctable+table_subscript(y,u,v))!=calculatePixel(data) )
					cout<<(int)data[0]<<","<<(int)data[1]<<","<<(int)data[2]<<":"<<(int)*(ctable+table_subscript(y,u,v))<<":"<< (int)calculatePixel(data)<<endl ;
			}



}
/*
dists=hsl[1];
		distl=(0x80<hsl[2])?(hsl[2]-0x80):hsl[2];

		if(i==j)
		 //cout<< hsl[0]<<","<<hsl[1]<<","<<hsl[2]<<endl<<endl;
		 cout<< hsl[0]<< " ";
		if(huemin<=hsl[0] && hsl[0]<src$=huemax && (dists*dists + distl+distl)< radius )//&& hsl[1]>0x8000&& hsl[2]<0x8000&& hsl[2]>0x1000)
		{
                   avgi+=i;
                   avgj+=j;
	           cnt++;
		   *(imgA+i*width+j)=250;
		}
		else
		  *(imgA+i*width+j)=hsl[0]*0.02;
*/

/**
 * Load up a configuration file
 */
KSegmentator::KSegmentator(std::ifstream &conf)
{

	conf.read(reinterpret_cast<char *>(&set),sizeof(set));

	//Check that file is indeed Segmentation configuration
	if (set.ID[0]!='K' || set.ID[1]!='S')
	{
		cout<<"KSegmentator(): Invalid configuration file"<<endl;
        Logger::Instance().WriteMsg("KSegmentator", "Invalid configuration file",Logger::Error);
		return;
	}
	if (set.size-'0'>sizeof(colormask_t))
	{
		cout<<"KSegmentator(): Not enough length in colormask_t"<<endl;
        Logger::Instance().WriteMsg("KSegmentator", "Not enough length in colormask_t",Logger::Error);
		return;
	}
	readComment(conf);
	readCalibration(conf);
	readColorInfo(conf);
	if (set.ruletype=='R')
		readRulefile(conf);
	else if (set.ruletype=='C')
		readColorTable(conf);
	lumascale=1;//Default setting;



}

void KSegmentator::readComment(ifstream & conf)
{
    char com[1024];
    conf.get(com,1024);
    Logger::Instance().WriteMsg("KSegmentator", std::string(com), Logger::Info);

}

void KSegmentator::readCalibration(ifstream & conf)
{
	for (int i=0;i<set.calLines-'0';i++)
		conf.ignore(256,'\n');

}
void KSegmentator::readColorInfo(ifstream & conf)
{
	for (int i=0;i<set.colorLines-'0';i++)
		conf.ignore(256,'\n');

}

void KSegmentator::readColorTable(ifstream & conf)
{
	if (set.conf[0]=='Y')
	{
		colors.clear();
		yres=set.conf[1]-'0';
		ures=set.conf[2]-'0';
		vres=set.conf[3]-'0';
		int dsize=set.size-'0';
		char* d=(char *)malloc(dsize);
		int y,u,v;

		ctable =  (colormask_t*)malloc((256>>yres)*(256>>ures)*(256>>vres)*sizeof(colormask_t));
		for (y=0;y<256>>yres;y++)
			for (u=0;u<256>>ures;u++)
				for (v=0;v<256>>vres;v++)
				{
					conf.read(d,dsize);

					memcpy(ctable+table_subscript(y,u,v),d,sizeof(colormask_t));

				}

		free(d);
		cout<<"Read Colortable:"<<yres<<"-"<<ures<<"-"<<vres<<endl;
	}
	else
		cout<<"KSegmentator():Invalid or unknown colortable file header"<<endl;
}
void KSegmentator::readRulefile(ifstream & conf)
{
	if (set.conf[0]=='H' && set.conf[1]=='S' && set.conf[2]=='Y' && set.conf[3]=='\n')
	{
		colors.clear();
		yres=YRES;
		ures=URES;
		vres=VRES;
		char peek;
		int c=0;
		int white=0;//White=0 Gray=1 Black=2
		int msk;
		while (!conf.eof())
		{
			peek=conf.peek();

			if (peek=='#'||peek=='\n')
			{
				conf.ignore(128,'\n');
				//cout<<"Comment"<<endl;
			}
			else
			{
				if (white==0)//STEP 1: White rules
				{
					if (conf>>skipws>>msk>>whiteRadius)
					{
						whiteMask=msk;
						cout<<"Read White settings:"<<msk<<" "<<whiteRadius<<endl;
						white++;

					}

				}
				else if (white==1)//STEP 2: Gray rules
				{
					if (conf>>skipws>>msk>>grayRadius)
					{
						grayMask=msk;
						cout<<"Read Gray settings:"<<msk<<" "<<grayRadius<<endl;
						white++;

					}

				}
				else if (white==2)//STEP 3: Black rules
				{
					if (conf>>skipws>>msk>>blackRadius)
					{
						blackMask=msk;
						cout<<"Read Black settings:"<<msk<<" "<<blackRadius<<endl;
						white++;

					}

				}
				else // STEP 4-.... Load specific colors
				{
					//cout<<"Expecting Color"<<endl;
					cclass_t  *a=new   cclass_t();
					if (conf>>msk>>a->HueMin>>a->HueMax>>a->Radius)
					{
						a->mask=msk;

						cout<<"Read color settings:"<<msk<<" "<<a->HueMin<<" "<<a->HueMax<<" "<<a->Radius<<endl;
						c++;
						colors.push_back(*a);

					}
					//if(c==MAXCOLORS) break;
				}

			}


		}
		//for(;c<MAXCOLORS;c++)//Zero out the rest
		//colors[c].HueMin=colors[c].HueMax=colors[c].Radius=0;
		//Create color table to use from rules
		filluptable();


	}
	else
		cout<<"KSegmentator():Invalid or unknown rule file header"<<endl;

}
