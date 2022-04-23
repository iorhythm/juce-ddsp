//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  ColoredNoise.h
//
//  Code generation for function 'ColoredNoise'
//
#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "randn.h"
#include "rt_nonfinite.h"


namespace coder::dsp
{
	//________________________________________________________________________________
	class ColoredNoise
	{
	public:
		ColoredNoise* init();
		void step( double varargout_1[ 4096 ], unsigned state[ 625 ] );
		void setupAndReset();
		void setup();
		void release();
	private:
		int isInitialized;
		boolean_T isSetupComplete;
	};

	//________________________________________________________________________________
	class b_ColoredNoise
	{
	public:
		b_ColoredNoise* init();
		void step( double varargout_1[ 4096 ], unsigned state[ 625 ] );
		void setupAndReset();
		void setup();
		void release();
	protected:
		double pFilterStates[ 10 ];
	private:
		int isInitialized;
		boolean_T isSetupComplete;
	};

	//________________________________________________________________________________
	class c_ColoredNoise
	{
	public:
		c_ColoredNoise* init();
		void step( double varargout_1[ 4096 ], unsigned state[ 625 ] );
		void setupAndReset();
		void setup();
		void release();
	protected:
		double pFilterStates[ 255 ];
	private:
		int isInitialized;
		boolean_T isSetupComplete;
	};
}
