/*
  ==============================================================================

	DDSPSynth.h
	Created: 23 Apr 2022 9:24:53pm
	Author:  St

  ==============================================================================
*/
#pragma once

#include <cstddef>
#include <cstdlib>

#include "rt_nonfinite.h"
#include "linspace.h"
#include "fft.h"
#include "fftshift.h"
#include "ifft.h"
#include "../codegen/ColoredNoise.h"
#include "../codegen/iseven.h"
#include "../codegen/weightingFilter.h"
#include "../codegen/findpeaks.h"
#include "compute_loudness.h"

struct DDSPSynth
{
	static constexpr size_t stateSize { 625 };

	void initialize();
	void terminate();

	void additive( double n_samples, double sample_rate, const double
		amplitudes[ 4096 ], double n_harmonics, double
		harmonic_distribution[ 100 ], const double f0[ 4096 ], const
		double prev_phases[ 100 ], double shift, double stretch,
		double audio[ 4096 ], double last_phases[ 100 ] );

	void subtractive( double n_samples, const double magnitudes[ 65 ], double color, double initial_bias, double out[ 4096 ] );

	int div_s32_floor( int numerator, int denominator );

	double rt_powd_snf( double u0, double u1 );

	double rt_roundd_snf( double u );

	void scale_f0( const double f0_in[ 100 ], boolean_T to_midi, double f0_scaled[ 100 ] );

	double getPitch( double n_samples, const double input[ 4096 ], double fs );

	double compute_loudness( double n_samples, const double audio[ 4096 ], double sample_rate );

	void eml_rand_mt19937ar_stateful_init();

	unsigned state[ 625 ] {};
	boolean_T isInitialized{ false };
};