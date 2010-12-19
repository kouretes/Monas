#ifndef __KMAT_H__
#define __KMAT_H__
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <iomanip>
#include <limits>
#include <math.h>


//#define KMAT_INSANE_MODE

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

	template<typename T,typename C> class COWRef
	{
		/*public:
		COWRef(C obj,int a, int b): p(obj),i(a),j(b){};
		//const AT& operator(){ return p.getFunc(i,j) ;} const;
		T& operator=(T const v) { return  p.get(i,j)=v;};
		T& operator=(COWRef<T,C>  const &v) { return p.get(i,j)=v.p.read(v.i,v.j);};
		template<typename AT,typename AC> T operator=(COWRef<AT,AC> const& v) { return p.get(i,j)=v.p.read(v.i,v.j);};
		private:
			C & p;
			int i,j;*/
	};

	template<typename C> class COWRef<float,C>
	{
		template <typename AT, typename AC>	friend class COWRef;
		public:
		COWRef(C obj,int a, int b): p(obj),i(a),j(b){};
		operator float() const { return p.read(i,j) ;} ;
		template<typename AT,typename AC> float operator=(COWRef<AT,AC> const& v) { return p.get(i,j)=v.p.read(v.i,v.j);};
		float operator=(COWRef<float,C>  const &v) { return p.get(i,j)=v.p.read(v.i,v.j);};
		float operator=(float  v) { return p.get(i,j)=v;};
		template<typename AT> float operator+=(AT  v) { return p.get(i,j)+=v;};
		template<typename AT> float operator-=(AT  v) { return p.get(i,j)-=v;};
		template<typename AT> float operator*=(AT  v) { return p.get(i,j)*=v;};
		template<typename AT> float operator/=(AT  v) { return p.get(i,j)/=v;};

		template<typename AT> float operator+(AT  v) { return p.read(i,j)+v;};
		template<typename AT> float operator-(AT  v) { return p.read(i,j)-v;};
		template<typename AT> float operator*(AT  v) { return p.read(i,j)*v;};
		template<typename AT> float operator/(AT  v) { return p.read(i,j)/v;};
		private:
			C & p;
			int i,j;
	};

	template<typename C> class COWRef<int,C>
	{
		template <typename AT, typename AC>	friend class COWRef;
		public:
		COWRef(C obj,int a, int b): p(obj),i(a),j(b){};
		operator int() const { return p.read(i,j) ;} ;
		template<typename AT, typename AC> int operator=(COWRef<AT,AC> const & v) { return p.get(i,j)=v.p.read(v.i,v.j);};
		int operator=(COWRef<int,C>  const &v) { return p.get(i,j)=v.p.read(v.i,v.j);};
		int operator= (int  v) { return p.get(i,j)=v;};
		template<typename AT> int operator+=(AT  v) { return p.get(i,j)+=v;};
		template<typename AT> int operator-=(AT  v) { return p.get(i,j)-=v;};
		template<typename AT> int operator*=(AT  v) { return p.get(i,j)*=v;};
		template<typename AT> int operator/=(AT  v) { return p.get(i,j)/=v;};

		template<typename AT> int operator+(AT  v) { return p.read(i,j)+v;};
		template<typename AT> int operator-(AT  v) { return p.read(i,j)-v;};
		template<typename AT> int operator*(AT  v) { return p.read(i,j)*v;};
		template<typename AT> int operator/(AT  v) { return p.read(i,j)/v;};
		private:
			C & p;
			int i,j;
	};
	/*
	 * Base class, utilizes the CRTP idiom to provide a nice way of implementing static polymorphism
	 *
	 */
	template <template<typename , unsigned , unsigned > class	D,typename T, unsigned M,unsigned N>
	class BaseMatrix
	{
			template <template<typename , unsigned , unsigned > class	AD,typename AT, unsigned AM,unsigned AN>	friend class BaseMatrix;


			template <typename AT, unsigned AM , unsigned AN> class DataContainer
			{
				public:
				AT data[AM][AN];
				std::size_t counts;

				DataContainer() : counts(0) {};
				DataContainer(DataContainer<AT,AM,AN>&) :counts(0) {

				}
				void inc() {++counts;};
				void dec() {--counts;};
				bool isExclusive() {return counts==1;};
				DataContainer<AT,AM,AN> *clone()
				{
					DataContainer<AT,AM,AN> *ngen= new DataContainer<AT,AM,AN>();
					memcpy(ngen->data ,this->data, sizeof(AT[AM][AN])	);

					return ngen;

				}


			};
			typedef DataContainer<T,M,N> DataContainer_t;
			protected:
			bool makeExclusive() //Return true if a new object has been created
			{
				if(h==NULL)
				{
					h= new DataContainer_t;
					//memset(h->data,0,sizeof(T[M][N]));
					h->inc();
					return true;
				}
				if(h->isExclusive())
					return false;
				h->dec();
				h=h->clone();
				h->inc();
				return false;
			}
			void zeroOut()
			{
				if(h==NULL)
				{
					h= new DataContainer_t;
					h->inc();
				}

				memset(h->data,0,sizeof(T[M][N]));
			}
			void cleanHandle()
			{
				if(h==NULL)
					return;
				if(h->isExclusive())
					delete h;
				else
					h->dec();
				h=NULL;
			}
		public:
			BaseMatrix(): h(NULL)
			{

			};
			BaseMatrix(BaseMatrix<D,T,M,N> const& o)
			{
				h=o.h;
				if(h!=NULL)
					h->inc();
			}
			~BaseMatrix()
			{

				cleanHandle();

			}
			typedef std::numeric_limits<T> Tlimits;

			//============================	BASIC FUNCTIONS ============================
			/**
			 *	In place add another matrix to this
			 **/
			D<T,M,N>& add( BaseMatrix<D,T,M,N> const& rop)
			{
				makeExclusive();
				for (unsigned i=0;i<M;i++)
					for (unsigned j=0;j<N;j++)
						h->data[i][j]+=rop.h->data[i][j];

				return static_cast< D<T,M,N> &> (*this);
			};

			/**
			 *	In place add another matrix to this, column wise add
			 **/
			D<T,M,N>& column_add( BaseMatrix<D,T,M,1> const& rop)
			{
				makeExclusive();
				for (unsigned i=0;i<M;i++)
					for (unsigned j=0;j<N;j++)
						h->data[i][j]+=rop.h->data[i][0];

				return static_cast< D<T,M,N> &> (*this);
			};
			/**
			 *	In place add another matrix to this, row wise add
			 **/
			D<T,M,N>& row_add( BaseMatrix<D,T,1,N> const& rop)
			{
				makeExclusive();
				for (unsigned i=0;i<M;i++)
					for (unsigned j=0;j<N;j++)
						h->data[i][j]+=rop.h->data[0][j];

				return static_cast< D<T,M,N> &> (*this);
			};
			/**
			 * In place subtract another matrix to this
			 **/
			D<T,M,N>& sub( BaseMatrix<D,T,M,N> const& rop)
			{
				makeExclusive();
				for (unsigned i=0;i<M;i++)
					for (unsigned j=0;j<N;j++)
						h->data[i][j]-=h->rop.h->data[i][j];

				return static_cast< D<T,M,N> &> (*this);
			};


			/** Generic Multiply with another matrix
			 *	TODO: not faster than "slow" multiplication
			 **/
			template<unsigned L> D<T,M,L>  mult( BaseMatrix<D,T,N,L> const& rop) const
			{
				D<T,M,L> res;
				res.makeExclusive();
				//std::cout<<"SLOW"<<std::endl;
				//For each line of the resulting array
				for (unsigned i=0;i<M;i++)
				{
					//For each element of this row
					for (unsigned j=0;j<L;j++)
					{
						//Clear value
						res.h->data[i][j]=0;
						for (unsigned k=0;k<N;k++)

						{
							res.h->data[i][j]+=h->data[i][k]*rop.h->data[k][j];

						}
					}

				}
				return res;
			};
			/** Generic Multiply with another square matrix, so result is same size as D<T,M,N>
			 *	TODO: not faster than "slow" multiplication
			 *	TODO: maybe "loose" the temp product space?
			 **/
			D<T,M,N> & mult( BaseMatrix<D,T,N,N> const& rop)//in place mult!!!
			{
				D<T,M,N> tmp;
				tmp.makeExclusive();
				//std::cout<<"IN PLACE"<<std::endl;
				//For each line of the resulting array
				for (unsigned i=0;i<M;i++)
				{
					//For each element of this row
					for (unsigned j=0;j<N;j++)
					{
						//Clear value
						tmp.h->data[i][j]=0;
						for (unsigned k=0;k<N;k++)
						{
							tmp.h->data[i][j]+=h->data[i][k]*rop.h->data[k][j];

						}
					}

				}
				this->copyFrom(tmp);//Share handle :)

				return static_cast< D<T,M,N> &> (*this);
			};

			//============================	Scalar operations ============================
			/**
			 * Add a scalar
			 */
			D<T,M,N>&  scalar_add(	const	 T	 scalar)
			{
				makeExclusive();
				for (unsigned i=0;i<M;i++)
					for (unsigned j=0;j<N;j++)
						h->data[i][j]+=scalar;

				return static_cast< D<T,M,N>& > (*this);
			};
			/**
			* Subtract a scalar
			*/
			D<T,M,N>&  scalar_sub(const	T scalar)
			{
				makeExclusive();

				for (unsigned i=0;i<M;i++)
					for (unsigned j=0;j<N;j++)
						h->data[i][j]-=scalar;

				return static_cast< D<T,M,N>& > (*this);
			};
			/**
			* Multiply with	a scalar
			*/
			D<T,M,N>& scalar_mult(const	T scalar)
			{
				makeExclusive();

				for (unsigned i=0;i<M;i++)
					for (unsigned j=0;j<N;j++)
						h->data[i][j]*=scalar;

				return static_cast< D<T,M,N> &> (*this);
			};

			/**
			 * Transpose Matrix
			 */
			D<T,N,M> transp() const
			{
				D<T,N,M> ngen;
				ngen.makeExclusive();
				for (unsigned i=0;i<M;i++)
				{
					for (unsigned j=0;j<N;j++)
					{
						ngen.h->data[j][i]=h->data[i][j];
					}
				}
				return ngen;

			};

			/**
			 * Return a new copy of this
			 **/
			D<T,M,N> clone() const
			{
				return D<T,M,N>(static_cast< D<T,M,N> const&> (*this));
				//return *(new D<T,M,N>(static_cast< D<T,M,N> const&> (*this))); //No biggie, COW

			};
			D<T,M,N> & copyTo(BaseMatrix<D,T,M,N> & dest ) const
			{
				dest.cleanHandle();
				if(h==NULL)
					return;
				dest.h=h;
				assert(h!=NULL);
				h->inc();
				return static_cast< D<T,M,N> &> (*this);
			};
			D<T,M,N> & copyFrom(BaseMatrix<D,T,M,N> const & src )
			{
				if(this==&src)
					return static_cast< D<T,M,N> &> (*this);
				cleanHandle();
				if(src.h==NULL)
					return static_cast< D<T,M,N> &> (*this);
				h=src.h;
				h->inc();
				return static_cast< D<T,M,N> &> (*this);
			};

			/** Zero out matrix
			 *
			 **/
			D<T,M,N>& zero()
			{
				makeExclusive();
				zeroOut();
				//Fill main diagonal
				//unsigned l=M<N?M:N;
				//for(unsigned i=0;i<l;i++)
				//data[i][i]=1;
				return static_cast< D<T,M,N> &> (*this);
			};

			/** Make matrix Identity (or in case its rectangular,pad with zeros)
			 *
			 **/
			D<T,M,N>& identity()
			{
				makeExclusive();
				zeroOut();
				//Fill main diagonal
				unsigned l=M<N?M:N;
				for (unsigned i=0;i<l;i++)
					h->data[i][i]=1;
				return static_cast< D<T,M,N> &> (*this);
			};
			//Accessor
			T& get(unsigned i,unsigned j)
			{
				makeExclusive();
#ifndef KMAT_INSANE_MODE
				if (i>M-1||j>N-1)
				{
					std::string d("BaseMatrix.get() ");

					throw MatrixIndexOutOfBoundsException(d);
					//throw MatrixIndexOutOfBoundsException(d);
					return h->data[0][0];
				}
#endif
				return h->data[i][j];
			};

			//Const accessor//Accessor
			const T& read(unsigned i,unsigned j) const
			{

#ifndef KMAT_INSANE_MODE
				if (i>M-1||j>N-1)
				{
					std::string d("BaseMatrix.get() ");

					throw MatrixIndexOutOfBoundsException(d);
					//throw MatrixIndexOutOfBoundsException(d);
					return h->data[0][0];
				}
#endif
				return h->data[i][j];
			};

			/**
			 * For debuging mainly
			 */
			D<T,M,N> & prettyPrint()
			{
				using namespace std;
				cout<<M<<"x"<<N<<" Matrix"<<endl;
				//Print header:
				if(h==NULL)
				{
					cout<<"(Empty Matrix)"<<endl;
					return static_cast< D<T,M,N> &> (*this);
				}

				cout<<"+";
				for (unsigned i=0;i<N;i++)
					cout<<"   -   ";
				cout<<"+"<<endl;

				for (unsigned i=0;i<M;i++)
				{
					cout<<"|";
					for (unsigned j=0;j<N;j++)
					{
						cout.width(7);
						cout.precision(2);
						cout<<fixed<<h->data[i][j]<<"";//setprecision(3)<<setw(6)<<
					}
					cout<<"|"<<endl;;
				}


				//Print footer:
				cout<<"+";
				for (unsigned i=0;i<N;i++)
					cout<<"   -   ";
				cout<<"+"<<endl;

				return static_cast< D<T,M,N> &> (*this);

			};
			//=== Operator overloading========
			COWRef<T,D<T,M,N> > operator() (unsigned i,unsigned j)
			{
				return COWRef<float,D<T,M,N> > ( static_cast< D<T,M,N>  &> (*this),i,j);
			};
			//Const accessor
			const T& operator() (unsigned i,unsigned j) const
			{
				return read(i,j);
			};
			D<T,M,N> & operator= (const BaseMatrix<D,T,M,N> & d)
			{
			  return copyFrom(d);
			};
			/*D<T,M,N> & operator= (const D<T,M,N> & d)
			{
			  return copyFrom(d);
			};*/


		protected:

			DataContainer_t *h;

	};
	//GenMatix: simply a BaseMatrix Instantation :)
	template <typename T, unsigned M,unsigned N>class GenMatrix: public BaseMatrix<GenMatrix,T,M,N>
	{
		public:
		using BaseMatrix<KMat::GenMatrix,T,M,N>::operator=;//Unhide  assignment operator
	};
	//========================================Square matrices====================================
	//Transpose for square matrices
	template <typename A,unsigned S>
	GenMatrix<A,S,S> & transpose_square_matrix(GenMatrix<A,S,S> & athis)
	{

		for (unsigned i=0;i<S;i++)
		{
			for (unsigned j=0;j<i;j++)
			{
				A tempdata=athis.h->data[i][j];
				athis.h->data[i][j]=athis.h->data[j][i];
				athis.h->data[j][i]=tempdata;
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
		A determ=athis.h->data[0][0]*athis.h->data[1][1]-athis.h->data[0][1]*athis.h->data[1][0];
		//std::cout<<"Det:"<<determ<<std::endl;
		//std::cout<<"Eps:"<<std::numeric_limits<T>::epsilon()<<std::endl;
		if (determ > std::numeric_limits<A>::epsilon()&& determ!=(A)0)//can invert
		{
			A temp1=athis.h->data[0][0];
			athis.h->data[0][0]=athis.h->data[1][1]/determ;
			athis.h->data[1][1]=temp1/determ;

			A temp2=athis.h->data[0][1];
			athis.h->data[0][1]=-athis.h->data[1][0]/determ;
			athis.h->data[1][0]=-temp2/determ;
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
		A m1=athis.h->data[0][0]*(athis.h->data[1][1]*athis.h->data[2][2]-athis.h->data[1][2]*athis.h->data[2][1]);
		//Minor 2: based on 1,2
		A m2=athis.h->data[0][1]*(athis.h->data[1][0]*athis.h->data[2][2]-athis.h->data[2][0]*athis.h->data[1][2]);
		//Minor 2: based on 1,3
		A m3=athis.h->data[0][2]*(athis.h->data[1][0]*athis.h->data[2][1]-athis.h->data[2][0]*athis.h->data[1][1]);
		A determ=m1-m2+m3;

		//std::cout<<"Det:"<<determ<<std::endl;
		//std::cout<<"Eps:"<<std::numeric_limits<A>::epsilon()<<std::endl;
		if (determ > std::numeric_limits<A>::epsilon()&& determ!=(A)0)//can invert
		{
			GenMatrix<A,3,3> t=athis.clone();

			athis.h->data[0][0]=(t.h->data[1][1]*t.h->data[2][2]-t.h->data[1][2]*t.h->data[2][1])/determ;
			athis.h->data[0][1]=(t.h->data[0][2]*t.h->data[2][1]-t.h->data[0][1]*t.h->data[2][2])/determ;
			athis.h->data[0][2]=(t.h->data[0][1]*t.h->data[1][2]-t.h->data[0][2]*t.h->data[1][1])/determ;

			athis.h->data[1][0]=(t.h->data[1][2]*t.h->data[2][0]-t.h->data[1][0]*t.h->data[2][2])/determ;
			athis.h->data[1][1]=(t.h->data[0][0]*t.h->data[2][2]-t.h->data[0][2]*t.h->data[2][0])/determ;
			athis.h->data[1][2]=(t.h->data[0][2]*t.h->data[1][0]-t.h->data[0][0]*t.h->data[1][2])/determ;

			athis.h->data[2][0]=(t.h->data[1][0]*t.h->data[2][1]-t.h->data[1][1]*t.h->data[2][0])/determ;
			athis.h->data[2][1]=(t.h->data[0][1]*t.h->data[2][0]-t.h->data[0][0]*t.h->data[2][1])/determ;
			athis.h->data[2][2]=(t.h->data[0][0]*t.h->data[1][1]-t.h->data[0][1]*t.h->data[1][0])/determ;

			return athis;

		}
		std::string d("KMat::invert_square_matrixGenMatrix<A,3,3>");
		throw SingularMatrixInvertionException(d);
	};

	//Partial specialization for square matrices
	template<typename T,unsigned S> class GenMatrix<T,S,S> : public BaseMatrix<GenMatrix,T,S,S>
	{
			friend GenMatrix<T,S,S> & transpose_square_matrix<>(GenMatrix<T,S,S> & athis);
			friend GenMatrix<T,S,S> & invert_square_matrix<>(GenMatrix<T,S,S> &athis);
		public:
			using BaseMatrix<KMat::GenMatrix,T,S,S>::operator=;
			GenMatrix<T,S,S> & transp()
			{
				return transpose_square_matrix(*this) ;
			};//Override for square
			GenMatrix<T,S,S> & invert()
			{
				return invert_square_matrix(*this) ;
			};//Override for square
	};

	template <typename T, unsigned S> class GenMatrix<T,S,1> : public BaseMatrix<GenMatrix,T,S,1>
	{
		//Add single dimentionall acess operator
		public:
		using BaseMatrix<KMat::GenMatrix ,T,S,1>::read;
		using BaseMatrix<KMat::GenMatrix ,T,S,1>::operator();
		using BaseMatrix<KMat::GenMatrix ,T,S,1>::operator=;

		COWRef<T,GenMatrix<T,S,1> > operator() (unsigned i)
		{
			return COWRef<T,GenMatrix<T,S,1> > ( static_cast< GenMatrix<T,S,1>  &> (*this),i,0);
		};
		const T& operator() (unsigned i) const
		{
			return read(i,0);
		};
	};
	//Vector of size 2
	template <typename T> class GenMatrix<T,2,1> : public BaseMatrix<GenMatrix,T,2,1>
	{
		//Add single dimentionall acess operator
		public:
			GenMatrix<T,2,1>() :BaseMatrix<KMat::GenMatrix,T,2,1>(),x(*this,0,0),y(*this,1,0) {};

			GenMatrix<T,2,1>(T ax,T ay) :BaseMatrix<KMat::GenMatrix,T,2,1>(),x(*this,0,0),y(*this,1,0)
			{
				this->makeExclusive();
				this->h->data[0][0]=ax;
				this->h->data[1][0]=ay;
			};
			//To avoid modifying .x,.y
			GenMatrix<T,2,1> (BaseMatrix<KMat::GenMatrix ,T,2,1> const& o):
			BaseMatrix<KMat::GenMatrix ,T,2,1>(o),x(*this,0,0),y(*this,1,0)
			{};
			GenMatrix<T,2,1> (GenMatrix<T,2,1> const& o):
			BaseMatrix<KMat::GenMatrix ,T,2,1>(o),x(*this,0,0),y(*this,1,0)
			{};
		using BaseMatrix<KMat::GenMatrix ,T,2,1>::read;
		using BaseMatrix<KMat::GenMatrix ,T,2,1>::operator();
		using BaseMatrix<KMat::GenMatrix ,T,2,1>::operator=;


		GenMatrix<T,2,1> & operator=( GenMatrix<T,2,1> const & o)
		{
			return BaseMatrix<KMat::GenMatrix ,T,2,1>::operator=(o);
		}
		COWRef<T,GenMatrix<T,2,1> > operator() (unsigned i)
		{
			return COWRef<T,GenMatrix<T,2,1> > ( static_cast< GenMatrix<T,2,1>  &> (*this),i,0);
		};
		const T& operator() (unsigned i) const
		{
			return read(i,0);
		};
		COWRef<T,GenMatrix<T,2,1> > x,y;

	};

	// Affine	transform matrix, using homogenous coordinates!!
	/* Internal represantation is one S-1 X S-1 matrix A
	   * and one S-1 x 1 B (homogenous coefs)
	   * | A B|
	   * | 0 1|
	   */
	class transformations;//Forward declaration
	template<typename T,unsigned S> class ATMatrix
	{

			friend class transformations;
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
				if (AisZero==true)
				{
					AisZero=rop.AisZero;
					AisIdentity=rop.AisIdentity;
				}
				else if (AisIdentity==true)
				{
					AisZero=false;
					AisIdentity=rop.AisZero;//If otherside is zero, remains Id
				}
				B.add(rop.B);
				if (BisZero==true)
					BisZero=rop.BisZero;
				return *this;
			};

			ATMatrix<T,S> & sub ( ATMatrix<T,S> const& rop)
			{
				A.sub(rop.A);
				if (AisZero==true)
				{
					AisZero=rop.AisZero;
					AisIdentity=rop.AisIdentity;
				}
				else if (AisIdentity==true)
				{
					AisZero=false;
					AisIdentity=rop.AisZero;//If otherside is zero, remains Id
				}
				B.sub(rop.B);
				if (BisZero==true)
					BisZero=rop.BisZero;
				return *this;
			};

			//Multiplication, optimized for HTMatrices!!
			ATMatrix<T,S> & mult (ATMatrix<T,S> const& rop)
			{
				GenMatrix<T,S-1,1> axd=A.mult(rop.B);
				//std::cout<<"WTF!"<<std::endl;
				if (AisIdentity==true)//Mult with id , just copy
				{
					A.copyFrom(rop.A);
					AisZero=rop.AisZero;
					AisIdentity=rop.AisIdentity;
				}
				else if (AisZero==false)//Not id and not zero, just do the math
				{
					A.mult(rop.A);
					AisZero=rop.AisZero;
					AisIdentity=rop.AisIdentity;
				}

				B.add(axd);
				if (BisZero==true)
					BisZero=rop.BisZero;

				return *this;

			}

			//Project a point !
			GenMatrix<T,S-1,1> transform(GenMatrix<T,S-1,1> const & c,T hom=1) const
			{
				GenMatrix<T,S-1,1> nc;
				if (AisZero==true)//A matrix zero, dont try to do the math:p
				{
					nc.zero();//Result is definately zero
				}
				else if ( AisIdentity==true)//A matrix id, no change in c
				{
					nc=c;//Just clone coords
				}
				else//Just do the math :p
				{
					nc=A.mult(c);
				}
				if (BisZero==false)//do some more math
				{
					GenMatrix<T,S-1,1> t=B;
					t.scalar_mult(hom);
					nc.add(t);
				}
				return nc;
			}

			ATMatrix<T,S> & invert()
			{
				if (AisIdentity!=true)
					A.invert();
				if (BisZero==false)
				{
					if (AisIdentity==false&&AisZero==false)//Just do the math
					{
						GenMatrix<T,S-1,1> newb=A.mult(B);
						newb.scalar_mult(-1);
						B=newb;
					}
					else if (AisZero==true)//Result is def zero
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
				for (unsigned i=1;i<S;i++)
					if (B(i,1)!=0)
					{
						BisZero=false;
						break;
					}

				AisZero=true;
				AisIdentity=true;
				for (unsigned i=1;i<S;i++)
				{
					if (A(i,i)!=1)
						AisIdentity=false;
					for (unsigned j=1;j<S;j++)
					{
						if (A(i,j)!=0)
							AisZero=false;
					}
					if (AisIdentity==false&&AisZero==false)
						break;
				}
				return this;
			}
            COWRef<T, ATMatrix<T,S> > operator() (unsigned i,unsigned j)
			{
			    return  COWRef<T, ATMatrix<T,S> >(*this,i,j);
			}

			const T& operator() (unsigned i,unsigned j) const { return read(i,j);}

			T& get(unsigned i,unsigned j)
			{
				if(j==S)
                    return B(i,0);
                else
                    return A(i,j);
			}
			const T& read(unsigned i,unsigned j) const
			{
			    if(j==S)
                    return B(i,0);
                else
                    return A(i,j);
			};

	};
	//Typedef vector type
	template<typename T,unsigned S> struct Vector
	{

		typedef  GenMatrix<T,S,1> type;
	};
	class transformations
	{
		public:
			static const double PI=3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811;
			template<typename T> static void rotate(ATMatrix<T,3> & m, T theta)
			{
				m.identity();
				m.AisIdentity=false;
				m.A(0,0)=cos(theta);
				m.A(0,1)=-sin(theta);
				m.A(1,0)=sin(theta);
				m.A(1,1)=cos(theta);
			}
			template<typename T> static void shearX(ATMatrix<T,3> & m, T factor)
			{
				m.identity();
				m.AisIdentity=false;
				m.A(0,1)=factor;
			};
			template<typename T> static void shearY(ATMatrix<T,3> & m, T factor)
			{
				m.identity();
				m.AisIdentity=false;
				m.A(1,0)=factor;
			};

			template<typename T,unsigned S> static void translate(ATMatrix<T,S> & m, GenMatrix<T,S-1,1> const& t)
			{

				m.identity();
				m.AisIdentity=true;
				m.AisZero=false;
				m.BisZero=false;
				m.B.copyFrom(t);
			}
			template<typename T, unsigned S> static void scale(ATMatrix<T,S> &m, GenMatrix<T,S-1,1> const & t)
			{
				m.identity();
				m.AisIdentity=false;
				for (unsigned i=1;i<S-1;i++)
					m.A(i,i)=t(i);

			}
			template<typename T> static void rotateZ(ATMatrix<T,4> & m, T theta)
			{
				m.identity();
				m.AisIdentity=false;
				m.A(0,0)=cos(theta);
				m.A(0,1)=-sin(theta);
				m.A(1,0)=sin(theta);
				m.A(1,1)=cos(theta);

			}
			template<typename T> static void rotateY(ATMatrix<T,4> & m, T theta)
			{
				m.identity();
				m.AisIdentity=false;
				m.A(0,0)=cos(theta);
				m.A(0,2)=sin(theta);
				m.A(2,0)=-sin(theta);
				m.A(2,2)=cos(theta);


			}
			template<typename T> static void rotateX(ATMatrix<T,4> & m, T theta)
			{
				m.identity();
				m.AisIdentity=false;

				m.A(1,1)=cos(theta);
				m.A(1,2)=-sin(theta);
				m.A(2,1)=sin(theta);
				m.A(2,2)=cos(theta);
			}
	};

};

//Short Definitions :)
typedef KMat::Vector<float,2>::type KVecFloat2;
typedef KMat::Vector<float,3>::type KVecFloat3;
typedef KMat::Vector<int,2>::type KVecInt2;
typedef KMat::Vector<int,3>::type KVecInt3;


#endif
