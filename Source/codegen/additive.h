//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  additive.h
//
//  Code generation for function 'additive'
//


#ifndef ADDITIVE_H
#define ADDITIVE_H

// Include files
#include <cstddef>
#include <cstdlib>
#include "rtwtypes.h"
#include "DDSPSynth_types.h"

// Function Declarations
extern void additive(int n_samples, double sample_rate, const double amplitudes
                     [4096], double harmonic_distribution[50], const double f0
                     [4096], const double prev_phases[50], double shift, double
                     stretch, double audio_data[], int audio_size[1], double
                     last_phases[50]);

#endif

// End of code generation (additive.h)
