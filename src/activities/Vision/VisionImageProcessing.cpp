#include "Vision.h"

#include <cmath>
#include <vector>

#include "core/include/Logger.hpp"

ACTIVITY_START

#define PREFETCH 2
#define CvDist(pa,pb) sqrt(  ( ((pa).x-(pb).x )*((pa).x-(pb).x )  )+( ((pa).y-(pb).y )*((pa).y-(pb).y) )  )
#define colorIsA(x,y) (((x)&(y))!=0)

using namespace std;
using namespace KMath;

inline measurement mWeightedMean(measurement m1, measurement m2)
{
	measurement res;
	/*
	(d1.mean*d2.var+d2.mean*d1.var)/(d1.var+d2.var);
		distance.var=d1.var*d2.var/(d1.var+d2.var);
		*/
	res.mean = (m1.mean * m2.var + m2.mean * m1.var) / (m1.var + m2.var);
	res.var = (m1.var * m2.var) / (m1.var + m2.var) * (sqrd(m1.mean - res.mean) / m1.var + sqrd(m2.mean - res.mean) / m2.var);
	return res;
}
inline measurement mWeightedMean(const std::vector<measurement> &l)
{
	float n = 0, d = 0, c = 0;
	measurement res;

	if(l.size() == 1)
		return l[0];

	for(unsigned i = 0; i < l.size(); i++)
	{
		n += l[i].mean / l[i].var;
		d += 1.0f / l[i].var;
		//cout<<l[i].mean<<" "<<l[i].var<<endl;
	}

	res.mean = n / d;

	for(unsigned i = 0; i < l.size(); i++)
		c += sqrd(l[i].mean - res.mean) / l[i].var;

	res.var = (c / d) / (l.size() - 1);
	return res;
}


class inttracer_s: public KVecInt2
{
	int idx, idy;
	int e, l, k; //Error, lower limit, upperlimit
	char sx, sy; //Steps : +/-1
	//int idx,idy;

public:
	void init(int a, int b)
	{
		x = a;
		y = b;
		e = 0;
	};
	void initVelocity(int dx, int dy)
	{
		sx = dx > 0 ? 1 : -1;
		sy = dy > 0 ? 1 : -1;

		if(dx == 0) sx = 0;

		if(dy == 0) sy = 0;

		idx = abs(dx);
		idy = abs(dy);
		l = -(-2 * idy + idx); //=-2*L as in analysis
		k = -(2 * idx - idy); //=-2*K as in analysis
		e = 0;
	}
	void initVelocity(float dx, float dy)
	{
		int idx = dx * 4096;
		int idy = dy * 4096;
		initVelocity(idx, idy);
	};
	void step()
	{
		int e2 = e * 2;

		if(e2 <= l)
		{
			y += sy;
			e += idx;
		}

		if(e2 >= k)
		{
			x += sx;
			e -= idy;
		}
	}

	void r_step() //Reverse step
	{
		int e2 = e * 2;

		if(e2 > -l)
		{
			y -= sy;
			e -= idx;
		}

		if(e2 < -k)
		{
			x -= sx;
			e += idy;
		}
	}

	void steps(unsigned s)
	{
		while(s > 0)
		{
			step();
			s--;
		}
	}
	void r_steps(unsigned s)
	{
		while(s > 0)
		{
			r_step();
			s--;
		}
	}

};


typedef struct inttracer_s tracer_t;


typedef struct
{
	tracer_t gtrc;
	KVecInt2 lastpoint ;
	bool ballfound, yfound, bfound;
	int cntother;
	bool done;
} linestruct;

