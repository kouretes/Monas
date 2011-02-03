#include "KCameraTransformation.h"
#include "hal/robot/generic_nao/kAlBroker.h"
#include "tools/logger.h"
#include "tools/toString.h"

#include <vector>

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#define VISIBLE __attribute__ ((visibility("default")))
#else
#define VISIBLE
#endif

using namespace std;

KCameraTranformation::KCameraTranformation(){}
void KCameraTranformation::Init()
{
    try {
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("KCameraTransformation","Error in getting motion proxy",Logger::FatalError);
	}

}
float  KCameraTranformation::cot(float theta) const
{
    //float  t;
    //if(theta<KMat::transformations::PI/4.0)
    return 1.0/tan((double)theta);
    //else
        //t=1.0/tan((double)theta);
    //return t;
};

std::vector<float>  KCameraTranformation::getKinematics(const std::string toAsk) const
{
    //Name, space, useSensors
    //TORSO=0
    //WORLD=1
    //NAO=2
   std::vector<float> values= motion->getPosition(toAsk, 2, true);
   return values;


}

void  KCameraTranformation::setPose(cpose p)
{
    thepose=p;
    KMat::ATMatrix<float,4> temp,h,t;
    KMat::transformations::rotateZ(h,thepose.yaw);

    KMat::transformations::rotateY(temp,thepose.pitch+thepose.cameraPitch);
    h*=temp;//Fixed head chain!
    cameraTorsoChain=h;//Copy half Part
    KMat::transformations::rotateY(cameraChain,thepose.angY);
    KMat::transformations::rotateX(temp,thepose.angX);
    cameraChain*=temp;
    cameraChain*=h;

    cameraChainInv=cameraChain;
    cameraChainInv.fast_invert();//Invert and we are done
}


KVecFloat3  KCameraTranformation::camera2dTo3d(const KVecFloat2 & coords) const
{
    KVecFloat3  res;
    res(0)=thepose.focallength;
    //cout<<"fpixels:"<<thepose.focallength;
    res(1)=-coords(0);
    res(2)=coords(1);
    return res;


}


KVecFloat2  KCameraTranformation::camera3dTo2d(const KVecFloat3 & coords) const
{
    KVecFloat3  a;
    KVecFloat2  res;

    a.copyFrom(coords);//Copy and modify
    a.scalar_mult(thepose.focallength/a(0));//Set x to focal distance
    res(0)=-a(1);
    res(1)=a(2);
    return res;
}

KVecFloat3  KCameraTranformation::camera2dToTorso(const KVecFloat2 & coords) const
{
	return cameraTorsoChain.transform(camera2dTo3d(coords));
    //KVecFloat3  res;
    //res.copyFrom(gc);
    //delete  &gc;
    //return res;

}

KVecFloat3 KCameraTranformation::cameraToGround(const KVecFloat3 & c3d) const
{
    return cameraChain.transform(c3d);
    //KVecFloat3  res;
    //res.copyFrom(gc);
    //delete  &gc;
    //return res;

}

KVecFloat3  KCameraTranformation::camera2dToGround(const KVecFloat2 & c2d) const
{
    return cameraToGround(camera2dTo3d(c2d));

}
KVecFloat3  KCameraTranformation::cameraToGroundProjection(const KVecFloat3 & c3d,float height) const
{
    KVecFloat3  res=cameraChain.transform(c3d);
    //KVecFloat3  res;
    res.scalar_mult((-thepose.cameraZ+height)/res(2));
    //res.copyFrom(gc);
    //delete  &gc;
    return res;
}


KVecFloat3  KCameraTranformation::camera2dToGroundProjection(const KVecFloat2 & c2d,float height) const
{
    return cameraToGroundProjection(camera2dTo3d(c2d),height);
}




KVecFloat2  KCameraTranformation::groundToCamera2d(const KVecFloat3 & g) const
{
    KVecFloat3  res=cameraChainInv.transform(g);
    //KVecFloat3  res;
    //res.copyFrom(c);
    //delete &c;
    return camera3dTo2d(res);
}


float KCameraTranformation::vectorAngle(const KVecFloat2 & v1,const KVecFloat2 & v2) const
{
    float sqrdf=sqrd(thepose.focallength);
    float nom= v1(0)*v2(0)+v1(1)*v2(1)+sqrdf;
    float den= sqrt( (sqrd(v1(0))+sqrd(v1(1)) + sqrdf )*(sqrd(v2(0))+sqrd(v2(1)) + sqrdf ) );
    return acos( nom/den);

}
measurement KCameraTranformation::angularDistance(const KVecFloat2 & v1,const KVecFloat2 & v2,float realsize) const
{

    //Keep V1 Constant, and move v2
    float angs,angl;
    float dist,ang;
    KVecFloat2 v=v2;
	ang=vectorAngle(v1,v);
	dist=realsize/(sin(ang));


    //Fail small : move v towards v1 by 1 pixel
    v1(0)<v(0)?v(0)-=0.707:v(0)+=0.707;
    v1(1)<v(1)?v(1)-=0.707:v(1)+=0.707;
    angs=vectorAngle(v1,v);

    v=v2;

     //Fail large : move v away from v1 by 1 pixel
    v1(0)<v(0)?v(0)+=0.707:v(0)-=0.707;
    v1(1)<v(1)?v(1)+=0.707:v(1)-=0.707;
    angl=vectorAngle(v1,v);


    float dists,distl;//Large distance comes from small angularDistance
    dists=realsize/(sin(angl));
    distl=realsize/(sin(angs));
    //cout<<"ds:"<<dists<<","<<"dl:"<<distl<<endl;
    measurement m;
    m.mean=dist;
    m.var=(sqrd(dists-dist)+sqrd(distl-dist))/2;
    return m;

}

