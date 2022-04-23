//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  DDSPSynth_initialize.cpp
//
//  Code generation for function 'DDSPSynth_initialize'
#include "DDSPSynth_initialize.h"


// Function Definitions
void DDSPSynth_initialize()
{
  eml_rand_mt19937ar_stateful_init();
  isInitialized_DDSPSynth = true;
}

// End of code generation (DDSPSynth_initialize.cpp)