void Vision::gridScan(const KSegmentator::colormask_t color)
{
	KPROF_SCOPE(vprof, "gridScan");
	//Horizontal + Vertical Scan
#ifdef DEBUGVISION
	cout << "Starting Grid scan" << endl;
#endif
	/*KMat::GenMatrix<float,2,2> r;
	KVecFloat2 end(0,-rawImage.height);
	//From rotated image plane, to an unrotated one
	KMat::transformations::rotate(r,-kinext.getRoll());
	end=r.slow_mult(end);
	end=cameraToImage(end);*/
	KVecFloat2 im;
	KVecFloat2 c;
	KVecFloat3 c3d;
	KSegmentator::colormask_t tempcolor;
	//static int startx=0;
	int linestep = (rawImage.width - 2 * config.bordersize) / (config.scanV - 1);
	int align = (rawImage.width - linestep * (config.scanV - 1) ) >> 1;
	int step;
	//float d;
	std::vector<linestruct> l;
	//((int)config.scanV);
	l.reserve((int)config.scanV);
	linestruct t;
	int sx = config.bordersize + align;
	int vstep = (sqrt(rawImage.height - 2 * config.bordersize - config.scanH * config.subsampling) - 1) / 2;
	step = config.subsampling + vstep;
	//Fix initial scanline positions :)
	int linesdone = 0;
	//Prefetch first batch

	//Prefetch prologue
	for(int i = 0; i < PREFETCH - 1; ++i)
		prepSeg(sx + i * linestep, rawImage.height - config.bordersize - 1);

	//std::cout<<"Vup:"<<Vup.x<<" "<<Vup.y<<std::endl;

	for(int i = 0 ; i < config.scanV; i++)
	{
		l.push_back(t);
		l[i].gtrc.init(sx, rawImage.height - config.bordersize - 1);
		l[i].lastpoint = l[i].gtrc;
		sx += linestep;
		l[i].gtrc.initVelocity(Vup.x, Vup.y);
		//l[i].gtrc.setScale(step);
		l[i].ballfound = false;
		l[i].yfound = false;
		l[i].bfound = false;
		l[i].cntother = 0;
		l[i].done = false;
	}

	//cout<<"init"<<endl;

	while(linesdone < config.scanV)
	{
		//std::cout<<"Step:"<<step<<endl;
		//std::cout<<l[0].gtrc.x<<" "<<l[0].gtrc.y <<std::endl;
		//stepx=2;
		//cout<<"while"<<endl;
		for(unsigned i = 0 ; i < l.size(); i++)
		{
			//cout<<"for"<<endl;
			linestruct &thisl = l[i];

			//Before actually segmenting this pixel, prefetch the next one
			if(l[(i + PREFETCH) % l.size()].done == false)
				prepSeg(l[(i + PREFETCH) % l.size()].gtrc.x, l[(i + PREFETCH) % l.size()].gtrc.y);

			if(thisl.done == true)
				continue;

			if(thisl.gtrc.x < config.bordersize || thisl.gtrc.y < config.bordersize ||
			        thisl.gtrc.x > rawImage.width - 1 - config.bordersize || thisl.gtrc.y > rawImage.height - 1 - config.bordersize ||
			        !validpixel(thisl.gtrc.x, thisl.gtrc.y))
			{
				thisl.done = true;
				linesdone++;
				continue;
			}

#ifdef DEBUGVISION
			tobeshown.push_back(KVecInt2(thisl.gtrc.x, thisl.gtrc.y));
#endif
			tempcolor = doSeg(thisl.gtrc.x, thisl.gtrc.y, white | orange | green | yellow ); //|skyblue);

			//cout<<"doseg:"<<(int)tempcolor<<endl;
			if (colorIsA(tempcolor, green)) //
			{
				//cntwhitegreenpixels++;
				//cntwhitegreenorangepixels++;
				thisl.cntother = 0;
				thisl.lastpoint = thisl.gtrc;
				thisl.ballfound = false;
				thisl.yfound = false;
				thisl.bfound = false;
			}
			else if(colorIsA(tempcolor, white))
			{
				thisl.cntother++;//hmm... white? valid but test for distance
				thisl.lastpoint = thisl.gtrc;
				thisl.ballfound = false;
				thisl.yfound = false;
				thisl.bfound = false;
			}
			else if (colorIsA(tempcolor, color))
			{
				//cntwhitegreenorangepixels++;
				thisl.cntother = 0;
				thisl.yfound = false;
				thisl.bfound = false;
			}
			else
			{
				//ballskip=0;
				thisl.cntother++;
				thisl.ballfound = false;
			}

			//Check see distance
			if (thisl.cntother > config.pixeltol) //No continuity, break
			{
				//thisl.done=true;
				//linesdone++;
				//continue;
				c = imageToCamera(thisl.gtrc);
				c3d = kinext.camera2dToGround(c);

				if(c3d(2) >= 0) //Looking up :)
				{
					//cout<<"--dup"<<endl;
					thisl.done = true;
					linesdone++;
					continue;
				}

				c3d = kinext.camera2dToGroundProjection(c, 0);
				float d = sqrt(sqrd(c3d(0)) + sqrd(c3d(1)));

				if (d >= config.seedistance)
				{
					//cout<<"--d"<<endl;
					thisl.done = true;
					linesdone++;
					continue;
				}

				c = imageToCamera(thisl.lastpoint);
				c3d = kinext.camera2dToGround(c);

				if(c3d(2) >= 0) //Looking up :)
				{
					//cout<<"--d1up"<<endl;
					thisl.done = true;
					linesdone++;
					continue;
				}

				c3d = kinext.camera2dToGroundProjection(c, 0);
				float d1 = sqrt(sqrd(c3d(0)) + sqrd(c3d(1)));

				if (d1 >= config.seedistance)
				{
					//cout<<"--d1"<<endl;
					thisl.done = true;
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

				if (abs(d1 - d) > config.skipdistance)
				{
					//cout<<"break"<<endl;
					//cout<<"--skip"<<endl;
					obstacles.push_back(thisl.gtrc);	//c3d.prettyPrint();
					thisl.done = true;
					linesdone++;
					continue;
				}
			}

			if (colorIsA(tempcolor, color) && thisl.ballfound == false)
			{
				ballpixels.push_back(thisl.gtrc);
				//cntwhitegreenpixels=0;
				thisl.ballfound = true;
				//continue;
				//ballpixel = j;
			}
			else if(colorIsA(tempcolor, yellow) && thisl.yfound == false)
			{
				//ballpixels.push_back(tmpPoint);
				ygoalpost.push_back(thisl.gtrc);
				thisl.yfound = true;
			}

			/*else if(colorIsA(tempcolor,skyblue)&&thisl.bfound==false)
			{
				//ballpixels.push_back(tmpPoint);
				bgoalpost.push_back(thisl.gtrc);
				thisl.bfound=true;
			}*/
			//Prepare for the next one
			thisl.gtrc.steps(step);
		}

		//Fix NEXT step length
		//Find next pixel
		step--;

		if (step < config.minH) step = config.minH;
	}

	//Now follow the lines
#ifdef DEBUGVISION
	cout << "End Grid scan" << endl;
#endif
}


/**
 * Hard Decision: Is it good enough for a ball?
 */

bool Vision::calculateValidBall(balldata_t const ball, KSegmentator::colormask_t c) const
{
	KPROF_SCOPE(vprof, "calculateValidBall");
	int  gd = 0, bd = 0, remin = 0, remout = 0;
	const int sub = config.subsampling;
	const int cr = floor(ball.cr + 0.5);
	const int crd = floor(sqrd(ball.cr) + 0.5);
	const int cx = floor(ball.x + 0.5);
	const int cy = floor(ball.y + 0.5);
	const int margin = cr + (cr / 4 < sub * 2 ? sub * 2 : cr / 4); //At least 2 pixels checked
	const int border = 2; //Only 2 pixels from the borders
	const int top = ball.y - margin < border ? border : ball.y - margin;
	const int bot = ball.y + margin > rawImage.height - 1 - border ? rawImage.height - 1 - border : ball.y + margin;
	const int left = ball.x - margin < border ? border : ball.x - margin;
	const int right = ball.x + margin > rawImage.width - 1 - border ? rawImage.width - 1 - border : ball.x + margin;
	const int ttl = floor((bot - top + 1) / (sub)) * floor((right - left + 1) / (sub));
	const int inside = floor((KMat::transformations::PI * cr * cr) / (sub * sub));
	const int bdlimit = (ttl - inside) * (0.25);
	const int gdlimit = inside * (0.5);
	remin = inside;
	remout = ttl - inside;
	float insidelim = 0;

	for(int y = top; y <= bot; y += sub)
	{
		for(int i = 0; i < PREFETCH - 1; ++i)
			prepSeg(left + sub * i, y);

		if(gd + remin <= gdlimit || bd >= bdlimit) //No hope
			return false;

		if(gd >= gdlimit && bd + remout <= bdlimit) // Certainty
			return true;

		insidelim = crd - sqrd(cy - y); //Given y, what is the limit for x , for x,y to be inside the circle

		for(int x = left; x <= right; x += sub)
		{
			//if(x+PREFETCH*sub<=right)
			prepSeg(x + PREFETCH * sub, y);
			//else
			//	prepSeg(left+PREFETCH*sub-sub,y+sub);
			bool iscolor = colorIsA(doSeg(x, y, c) , c);

			if(sqrd(cx - x) <= insidelim)
			{
				remin--;

				if(iscolor) gd++;
			}
			else
			{
				remout--;

				if(iscolor) bd++;
			}
		}
	}

	if(gd + remin <= gdlimit || bd >= bdlimit) //No hope
		return false;

	if(gd >= gdlimit && bd + remout <= bdlimit) // Certainty
		return true;

	return false;
}
bool Vision::calculateValidGoalPost(goalpostdata_t & goal, KSegmentator::colormask_t c) const
{
	KPROF_SCOPE(vprof, "calculateValidGoalPost");
	unsigned int ttl = 0, bd = 0, gd = 0;
	float ratio;
	tracer_t l, r;

	/*cout<<"Start:"<<goal.tl.x<<" "<<goal.tl.y<<endl;
	cout<<"End:"<<goal.ll.x<<" "<<goal.ll.y<<endl;
	cout<<"Start:"<<goal.tr.x<<" "<<goal.tr.y<<endl;
	cout<<"End:"<<goal.lr.x<<" "<<goal.lr.y<<endl;*/
	if(goal.tl == goal.tr || goal.ll == goal.tl)
		return false;

	l.init(goal.tl.x, goal.tl.y);
	l.initVelocity(goal.ll.x - goal.tl.x, goal.ll.y - goal.tl.y);
	//l.setScale(config.subsampling);
	r.init(goal.tr.x, goal.tr.y);
	r.initVelocity(goal.lr.x - goal.tr.x, goal.lr.y - goal.tr.y);
	//r.setScale(config.subsampling);
	const int sub = config.subsampling;
	int w;//=(r.x-l.x)+sub+1;

	/*for(int i=0;i<PREFETCH-1;++i)
		prepSeg(l.x-w-1+sub*i,l.y);*/
	//cout<< simpleRotation(l).y<<" " << simpleRotation(goal.ll).y <<" " << simpleRotation(r).y<<" " << simpleRotation(goal.lr).y;
	while(simpleRotation(l).y >= simpleRotation(goal.ll).y && simpleRotation(r).y >= simpleRotation(goal.lr).y)
	{
		//cout<<l.x<<" "<<l.y<<" "<<r.x<<" "<<r.y<<endl;
		w = (r.x - l.x) + sub + 1;

		for(int x = l.x - w - 1; x < r.x + w + 1; x += sub)
		{
			/*if(x+PREFETCH*sub<=r.x+w+1)
				prepSeg(x+PREFETCH*sub,l.y);//Prefetching*/
			if(colorIsA(doSeg(x, (l.y + r.y) / 2, c), c))
			{
				if(x >= l.x && x <= r.x)
					gd++;
				else
					bd++;
			}

			if(x >= l.x && x <= r.x)
				ttl++;
		}

		//cout<<"why oh why"<<endl;
		l.steps(sub);
		r.steps(sub);
		//cout<<"why oh why not"<<endl;
	}

	ratio = (float (bd + 1)) / (ttl + 1);

	//cout<<"Bad ratio:"<<ratio<<" "<<bd<< " "<< ttl <<endl;
	if(ratio > 0.25)
		return false;

	ratio = (float (gd + 1)) / (ttl + 1);

	//cout<<"Goodratio:"<<ratio<<endl;
	if(ratio < 0.65)
		return false;

	//if((float (bd+1))/(float (gd+1)) > 0.35)
	//	return false;
	//cout<<"Accepted"<<ratio<<endl;
	return true;
}

bool Vision::calculateValidGoalPostBase(const goalpostdata_t& goal, KSegmentator::colormask_t c) const
{
	KPROF_SCOPE(vprof, "calculateValidGoalPostBase");
	int width = goal.lr.x - goal.ll.x + 1;
	unsigned int ttl = 0, gd = 0;
	float ratio;

	for (int j = goal.bot.y + width; j > goal.bot.y; j -= config.subsampling)
	{
		for (int i = goal.ll.x; i <= goal.lr.x; i += config.subsampling)
		{
			if (!validpixel(i, j))
				continue;

			if(colorIsA(doSeg(i, j, green), green))
				gd++;

			ttl++;
		}
	}

	ratio = (float(gd + 1)) / (ttl + 1);

	//cout<<"ratio:"<<ratio<<endl;
	if (ratio < 0.45)
		return false;

	return true;
}

bool Vision::calculateValidGoalPostTop( goalpostdata_t & goal, KSegmentator::colormask_t c) const
{
	KPROF_SCOPE(vprof, "calculateValidGoalPostTop");
	traceResult r;
	KVecFloat2 tl, tr, s;
	goal.leftOrRight = 0; //Initialization
	tl = simpleRotation(goal.tl);
	tr = simpleRotation(goal.tr);

	//i*1/8 of a width below ?
	for (float i = 1.0; i > 0; i = i - config.horizontalpostratio)//8.0)
	{
		s = tr;
		s.y = s.y - (tr.x - tl.x) * i;
		s = simpleRotation(s); //Unrotate
		r = traceline(KVecInt2(s.x, s.y), Vrt, c);

		if(simpleRotation(r.p).x > tr.x)
		{
			goal.leftOrRight = 2; //Left goal post, has something on its right
			return true;
		}
	}

	for (float i = 1.0; i > 0; i = i - config.horizontalpostratio)//8.0)
	{
		s = tl;
		s.y = s.y - (tr.x - tl.x) * i;
		s = simpleRotation(s); //Unrotate
		r = traceline(KVecInt2(s.x, s.y), Vlt, c);

		if(simpleRotation(r.p).x < tl.x)
		{
			goal.leftOrRight = 1; //Right goal post , has something on its left
			return true;
		}
	}

	return false;
}



bool cmpgoalpostdata_t (Vision::goalpostdata_t a,  Vision::goalpostdata_t b)
{
	return a.distance.mean < b.distance.mean;
}

int Vision::locateGoalPost(vector<KVecInt2> const& cand, KSegmentator::colormask_t c)
{
	KPROF_SCOPE(vprof, "locateGoalPost");
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

		if (!validpixel((*i).x, (*i).y))
			continue;

		//cout<<"GoalPOst new"<<endl;
		goalpostdata_t newpost;
		traceResult trcrs;
		trcrs = traceline((*i), Vlt, c);
		KVecInt2 m = trcrs.p;
		trcrs = traceline((*i), Vrt, c);
		m.x = (m.x + trcrs.p.x) / 2;
		m.y = (m.y + trcrs.p.y) / 2;
		KVecFloat2 b, t, ll, lr, tl, tr, temp;
		trcrs = traceline(m, Vdn, c);
		newpost.haveBot = trcrs.smartsuccess;
		newpost.bot = trcrs.p;
		newpost.top = newpost.bot; //Initialization of the top pixel
		trcrs = traceline(m, Vup, c);

		while(trcrs.smartsuccess && simpleRotation(newpost.top).y < simpleRotation(trcrs.p).y)
		{
			newpost.top = trcrs.p;
			temp.x = (trcrs.p.x + m.x) / 2;
			temp.y = (trcrs.p.y + m.y) / 2;
			trcrs = traceline(KVecInt2(temp.x, temp.y), Vlt, c);
			m = trcrs.p;
			trcrs = traceline(KVecInt2(temp.x, temp.y), Vrt, c);
			m.x = (m.x + trcrs.p.x) / 2;
			m.y = (m.y + trcrs.p.y) / 2;
			//again up
			trcrs = traceline(m, Vup, c);
			//Delta
			temp = simpleRotation(trcrs.p);
			temp -= simpleRotation(newpost.top);

			//No point moving anymore, moving sideways
			if(temp.y < fabs(temp.x) * 2)
				break;

			//cout<<trcrs.p.x<<" "<<trcrs.p.y<<endl;
		}

		newpost.top = trcrs.p;
		newpost.haveTop = trcrs.smartsuccess;
		b = simpleRotation(newpost.bot);
		t = simpleRotation(newpost.top);
		/*
		newpost.ll.y=newpost.bot.y;
		newpost.lr.y=newpost.bot.y;
		newpost.ll.x=rawImage->width;
		newpost.lr.x=0;*/
		ll.y = b.y;
		lr.y = b.y;
		ll.x = 0;
		lr.x = 0;
		at.init(newpost.bot.x + (newpost.top.x - newpost.bot.x) / 3, newpost.bot.y + (newpost.top.y - newpost.bot.y) / 3);
		at.initVelocity(newpost.top.x - newpost.bot.x, newpost.top.y - newpost.bot.y	);

		//int suml=0,sumr=0;
		for(int k = 0; k < config.subsampling; k++)
		{
			trcrs = traceline(at, Vlt, c);
			//cout<<at.x<<" "<<at.y<<endl;
			temp = simpleRotation(trcrs.p);
			ll.x += temp.x;
			//newpost.ll.x=newpost.ll.x>trcrs.p.x?trcrs.p.x :newpost.ll.x;
			trcrs = traceline(at, Vrt, c);
			temp = simpleRotation(trcrs.p);
			lr.x += temp.x;
			//newpost.lr.x=newpost.lr.x<trcrs.p.x?trcrs.p.x :newpost.lr.x;
			//sumr+=trcrs.p.x;
			at.step();
		}

		ll.x /= config.subsampling;
		lr.x /= config.subsampling;

		//Increase the trapezoid size by half a pixel
		/*ll.x-=0.5;
		ll.y-=0.5;
		lr.x+=0.5;
		lr.y-=0.5;*/
		//Too small
		if(lr.x - ll.x < config.pixeltol)
			continue;

		//Back to unrotated coords
		newpost.ll = simpleRotation(ll);
		newpost.lr = simpleRotation(lr);
		//newpost.ll.x=suml/(config.subsampling);
		//newpost.lr.x=sumr/(config.subsampling);
		tl.y = t.y;
		tr.y = t.y;
		tl.x = 0;
		tr.x = 0;
		at.init(newpost.top.x + (newpost.bot.x - newpost.top.x) / 3, newpost.top.y + (newpost.bot.y - newpost.top.y) / 3);
		at.initVelocity(newpost.bot.x - newpost.top.x, newpost.bot.y - newpost.top.y	);

		//suml=0,sumr=0;
		for(int k = 0; k < config.subsampling; k++)
		{
			trcrs = traceline(at, Vlt, c);
			//cout<<at.x<<" "<<at.y<<endl;
			temp = simpleRotation(trcrs.p);
			tl.x += temp.x;
			//newpost.tl.x=newpost.tl.x>trcrs.p.x?trcrs.p.x :newpost.tl.x;
			trcrs = traceline(at, Vrt, c);
			temp = simpleRotation(trcrs.p);
			tr.x += temp.x;
			//newpost.tr.x=newpost.tr.x<trcrs.p.x?trcrs.p.x :newpost.tr.x;
			at.step();
		}

		tl.x /= config.subsampling;
		tr.x /= config.subsampling;

		//Too small
		if(tr.x - tl.x < config.pixeltol )
			continue;

		//Back to unrotated coords
		newpost.tl = simpleRotation(tl);
		newpost.tr = simpleRotation(tr);
		{
			float md, rd;
			md = CvDist(tl, lr);
			rd = CvDist(tr, ll);

			if(md < rd)   swap(md, rd);

			//Diagonals not approx. the same size
			if(md > 6 * rd / 5) // md/rd > 6/5 ,md>rd
				continue;
		}
		//Now fix the top and bottom pixels.
		newpost.top.x = (newpost.tl.x + newpost.tr.x) / 2;
		newpost.top.y = (newpost.tl.y + newpost.tr.y) / 2;
		newpost.bot.x = (newpost.ll.x + newpost.lr.x) / 2;
		newpost.bot.y = (newpost.ll.y + newpost.lr.y) / 2;
		//Now perform inverse projections
		KVecFloat2 ci, i;
		KVecFloat3 c3d;
		ci = imageToCamera(newpost.bot);
		c3d = kinext.camera2dToGround(ci);
		measurement *a = kinext.projectionDistance(ci, 0);
		//cout<<"Camera X-Y-Z"<<p.cameraX<<" " <<p.cameraY<<" "<<p.cameraZ<<endl;
		newpost.distBot = a[0];
		newpost.distBot.mean += config.goaldiam / 2;
		//cout<<"See bot:"<<newpost.distBot.mean<<endl;
		newpost.bBot = a[1];
		delete[] a;

		if(newpost.haveBot)
		{
			if(c3d(2) >= 0 || calculateValidGoalPostBase(newpost, c) == false) //check for some green under:)
			{
				newpost.haveBot = false;
				//cout<<"Goal no Base"<<endl;
			}
		}

		ci = imageToCamera(newpost.top);
		c3d = kinext.camera2dToGround(ci);
		a = kinext.projectionDistance(ci, config.goalheight);
		newpost.distTop = a[0];
		if(config.goaltopisrounded!=0)
			newpost.distTop.mean += config.goaldiam / 2;
		//cout<<"See top:"<<newpost.distTop.mean<<endl;
		newpost.bTop = a[1];
		delete[] a;

		if(newpost.haveTop)
		{
			if(c3d(2) <= 1 || calculateValidGoalPostTop(newpost, c) == false)
			{
				newpost.haveTop = false;
				//cout<<"Goal no top"<<endl;
			}
		}

		if(newpost.haveTop && newpost.haveBot)
		{
			//measurement md=mWeightedMean(newpost.distBot,newpost.distTop);
			//newpost.dist.push_back(md);
			//measurement mb=mWeightedMean(newpost.bBot,newpost.bTop);
			//newpost.ber.push_back(mb);
			fillGoalPostHeightMeasurments(newpost);
			//cout<<"HEIGHT!"<<endl;
		}
		else
		{
			if(newpost.haveBot && newpost.distBot.mean < config.seedistance)
			{
				//fillGoalPostWidthMeasurments(newpost,c);
				//if(newpost.haveWidth)
				newpost.dist.push_back(newpost.distBot);
			}
			else if(newpost.haveTop && newpost.distTop.mean < config.seedistance)
			{
				//fillGoalPostWidthMeasurments(newpost,c);
				//if(newpost.haveWidth)
				newpost.dist.push_back(newpost.distTop);
			}

			newpost.haveHeight = false;
			//cout<<"Goal no Height"<<endl;
		}

		if(newpost.haveBot)
			newpost.ber.push_back(newpost.bBot);

		if(newpost.haveTop)
			newpost.ber.push_back(newpost.bTop);

		if(newpost.ber.size() == 0 || newpost.dist.size() == 0)
			continue;

		//cout<<"LocateGoalPost"<<endl;
		//newpost.distance=mWeightedMean(newpost.dist);
		bool failed = false;

		for(std::vector<measurement>::iterator it = newpost.dist.begin(); it != newpost.dist.end(); ++it)
		{
			if((*it).mean >= config.seedistance)
			{
				failed = true;
				break;
			}
		}

		if(failed)
			continue;

		newpost.distance = mWeightedMean(newpost.dist);
		newpost.bearing = mWeightedMean(newpost.ber);

		if(newpost.distance.mean >= config.seedistance)
			continue;

		//if(newpost.haveWidth)
		//	cout<<"WDistance"<<newpost.distWidth.mean<<" "<<newpost.distWidth.var<<endl;
		/*cout<<"Haves:"<<newpost.haveBot<<" "<<newpost.haveTop<<endl;
		cout<<"Distance top-bot:"<<newpost.distTop.mean<<" "<<newpost.distBot.mean<<endl;
		cout<<"bearing top-bot:"<<newpost.bTop.mean<<" "<<newpost.bBot.mean<<endl;
		cout<<"Distance"<<newpost.distance.mean<<" "<<newpost.distance.var<<endl;
		cout<<"Bearing"<<newpost.bearing.mean<<" "<<newpost.bearing.var<<endl;*/

		if(calculateValidGoalPost(newpost, c) == false)
		{
			//cout<<"Goal no Valid"<<endl;
			continue;
		}

		KVecFloat2 polar;
		//cout<<"ttldist:"<<distance.mean<<endl;
		polar(0) = newpost.distance.mean;
		polar(1) = newpost.bearing.mean;
		//Convert to robot space
		KVecFloat2 robotpolar = camToRobot(polar);
		newpost.distance.mean = polar(0);
		newpost.bearing.mean = polar(1);
		history.push_back(newpost);
	}

	std::sort (history.begin(), history.end(), cmpgoalpostdata_t);
	goalpostdata_t t;

	if(history.size() == 0)
		return 0;

	if(history.size() == 1)
	{
		goalpostdata_t d1 = history[0];
		NamedObject * o = obs.add_regular_objects();
		o->set_bearing(d1.bearing.mean);
		o->set_bearing_dev(sqrt(d1.bearing.var));
		o->set_distance(d1.distance.mean);
		o->set_distance_dev(sqrt(d1.distBot.var));
		std::string name;

		if(colorIsA(c, yellow))
			name = "Yellow";

		if(d1.leftOrRight == 1)
			name += "Right";
		else if(d1.leftOrRight == 2)
			name += "Left";

		o->set_object_name(name);

		Polygon *apoly = vdm.add_polygon();
		Point *p;

		p = apoly->add_points();
		p->set_x(d1.tl.x); p->set_y(d1.tl.y);
		p = apoly->add_points();
		p->set_x(d1.top.x); p->set_y(d1.top.y);
		p = apoly->add_points();
		p->set_x(d1.tr.x); p->set_y(d1.tr.y);
		p = apoly->add_points();
		p->set_x(d1.lr.x); p->set_y(d1.lr.y);
		p = apoly->add_points();
		p->set_x(d1.bot.x); p->set_y(d1.bot.y);
		p = apoly->add_points();
		p->set_x(d1.ll.x); p->set_y(d1.ll.y);
		apoly->set_color(c);
		apoly->set_confidence(1);

#ifdef DEBUGVISION
		LogEntry(LogLevel::Info,GetName())<<"Di:" <<(d1.distance.mean) << " "<<(d1.distance.var);
		LogEntry(LogLevel::Info,GetName())<<"Be: " << (d1.bearing.mean) << " " << (d1.bearing.var);

		if(d1.haveBot)
			LogEntry(LogLevel::Info,GetName())<<"DiB: " << (d1.distBot.mean) << " " << (d1.distBot.var);

		if(d1.haveHeight)
			LogEntry(LogLevel::Info,GetName())<<"DiH: " << (d1.distHeight.mean) << " " << (d1.distHeight.var);

#endif
		return 1;
	}

	goalpostdata_t d1 = history[0];
	goalpostdata_t d2 = history[1];
	float x1, x2, y1, y2;
	x1 = d1.distance.mean * cos( d1.bearing.mean);
	y1 = d1.distance.mean * sin( d1.bearing.mean);
	x2 = d2.distance.mean * cos( d2.bearing.mean);
	y2 = d2.distance.mean * sin( d2.bearing.mean);
	float d = sqrt(sqrd(x1 - x2) + sqrd(y1 - y2));

	if( abs(d - config.goaldist) / config.goaldist > 0.6)
	{
		//cout<<"Goalaaaaa"<<endl;
		//return 0;
	}

	if(d1.bearing.mean < d2.bearing.mean)
	{
		d1 = d2;
		d2 = history[0];
	}

	NamedObject * o = obs.add_regular_objects();
	o->set_bearing(d1.bearing.mean);
	o->set_bearing_dev(sqrt(d1.bearing.var));
	o->set_distance(d1.distance.mean);
	o->set_distance_dev(sqrt(d1.distBot.var));
	std::string name;

	if(colorIsA(c, yellow))
		name = "Yellow";

	name += "Left";
	o->set_object_name(name);
	o = obs.add_regular_objects();
	o->set_bearing(d2.bearing.mean);
	o->set_bearing_dev(sqrt(d2.bearing.var));
	o->set_distance(d2.distance.mean);
	o->set_distance_dev(sqrt(d2.distBot.var));

	//name;
	if(colorIsA(c, yellow))
		name = "Yellow";

	name += "Right";
	o->set_object_name(name);

	Polygon *apoly = vdm.add_polygon();
	Point *p;

	p = apoly->add_points();
	p->set_x(d1.tl.x); p->set_y(d1.tl.y);
	p = apoly->add_points();
	p->set_x(d1.top.x); p->set_y(d1.top.y);
	p = apoly->add_points();
	p->set_x(d1.tr.x); p->set_y(d1.tr.y);
	p = apoly->add_points();
	p->set_x(d1.lr.x); p->set_y(d1.lr.y);
	p = apoly->add_points();
	p->set_x(d1.bot.x); p->set_y(d1.bot.y);
	p = apoly->add_points();
	p->set_x(d1.ll.x); p->set_y(d1.ll.y);
	apoly->set_color(c);
	apoly->set_confidence(1);

	apoly = vdm.add_polygon();
	p = apoly->add_points();
	p->set_x(d2.tl.x); p->set_y(d2.tl.y);
	p = apoly->add_points();
	p->set_x(d2.top.x); p->set_y(d2.top.y);
	p = apoly->add_points();
	p->set_x(d2.tr.x); p->set_y(d2.tr.y);
	p = apoly->add_points();
	p->set_x(d2.lr.x); p->set_y(d2.lr.y);
	p = apoly->add_points();
	p->set_x(d2.bot.x); p->set_y(d2.bot.y);
	p = apoly->add_points();
	p->set_x(d2.ll.x); p->set_y(d2.ll.y);
	apoly->set_color(c);
	apoly->set_confidence(1);

	return 2;
}

