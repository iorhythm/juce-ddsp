/*
  ==============================================================================

    DDSPSynth.cpp
    Created: 23 Apr 2022 9:24:53pm
    Author:  St

  ==============================================================================
*/
#include "DDSPSynth.h"


void DDSPSynth::initialize()
{
	eml_rand_mt19937ar_stateful_init();
	isInitialized = true;
}


void DDSPSynth::terminate()
{
	// (no terminate code required)
	isInitialized = false;
}


void DDSPSynth::additive( double n_samples, double sample_rate, const double amplitudes[ 4096 ],
	double n_harmonics, double harmonic_distribution[ 100 ], const
	double f0[ 4096 ], const double prev_phases[ 100 ], double shift,
	double stretch, double audio[ 4096 ], double last_phases[ 100 ] )
{
	coder::array<double, 2U> b_harmonic_frequencies;
	coder::array<double, 2U> f_ratios;
	coder::array<double, 2U> harmonic_amplitudes;
	coder::array<double, 2U> harmonic_frequencies;
	coder::array<int, 2U> r1;
	coder::array<boolean_T, 2U> r;
	double amplitudes_data[ 4096 ];
	double f0_data[ 4096 ];
	double x_data[ 4096 ];
	double b_sample_rate;
	double q;
	int b_loop_ub;
	int c_loop_ub;
	int i;
	int i1;
	int k;
	int loop_ub;
	int nx;
	int subsb_idx_1;
	int unnamed_idx_1;

	if(!isInitialized)
	{
		initialize();
	}

	//  ADDITIVE SYNTH
	//
	//  INPUTS:
	//
	//  sample_rate: int
	//  amplitudes: Frame-wise oscillator peak amplitude. Shape: [n_frames, 1]
	//  harmonic distribution: Frame-wise harmonic amplitude variations. Shape. [n_harmonics] 
	//  f0: Frame-wise fundamental frequency in Hz. Shape: [n_frames, 1]
	//  shift: Shift fundamental frequency, range -12 (one octave down) to 12 (one octave up), 0: no effect 
	//  stretch: Stretch/squeeze harmonic frequencies, range -1 (max squeezed) to 1 (max stretched), 0: no effect 
	//
	//  RETURNS:
	//
	//  Sample-wise audio signal
	//  Resize the input
	if(1.0 > n_samples)
	{
		loop_ub = 0;
		b_loop_ub = 0;
	}
	else
	{
		loop_ub = static_cast<int>( n_samples );
		b_loop_ub = static_cast<int>( n_samples );
	}

	if(0 <= b_loop_ub - 1)
	{
		std::memcpy( &f0_data[ 0 ], &f0[ 0 ], b_loop_ub * sizeof( double ) );
	}

	//  Incorporate shift parameter
	if(shift > 0.0)
	{
		for(i = 0; i < b_loop_ub; i++)
		{
			b_sample_rate = f0[ i ];
			f0_data[ i ] = b_sample_rate + b_sample_rate / 12.0 * shift;
		}
	}
	else
	{
		if(shift < 0.0)
		{
			for(i = 0; i < b_loop_ub; i++)
			{
				b_sample_rate = f0[ i ];
				f0_data[ i ] = b_sample_rate + b_sample_rate / 12.0 * shift * 0.5;
			}
		}
	}

	//  Scale the amplitudes
	//  Scale Function
	//  Exponentiated Sigmoid pointwise nonlinearity
	for(i = 0; i < loop_ub; i++)
	{
		x_data[ i ] = -amplitudes[ i ];
	}

	for(k = 0; k < loop_ub; k++)
	{
		x_data[ k ] = std::exp( x_data[ k ] );
	}

	for(i = 0; i < loop_ub; i++)
	{
		x_data[ i ] = 1.0 / ( x_data[ i ] + 1.0 );
	}

	for(k = 0; k < loop_ub; k++)
	{
		amplitudes_data[ k ] = rt_powd_snf( x_data[ k ], 2.3025850929940459 );
	}

	for(i = 0; i < loop_ub; i++)
	{
		amplitudes_data[ i ] = 2.0 * amplitudes_data[ i ] + 1.0E-7;
	}

	//  Scale Function
	//  Exponentiated Sigmoid pointwise nonlinearity
	for(k = 0; k < 100; k++)
	{
		harmonic_distribution[ k ] = 2.0 * rt_powd_snf( 1.0 / ( std::exp
		( -harmonic_distribution[ k ] ) + 1.0 ), 2.3025850929940459 ) + 1.0E-7;
	}

	//  Bandlimit the harmonic distribution
	//  Calculate sample-wise oscillator frequencies of harmonics
	coder::linspace( n_harmonics, n_harmonics, f_ratios );

	//  Incorporate stretch parameter and shift back to f0
	if(stretch > 0.0)
	{
		i = f_ratios.size( 0 ) * f_ratios.size( 1 );
		f_ratios.set_size( 1, f_ratios.size( 1 ) );
		nx = i - 1;
		for(i = 0; i <= nx; i++)
		{
			b_sample_rate = f_ratios[ i ];
			b_sample_rate = ( b_sample_rate + b_sample_rate * stretch ) - stretch;
			f_ratios[ i ] = b_sample_rate;
		}
	}
	else
	{
		if(stretch < 0.0)
		{
			i = f_ratios.size( 0 ) * f_ratios.size( 1 );
			f_ratios.set_size( 1, f_ratios.size( 1 ) );
			q = stretch * 0.5;
			nx = i - 1;
			for(i = 0; i <= nx; i++)
			{
				b_sample_rate = f_ratios[ i ];
				b_sample_rate = ( b_sample_rate + b_sample_rate * stretch * 0.5 ) - q;
				f_ratios[ i ] = b_sample_rate;
			}
		}
	}

	harmonic_frequencies.set_size( b_loop_ub, f_ratios.size( 1 ) );
	nx = f_ratios.size( 1 );
	for(i = 0; i < nx; i++)
	{
		for(i1 = 0; i1 < b_loop_ub; i1++)
		{
			harmonic_frequencies[ i1 + harmonic_frequencies.size( 0 ) * i ] = f0_data[ i1 ] *
				f_ratios[ i ];
		}
	}

	//  Set amplitudes for oscillators above nyquist to 0
	if(1 > harmonic_frequencies.size( 1 ))
	{
		b_loop_ub = 0;
	}
	else
	{
		b_loop_ub = harmonic_frequencies.size( 1 );
	}

	b_sample_rate = sample_rate / 2.0;
	r.set_size( 1, b_loop_ub );
	for(i = 0; i < b_loop_ub; i++)
	{
		r[ i ] = ( harmonic_frequencies[ harmonic_frequencies.size( 0 ) * i ] >=
			b_sample_rate );
	}

	b_loop_ub = r.size( 1 ) - 1;
	nx = 0;
	for(k = 0; k <= b_loop_ub; k++)
	{
		if(r[ k ])
		{
			nx++;
		}
	}

	r1.set_size( 1, nx );
	nx = 0;
	for(k = 0; k <= b_loop_ub; k++)
	{
		if(r[ k ])
		{
			r1[ nx ] = k + 1;
			nx++;
		}
	}

	b_loop_ub = r1.size( 0 ) * r1.size( 1 );
	for(i = 0; i < b_loop_ub; i++)
	{
		harmonic_distribution[ r1[ i ] - 1 ] = 0.0;
	}

	//  function plot_controls(amplitudes, harmonic_distribution, f0)
	//
	//      % figure('Name', 'Synth Controls');
	//      t = tiledlayout(3,1);
	//      nexttile;
	//      plot(amplitudes);
	//      title('Amplitudes');
	//      ylabel('amplitude');
	//      nexttile;
	//      plot(harmonic_distribution);
	//      title('Harmonic Distribution');
	//      ylabel('amplitude');
	//      nexttile;
	//      plot(f0);
	//      title('Fundamental Frequency');
	//      ylabel('frequency');
	//      xlabel(t,'samples');
	//      t.TileSpacing = 'none';
	//
	//  end
	//      % Normalize the harmonic distribution
	//      harm_sum = sum(harmonic_distribution,2);
	//      for c = 1:size(harmonic_distribution,2)
	//          harmonic_distribution(1:end,c) = harmonic_distribution(1:end,c) ./ harm_sum; 
	//      end
	//  Create harmonic amplitudes
	harmonic_amplitudes.set_size( loop_ub, 100 );
	b_loop_ub = loop_ub * 100;
	for(i = 0; i < b_loop_ub; i++)
	{
		harmonic_amplitudes[ i ] = 0.0;
	}

	for(nx = 0; nx < 100; nx++)
	{
		for(i = 0; i < loop_ub; i++)
		{
			harmonic_amplitudes[ i + harmonic_amplitudes.size( 0 ) * nx ] =
				harmonic_distribution[ nx ] * amplitudes_data[ i ];
		}
	}

	// radiant/second
	loop_ub = harmonic_frequencies.size( 0 ) * harmonic_frequencies.size( 1 );
	for(i = 0; i < loop_ub; i++)
	{
		harmonic_frequencies[ i ] = harmonic_frequencies[ i ] * 2.0 * 3.1415926535897931
			/ sample_rate;
	}

	// radiant/sample
	//  Accumulate phase and synthesize
	nx = 2;
	if(harmonic_frequencies.size( 0 ) != 1)
	{
		nx = 1;
	}

	if(1 == nx)
	{
		if(( harmonic_frequencies.size( 0 ) != 0 ) && ( harmonic_frequencies.size( 1 ) !=
			0 ))
		{
			i = harmonic_frequencies.size( 1 );
			for(k = 0; k < i; k++)
			{
				i1 = harmonic_frequencies.size( 0 );
				if(0 <= i1 - 2)
				{
					subsb_idx_1 = k + 1;
				}

				for(nx = 0; nx <= i1 - 2; nx++)
				{
					harmonic_frequencies[ ( nx + harmonic_frequencies.size( 0 ) * ( subsb_idx_1
						- 1 ) ) + 1 ] = harmonic_frequencies[ nx + harmonic_frequencies.size( 0 ) *
						k ] + harmonic_frequencies[ ( nx + harmonic_frequencies.size( 0 ) *
							( subsb_idx_1 - 1 ) ) + 1 ];
				}
			}
		}
	}
	else
	{
		if(( harmonic_frequencies.size( 0 ) != 0 ) && ( harmonic_frequencies.size( 1 ) !=
			0 ))
		{
			i = harmonic_frequencies.size( 1 );
			for(k = 0; k <= i - 2; k++)
			{
				i1 = harmonic_frequencies.size( 0 );
				for(nx = 0; nx < i1; nx++)
				{
					harmonic_frequencies[ nx + harmonic_frequencies.size( 0 ) * ( k + 1 ) ] =
						harmonic_frequencies[ nx + harmonic_frequencies.size( 0 ) * k ] +
						harmonic_frequencies[ nx + harmonic_frequencies.size( 0 ) * ( k + 1 ) ];
				}
			}
		}
	}

	//  Save last phases of all harmonics for next buffer;
	i = harmonic_frequencies.size( 1 );
	for(nx = 0; nx < i; nx++)
	{
		if(1 > harmonic_frequencies.size( 0 ))
		{
			loop_ub = 0;
		}
		else
		{
			loop_ub = harmonic_frequencies.size( 0 );
		}

		for(i1 = 0; i1 < loop_ub; i1++)
		{
			x_data[ i1 ] = harmonic_frequencies[ i1 + harmonic_frequencies.size( 0 ) * nx ]
				+ prev_phases[ nx ];
		}

		for(i1 = 0; i1 < loop_ub; i1++)
		{
			harmonic_frequencies[ i1 + harmonic_frequencies.size( 0 ) * nx ] = x_data[ i1 ];
		}
	}

	//  For now: check if f0 == 0hz and reset phase
	i = harmonic_frequencies.size( 0 );
	for(k = 0; k < i; k++)
	{
		if(f0_data[ k ] == 0.0)
		{
			if(1 > harmonic_frequencies.size( 1 ))
			{
				unnamed_idx_1 = 0;
			}
			else
			{
				unnamed_idx_1 = harmonic_frequencies.size( 1 );
			}

			if(1 > harmonic_frequencies.size( 1 ))
			{
				loop_ub = 0;
			}
			else
			{
				loop_ub = harmonic_frequencies.size( 1 );
			}

			for(i1 = 0; i1 < loop_ub; i1++)
			{
				harmonic_frequencies[ k + harmonic_frequencies.size( 0 ) * i1 ] = 0.0;
			}
		}
	}

	b_harmonic_frequencies.set_size( harmonic_frequencies.size( 0 ),
		harmonic_frequencies.size( 1 ) );
	loop_ub = harmonic_frequencies.size( 0 ) * harmonic_frequencies.size( 1 ) - 1;
	for(i = 0; i <= loop_ub; i++)
	{
		b_harmonic_frequencies[ i ] = harmonic_frequencies[ i ];
	}

	harmonic_frequencies.set_size( b_harmonic_frequencies.size( 0 ),
		b_harmonic_frequencies.size( 1 ) );
	nx = b_harmonic_frequencies.size( 0 ) * b_harmonic_frequencies.size( 1 );
	for(k = 0; k < nx; k++)
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
			boolean_T rEQ0;
			b_sample_rate = std::fmod( b_harmonic_frequencies[ k ], 6.2831853071795862 );
			rEQ0 = ( b_sample_rate == 0.0 );
			if(!rEQ0)
			{
				q = std::abs( b_harmonic_frequencies[ k ] / 6.2831853071795862 );
				rEQ0 = !( std::abs( q - std::floor( q + 0.5 ) ) > 2.2204460492503131E-16 * q );
			}

			if(rEQ0)
			{
				b_sample_rate = 0.0;
			}
			else
			{
				if(b_harmonic_frequencies[ k ] < 0.0)
				{
					b_sample_rate += 6.2831853071795862;
				}
			}
		}

		harmonic_frequencies[ k ] = b_sample_rate;
	}

	std::memset( &last_phases[ 0 ], 0, 100U * sizeof( double ) );
	i = harmonic_frequencies.size( 1 );
	for(nx = 0; nx < i; nx++)
	{
		last_phases[ nx ] = harmonic_frequencies[ ( harmonic_frequencies.size( 0 ) +
			harmonic_frequencies.size( 0 ) * nx ) - 1 ];
	}

	//  Convert to waveforms
	nx = harmonic_frequencies.size( 0 ) * harmonic_frequencies.size( 1 );
	for(k = 0; k < nx; k++)
	{
		harmonic_frequencies[ k ] = std::sin( harmonic_frequencies[ k ] );
	}

	std::memset( &audio[ 0 ], 0, 4096U * sizeof( double ) );
	i = static_cast<int>( n_harmonics );
	if(0 <= static_cast<int>( n_harmonics ) - 1)
	{
		if(1 > harmonic_amplitudes.size( 0 ))
		{
			unnamed_idx_1 = 0;
		}
		else
		{
			unnamed_idx_1 = harmonic_amplitudes.size( 0 );
		}

		c_loop_ub = unnamed_idx_1;
	}

	for(nx = 0; nx < i; nx++)
	{
		for(i1 = 0; i1 < c_loop_ub; i1++)
		{
			f0_data[ i1 ] = audio[ i1 ] + harmonic_amplitudes[ i1 +
				harmonic_amplitudes.size( 0 ) * nx ] * harmonic_frequencies[ i1 +
				harmonic_frequencies.size( 0 ) * nx ];
		}

		if(0 <= unnamed_idx_1 - 1)
		{
			std::memcpy( &audio[ 0 ], &f0_data[ 0 ], unnamed_idx_1 * sizeof( double ) );
		}
	}
}

