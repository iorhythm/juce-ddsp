//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  subtractive.h
//
//  Code generation for function 'subtractive'
#pragma once

#include "ColoredNoise.h"
#include "DDSPSynth_initialize.h"
#include "DDSPSynth_rtwutil.h"
#include "fft.h"
#include "fftshift.h"
#include "ifft.h"

// Function Declarations
extern void subtractive(double n_samples, const double magnitudes[65], double
  color, double initial_bias, double out[4096]);
