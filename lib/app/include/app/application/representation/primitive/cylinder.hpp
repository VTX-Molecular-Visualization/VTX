#ifndef __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_CYLINDER__
#define __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_CYLINDER__

#include "app/internal/chemdb/color.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	struct Cylinder
	{
	  public:
		Cylinder();

		float											  radius;
		App::Internal::ChemDB::Color::COLOR_BLENDING_MODE colorBlendingMode;
	};

} // namespace VTX::App::Application::Representation::Primitive
#endif