void DDSPSynth::subtractive( double n_samples, const double magnitudes[ 65 ], double color, double initial_bias, double out[ 4096 ] )
{
	static const double dv[ 65 ] = { 0.0, 0.0024076366639015356,
		0.0096073597983847847, 0.021529832133895588, 0.038060233744356631,
		0.059039367825822475, 0.084265193848727382, 0.1134947733186315,
		0.14644660940672621, 0.18280335791817726, 0.22221488349019886,
		0.2643016315870011, 0.30865828381745508, 0.35485766137276886,
		0.40245483899193585, 0.45099142983521961, 0.49999999999999994,
		0.54900857016478033, 0.5975451610080641, 0.645142338627231,
		0.69134171618254481, 0.73569836841299885, 0.777785116509801,
		0.81719664208182263, 0.85355339059327373, 0.88650522668136844,
		0.91573480615127267, 0.94096063217417747, 0.96193976625564337,
		0.97847016786610441, 0.99039264020161522, 0.99759236333609835, 1.0,
		0.99759236333609835, 0.99039264020161522, 0.97847016786610441,
		0.96193976625564337, 0.94096063217417747, 0.91573480615127267,
		0.88650522668136844, 0.85355339059327373, 0.81719664208182263,
		0.777785116509801, 0.73569836841299885, 0.69134171618254481,
		0.645142338627231, 0.5975451610080641, 0.54900857016478033,
		0.49999999999999994, 0.45099142983521961, 0.40245483899193585,
		0.35485766137276886, 0.30865828381745508, 0.2643016315870011,
		0.22221488349019886, 0.18280335791817726, 0.14644660940672621,
		0.1134947733186315, 0.084265193848727382, 0.059039367825822475,
		0.038060233744356631, 0.021529832133895588, 0.0096073597983847847,
		0.0024076366639015356, 0.0 };

	coder::dsp::ColoredNoise white_n;
	coder::dsp::b_ColoredNoise brown_n;
	coder::dsp::c_ColoredNoise violet_n;

	coder::array<creal_T, 1U> H;
	coder::array<creal_T, 1U> X;
	coder::array<double, 1U> sub;
	creal_T dcv[ 65 ];
	creal_T dcv1[ 65 ];
	double brown_noise[ 4096 ];
	double violet_noise[ 4096 ];
	double white_noise[ 4096 ];
	double x[ 65 ];
	double z1[ 65 ];
	double r1;
	double r3;
	double sigma;
	int signal_size[ 1 ];
	int b_eint;
	int eint;
	int idx;
	int loop_ub;
	short b_tmp_data[ 4096 ];
	boolean_T tmp_data[ 4096 ];
	if(!isInitialized)
	{
		initialize();
	}

	//  function [out, b] = subtractive(n_samples, magnitudes, color, ir_coeffs, recalculate_ir) 
	//  magnitudes: row = frames, column = freq responses
	//  magnitudes should be 65
	for(int k = 0; k < 65; k++)
	{
		z1[ k ] = rt_powd_snf( 1.0 / ( std::exp( -( magnitudes[ k ] + initial_bias ) ) + 1.0 ),
			2.3025850929940459 );
	}

	//  generate white noise
	white_n.init();
	brown_n.init();
	violet_n.init();
	white_n.step( white_noise, state );
	brown_n.step( brown_noise, state );
	violet_n.step( violet_noise, state );
	white_n.release();
	brown_n.release();
	violet_n.release();
	sigma = std::abs( color );
	for(idx = 0; idx < 4096; idx++)
	{
		white_noise[ idx ] *= 1.0 - sigma;
	}

	if(color > 0.0)
	{
		for(idx = 0; idx < 4096; idx++)
		{
			white_noise[ idx ] += color * violet_noise[ idx ];
		}
	}

	if(color < 0.0)
	{
		for(idx = 0; idx < 4096; idx++)
		{
			white_noise[ idx ] += sigma * brown_noise[ idx ];
		}
	}

	if(1.0 > n_samples)
	{
		loop_ub = 0;
	}
	else
	{
		loop_ub = static_cast<int>( n_samples );
	}

	signal_size[ 0 ] = loop_ub;
	if(0 <= loop_ub - 1)
	{
		std::memcpy( &brown_noise[ 0 ], &white_noise[ 0 ], loop_ub * sizeof( double ) );
	}

	if(loop_ub <= 2)
	{
		if(loop_ub == 1)
		{
			r1 = white_noise[ 0 ];
			r3 = white_noise[ 0 ];
		}
		else
		{
			if(( white_noise[ 0 ] > white_noise[ 1 ] ) || ( rtIsNaN( white_noise[ 0 ] ) &&
				( !rtIsNaN( white_noise[ 1 ] ) ) ))
			{
				r1 = white_noise[ 1 ];
			}
			else
			{
				r1 = white_noise[ 0 ];
			}

			if(( white_noise[ 0 ] < white_noise[ 1 ] ) || ( rtIsNaN( white_noise[ 0 ] ) &&
				( !rtIsNaN( white_noise[ 1 ] ) ) ))
			{
				r3 = white_noise[ 1 ];
			}
			else
			{
				r3 = white_noise[ 0 ];
			}
		}
	}
	else
	{
		boolean_T b;
		boolean_T exitg1;
		b = rtIsNaN( white_noise[ 0 ] );
		if(!b)
		{
			idx = 1;
		}
		else
		{
			idx = 0;
			int k = 2;
			exitg1 = false;
			while(( !exitg1 ) && ( k <= loop_ub ))
			{
				if(!rtIsNaN( white_noise[ k - 1 ] ))
				{
					idx = k;
					exitg1 = true;
				}
				else
				{
					k++;
				}
			}
		}

		if(idx == 0)
		{
			r1 = white_noise[ 0 ];
		}
		else
		{
			r1 = white_noise[ idx - 1 ];
			idx++;
			for(int k = idx; k <= loop_ub; k++)
			{
				sigma = white_noise[ k - 1 ];
				if(r1 > sigma)
				{
					r1 = sigma;
				}
			}
		}

		if(!b)
		{
			idx = 1;
		}
		else
		{
			idx = 0;
			int k = 2;
			exitg1 = false;
			while(( !exitg1 ) && ( k <= loop_ub ))
			{
				if(!rtIsNaN( white_noise[ k - 1 ] ))
				{
					idx = k;
					exitg1 = true;
				}
				else
				{
					k++;
				}
			}
		}

		if(idx == 0)
		{
			r3 = white_noise[ 0 ];
		}
		else
		{
			r3 = white_noise[ idx - 1 ];
			idx++;
			for(int k = idx; k <= loop_ub; k++)
			{
				sigma = white_noise[ k - 1 ];
				if(r3 < sigma)
				{
					r3 = sigma;
				}
			}
		}
	}

	if(loop_ub != 0)
	{
		double iMax;
		double iMin;
		if(( 0.0 < r3 ) || rtIsNaN( r3 ))
		{
			sigma = 0.0;
		}
		else
		{
			sigma = r3;
		}

		if(( !( sigma > r1 ) ) && ( !rtIsNaN( r1 ) ))
		{
			sigma = r1;
		}

		signal_size[ 0 ] = loop_ub;
		for(idx = 0; idx < loop_ub; idx++)
		{
			brown_noise[ idx ] = white_noise[ idx ] - sigma;
		}

		iMin = r1 - sigma;
		iMax = r3 - sigma;
		sigma = std::abs( iMax );
		r1 = std::abs( iMin );
		if(( !( sigma > r1 ) ) && ( !rtIsNaN( r1 ) ))
		{
			sigma = r1;
		}

		if(( !rtIsInf( sigma ) ) && ( !rtIsNaN( sigma ) ))
		{
			frexp( sigma, &b_eint );
		}
		else
		{
			b_eint = 0;
		}

		r1 = rt_powd_snf( 2.0, static_cast<double>( b_eint ) - 1.0 );
		sigma = ( static_cast<double>( b_eint ) + 1.0 ) / 2.0;
		if(sigma < 0.0)
		{
			sigma = std::ceil( sigma );
		}
		else
		{
			sigma = std::floor( sigma );
		}

		r3 = rt_powd_snf( 2.0, sigma - 1.0 );
		if(iMin == iMax)
		{
			for(idx = 0; idx < loop_ub; idx++)
			{
				tmp_data[ idx ] = rtIsNaN( brown_noise[ idx ] );
			}

			for(idx = 0; idx < loop_ub; idx++)
			{
				tmp_data[ idx ] = !tmp_data[ idx ];
			}

			int k = loop_ub - 1;
			loop_ub = 0;
			idx = 0;
			for(b_eint = 0; b_eint <= k; b_eint++)
			{
				if(tmp_data[ b_eint ])
				{
					loop_ub++;
					b_tmp_data[ idx ] = static_cast<short>( b_eint + 1 );
					idx++;
				}
			}

			for(idx = 0; idx < loop_ub; idx++)
			{
				brown_noise[ b_tmp_data[ idx ] - 1 ] = -1.0;
			}
		}
		else
		{
			double c1;
			c1 = iMax / r1;
			sigma = iMin / r1;
			r1 = 2.0 / ( iMax / r3 - iMin / r3 ) / r3;
			sigma = r3 * ( ( c1 * ( -1.0 / r3 ) - sigma * ( 1.0 / r3 ) ) / ( c1 - sigma ) );
			sub.set_size( loop_ub );
			for(idx = 0; idx < loop_ub; idx++)
			{
				sub[ idx ] = brown_noise[ idx ];
			}

			for(idx = 0; idx < loop_ub; idx++)
			{
				brown_noise[ idx ] = r1 * brown_noise[ idx ] + sigma;
			}

			int k = sub.size( 0 );
			for(b_eint = 0; b_eint < k; b_eint++)
			{
				if(brown_noise[ b_eint ] < -1.0)
				{
					brown_noise[ b_eint ] = -1.0;
				}
			}

			for(b_eint = 0; b_eint < loop_ub; b_eint++)
			{
				if(brown_noise[ b_eint ] > 1.0)
				{
					brown_noise[ b_eint ] = 1.0;
				}
			}
		}
	}

	sigma = std::abs( n_samples );
	if(!rtIsInf( sigma ))
	{
		r1 = frexp( sigma, &eint );
		sigma = eint;
		if(r1 == 0.5)
		{
			sigma = static_cast<double>( eint ) - 1.0;
		}
	}

	sigma = rt_powd_snf( 2.0, sigma );
	for(b_eint = 0; b_eint < 65; b_eint++)
	{
		dcv[ b_eint ].re = 2.0 * z1[ b_eint ] + 1.0E-7;
		dcv[ b_eint ].im = 0.0;
		x[ b_eint ] = dv[ b_eint ];
	}

	coder::ifft( dcv, dcv1 );
	coder::fftshift( x );
	for(idx = 0; idx < 65; idx++)
	{
		x[ idx ] *= dcv1[ idx ].re;
	}

	coder::fftshift( x );
	coder::fft( x, sigma, H );
	coder::fft( brown_noise, signal_size, sigma, X );
	loop_ub = X.size( 0 );
	for(idx = 0; idx < loop_ub; idx++)
	{
		sigma = X[ idx ].re * H[ idx ].im + X[ idx ].im * H[ idx ].re;
		X[ idx ].re = X[ idx ].re * H[ idx ].re - X[ idx ].im * H[ idx ].im;
		X[ idx ].im = sigma;
	}

	coder::ifft( X, n_samples, H );
	sub.set_size( H.size( 0 ) );
	loop_ub = H.size( 0 );
	for(idx = 0; idx < loop_ub; idx++)
	{
		sub[ idx ] = H[ idx ].re;
	}

	std::memset( &out[ 0 ], 0, 4096U * sizeof( double ) );
	if(1.0 > n_samples)
	{
		loop_ub = 0;
	}
	else
	{
		loop_ub = static_cast<int>( n_samples );
	}

	for(idx = 0; idx < loop_ub; idx++)
	{
		out[ idx ] = sub[ idx ];
	}
}


