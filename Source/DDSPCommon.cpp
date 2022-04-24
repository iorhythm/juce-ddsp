/*
  ==============================================================================

	DDSPCommon.cpp
	Created: 24 Apr 2022 8:46:42pm
	Author:  St

  ==============================================================================
*/
#include "DDSPCommon.h"


namespace DDSP
{
	double sigmoid( const double x )
	{
		return 1.0 / ( 1.0 + std::exp( -x ) );
	}

	double rt_hypotd_snf( const double u0, const double u1 )
	{
		{
			double a = std::abs( u0 );
			double y = std::abs( u1 );

			if(a < y)
			{
				a /= y;
				y *= std::sqrt( a * a + 1.0 );
			}
			else if(a > y)
			{
				y /= a;
				y = a * std::sqrt( y * y + 1.0 );
			}
			else
			{
				if(!rtIsNaN( y ))
				{
					y = a * 1.4142135623730951;
				}
			}

			return y;
		}
	}


	double rt_powd_snf( const double u0, const double u1 )
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
						y = rtInf;
					else
						y = 0.0;
				}
				else if(u1 > 0.0)
					y = 0.0;
				else
					y = rtInf;
			}
			else if(d1 == 0.0)
				y = 1.0;
			else if(d1 == 1.0)
			{
				if(u1 > 0.0)
					y = u0;
				else
					y = 1.0 / u0;
			}
			else if(u1 == 2.0)
				y = u0 * u0;
			else if(( u1 == 0.5 ) && ( u0 >= 0.0 ))
				y = std::sqrt( u0 );
			else if(( u0 < 0.0 ) && ( u1 > std::floor( u1 ) ))
				y = rtNaN;
			else
				y = std::pow( u0, u1 );
		}

		return y;
	}

	double rt_roundd_snf( const double u )
	{
		if(std::abs( u ) >= 4.503599627370496E+15)
			return u;

		if(u >= 0.5)
			return std::floor( u + 0.5 );
		else if(u > -0.5)
			return u * 0.0;
		else
			return std::ceil( u - 0.5 );
	}

	int div_s32_floor( const int numerator, const int denominator )
	{
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
			unsigned absNumerator = [&]()
			{
				if(numerator < 0)
					return ~static_cast<unsigned>( numerator ) + 1U;

				return static_cast<unsigned>( numerator );
			}( );

			unsigned absDenominator = [&]()
			{
				if(denominator < 0)
					return ~static_cast<unsigned int>( denominator ) + 1U;

				return static_cast<unsigned int>( denominator );
			}( );

			const boolean_T quotientNeedsNegation = ( ( numerator < 0 ) != ( denominator < 0 ) );
			unsigned tempAbsQuotient = absNumerator / absDenominator;

			if(quotientNeedsNegation)
			{
				absNumerator %= absDenominator;
				if(absNumerator > 0U)
				{
					++tempAbsQuotient;
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
}