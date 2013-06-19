#include <queue>
#include <iostream>

template <class T,class Y = std::vector<T>, class U=std::less<T> >
class reservable_priority_queue: public std::priority_queue<T, Y,U>
{
public:
    typedef typename std::priority_queue<T,Y,U>::size_type size_type;
    reservable_priority_queue(size_type capacity = 0) { reserve(capacity); };
    void reserve(size_type capacity) { this->c.reserve(capacity); } 
    size_type capacity() const { return this->c.capacity(); } 
};




