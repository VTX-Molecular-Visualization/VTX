#ifndef __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_RIBBON__
#define __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_RIBBON__

#include "app/old_app/generic/base_colorable.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	struct Ribbon
	{
	  public:
		Ribbon();

		Generic::SECONDARY_STRUCTURE_COLOR_MODE colorMode;
		Generic::COLOR_BLENDING_MODE			colorBlendingMode;
	};

} // namespace VTX::App::Application::Representation::Primitive
#endif
