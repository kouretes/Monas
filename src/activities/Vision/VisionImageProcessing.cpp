#include "Vision.h"
#ifndef DEBUGVISION
#define BOOST_PROFILING_OFF
#endif
#include "profiler.hpp"

#include "architecture/archConfig.h"
#include <cmath>
#include "sys/stat.h"
#include "tools/logger.h"
#include "tools/XMLConfig.h"
#include "tools/toString.h"
#include "hal/syscall.h"
#include <vector>
#include "messages/motion.pb.h"

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#define VISIBLE __attribute__ ((visibility("default")))
#else
#define VISIBLE
#endif

#define PREFETCH 2
#define CvDist(pa,pb) sqrt(  ( ((pa).x-(pb).x )*((pa).x-(pb).x )  )+( ((pa).y-(pb).y )*((pa).y-(pb).y) )  )
#define colorIsA(x,y) (((x)&(y))!=0)

inline measurement mWeightedMean(measurement m1,measurement m2)
{
	measurement res;
	/*
	(d1.mean*d2.var+d2.mean*d1.var)/(d1.var+d2.var);
		distance.var=d1.var*d2.var/(d1.var+d2.var);
		*/
	res.mean=(m1.mean*m2.var+m2.mean*m1.var)/(m1.var+m2.var);
	res.var=(m1.var*m2.var)/(m1.var+m2.var)*(sqrd(m1.mean-res.mean)/m1.var+sqrd(m2.mean-res.mean)/m2.var);
	return res;
}
inline measurement mWeightedMean(const std::vector<measurement> &l)
{
	float n=0,d=0,c=0;
	measurement res;
	if(l.size()==1)
		return l[0];
	for(unsigned i=0;i<l.size();i++)
	{
		n+=l[i].mean/l[i].var;
		d+=1.0f/l[i].var;
		//cout<<l[i].mean<<" "<<l[i].var<<endl;
	}


	res.mean=n/d;
	for(unsigned i=0;i<l.size();i++)
		c+=sqrd(l[i].mean-res.mean)/l[i].var;
	res.var=(c/d)/(l.size()-1);
	return res;


}


class inttracer_s: public KVecInt2
{
	int s,c;
	int e,t,i;
	bool steep;
	//int idx,idy;

	public:
	void init(int a,int b)
	{
		x=a;
		y=b;
		e=0;
	};
	void initVelocity(float dx,float dy)
	{
		int idx=abs(dx*32768);
		int idy=abs(dy*32768);
		if (abs(dx)>=abs(dy))
			steep=false;
		else
			steep=true;
		if (!steep)
		{
			s= dx>0?1:-1;
			c=dy>0?1:-1;
			t=idx;
			i=2*idy;
			//Correction


		}
		else
		{
			s= dy>0?1:-1;
			c=dx>0?1:-1;
			t=idy;
			i=2*idx;

		};
		e=0;
	};
	void setScale(int scale)
	{
		s=(s<0?-1:1)*scale;
		c=(c<0?-1:1)*scale;
	};
	void step()
	{
		e+=i;
		if (!steep)
		{
			x+=s;
			if (e>t)
			{
				y+=c;
				e-=t;
			}

		}
		else
		{
			y+=s;
			if (e>t)
			{
				x+=c;
				e-=t;
			}

		}
	}

};


typedef struct inttracer_s tracer_t;


typedef struct
{
	tracer_t gtrc;
	KVecInt2 lastpoint ;
	bool ballfound,yfound,bfound;
	int cntother; bool done;
	int step;
} linestruct;

void Vision::gridScan(const KSegmentator::colormask_t color)
{

	//Horizontal + Vertical Scan
#ifdef DEBUGVISION
	boost::prof::profiler::generate_report();
	cout<<"Starting Grid scan"<<endl;
#endif

	KVecFloat2 im;
	KVecFloat2 c;
	KVecFloat3 c3d;
	KSegmentator::colormask_t tempcolor;
	//static int startx=0;
	int linestep=(rawImage->width-2*config.bordersize)/(config.scanV-1);
	int align=(rawImage->width-linestep*(config.scanV-1) )>>1;
	//float d;

	std::vector<linestruct> l;
	//((int)config.scanV);
	l.reserve((int)config.scanV);
	linestruct t;

	int sx=config.bordersize+align;
	int vstep=(sqrt(rawImage->height-2*config.bordersize-config.scanH*config.subsampling)-1)/2;
	//Fix initial scanline positions :)
	int linesdone=0;
	//Prefetch first batch


	//Prefetch prologue
	for(int i=0;i<PREFETCH-1;++i)
		prepSeg(sx+i*linestep,rawImage->height - config.bordersize-1);

	for(int i=0 ; i< config.scanV;i++)
	{
		l.push_back(t);
		l[i].gtrc.init(sx,rawImage->height - config.bordersize-1);
		l[i].lastpoint=l[i].gtrc;
		sx+=linestep;
		l[i].gtrc.initVelocity(Vup.x,Vup.y);
		l[i].gtrc.setScale(config.subsampling+vstep);
		l[i].ballfound=false;
		l[i].yfound=false;
		l[i].bfound=false;
		l[i].cntother=0;
		l[i].done=false;
		l[i].step=config.subsampling+vstep;

	}
	//cout<<"init"<<endl;

	while(linesdone<config.scanV)
	{
		//cout<<"while"<<endl;
		for(unsigned i=0 ; i< l.size();i++)
		{

			//cout<<"for"<<endl;
			linestruct &thisl=l[i];

			//Before actually segmenting this pixel, prefetch the next one
			if(l[(i+PREFETCH)%l.size()].done==false)
				prepSeg(l[(i+PREFETCH)%l.size()].gtrc.x,l[(i+PREFETCH)%l.size()].gtrc.y);

			if(thisl.done==true)
				continue;
			if(thisl.gtrc.x<config.bordersize||thisl.gtrc.y<config.bordersize||
				thisl.gtrc.x>rawImage->width-1- config.bordersize ||thisl.gtrc.y>rawImage->height-1- config.bordersize ||
				!validpixel(thisl.gtrc.x,thisl.gtrc.y))
			{

				thisl.done=true;
				linesdone++;
				continue;
			}


			tempcolor = doSeg(thisl.gtrc.x,thisl.gtrc.y, white | orange | green| yellow |skyblue);

			//cout<<"doseg:"<<(int)tempcolor<<endl;
			if (colorIsA(tempcolor,green))//
			{
				//cntwhitegreenpixels++;
				//cntwhitegreenorangepixels++;
				thisl.cntother=0;
				thisl.lastpoint=thisl.gtrc;
				thisl.ballfound=false;
				thisl.yfound=false;
				thisl.bfound=false;
			}
			else if(colorIsA(tempcolor,white))
			{
				thisl.cntother++;//hmm... white? valid but test for distance
				thisl.lastpoint=thisl.gtrc;
				thisl.ballfound=false;
				thisl.yfound=false;
				thisl.bfound=false;
			}
			else if (colorIsA(tempcolor,color))
			{
				//cntwhitegreenorangepixels++;
				thisl.cntother=0;
				thisl.yfound=false;
				thisl.bfound=false;

			}
			else
			{
				//ballskip=0;
				thisl.cntother++;
				thisl.ballfound=false;
			}
			//Check see distance
			if (thisl.cntother>config.pixeltol)//No continuity, break
			{
				//thisl.done=true;
				//linesdone++;
				//continue;
				c=imageToCamera(thisl.gtrc);
				c3d=kinext.camera2dToGroundProjection(c,0);
				float d=sqrt(sqrd(c3d(0))+sqrd(c3d(1)));
				if(c3d(2)>=0)//Looking up :)
				{
					//cout<<"--dup"<<endl;

					thisl.done=true;
					linesdone++;
					continue;
				}

				if (d>=config.seedistance)
				{
					//cout<<"--d"<<endl;
					thisl.done=true;
					linesdone++;
					continue;
				}
				c=imageToCamera(thisl.lastpoint);

				c3d=kinext.camera2dToGroundProjection(c,0);
				if(c3d(2)>=0)//Looking up :)
				{
					//cout<<"--d1up"<<endl;
					thisl.done=true;
					linesdone++;
					continue;
				}
				float d1=sqrt(sqrd(c3d(0))+sqrd(c3d(1)));


				if (d1>=config.seedistance)
				{
					//cout<<"--d1"<<endl;
					thisl.done=true;
					linesdone++;
					continue;
				}

				/*
				//thisl.cntother=0;
				if(!thisl.lastpoint.isInitialized())//No good point, matrix uninitialized
				{
					thisl.done=true;
					linesdone++;
					continue;
				}
				c=imageToCamera(thisl.lastpoint);

				c3d=kinext.camera2dToGround(c);
				if(c3d(2)>=0)//Looking up :)
				{
					thisl.done=true;
					linesdone++;
					continue;
				}*/

				if (abs(d1-d)>config.skipdistance)
				{
					//cout<<"break"<<endl;
					//cout<<"--skip"<<endl;
					obstacles.push_back(thisl.gtrc);	//c3d.prettyPrint();
					thisl.done=true;
					linesdone++;
					continue;
				}
			}
			if (colorIsA(tempcolor,color) &&thisl.ballfound==false)
			{
				ballpixels.push_back(thisl.gtrc);
				//cntwhitegreenpixels=0;
				thisl.ballfound=true;
				//continue;
				//ballpixel = j;
			}
			else if(colorIsA(tempcolor,yellow)&&thisl.yfound==false)
			{
				//ballpixels.push_back(tmpPoint);
				ygoalpost.push_back(thisl.gtrc);
				thisl.yfound=true;
			}
			else if(colorIsA(tempcolor,skyblue)&&thisl.bfound==false)
			{
				//ballpixels.push_back(tmpPoint);
				bgoalpost.push_back(thisl.gtrc);
				thisl.bfound=true;
			}


			//Find next pixel
			thisl.gtrc.step();
			thisl.step--;
			if (thisl.step<config.minH) thisl.step=config.minH;
			thisl.gtrc.setScale(thisl.step);
			//std::cout<<"Step:"<<thisl.step<<endl;
			//stepx=2;

		}

	}
	//Now follow the lines

#ifdef DEBUGVISION
cout<<"End Grid scan"<<endl;
#endif

}


