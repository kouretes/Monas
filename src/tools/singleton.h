#ifndef _singleton_h_
#define _singleton_h_ 1

template< class T>
class Singleton : public T {

    public:

        static T* Instance() {
            static T t;
            return &t;
        }

    private:

        Singleton();
        Singleton(const Singleton&);
        Singleton& operator=(const Singleton&);
        ~Singleton();

};

#endif // _singleton_h_
