//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  SystemProp.cpp
//
//  Code generation for function 'SystemProp'
#include "SystemProp.h"


// Function Definitions
namespace coder
{
	namespace matlab
	{
		namespace system
		{
			namespace coder
			{
				void SystemProp::matlabCodegenNotifyAnyProp( WeightingFilter* obj )
				{
					if(obj->isLockedAndNotReleased())
					{
						obj->TunablePropsChanged = true;
					}
				}
			}
		}
	}
}

// End of code generation (SystemProp.cpp)