/* Estimate a distance from observed angular size, create also a  estimate of variance, by assuming +/-1 pixel error
*/
measurement KCameraTranformation::angularDistanceProjected(const KVecFloat2 & v1,const KVecFloat2 & v2,float realsize) const
{

    //Keep V1 Constant, and move v2
    float angs,angl;
	float dist,ang;
    KVecFloat2 v=v2;
	ang=vectorAngle(v1,v);
	dist=realsize/(sin(ang));

    //Fail small : move v towards v1 by 1 pixel
    v1(0)<v(0)?v(0)-=0.707:v(0)+=0.707;
    v1(1)<v(1)?v(1)-=0.707:v(1)+=0.707;
    angs=vectorAngle(v1,v);

    v=v2;

     //Fail large : move v away from v1 by 1 pixel
    v1(0)<v(0)?v(0)+=0.707:v(0)-=0.707;
    v1(1)<v(1)?v(1)+=0.707:v(1)-=0.707;
    angl=vectorAngle(v1,v);


    float dists,distl;//Large distance comes from small angularDistance
    dists=realsize/(sin(angl));
    distl=realsize/(sin(angs));
    //Project to ground...
	//cout<<"ds:"<<dists<<","<<"dl:"<<distl<<endl;
	dist=sqrt(sqrd(dist)-sqrd(thepose.cameraZ-realsize));
    dists=sqrt(sqrd(dists)-sqrd(thepose.cameraZ-realsize));
    distl=sqrt(sqrd(distl)-sqrd(thepose.cameraZ-realsize));
    if(isnan(dists)) dists=0;
    if(isnan(distl)) distl=0;
    //cout<<"ds:"<<dists<<","<<"dl:"<<distl<<endl;
    measurement m;
    m.mean=dist;//(dists+distl)/2;
    m.var=(sqrd(dists-m.mean)+sqrd(distl-m.mean))/2;
    return m;

}
//Heght:: Point distance from ground plane
measurement* KCameraTranformation::projectionDistance(KVecFloat2 &v,float height) const
{
    measurement* res=new measurement[2];
    KVecFloat2 t;
    KVecFloat3  s[4];
    KVecFloat2  p[4];//Polar
    t=v;
    t(0)-=1;
    t(1)-=1;
    s[0]=camera2dToGroundProjection(t,height);
    t=v;
    t(0)+=1;
    t(1)-=1;
    s[1]=camera2dToGroundProjection(t,height);
    t=v;
    t(0)+=1;
    t(1)+=1;
    s[2]=camera2dToGroundProjection(t,height);
    t=v;
    t(0)-=1;
    t(1)+=1;
    s[3]=camera2dToGroundProjection(t,height);



    for(int i=0;i<4;i++)
    {
        //s[i].prettyPrint();
        p[i](0)=sqrt( sqrd(s[i](0))+sqrd(s[i](1)));
        if(s[i](0)!=0)
			p[i](1)=atan2(s[i](1),s[i](0));//y/x
		else
			p[i](1)=0;
		//cout<<"b:"<<p[i](1)<<endl;
    }
    res[0].mean=(p[0](0)+p[1](0)+p[2](0)+p[3](0))/4;
    res[0].var=sqrd(p[0](0)-res[0].mean)+sqrd(p[1](0)-res[0].mean)+sqrd(p[2](0)-res[0].mean)+sqrd(p[3](0)-res[0].mean);
    res[0].var/=4;

    res[1].mean=(p[0](1)+p[1](1)+p[2](1)+p[3](1))/4;


    res[1].var=sqrd(p[0](1)-res[1].mean)+sqrd(p[1](1)-res[1].mean)+sqrd(p[2](1)-res[1].mean)+sqrd(p[3](1)-res[1].mean);
    res[1].var/=4;
    //cout<<"bearing mean:"<<res[1].mean<<endl;
    return res;

}


float KCameraTranformation::getPitch() const
{

    //TODO::
    return atan(-cameraChain(2,0)/sqrt(sqrd(cameraChain(2,1))+cameraChain(2,2)));
}


float KCameraTranformation::getRoll() const
{
    return atan(cameraChain(2,1)/cameraChain(2,2));
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif
