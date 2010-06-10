#ifndef __KMAT_H__
#define __KMAT_H__
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <iomanip>
#include <limits>
#include <math.h>

/**
 * KMat (koimat`) :: Kouretes Matrix Library!
 * Provides a templated statically binded (sounds exotic :P )
 * matrix library 
 * optimized for small matrices (ie NOT optimized for large matrices) 
 * 
 * @author vosk
 * */
namespace KMat
{
	using std::runtime_error;
	class MatrixIndexOutOfBoundsException : public std::runtime_error
	{
		public:
		MatrixIndexOutOfBoundsException(std::string m="") :	runtime_error(m.append( ": MatrixIndexOutOfBoundsException : attempted to access an invalid element") ) {}
	};
	class SingularMatrixInvertionException : public std::runtime_error
	{
		public:
		SingularMatrixInvertionException(std::string m="") :	runtime_error(m.append( ": SingularMatrixInvertionException : attempted to invert a singular matrix") ) {}
	};
			
	/*
	 * Base class, utilizes the CRTP idiom to provide a nice way of implementing static polymorphism
	 * 
	 */
	template <template<typename , unsigned , unsigned > class	D,typename T, unsigned M,unsigned N>
	class BaseMatrix{
		template <template<typename , unsigned , unsigned > class	AD,typename AT, unsigned AM,unsigned AN>	friend class BaseMatrix;
		
		public:
			typedef std::numeric_limits<T> Tlimits;
			
			//============================	BASIC FUNCTIONS ============================
			/**
			 *	In place add another matrix to this
			 **/
			D<T,M,N>& add( D<T,M,N> const& rop)
			{
				for(int i=0;i<M;i++)
					for(int j=0;j<N;j++)
						data[i][j]+=rop.data[i][j];
					
				return static_cast< D<T,M,N> &> (*this);
			};
			/**
			 * In place subtract another matrix to this
			 **/
			D<T,M,N>& sub( D<T,M,N> const& rop)
			{
				
				for(int i=0;i<M;i++)
					for(int j=0;j<N;j++)
						data[i][j]-=rop.data[i][j];
					
				return static_cast< D<T,M,N> &> (*this);
			};

			
			/** Generic Multiply with another matrix
			 *	TODO: not faster than "slow" multiplication
			 **/
			template<unsigned L> D<T,M,L> & mult( D<T,N,L> const& rop)
			{
				D<T,M,L> *res= new D<T,M,L>();
				std::cout<<"SLOW"<<std::endl;
				//For each line of the resulting array
				for(int i=0;i<M;i++)
				{
					//For each element of this row
					for(int j=0;j<L;j++)
					{
						//Clear value
						res->data[i][j]=0;
						for(int k=0;k<N;k++)
						{
							res->data[i][j]+=data[i][k]*rop.data[k][j];
							
						}
					}

				}
				return *res;
			};
			
			/** Generic Multiply with another square matrix, so result is same size as D<T,M,N>
			 *	TODO: not faster than "slow" multiplication 
			 *	TODO: maybe "loose" the temp product space?
			 **/
			D<T,M,N>& mult( D<T,N,N> const& rop)//in place mult!!!
			{
				D<T,M,N> *tmp= new D<T,M,N>();
				std::cout<<"IN PLACE"<<std::endl;
				//For each line of the resulting array
				for(int i=0;i<M;i++)
				{
					//For each element of this row
					for(int j=0;j<N;j++)
					{
						//Clear value
						tmp->data[i][j]=0;
						for(int k=0;k<N;k++)
						{
							tmp->data[i][j]+=data[i][k]*rop.data[k][j];
							
						}
					}

				}
				//Get Data back to *this
				for(int i=0;i<M;i++)
					for(int j=0;j<N;j++)
						data[i][j]=tmp->data[i][j];
				delete tmp;
				return static_cast< D<T,M,N> &> (*this);
			};
			
		



