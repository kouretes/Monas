#ifndef KSegmentator_H__
#define KSegmentator_H__

// basic file operations
#include <iostream>
#include <fstream>


#include "opencv/cv.h"




/**
 * Basic segmentation class, loads up a custom configuration file in the constructor
 * and fills up a colortable using the rules defined there
 */

//Assign disjoint bits to the color values to compose colormasks
enum colors
{
	red=1, blue=2 , green=4 , skyblue = 8 , yellow=16, orange=32, white=64, black=128
};


class KSegmentator{
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

        void attachToIplImage(IplImage *data);
		/**
		*Clasify a pixel of the image: i is row and j is column, ie i is 0-640 and j is 0-480 in a 640x480 image
		*/
        inline colormask_t classifyPixel(int i,int j)
        {
        	if (classifyFunc==NULL)
				return 0;
			return ((*this).*(classifyFunc))(i,j);
        };
        /**
        * Set Luminace scale of the environtment, to account for
        * luminance variations (ie exposure differences from ideal)
        */
		void setLumaScale(float s);
	private:

		//Pointer to attached IplImage data
		char *dataPointer;
		int widthmult2;//width*2 :D
		int width;//:)
		//Scale up Y component to compensate for exposure or lighting variations
		float lumascale;

		enum colorValues
		{
			red_ = 1, blue_ = 2, green_ = 3, skyblue_ = 4, yellow_ = 5, orange_ = 6, white_ = 7, black_  = 8
		};

        struct SegHeader{
            char ID[2];//==KS
            char ruletype;//==R/C
            char size;//=1?
            char calType;//A:Autowb
            char calLines;
            char colorInfo;//A:Aliases
            char colorLines;
            char conf[4];//HSY\n OR Y[yres][ures][vres]
        };
		struct SegHeader set;

		//Value lookup
		colormask_t YLUT[256];
		colormask_t ULUT[256];
		colormask_t VLUT[256];
		//Same, prescaled :), reference values
		colormask_t rYLUT[256];
		colormask_t rULUT[256];
		colormask_t rVLUT[256];

		void readComment(std::ifstream &conf);
		void readCalibration(std::ifstream &conf);
		void readColorInfo(std::ifstream &conf);
		//Based on header, one of these should do the trick :)
		void readRulefile(std::ifstream & conf);
		void readColorTable(std::ifstream & conf);

		//pointer to classifierfuncUsed by classifyPixel
		typedef colormask_t (KSegmentator::*classFncPtr)(int, int);
		classFncPtr classifyFunc;

		//matches :
		colormask_t classify444(int i,int j)
		{
			return classifyLUT(*(dataPointer+j*width*3+i*3),*(dataPointer+j*width*3+i*3+1),*(dataPointer+j*width*3+i*3+2));
		};
        colormask_t classify422(int i,int j)
        {
			unsigned char y,u,v;

			//widthStep
			y=*(dataPointer+j*widthmult2+(i<<1));//Y is right where we want it

			//a block is a yuyv sequence, and from that block extract the second (Y) and 4th byte (V)
			int startofBlock =j*widthmult2+ ((i>>1)<<2); //every 2 pixels (i/2) swap block (size of block=4)
			// cout<<"sob"<<endl;
			u=*(dataPointer+startofBlock+1);
			// cout<<"u"<<endl;
			v= *(dataPointer+startofBlock+3);

			return classifyLUT(y,u,v);

        }
        //This does the job
        inline colormask_t classifyLUT(unsigned char y, unsigned char u , unsigned  char v)
        {
        	//std::cout<<"LUTS:"<<(int)YLUT[y]<<" "<<(int)ULUT[u]<<" "<<(int)VLUT[v]<<" "<<((int)YLUT[y]&ULUT[u]&VLUT[v]);
			return YLUT[y]&ULUT[u]&VLUT[v];
        }


};

#endif //KSegmentator_H__
