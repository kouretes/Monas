#ifndef BASICTIMESTAMP_HPP
#define BASICTIMESTAMP_HPP

#include "boost/static_assert.hpp"
#include <cmath>
#include <limits>
#include <sstream>
#include <string>


namespace KSystem
{
    namespace Time
    {

        template<typename Rep,unsigned long long Denominator>
        class BasicTimeStamp
        {
            public:
            typedef Rep rep;
            enum constnts_e{TPS=Denominator};
            rep p;



            template<typename T >
            static  BasicTimeStamp upcast(T const&b)
            {
                BasicTimeStamp a;
                BOOST_STATIC_ASSERT(Denominator>=T::TPS);//Denominator);
                //d();
                a.p=b.p*(Denominator/T::TPS);
                return a;

            };



            BasicTimeStamp operator-(BasicTimeStamp const &b) const
            {
                BasicTimeStamp res;
                res.p=this->p;
                res.p-=b.p;
                return res;
            };
            BasicTimeStamp operator+(BasicTimeStamp const &b) const
            {
                BasicTimeStamp res;
                res.p=this->p;
                res.p+=b.p;
                return res;
            };

             BasicTimeStamp & operator+=(BasicTimeStamp const &b)
            {

                p+=b.p;
                return (*this);
            };

             BasicTimeStamp & operator-=(BasicTimeStamp const &b)
            {
                p-=b.p;
                return (*this);
            };

            template<typename T>
            BasicTimeStamp operator*=(T const b)
            {
                p*=b;
                return *this;
            }
            bool operator<(BasicTimeStamp const&b) const { return p<b.p; };
            bool operator>(BasicTimeStamp const&b) const { return p>b.p; };
            bool operator<=(BasicTimeStamp const&b) const { return p<=b.p; };
            bool operator>=(BasicTimeStamp const&b) const { return p>=b.p; };
            bool operator==(BasicTimeStamp const&b) const { return p==b.p; };




            float toFloat() const
            {
                float r=p;
                //std::cout<<"Tofloat:"<<p<<std::endl;
                r/=Denominator;
                return r;
            }
            static BasicTimeStamp fromFloat(float d)
            {
                BasicTimeStamp res;
                res.p=d*Denominator;
                return res;

            }
            static BasicTimeStamp hours(long h)
            {
                return seconds(h*60);
            }
            static BasicTimeStamp minutes(long h)
            {
                return seconds(h*60);
            }
            static BasicTimeStamp seconds(long sec)
            {
                BasicTimeStamp out;
                out.p=sec*Denominator;
                return out;
            }
            static BasicTimeStamp milliseconds(long ms)
            {
                BasicTimeStamp out;
                out.p=autoScale<1000ul,long>(ms);
                return out;
            }
            static BasicTimeStamp microseconds(long micros)
            {
                BasicTimeStamp out;
                out.p=autoScale<1000000ul,long>(micros);
                return out;
            }

            static BasicTimeStamp getWrap()
            {
                BasicTimeStamp output;
                double r=std::numeric_limits<Rep>::max();
                r=r/Denominator;
                if(r>60)
                    r=floor(r/60)*60;
                else if(r>1)
                    r=floor(r);
                output.p=(r)*Denominator;

                return output;
            }
            Rep raw() const { return p;};


             //Null unwrap
            BasicTimeStamp unwrap(BasicTimeStamp const& a)
            {
                return a;
            };
            template<typename T>
            BasicTimeStamp unwrap(T const& a) const
            {
                BasicTimeStamp res;
                res.p=p;
                Rep wr=upcast(T::getWrap()).p;
                Rep ap=upcast(a).p;
                BOOST_STATIC_ASSERT(sizeof(rep)>= sizeof(typename T::rep));
                //BOOST_STATIC_ASSERT(getWrap()>= upcast(a.getWrap()));
                Rep r=res.p%wr;
                if(r<ap) res.p-=wr;
                res.p-=r;
                res.p+=ap;
                return res;
            }

            /*template<typename Base,typename Result>
            Result wrapTo() const
            {
                Base unwraped=Base::unwrap(*this);
                return unwraped.wrapBase<Result>();
            }*/
            //private:
            template<typename T>
            T wrapTo() const
            {

                BasicTimeStamp r;
                r.p=p;

                Rep wr=upcast(T::getWrap()).p;
                //std::cout<<wr<<std::endl;
                //std::cout<<r.p<<std::endl;
                r.p=r.p%wr;
                //std::cout<<r.p<<std::endl;
                return r.downcast<T>();
            }

            std::string toString() const{
                std::ostringstream ost;
                ost << p;
                ost.flush();
                return ost.str();
            }
            void fromString(std::string const&a) {
                 std::istringstream ist(a);
                 ist>>p;
            }

            private:
            template<typename T >
            T downcast()
            {
                T a;
                if(Denominator<T::TPS)
                    a.p=p*(T::TPS/Denominator);
                else if(Denominator>T::TPS)
                    a.p=p/(Denominator/T::TPS);
                else
                    a.p=p;
                return a;

            };

            template<unsigned long long SCALE,typename D>
            static rep autoScale(D const&a)
            {
                return autoscalehelper<SCALE, D,Denominator<SCALE,Denominator==SCALE>::scale(a);
            }

            template<unsigned long long SCALE,typename D,bool L,bool E>
            class autoscalehelper {};

            template<unsigned long long SCALE,typename D>
            class autoscalehelper<SCALE,D,false,true> {
                public:
                static rep scale(D const &a) { return a;};
            };

            template<unsigned long long SCALE,typename D>
            class autoscalehelper<SCALE,D,true,false> {
                public:
                static rep scale(D const &a) { return a/(SCALE/Denominator);};
            };
              template<unsigned long long SCALE,typename D>
            class autoscalehelper<SCALE,D,false,false> {
                public:
                static rep scale(D const &a) { return a*(Denominator/SCALE);};
            };


        };
    }
}






#endif //BASICTIMESTAMP_HPP
