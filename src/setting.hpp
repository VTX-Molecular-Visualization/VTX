#ifndef __VTX_SETTING__
#define __VTX_SETTING__

#include "renderer/base_renderer.hpp"

namespace VTX
{
	namespace Setting
	{
		namespace Rendering
		{
			static Renderer::MODE mode	  = Renderer::MODE::DEFERRED;
			static bool			  useAA	  = true;
			static bool			  useSSAO = true;
			static bool			  useBlur = true;
		}; // namespace Rendering
	}	   // namespace Setting
} // namespace VTX
#endif