/**
 * Hard Decision: Is it good enough for a ball?
 */

bool Vision::calculateValidBall(balldata_t const ball, KSegmentator::colormask_t c) const
{
	BOOST_PROFILER("calculateValidBall");

	unsigned int  gd = 0,bd=0;
//	float innerrad = ball.cr * 0.707;
	int top,left,right,bot;
	const int sub=config.subsampling;

	const int cr=floor(ball.cr*2+0.5);
	const int crd=floor(sqrd(ball.cr)+0.5);
	const int cx=floor(ball.x+0.5);
	const int cy=floor(ball.y+0.5);

	top=ball.y-cr;
	bot=ball.y+cr;
	left=ball.x-cr;
	right=ball.x+cr;
	top=top<config.bordersize?config.bordersize:top;
	left=left<config.bordersize?config.bordersize:left;
	bot=bot>rawImage->height-1 -config.bordersize?rawImage->height-1 -config.bordersize:bot;
	right=right>rawImage->width-1 - config.bordersize?rawImage->width -1 - config.bordersize:right;

	//cout<<"Ball:"<<cx<<","<<cy<<","<<cr<<endl;
	//cout<<"Limits:"<<top<<","<<bot<<","<<left<<","<<right<<endl;
	//cout<<"crs:"<<cr<<" "<<crd<<endl;

	for(int i=0;i<PREFETCH-1;++i)
		prepSeg(left+sub*i,top);
	for(int y=top;y<=bot;y+=sub)
		for(int x=left;x<=right;x+=sub)
		{
			if(x+PREFETCH*sub<=right)
				prepSeg(x+PREFETCH*sub,y);
			else
				prepSeg(left+PREFETCH*sub-sub,y+sub);

			bool iscolor=colorIsA(doSeg(x,y,c) , c);
			//cout<<"x,y,c:"<<x<<","<<y<<","<<iscolor<<endl;
			if(!iscolor)
				continue;
			if(sqrd(cx-x)+sqrd(cy-y)-2<=crd)
				gd++;
			else
				bd++;

		}

	const int ttl = floor((bot-top+1)/sub)*floor((right-left+1)/sub);
	const int inside=floor(KMat::transformations::PI*cr*cr/4.0)/(sub*sub);
	float ratio = ((float) gd) / (inside);
	//cout<<"Validratio:"<<ratio<<endl;
	if (ratio < 0.65)
		return false;
	ratio=((float) bd) / (ttl-inside);
	//cout<<"Validratio2:"<<ratio<<endl;
	if (ratio > 0.25)
		return false;
	return true;
	//Inner circle
	/*
	for (int j =ball. y - innerrad+1; j <= ball.y ; j+=config.subsampling)
		for (int i = ball.x - innerrad+1; i <= ball.x + innerrad-1; i+=config.subsampling)
		{
			//prepSeg(i+config.subsampling,j+config.subsampling);//Prefetching
			if (!validpixel(i,j))
				continue;
			if (colorIsA(doSeg(i, j,c) , c))
				gd+=3;
			ttl+=3;
		}
	for (int j =ball. y+1 ; j <= ball.y + innerrad-1; j+=config.subsampling)
		for (int i = ball.x- innerrad+1; i <= ball.x + innerrad-1; i+=config.subsampling)
		{
			//prepSeg(i+config.subsampling,j+config.subsampling);//Prefetching
			if (!validpixel(i,j))
				continue;
			if (colorIsA(doSeg(i, j,c) , c))
				gd++;
			ttl++;
		}
		*/
/*
	ratio = ((float) gd+1) / (ttl+1);
	//cout<<"Validratio:"<<ratio<<endl;
	if (ratio < 0.75)
		return false;
		*/
	/*
	gd=0;ttl=0;
	KSegmentator::colormask_t r;
	for (int j =ball.y+ball.cr+1; j <= ball.y + ball.cr+ ball.cr/2;	j+=config.subsampling)
		for (int i = ball.x - ball.cr; i <= ball.x + ball.cr; i+=config.subsampling)
		{
			//prepSeg(i+config.subsampling,j+config.subsampling);//Prefetching
			if (!validpixel(i,j))
				continue;
			r=doSeg(i, j,c|white|green);
			if (colorIsA(r,green | white))
				gd++;
			else if(colorIsA(r,c))
				gd--;
			ttl++;
		}

	for (int j =ball.y-ball.cr-ball.cr/2; j <= ball.y + ball.cr-1;	j+=3)
		for (int i = ball.x - ball.cr; i <= ball.x + ball.cr; i+=3)
		{
			if (!validpixel(i,j))
				continue;
			r=doSeg(i, j);
			if (r == green||r==white)
				gd++;
			else if(r==c)
				gd--;
			ttl++;
		}

	for (int j =ball.y-ball.cr; j <= ball.y + ball.cr;	j+=3)
	{

		for (int i = ball.x - ball.cr-ball.cr/2; i <= ball.x - ball.cr-2; i++)
		{
			if (!validpixel(i,j))
				continue;
			r=doSeg(i, j);
			if (r == green||r==white)
				gd++;
			//lse if(r==c)
			//	gd--;
			ttl++;
		}
		for (int i = ball.x + ball.cr+2 ;i <= ball.x + ball.cr +ball.cr/2; i++)
		{
			if (!validpixel(i,j))
				continue;
			r=doSeg(i, j);
			if (r == green||r==white)
				gd++;
			//else if(r==c)
			//	gd--;
			ttl++;
		}
	}

	ratio = ((float) gd+1) / (ttl+1);
	cout<<"Validratio2:"<<ratio<<endl;
	//if (ratio < 0.25)
	//	return false;*/
	return true;

}
bool Vision::calculateValidGoalPost(goalpostdata_t & goal, KSegmentator::colormask_t c) const
{
	BOOST_PROFILER("calculateValidGoalPost");

	unsigned int ttl=0,bd=0,gd=0;
	float ratio;

	tracer_t l,r;
	l.init(goal.tl.x,goal.tl.y);
	l.initVelocity(goal.ll.x-goal.tl.x,goal.ll.y-goal.tl.y);
	l.setScale(config.subsampling);
	r.init(goal.tr.x,goal.tr.y);
	r.initVelocity(goal.lr.x-goal.tr.x,goal.lr.y-goal.tr.y);
	r.setScale(config.subsampling);
	const int w=(r.x-l.x)+1;
	const int sub=config.subsampling;

	for(int i=0;i<PREFETCH-1;++i)
		prepSeg(l.x-w-1+sub*i,l.y);

	while(l.y<goal.bot.y && r.y<goal.bot.y)
	{

		for(int x=l.x-w-1; x<r.x+w+1;x+=sub)
		{
			if(x+PREFETCH*sub<=r.x+w+1)
				prepSeg(x+PREFETCH*sub,l.y);//Prefetching

			if(colorIsA(doSeg(x,l.y,c),c))
			{
				if(x>l.x&&x<r.x)
					gd++;
				else
					bd++;

			}
			if(x>l.x&&x<r.x)
				ttl++;
		}
		l.step();
		r.step();



	}
	ratio=(float (bd+1))/(ttl+1);
	//cout<<"Bad ratio:"<<ratio<<endl;
	if(ratio>0.35)
		return false;
	ratio=(float (gd+1))/(ttl+1);
	//cout<<"Goodratio:"<<ratio<<endl;
	if(ratio<0.8)
		return false;
	//if((float (bd+1))/(float (gd+1)) > 0.35)
	//	return false;
	//cout<<"Accepted"<<ratio<<endl;
	return true;

}

