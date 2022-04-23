//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  getPitch.h
//
//  Code generation for function 'getPitch'
#pragma once

#include <cstring>

#include "DDSPSynth_initialize.h"
#include "DDSPSynth_rtwutil.h"
#include "fft.h"
#include "findpeaks.h"
#include "gencoswin.h"
#include "iseven.h"
#include "linspace.h"


// Function Declarations
extern double getPitch(double n_samples, const double input[4096], double fs);
