//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  FFTImplementationCallback.h
//
//  Code generation for function 'FFTImplementationCallback'
//
#pragma once

#include <cstddef>
#include <cstdlib>

#include "rt_nonfinite.h"
#include "coder_array.h"

// Type Definitions
namespace coder::internal
{
	class FFTImplementationCallback
	{
	public:
		static void get_algo_sizes( int nfft, boolean_T useRadix2, int* n2blue, int* nRows );

		static void generate_twiddle_tables( int nRows, boolean_T useRadix2, array<double, 2U>& costab, array<double, 2U>& sintab, array<double, 2U>& sintabinv );

		static void r2br_r2dit_trig_impl( const array<creal_T, 1U>& x, int unsigned_nRows, const array<double, 2U>& costab, const array<double, 2U>& sintab, array<creal_T, 1U>& y );

		static void doHalfLengthRadix2( const double x_data[], const int x_size[ 1 ], creal_T y[ 2048 ] );
		static void doHalfLengthRadix2( const double x[ 4096 ], creal_T y[ 4096 ] );
		static void doHalfLengthRadix2( const double x_data[], const int x_size[ 1 ], array<creal_T, 1U>& y, int unsigned_nRows, const array< double, 2U>& costab, const array<double, 2U>& sintab );

		static void doHalfLengthBluestein( const double x_data[], const int x_size[ 1 ], array<creal_T, 1U>& y, int nrowsx, int nRows, int nfft, const array<creal_T, 1U>& wwc, const array<double, 2U>& costab, const array<double, 2U>& sintab, const array<double, 2U>& costabinv, const array<double, 2U>& sintabinv );

		static void doHalfLengthRadix2( const double x[ 65 ], array<creal_T, 1U>& y, int unsigned_nRows, const array<double, 2U>& costab, const array<double, 2U>& sintab );

		static void doHalfLengthBluestein( const double x[ 65 ], array< creal_T, 1U>& y, int nRows, int nfft, const array<creal_T, 1U>& wwc, const array<double, 2U>& costab, const array<double, 2U>& sintab, const array<double, 2U>& costabinv, const array<double, 2U>& sintabinv );

	protected:
		static void get_bitrevIndex( int nRowsM1, int nfftLen, array<int, 1U>& bitrevIndex );
		static void b_generate_twiddle_tables( int nRows, array<double, 2U>& costab, array<double, 2U>& sintab, array<double, 2U>& sintabinv );
		static void get_half_twiddle_tables( const array<double, 2U>& costab, const array<double, 2U>& sintab, const array<double, 2U>& costabinv, const array<double, 2U>& sintabinv, array<double, 2U>& hcostab, array<double, 2U>& hsintab, array<double, 2U>& hcostabinv, array<double, 2U>& hsintabinv );
		static void getback_radix2_fft( array<creal_T, 1U>& y, const array<creal_T, 1U>& reconVar1, const array<creal_T, 1U>& reconVar2, const array<int, 2U>& wrapIndex, int hnRows );
	};
}