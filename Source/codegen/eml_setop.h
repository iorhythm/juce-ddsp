//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  eml_setop.h
//
//  Code generation for function 'eml_setop'
#pragma once

#include <cstddef>
#include <cstdlib>

#include "rt_nonfinite.h"
#include "coder_array.h"


namespace coder
{
	void do_vectors( const array<int, 1U>& a, const array<int, 1U>& b, 
	array<int, 1U>& c, array<int, 1U>& ia, array<int, 1U>& ib );
}
