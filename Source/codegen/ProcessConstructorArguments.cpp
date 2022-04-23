//
//  Academic License - for use in teaching, academic research, and meeting
//  course requirements at degree granting institutions only.  Not for
//  government, commercial, or other organizational use.
//
//  ProcessConstructorArguments.cpp
//
//  Code generation for function 'ProcessConstructorArguments'
#include "ProcessConstructorArguments.h"


// Function Definitions
namespace coder
{
	namespace matlab
	{
		namespace system
		{
			namespace coder
			{
				void ProcessConstructorArguments::b_do( weightingFilter* obj, double
					varargin_2 )
				{
					SystemProp::matlabCodegenNotifyAnyProp( ( obj ) );
					obj->set_SampleRate( varargin_2 );
				}
			}
		}
	}
}

// End of code generation (ProcessConstructorArguments.cpp)
