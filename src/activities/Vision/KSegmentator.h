#ifndef KSegmentator_H__
#define KSegmentator_H__

// basic file operations
#include <iostream>
#include <fstream>

#include "alvalue.h"
#include "alvision/alvisiondefinitions.h"
#include "alvideodeviceproxy.h"
#include "opencv/cv.h"

//Colortable resolution: every value represents how many bits to shift to the right on each dimension:
// ie 4x2x2 means table is 256>>4 x 256>>2 x 256>>2 = 16x64x64
// Generaly prefer to have more resolution in chroma components (U,V) since
#define YRES 4
#define URES 1
#define VRES 1





/**
 * Basic segmentation class, loads up a custom configuration file in the constructor
 * and fills up a colortable using the rules defined there
 */

class KSegmentator{
	public:


        typedef unsigned char colormask_t;// Color class values
        typedef struct cclass {
            int HueMin,HueMax;
            int Radius;
            colormask_t mask;
        }  cclass_t;
		KSegmentator(std::ifstream &conf);
		~KSegmentator(){ if(ctable!=NULL) free(ctable);};//Free colortable

		static void yuv2hsy(unsigned char yuv[3], unsigned  int hsy[3]);
		//Use the colortable to classify a pixel
		colormask_t classifyPixel(unsigned char yuv[3]);
		/*
                 *Clasify a pixel of the image: i is row and j is column, ie i is 0-640 and j is 0-480 in a 640x480 image
		 * type is KYUVColorspace or KYUV422InterlacedColorspace ONLY
                 */
        void attachToIplImage(IplImage *data,int type);
        colormask_t classifyYUV422(int i,int j);
        colormask_t classifyYUV(int,int j);
        colormask_t classifyPixel(int i,int j);
		//colormask_t classifyPixel(IplImage*data, int i,int j,int type);
		//Inform segmentator of Luma in environment;
		void setLumaScale(float s);
	private:
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
		int yres;
		int ures;
		int vres;
		//White Region: radius^2 from s=256,l=256  so the center of a the sphere is at the top of HSL space
		int whiteRadius;
		//Value to return when color is in white sphere
		colormask_t whiteMask;
		//Gray cylinder a cylinder from the center of HSL space  (s=256 ,see notes above) to the exterior of the cylinder s=grayRadius
		int grayRadius;
		//Value to return upon detecting a gray pixel (in gray cylindrical region)
		colormask_t grayMask;
		//When l<blackRadius color is clasified as black. This is a slice of the whole HSL cylinder (a bottom part of the cylinder)
		int blackRadius;
		//Value to return for black
		colormask_t blackMask;
		//User color classes, represent a "slice" in the HSL cylinder, and a radius is defined as a distance from s=0,l=1 in the cylinder
		std::vector<cclass_t> colors;
		//Use the RULES (not the table) to classify a pixel
		colormask_t calculatePixelFromRules(unsigned char yuv[3]);
		//Fill up a color table from the rules (uses only calculatePixel())
		void filluptable();

		colormask_t *ctable;//The colortable

		//Scale up Y component to compensate for exposure or lighting
		float lumascale;
		void readComment(std::ifstream &conf);
		void readCalibration(std::ifstream &conf);
		void readColorInfo(std::ifstream &conf);
		void readRulefile(std::ifstream & conf);
		void readColorTable(std::ifstream & conf);

		//pointer to classifierfuncUsed by classifyPixel

		typedef colormask_t (KSegmentator::*classFncPtr)(int, int);

		classFncPtr classifyFunc;
		//Pointer to attached IplImage data
		char *dataPointer;
		int widthmult2;//width*2 :D
		int width;//:)
		//Value prelookup
		char YLookup[256];
		char UVLookp[256];

};










#endif //KSegmentator_H__
