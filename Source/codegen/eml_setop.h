//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  eml_setop.h
//
//  Code generation for function 'eml_setop'
//


#ifndef EML_SETOP_H
#define EML_SETOP_H

// Include files
#include <cstddef>
#include <cstdlib>
#include "rtwtypes.h"
#include "omp.h"
#include "DDSPSynth_types.h"
#define MAX_THREADS                    omp_get_max_threads()

// Function Declarations
extern void do_vectors(const coder::array<int, 1U> &a, const coder::array<int,
  1U> &b, coder::array<int, 1U> &c, coder::array<int, 1U> &ia, coder::array<int,
  1U> &ib);

#endif

// End of code generation (eml_setop.h)
