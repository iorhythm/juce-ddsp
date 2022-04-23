//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  ifft.h
//
//  Code generation for function 'ifft'
//
#pragma once

#include <cstring>

#include "FFTImplementationCallback.h"


// Function Declarations
namespace coder
{
	void ifft( const creal_T x[ 65 ], creal_T y[ 65 ] );
	void ifft( const ::coder::array<creal_T, 1U>& x, double varargin_1, ::coder::
		array<creal_T, 1U>& y );
}
