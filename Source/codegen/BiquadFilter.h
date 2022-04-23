//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  BiquadFilter.h
//
//  Code generation for function 'BiquadFilter'
//
#pragma once

#include <cstddef>
#include <cstdlib>

#include "rt_nonfinite.h"


// Type Definitions
struct dsp_BiquadFilter_0
{
	int S0_isInitialized;
	double W0_FILT_STATES[ 6 ];
	int W1_PreviousNumChannels;
	double P0_ICRTP;
};

namespace coder::dspcodegen
{
	class BiquadFilter
	{
	public:
		BiquadFilter();
		~BiquadFilter();

		BiquadFilter* init();
		void setup();
		void reset();
		void step( const double varargin_1[ 2048 ], const double varargin_2[ 9 ], const
			double varargin_3[ 6 ], const double varargin_4[ 4 ], double
			varargout_1[ 2048 ] );
		void setupAndReset();
		void matlabCodegenDestructor();
		void release();

		boolean_T matlabCodegenIsDeleted;

	private:
		int isInitialized;
		boolean_T isSetupComplete;
		dsp_BiquadFilter_0 cSFunObject;
	};
}
