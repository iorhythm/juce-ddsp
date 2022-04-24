/*
  ==============================================================================

	DDSPCommon.h
	Created: 24 Apr 2022 8:46:42pm
	Author:  St

  ==============================================================================
*/
#pragma once

#include <cstddef>
#include <cstdlib>

#include <JuceHeader.h>

#include "codegen/linspace.h"
#include "codegen/coder_array.h"
#include "codegen/rt_nonfinite.h"
#include "codegen/gencoswin.h"


namespace DDSP
{
	static constexpr auto stateSize{ 625 };
	static constexpr auto maxAmplitudes{ 4096 };
	static constexpr auto maxMagnitudes{ 65 };
	static constexpr auto maxHarmonics{ 100 };

	using FVOP = juce::FloatVectorOperations;

	using Amplitudes = std::array< double, maxAmplitudes >;
	using Magnitudes = std::array< double, maxMagnitudes >;
	using Harmonics = std::array< double, maxHarmonics >;

	double sigmoid( const double x );

	double rt_hypotd_snf( const double, const double );
	
	double rt_powd_snf( const double, const double );

	double rt_roundd_snf( const double );

	int div_s32_floor( const int numerator, const int denominator );
}