void Vision::fillGoalPostHeightMeasurments(goalpostdata_t & newpost) const
{
	KPROF_SCOPE(vprof, "fillGoalPostHeightMeasurments");
	float t, g, h, dS, dL;
	//Single solution of a  trionym
	if(config.goaltopisrounded!=0) //If the goal post top is rounded, we observe a Hypotenuse
	{
		g=sqrt(conf.goalheight*conf.goalheight+conf.goaldiam*conf.goaldiam);
	}
	else
		g = config.goalheight;
	h = p.cameraZ;
	KVecFloat2 c1, c2, i1, i2;
	i1(0) = newpost.bot.x;
	i1(1) = newpost.bot.y + 0.5;
	i2(0) = newpost.bot.x - (newpost.top.y - newpost.bot.y) * sin(-kinext.getRoll()); //No this is not a mistake:), compensate for cmos skew the stupid way
	i2(1) = newpost.top.y - 0.5;
	c1 = imageToCamera(i1);
	c2 = imageToCamera(i2);
	float hAngL = kinext.vectorAngle(c1, c2);
	t = tan(KMat::transformations::PI - hAngL);

	//Analytical solution: we know the angle of a triangle ,the oposite side and the parts that it is split by
	//The corresponding triangle height. What is the height it self?
	if(t >= 0)
	{
		//cout<<"Goal Post t failed"<<endl;
		newpost.haveHeight = false;
		return ;
	}

	dL = - (g + sqrt( 4 * t * t * h * (g - h) + g * g  )) / (2 * t) + config.goaldiam / 2;
	//Single solution of a  trionym
	i1(0) = newpost.bot.x;
	i1(1) = newpost.bot.y - 0.5;
	i2(0) = newpost.bot.x - (newpost.top.y - newpost.bot.y) * sin(-kinext.getRoll()); //No this is not a mistake:), compensate for cmos skew the stupid way
	i2(1) = newpost.top.y + 0.5;
	c1 = imageToCamera(i1);
	c2 = imageToCamera(i2);
	float hAngS = kinext.vectorAngle(c1, c2);
	t = tan(KMat::transformations::PI - hAngS);

	//Analytical solution: we know the angle of a triangle ,the oposite side and the parts that it is split by
	//The corresponding triangle height. What is the height it self?
	if(t >= 0)
	{
		//cout<<"Goal Post t failed"<<endl;
		newpost.haveHeight = false;
		return ;
	}

	//cout<<g<<" "<<h<<" "<<t<<endl;
	dS = - (g + sqrt( 4 * t * t * h * (g - h) + g * g  )) / (2 * t) + config.goaldiam / 2;
	measurement m;
	m.mean = (dS + dL) / 2;
	m.mean = (dS + dL) / 2 ;
	m.var = (sqrd(dS - m.mean) + sqrd(dL - m.mean)) / 2;

	if(  !(newpost.distBot.mean <= m.mean && newpost.distTop.mean >= m.mean) &&
	        !(newpost.distBot.mean >= m.mean && newpost.distTop.mean <= m.mean)
	  )
	{
		//cout<<"Distance logic test failed!"<<endl;
		newpost.haveHeight = false;
		return ;
	}

	newpost.haveHeight = true;
	newpost.distHeight = m;
	//cout<<"Height Only:"<<m.mean;
	newpost.dist.push_back(m);//add to vector of results:)
}
void Vision::fillGoalPostWidthMeasurments(goalpostdata_t & newpost, KSegmentator::colormask_t c) const
{
	KPROF_SCOPE(vprof, "fillGoalPostWidthMeasurments");
	//traceResult trcrs;
	//CvPoint l,r;
	KVecFloat2 mT, mB;
	float angV;
	measurement mrT, mrB, mr1, mr2;
	KVecFloat2 c1, c2, t1, t2; //i1,i2;
	int lim = config.bordersize;
	newpost.haveWidth = false;

	//Some safe margin from image edge
	if(newpost.tl.x < lim || newpost.tr.x < lim || newpost.ll.x < lim || newpost.lr.x < lim)
		return;

	if(newpost.tl.y < lim || newpost.tr.y < lim || newpost.ll.y < lim || newpost.lr.y < lim)
		return;

	lim = rawImage.width - 1 - lim;

	if(newpost.tl.x > lim || newpost.tr.x > lim || newpost.ll.x > lim || newpost.lr.x > lim)
		return;

	if(newpost.tl.y > lim || newpost.tr.y > lim || newpost.ll.y > lim || newpost.lr.y > lim)
		return;

	t1 = newpost.tl;
	t2 = newpost.tr;
	t1 += Vlt;
	t2 += Vrt;
	//Find a width and mark measurements!
	mT.x = (t1.x + t2.x) / 2.0;
	mT.y = (t1.y + t2.y) / 2.0;
	//For top. move 10% of estimated height down!
	/*newpost.tl.prettyPrint();
	newpost.tr.prettyPrint();
	mT.prettyPrint();*/
	c1 = imageToCamera(mT);
	c2 = imageToCamera(t1);
	mr1 = kinext.angularDistance(c1, c2, config.goaldiam / 2);
	c2 = imageToCamera(t2);
	mr2 = kinext.angularDistance(c1, c2, config.goaldiam / 2);
	mrT = mWeightedMean(mr1, mr2);
	mrT.mean -= config.goaldiam / 2;
	//cout<<"mr1,mr2,mean:"<<mr1.mean<<" "<<mr2.mean<<mrT.mean<<endl;
	t1 = newpost.ll;
	t2 = newpost.lr;
	t1 += Vlt;
	t2 += Vrt;
	mB.x = (t1.x + t2.x) / 2.0;
	mB.y = (t1.y + t2.y) / 2.0;
	//cout<<"l,r:"<<l.x<<","<<l.y<<" "<<r.x<<","<<r.y<<endl;
	c1 = imageToCamera(mB);
	c2 = imageToCamera(t1);
	mr1 = kinext.angularDistance(c1, c2, config.goaldiam / 2);
	c2 = imageToCamera(t2);
	mr2 = kinext.angularDistance(c1, c2, config.goaldiam / 2);
	mrB = mWeightedMean(mr1, mr2);
	mrB.mean -= config.goaldiam / 2;
	//cout<<"mr1,mr2,mean:"<<mr1.mean<<" "<<mr2.mean<<mrB.mean<<endl;
	c1 = imageToCamera(mT);
	c2 = imageToCamera(mB);
	angV = kinext.vectorAngle(c1, c2);
	float a, b, dS, dL, cest;
	measurement m;
	a = mrT.mean;
	b = mrB.mean;

	if(newpost.haveTop)
	{
		float td = sqrt(sqrd(a) - sqrd(config.goalheight - p.cameraZ) );

		if( fabs(td - a) / a > config.widthestimateotolerance)
		{
			return;
		}
	}

	if(newpost.haveBot)
	{
		float td = sqrt(sqrd(b) - sqrd(p.cameraZ) );

		if( fabs(td - b) / b > config.widthestimateotolerance)
		{
			return;
		}
	}

	cest = sqrt(sqrd(a) + sqrd(b) - 2 * a * b * cos(angV));
	m.mean = (a * b * sin(angV) / cest) + config.goaldiam / 2;
	a = mrT.mean - sqrt(mrT.var);
	b = mrB.mean - sqrt(mrB.var);
	//cout<<"a,b:"<<a<<" "<<b<<endl;
	cest = sqrt(sqrd(a) + sqrd(b) - 2 * a * b * cos(angV));
	dS = (a * b * sin(angV) / cest) + config.goaldiam / 2; //* (cest/config.goalheight);
	a = mrT.mean + sqrt(mrT.var);
	b = mrB.mean + sqrt(mrB.var);
	cest = sqrt(sqrd(a) + sqrd(b) - 2 * a * b * cos(angV));
	dL = (a * b * sin(angV) / cest) + config.goaldiam / 2; // * (cest/config.goalheight);
	m.var = (sqrd(dS - m.mean) + sqrd(dL - m.mean)) / 2;
	//<<"Width:"<<m.mean<<" "<<m.var<<endl;
	newpost.haveWidth = true;
	newpost.dist.push_back(m);
}

