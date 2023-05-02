#ifndef __VTX_APP_RENDER_DEFINE__
#define __VTX_APP_RENDER_DEFINE__

#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Render
{
	// Mask.
	using VTX_MASK = unsigned char;

	constexpr VTX_MASK VTX_MASK_NEED_UPDATE		  = 0xFF;
	constexpr VTX_MASK VTX_MASK_NO_UPDATE		  = 0x00;
	constexpr VTX_MASK VTX_MASK_SELECTION_UPDATED = 0x01;
	constexpr VTX_MASK VTX_MASK_CAMERA_UPDATED	  = 0x02;
	constexpr VTX_MASK VTX_MASK_3D_MODEL_UPDATED  = 0x04;
	constexpr VTX_MASK VTX_MASK_UNIFORM_UPDATED	  = 0x08;
	constexpr VTX_MASK VTX_MASK_FORCE_AA		  = 0x10;

} // namespace VTX::App::Render

#endif
