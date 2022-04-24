/*
  ==============================================================================

    AdditiveSynth.cpp
    Created: 24 Apr 2022 8:38:07pm
    Author:  St

  ==============================================================================
*/

#include "AdditiveSynth.h"

namespace DDSP
{
	void AdditiveSynth::prepare( const dsp::ProcessSpec& spec )
	{
		sampleRate = spec.sampleRate;
	}


	void AdditiveSynth::process(
		double n_samples,
		const Amplitudes& amplitudes,
		double n_harmonics,
		Harmonics& harmonicDistro,
		const Amplitudes& f0,
		const Harmonics& prev_phases,
		Amplitudes& audio,
		Harmonics& last_phases )
	{
		static constexpr auto& twopi{ juce::MathConstants< double >::twoPi };

		// Resize the input
		int loop_ub{ n_samples < 1.0 ? 0 : static_cast<int>( n_samples ) };
		int b_loop_ub{ n_samples < 1.0 ? 0 : static_cast<int>( n_samples ) };

		if(( b_loop_ub - 1 ) >= 0)
			std::memcpy( &f0_data[ 0 ], &f0[ 0 ], b_loop_ub * sizeof( double ) );

		// Incorporate shift parameter
		double b_sample_rate{};

		if(shift != 0.0)
		{
			const auto multiplier{ shift < 0.0 ? 0.5 : 1.0 };

			for(int i = 0; i < b_loop_ub; ++i)
			{
				b_sample_rate = f0[ i ];
				f0_data[ i ] = b_sample_rate + b_sample_rate / 12.0 * shift * multiplier;
			}
		}

		// Scale the amplitudes
		for(int i = 0; i < loop_ub; ++i)
		{
			x_data[ i ] = sigmoid( amplitudes[ i ] );
			amplitudes_data[ i ] = rt_powd_snf( x_data[ i ], 2.3025850929940459 );
			amplitudes_data[ i ] = 2.0 * amplitudes_data[ i ] + 1.0E-7;
		}

		for(int k = 0; k < maxHarmonics; ++k)
			harmonicDistro[ k ] = 2.0 * rt_powd_snf( sigmoid( harmonicDistro[ k ] ), 2.3025850929940459 ) + 1.0E-7;

		// Bandlimit the harmonic distribution
		// Calculate sample-wise oscillator frequencies of harmonics
		coder::linspace( n_harmonics, n_harmonics, f_ratios );

		int nx{};

		// Incorporate stretch parameter and shift back to f0
		if(stretch != 0.0)
		{
			const auto sz{ f_ratios.size( 0 ) * f_ratios.size( 1 ) };

			f_ratios.set_size( 1, f_ratios.size( 1 ) );

			const auto q{ stretch < 0.0 ? stretch * 0.5 : stretch };
			nx = sz - 1;

			for(int i = 0; i <= nx; ++i)
			{
				b_sample_rate = f_ratios[ i ];
				b_sample_rate = ( b_sample_rate + b_sample_rate * q ) - q;
				f_ratios[ i ] = b_sample_rate;
			}
		}

		harmonicFreqs.set_size( b_loop_ub, f_ratios.size( 1 ) );
		nx = f_ratios.size( 1 );

		for(int i = 0; i < nx; ++i)
		{
			for(int i1 = 0; i1 < b_loop_ub; i1++)
				harmonicFreqs[ i1 + harmonicFreqs.size( 0 ) * i ] = f0_data[ i1 ] * f_ratios[ i ];
		}

		// Set amplitudes for oscillators above nyquist to 0
		b_loop_ub = ( harmonicFreqs.size( 1 ) < 1 ) ? 0 : harmonicFreqs.size( 1 );

		b_sample_rate = sampleRate * 0.5;
		r.set_size( 1, b_loop_ub );

		for(int i = 0; i < b_loop_ub; ++i)
			r[ i ] = ( harmonicFreqs[ harmonicFreqs.size( 0 ) * i ] >= b_sample_rate );

		b_loop_ub = r.size( 1 ) - 1;
		nx = 0;

		for(int k = 0; k <= b_loop_ub; ++k)
		{
			if(r[ k ])
				++nx;
		}

		r1.set_size( 1, nx );
		nx = 0;

		for(int k = 0; k <= b_loop_ub; ++k)
		{
			if(r[ k ])
			{
				r1[ nx ] = k + 1;
				++nx;
			}
		}

		b_loop_ub = r1.size( 0 ) * r1.size( 1 );

		for(int i = 0; i < b_loop_ub; ++i)
			harmonicDistro[ r1[ i ] - 1 ] = 0.0;

		// Create harmonic amplitudes
		harmonic_amplitudes.set_size( loop_ub, maxHarmonics );
		b_loop_ub = loop_ub * maxHarmonics;

		for(int i = 0; i < b_loop_ub; ++i)
			harmonic_amplitudes[ i ] = 0.0;

		for(nx = 0; nx < maxHarmonics; ++nx)
			for(int i = 0; i < loop_ub; ++i)
				harmonic_amplitudes[ i + harmonic_amplitudes.size( 0 ) * nx ] = harmonicDistro[ nx ] * amplitudes_data[ i ];

		// rad/second
		loop_ub = harmonicFreqs.size( 0 ) * harmonicFreqs.size( 1 );

		for(int i = 0; i < loop_ub; ++i)
			harmonicFreqs[ i ] = harmonicFreqs[ i ] * twopi / sampleRate;

		// rad/sample
		// Accumulate phase and synthesize
		nx = 2;

		if(harmonicFreqs.size( 0 ) != 1)
			nx = 1;

		if(nx == 1)
		{
			if(( harmonicFreqs.size( 0 ) != 0 ) && ( harmonicFreqs.size( 1 ) != 0 ))
			{
				int idx{};

				for(int k = 0; k < harmonicFreqs.size( 1 ); ++k)
				{
					if(0 <= harmonicFreqs.size( 0 ) - 2)
						idx = k + 1;

					for(nx = 0; nx <= harmonicFreqs.size( 0 ) - 2; ++nx)
					{
						harmonicFreqs[ ( nx + harmonicFreqs.size( 0 ) * ( idx - 1 ) ) + 1 ] =
							harmonicFreqs[ nx + harmonicFreqs.size( 0 ) * k ] +
							harmonicFreqs[ ( nx + harmonicFreqs.size( 0 ) * ( idx - 1 ) ) + 1 ];
					}
				}
			}
		}
		else
		{
			if(( harmonicFreqs.size( 0 ) != 0 ) && ( harmonicFreqs.size( 1 ) != 0 ))
			{
				for(int k = 0; k <= harmonicFreqs.size( 1 ) - 2; ++k)
				{
					for(nx = 0; nx < harmonicFreqs.size( 0 ); ++nx)
					{
						harmonicFreqs[ nx + harmonicFreqs.size( 0 ) * ( k + 1 ) ] =
							harmonicFreqs[ nx + harmonicFreqs.size( 0 ) * k ] +
							harmonicFreqs[ nx + harmonicFreqs.size( 0 ) * ( k + 1 ) ];
					}
				}
			}
		}

		// Save last phases of all harmonics for next buffer
		for(nx = 0; nx < harmonicFreqs.size( 1 ); ++nx)
		{
			if(1 > harmonicFreqs.size( 0 ))
				loop_ub = 0;
			else
				loop_ub = harmonicFreqs.size( 0 );

			for(int i1 = 0; i1 < loop_ub; i1++)
				x_data[ i1 ] = harmonicFreqs[ i1 + harmonicFreqs.size( 0 ) * nx ] + prev_phases[ nx ];

			for(int i1 = 0; i1 < loop_ub; i1++)
				harmonicFreqs[ i1 + harmonicFreqs.size( 0 ) * nx ] = x_data[ i1 ];
		}

		int unnamed_idx_1{};

		// For now: check if f0 == 0hz and reset phase
		for(int k = 0; k < harmonicFreqs.size( 0 ); ++k)
		{
			if(f0_data[ k ] == 0.0)
			{
				if(1 > harmonicFreqs.size( 1 ))
					unnamed_idx_1 = 0;
				else
					unnamed_idx_1 = harmonicFreqs.size( 1 );

				if(1 > harmonicFreqs.size( 1 ))
					loop_ub = 0;
				else
					loop_ub = harmonicFreqs.size( 1 );

				for(int i1 = 0; i1 < loop_ub; i1++)
					harmonicFreqs[ k + harmonicFreqs.size( 0 ) * i1 ] = 0.0;
			}
		}

		b_harmonic_frequencies.set_size( harmonicFreqs.size( 0 ), harmonicFreqs.size( 1 ) );
		loop_ub = harmonicFreqs.size( 0 ) * harmonicFreqs.size( 1 ) - 1;

		for(int i = 0; i <= loop_ub; ++i)
			b_harmonic_frequencies[ i ] = harmonicFreqs[ i ];

		harmonicFreqs.set_size( b_harmonic_frequencies.size( 0 ), b_harmonic_frequencies.size( 1 ) );
		nx = b_harmonic_frequencies.size( 0 ) * b_harmonic_frequencies.size( 1 );

		for(int k = 0; k < nx; ++k)
		{
			if(rtIsNaN( b_harmonic_frequencies[ k ] ) || rtIsInf( b_harmonic_frequencies[ k ] ))
			{
				b_sample_rate = rtNaN;
			}
			else if(b_harmonic_frequencies[ k ] == 0.0)
			{
				b_sample_rate = 0.0;
			}
			else
			{
				b_sample_rate = std::fmod( b_harmonic_frequencies[ k ], twopi );
				boolean_T rEQ0 = ( b_sample_rate == 0.0 );

				if(!rEQ0)
				{
					const auto q = std::abs( b_harmonic_frequencies[ k ] / twopi );
					rEQ0 = !( std::abs( q - std::floor( q + 0.5 ) ) > 2.2204460492503131E-16 * q );
				}

				if(rEQ0)
					b_sample_rate = 0.0;
				else
					if(b_harmonic_frequencies[ k ] < 0.0)
						b_sample_rate += twopi;
			}

			harmonicFreqs[ k ] = b_sample_rate;
		}

		last_phases.fill( {} );

		for(nx = 0; nx < harmonicFreqs.size( 1 ); ++nx)
			last_phases[ nx ] = harmonicFreqs[ ( harmonicFreqs.size( 0 ) + harmonicFreqs.size( 0 ) * nx ) - 1 ];

		// Convert to waveforms
		nx = harmonicFreqs.size( 0 ) * harmonicFreqs.size( 1 );
		for(int k = 0; k < nx; ++k)
			harmonicFreqs[ k ] = std::sin( harmonicFreqs[ k ] );

		audio.fill( {} );

		const auto totalHarmonics{ static_cast<int>( n_harmonics ) };

		int c_loop_ub{};

		if(0 <= totalHarmonics - 1)
		{
			if(1 > harmonic_amplitudes.size( 0 ))
				unnamed_idx_1 = 0;
			else
				unnamed_idx_1 = harmonic_amplitudes.size( 0 );

			c_loop_ub = unnamed_idx_1;
		}

		for(nx = 0; nx < totalHarmonics; ++nx)
		{
			for(int i1 = 0; i1 < c_loop_ub; i1++)
			{
				f0_data[ i1 ] = audio[ i1 ] + harmonic_amplitudes[ i1 +
					harmonic_amplitudes.size( 0 ) * nx ] * harmonicFreqs[ i1 + harmonicFreqs.size( 0 ) * nx ];
			}

			if(unnamed_idx_1 - 1 >= 0)
				FVOP::copy( audio.data(), f0_data, unnamed_idx_1 );
			//std::memcpy( &audio[ 0 ], &f0_data[ 0 ], unnamed_idx_1 * sizeof( double ) );
		}
	}


}