int DDSPSynth::div_s32_floor( int numerator, int denominator )
{
	unsigned absNumerator;
	int quotient;

	if(denominator == 0)
	{
		if(numerator >= 0)
		{
			quotient = MAX_int32_T;
		}
		else
		{
			quotient = MIN_int32_T;
		}
	}
	else
	{
		unsigned absDenominator;
		unsigned tempAbsQuotient;
		boolean_T quotientNeedsNegation;

		if(numerator < 0)
		{
			absNumerator = ~static_cast<unsigned int>( numerator ) + 1U;
		}
		else
		{
			absNumerator = static_cast<unsigned int>( numerator );
		}

		if(denominator < 0)
		{
			absDenominator = ~static_cast<unsigned int>( denominator ) + 1U;
		}
		else
		{
			absDenominator = static_cast<unsigned int>( denominator );
		}

		quotientNeedsNegation = ( ( numerator < 0 ) != ( denominator < 0 ) );
		tempAbsQuotient = absNumerator / absDenominator;
		if(quotientNeedsNegation)
		{
			absNumerator %= absDenominator;
			if(absNumerator > 0U)
			{
				tempAbsQuotient++;
			}

			quotient = -static_cast<int>( tempAbsQuotient );
		}
		else
		{
			quotient = static_cast<int>( tempAbsQuotient );
		}
	}

	return quotient;
}