			//============================	Scalar operations ============================
			/**
			 * Add a scalar
			 */
			D<T,M,N> & scalar_add(	const	 T	 scalar)
			{
					
				for(int i=0;i<M;i++)
					for(int j=0;j<N;j++)
						data[i][j]+=scalar;
					
				return static_cast< D<T,M,N> &> (*this);
			};
			 /**
			 * Subtract a scalar
			 */
			D<T,M,N> & scalar_sub(const	T scalar)
			{
					
				for(int i=0;i<M;i++)
					for(int j=0;j<N;j++)
						data[i][j]-=scalar;
					
				return static_cast< D<T,M,N> &> (*this);
			};
			 /**
			 * Multiply with	a scalar
			 */
			D<T,M,N> & scalar_mult(const	T scalar)
			{
					
				for(int i=0;i<M;i++)
					for(int j=0;j<N;j++)
						data[i][j]*=scalar;
					
				return static_cast< D<T,M,N> &> (*this);
			};

			/**
			 * Transpose Matrix
			 */
			D<T,N,M>& transp()
			{
				D<T,N,M> *ngen= new D<T,N,M>();
				for(int i=0;i<M;i++)
				{
					for(int j=0;j<N;j++)
					{
						ngen->data[j][i]=data[i][j];
					}
				}
				return *ngen;

			};

			/**
			 * Return a new copy of this
			 **/
			D<T,M,N> & clone()
			{
				//std::cout<<sizeof(D<T,M,N>)<<std::endl;
				//std::cout<<sizeof(D<T,M,N>)<<std::endl;
				D<T,M,N> *ngen= new D<T,M,N>();
				memcpy(ngen ,this, sizeof(D<T,M,N>)	);
				return static_cast< D<T,M,N> &> (*ngen);
			};
			D<T,M,N> & copyTo(D<T,M,N> & dest ) const
			{
				//std::cout<<sizeof(D<T,M,N>)<<std::endl;
				//std::cout<<sizeof(D<T,M,N>)<<std::endl;
				//D<T,M,N> *ngen= new D<T,M,N>();
				memcpy(dest.data ,this->data, sizeof(T)*M*N);
				return static_cast< D<T,M,N> &> (*this);
			};
			D<T,M,N> & copyFrom(const D<T,M,N> & src )
			{
				//std::cout<<sizeof(D<T,M,N>)<<std::endl;
				//std::cout<<sizeof(D<T,M,N>)<<std::endl;
				//D<T,M,N> *ngen= new D<T,M,N>();
				memcpy(this->data,src.data, sizeof(T)*M*N);
				return static_cast< D<T,M,N> &> (*this);
			};
						
			/** Zero out matrix
			 * 
			 **/
			D<T,M,N>& zero()
			{
				memset(data ,0 , sizeof(T)*N*M);
				//Fill main diagonal
				//int l=M<N?M:N;
				//for(int i=0;i<l;i++)
					//data[i][i]=1;
				return static_cast< D<T,M,N> &> (*this);
			};
			
			/** Make matrix Identity (or in case its rectangular,pad with zeros)
			 * 
			 **/
			D<T,M,N>& identity()
			{
				memset(data ,0 , sizeof(T)*N*M);
				//Fill main diagonal
				int l=M<N?M:N;
				for(int i=0;i<l;i++)
					data[i][i]=1;
				return static_cast< D<T,M,N> &> (*this);
			};
			//Accessor
			T& get(unsigned i,unsigned j) 
			{ 
				if(i<1 || j<1 ||i>M ||j>N)
				{	
					std::string d("BaseMatrix.get() ");

					throw MatrixIndexOutOfBoundsException(d);
					//throw MatrixIndexOutOfBoundsException(d);
					return data[0][0];
				}
				return data[i-1][j-1];
			};

			/**
			 * For debuging mainly
			 */
			D<T,M,N> & prettyPrint()
			{
				using namespace std;
				cout<<M<<"x"<<N<<" Matrix"<<endl;
				//Print header:
				
				cout<<"+";
				for(int i=0;i<N;i++)
					cout<<"		 -";
				cout<<"+"<<endl;
				for(int i=0;i<M;i++)
				{
					cout<<"|";
					for(int j=0;j<N;j++)
					{
						cout<<setprecision(3)<<setw(6)<<data[i][j];
					}
					cout<<"|"<<endl;;
				}
				
				
				//Print footer:
				cout<<"+";
				for(int i=0;i<N;i++)
					cout<<"		 -";
				cout<<"+"<<endl;
				
				return static_cast< D<T,M,N> &> (*this);
				
			};
			//=== Operator overloading========
			T& operator() (unsigned i,unsigned j) { return get(i,j);};
			
		protected:
			 T data[M][N];
	 };
	 //GenMatix: simply a BaseMatrix Instantation :)
	 template <typename T, unsigned M,unsigned N>class GenMatrix: public BaseMatrix<GenMatrix,T,M,N>{ };
	 //========================================Square matrices====================================
	 //Transpose for square matrices
	 template <typename A,unsigned S> 
	 GenMatrix<A,S,S> & transpose_square_matrix(GenMatrix<A,S,S> & athis)
	 {
		 
		 for(int i=0;i<S;i++)
				{
					for(int j=0;j<i;j++)
					{
						A tempdata=athis.data[i][j];
						athis.data[i][j]=athis.data[j][i];
						athis.data[j][i]=tempdata;
					}
				}
			
		 return athis;
	 };
	 
	 // Matrix inversion	function -- TODO: Implement :P
	 template <typename A,unsigned S> 
	 GenMatrix<A,S,S> & invert_square_matrix(GenMatrix<A,S,S> & athis)
	 {
				std::string d("KMat:invert_square_matrix<T,S,S>() ");
				throw SingularMatrixInvertionException(d);
	 };
	 template <typename A>
	 GenMatrix<A,2,2> & invert_square_matrix(GenMatrix<A,2,2> & athis)
	 {
				//using BaseMatrix<typename GenMatrix,2,2>::data;
				A determ=athis.data[0][0]*athis.data[1][1]-athis.data[0][1]*athis.data[1][0];
				//std::cout<<"Det:"<<determ<<std::endl;
				//std::cout<<"Eps:"<<std::numeric_limits<T>::epsilon()<<std::endl;
				if(determ > std::numeric_limits<A>::epsilon()&& determ!=(A)0)//can invert
				{
					A temp1=athis.data[0][0];
					athis.data[0][0]=athis.data[1][1]/determ;
					athis.data[1][1]=temp1/determ;
					
					A temp2=athis.data[0][1];
					athis.data[0][1]=-athis.data[1][0]/determ;
					athis.data[1][0]=-temp2/determ;
					return athis;
					
				}
				std::string d("KMat::invert_square_matrixGenMatrix<A,2,2>");
				throw SingularMatrixInvertionException(d);
	 };
	 
	template <typename A>
	 GenMatrix<A,3,3> & invert_square_matrix(GenMatrix<A,3,3> & athis)
	 {
				//using BaseMatrix<typename GenMatrix,2,2>::data;
				//Minor 1: based on 1,1
				A m1=athis.data[0][0]*(athis.data[1][1]*athis.data[2][2]-athis.data[1][2]*athis.data[2][1]);
				//Minor 2: based on 1,2
				A m2=athis.data[0][1]*(athis.data[1][0]*athis.data[2][2]-athis.data[2][0]*athis.data[1][2]);
				//Minor 2: based on 1,3
				A m3=athis.data[0][2]*(athis.data[1][0]*athis.data[2][1]-athis.data[2][0]*athis.data[1][1]);
				A determ=m1-m2+m3;
				
				std::cout<<"Det:"<<determ<<std::endl;
				std::cout<<"Eps:"<<std::numeric_limits<A>::epsilon()<<std::endl;
				if(determ > std::numeric_limits<A>::epsilon()&& determ!=(A)0)//can invert
				{
					GenMatrix<A,3,3> & t=athis.clone();
					
					athis.data[0][0]=(t.data[1][1]*t.data[2][2]-t.data[1][2]*t.data[2][1])/determ;
					athis.data[0][1]=(t.data[0][2]*t.data[2][1]-t.data[0][1]*t.data[2][2])/determ;
					athis.data[0][2]=(t.data[0][1]*t.data[1][2]-t.data[0][2]*t.data[1][1])/determ;
					
					athis.data[1][0]=(t.data[1][2]*t.data[2][0]-t.data[1][0]*t.data[2][2])/determ;
					athis.data[1][1]=(t.data[0][0]*t.data[2][2]-t.data[0][2]*t.data[2][0])/determ;
					athis.data[1][2]=(t.data[0][2]*t.data[1][0]-t.data[0][0]*t.data[1][2])/determ;
					
					athis.data[2][0]=(t.data[1][0]*t.data[2][1]-t.data[1][1]*t.data[2][0])/determ;
					athis.data[2][1]=(t.data[0][1]*t.data[2][0]-t.data[0][0]*t.data[2][1])/determ;
					athis.data[2][2]=(t.data[0][0]*t.data[1][1]-t.data[0][1]*t.data[1][0])/determ;
					delete	&t;
					return athis;
					
				}
				std::string d("KMat::invert_square_matrixGenMatrix<A,2,2>");
				throw SingularMatrixInvertionException(d);
	 };
	 
	 
	 
	 
	 
