#include "KSegmentator.hpp"
#include "core/include/Logger.hpp"
#include <iostream>
#include <vector>


using namespace std;
#define max(x,y) ((x)>(y))?(x):(y)
#define min(x,y) ((x)<(y))?(x):(y)

#define DYRES 3
#define DURES 2
#define DVRES 2

typedef KSegmentator::colormask_t colormask_t;
typedef struct cclass
{
	int HueMin, HueMax;
	int Radius;
	colormask_t mask;
}  cclass_t;

struct RuleSet
{
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

};



void yuv2hsy(const unsigned char yuv[3], unsigned  int hsy[3])
{
	float r, g, b;
	float  mx, mn;
	int y, u, v;
	const  double A = 1.4075, B = 0.3455, C = 0.7169, D = 1.7790;
	y = yuv[0]; //-16;//crop(yuv[0]-16);
	u = yuv[1] - 128;
	v = yuv[2] - 128;
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
	mx = max(max(r, g), b);
	mn = min(min(r, g), b);

	if (mx == mn)
		hsy[0] = 3800; //H is undefined return something off the scale
	else
	{
		if (mx == r)
		{
			hsy[0] = (600.0 * (g - b)) / (mx - mn) + 3600.0;

			if (hsy[0] > 3600.0) //wrap around
				hsy[0] -= 3600.0;
		}
		else if (mx == g)
			hsy[0] = (600.0 * (b - r)) / (mx - mn) + 1200.0;
		else
			hsy[0] = (600.0 * (r - g)) / (mx - mn) + 2400.0;
	}

	hsy[2] = yuv[0]; //Use luma from yuv as a great perceptually relevant estimate of luminance
	hsy[1] = (mn * 256) / yuv[0];
}