Vision::balldata_t Vision::locateBall(vector<KVecInt2> const& cand)
{
	KPROF_SCOPE(vprof, "locateBall");
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
			if (CvDist(*bd, *i) <= sqrd((*bd).cr) )
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

		bd = banlist.begin();

		while (bd != banlist.end() )
		{
			if (CvDist(*bd, *i) <= sqrd((*bd).cr))
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

		if (!validpixel((*i).x, (*i).y))
			continue;

		//cout<<"Ball Tracing:";
		//cout<<(*i).x<<" "<<(*i).y<<endl;
		traceResult trcrs;
		trcrs = traceline((*i), KVecInt2(0, 1), orange);//Stupid
		KVecInt2 b = trcrs.p;
		trcrs = traceline((*i), KVecInt2(0, -1), orange);//stupid
		KVecInt2 t = trcrs.p;
		KVecInt2 m;
		m.x = (*i).x;
		m.y = (b.y + t.y) / 2;
		trcrs = traceline(m, KVecInt2(1, 0), orange);//Stupid
		KVecInt2 r = trcrs.p;
		trcrs = traceline(m, KVecInt2(-1, 0), orange);//Stupid
		KVecInt2 l = trcrs.p;
		m.x = (l.x + r.x) / 2;
		//bounds vector
std:
		vector<KVecFloat2> pv; //Eight points
		//Repeat up down :)
		trcrs = traceline(m, KVecInt2(0, 1), orange);//Smart?
		b = trcrs.p;
		//if(trcrs.smartsuccess)
		//pv.push_back(KVecFloat2(b.x,b.y+0.5));
		trcrs = traceline(m, KVecInt2(0, -1), orange);//Smart
		t = trcrs.p;
		m.y = (b.y + t.y) / 2;
		// if(trcrs.smartsuccess)
		//	pv.push_back(KVecFloat2(b.x,b.y-0.5));

		for(int dx = -1; dx < 2; dx++)
			for(int dy = -1; dy < 2; dy++)
			{
				if(dx == 0 && dy == 0)
					continue;

				trcrs = traceline(m, KVecInt2(dx, dy), orange);

				if(trcrs.smartsuccess)
					pv.push_back(KVecFloat2(trcrs.p.x, trcrs.p.y));

				//pv.push_back(KVecFloat2(trcrs.p.x+0.5*dx,trcrs.p.y+0.5*dy));
			}

		if(pv.size() < 3)
			continue;

		KVecFloat2 center;
		float radius = 0, er = 99999999;

		for(unsigned i = 0; i < pv.size() - 2; i++)
			for(unsigned j = i + 1; j < pv.size() - 1; j++)
				for(unsigned k = j + 1; k < pv.size(); k++)
				{
					KVecFloat2 c = centerOfCircle(pv[i], pv[j], pv[k]);

					if(c.x == -1)
						continue;

					float error = 0;
					float r = CvDist(c, pv[i]);

					for(unsigned o = 0; o < pv.size(); o++)
						error += fabs(CvDist(c, pv[o]) - r);

					if(error < er)
					{
						radius = r;
						center = c;
						er = error;
						//cout<<radius<<endl;
					}
				}

		if(radius == 0)
			continue;

		for(unsigned o = 0; o < pv.size(); o++)
		{
			float r = CvDist(center, pv[o]);

			if(r > radius)
				radius = r;
		}

		//radius+=0.707;
		//cout<<"Got 3 points"<<endl;
		//center.y+=0.707;
		//cout << "Wtf" << endl;
		balldata_t newdata;
		newdata.x = center.x;
		newdata.y = center.y;
		newdata.cr = radius;//0.35;
		//Looks like a ball?
		//Estimate Radius:
		KVecFloat3 c3d;
		KVecFloat2 c1, c2;
		KVecFloat2 im1;
		im1(0) = center.x + (center.x > 0 ? -1 : 1) * (radius) * 0.707;
		im1(1) = center.y + (center.y > 0 ? -1 : 1) * (radius) * 0.707;
		//im1.prettyPrint();
		//im2.prettyPrint();
		c1 = imageToCamera(center);
		c2 = imageToCamera(im1);
		float sinhalfdelta = kinext.vectorAngle(c1, c2);
		c3d = kinext.camera2dToGround(c1); //Transform center vector  to ground coordinates
		float tanpitch = fabs(c3d(2)) / sqrt(sqrd(c3d(0)) + sqrd(c3d(1)) );
		float pa = sinhalfdelta * sqrt(tanpitch * tanpitch + 1);
		//cout<<"E:"<<sinhalfdelta<<" "<<tanpitch<<" "<<pa<<std::endl;
		float rest = 1.0 * p.cameraZ / (tanpitch / pa + 1);
		//cout<<"rest:"<<rest<<endl;
		newdata.ballradius = rest;
		measurement d1 = kinext.angularDistanceProjected(c1, c2, rest); //Find distance based on size and project to ground, at radius height

		//float w=d1.mean/2;
		//w=w<0.5?0.5:w;
		//rest= rest*(0.5-w)/w + (config.ballsize/2.0*(w-0.5))/w;
		if (abs( (rest * 2 - config.ballsize) / config.ballsize) > config.balltolerance) //Wrong diameter ball
		{
			//Logger::Instance().WriteMsg("Vision", "Ball size estimation check failed rest = " + _toString(rest), Logger::Info);
			banlist.push_back(newdata);
			continue;
		}

		if (!calculateValidBall(newdata, (KSegmentator::colormask_t) orange))
			continue;

		measurement* a = kinext.projectionDistance(c1, rest);
		measurement d2 = a[0];
		measurement bearing = a[1];
		//cout<<"proj bearing:"<<bearing.mean<<" "<<bearing.var<<endl;
		delete[] a;
		measurement distance;
		distance = mWeightedMean(d1, d2);
		KVecFloat2 polar;
		//cout<<"ttldist:"<<distance.mean<<endl;
		polar(0) = distance.mean;
		polar(1) = bearing.mean;
		//Convert to robot space
		KVecFloat2 robotpolar = camToRobot(polar);
		distance.mean = polar(0);
		bearing.mean = polar(1);
		newdata.distance = distance;
		newdata.bearing = bearing;
		history.push_back(newdata);
	}

	vector<balldata_t>::iterator bd = history.begin();
	balldata_t best;
	best.cr = 0;
	best.ballradius = 1000;
	best.distance.mean = 1000;
	best.distance.var = 10000;

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
	if(best.cr > 0)
	{
		LogEntry(LogLevel::Info,GetName())<<"Ball Distance: " << (best.distance.mean) << " " << (best.distance.var);
		LogEntry(LogLevel::Info,GetName())<<"Ball Bearing: " << (best.bearing.mean) << " " << (best.bearing.var);
		LogEntry(LogLevel::Info,GetName())<<"Ball Size: " << (best.ballradius * 2.0);
		KVecFloat2 c1(best.x, best.y);
		c1 = imageToCamera(c1);
		measurement* a = kinext.projectionDistance(c1, config.ballsize / 2.0);
		measurement d2 = a[0];
		measurement bearing = a[1];
		delete[] a;
		KVecFloat2 polar;
		//cout<<"ttldist:"<<distance.mean<<endl;
		polar(0) = d2.mean;
		polar(1) = bearing.mean;
		KVecFloat2 robotpolar = camToRobot(polar);
		LogEntry(LogLevel::Info,GetName())<<"Stupid Distance:" << (robotpolar.x) << " " << (d2.var);
		LogEntry(LogLevel::Info,GetName())<<"Stupid Bearing:" << (robotpolar.y) << " " << (bearing.var);
	}

#endif

	bd = history.begin();
	while(bd != history.end())
	{
		BallCircle *bc = vdm.mutable_ball_circle();
		bc->mutable_center()->set_x((*bd).x);
		bc->mutable_center()->set_y((*bd).y);
		bc->set_radius((*bd).cr);
		bc->set_valid((*bd).distance.mean == best.distance.mean);
		bc->set_confidence(bc->valid());
		bd++;
	}

	return best;
}