double DDSPSynth::rt_powd_snf( double u0, double u1 )
{
	double y;
	if(rtIsNaN( u0 ) || rtIsNaN( u1 ))
	{
		y = rtNaN;
	}
	else
	{
		double d = std::abs( u0 );
		double d1 = std::abs( u1 );

		if(rtIsInf( u1 ))
		{
			if(d == 1.0)
			{
				y = 1.0;
			}
			else if(d > 1.0)
			{
				if(u1 > 0.0)
				{
					y = rtInf;
				}
				else
				{
					y = 0.0;
				}
			}
			else if(u1 > 0.0)
			{
				y = 0.0;
			}
			else
			{
				y = rtInf;
			}
		}
		else if(d1 == 0.0)
		{
			y = 1.0;
		}
		else if(d1 == 1.0)
		{
			if(u1 > 0.0)
			{
				y = u0;
			}
			else
			{
				y = 1.0 / u0;
			}
		}
		else if(u1 == 2.0)
		{
			y = u0 * u0;
		}
		else if(( u1 == 0.5 ) && ( u0 >= 0.0 ))
		{
			y = std::sqrt( u0 );
		}
		else if(( u0 < 0.0 ) && ( u1 > std::floor( u1 ) ))
		{
			y = rtNaN;
		}
		else
		{
			y = std::pow( u0, u1 );
		}
	}

	return y;
}