	 //Partial specialization for square matrices	 
	 template<typename T,unsigned S> class GenMatrix<T,S,S> : public BaseMatrix<GenMatrix,T,S,S>
	 {
		 friend GenMatrix<T,S,S> & transpose_square_matrix<>(GenMatrix<T,S,S> & athis);
		 friend GenMatrix<T,S,S> & invert_square_matrix<>(GenMatrix<T,S,S> &athis);
		 public:
			GenMatrix<T,S,S> & transp() { return transpose_square_matrix(*this) ; };//Override for square
			GenMatrix<T,S,S> & invert() { return invert_square_matrix(*this) ; };//Override for square
	 };
	 
	 //A class to represent homogenous coordinates!
	 template<typename T,unsigned S> class HCoords : public GenMatrix<T,S,1>{
			public:
				using GenMatrix<T,S,1>::get;
				//using GenMatrix<T,S,1>::clone;
				HCoords() {};
				//Constructor from a GenMatrix<T,S,1>
				HCoords( GenMatrix<T,S,1> & m) 
				{
					copyFrom(m);//Simply use GenMatrix`s ability to copyitself :)
				}
				HCoords<T,S> & cloneCoords() const
				{
					//HCoords<T,S> *a=new HCoords<T,S>();
					//a->copyFrom(*this);
					return * (new HCoords<T,S>(*this));
				}
				//=== Operator overloading========
				T& operator() (unsigned i){ return get(i,1);};
		 
		 };
	 /*
	 {
		 protected:
			GenMatrix<T,S,1> Coords;
			T hpart;//Should be 1 :D
		public:
			HCoords(){ Coords.zero(); hpart=1;};
			HCoords<T,S> & zero() {Coords.zero(); hpart=1;};S
	 }*/
	 
