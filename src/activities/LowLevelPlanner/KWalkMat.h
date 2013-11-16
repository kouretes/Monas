//  KWalkMath.h
//  Kouretes Walk Engine
//
//  Created by Stelios Piperakis on 8/14/13.
//  Copyright (c) 2013 SP. All rights reserved.
//
/* Mathematics Used in Walking */
/* More MATH will be added in the way */
#ifndef _KWALKMAT_
#define _KWALKMAT_
#include <math.h>
class KWalkMat
{

	public:

		void LinearInterpolation(float *buffer, int &counter, float start, float end, float Ts, float T)
		{
			//std::cout << counter<< "Ts " << Ts << " T " << T << std::endl;
			for (float t = 0; t <= 1; t += Ts / T, counter++){
				buffer[counter] = (1 - t) * start + t * end; //    vec.push_back();

			//	std::cout << counter << " "<< buffer[counter] <<  std::endl;
			}
		}

		void CubicBezierInterpolation(float *buffer, int &counter, float start, float end, float Ts, float Tss)
		{ /**
		 float H=0.020 for max stepping height 0.015m
		 **/

			float H = 0.02, P0 = start, P1 = start + H, P2 = end + H, P3 = end;
			for (float t = 0; t <= 1; t = t + Ts / Tss, counter++)

				buffer[counter] = (pow(1 - t, 3) * P0 + 3 * pow(1 - t, 2) * t * P1 + 3 * (1 - t) * pow(t, 2) * P2 + pow(t, 3) * P3);

		}

		void trigInterpolation2(float *buffer, int &counter, float Ts, float Tss)
		{
			float s = 0.008/1.414;
			for (float t = 0; t <= Tss; t = t + Ts , counter++)
				buffer[counter]  =s*sqrt(1-cos((t*M_PI*2)/Tss));

		}

		void trigIntegInterpolation(float *buffer, int &counter, float start, float end, float Ts, float Tss)
		{
			float delta = (end-start)/Tss;

			for (float t = 0; t <= Tss; t = t + Ts , counter++)
				buffer[counter]  = start+delta*t-delta*sin((t*M_PI*2)/Tss)/((M_PI*2)/Tss);

		}
void CubicSplineInterpolation(float *buffer,int &counter,float p0,float p1,float p2, float p3,float p4,float Ts,float T)
        {
            float h=T/4;
            float t;
            KMath::KMat::GenMatrix<float,1,7> C;
            KMath::KMat::GenMatrix<float,7,1> D;
            KMath::KMat::GenMatrix<float,5,1> B;
            KMath::KMat::GenMatrix<float,5,5> A;
            B(0)=p0;
            B(1)=6*p1;
            B(2)=6*p2;
            B(3)=6*p3;
            B(4)=p4;

            B.scalar_mult((1/h)*(1/h)*(1/h));

            A.zero();
            A(0,0)=1;
            A(1,0)=1;
            A(1,1)=4;
            A(1,2)=1;
            A(2,1)=1;
            A(2,2)=4;
            A(2,3)=1;
            A(3,2)=1;
            A(3,3)=4;
            A(3,4)=1;
            A(4,4)=1;

            A.fast_invert();
            B=A*B;

            C.zero();
            C(0,0)=2*B(0)-B(1) ;
            C(0,1)=B(0);
            C(0,2)=B(1);
            C(0,3)=B(2);
            C(0,4)=B(3);
            C(0,5)=B(4);
            C(0,6)=2*B(4)-B(3);

           // std::cout<<"C"<<std::endl;
           // C.prettyPrint();
            for(float tdot=0;tdot<=T;tdot=tdot+Ts,counter++)
            {
                int i=0;
                for(int j=-1;j<=6;j++)
                {

                    t=tdot-j*h;

                    if (t<=-2*h)
                    {
                        D(i)=0;
                    }
                    else if ((t>-2*h) &&(t<=-h))
                    {
                        D(i)=(1.0/6.0)*(2*h+t)*(2*h+t)*(2*h+t);
                    }
                    else if((t>-h) && (t<=0))
                    {
                        D(i)=(2.0/3.0)*h*h*h-(1.0/2.0)*t*t*(2*h+t);
                    }
                    else if((t>0)&&(t<=h))
                    {
                        D(i)=(2.0/3.0)*h*h*h-(1.0/2.0)*t*t*(2*h-t);

                    }
                    else  if((t>h)&&(t<=2*h))
                    {
                        D(i)=(1.0/6.0)*(2*h-t)*(2*h-t)*(2*h-t);
                    }
                    else
                    {
                        D(i)=0;
                    }

                   i++;
                }

                buffer[counter]=C*D;
                //std::cout<<"buffer"<<std::endl;
                //std::cout<<C*D<<std::endl;
            }
        }

