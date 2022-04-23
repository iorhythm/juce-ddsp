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

#include "BiquadFilter.h"
#include "DDSPSynth_initialize.h"
#include "DDSPSynth_rtwutil.h"
#include "FFTImplementationCallback.h"
#include "gencoswin.h"
#include "iseven.h"
#include "weightingFilter.h"


// Function Declarations
extern double compute_loudness(double n_samples, const double audio[4096], double sample_rate);