bool Vision::calculateValidGoalPostBase(const goalpostdata_t& goal, KSegmentator::colormask_t c) const
{
	BOOST_PROFILER("calculateValidGoalPostBase");
	int width =goal.lr.x-goal.ll.x+1;
	unsigned int ttl = 0, gd = 0;
	float ratio;

	for (int j=goal.bot.y+width; j>goal.bot.y; j-=config.subsampling)
	{
		for (int i=goal.ll.x; i<=goal.lr.x; i+=config.subsampling)
		{
			if (!validpixel(i,j))
				continue;
			if(colorIsA(doSeg(i,j,green),green))
				gd++;
			ttl++;
		}
	}
	ratio=(float(gd+1))/(ttl+1);
	//cout<<"ratio:"<<ratio<<endl;
	if (ratio<0.75)
		return false;
	return true;
}

bool Vision::calculateValidGoalPostTop( goalpostdata_t & goal, KSegmentator::colormask_t c) const
{
	BOOST_PROFILER("calculateValidGoalPostTop");
	traceResult r;
	KVecInt2 s;
	//s=goal.top;
	//s.y+=1;
	s=goal.tl;
	r=traceline(s,KVecInt2(-2,+1),c);
	//cout<<"tp:"<<r.p.x<< " "<<r.p.y<<endl;
	//m.x=(r.p.x+s.x)/2;
	//m.y=(r.p.y+s.y)/2;
	//r=traceBlobEdge(m,Vlt,c);
	//cout<<"r:"<<r.p.x<<r.p.y<<endl;
	//cout<<"t:"<<goal.tl.x<<goal.tr.x<<endl;
	goal.leftOrRight=0;
	if(r.p.x<goal.tl.x)
	{
		goal.leftOrRight=1;
		return true;
	}

	//cout<<"No lt proeks"<<endl;
	s=goal.tr;
	r=traceline(s,KVecInt2(+2,+1),c);
	//cout<<"tp:"<<r.p.x<< " "<<r.p.y<<endl;
	//m.x=(r.p.x+s.x)/2;
	//m.y=(r.p.y+s.y)/2;
	//r=traceBlobEdge(m,Vrt,c);

	//cout<<"r:"<<r.p.x<<r.p.y<<endl;
	//cout<<"t:"<<goal.tl.x<<goal.tr.x<<endl;
	if(r.p.x>goal.tr.x)
	{
		goal.leftOrRight=2;//Left
		return true;
	}

	//cout<<"No rt proeks"<<endl;
	return false;

}



bool cmpgoalpostdata_t (Vision::goalpostdata_t a,  Vision::goalpostdata_t b)
{
	return a.distance.mean < b.distance.mean;
}