		void FifthOrderPolynomialInterpolation(float *buffer, int &counter, float Ts, float T)
		{
			float zi[2] = { 0, 0.0414 };
			float zf[2] = { 0.0414, 0 };
			float zdoti[2] = { 0, 0.0004 };
			float zddoti[2] = { 0, -0.00002 };
			float zdotf[2] = { 0.0004, 0 };
			float zddotf[2] = { -0.00002, 0 };
			float w1[2] = { 0, 0 };
			float w2[2] = { 0, 0 };
			float w3[2] = { 0, 0 };
			float w4[2] = { 0, 0 };
			float w5[2] = { 0, 0 };
			float w6[2] = { 0, 0 };
			for (unsigned j = 0; j < 2; j++)
			{
				w1[j] = zi[j];
				w2[j] = zdoti[j];
				w3[j] = zddoti[j] / 2;
				w4[j] = 10 * ((zf[j] - zi[j]) / (pow(T, 3)) - zdoti[j] / pow(T, 2) - zddoti[j] / (2 * T)) - 4 * ((zdotf[j] - zdoti[j]) / pow(T, 2) - zddoti[j] / T)
						+ (1 / 2) * ((zddotf[j] - zddoti[j]) / T);

				w5[j] = -15 * ((zf[j] - zi[j]) / pow(T, 4) - zdoti[j] / pow(T, 3) - zddoti[j] / (2 * pow(T, 2))) + 7 * ((zdotf[j] - zdoti[j]) / pow(T, 3) - zddoti[j] / pow(T, 2))
						- ((zddotf[j] - zddoti[j]) / pow(T, 2));
				w6[j] = 6 * ((zf[j] - zi[j]) / pow(T, 5) - zdoti[j] / pow(T, 4) - zddoti[j] / pow(T, 3)) - 3 * ((zdotf[j] - zdoti[j]) / pow(T, 4) - zddoti[j] / pow(T, 3))
						+ (1 / 2) * ((zddotf[j] - zddoti[j]) / pow(T, 3));
			}
			for (float t = 0; t <= T; t += Ts,counter++)
			{

				if (t < T / 2)
					buffer[counter]=(w6[1] * pow(t, 5) + w5[1] * pow(t, 4) + w4[1] * pow(t, 3) + w3[1] * pow(t, 2) + w2[1] * t + w1[1]);
				else
					buffer[counter]=(w6[2] * pow(t, 5) + w5[2] * pow(t, 4) + w4[2] * pow(t, 3) + w3[2] * pow(t, 2) + w2[2] * t + w1[2]);

			}

		}

		void LinearInterpolation(std::vector<float> &vec, float start, float end, float Ts, float T)
		{

			for (float t = 0; t <= 1; t += Ts / T)
			{
				vec.push_back((1 - t) * start + t * end);
			}

		}
		void LinearInterpolationINIT(std::vector<float> &vec, float start, float end, float Ts, float Tstep)
		{

			for (float t = 0; t <= 1; t += Ts / Tstep)
			{
				vec.push_back((1 - t) * start + t * end);
			}

		}
		void CubicBezierInterpolation(std::vector<float> &vec, float start, float end, float Ts, float Tss)
		{ /**
		 float H=0.0207 for max stepping height 0.020m
		 **/

			float H = 0.027, P0 = start, P1 = start + H, P2 = end + H, P3 = end;
			for (float t = 0; t <= 1; t = t + Ts / Tss)
			{
				vec.push_back(pow(1 - t, 3) * P0 + 3 * pow(1 - t, 2) * t * P1 + 3 * (1 - t) * pow(t, 2) * P2 + pow(t, 3) * P3);
			}

		}
		void FifthOrderPolynomialInterpolation(std::vector<float> &vec, float Ts, float T)
		{
			float zi[2] = { 0, 0.0414 };
			float zf[2] = { 0.0414, 0 };
			float zdoti[2] = { 0, 0.0004 };
			float zddoti[2] = { 0, -0.00002 };
			float zdotf[2] = { 0.0004, 0 };
			float zddotf[2] = { -0.00002, 0 };
			float w1[2] = { 0, 0 };
			float w2[2] = { 0, 0 };
			float w3[2] = { 0, 0 };
			float w4[2] = { 0, 0 };
			float w5[2] = { 0, 0 };
			float w6[2] = { 0, 0 };
			for (unsigned j = 0; j < 2; j++)
			{
				w1[j] = zi[j];
				w2[j] = zdoti[j];
				w3[j] = zddoti[j] / 2;
				w4[j] = 10 * ((zf[j] - zi[j]) / (pow(T, 3)) - zdoti[j] / pow(T, 2) - zddoti[j] / (2 * T)) - 4 * ((zdotf[j] - zdoti[j]) / pow(T, 2) - zddoti[j] / T)
						+ (1 / 2) * ((zddotf[j] - zddoti[j]) / T);

				w5[j] = -15 * ((zf[j] - zi[j]) / pow(T, 4) - zdoti[j] / pow(T, 3) - zddoti[j] / (2 * pow(T, 2))) + 7 * ((zdotf[j] - zdoti[j]) / pow(T, 3) - zddoti[j] / pow(T, 2))
						- ((zddotf[j] - zddoti[j]) / pow(T, 2));
				w6[j] = 6 * ((zf[j] - zi[j]) / pow(T, 5) - zdoti[j] / pow(T, 4) - zddoti[j] / pow(T, 3)) - 3 * ((zdotf[j] - zdoti[j]) / pow(T, 4) - zddoti[j] / pow(T, 3))
						+ (1 / 2) * ((zddotf[j] - zddoti[j]) / pow(T, 3));
			}
			for (float t = 0; t <= T; t += Ts)
			{

				if (t < T / 2)
					vec.push_back(w6[1] * pow(t, 5) + w5[1] * pow(t, 4) + w4[1] * pow(t, 3) + w3[1] * pow(t, 2) + w2[1] * t + w1[1]);
				else
					vec.push_back(w6[2] * pow(t, 5) + w5[2] * pow(t, 4) + w4[2] * pow(t, 3) + w3[2] * pow(t, 2) + w2[2] * t + w1[2]);

			}

		}
};
#endif
