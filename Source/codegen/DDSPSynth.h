/*
  ==============================================================================

	DDSPSynth.h
	Created: 23 Apr 2022 9:24:53pm
	Author:  St

  ==============================================================================
*/
#pragma once

#include "../AdditiveSynth.h"
#include "../SubtractiveSynth.h"

#include "linspace.h"
#include "fft.h"
#include "ifft.h"
#include "../codegen/iseven.h"
#include "../codegen/weightingFilter.h"
#include "../codegen/findpeaks.h"


namespace DDSP
{
	//________________________________________________________________________________
	class DDSPSynth
	{
	public:
		void scale_f0( const double f0_in[ maxHarmonics ], boolean_T to_midi, double f0_scaled[ maxHarmonics ] );

		double getPitch( double n_samples, const double input[ maxAmplitudes ], double fs );

		double compute_loudness( double n_samples, const double audio[ maxAmplitudes ], double sample_rate );

		AdditiveSynth additiveSynth;
		SubtractiveSynth subtractiveSynth;

		boolean_T isInitialized{ false };
	};
}