int Vision::locateGoalPost(vector<KVecInt2> const& cand, KSegmentator::colormask_t c)
{
	BOOST_PROFILER("locateGoalPost");
	vector<goalpostdata_t> history;
	tracer_t at;
	vector<KVecInt2>::const_iterator i;//candidate iterator
	for (i = cand.begin(); i != cand.end(); i++)
	{

		vector<goalpostdata_t>::const_iterator hi = history.begin();
		while (hi != history.end() && i != cand.end())
		{

			if((*hi).contains(*i))
			{
				//cout<<"s";
				i++;//Skip pixel
				hi = history.begin();

			}
			else
				hi++;
		}

		if (i == cand.end())
			break;

		if (!validpixel((*i).x,(*i).y))
			continue;
		//cout<<"GoalPOst new"<<endl;
		GoalPostdata newpost;
		traceResult trcrs;
		trcrs=traceline((*i),Vlt,c);
		KVecInt2 m=trcrs.p;
		trcrs=traceline((*i),Vrt,c);
		m.x=(m.x+trcrs.p.x)/2;
		m.y=(m.y+trcrs.p.y)/2;
		KVecFloat2 b,t,ll,lr,tl,tr,temp;


		trcrs = traceBlobEdge(m,Vdn,c);
		newpost.haveBot=trcrs.smartsuccess;
		newpost.bot=trcrs.p;


		trcrs = traceBlobEdge(m,Vup,c);
		newpost.haveTop=trcrs.smartsuccess;
		newpost.top=trcrs.p;

		b=simpleRotation(newpost.bot);
		t=simpleRotation(newpost.top);

		/*
		newpost.ll.y=newpost.bot.y;
		newpost.lr.y=newpost.bot.y;
		newpost.ll.x=rawImage->width;
		newpost.lr.x=0;*/
		ll.y=b.y;
		lr.y=b.y;
		ll.x=4*rawImage->width;
		lr.x=-4*rawImage->width;


		at.init(newpost.bot.x,newpost.bot.y);
		at.initVelocity(newpost.top.x-newpost.bot.x,newpost.top.y-newpost.bot.y	);
		//int suml=0,sumr=0;
		for(int k=0;k<config.subsampling;k++)
		{
			trcrs=traceline(at,Vlt,c);
			//cout<<at.x<<" "<<at.y<<endl;
			temp=simpleRotation(trcrs.p);
			ll.x=ll.x>temp.x?temp.x:ll.x;
			//newpost.ll.x=newpost.ll.x>trcrs.p.x?trcrs.p.x :newpost.ll.x;
			trcrs=traceline(at,Vrt,c);

			temp=simpleRotation(trcrs.p);
			lr.x=lr.x<temp.x?temp.x:lr.x;

			//newpost.lr.x=newpost.lr.x<trcrs.p.x?trcrs.p.x :newpost.lr.x;
			//sumr+=trcrs.p.x;
			at.step();
			at.step();
		}
		//Back to unrotated coords
		newpost.ll=simpleRotation(ll);
		newpost.lr=simpleRotation(lr);
		//newpost.ll.x=suml/(config.subsampling);
		//newpost.lr.x=sumr/(config.subsampling);

		tl.y=t.y;
		tr.y=t.y;
		tl.x=4*rawImage->width;
		tr.x=-4*rawImage->width;

		at.init(newpost.top.x+(newpost.bot.x-newpost.top.x)/5,newpost.top.y+(newpost.bot.y-newpost.top.y)/5);
		at.initVelocity(newpost.bot.x-newpost.top.x,newpost.bot.y-newpost.top.y	);

		//suml=0,sumr=0;
		for(int k=0;k<config.subsampling;k++)
		{
			trcrs=traceline(at,Vlt,c);
			//cout<<at.x<<" "<<at.y<<endl;

			temp=simpleRotation(trcrs.p);
			tl.x=tl.x>temp.x?temp.x:tl.x;

			//newpost.tl.x=newpost.tl.x>trcrs.p.x?trcrs.p.x :newpost.tl.x;

			trcrs=traceline(at,Vrt,c);

			temp=simpleRotation(trcrs.p);
			tr.x=tr.x<temp.x?temp.x:tr.x;

			//newpost.tr.x=newpost.tr.x<trcrs.p.x?trcrs.p.x :newpost.tr.x;
			at.step();
			at.step();
		}
		//newpost.tl.x=suml/(config.subsampling);
		//newpost.tr.x=sumr/(config.subsampling);

		//Back to unrotated coords
		newpost.tl=simpleRotation(tl);
		newpost.tr=simpleRotation(tr);

		newpost.top.x=(newpost.tl.x+newpost.tr.x)/2;
		newpost.top.y=(newpost.tl.y+newpost.tr.y)/2;

		newpost.bot.x=(newpost.ll.x+newpost.lr.x)/2;
		newpost.bot.y=(newpost.ll.y+newpost.lr.y)/2;

		//Now find again bot and top
		trcrs = traceline(newpost.bot,Vdn,c);
		newpost.haveBot=trcrs.smartsuccess;
		newpost.bot=trcrs.p;

		trcrs = traceline(newpost.top,Vup,c);
		newpost.haveTop=trcrs.smartsuccess;
		newpost.top=trcrs.p;
		t=simpleRotation(newpost.top);
		b=simpleRotation(newpost.bot);

		tl.y=t.y;
		tr.y=t.y;
		ll.y=b.y;
		lr.y=b.y;
		newpost.tl=simpleRotation(tl);
		newpost.tr=simpleRotation(tr);
		newpost.ll=simpleRotation(ll);
		newpost.lr=simpleRotation(lr);
		/*
		newpost.ll.y=newpost.bot.y;
		newpost.lr.y=newpost.bot.y;
		newpost.tl.y=newpost.top.y;
		newpost.tr.y=newpost.top.y;*/

		//cout<<"post:"<<newpost.tl.x<<" " <<newpost.tl.y<<" "<<newpost.tr.x<<" " <<newpost.tr.y<<" "<<newpost.ll.x<<" " <<newpost.ll.y<<" "<<newpost.lr.x<<" " <<newpost.lr.y<<endl;

		//cout<<"Smarts:"<<newpost.haveBot<< " "<<newpost.haveTop<<endl;
		if(tr.x-tl.x<config.pixeltol || lr.x-ll.x<config.pixeltol ||
		  -ll.y+tl.y<(tr.x-tl.x) ||
		  -lr.y+tr.y<(lr.x-ll.x))//
		{
			//cout <<"Goal size test failed"<<endl;
			continue;
		}
		KVecFloat2 ci,i;
		KVecFloat3 c3d;
		ci=imageToCamera(newpost.bot);
		c3d=kinext.camera2dToGround(ci);
		measurement *a=kinext.projectionDistance(ci,0);
		newpost.distBot=a[0];
		newpost.distBot.mean+=config.goaldiam/2;
		newpost.bBot=a[1];
		delete[] a;

		if(newpost.haveBot)
		{
			if(c3d(2)>=0||calculateValidGoalPostBase(newpost,c)==false)//check for some green under:)
			{
				newpost.haveBot=false;
			}

		}

		ci=imageToCamera(newpost.top);
		c3d=kinext.camera2dToGround(ci);
		a=kinext.projectionDistance(ci,config.goalheight);
		newpost.distTop=a[0];
		newpost.distTop.mean+=config.goaldiam/2;
		newpost.bTop=a[1];
		delete[] a;
		if(newpost.haveTop)
		{

			if(c3d(2)<=1||calculateValidGoalPostTop(newpost,c)==false)
			{
				newpost.haveTop=false;
			}

		}

		if(newpost.haveTop&&newpost.haveBot)
		{
			//measurement md=mWeightedMean(newpost.distBot,newpost.distTop);
			//newpost.dist.push_back(md);
			//measurement mb=mWeightedMean(newpost.bBot,newpost.bTop);
			//newpost.ber.push_back(mb);
			fillGoalPostHeightMeasurments(newpost);
		}
		else
		{
			if(newpost.haveBot&&newpost.distBot.mean<config.seedistance)
			{
				newpost.dist.push_back(newpost.distBot);
			}
			else if(newpost.haveTop&&newpost.distTop.mean<config.seedistance)
			{
				newpost.dist.push_back(newpost.distTop);
			}

			newpost.haveHeight=false;
			//fillGoalPostWidthMeasurments(newpost,c);
		}
		if(newpost.dist.size()==0)
			continue;
		newpost.ber.push_back(newpost.bBot);
		newpost.ber.push_back(newpost.bTop);
		//cout<<"LocateGoalPost"<<endl;

		//newpost.distance=mWeightedMean(newpost.dist);
		bool failed=false;
		for(std::vector<measurement>::iterator it=newpost.dist.begin();it!=newpost.dist.end();++it)
		{
			if((*it).mean>=config.seedistance)
			{
				failed=true;
				break;
			}
		}
		if(failed)
			continue;

		newpost.distance=mWeightedMean(newpost.dist);
		newpost.bearing=mWeightedMean(newpost.ber);
		if(newpost.distance.mean>=config.seedistance)
			continue;
		//if(newpost.haveWidth)
		//	cout<<"WDistance"<<newpost.distWidth.mean<<" "<<newpost.distWidth.var<<endl;
		//cout<<"Haves:"<<newpost.haveBot<<" "<<newpost.haveTop<<endl;
		//cout<<"Distance"<<newpost.distance.mean<<" "<<newpost.distance.var<<endl;
		//cout<<"Bearing"<<newpost.bearing.mean<<" "<<newpost.bearing.var<<endl;



		if(calculateValidGoalPost(newpost,c)==false)
			continue;
		KVecFloat2 polar;
		//cout<<"ttldist:"<<distance.mean<<endl;
		polar(0)=newpost.distance.mean;
		polar(1)=newpost.bearing.mean;
		//Convert to robot space
		KVecFloat2 robotpolar=camToRobot(polar);
		newpost.distance.mean=polar(0);
		newpost.bearing.mean=polar(1);
		history.push_back(newpost);

	}
	std::sort (history.begin(), history.end(), cmpgoalpostdata_t);
	goalpostdata_t t;
	if(history.size()==0)
		return 0;
	if(history.size()==1)
	{
		GoalPostdata d1=history[0];
		NamedObject * o=obs.add_regular_objects();
		o->set_bearing(d1.bearing.mean);
		o->set_bearing_dev(sqrt(d1.bearing.var));
		o->set_distance(d1.distance.mean);
		o->set_distance_dev(sqrt(d1.distBot.var));
		std::string name;
		if(colorIsA(c,yellow))
			name="Yellow";
		else
			name="Skyblue";
		if(d1.leftOrRight==1)
			name+="Right";
		else if(d1.leftOrRight==2)
			name+="Left";
		o->set_object_name(name);
		Polygon *apoly=img.add_p();
		point *p;
		p=apoly->add_points();
		p->set_x(d1.tl.x);p->set_y(d1.tl.y);
		p=apoly->add_points();
		p->set_x(d1.tr.x);p->set_y(d1.tr.y);
		p=apoly->add_points();
		p->set_x(d1.lr.x);p->set_y(d1.lr.y);
		p=apoly->add_points();
		p->set_x(d1.ll.x);p->set_y(d1.ll.y);
		apoly->set_color(c);
		apoly->set_confidence(1);
		return 1;

	}

	GoalPostdata d1=history[0];
	GoalPostdata d2=history[1];

	float x1,x2,y1,y2;

	x1=d1.distance.mean* cos( d1.bearing.mean);
	y1=d1.distance.mean* sin( d1.bearing.mean);

	x2=d2.distance.mean* cos( d2.bearing.mean);
	y2=d2.distance.mean* sin( d2.bearing.mean);
	float d=sqrt(sqrd(x1-x2)+sqrd(y1-y2));
	if( abs(d-config.goaldist)/config.goaldist> 0.6)
		return 0;

	if(d1.bearing.mean<d2.bearing.mean)
	{
		d1=d2;
		d2=history[0];
	}
	NamedObject * o=obs.add_regular_objects();
	o->set_bearing(d1.bearing.mean);
	o->set_bearing_dev(sqrt(d1.bearing.var));
	o->set_distance(d1.distance.mean);
	o->set_distance_dev(sqrt(d1.distBot.var));
	std::string name;
	if(colorIsA(c,yellow))
		name="Yellow";
	else
		name="Skyblue";
	name+="Left";
	o->set_object_name(name);


	o=obs.add_regular_objects();
	o->set_bearing(d2.bearing.mean);
	o->set_bearing_dev(sqrt(d2.bearing.var));
	o->set_distance(d2.distance.mean);
	o->set_distance_dev(sqrt(d2.distBot.var));
	//name;
	if(colorIsA(c,yellow))
		name="Yellow";
	else
		name="Skyblue";
	name+="Right";
	o->set_object_name(name);

	Polygon *apoly=img.add_p();
	point *p;
	p=apoly->add_points();
	p->set_x(d1.tl.x);p->set_y(d1.tl.y);
	p=apoly->add_points();
	p->set_x(d1.tr.x);p->set_y(d1.tr.y);
	p=apoly->add_points();
	p->set_x(d1.lr.x);p->set_y(d1.lr.y);
	p=apoly->add_points();
	p->set_x(d1.ll.x);p->set_y(d1.ll.y);
	apoly->set_color(c);
	apoly->set_confidence(1);

	apoly=img.add_p();
	p=apoly->add_points();
	p->set_x(d2.tl.x);p->set_y(d2.tl.y);
	p=apoly->add_points();
	p->set_x(d2.tr.x);p->set_y(d2.tr.y);
	p=apoly->add_points();
	p->set_x(d2.lr.x);p->set_y(d2.lr.y);
	p=apoly->add_points();
	p->set_x(d2.ll.x);p->set_y(d2.ll.y);
	apoly->set_color(c);
	apoly->set_confidence(1);
	return 2;

}

