//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  fftshift.cpp
//
//  Code generation for function 'fftshift'
#include "fftshift.h"


namespace coder
{
	void fftshift( double x[ 65 ] )
	{
		int i2 {};

		for(int i = 0; i < 1; ++i)
		{
			int i1 = i2;

			i2 += 64;
			
			for(int j {}; j < 1; ++j)
			{
				++i1;
				++i2;

				int ia = i1 - 1;
				int ib = i1 + 31;

				auto xtmp = x[ i1 + 31 ];

				for(int k {}; k < 32; ++k)
				{
					x[ ib ] = x[ ia ];
					x[ ia ] = x[ ib + 1 ];
					++ia;
					++ib;
				}

				x[ ib ] = xtmp;
			}
		}
	}
}
