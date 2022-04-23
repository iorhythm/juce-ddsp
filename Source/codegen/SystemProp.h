//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  SystemProp.h
//
//  Code generation for function 'SystemProp'
#pragma once

#include "weightingFilter.h"


namespace coder
{
	class WeightingFilter;
}

// Type Definitions
namespace coder
{
	namespace matlab
	{
		namespace system
		{
			namespace coder
			{
				class SystemProp
				{
				public:
					static void matlabCodegenNotifyAnyProp( WeightingFilter* obj );
				};
			}
		}
	}
}
