/*
  ==============================================================================

    SubtractiveSynth.cpp
    Created: 24 Apr 2022 8:38:49pm
    Author:  St

  ==============================================================================
*/
#include "SubtractiveSynth.h"


namespace DDSP
{
	SubtractiveSynth::SubtractiveSynth()
	{
		eml_rand_mt19937ar_stateful_init();
	}


	void SubtractiveSynth::eml_rand_mt19937ar_stateful_init()
	{
		unsigned r{ 5489U };

		state.fill( {} );
		state[ 0 ] = 5489U;

		for(int mti = 0; mti < 623; mti++)
		{
			r = ( ( r ^ r >> 30U ) * 1812433253U + mti ) + 1U;
			state[ mti + 1 ] = r;
		}

		state[ 624 ] = 624U;
	}


	void SubtractiveSynth::process( double n_samples, const Magnitudes& magnitudes, double initial_bias, Amplitudes& out )
	{
		coder::array<creal_T, 1U> H;
		coder::array<creal_T, 1U> X;
		coder::array<double, 1U> sub;

		double r1;
		double r3;
		double sigma;
		int signal_size[ 1 ];
		int b_eint;
		int eint;
		int loop_ub;
		short b_tmp_data[ maxAmplitudes ];
		boolean_T tmp_data[ maxAmplitudes ];

		// function [out, b] = subtractive(n_samples, magnitudes, color, ir_coeffs, recalculate_ir) 
		// magnitudes: row = frames, column = freq responses
		// magnitudes should be 65
		for(int k = 0; k < maxMagnitudes; ++k)
			z1[ k ] = rt_powd_snf( sigmoid( magnitudes[ k ] + initial_bias ), 2.3025850929940459 );

		// generate white noise
		white_n.init();
		brown_n.init();
		violet_n.init();
		white_n.step( white_noise, state );
		brown_n.step( brown_noise, state );
		violet_n.step( violet_noise, state );
		white_n.release();
		brown_n.release();
		violet_n.release();
		sigma = std::abs( colour );

		for(int idx = 0; idx < white_noise.size(); ++idx)
			white_noise[ idx ] *= 1.0 - sigma;


		if(colour > 0.0)
		{
			for(int idx = 0; idx < white_noise.size(); ++idx)
				white_noise[ idx ] += colour * violet_noise[ idx ];
		}
		else if(colour < 0.0)
		{
			for(int idx = 0; idx < white_noise.size(); ++idx)
				white_noise[ idx ] += sigma * brown_noise[ idx ];
		}

		if(1.0 > n_samples)
			loop_ub = 0;
		else
			loop_ub = static_cast<int>( n_samples );

		signal_size[ 0 ] = loop_ub;

		if(0 <= loop_ub - 1)
			std::memcpy( &brown_noise[ 0 ], &white_noise[ 0 ], loop_ub * sizeof( double ) );

		if(loop_ub <= 2)
		{
			if(loop_ub == 1)
			{
				r1 = white_noise[ 0 ];
				r3 = white_noise[ 0 ];
			}
			else
			{
				if(( white_noise[ 0 ] > white_noise[ 1 ] ) || ( rtIsNaN( white_noise[ 0 ] ) && ( !rtIsNaN( white_noise[ 1 ] ) ) ))
					r1 = white_noise[ 1 ];
				else
					r1 = white_noise[ 0 ];

				if(( white_noise[ 0 ] < white_noise[ 1 ] ) || ( rtIsNaN( white_noise[ 0 ] ) && ( !rtIsNaN( white_noise[ 1 ] ) ) ))
					r3 = white_noise[ 1 ];
				else
					r3 = white_noise[ 0 ];
			}
		}
		else
		{
			boolean_T b = rtIsNaN( white_noise[ 0 ] );
			boolean_T exitg1;
			int index{};

			if(!b)
			{
				index = 1;
			}
			else
			{
				index = 0;
				int k = 2;
				exitg1 = false;
				while(( !exitg1 ) && ( k <= loop_ub ))
				{
					if(!rtIsNaN( white_noise[ k - 1 ] ))
					{
						index = k;
						exitg1 = true;
					}
					else
						++k;
				}
			}

			if(index == 0)
			{
				r1 = white_noise[ 0 ];
			}
			else
			{
				r1 = white_noise[ index - 1 ];
				++index;

				for(int k = index; k <= loop_ub; ++k)
				{
					sigma = white_noise[ k - 1 ];
					if(r1 > sigma)
						r1 = sigma;
				}
			}

			if(!b)
			{
				index = 1;
			}
			else
			{
				index = 0;
				int k = 2;
				exitg1 = false;

				while(( !exitg1 ) && ( k <= loop_ub ))
				{
					if(!rtIsNaN( white_noise[ k - 1 ] ))
					{
						index = k;
						exitg1 = true;
					}
					else
						++k;
				}
			}

			if(index == 0)
				r3 = white_noise[ 0 ];
			else
			{
				r3 = white_noise[ index - 1 ];
				++index;

				for(int k = index; k <= loop_ub; ++k)
				{
					sigma = white_noise[ k - 1 ];
					if(r3 < sigma)
						r3 = sigma;
				}
			}
		}

		if(loop_ub != 0)
		{
			double iMax;
			double iMin;

			if(( 0.0 < r3 ) || rtIsNaN( r3 ))
				sigma = 0.0;
			else
				sigma = r3;

			if(( !( sigma > r1 ) ) && ( !rtIsNaN( r1 ) ))
				sigma = r1;

			signal_size[ 0 ] = loop_ub;

			for(int idx = 0; idx < loop_ub; ++idx)
				brown_noise[ idx ] = white_noise[ idx ] - sigma;

			iMin = r1 - sigma;
			iMax = r3 - sigma;
			sigma = std::abs( iMax );
			r1 = std::abs( iMin );

			if(( !( sigma > r1 ) ) && ( !rtIsNaN( r1 ) ))
				sigma = r1;

			if(( !rtIsInf( sigma ) ) && ( !rtIsNaN( sigma ) ))
				frexp( sigma, &b_eint );
			else
				b_eint = 0;

			r1 = rt_powd_snf( 2.0, static_cast<double>( b_eint ) - 1.0 );
			sigma = ( static_cast<double>( b_eint ) + 1.0 ) / 2.0;

			if(sigma < 0.0)
				sigma = std::ceil( sigma );
			else
				sigma = std::floor( sigma );

			r3 = rt_powd_snf( 2.0, sigma - 1.0 );
			if(iMin == iMax)
			{
				for(int idx = 0; idx < loop_ub; ++idx)
					tmp_data[ idx ] = rtIsNaN( brown_noise[ idx ] );

				for(int idx = 0; idx < loop_ub; ++idx)
					tmp_data[ idx ] = !tmp_data[ idx ];

				int k = loop_ub - 1;
				loop_ub = 0;
				int index{};
				for(b_eint = 0; b_eint <= k; ++b_eint)
				{
					if(tmp_data[ b_eint ])
					{
						++loop_ub;
						b_tmp_data[ index ] = static_cast<short>( b_eint + 1 );
						++index;
					}
				}

				for(int idx = 0; idx < loop_ub; ++idx)
					brown_noise[ b_tmp_data[ idx ] - 1 ] = -1.0;
			}
			else
			{
				double c1 = iMax / r1;
				sigma = iMin / r1;
				r1 = 2.0 / ( iMax / r3 - iMin / r3 ) / r3;
				sigma = r3 * ( ( c1 * ( -1.0 / r3 ) - sigma * ( 1.0 / r3 ) ) / ( c1 - sigma ) );
				sub.set_size( loop_ub );

				for(int idx = 0; idx < loop_ub; ++idx)
					sub[ idx ] = brown_noise[ idx ];

				for(int idx = 0; idx < loop_ub; ++idx)
					brown_noise[ idx ] = r1 * brown_noise[ idx ] + sigma;

				int k = sub.size( 0 );
				for(b_eint = 0; b_eint < k; ++b_eint)
				{
					if(brown_noise[ b_eint ] < -1.0)
						brown_noise[ b_eint ] = -1.0;
				}

				for(b_eint = 0; b_eint < loop_ub; ++b_eint)
				{
					if(brown_noise[ b_eint ] > 1.0)
						brown_noise[ b_eint ] = 1.0;
				}
			}
		}

		sigma = std::abs( n_samples );
		if(!rtIsInf( sigma ))
		{
			r1 = frexp( sigma, &eint );
			sigma = eint;

			if(r1 == 0.5)
				sigma = static_cast<double>( eint ) - 1.0;
		}

		sigma = rt_powd_snf( 2.0, sigma );
		for(b_eint = 0; b_eint < maxMagnitudes; ++b_eint)
		{
			dcv[ b_eint ].re = 2.0 * z1[ b_eint ] + 1.0E-7;
			dcv[ b_eint ].im = 0.0;
			x[ b_eint ] = dv[ b_eint ];
		}

		coder::ifft( dcv, dcv1 );
		fftshift( x );

		for(int idx = 0; idx < maxMagnitudes; ++idx)
			x[ idx ] *= dcv1[ idx ].re;

		fftshift( x );
		coder::fft( x.data(), sigma, H );
		coder::fft( brown_noise.data(), signal_size, sigma, X );
		loop_ub = X.size( 0 );

		for(int idx = 0; idx < loop_ub; ++idx)
		{
			sigma = X[ idx ].re * H[ idx ].im + X[ idx ].im * H[ idx ].re;
			X[ idx ].re = X[ idx ].re * H[ idx ].re - X[ idx ].im * H[ idx ].im;
			X[ idx ].im = sigma;
		}

		coder::ifft( X, n_samples, H );
		sub.set_size( H.size( 0 ) );
		loop_ub = H.size( 0 );

		for(int idx = 0; idx < loop_ub; ++idx)
			sub[ idx ] = H[ idx ].re;

		std::memset( &out[ 0 ], 0, 4096U * sizeof( double ) );

		if(1.0 > n_samples)
			loop_ub = 0;
		else
			loop_ub = static_cast<int>( n_samples );

		for(int idx = 0; idx < loop_ub; ++idx)
			out[ idx ] = sub[ idx ];
	}

	void SubtractiveSynth::fftshift( Magnitudes& x )
	{
		int i2{};

		for(int i = 0; i < 1; ++i)
		{
			int i1 = i2;

			i2 += 64;

			for(int j{}; j < 1; ++j)
			{
				++i1;
				++i2;

				int ia = i1 - 1;
				int ib = i1 + 31;

				auto xtmp = x[ i1 + 31 ];

				for(int k{}; k < 32; ++k)
				{
					x[ ib ] = x[ ia ];
					x[ ia ] = x[ ib + 1 ];
					++ia;
					++ib;
				}

				x[ ib ] = xtmp;
			}
		}
	}
}
