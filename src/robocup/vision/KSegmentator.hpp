#ifndef KSEGMENTATOR_HPP
#define KSEGMENTATOR_HPP

// basic file operations
#include <iostream>
#include <fstream>
#include <memory>

#include <time.h>
#include "core/elements/vision/KImage.h"




#define CACHETAG 0x1F

/**
 * Basic segmentation class, loads up a custom configuration file in the constructor
 * and fills up a colortable using the rules defined there
 */



//Assign disjoint bits to the color values to compose colormasks
enum colors
{
    red = 1, blue = 2 , green = 4 , skyblue = 8 , yellow = 16, orange = 32, white = 64, black = 128
};


class KSegmentator
{
public:
	typedef unsigned char colormask_t;// Color class values
	/**
	* One and only constructor: load configuration
	*/
	KSegmentator(std::ifstream &conf);
	/**
	* Link Segmentator with an (compatible) IplImage object, subsequent calls to classifyPixel()
	* are valid and produce a valid segmentation result
	*/
	KSegmentator(int yres, int ures, int vres);
	/**
	* destructor
	**/
	~KSegmentator(){
		delete[] ctable;
	}

	void writeFile(std::ofstream &of, std::string const comment) const;
	void attachToIplImage(KImageConst const& data);
	//Direct access to the colortable
	inline  colormask_t  * ctableAccessDirect( const unsigned char v, const unsigned char u, const unsigned char y)
	{
		//y>>=yres;u>>=ures;v>>=vres;
		//return ctable + y + u*ysize+v*ysize*usize;
		return ctable + unsigned(y >> yres) + unsigned((u >> ures) << (8 - yres)) + unsigned((v >> vres) << (16 - yres - ures));
	}
	/**
	*Clasify a pixel of the image: i is row and j is column, ie i is 0-640 and j is 0-480 in a 640x480 image
	*/
	inline colormask_t classifyPixel(const int i, const int j, const colormask_t h) const
	{
		unsigned char y, u, v;
		//timespec s,e;
		//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &s)
		//widthStep
		y = *(dataPointer + j * widthmult2 + (i << 1)); //Y is right where we want it
		//a block is a yuyv sequence, and from that block extract the second (Y) and 4th byte (V)
		int startofBlock = j * widthmult2 + ((i >> 1) << 2); //every 2 pixels (i/2) swap block (size of block=4)
		// cout<<"sob"<<endl;
		u = *(dataPointer + startofBlock + 1);
		// cout<<"u"<<endl;
		v = *(dataPointer + startofBlock + 3);
		//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &e);
		//fetch.tv_nsec+=ttdiff(s,e).tv_nsec;
		return classifyWithPrecheck(y, u, v, h);
	};
	inline void prefetchPixelData(const int i, const int j)
	{
		int startofBlock = j * widthmult2 + ((i >> 1) << 2); //every 2 pixels (i/2) swap block (size of block=4)
		__builtin_prefetch(dataPointer + startofBlock);
		//How much luck do we NOT have :)
		//if((unsigned(dataPointer+startofBlock) & ~(CACHETAG) )!=(unsigned(dataPointer+startofBlock+3)& ~(CACHETAG) ))
		//	__builtin_prefetch(dataPointer+startofBlock+3);
	}
	/**
	* Set Luminace scale of the environtment, to account for
	* luminance variations (ie exposure differences from ideal)
	*/
	void setLumaScale(float s);
	float getLumaScale()
	{
		return lumascale;
	}

	inline colormask_t classifyNoPrecheck(unsigned char  y, unsigned char  u , unsigned  char   v) const
	{
		//Well, it does. Look for it
		return * ctableAccess(V_SCALESUB[v], U_SCALESUB[u], Y_SCALESUB[y]);
	}
	colormask_t *ctable;
private:

	static const unsigned char LUTres = 2, LUTsize = 64; //Carefull. LUTSIZE=256>>LUTRES
	//Pointer to attached IplImage data

	int yres, ysize;
	int ures, usize;
	int vres, vsize;
	const char  *dataPointer;
	int widthmult2;//width*2 :D
	int width;//:)
	int height;
	//Scale up Y component to compensate for exposure or lighting variations
	float lumascale;

	struct SegHeader
	{
		char ID[2];//==KS
		char ruletype;//==R/C
		char size;//=1?
		char calType;//A:Autowb
		char calLines;
		char colorInfo;//A:Aliases
		char colorLines;
		char conf[4];
	};
	struct SegHeader set;



	void readComment(std::ifstream &conf);
	void readCalibration(std::ifstream &conf);
	void readColorInfo(std::ifstream &conf);
	//Based on header, one of these should do the trick :)
	void readRulefile(std::ifstream & conf);
	void readColorTable(std::ifstream & conf);


	//Value prelookup,reference values
	colormask_t rYLUT[LUTsize], rULUT[LUTsize], rVLUT[LUTsize];
	//Value prelookup,exposure-scaled values
	colormask_t pYLUT[LUTsize], pULUT[LUTsize], pVLUT[LUTsize];
	//Value transformation, scale by lumascale and subsampled
	colormask_t Y_SCALESUB[256], U_SCALESUB[256], V_SCALESUB[256];



	//This does the job
	inline colormask_t classifyWithPrecheck(unsigned char  y, unsigned char  u , unsigned  char   v, colormask_t const hint) const
	{
		//std::cout<<"LUTS:"<<(int)YLUT[y]<<" "<<(int)ULUT[u]<<" "<<(int)VLUT[v]<<" "<<((int)YLUT[y]&ULUT[u]&VLUT[v]);
		//timespec s,e;
		//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &s);
		//colormask_t t=YLUT[y]&ULUT[u]&VLUT[v];
		//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &e);
		//segment.tv_nsec+=ttdiff(s,e).tv_nsec;

		//Precheck... Does it SEEM at lest to contain needed values?

		if(!( pYLUT[y >> LUTres]& pULUT[u >> LUTres]& pVLUT[v >> LUTres]&hint))
			return 0;
		//Well, it does. Look for it
		return * ctableAccess(V_SCALESUB[v], U_SCALESUB[u], Y_SCALESUB[y]);
	}

	inline  colormask_t const * ctableAccess( const unsigned char v, const unsigned char u, const unsigned char y) const
	{
		//y>>=yres;u>>=ures;v>>=vres;
		//return ctable + y + u*ysize+v*ysize*usize;
		return ctable + (unsigned(y)) + ((unsigned(u)) << (8 - yres)) + ((unsigned(v)) << (16 - yres - ures));
	}


};

#endif //KSegmentator_H__
