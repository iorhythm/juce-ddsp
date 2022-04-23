//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  WeightingFilter.cpp
//
//  Code generation for function 'WeightingFilter'
//
#include "weightingFilter.h"


namespace coder
{
	void WeightingFilter::checkTunableProps()
	{
		if(this->TunablePropsChanged)
		{
			this->TunablePropsChanged = false;
			this->designFilter();
		}
	}

	void WeightingFilter::designFilter()
	{
		static const double p[ 6 ] = { -129.42731529303637, -129.42731529303637,
			-676.40154875894643, -4636.1251222587643, -76618.525086959533,
			-76618.525086959533 };

		double sos[ 18 ];
		double new_z[ 6 ];
		double pd_tmp[ 6 ];
		double remaining_z[ 6 ];
		double varargin_1[ 6 ];
		double x[ 6 ];
		double den_tmp;
		double fs;
		double twoFs;
		double zb_idx_1;
		int iidx[ 6 ];
		int b_i;
		int i;
		int idx;
		int k;
		fs = this->get_SampleRate();
		twoFs = 2.0 * fs;
		den_tmp = 0.0 / twoFs;
		fs = ( den_tmp + 1.0 ) / ( 1.0 - den_tmp );
		remaining_z[ 0 ] = fs;
		remaining_z[ 1 ] = fs;
		remaining_z[ 2 ] = fs;
		remaining_z[ 3 ] = fs;
		remaining_z[ 4 ] = -1.0;
		remaining_z[ 5 ] = -1.0;
		for(i = 0; i < 6; i++)
		{
			den_tmp = p[ i ];
			fs = den_tmp / twoFs;
			fs = ( fs + 1.0 ) / ( 1.0 - fs );
			pd_tmp[ i ] = fs;
			x[ i ] = twoFs - den_tmp;
		}

		zb_idx_1 = x[ 0 ];
		for(k = 0; k < 5; k++)
		{
			zb_idx_1 *= x[ k + 1 ];
		}

		for(k = 0; k < 6; k++)
		{
			den_tmp = pd_tmp[ k ];
			fs = den_tmp;
			if(den_tmp < 0.0)
			{
				fs = -1.0;
			}
			else if(den_tmp > 0.0)
			{
				fs = 1.0;
			}
			else
			{
				if(den_tmp == 0.0)
				{
					fs = 0.0;
				}
			}

			x[ k ] = std::abs( den_tmp - fs );
		}

		internal::sort( x, iidx );
		for(i = 0; i < 6; i++)
		{
			x[ i ] = iidx[ i ];
			for(k = 0; k < 6; k++)
			{
				varargin_1[ k ] = std::abs( remaining_z[ k ] - pd_tmp[ static_cast<int>( x[ i ] )
					- 1 ] );
			}

			if(!rtIsNaN( varargin_1[ 0 ] ))
			{
				idx = 1;
			}
			else
			{
				boolean_T exitg1;
				idx = 0;
				k = 2;
				exitg1 = false;
				while(( !exitg1 ) && ( k < 7 ))
				{
					if(!rtIsNaN( varargin_1[ k - 1 ] ))
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
				idx = 1;
			}
			else
			{
				fs = varargin_1[ idx - 1 ];
				b_i = idx + 1;
				for(k = b_i; k < 7; k++)
				{
					den_tmp = varargin_1[ k - 1 ];
					if(fs > den_tmp)
					{
						fs = den_tmp;
						idx = k;
					}
				}
			}

			new_z[ i ] = remaining_z[ idx - 1 ];
			remaining_z[ idx - 1 ] = rtInf;
		}

		std::memset( &sos[ 0 ], 0, 18U * sizeof( double ) );
		this->pScaleValues[ 0 ] = 7.3863929841439228E+9 * ( twoFs * twoFs * twoFs *
			twoFs ) / zb_idx_1;
		for(i = 0; i < 3; i++)
		{
			double zb_idx_2;
			double zb_tmp;
			idx = ( i << 1 ) + 1;
			fs = -pd_tmp[ iidx[ idx - 1 ] - 1 ];
			zb_idx_1 = -new_z[ idx - 1 ];
			den_tmp = pd_tmp[ iidx[ idx ] - 1 ];
			twoFs = -den_tmp * fs;
			zb_tmp = new_z[ idx ];
			zb_idx_2 = -zb_tmp * zb_idx_1;
			for(k = 2; k >= 2; k--)
			{
				fs -= den_tmp;
				zb_idx_1 -= zb_tmp;
			}

			b_i = 3 - static_cast<int>( std::ceil( static_cast<double>( idx ) / 2.0 ) );
			sos[ b_i ] = 1.0;
			sos[ b_i + 9 ] = 1.0;
			sos[ b_i + 3 ] = zb_idx_1;
			sos[ b_i + 12 ] = fs;
			sos[ b_i + 6 ] = zb_idx_2;
			sos[ b_i + 15 ] = twoFs;
			this->pScaleValues[ i + 1 ] = 1.0;
		}

		for(b_i = 0; b_i < 3; b_i++)
		{
			this->pNumMatrix[ 3 * b_i ] = sos[ b_i ];
			this->pNumMatrix[ 3 * b_i + 1 ] = sos[ b_i + 3 ];
			this->pNumMatrix[ 3 * b_i + 2 ] = sos[ b_i + 6 ];
			idx = b_i << 1;
			this->pDenMatrix[ idx ] = sos[ b_i + 12 ];
			this->pDenMatrix[ idx + 1 ] = sos[ b_i + 15 ];
		}
	}

	double WeightingFilter::get_SampleRate() const
	{
		return this->pSampleRateDialog;
	}

	void WeightingFilter::release()
	{
		if(this->isInitialized == 1)
		{
			this->isInitialized = 2;
			this->releaseWrapper();
		}
	}

	void WeightingFilter::releaseWrapper()
	{
		if(this->isSetupComplete)
		{
			this->pFilter.release();
			this->pNumChannels = -1.0;
		}
	}

	void WeightingFilter::setup()
	{
		this->isSetupComplete = false;
		this->isInitialized = 1;
		this->get_SampleRate();
		this->pNumChannels = 1.0;
		this->designFilter();
		this->pFilter.init();
		this->pFilter.setup();
		this->isSetupComplete = true;
		this->TunablePropsChanged = false;
	}

	void WeightingFilter::setupAndReset()
	{
		this->setup();
		this->pFilter.reset();
		this->designFilter();
	}

	WeightingFilter::WeightingFilter()
	{
		this->matlabCodegenIsDeleted = true;
	}

	WeightingFilter::~WeightingFilter()
	{
		this->matlabCodegenDestructor();
	}

	WeightingFilter* WeightingFilter::init( double varargin_2 )
	{
		WeightingFilter* obj;
		obj = this;
		obj->pNumChannels = -1.0;
		obj->isInitialized = 0;
		matlab::system::coder::ProcessConstructorArguments::b_do( ( obj ), ( varargin_2 ) );
		obj->matlabCodegenIsDeleted = false;
		return obj;
	}

	boolean_T WeightingFilter::isLockedAndNotReleased() const
	{
		return this->isInitialized == 1;
	}

	void WeightingFilter::matlabCodegenDestructor()
	{
		if(!this->matlabCodegenIsDeleted)
		{
			this->matlabCodegenIsDeleted = true;
			this->release();
		}
	}

	void WeightingFilter::set_SampleRate( double value )
	{
		this->pSampleRateDialog = value;
	}

	void WeightingFilter::step( const double varargin_1[ 2048 ], double varargout_1
		[ 2048 ] )
	{
		double obj[ 9 ];
		double c_obj[ 6 ];
		double b_obj[ 4 ];
		if(this->isInitialized != 1)
		{
			this->setupAndReset();
		}

		this->checkTunableProps();
		std::memcpy( &obj[ 0 ], &this->pNumMatrix[ 0 ], 9U * sizeof( double ) );
		for(int i = 0; i < 6; i++)
		{
			c_obj[ i ] = this->pDenMatrix[ i ];
		}

		b_obj[ 0 ] = this->pScaleValues[ 0 ];
		b_obj[ 1 ] = this->pScaleValues[ 1 ];
		b_obj[ 2 ] = this->pScaleValues[ 2 ];
		b_obj[ 3 ] = this->pScaleValues[ 3 ];
		this->pFilter.step( varargin_1, obj, c_obj, b_obj, varargout_1 );
	}
}

// End of code generation (WeightingFilter.cpp)
