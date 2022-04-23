//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  compute_loudness.h
//
//  Code generation for function 'compute_loudness'
//
#pragma once

#include "FFTImplementationCallback.h"
#include "gencoswin.h"
#include "iseven.h"
#include "weightingFilter.h"


static double rt_hypotd_snf( double u0, double u1 )
{
	double a = std::abs( u0 );
	double y = std::abs( u1 );

	if(a < y)
	{
		a /= y;
		y *= std::sqrt( a * a + 1.0 );
	}
	else if(a > y)
	{
		y /= a;
		y = a * std::sqrt( y * y + 1.0 );
	}
	else
	{
		if(!rtIsNaN( y ))
		{
			y = a * 1.4142135623730951;
		}
	}

	return y;
}

//double compute_loudness(double n_samples, const double audio[4096], double sample_rate);
