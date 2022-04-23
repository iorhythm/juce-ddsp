//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  gencoswin.h
//
//  Code generation for function 'gencoswin'
#pragma once

#include <cstddef>
#include <cstdlib>

#include "coder_array.h"
#include "rt_nonfinite.h"


namespace coder
{
	void calc_window( double m, double n, ::coder::array<double, 1U>& w );
	void calc_window( double m, double n, double w_data[], int w_size[ 1 ] );
}