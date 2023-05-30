#ifndef __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_CYLINDER__
#define __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_CYLINDER__

#include <core/chemdb/color.hpp>

namespace VTX::App::Application::Representation::Primitive
{
	struct Cylinder
	{
	  public:
		Cylinder();

		float										  radius;
		VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE colorBlendingMode;
	};

} // namespace VTX::App::Application::Representation::Primitive
#endif
