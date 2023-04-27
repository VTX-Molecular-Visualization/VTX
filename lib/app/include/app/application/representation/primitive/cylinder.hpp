#ifndef __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_CYLINDER__
#define __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_CYLINDER__

#include "app/old_app/generic/base_colorable.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	struct Cylinder
	{
	  public:
		Cylinder();

		float						 radius;
		Generic::COLOR_BLENDING_MODE colorBlendingMode;
	};

} // namespace VTX::App::Application::Representation::Primitive
#endif
