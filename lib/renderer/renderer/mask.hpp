#ifndef __VTX_RENDERER_GL_DEFINE__
#define __VTX_RENDERER_GL_DEFINE__

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VTX
{
	// Mask.
	using VTX_RENDER_MASK = unsigned char;

	constexpr VTX_RENDER_MASK VTX_MASK_NEED_UPDATE		 = 0xFF;
	constexpr VTX_RENDER_MASK VTX_MASK_NO_UPDATE		 = 0x00;
	constexpr VTX_RENDER_MASK VTX_MASK_SELECTION_UPDATED = 0x01;
	constexpr VTX_RENDER_MASK VTX_MASK_CAMERA_UPDATED	 = 0x02;
	constexpr VTX_RENDER_MASK VTX_MASK_3D_MODEL_UPDATED	 = 0x04;
	constexpr VTX_RENDER_MASK VTX_MASK_UNIFORM_UPDATED	 = 0x08;
	constexpr VTX_RENDER_MASK VTX_MASK_FORCE_AA			 = 0x10;
} // namespace VTX

#endif
