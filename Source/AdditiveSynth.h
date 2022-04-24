/*
  ==============================================================================

    AdditiveSynth.h
    Created: 24 Apr 2022 8:38:07pm
    Author:  St

  ==============================================================================
*/
#pragma once

#include "DDSPCommon.h"


namespace DDSP
{
	//________________________________________________________________________________
	class AdditiveSynth
	{
	public:
		void prepare( const dsp::ProcessSpec& spec );
		/**
		* INPUTS:
		* - sample_rate: int
		* - amplitudes: Frame-wise oscillator peak amplitude. Shape: [n_frames, 1]
		* - harmonic distribution: Frame-wise harmonic amplitude variations. Shape. [n_harmonics]
		* - f0: Frame-wise fundamental frequency in Hz. Shape: [n_frames, 1]
		* - shift: Shift fundamental frequency, range -12 (one octave down) to 12 (one octave up), 0: no effect
		* - stretch: Stretch/squeeze harmonic frequencies, range -1 (max squeezed) to 1 (max stretched), 0: no effect
		* RETURNS:
		* - Sample-wise audio signal
		*/
		void process( double n_samples,
			const Amplitudes& amplitudes,
			double n_harmonics,
			Harmonics& harmonicDistro,
			const Amplitudes& f0,
			const Harmonics& prev_phases,
			Amplitudes& audio,
			Harmonics& last_phases );

		coder::array<double, 2U> b_harmonic_frequencies;
		coder::array<double, 2U> f_ratios;
		coder::array<double, 2U> harmonic_amplitudes;
		coder::array<double, 2U> harmonicFreqs;
		coder::array<int, 2U> r1;
		coder::array<boolean_T, 2U> r;
		double amplitudes_data[ maxAmplitudes ]{};
		double f0_data[ maxAmplitudes ]{};
		double x_data[ maxAmplitudes ]{};

		double shift{};
		double stretch{};
		double sampleRate{ 44100.0 };
	};
}