double DDSPSynth::rt_roundd_snf( double u )
{
	double y;
	if(std::abs( u ) < 4.503599627370496E+15)
	{
		if(u >= 0.5)
		{
			y = std::floor( u + 0.5 );
		}
		else if(u > -0.5)
		{
			y = u * 0.0;
		}
		else
		{
			y = std::ceil( u - 0.5 );
		}
	}
	else
	{
		y = u;
	}

	return y;
}

void DDSPSynth::scale_f0( const double f0_in[ 100 ], boolean_T to_midi, double f0_scaled[ 100 ] )
{
	int i;

	if(!isInitialized)
	{
		initialize();
	}

	std::memset( &f0_scaled[ 0 ], 0, 100U * sizeof( double ) );

	if(to_midi)
	{
		double note;
		if(f0_in[ 0 ] == -1.0)
		{
			note = 0.0;
		}
		else
		{
			if(f0_in[ 0 ] == 0.0)
			{
				note = rtMinusInf;
			}
			else if(f0_in[ 0 ] < 0.0)
			{
				note = rtNaN;
			}
			else if(( !rtIsInf( f0_in[ 0 ] ) ) && ( !rtIsNaN( f0_in[ 0 ] ) ))
			{
				note = frexp( f0_in[ 0 ], &i );
				if(note == 0.5)
				{
					note = static_cast<double>( i ) - 1.0;
				}
				else if(( i == 1 ) && ( note < 0.75 ))
				{
					note = std::log( 2.0 * note ) / 0.69314718055994529;
				}
				else
				{
					note = std::log( note ) / 0.69314718055994529 + static_cast<double>( i );
				}
			}
			else
			{
				note = f0_in[ 0 ];
			}

			note = 12.0 * ( note - 8.78135971352466 ) + 69.0;
		}

		for(i = 0; i < 100; i++)
		{
			f0_scaled[ i ] = note / 127.0;
		}
	}

	if(!to_midi)
	{
		for(i = 0; i < 100; i++)
		{
			f0_scaled[ i ] = 440.0 * rt_powd_snf( 2.0, ( f0_in[ i ] * 127.0 - 69.0 ) / 12.0 );
		}
	}
}