void KSegmentator::setLumaScale(float s)
{
	lumascale = s;

	//cout<<"KSegmentator:setLumaScale():"<<s<<endl;
	for(int i = 0; i < LUTsize; i++)
	{
		pYLUT[i] = 0;
		pULUT[i] = 0;
		pVLUT[i] = 0;
	}


	for(int i = 0; i < 256; i++)
	{
		int r = i * lumascale;
		r = r > 255 ? 255 : r;
		r = r < 0 ? 0 : r;
		pYLUT[i >> LUTres] |= rYLUT[r >> LUTres];
		Y_SCALESUB[i] = (unsigned(r)) >> yres;
		r = (i - 128) * lumascale + 128;
		r = r > 255 ? 255 : r;
		r = r < 0 ? 0 : r;
		pULUT[i >> LUTres] |= rULUT[r >> LUTres];
		pVLUT[i >> LUTres] |= rVLUT[r >> LUTres];
		U_SCALESUB[i] = (unsigned(r)) >> ures;
		V_SCALESUB[i] = (unsigned(r)) >> vres;
	}
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
KSegmentator::colormask_t ruleFileClassifyPixel(struct RuleSet const& r, unsigned char yuv[3])
{
	unsigned int hsy[3];
	int dists, distl;
	yuv2hsy(yuv, hsy);

	if ((int)hsy[2] < r.blackRadius)
		return r.blackMask;

	//Color Radius
	dists = 24 - hsy[1];
	distl = 128 - hsy[2]; //(0x80<hsy[2])?(hsy[2]-0x80):hsy[2];

	//cout<<"colors.size():"<<colors.size()<<endl;
	for (int i = 0; i < (int)r.colors.size(); i++)
	{
		//cout<<colors[i].HueMin<<endl;
		if (r.colors[i].HueMin < (int) hsy[0] && (int) hsy[0] < r.colors[i].HueMax && ( dists * dists + distl * distl) < r.colors[i].Radius)
		{
			//cout<<" "<<((int)colors[i].HueMin)<< " ";
			return r.colors[i].mask;
		}
	}

	//White Radius
	dists = 256 - ((int)hsy[1]);
	distl = 256 - ((int)hsy[2]);

	if (dists * dists + distl * distl < r.whiteRadius)
	{
		//cout<<hsy[0]<<" "<<hsy[1]<<" "<<hsy[2]<<endl;
		return r.whiteMask;
	}

	//Gray
	if ((int)hsy[1] > r.grayRadius)
	{
		//cout<<"g ";
		//cout<<hsy[0]<<" "<<hsy[1]<<" "<<hsy[2]<<endl;
		return r.grayMask;
	}

	return 0;
}

void KSegmentator::attachToIplImage(KImageConst const& data)
{
	//string m="Seg times:"+_toString(fetch.tv_nsec)+" "+_toString(segment.tv_nsec);
	//fetch.tv_nsec=0;
	//segment.tv_nsec=0;
	//Logger::Instance().WriteMsg("KSegmentator",m,Logger::Warning);
	dataPointer = data.imageData;
	widthmult2 = data.width * 2;
	width = data.width;
	height = data.height;
}

KSegmentator::KSegmentator(std::ifstream &conf)
{
	lumascale = 0;
	conf.read(reinterpret_cast<char *>(&set), sizeof(set));

	//Check that file is indeed Segmentation configuration
	if (set.ID[0] != 'K' || set.ID[1] != 'S')
	{
		cout << "KSegmentator(): Invalid configuration file" << endl;
		LogEntry(LogLevel::Error,"KSegmentator")<< "Invalid configuration file";
		return;
	}

	if (set.size - '0' > (int)sizeof(colormask_t))
	{
		cout << "KSegmentator(): Not enough length in colormask_t" << endl;
		LogEntry(LogLevel::Error,"KSegmentator")<< "Not enough length in colormask_t";
		return;
	}

	readComment(conf);
	readCalibration(conf);
	readColorInfo(conf);

	for(int i = 0 ; i < LUTsize; i++)
	{
		rYLUT[i] = rULUT[i] = rVLUT[i] = 0;
	}

	if (set.ruletype == 'R')
		readRulefile(conf);
	else if (set.ruletype == 'C')
		readColorTable(conf);

	setLumaScale(1.0);


	for (int v = 0; v < 256; v++)
		for (int u = 0; u < 256; u++)
			for (int y = 0; y < 256; y++)
			{
				colormask_t val = * (ctableAccess(V_SCALESUB[v], U_SCALESUB[u], Y_SCALESUB[y]));
				rYLUT[y >> LUTres] |= val;
				rULUT[u >> LUTres] |= val;
				rVLUT[v >> LUTres] |= val;
			}
	setLumaScale(1.0); //WE NEED IT BOTH TIMES
}

void KSegmentator::readComment(ifstream & conf)
{
	char com[1024];
	conf.getline(com, 1024);
	LogEntry(LogLevel::Error,"KSegmentator")<< "Color table comment:" << std::string(com);
}

void KSegmentator::readCalibration(ifstream & conf)
{
	for (int i = 0; i < set.calLines - '0'; i++)
		conf.ignore(256, '\n');
}
void KSegmentator::readColorInfo(ifstream & conf)
{
	for (int i = 0; i < set.colorLines - '0'; i++)
		conf.ignore(256, '\n');
}

void KSegmentator::readColorTable(ifstream & conf)
{
	if (set.conf[0] == 'Y')
	{
		yres = set.conf[1] - '0';
		ysize = 256 >> yres;
		ures = set.conf[2] - '0';
		usize = 256 >> ures;
		vres = set.conf[3] - '0';
		vsize = 256 >> vres;
		int dsize = set.size - '0';
		colormask_t t, r;
		t = 0;
		char *dest = ((char*)(&t)) + sizeof(colormask_t) - dsize; //For little endian systems like x86 :)
		int y, u, v;
		colormask_t* nctable = new colormask_t[ysize * usize * vsize];

		for (y = 0; y < ysize; y++)
			for (u = 0; u < usize; u++)
				for (v = 0; v < vsize; v++)
				{
					conf.read(dest, dsize);
					//if(y==128>>yres)
					//cout<<"t:"<<(int)t<<endl;
					r = t;
					//Store it :)
					*(nctable + y + u * ysize + v * usize * ysize) = r;
				}

		ctable = nctable;
		cout << "Read Colortable:" << ysize << "-" << usize << "-" << vsize << endl;
	}
	else
		cout << "KSegmentator():Invalid or unknown colortable file header" << endl;
}

KSegmentator::KSegmentator(int nyres, int nures, int nvres)
{
	lumascale = 0;
	set.ID[0] = 'K';
	set.ID[1] = 'S';
	set.ruletype = 'C';
	set.size = '0' + ((int)sizeof(colormask_t));
	set.calType = 'A';
	set.calLines = '0';
	set.colorInfo = 'A';
	set.colorLines = '0';
	set.conf[0] = 'Y';
	set.conf[1] = '0' + nyres;
	set.conf[2] = '0' + nures;
	set.conf[3] = '0' + nvres;
	yres = nyres;
	ysize = 256 >> yres;
	ures = nures;
	usize = 256 >> ures;
	vres = nvres;
	vsize = 256 >> vres;
	colormask_t* nctable = new colormask_t[ysize * usize * vsize];
	int y, u, v;
	for (y = 0; y < ysize; y++)
		for (u = 0; u < usize; u++)
			for (v = 0; v < vsize; v++)
			{
				*(nctable + y + u * ysize + v * usize * ysize) = black;
			}

	ctable = nctable;

	for(int i = 0 ; i < LUTsize; i++)
	{
		rYLUT[i] = rULUT[i] = rVLUT[i] = 0;
	}

	setLumaScale(1);//Default setting
}

void KSegmentator::writeFile(std::ofstream &of, const std::string  comment) const
{
	of.write(reinterpret_cast<const char *>(&set), sizeof(set));
	of.write(comment.c_str(),comment.size());
	of.put('\n');
	//int y, u, v;
	//of.write(reinterpret_cast<colormask_t *>(ctable),(256>>yres)*(256>>ures)*(256>>vres)*sizeof(colormask_t));

	for (int y = 0; y < ysize; y++){
		for (int u = 0; u < usize; u++){
			for (int v = 0; v < vsize; v++){
				of.put(*(ctable + y + u * ysize + v * usize * ysize));
			}
		}
	}
}

void KSegmentator::readRulefile(ifstream & conf)
{
	if (set.conf[0] == 'H' && set.conf[1] == 'S' && set.conf[2] == 'Y' && set.conf[3] == '\n')
	{
		struct RuleSet r;
		r.colors.clear();
		yres = DYRES;
		ysize = 256 >> yres;
		ures = DYRES;
		usize = 256 >> ures;
		vres = DYRES;
		vsize = 256 >> vres;
		char peek;
		int c = 0;
		int white = 0; //White=0 Gray=1 Black=2
		int msk;

		while (!conf.eof())
		{
			peek = conf.peek();

			if (peek == '#' || peek == '\n')
			{
				conf.ignore(128, '\n');
				//cout<<"Comment"<<endl;
			}
			else
			{
				if (white == 0) //STEP 1: White rules
				{
					if (conf >> skipws >> msk >> r.whiteRadius)
					{
						r.whiteMask = msk;
						cout << "Read White settings:" << (msk) << " " << (r.whiteRadius) << endl;
						white++;
					}
				}
				else if (white == 1) //STEP 2: Gray rules
				{
					if (conf >> skipws >> msk >> r.grayRadius)
					{
						r.grayMask = msk;
						cout << "Read Gray settings:" << msk << " " << (r.grayRadius) << endl;
						white++;
					}
				}
				else if (white == 2) //STEP 3: Black rules
				{
					if (conf >> skipws >> msk >> r.blackRadius)
					{
						r.blackMask = msk;
						cout << "Read Black settings:" << msk << " " << r.blackRadius << endl;
						white++;
					}
				}
				else // STEP 4-.... Load specific colors
				{
					//cout<<"Expecting Color"<<endl;
					cclass_t  a;

					if (conf >> msk >> a.HueMin >> a.HueMax >> a.Radius)
					{
						a.mask = msk;
						cout << "Read color settings:" << msk << " " << a.HueMin << " " << a.HueMax << " " << a.Radius << endl;
						c++;
						r.colors.push_back(a);
					}
				}
			}
		}

		unsigned char * nctable = new colormask_t[ysize * usize * vsize];
		unsigned char yuv[3];

		for (int v = 0; v < vsize; v++)
			for (int u = 0; u < usize; u++)
				for (int y = 0; y < ysize; y++)
				{
					yuv[0] = y;
					yuv[1] = u;
					yuv[2] = v;
					colormask_t val = ruleFileClassifyPixel(r, yuv);
					//Store it :)
					*(nctable + y + (u + v * usize)*ysize) = val;
				}

		ctable = nctable;
	}
	else
		cout << "KSegmentator():Invalid or unknown rule file header" << endl;
}