	 // Affine	transform matrix, using homogenous coordinates!!
	 /* Internal represantation is one S-1 X S-1 matrix A
		* and one S-1 x 1 B (homogenous coefs)
		* | A B|
		* | 0 1|
		*/
		class rigid;//Forward declaration
	 template<typename T,unsigned S> class ATMatrix
	 {
		
			friend class rigid;
		protected: 
			GenMatrix<T,S-1,S-1> A;//The main tranform matrix
			GenMatrix<T,S-1,1> B; // The homogenous part of the transform
			/*
			 * Since this library is focused on rigid transformations, most of
			 * ATMatrices are going to be rigid transformations, so either B is
			 * going to be zero, or A is going to be identity...
			 * Therefore, keeping this extra info might just save a few cycles
			 * by avoiding unnecessary computations
			 */
			bool AisZero, AisIdentity,BisZero;
		public:
			ATMatrix():AisZero(false),AisIdentity(false),BisZero(false) {};
			ATMatrix<T,S> & add ( ATMatrix<T,S> const& rop)
			{
				A.add(rop.A);
				if(AisZero==true)
				{
					AisZero=rop.AisZero;
					AisIdentity=rop.AisIdentity;
				}
				else if(AisIdentity==true)
				{
					AisZero=false;
					AisIdentity=rop.AisZero;//If otherside is zero, remains Id
				}
				B.add(rop.B);
				if(BisZero==true)
					BisZero=rop.BisZero;
				return *this;
			};
		
			ATMatrix<T,S> & sub ( ATMatrix<T,S> const& rop)
			{
				A.sub(rop.A);
				if(AisZero==true)
				{
					AisZero=rop.AisZero;
					AisIdentity=rop.AisIdentity;
				}
				else if(AisIdentity==true)
				{
					AisZero=false;
					AisIdentity=rop.AisZero;//If otherside is zero, remains Id
				}
				B.sub(rop.B);
				if(BisZero==true)
					BisZero=rop.BisZero;
				return *this;
			};
			
			//Multiplication, optimized for HTMatrices!!
			ATMatrix<T,S> & mult (ATMatrix<T,S> const& rop)
			{
				GenMatrix<T,S-1,1> & axd=A.mult(rop.B);
				//std::cout<<"WTF!"<<std::endl;
				if(AisIdentity==true)//Mult with id , just copy
				{
					A.copyFrom(rop.A);
					AisZero=rop.AisZero;
					AisIdentity=rop.AisIdentity;
				}
				else if(AisZero==false)//Not id and not zero, just do the math
				{
					A.mult(rop.A);
					AisZero=rop.AisZero;
					AisIdentity=rop.AisIdentity;
				}
				
				B.add(axd);
				if(BisZero==true)
					BisZero=rop.BisZero;
				
				return *this;
			
			}
			
			//Project a point !
			HCoords<T,S-1>& transform(HCoords<T,S-1> const & c) 
			{
				HCoords<T,S-1> *nc;
				if(AisZero==true)//A matrix zero, dont try to do the math:p
				{
					nc=new  HCoords<T,S-1>();
					nc->zero();//Result is definately zero
				}
				else if( AisIdentity==true)//A matrix id, no change in c
				{
					nc=& c.cloneCoords();//Just clone coords
				}
				else//Just do the math :p
				{
					nc=new HCoords<T,S-1>();
					GenMatrix<T,S-1,1> & a=A.mult(c);
					nc->copyFrom(a);
					delete &a;
				}
				if(BisZero==false)//do some more math
					nc->add(B);
				return *nc;
			}
			
			ATMatrix<T,S> & invert()
			{
				if(AisIdentity!=true)
					A.invert();
				if(BisZero==false)
				{
					if(AisIdentity==false&&AisZero==false)//Just do the math
					{
						GenMatrix<T,S-1,1> & newb=A.mult(B);
						newb.scalar_mult(-1);
						B=newb;
						delete &newb;
					}
					else if(AisZero==true)//Result is def zero
						B.zero();
					//else unchanged :) AisIdentity==true
				}
				return *this;
			}
			
			ATMatrix<T,S>& prettyPrint()
			{
				A.prettyPrint();
				B.prettyPrint();
				return *this;
			}
			
			ATMatrix<T,S>& identity()
			{
				A.identity();
				AisIdentity=true;
				AisZero=false;
				B.zero();
				BisZero=true;
				return *this;
			}
			ATMatrix<T,S>& zero()
			{
				A.zero();
				AisIdentity=false;
				AisZero=true;
				B.zero();
				BisZero=true;
				return *this;
			}
			ATMatrix<T,S>& check()//Just update booleans
			{
				BisZero=true;
				for(int i=1;i<S;i++)
					if(B(i,1)!=0)
					{
						BisZero=false;
						break;
					}
				
				AisZero=true;
				AisIdentity=true;
					for(int i=1;i<S;i++)
					{
						if(A(i,i)!=1)
							AisIdentity=false;
						for(int j=1;j<S;j++)
						{
							if(A(i,j)!=0)
								AisZero=false;
						}
						if(AisIdentity==false&&AisZero==false)
							break;
					}
				
			}
		 
		};
		class rigid{
			public:
				static double PI;
				template<typename T> static void rotate(ATMatrix<T,3> & m, T theta)
				{
					m.identity();
					m.AisIdentity=false;
					m.A(1,1)=cos(theta);
					m.A(1,2)=-sin(theta);
					m.A(2,1)=sin(theta);
					m.A(2,2)=cos(theta);
				}
				template<typename T> static void shearX(ATMatrix<T,3> & m, T factor)
				{
					m.identity();
					m.AisIdentity=false;
					m.A(1,2)=factor;
				};
				template<typename T> static void shearY(ATMatrix<T,3> & m, T factor)
				{
					m.identity();
					m.AisIdentity=false;
					m.A(2,1)=factor;
				};
				
