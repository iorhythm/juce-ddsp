//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  linspace.h
//
//  Code generation for function 'linspace'
#pragma once

#include <cstddef>
#include <cstdlib>

#include "rt_nonfinite.h"
#include "coder_array.h"


namespace coder
{
	void linspace( double d2, double n, ::coder::array<double, 2U>& y );
}