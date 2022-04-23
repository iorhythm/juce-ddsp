//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  additive.h
//
//  Code generation for function 'additive'
//
#pragma once

#include <cstring>

#include "DDSPSynth_initialize.h"
#include "DDSPSynth_rtwutil.h"
#include "linspace.h"
#include "coder_array.h"


// Function Declarations
extern void additive(double n_samples, double sample_rate, const double
                     amplitudes[4096], double n_harmonics, double
                     harmonic_distribution[100], const double f0[4096], const
                     double prev_phases[100], double shift, double stretch,
                     double audio[4096], double last_phases[100]);