void Vision::fillGoalPostHeightMeasurments(GoalPostdata & newpost) const
{
	BOOST_PROFILER("fillGoalPostHeightMeasurments");
	float t,g,h,dS,dL;
	//Single solution of a  trionym
	g=config.goalheight;
	h=p.cameraZ;

	KVecFloat2 c1,c2,i1,i2;
	i1(0)=newpost.bot.x;
	i1(1)=newpost.bot.y+0.5;
	i2(0)=newpost.bot.x-(newpost.top.y-newpost.bot.y)*sin(-kinext.getRoll());//No this is not a mistake:), compensate for cmos skew the stupid way
	i2(1)=newpost.top.y-0.5;
	c1=imageToCamera(i1);
	c2=imageToCamera(i2);


	float hAngL=kinext.vectorAngle(c1,c2);
	t=tan(KMat::transformations::PI-hAngL);

	//Analytical solution: we know the angle of a triangle ,the oposite side and the parts that it is split by
	//The corresponding triangle height. What is the height it self?
	if(t>=0)
	{
		//cout<<"Goal Post t failed"<<endl;
		newpost.haveHeight=false;
		return ;
	}
	dL= - (g+sqrt( 4*t*t*h*(g-h)+g*g  ))/(2*t)+config.goaldiam/2;
	//Single solution of a  trionym


	i1(0)=newpost.bot.x;
	i1(1)=newpost.bot.y-0.5;
	i2(0)=newpost.bot.x-(newpost.top.y-newpost.bot.y)*sin(-kinext.getRoll());//No this is not a mistake:), compensate for cmos skew the stupid way
	i2(1)=newpost.top.y+0.5;
	c1=imageToCamera(i1);
	c2=imageToCamera(i2);

	float hAngS=kinext.vectorAngle(c1,c2);
	t=tan(KMat::transformations::PI-hAngS);

	//Analytical solution: we know the angle of a triangle ,the oposite side and the parts that it is split by
	//The corresponding triangle height. What is the height it self?
	if(t>=0)
	{
		//cout<<"Goal Post t failed"<<endl;
		newpost.haveHeight=false;
		return ;
	}

	//cout<<g<<" "<<h<<" "<<t<<endl;

	dS= - (g+sqrt( 4*t*t*h*(g-h)+g*g  ))/(2*t)+config.goaldiam/2;

	measurement m;
	m.mean=(dS+dL)/2;
	m.mean=(dS+dL)/2 ;
	m.var=(sqrd(dS-m.mean)+sqrd(dL-m.mean))/2;

	if(  !(newpost.distBot.mean<m.mean&&newpost.distTop.mean>m.mean) &&
		 !(newpost.distBot.mean>m.mean&&newpost.distTop.mean<m.mean)
		)
	{
		//cout<<"Distance logic test failed!"<<endl;
		newpost.haveHeight=false;
		return ;
	}
	newpost.haveHeight=true;
	newpost.dist.push_back(m);//add to vector of results:)

}
void Vision::fillGoalPostWidthMeasurments(GoalPostdata & newpost, KSegmentator::colormask_t c) const
{
	BOOST_PROFILER("fillGoalPostWidthMeasurments");
	//traceResult trcrs;

	//CvPoint l,r;
	KVecFloat2 mT,mB;
	float angV;
	measurement mrT,mrB,mr1,mr2;

	KVecFloat2 c1,c2;//i1,i2;
	int lim=config.pixeltol;
	newpost.haveWidth=false;

	//Some safe margin from image edge
	if(newpost.tl.x<lim||newpost.tr.x<lim||newpost.ll.x<lim||newpost.lr.x<lim)
		return;
	if(newpost.tl.y<lim||newpost.tr.y<lim||newpost.ll.y<lim||newpost.lr.y<lim)
		return;
	lim= rawImage->width-1 - lim;
	if(newpost.tl.x>lim||newpost.tr.x>lim||newpost.ll.x>lim||newpost.lr.x>lim)
		return;
	if(newpost.tl.y>lim||newpost.tr.y>lim||newpost.ll.y>lim||newpost.lr.y>lim)
		return;

	//Find a width and mark measurements!
	mT.x=(newpost.tl.x+newpost.tr.x)/2.0;
	mT.y=(newpost.tl.y+newpost.tr.y)/2.0;
	//For top. move 10% of estimated height down!


	c1=imageToCamera(mT);

	c2=imageToCamera(newpost.tl);
	mr1=kinext.angularDistance(c1,c2,config.goaldiam/2);


	c2=imageToCamera(newpost.tr);
	mr2=kinext.angularDistance(c1,c2,config.goaldiam/2);
	mrT=mWeightedMean(mr1,mr2);
	//cout<<"mr1,mr2,mean:"<<mr1.mean<<" "<<mr2.mean<<mrT.mean<<endl;

	mB.x=(newpost.ll.x+newpost.lr.x)/2.0f;
	mB.y=(newpost.ll.y+newpost.lr.y)/2.0f;
	//cout<<"l,r:"<<l.x<<","<<l.y<<" "<<r.x<<","<<r.y<<endl;
	c1=imageToCamera(mB);
	c2=imageToCamera(newpost.ll);
	mr1=kinext.angularDistance(c1,c2,config.goaldiam/2);

	c2=imageToCamera(newpost.lr);
	mr2=kinext.angularDistance(c1,c2,config.goaldiam/2);


	mrB=mWeightedMean(mr1,mr2);
	//cout<<"mr1,mr2,mean:"<<mr1.mean<<" "<<mr2.mean<<mrB.mean<<endl;

	c1=imageToCamera(mT);
	c2=imageToCamera(mB);
	angV=kinext.vectorAngle(c1,c2);

	float a,b,cosG,dS,dL,ab,cest;

	cosG=cos(angV);
	cest=sqrt(sqrd(mrT.mean)+sqrd(mrB.mean)-2*mrT.mean*mrB.mean*cosG);


	/*cout<<"Cest:"<<cest<<endl;
	if(abs(cest-config.goalheight)/config.goalheight > config.widthestimateotolerance)
	{
		cout<<"check failed"<<endl;
		newpost.haveWidth=false;
		return ;
	}*/

	a=mrT.mean-sqrt(mrT.var);
	b=mrB.mean-sqrt(mrB.var);
	//cout<<"a,b:"<<a<<" "<<b<<endl;

	ab=a*b;
	//dS=sqrt((1.0f/sqrd(cosG) -1.0f)/( (sqrd(a)+sqrd(b)/sqrd(abcos)) - 2.0f/abcos ) );
	dS=sqrt((sqrd(a*b)-sqrd(ab*cosG))/(sqrd(a)+sqrd(b)-2*ab*cosG) );//* (cest/config.goalheight);
	a=mrT.mean+sqrt(mrT.var);
	b=mrB.mean+sqrt(mrB.var);

	//cout<<"a,b:"<<a<<" "<<b<<endl;
	ab=a*b;
	//dL=sqrt((1.0f/sqrd(cosG) -1.0f)/( (sqrd(a)+sqrd(b)/sqrd(abcos)) - 2.0f/abcos ) );
	dL=sqrt((sqrd(a*b)-sqrd(ab*cosG))/(sqrd(a)+sqrd(b)-2*ab*cosG) );// * (cest/config.goalheight);
	//cout<<"mT,mB"<<mrT.mean<<" "<<mrB.mean<<endl;
	//cout<<"Ds,dl ang"<<dS<<" "<<dL<<" "<<angV/TO_RAD<<endl;
	measurement m;
	m.mean=(dS+dL)/2 ;
	/*if(newpost.haveHeight)
	{
		if(abs(m.mean-newpost.dHeight)>config.widthestimateotolerance)
		{
			newpost.haveWidth=false;
			return;
		}
	}*/
	m.var=(sqrd(dS-m.mean)+sqrd(dL-m.mean))/2;
	//cout<<"Width:"<<m.mean<<" "<<m.var<<endl;
	newpost.haveWidth=true;
	newpost.dist.push_back(m);
}