double DDSPSynth::getPitch( double n_samples, const double input[ 4096 ], double fs )
{
	coder::array<creal_T, 1U> Y;
	coder::array<double, 2U> b;
	coder::array<double, 2U> z1;
	coder::array<double, 1U> Vq;
	coder::array<double, 1U> bw;
	coder::array<double, 1U> bw_temp;
	coder::array<double, 1U> candidates;
	coder::array<double, 1U> log_spaced_frequency;
	coder::array<double, 1U> r;
	double b_win_data[ 4097 ];
	double win_data[ 4097 ];
	double input_data[ 4096 ];
	double tmp_data[ 2048 ];
	double NFFT;
	double delta1;
	double f0;
	int input_size[ 1 ];
	int tmp_size[ 1 ];
	int i;
	int k;
	int loop_ub;
	int low_ip1;
	int mid_i;
	int n;
	int nrows;
	int nx;
	unsigned int u;
	signed char w_data[ 1 ];
	if(!isInitialized)
	{
		initialize();
	}

	// getPitch
	// parameters: n_samples (int32), input (double[4096]), fs (double)
	// output: f0 (double)
	if(1.0 > n_samples)
	{
		loop_ub = 0;
	}
	else
	{
		loop_ub = static_cast<int>( n_samples );
	}

	delta1 = frexp( static_cast<double>( static_cast<int>( std::abs( 2.0 *
		static_cast<double>( loop_ub ) - 1.0 ) ) ), &nrows );
	if(delta1 == 0.5)
	{
		nrows--;
	}

	NFFT = rt_powd_snf( 2.0, static_cast<double>( nrows ) );
	delta1 = fs / 2.0 - 1.0;
	if(!( delta1 < 4000.0 ))
	{
		delta1 = 4000.0;
	}

	coder::linspace( std::log10( delta1 ), NFFT, b );
	z1.set_size( 1, b.size( 1 ) );
	nx = b.size( 1 );
	for(k = 0; k < nx; k++)
	{
		z1[ k ] = rt_powd_snf( 10.0, b[ k ] );
	}

	log_spaced_frequency.set_size( z1.size( 1 ) );
	nrows = z1.size( 1 );
	for(i = 0; i < nrows; i++)
	{
		log_spaced_frequency[ i ] = z1[ i ];
	}

	delta1 = fs / 2.0;
	u = static_cast<unsigned int>( rt_roundd_snf( NFFT / 2.0 ) );
	z1.set_size( 1, ( static_cast<int>( u ) + 1 ) );
	z1[ static_cast<int>( u ) ] = delta1;
	if(z1.size( 1 ) >= 2)
	{
		z1[ 0 ] = 0.0;
		if(z1.size( 1 ) >= 3)
		{
			if(( 0.0 == -delta1 ) && ( static_cast<int>( u ) + 1 > 2 ))
			{
				i = static_cast<int>( u );
				for(k = 2; k <= i; k++)
				{
					z1[ k - 1 ] = delta1 * ( ( static_cast<double>( ( ( k << 1 ) - static_cast<int>
						( u ) ) - 1 ) - 1.0 ) / ( static_cast<double>( static_cast<int>( u ) + 1 ) -
							1.0 ) );
				}

				if(( ( static_cast<int>( u ) + 1 ) & 1 ) == 1)
				{
					z1[ ( static_cast<int>( u ) + 1 ) >> 1 ] = 0.0;
				}
			}
			else if(( delta1 < 0.0 ) && ( std::abs( delta1 ) > 8.9884656743115785E+307 ))
			{
				delta1 /= static_cast<double>( z1.size( 1 ) ) - 1.0;
				i = z1.size( 1 );
				for(k = 0; k <= i - 3; k++)
				{
					z1[ k + 1 ] = delta1 * ( static_cast<double>( k ) + 1.0 );
				}
			}
			else
			{
				delta1 /= static_cast<double>( z1.size( 1 ) ) - 1.0;
				i = z1.size( 1 );
				for(k = 0; k <= i - 3; k++)
				{
					z1[ k + 1 ] = ( static_cast<double>( k ) + 1.0 ) * delta1;
				}
			}
		}
	}

	if(3 > log_spaced_frequency.size( 0 ))
	{
		i = 0;
		n = 0;
	}
	else
	{
		i = 2;
		n = log_spaced_frequency.size( 0 );
	}

	nrows = n - i;
	bw_temp.set_size( nrows );
	for(n = 0; n < nrows; n++)
	{
		bw_temp[ n ] = ( log_spaced_frequency[ i + n ] - log_spaced_frequency[ n ] ) / 2.0;
	}

	bw.set_size( ( bw_temp.size( 0 ) + 2 ) );
	bw[ 0 ] = bw_temp[ 0 ] / NFFT;
	nrows = bw_temp.size( 0 );
	for(i = 0; i < nrows; i++)
	{
		bw[ i + 1 ] = bw_temp[ i ] / NFFT;
	}

	bw[ bw_temp.size( 0 ) + 1 ] = bw_temp[ bw_temp.size( 0 ) - 1 ] / NFFT;
	nrows = 1;
	w_data[ 0 ] = 0;
	n = 0;
	if(loop_ub == 0)
	{
		nrows = 0;
		n = 1;
	}
	else
	{
		if(loop_ub == 1)
		{
			w_data[ 0 ] = 1;
			n = 1;
		}
	}

	if(0 <= nrows - 1)
	{
		win_data[ 0 ] = w_data[ 0 ];
	}

	if(n == 0)
	{
		if(coder::iseven( static_cast<double>( loop_ub ) ))
		{
			coder::calc_window( static_cast<double>( loop_ub ) / 2.0, static_cast<double>
				( loop_ub ), tmp_data, tmp_size );
			nx = tmp_size[ 0 ];
			nrows = tmp_size[ 0 ];
			if(0 <= nrows - 1)
			{
				std::memcpy( &win_data[ 0 ], &tmp_data[ 0 ], nrows * sizeof( double ) );
			}

			if(1 > tmp_size[ 0 ])
			{
				i = 0;
				n = 1;
				low_ip1 = -1;
			}
			else
			{
				i = tmp_size[ 0 ] - 1;
				n = -1;
				low_ip1 = 0;
			}

			nrows = div_s32_floor( low_ip1 - i, n );
			mid_i = ( tmp_size[ 0 ] + nrows ) + 1;
			if(0 <= nx - 1)
			{
				std::memcpy( &b_win_data[ 0 ], &win_data[ 0 ], nx * sizeof( double ) );
			}

			for(low_ip1 = 0; low_ip1 <= nrows; low_ip1++)
			{
				b_win_data[ low_ip1 + nx ] = win_data[ i + n * low_ip1 ];
			}

			if(0 <= mid_i - 1)
			{
				std::memcpy( &win_data[ 0 ], &b_win_data[ 0 ], mid_i * sizeof( double ) );
			}
		}
		else
		{
			coder::calc_window( ( static_cast<double>( loop_ub ) + 1.0 ) / 2.0,
				static_cast<double>( loop_ub ), tmp_data, tmp_size );
			nx = tmp_size[ 0 ];
			nrows = tmp_size[ 0 ];
			if(0 <= nrows - 1)
			{
				std::memcpy( &win_data[ 0 ], &tmp_data[ 0 ], nrows * sizeof( double ) );
			}

			if(1 > tmp_size[ 0 ] - 1)
			{
				i = 0;
				n = 1;
				low_ip1 = -1;
			}
			else
			{
				i = tmp_size[ 0 ] - 2;
				n = -1;
				low_ip1 = 0;
			}

			nrows = div_s32_floor( low_ip1 - i, n );
			mid_i = ( tmp_size[ 0 ] + nrows ) + 1;
			if(0 <= nx - 1)
			{
				std::memcpy( &b_win_data[ 0 ], &win_data[ 0 ], nx * sizeof( double ) );
			}

			for(low_ip1 = 0; low_ip1 <= nrows; low_ip1++)
			{
				b_win_data[ low_ip1 + nx ] = win_data[ i + n * low_ip1 ];
			}

			if(0 <= mid_i - 1)
			{
				std::memcpy( &win_data[ 0 ], &b_win_data[ 0 ], mid_i * sizeof( double ) );
			}
		}
	}

	input_size[ 0 ] = loop_ub;
	for(i = 0; i < loop_ub; i++)
	{
		input_data[ i ] = input[ i ] * win_data[ i ];
	}

	coder::fft( input_data, input_size, NFFT, Y );
	loop_ub = static_cast<int>( NFFT / 2.0 + 1.0 );
	r.set_size( bw.size( 0 ) );
	nrows = bw.size( 0 );
	for(k = 0; k < nrows; k++)
	{
		r[ k ] = bw[ k ];
	}

	bw_temp.set_size( z1.size( 1 ) );
	nrows = z1.size( 1 );
	for(i = 0; i < nrows; i++)
	{
		bw_temp[ i ] = z1[ i ];
	}

	bw.set_size( loop_ub );
	for(i = 0; i < loop_ub; i++)
	{
		bw[ i ] = Y[ i ].re * Y[ i ].re - Y[ i ].im * -Y[ i ].im;
	}

	nx = bw_temp.size( 0 ) - 1;
	Vq.set_size( log_spaced_frequency.size( 0 ) );
	loop_ub = log_spaced_frequency.size( 0 );
	for(i = 0; i < loop_ub; i++)
	{
		Vq[ i ] = rtNaN;
	}

	if(log_spaced_frequency.size( 0 ) != 0)
	{
		k = 0;
		int exitg1;
		do
		{
			exitg1 = 0;
			if(k <= nx)
			{
				if(rtIsNaN( bw_temp[ k ] ))
				{
					exitg1 = 1;
				}
				else
				{
					k++;
				}
			}
			else
			{
				if(bw_temp[ 1 ] < bw_temp[ 0 ])
				{
					i = ( nx + 1 ) >> 1;
					for(n = 0; n < i; n++)
					{
						delta1 = bw_temp[ n ];
						nrows = nx - n;
						bw_temp[ n ] = bw_temp[ nrows ];
						bw_temp[ nrows ] = delta1;
					}

					if(bw.size( 0 ) > 1)
					{
						n = bw.size( 0 ) - 1;
						nrows = bw.size( 0 ) >> 1;
						for(k = 0; k < nrows; k++)
						{
							delta1 = bw[ k ];
							nx = n - k;
							bw[ k ] = bw[ nx ];
							bw[ nx ] = delta1;
						}
					}
				}

				nrows = log_spaced_frequency.size( 0 );
				for(k = 0; k < nrows; k++)
				{
					if(rtIsNaN( log_spaced_frequency[ k ] ))
					{
						Vq[ k ] = rtNaN;
					}
					else
					{
						if(( !( log_spaced_frequency[ k ] > bw_temp[ bw_temp.size( 0 ) - 1 ] ) ) && (
							!( log_spaced_frequency[ k ] < bw_temp[ 0 ] ) ))
						{
							n = bw_temp.size( 0 );
							nx = 1;
							low_ip1 = 2;
							while(n > low_ip1)
							{
								mid_i = ( nx >> 1 ) + ( n >> 1 );
								if(( ( nx & 1 ) == 1 ) && ( ( n & 1 ) == 1 ))
								{
									mid_i++;
								}

								if(log_spaced_frequency[ k ] >= bw_temp[ mid_i - 1 ])
								{
									nx = mid_i;
									low_ip1 = mid_i + 1;
								}
								else
								{
									n = mid_i;
								}
							}

							delta1 = bw_temp[ nx - 1 ];
							delta1 = ( log_spaced_frequency[ k ] - delta1 ) / ( bw_temp[ nx ] -
								delta1 );
							if(delta1 == 0.0)
							{
								Vq[ k ] = bw[ nx - 1 ];
							}
							else if(delta1 == 1.0)
							{
								Vq[ k ] = bw[ nx ];
							}
							else
							{
								NFFT = bw[ nx - 1 ];
								if(NFFT == bw[ nx ])
								{
									Vq[ k ] = NFFT;
								}
								else
								{
									Vq[ k ] = ( 1.0 - delta1 ) * NFFT + delta1 * bw[ nx ];
								}
							}
						}
					}
				}

				exitg1 = 1;
			}
		} while(exitg1 == 0);
	}

	loop_ub = Vq.size( 0 );
	for(i = 0; i < loop_ub; i++)
	{
		Vq[ i ] = Vq[ i ] * r[ i ];
	}

	coder::findpeaks( Vq, bw_temp, bw );
	nrows = bw_temp.size( 0 ) - 1;
	n = 0;
	for(nx = 0; nx <= nrows; nx++)
	{
		if(bw_temp[ nx ] > 10.0)
		{
			n++;
		}
	}

	candidates.set_size( n );
	n = 0;
	for(nx = 0; nx <= nrows; nx++)
	{
		if(bw_temp[ nx ] > 10.0)
		{
			candidates[ n ] = log_spaced_frequency[ static_cast<int>( bw[ nx ] ) - 1 ];
			n++;
		}
	}

	if(candidates.size( 0 ) < 1)
	{
		f0 = -1.0;
	}
	else
	{
		f0 = candidates[ 0 ];
	}

	return f0;
}

