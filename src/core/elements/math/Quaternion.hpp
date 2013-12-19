#ifndef QUATERNION_HPP__
#define QUATERNION_HPP__
#include <core/elements/math/KMat.hpp>


/**
 * Quaternion Representation
 * @author vosk
 * */
namespace KMath{


	template<typename T> class Quaternion : public KMat::GenMatrix<T,4,1>
	{


		public:

		Quaternion<T> & quaternion_product(Quaternion<T> const&rhs)
		{
			if(this->h==NULL)
				return *this;

			float q=this->read(3,0);
			float p=rhs.read(3,0);
			KMat::GenMatrix<T,3,1> vq,vp,r;
			vq.zero();
			vp.zero();
			r.zero();
			for(unsigned i=0;i<3;i++)
			{
				vq(i)=this->read(i,0);
				vp(i)=rhs.read(i,0);
			}

			this->get(3,0)=q*p-KMath::KMat::transformations::inner_product(vq,vp);
		    r=vp*q+vq*p+KMath::KMat::transformations::cross_product(vq,vp);
		    for(unsigned i=0;i<3;i++)
			{
				this->get(i,0)=r.read(i,0);
			}


			return *this;
		}


		Quaternion<T>  conjugate() const
		{
			if(this->h==NULL)
				return Quaternion<T>();
			Quaternion<T> r;
			r=*this;
			r(0)=-r(0);
			r(1)=-r(1);
			r(2)=-r(2);
			return r;
		}
		KMat::GenMatrix<T,3,1>  transform(KMat::GenMatrix<T,3,1> const& element) const
		{
			if(this->h=NULL)
				return KMat::GenMatrix<T,3,1>();
			KMat::GenMatrix<T,3,1> r;
			Quaternion<T> qc=conjugate(),v,t;
			v.zero();
			r.zero();
			for(unsigned i=0;i<3;i++)
			{
				v.get(i)=element.read(i);
			}
			t=*this;
			t.quaternion_product(v).quaternion_product(r);
			for(unsigned i=0;i<3;i++)
			{
				r.get(i)=t.read(i);
			}

			return r;
		}
		KMat::GenMatrix<T,3,3> castToMatrix() const
		{
			KMat::GenMatrix<T,3,3> res,s;


			if(this->h==NULL)
				return res;
			KMat::GenMatrix<T,3,1> v;
			float w=this->read(3,0);
			v.zero();
			v(0)=this->read(0,0);
			v(1)=this->read(1,0);
			v(2)=this->read(2,0);
			res.identity();
			res.scalar_mult(w*w-v.norm2());


			res=res+((v*v.transp()).scalar_mult(2));
			s.zero();
			s(0,1)=-v(3);
			s(0,2)=v(2);
			s(1,2)=-v(1);
			s+=s.transp().scalar_mult(-1);//Skew Symmetric Duh
			s.scalar_mult(2*w);
			return res;



		}
		void fromAxisAngle(T x,T y,T z,T the)
		{
			T sth=sin(the/2.0);

			this->get(0,0)=x*sth;
			this->get(1,0)=y*sth;
			this->get(2,0)=z*sth;
			this->get(3,0)=cos(the/2.0);
		}
		void fromDenormalizedAxis(KMat::GenMatrix<T,3,1> t)
		{
			T n=sqrt(t.norm2());
			T sth= sin(n/2.0)/n;
			this->get(0,0)=t(0)*sth;
			this->get(1,0)=t(1)*sth;
			this->get(2,0)=t(2)*sth;
			this->get(3,0)=cos(n/2.0);
		}
		Quaternion<T> &  normalize()
		{
			T n=sqrt(this->norm2());
			for(unsigned i=0;i<4;i++)
				this->get(i)/=n;


			return *this;
		}

	};

};


#endif
