#ifndef __VTX_BASE_PASS__
#define __VTX_BASE_PASS__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			class BasePass
			{
			  public:
				BasePass()			= default;
				virtual ~BasePass() = default;

			  private:
			};
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX

#endif