Vision::balldata_t Vision::locateBall(vector<KVecInt2> const& cand)
{
	BOOST_PROFILER("locateBall");
	//Skip first/last row/col
	KVecFloat2 point;
	vector<balldata_t> history;
	vector<balldata_t> banlist;


	vector<KVecInt2>::const_iterator i;
	//For all detected points
	//cout << "locateball" << endl;
	for (i = cand.begin(); i != cand.end(); i++)
	{
		vector<KVecInt2> points;
		vector<balldata_t>::const_iterator bd = history.begin();
		while (bd != history.end() )
		{
			if (CvDist(*bd,*i) <= sqrd((*bd).cr) )
			{
				//cout<<"skip"<<endl;
				i++;//Skip pixels
				if (i == cand.end())
					break;
				bd = history.begin();
			}
			else
				bd++;
		}
		if (i == cand.end())
			break;

		bd= banlist.begin();
		while (bd != banlist.end() )
		{
			if (CvDist(*bd,*i) <= sqrd((*bd).cr))
			{
				//cout<<"ban list"<<endl;
				i++;//Skip pixels
				if (i == cand.end())
					break;
				bd = banlist.begin();
			}
			else
				bd++;
		}
		if (i == cand.end())
			break;
		if (!validpixel((*i).x,(*i).y))
			continue;
		//cout<<"Ball Tracing:";
		traceResult trcrs;

		trcrs = traceline((*i), KVecInt2(0, 1), orange);//Stupid
		KVecInt2 b=trcrs.p;
		trcrs = traceline((*i), KVecInt2(0, -1), orange);//stupid
		KVecInt2 t=trcrs.p;
		KVecInt2 m;
		m.x=(*i).x;
		m.y=(b.y+t.y)/2;

		trcrs = traceline(m, KVecInt2(1, 0), orange);//Stupid
		KVecInt2 r=trcrs.p;

		trcrs = traceline(m, KVecInt2(-1, 0), orange);//Stupid
		KVecInt2 l=trcrs.p;

		m.x=(l.x+r.x)/2;

		//Repeat up down :)
		trcrs = traceline(m, KVecInt2(0, 1), orange);//Smart?
		b=trcrs.p;
		bool bsuccess=trcrs.smartsuccess;

		trcrs = traceline(m, KVecInt2(0, -1), orange);//Smart
		t=trcrs.p;
        m.y=(b.y+t.y)/2;


		KVecFloat2 fl,fr,ft;

		ft=KVecFloat2(t.x,t.y-0.5);
		if (trcrs.smartsuccess==false)
		{
			if(bsuccess)//Top failed , use bottom
				ft=KVecFloat2(b.x,b.y+0.5);//trcrs = traceline(m, cvPoint(0, +1), orange);//try bottom
			else
				continue;
		}
		//cout<<"Got 3 points"<<endl;

		trcrs = traceline(m, KVecInt2(1, 0), orange);//Prefer  r
		fr=KVecFloat2(trcrs.p.x+0.5,trcrs.p.y);//KVecFloat2(trcrs.p.x,trcrs.p.y);;//
		if (trcrs.smartsuccess==false)
		{
			trcrs = traceline(m, KVecInt2(1, -1), orange);// try top r
			if (trcrs.smartsuccess==false)
				continue;
			fr=KVecFloat2(trcrs.p.x+0.5,trcrs.p.y-0.5);//KVecFloat2(trcrs.p.x,trcrs.p.y);//

		}

		trcrs = traceline(m, KVecInt2(-1, 0), orange);//Prefer  r
		fl=KVecFloat2(trcrs.p.x-0.5,trcrs.p.y);//KVecFloat2(trcrs.p.x,trcrs.p.y);//
		if (trcrs.smartsuccess==false)
		{
			trcrs = traceline(m, KVecInt2(-1, -1), orange);// try top r
			if (trcrs.smartsuccess==false)
				continue;
			fl=KVecFloat2(trcrs.p.x-0.5,trcrs.p.y-0.5);//KVecFloat2(trcrs.p.x,trcrs.p.y);//

		}
		//Call centerOfCircle with the margins of ball color, not the pixels them selves
		KVecFloat2 center=centerOfCircle(fl,ft,fr);
		if(center.x==-1)
			continue;

		float radius = CvDist(center,ft);//-0.707;
		center.y+=0.707;
		//cout << "Wtf" << endl;
		balldata_t newdata;
		newdata.x = center.x;
		newdata.y = center.y;
		newdata.cr = radius;//0.35;
		//Looks like a ball?

		//Estimate Radius:
		KVecFloat3 c3d;
		KVecFloat2 c1,c2;
		KVecFloat2 im1;

		im1(0)=center.x +(radius)*0.707;
		im1(1)=center.y +(radius)*0.707;
		//im1.prettyPrint();
		//im2.prettyPrint();
		c1=imageToCamera(center);
		c2=imageToCamera(im1);
		float tantths=kinext.vectorAngle(c1,c2);
		c3d=kinext.camera2dToGround(c1);//Transform center vector  to ground coordinates
		float par=tantths*tantths*( (sqrd(c3d(0))+sqrd(c3d(1)))/sqrd(c3d(2)))+tantths*tantths;

		if (par>1)//Something is terribly wrong!
		{
			Logger::Instance().WriteMsg("Vision", "Ball size estimation went boggus!", Logger::Error);
			continue;
		}
		float rest=p.cameraZ*(par-sqrt(par))/(par-1);
		//cout<<"rest:"<<rest<<endl;
		newdata.ballradius=rest;
		measurement d1=kinext.angularDistanceProjected(c1,c2,rest);//Find distance based on size and project to ground, at radius height
		//float w=d1.mean/2;
		//w=w<0.5?0.5:w;
		//rest= rest*(0.5-w)/w + (config.ballsize/2.0*(w-0.5))/w;
		if (abs( (rest*2-config.ballsize)/config.ballsize)>config.balltolerance)//Wrong diameter ball
		{
			Logger::Instance().WriteMsg("Vision", "Ball size estimation check failed", Logger::Info);
			banlist.push_back(newdata);
			continue;
		}
		if (!calculateValidBall(newdata,(KSegmentator::colormask_t) orange))
			continue;
		measurement* a=kinext.projectionDistance(c1,rest);
		measurement d2=a[0];
		measurement bearing=a[1];
		//cout<<"proj bearing:"<<bearing.mean<<" "<<bearing.var<<endl;
		delete[] a;
		measurement distance;

		distance=mWeightedMean(d1,d2);
		KVecFloat2 polar;
		//cout<<"ttldist:"<<distance.mean<<endl;
		polar(0)=distance.mean;
		polar(1)=bearing.mean;
		//Convert to robot space
		KVecFloat2 robotpolar=camToRobot(polar);
		distance.mean=polar(0);
		bearing.mean=polar(1);
		newdata.distance=distance;
		newdata.bearing=bearing;
		history.push_back(newdata);

	}
	vector<balldata_t>::iterator bd = history.begin();
	balldata_t best;
	best.cr = 0;
	best.ballradius=1000;
	best.distance.mean=1000;
	best.distance.var =10000;
	//debugcout << "BEST found" << endl;
	while (bd != history.end())
	{
		//cout << best.x << " " << best.y << " "<<best.d<< endl;
		//cout << (*bd).x << " " << (*bd).y << " "<<(*bd).d<< endl;
		//cout<<(*bd).d<<endl;
		if ((*bd).distance.mean < best.distance.mean )//abs((*bd).ballradius*2.0-config.ballsize)/config.ballsize,*abs(best.ballradius*2.0-config.ballsize)/config.ballsize
			best = *bd;
		//cout << best.x << " " << best.y << " "<<best.distance.mean<< endl;
		bd++;
	}

#ifdef DEBUGVISION
	//KVecFloat2 & w=camToRobot(o)
	Logger::Instance().WriteMsg("Vision", "Distance:"+_toString(best.distance.mean), Logger::Info);
	cout<<"Bearing:"<<best.bearing.mean<<" "<<best.bearing.var<<endl;
	cout<<"Distance:"<<best.distance.mean<<" "<<best.distance.var<<endl;

#endif
	bd=history.begin();
	while(bd!=history.end())
	{
		BallCircle *bc=img.add_ball();
		bc->mutable_center()->set_x((*bd).x);
		bc->mutable_center()->set_y((*bd).y);
		bc->set_radius((*bd).cr);
		bc->set_valid((*bd).distance.mean==best.distance.mean);
		bc->set_confidence(bc->valid());

		bd++;
	}
	return best;


}
KVecFloat2 Vision::centerOfCircle(KVecFloat2 l, KVecFloat2 m, KVecFloat2 r) const
{
	BOOST_PROFILER("centerOfCircle");
	KVecFloat2 center;
	center.x=-1;
	if (m.x==l.x||m.x==r.x)//Some points form  a vertical line, swap and retry
	{
		KVecFloat2 tmp;
		tmp=m;
		m=l;
		l=m;
	}

	if (m.x==l.x||m.x==r.x)//3 Points are collinear :)
		return center;
	float ma=((float)(m.y-l.y))/((float)(m.x-l.x));
	float mb=((float)(r.y-m.y))/((float)(r.x-m.x));
	if (mb==ma||ma==0)
		return center;
	center.x= ( ma*mb*((float)(l.y-r.y))+mb*((float)(l.x+m.x))-ma*((float)(m.x+r.x)) )/ (2.0*(mb-ma));
	center.y=-(center.x -((float)(l.x+m.x))/2)/ma+((float)(l.y+m.y))/2.0;
	//center.y+=0.707;
	return center;
}

