//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  WeightingFilter.h
//
//  Code generation for function 'WeightingFilter'
//
#pragma once

#include <cstring>

#include "BiquadFilter.h"
#include "ProcessConstructorArguments.h"
#include "sort.h"


struct cell_wrap_3
{
	unsigned int f1[ 8 ];
};

namespace coder
{
	class WeightingFilter
	{
	public:
		WeightingFilter();
		~WeightingFilter();

		WeightingFilter* init( double varargin_2 );
		void setupAndReset();
		void setup();
		void release();
		void releaseWrapper();
		
		double get_SampleRate() const;
		void set_SampleRate( double value );
		
		void step( const double varargin_1[ 2048 ], double varargout_1[ 2048 ] );

		boolean_T isLockedAndNotReleased() const;
		void checkTunableProps();
		void matlabCodegenDestructor();

		boolean_T matlabCodegenIsDeleted;
		boolean_T tunablePropsChanged;
		dspcodegen::BiquadFilter pFilter;

	protected:
		void designFilter();

		double pSampleRateDialog;
		double pNumMatrix[ 9 ];
		double pDenMatrix[ 6 ];
		double pScaleValues[ 4 ];
		double pNumChannels;

	private:
		int isInitialized;
		boolean_T isSetupComplete;
		cell_wrap_3 inputVarSize[ 1 ];
	};
}
