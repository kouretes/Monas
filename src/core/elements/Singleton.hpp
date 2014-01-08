#ifndef SINGLETON_HPP
#define SINGLETON_HPP

template< class T>
class Singleton : public T
{

public:

	static T& Instance()
	{
		static T t;
		return t;
	}

private:

	Singleton();
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
	~Singleton();

};

#endif // _singleton_h_
