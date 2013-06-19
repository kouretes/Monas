template<typename T> class K3DArray{
private:
	T *myArray;
	int x,y,z;
public:
	K3DArray(){
		myArray = NULL;	
	}
	~K3DArray(){
		if( myArray != NULL){
			delete myArray;
		}
	}
	void setup(int i, int j, int k){
		x = i;
		y = j;
		z = k;
		myArray = new T[i*j*k];
	}
	void init(){
		memset(myArray,0,x*y*z*sizeof(T));
	}
	inline T getElement(int i, int j, int k){
		return myArray[i + j*x + k*x*y];	
	}
	inline void setElement(int i, int j, int k, T value){
		myArray[i + j*x + k*x*y] = value;	
	}
};