Vision::traceResult Vision::traceline(KVecInt2 start, KVecInt2 vel, KSegmentator::colormask_t c) const
{
	BOOST_PROFILER("tracelineInt");
	int skipcount = 0;
	const int tol=config.pixeltol;
	KVecInt2 curr = start,prftch;
	prftch=curr;
	for(int i=0;i<PREFETCH-1;++i)
	{
		prepSeg(prftch.x,prftch.y);
		prftch.add(vel);
	}

	KVecInt2 latestValid = start;

	while (validpixel(curr.x,curr.y))
	{
		if (doSeg(curr.x, curr.y,c) != c)
		{
			skipcount++;
		}
		else
		{
			latestValid = curr;
			skipcount = 0;
		};

		if (skipcount > tol )//|| globalcount > GLOBALTRACESKIP)
		{
			break;
		}
		prepSeg(prftch.x,prftch.y);
		curr.add(vel);
		prftch.add(vel);

	}
	traceResult r;
	if (!validpixel(curr.x,curr.y))
		r.smartsuccess=false;
	else
		r.smartsuccess=true;

	r.p=latestValid;

	return r;
}


Vision::traceResult Vision::traceline(KVecInt2 start, KVecFloat2 vel, KSegmentator::colormask_t c) const
{
	BOOST_PROFILER("tracelineFloat");
	int skipcount = 0;
	const int tol=config.pixeltol;
	KVecInt2 latestValid = start;
	tracer_t curr,prftch;
	curr.init(start.x,start.y);
	curr.initVelocity(vel.x,vel.y);
	prftch=curr;
	for(int i=0;i<PREFETCH-1;++i)
	{
		prepSeg(prftch.x,prftch.y);
		prftch.step();
	}
	/////cout << "traceline:"<<start.x<<" "<<start.y<<endl;
	while (validpixel(curr.x,curr.y))
	{
		if (doSeg(curr.x, curr.y,c) != c)
		{
			skipcount++;
			//globalcount++;
		}
		else
		{
			latestValid.x = curr.x;
			latestValid.y = curr.y;
			skipcount = 0;
		};

		if (skipcount > tol )//|| globalcount > GLOBALTRACESKIP)
		{
			//curr=start;
			break;
		}
		prepSeg(prftch.x,prftch.y);
		curr.step();
		prftch.step();

	}
	traceResult r;
	if (!validpixel(curr.x,curr.y))
		r.smartsuccess=false;
	else
		r.smartsuccess=true;

	//cout<<"ret"<<latestValid.x<<" "<<latestValid.y<<endl;

	r.p=latestValid;

	return r;
}


