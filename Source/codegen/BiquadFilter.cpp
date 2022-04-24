//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  BiquadFilter.cpp
//
//  Code generation for function 'BiquadFilter'
//
#include "BiquadFilter.h"


// Function Definitions
namespace coder::dspcodegen
{
	BiquadFilter::BiquadFilter()
	{
		matlabCodegenIsDeleted = true;
	}

	BiquadFilter::~BiquadFilter()
	{
		matlabCodegenDestructor();
	}

	BiquadFilter* BiquadFilter::init()
	{
		auto* obj = this;
		obj->isInitialized = 0;

		// System object Constructor function: dsp.BiquadFilter
		obj->cSFunObject.P0_ICRTP = 0.0;
		obj->matlabCodegenIsDeleted = false;

		return obj;
	}

	void BiquadFilter::setupAndReset()
	{
		setup();

		// System object Initialization function: dsp.BiquadFilter
		for(int i = 0; i < 6; i++)
			cSFunObject.W0_FILT_STATES[ i ] = cSFunObject.P0_ICRTP;
	}

	void BiquadFilter::matlabCodegenDestructor()
	{
		if(!matlabCodegenIsDeleted)
		{
			matlabCodegenIsDeleted = true;
			release();
		}
	}

	void BiquadFilter::release()
	{
		if(isInitialized == 1)
			isInitialized = 2;
	}

	void BiquadFilter::reset()
	{
		if(isInitialized == 1)
		{
			// System object Initialization function: dsp.BiquadFilter
			for(int i = 0; i < 6; i++)
				cSFunObject.W0_FILT_STATES[ i ] = cSFunObject.P0_ICRTP;
		}
	}

	void BiquadFilter::setup()
	{
		isSetupComplete = false;
		isInitialized = 1;
		isSetupComplete = true;
	}

	void BiquadFilter::step( const double varargin_1[ 2048 ], 
							 const double varargin_2[ 9 ], 
							 const double varargin_3[ 6 ], 
							 const double varargin_4[ 4 ], 
							 double varargout_1[ 2048 ] )
	{
		if(isInitialized != 1)
			setupAndReset();

		auto* obj = &cSFunObject;

		// System object Outputs function: dsp.BiquadFilter
		int ioIdx {};
		auto d = varargin_4[ 0 ];
		auto d1 = varargin_4[ 1 ];
		auto d2 = varargin_4[ 2 ];
		auto d3 = varargin_4[ 3 ];

		auto d4 = varargin_2[ 0 ];
		auto d5 = varargin_2[ 1 ];
		auto d6 = varargin_2[ 2 ];
		auto d7 = varargin_2[ 3 ];
		auto d8 = varargin_2[ 4 ];
		auto d9 = varargin_2[ 5 ];
		auto d10 = varargin_2[ 6 ];
		auto d11 = varargin_2[ 7 ];
		auto d12 = varargin_2[ 8 ];
		
		auto d13 = varargin_3[ 0 ];
		auto d14 = varargin_3[ 1 ];
		auto d15 = varargin_3[ 2 ];
		auto d16 = varargin_3[ 3 ];
		auto d17 = varargin_3[ 4 ];
		auto d18 = varargin_3[ 5 ];

		for(int i = 0; i < 2048; i++)
		{
			double numAccum = obj->W0_FILT_STATES[ 0 ];
			double stageIn = d * varargin_1[ ioIdx ];
			double stageOut = numAccum + d4 * stageIn;

			numAccum = obj->W0_FILT_STATES[ 1 ];
			obj->W0_FILT_STATES[ 0 ] = ( numAccum + d5 * stageIn ) - d13 * stageOut;
			obj->W0_FILT_STATES[ 1 ] = d6 * stageIn - d14 * stageOut;
			stageIn = d1 * stageOut;
			numAccum = obj->W0_FILT_STATES[ 2 ];
			stageOut = numAccum + d7 * stageIn;
			numAccum = obj->W0_FILT_STATES[ 3 ];
			obj->W0_FILT_STATES[ 2 ] = ( numAccum + d8 * stageIn ) - d15 * stageOut;
			obj->W0_FILT_STATES[ 3 ] = d9 * stageIn - d16 * stageOut;
			stageIn = d2 * stageOut;
			numAccum = obj->W0_FILT_STATES[ 4 ];
			stageOut = numAccum + d10 * stageIn;
			numAccum = obj->W0_FILT_STATES[ 5 ];
			obj->W0_FILT_STATES[ 4 ] = ( numAccum + d11 * stageIn ) - d17 * stageOut;
			obj->W0_FILT_STATES[ 5 ] = d12 * stageIn - d18 * stageOut;
			varargout_1[ ioIdx ] = d3 * stageOut;
			ioIdx++;
		}
	}
}