KVecFloat2 Vision::centerOfCircle(KVecFloat2 l, KVecFloat2 m, KVecFloat2 r) const
{
	KPROF_SCOPE(vprof, "centerOfCircle");
	KVecFloat2 center;
	center.x = -1;

	if (m.x == l.x || m.x == r.x) //Some points form  a vertical line, swap and retry
	{
		KVecFloat2 tmp;
		tmp = m;
		m = l;
		l = m;
	}

	if (m.x == l.x || m.x == r.x) //3 Points are collinear :)
		return center;

	float ma = ((float)(m.y - l.y)) / ((float)(m.x - l.x));
	float mb = ((float)(r.y - m.y)) / ((float)(r.x - m.x));

	if (mb == ma || ma == 0)
		return center;

	center.x = ( ma * mb * ((float)(l.y - r.y)) + mb * ((float)(l.x + m.x)) - ma * ((float)(m.x + r.x)) ) / (2.0 * (mb - ma));
	center.y = -(center.x - ((float)(l.x + m.x)) / 2) / ma + ((float)(l.y + m.y)) / 2.0;
	//center.y+=0.707;
	return center;
}

Vision::traceResult Vision::traceline(KVecInt2 const & start, KVecInt2 const& vel, KSegmentator::colormask_t c) const
{
	KPROF_SCOPE(vprof, "tracelineInt");
	KVecInt2 curr = start, limit = start, prftch, ftch;
	KVecInt2 fforward = vel;
	fforward.scalar_mult(config.pixeltol + 1);
	prftch = curr;
	prftch += fforward;
	ftch = prftch;

	for(int i = 0; i < PREFETCH - 1; ++i)
	{
		prepSeg(prftch.x, prftch.y);
		prftch -= vel;
	}

	while (true)
	{
		if(colorIsA(doSeg(ftch.x, ftch.y, c) , c) )
		{
			limit = curr;
			limit += fforward;
			curr = ftch;
			ftch += fforward;
			prftch = ftch;

			if(!validpixel(ftch.x, ftch.y))
				break;

			for(int i = 0; i < PREFETCH - 1; ++i)
			{
				if(limit != prftch)
				{
					prepSeg(prftch.x, prftch.y);
					prftch -= vel;
				}
			}
		}
		else
		{
			ftch -= vel;

			if(ftch == limit)
				break;

			if(limit != prftch)
			{
				prepSeg(prftch.x, prftch.y);
				prftch -= vel;
			}
		}
	}

	traceResult r;

	if (!validpixel(ftch.x, ftch.y))
		r.smartsuccess = false;
	else
		r.smartsuccess = true;

	r.p = curr;
	return r;
}



