#ifndef KSegmentator_H__
#define KSegmentator_H__

// basic file operations
#include <iostream>
#include <fstream>


#include <time.h>




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
        * Constructor: load configuration
        */
		KSegmentator(std::ifstream &conf);
		/**
		 * Constructor : empty table
		 */
		KSegmentator(int yres,int ures,int vres);

		~KSegmentator();

		void writeFile(std::ofstream &of,std::string const comment) const;
		/**
        * Set Luminace scale of the environtment, to account for
        * luminance variations (ie exposure differences from ideal)
        */
		void setLumaScale(float s);
		float getLumaScale(){
			return lumascale;
		}

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

			if(!( pYLUT[y>>LUTres]& pULUT[u>>LUTres]& pVLUT[v>>LUTres]&hint))
				return 0;


			//Well, it does. Look for it

			return * ctableAccess(v,u,y);
        }

         //This does the job
        inline colormask_t classifyNoPrecheck(unsigned char  y, unsigned char  u , unsigned  char   v) const
        {
			//Well, it does. Look for it

			return * ctableAccess(v,u,y);
        }

        inline  colormask_t  * ctableAccess( const unsigned char v, const unsigned char u, const unsigned char y)
        {
        	//y>>=yres;u>>=ures;v>>=vres;
			//return ctable + y + u*ysize+v*ysize*usize;
        	return ctable+(unsigned(Y_SCALESUB[y]))+
					((unsigned(U_SCALESUB[u]))<<(8-yres))+((unsigned(V_SCALESUB[v]))<<(16-yres-ures));
        }
          inline  colormask_t const * ctableAccess( const unsigned char v, const unsigned char u, const unsigned char y) const
        {
        	//y>>=yres;u>>=ures;v>>=vres;
			//return ctable + y + u*ysize+v*ysize*usize;
        	return ctable+(unsigned(Y_SCALESUB[y]))+
					((unsigned(U_SCALESUB[u]))<<(8-yres))+((unsigned(V_SCALESUB[v]))<<(16-yres-ures));
        }

	private:
		static const unsigned char LUTres=2,LUTsize=64;//Carefull. LUTSIZE=256>>LUTRES


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
		colormask_t rYLUT[LUTsize],rULUT[LUTsize],rVLUT[LUTsize];
		//Value prelookup,exposure-scaled values
		colormask_t pYLUT[LUTsize],pULUT[LUTsize],pVLUT[LUTsize];
		//Value transformation, scale by lumascale and subsampled
		colormask_t Y_SCALESUB[256],U_SCALESUB[256],V_SCALESUB[256];

		int yres,ysize;
		int ures,usize;
		int vres,vsize;

		colormask_t  *ctable;




};

#endif //KSegmentator_H__
