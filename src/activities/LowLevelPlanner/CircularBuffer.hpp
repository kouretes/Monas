//
//  ControlThread.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//

#ifndef __CIRCULARBUFFER_HPP__
#define __CIRCULARBUFFER_HPP__


template<typename T>
class CircularBuffer
{
	private:
		int buffer_size; /* maximum number of elements           */
		int start; /* index of oldest element              */

		int count;/* index at which to write new element  */
		T *elems; /* vector of elements                   */

		int allcounter;
		/* This approach replaces the CircularBuffer 'end' field with the
		 'count' field and changes these functions: */
	public:
		CircularBuffer(int maxsize)
		{
			buffer_size = maxsize;
			start = 0;
			count = 0;
			allcounter = 0;
			elems = new T[maxsize];
		}
		~CircularBuffer()
		{
			delete elems;
		}

		int cbIsFull()
		{
			return count == buffer_size;
		}

		int cbIsEmpty()
		{
			return count == 0;
		}

		int getBufferSize()
		{
			return buffer_size;
		}

		void cbPush(T elem)
		{
			int end = (start + count) % buffer_size;
			elems[end] = elem;
			if (count == buffer_size)
				start = (start + 1) % buffer_size; /* full, overwrite */
			else
				++count;
		}

		void cbPush(T elem, bool print)
		{
			std::cout << " Inserted " << allcounter << std::endl;
			allcounter++;
			int end = (start + count) % buffer_size;
			elems[end] = elem;
			if (count == buffer_size)
				start = (start + 1) % buffer_size; /* full, overwrite */
			else
				++count;
		}

		void cbRead(T *elem)
		{
			*elem = elems[start];
			start = (start + 1) % buffer_size;
			--count;
		}

		T pop()
		{
			T temp = elems[start];
			start = (start + 1) % buffer_size;
			--count;
			return temp;
		}

		T operator [](int subscript) const
		{
			if (subscript >= 0 && subscript < count)
				return elems[(start + subscript) % buffer_size];
			else
				std::cerr << "Out of rangeeee" << std::endl;
			return 0;
		}

		int size()
		{
			return count;
		}
		;

};
#endif //__CIRCULARBUFFER_HPP__