Vision::traceResult Vision::traceline(KVecInt2 const & start, KVecFloat2 const& vel, KSegmentator::colormask_t c) const
{
	KPROF_SCOPE(vprof, "tracelineFloat");
	//KVecInt2 curr = start, limit=start,prftch, ftch;
	tracer_t curr, limit, prftch, ftch;
	//KVecInt2 fforward=vel;
	//fforward.scalar_mult(config.pixeltol);
	curr.init(start.x, start.y);
	curr.initVelocity(vel.x, vel.y);
	limit = curr;
	prftch = curr;
	prftch.steps(config.pixeltol + 1);
	ftch = prftch;

	for(int i = 0; i < PREFETCH - 1; ++i)
	{
		//prftch.prettyPrint();
		prepSeg(prftch.x, prftch.y);
		prftch.r_step();
		//prftch.prettyPrint();
	}

	while (true)
	{
		if(colorIsA(doSeg(ftch.x, ftch.y, c) , c) )
		{
			limit = curr;
			limit.steps(config.pixeltol + 1);
			curr = ftch;
			ftch.steps(config.pixeltol + 1);
			prftch = ftch;

			if(!validpixel(ftch.x, ftch.y))
				break;

			for(int i = 0; i < PREFETCH - 1; ++i)
			{
				if(limit != prftch)
				{
					prepSeg(prftch.x, prftch.y);
					prftch.r_step();
				}
			}
		}
		else
		{
			ftch.r_step();

			if(ftch == limit)
				break;

			if(limit != prftch)
			{
				prepSeg(prftch.x, prftch.y);
				prftch.r_step();
			}
		}
	}

	traceResult r;

	if (!validpixel(ftch.x, ftch.y))
		r.smartsuccess = false;
	else
		r.smartsuccess = true;

	r.p = curr;
	return r;
}



bool Vision::validpixel(int x, int y) const
{
	if ((x >= 0 && x < (rawImage.width) && y >= 0 && y < (rawImage.height)))
		return true;
	else
		return false;
}
KSegmentator::colormask_t Vision::doSeg(const int x, const int y, const KSegmentator::colormask_t h ) const
{
	KPROF_SCOPE(vprof, "dopSeg");

	if (x >= 0 && x < (rawImage.width) && y >= 0 && y < (rawImage.height))
	{
		return seg->classifyPixel(x, y, h);
	}
	else
	{
		return 0;
	}
}

void Vision::prepSeg(const int x, const int y) const
{
	KPROF_SCOPE(vprof, "prepSeg");
	//	if (x >= 0 && x < (rawImage-> width) && y >= 0 && y < (rawImage-> height))
	//	{
	seg->prefetchPixelData(x, y);
	//	}
}

ACTIVITY_END
