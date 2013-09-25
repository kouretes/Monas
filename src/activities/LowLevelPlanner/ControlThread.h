//
//  ControlThread.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
#include "core/elements/math/KMat.hpp"
#include <vector>
#include "RobotParameters.h"
#include <iostream>
#ifndef __Kouretes_Walk_Engine__ControlThread__
#define __Kouretes_Walk_Engine__ControlThread__

#define PredictionHorizon 51

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

class LIPMPreviewController
{
	private:
		KMath::KMat::GenMatrix<float, 3, 3> Aobs, Ad;
		KMath::KMat::GenMatrix<float, 3, 1> Bd, Cd, L, State, temp;
		KMath::KMat::GenMatrix<float, 1, 3> Gx;
		KMath::KMat::GenMatrix<float, 50, 1> Gd;
		KMath::KMat::GenMatrix<float, 51, 1> ZMPReference;
		KMath::KMat::GenMatrix<float, 51, 1> ZMPReference2;
		float Gi, Integrationfb, Statefb, Predictionfb, ZMPMeasured, u;
		int counter;
		RobotParameters OurRobot;
	public:
		LIPMPreviewController(RobotParameters robot);

		void LIPMComPredictor(CircularBuffer<float> & ZmpBuffer);

		float Com;

};

#endif /* defined(__Kouretes_Walk_Engine__ControlThread__) */