double DDSPSynth::compute_loudness( double n_samples, const double audio[ 4096 ], double sample_rate )
{
	static creal_T y[ 4096 ];
	coder::WeightingFilter A_weighting;
	coder::array<double, 1U> b_window;
	coder::array<double, 1U> window;
	creal_T X[ 2048 ];
	double x_win_data[ 4096 ];
	double amplitude[ 2048 ];
	double b_loudness[ 2048 ];
	double b_y[ 2048 ];
	double L;
	double loudness;
	int x_win_size[ 1 ];
	int i;
	int loop_ub;
	int trivialwin;
	int w_size_idx_0;
	signed char w_data[ 1 ];

	if(!isInitialized)
	{
		initialize();
	}

	A_weighting.pFilter.matlabCodegenIsDeleted = true;
	A_weighting.matlabCodegenIsDeleted = true;

	//  MATLAB Reimplementation of ddsp.spectral_ops.compute_loudness
	if(1.0 > n_samples)
	{
		loop_ub = 0;
	}
	else
	{
		loop_ub = static_cast<int>( n_samples );
	}

	w_size_idx_0 = 1;
	w_data[ 0 ] = 0;
	trivialwin = 0;
	if(n_samples == std::floor( n_samples ))
	{
		L = n_samples;
	}
	else
	{
		L = rt_roundd_snf( n_samples );
	}

	if(L == 0.0)
	{
		w_size_idx_0 = 0;
		trivialwin = 1;
	}
	else
	{
		if(L == 1.0)
		{
			w_data[ 0 ] = 1;
			trivialwin = 1;
		}
	}

	window.set_size( w_size_idx_0 );
	for(i = 0; i < w_size_idx_0; i++)
	{
		window[ 0 ] = w_data[ 0 ];
	}

	if(trivialwin == 0)
	{
		if(coder::iseven( L ))
		{
			int b_loop_ub;
			int c_loop_ub;
			coder::calc_window( L / 2.0, L, window );
			if(1 > window.size( 0 ))
			{
				i = 0;
				w_size_idx_0 = 1;
				trivialwin = -1;
			}
			else
			{
				i = window.size( 0 ) - 1;
				w_size_idx_0 = -1;
				trivialwin = 0;
			}

			b_loop_ub = div_s32_floor( trivialwin - i, w_size_idx_0 );
			b_window.set_size( ( ( window.size( 0 ) + b_loop_ub ) + 1 ) );
			c_loop_ub = window.size( 0 );
			for(trivialwin = 0; trivialwin < c_loop_ub; trivialwin++)
			{
				b_window[ trivialwin ] = window[ trivialwin ];
			}

			for(trivialwin = 0; trivialwin <= b_loop_ub; trivialwin++)
			{
				b_window[ trivialwin + window.size( 0 ) ] = window[ i + w_size_idx_0 *
					trivialwin ];
			}

			window.set_size( b_window.size( 0 ) );
			b_loop_ub = b_window.size( 0 );
			for(i = 0; i < b_loop_ub; i++)
			{
				window[ i ] = b_window[ i ];
			}
		}
		else
		{
			int b_loop_ub;
			int c_loop_ub;
			coder::calc_window( ( L + 1.0 ) / 2.0, L, window );
			if(1 > window.size( 0 ) - 1)
			{
				i = 0;
				w_size_idx_0 = 1;
				trivialwin = -1;
			}
			else
			{
				i = window.size( 0 ) - 2;
				w_size_idx_0 = -1;
				trivialwin = 0;
			}

			b_loop_ub = div_s32_floor( trivialwin - i, w_size_idx_0 );
			b_window.set_size( ( ( window.size( 0 ) + b_loop_ub ) + 1 ) );
			c_loop_ub = window.size( 0 );
			for(trivialwin = 0; trivialwin < c_loop_ub; trivialwin++)
			{
				b_window[ trivialwin ] = window[ trivialwin ];
			}

			for(trivialwin = 0; trivialwin <= b_loop_ub; trivialwin++)
			{
				b_window[ trivialwin + window.size( 0 ) ] = window[ i + w_size_idx_0 *
					trivialwin ];
			}

			window.set_size( b_window.size( 0 ) );
			b_loop_ub = b_window.size( 0 );
			for(i = 0; i < b_loop_ub; i++)
			{
				window[ i ] = b_window[ i ];
			}
		}
	}

	x_win_size[ 0 ] = loop_ub;
	for(i = 0; i < loop_ub; i++)
	{
		x_win_data[ i ] = audio[ i ] * window[ i ];
	}

	if(loop_ub == 0)
	{
		std::memset( &X[ 0 ], 0, 2048U * sizeof( creal_T ) );
	}
	else
	{
		if(2048 > loop_ub)
		{
			std::memset( &X[ 0 ], 0, 2048U * sizeof( creal_T ) );
		}

		coder::internal::FFTImplementationCallback::doHalfLengthRadix2( ( x_win_data ),
			( x_win_size ), ( X ) );
	}

	//      s = stft(x, sample_rate, 'Window', window, 'FFTLength', NFFT);
	for(w_size_idx_0 = 0; w_size_idx_0 < 2048; w_size_idx_0++)
	{
		L = rt_hypotd_snf( X[ w_size_idx_0 ].re, X[ w_size_idx_0 ].im );
		if(( 1.0E-20 > L ) || rtIsNaN( L ))
		{
			L = 1.0E-20;
		}

		L = std::log10( L );
		b_loudness[ w_size_idx_0 ] = L;
	}

	coder::internal::FFTImplementationCallback::doHalfLengthRadix2( ( audio ), ( y ) );
	A_weighting.init( sample_rate );
	for(i = 0; i < 2048; i++)
	{
		b_y[ i ] = y[ i ].re;
	}

	A_weighting.step( b_y, amplitude );
	for(w_size_idx_0 = 0; w_size_idx_0 < 2048; w_size_idx_0++)
	{
		L = ( b_loudness[ w_size_idx_0 ] * 20.0 + amplitude[ w_size_idx_0 ] ) - 20.7;
		b_loudness[ w_size_idx_0 ] = L;
		if(L > -120.0)
		{
			amplitude[ w_size_idx_0 ] = L;
		}
		else
		{
			amplitude[ w_size_idx_0 ] = -120.0;
		}
	}

	L = amplitude[ 0 ];
	for(w_size_idx_0 = 0; w_size_idx_0 < 2047; w_size_idx_0++)
	{
		L += amplitude[ w_size_idx_0 + 1 ];
	}

	loudness = L / 2048.0;
	A_weighting.matlabCodegenDestructor();
	A_weighting.pFilter.matlabCodegenDestructor();

	return loudness;
}

void DDSPSynth::eml_rand_mt19937ar_stateful_init()
{
	unsigned r{ 5489U };

	std::memset( &state[ 0 ], 0, 625U * sizeof( unsigned int ) );
	state[ 0 ] = 5489U;

	for(int mti = 0; mti < 623; mti++)
	{
		r = ( ( r ^ r >> 30U ) * 1812433253U + mti ) + 1U;
		state[ mti + 1 ] = r;
	}

	state[ 624 ] = 624U;
}