Vision::traceResult Vision::traceBlobEdge(KVecInt2 start, KVecFloat2 vel, KSegmentator::colormask_t c) const
{
	BOOST_PROFILER("traceBlobEdge");
	int skipcount = 0;

	KVecInt2 latestValid = start;
	tracer_t curr,prftch;
	curr.init(start.x,start.y);
	curr.initVelocity(vel.x,vel.y);
	prftch=curr;
	for(int i=0;i<PREFETCH-1;++i)
	{
		prepSeg(prftch.x,prftch.y);
		prftch.step();
	}

	int rebounce=0;
	const int tol=config.pixeltol;

	while (validpixel(curr.x,curr.y))
	{

		if (doSeg(curr.x, curr.y,c) != c)
		{

			skipcount++;
			//globalcount++;
		}
		else
		{
			latestValid=curr;
			skipcount = 0;
			rebounce--;
		};


		prepSeg(prftch.x,prftch.y);
		curr.step();
		prftch.step();

		if ( skipcount>tol)
		{
			//Ok, try some readaptation
			if(rebounce>0)
				break;
			traceResult trs=traceline(latestValid,KVecFloat2(vel.y,-vel.x),c);
			KVecInt2 l=trs.p;
			trs=traceline(latestValid,KVecFloat2(-vel.y,vel.x),c);
			KVecInt2 m=trs.p;
			m.x=(m.x+l.x)/2;
			m.y=(m.y+l.y)/2;
			float d=CvDist(m,latestValid);
			if(d==0)
				break;
			if(vel.y<vel.x)
			{
				if(abs(latestValid.y-m.y)>=config.pixeltol)
					break;
			}
			else
				if(abs(latestValid.x-m.x)>=config.pixeltol)
					break;

			curr.init((m.x+curr.x)/2,(m.y+curr.y)/2);
			skipcount=0;
			rebounce=tol+1;

		}


	}
	traceResult r;
	if (!validpixel(curr.x,curr.y))
		r.smartsuccess=false;
	else
		r.smartsuccess=true;

	r.p=latestValid;

	return r;


}

bool Vision::validpixel(int x,int y) const
{
	if ((x >= 0 && x < (rawImage-> width) && y >= 0 && y < (rawImage-> height)))
		return true;
	else
		return false;

}
KSegmentator::colormask_t Vision::doSeg(const int x, const int y,const KSegmentator::colormask_t h ) const
{
	BOOST_PROFILER("dopSeg");
	if (x >= 0 && x < (rawImage-> width) && y >= 0 && y < (rawImage-> height))
	{
		//return seg->classifyPixel(rawImage, x, y, type);
		return seg->classifyPixel(x,y,h);
	}
	else
	{
		return 0;
	}

}

void Vision::prepSeg(const int x,const int y) const
{
	BOOST_PROFILER("prepSeg");

	if (x >= 0 && x < (rawImage-> width) && y >= 0 && y < (rawImage-> height))
	{
		seg->prefetchPixelData(x,y);
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif
