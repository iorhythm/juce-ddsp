//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  sortIdx.h
//
//  Code generation for function 'sortIdx'
//
#pragma once

#include <cstddef>
#include <cstdlib>

#include "rt_nonfinite.h"


namespace coder::internal
{
	void merge( int idx[ 6 ], double x[ 6 ], int offset, int np, int nq, int iwork[ 6 ],
		double xwork[ 6 ] );
}