				template<typename T,unsigned S> static void translate(ATMatrix<T,S> & m, HCoords<T,S-1> t)
				{
					
					m.identity();
					m.AisIdentity=true;
					m.AisZero=false;
					m.BisZero=false;
					m.B.copyFrom(t);
				}
				template<typename T, unsigned S> static void scale(ATMatrix<T,S> &m, HCoords<T,S-1> t)
				{
					m.identity();
					m.AisIdentity=false;
					for(int i=1;i<S-1;i++)
						m.A(i,i)=t(i);
					
				}
				template<typename T> static void rotateZ(ATMatrix<T,4> & m, T theta)
				{
					m.identity();
					m.AisIdentity=false;
					m.A(1,1)=cos(theta);
					m.A(1,2)=-sin(theta);
					m.A(2,1)=sin(theta);
					m.A(2,2)=cos(theta);
					
				}
				template<typename T> static void rotateY(ATMatrix<T,4> & m, T theta)
				{
					m.identity();
					m.AisIdentity=false;
					m.A(1,1)=cos(theta);
					m.A(1,3)=sin(theta);
					m.A(3,1)=-sin(theta);
					m.A(3,3)=cos(theta);
					
					
				}
				template<typename T> static void rotateX(ATMatrix<T,4> & m, T theta)
				{
					m.identity();
					m.AisIdentity=false;
					
					m.A(2,2)=cos(theta);
					m.A(2,3)=-sin(theta);
					m.A(3,2)=sin(theta);
					m.A(3,3)=cos(theta);
				}
		};
		double rigid::PI=3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811;
		
	 /*template<typename T> class GenMatrix<T,2,2> : public BaseMatrix<GenMatrix,T,2,2> 
	 {
		 private:
			using BaseMatrix< KMat :: template GenMatrix ,T,2,2> ::data;
			
			friend GenMatrix<T,2,2> & transpose_square_matrix<>(GenMatrix<T,2,2> & athis);
		 public:
			GenMatrix<T,2,2> & transp() { return transpose_square_matrix(*this) ; };//Override for square
			//Invert 2x2 matrix!
			GenMatrix<T,2,2> & invert()			{
				
			
				
			};
			
			
	 };*/
		
	 //Ugly way to implement typedef templates :/
	 //template<typename T,unsigned M,unsigned N>
	 //struct RectMatrix{
	//	 typedef KMat::GenMatrix<GenMatrix,T,M,N> type;
		 
	// }
	 
	 // template < typename T, unsigned M,unsigned N> class GenMatrix<T,M,N,GenMatrix<T,M,N,GenMatrix> >: public BaseMatrix<GenMatrix,T,M,N>{};
//#define SquareMatrix$	SquareMatrix<T,S,S>
//	//TODO: redefine T_* stuff and transp for in-place operation
//	 template <typename T,unsigned S> class SquareMatrix: public BaseMatrix<T,S,S>
//	 {
//		/**
//		*	Transpose , add rhs and transpose 
//back : this'+rhs
//		**/
//		SquareMatrix<T,S,S> & T_add( const BaseMatrix<T,S,S> &rhs);
//		 
//		 
//	 };
	//typedef BaseMatrix<float,3,3> BaseMatrix3d;
};



//WTF!! C++ I REALLY HATE YOU!! WHY WHY WHY
//#include "KMat.cc"




#endif 
